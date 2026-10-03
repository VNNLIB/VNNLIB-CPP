#define VNNLIB_NO_DEPRECATED_QUERY_API

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

#include "CompatTransformer.h"
#include "DNFConverter.h"
#include "LinearArithExpr.h"
#include "VNNLib.h"

namespace query = vnnlib::query;

namespace {

constexpr const char* queryText = R"(
(vnnlib-version <2.0>)
(declare-network test
    (declare-input X real [1])
    (declare-output Y real [1])
)
(assert (<= (+ (* 2.0 X[0]) 3.0) 10.0))
)";

void testParsingAndAstTypes() {
    static_assert(std::is_same_v<
        decltype(query::parseQueryString(std::string{})),
        std::unique_ptr<query::TQuery>>);

    auto parsed = query::parseQueryString(queryText);
    assert(parsed);
    assert(parsed->version);
    assert(parsed->networks.size() == 1);
    assert(parsed->assertions.size() == 1);

    const auto* comparison =
        dynamic_cast<const query::TLessEqual*>(parsed->assertions[0]->cond.get());
    assert(comparison);
    assert(dynamic_cast<const query::TPlus*>(comparison->lhs.get()));
    assert(dynamic_cast<const query::TFloat*>(comparison->rhs.get()));
}

void testTransformations() {
    auto parsed = query::parseQueryString(queryText);
    const auto* comparison =
        dynamic_cast<const query::TCompare*>(parsed->assertions[0]->cond.get());
    assert(comparison);

    auto linear = query::linearize(comparison->lhs.get());
    assert(linear);
    assert(linear->getConstant() == 3.0);
    assert(linear->getNumTerms() == 1);
    assert(linear->getCoefficient("X[0]") == 2.0);

    const query::DNF dnf = query::toDNF(parsed->assertions[0]->cond.get());
    assert(dnf.size() == 1);
    assert(dnf[0].size() == 1);
    assert(dnf[0][0] == comparison);

    query::CompatTransformer transformer(parsed.get());
    const auto cases = transformer.transform();
    assert(!cases.empty());
}

void testEnumsHelpersAndExceptions() {
    assert(query::dtypeToString(query::TDataType::F32) == "F32");
    assert(query::sameType(query::TDataType::Real, query::TDataType::Real));
    assert(query::SymbolKind::Input != query::SymbolKind::Output);
    assert(query::checkQueryString(queryText).empty());

    bool caught = false;
    try {
        (void)query::parseQueryString("not a query");
    } catch (const query::VNNLibException&) {
        caught = true;
    }
    assert(caught);
}

void testQueryTheories() {
    auto parsed = query::parseQueryString(queryText);
    assert(query::hiddenNodeTheory(*parsed) == "NH");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "SNET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    // Hidden Nodes Theory
    const std::string hidden = R"(
(vnnlib-version <2.0>)
(declare-network test
    (declare-input X real [2])
    (declare-hidden H real [1] "hidden")
    (declare-output Y real [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(hidden);
    assert(query::hiddenNodeTheory(*parsed) == "H");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "SNET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    // Multiple Inputs/Outputs Theory
    const std::string multipleInputs = R"(
(vnnlib-version <2.0>)
(declare-network test
    (declare-input X real [1])
    (declare-input Z real [1])
    (declare-output Y real [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(multipleInputs);
    assert(query::hiddenNodeTheory(*parsed) == "NH");
    assert(query::inputOutputTheory(*parsed) == "MIO");
    assert(query::multipleNetworksTheory(*parsed) == "SNET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    // Multiple Networks Theory
    const std::string snet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [1])
    (declare-output Y float32 [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(snet);
    assert(query::hiddenNodeTheory(*parsed) == "NH");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "SNET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    const std::string mnet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [1])
    (declare-output Y float32 [1])
)
(declare-network g
    (declare-input U float32 [1])
    (declare-output W float32 [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(mnet);
    assert(query::hiddenNodeTheory(*parsed) == "NH");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "MNET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    const std::string minet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input A float32 [1,10])
    (declare-output B float32 [1,2])
)
(declare-network g
    (isomorphic-to f)
    (declare-input C float32 [1,10])
    (declare-output D float32 [1,2])
)
(assert (<= A[0, 0] 1.0))
)";
    parsed = query::parseQueryString(minet);
    assert(query::hiddenNodeTheory(*parsed) == "NH");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "MINET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    const std::string menet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input A float32 [1,10])
    (declare-output B float32 [1,2])
)
(declare-network f_copy
    (equal-to f)
    (declare-input C float32 [1,10])
    (declare-output D float32 [1,2])
)
(assert (<= A[0, 0] 1.0))
)";
    parsed = query::parseQueryString(menet);
    assert(query::hiddenNodeTheory(*parsed) == "NH");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "MENET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    const std::string snc = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [2])
    (declare-output Y float32 [1])
)
(declare-network g
    (declare-input A float32 [2])
    (declare-hidden H float32 [1] "hidden")
    (declare-output B float32 [1])
)
(assert (<= (+ X[0] X[1]) 0.1))
(assert (<= Y[0] 0.1))
(assert (<= H[0] 0.5))
(assert (== Y[0] A[0]))
)";
    parsed = query::parseQueryString(snc);
    assert(query::hiddenNodeTheory(*parsed) == "H");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "MNET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "SNC");

    const std::string mnc = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [2])
    (declare-output Y float32 [1])
)
(declare-network g
    (declare-input A float32 [2])
    (declare-hidden H float32 [1] "hidden")
    (declare-output B float32 [1])
)
(assert (<= (+ X[0] Y[0]) 0.1))
(assert (<= H[0] A[1]))
(assert (== B[0] H[0]))
)";
    parsed = query::parseQueryString(mnc);
    assert(query::hiddenNodeTheory(*parsed) == "H");
    assert(query::inputOutputTheory(*parsed) == "SIO");
    assert(query::multipleNetworksTheory(*parsed) == "MNET");
    assert(query::multipleNodeComparisonsTheory(*parsed) == "MNC");

    // Arithmetic Complexity Theory (examples from standard section 4.1.5)
    const std::string net = R"(
(vnnlib-version <2.0>)
(declare-network test
    (declare-input X real [2])
    (declare-output Y real [2])
)
)";
    parsed = query::parseQueryString(net + "(assert (<= X[0] 1.0))\n(assert (>= Y[0] 0.5))");
    assert(query::arithmeticComplexityTheory(*parsed) == "BND");
    parsed = query::parseQueryString(net + "(assert (<= X[0] 1.0))\n(assert (>= Y[0] Y[1]))");
    assert(query::arithmeticComplexityTheory(*parsed) == "OUTC");
    parsed = query::parseQueryString(net + "(assert (<= (+ (* 0.5 X[0]) (* 0.75 X[1])) 1.0))\n(assert (>= (+ Y[0] Y[1]) 0.5))");
    assert(query::arithmeticComplexityTheory(*parsed) == "LIN");
    parsed = query::parseQueryString(net + "(assert (<= (* X[0] X[1]) 1.0))\n(assert (>= (+ Y[0] Y[1]) 0.5))");
    assert(query::arithmeticComplexityTheory(*parsed) == "POLY");

    // Element Type Theories
    parsed = query::parseQueryString(net + "(assert (<= X[0] 1.0))");
    assert((query::elementTypeTheories(*parsed) == std::vector<std::string>{"Real"}));
    const std::string mixed = R"(
(vnnlib-version <2.0>)
(declare-network a
    (declare-input X float16 [2])
    (declare-output Y float32 [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(mixed);
    assert((query::elementTypeTheories(*parsed) == std::vector<std::string>{"F16", "F32"}));
}

} // namespace

int main() {
    testParsingAndAstTypes();
    testTransformations();
    testEnumsHelpersAndExceptions();
    testQueryTheories();
}
