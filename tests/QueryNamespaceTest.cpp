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

} // namespace

int main() {
    testParsingAndAstTypes();
    testTransformations();
    testEnumsHelpersAndExceptions();
}
