#define VNNLIB_NO_DEPRECATED_QUERY_API

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

#include "CompatTransformer.h"
#include "DNFConverter.h"
#include "LinearArithExpr.h"
#include "VNNLib.h"

namespace {

constexpr const char* queryText = R"(
(vnnlib-version <2.0>)
(declare-network test
    (declare-input X real [1])
    (declare-output Y real [1])
)
(assert (<= (+ (* 2.0 X[0]) 3.0) 10.0))
)";

void testPublicTypesAndFunctions() {
    static_assert(sizeof(vnnlib::query::Shape) > 0);
    static_assert(sizeof(vnnlib::query::Indices) > 0);
    static_assert(sizeof(vnnlib::query::TDataType) > 0);
    static_assert(sizeof(vnnlib::query::SymbolKind) > 0);
    static_assert(sizeof(vnnlib::query::SymbolInfo) > 0);
    static_assert(sizeof(vnnlib::query::TNode) > 0);
    static_assert(sizeof(vnnlib::query::TElementType) > 0);
    static_assert(sizeof(vnnlib::query::TArithExpr) > 0);
    static_assert(sizeof(vnnlib::query::TVarExpr) > 0);
    static_assert(sizeof(vnnlib::query::TLiteral) > 0);
    static_assert(sizeof(vnnlib::query::TFloat) > 0);
    static_assert(sizeof(vnnlib::query::TInt) > 0);
    static_assert(sizeof(vnnlib::query::TNegate) > 0);
    static_assert(sizeof(vnnlib::query::TPlus) > 0);
    static_assert(sizeof(vnnlib::query::TMinus) > 0);
    static_assert(sizeof(vnnlib::query::TMultiply) > 0);
    static_assert(sizeof(vnnlib::query::TBoolExpr) > 0);
    static_assert(sizeof(vnnlib::query::TCompare) > 0);
    static_assert(sizeof(vnnlib::query::TGreaterThan) > 0);
    static_assert(sizeof(vnnlib::query::TLessThan) > 0);
    static_assert(sizeof(vnnlib::query::TGreaterEqual) > 0);
    static_assert(sizeof(vnnlib::query::TLessEqual) > 0);
    static_assert(sizeof(vnnlib::query::TEqual) > 0);
    static_assert(sizeof(vnnlib::query::TNotEqual) > 0);
    static_assert(sizeof(vnnlib::query::TConnective) > 0);
    static_assert(sizeof(vnnlib::query::TAnd) > 0);
    static_assert(sizeof(vnnlib::query::TOr) > 0);
    static_assert(sizeof(vnnlib::query::TAssertion) > 0);
    static_assert(sizeof(vnnlib::query::TInputDefinition) > 0);
    static_assert(sizeof(vnnlib::query::THiddenDefinition) > 0);
    static_assert(sizeof(vnnlib::query::TOutputDefinition) > 0);
    static_assert(sizeof(vnnlib::query::TNetworkDefinition) > 0);
    static_assert(sizeof(vnnlib::query::TVersion) > 0);
    static_assert(sizeof(vnnlib::query::TQuery) > 0);
    static_assert(sizeof(vnnlib::query::LinearArithExpr) > 0);
    static_assert(sizeof(vnnlib::query::Literal) > 0);
    static_assert(sizeof(vnnlib::query::Clause) > 0);
    static_assert(sizeof(vnnlib::query::DNF) > 0);
    static_assert(sizeof(vnnlib::query::Polytope) > 0);
    static_assert(sizeof(vnnlib::query::Box) > 0);
    static_assert(sizeof(vnnlib::query::PolyUnion) > 0);
    static_assert(sizeof(vnnlib::query::SpecCase) > 0);
    static_assert(sizeof(vnnlib::query::CompatTransformer) > 0);
    static_assert(sizeof(vnnlib::query::VNNLibException) > 0);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::parseQueryFile)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::parseQueryString)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::checkQueryFile)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::checkQueryString)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::dtypeToString)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::isConstant)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::sameFamily)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::sameType)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::linearize)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::toDNF)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::dnfOf)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::dnfOr)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::dnfAnd)>);
    static_assert(std::is_pointer_v<decltype(&vnnlib::query::distrib)>);
}

void testParsingAndAstTypes() {
    static_assert(std::is_same_v<
        decltype(vnnlib::query::parseQueryString(std::string{})),
        std::unique_ptr<vnnlib::query::TQuery>>);

    auto parsed = vnnlib::query::parseQueryString(queryText);
    assert(parsed);
    assert(parsed->version);
    assert(parsed->networks.size() == 1);
    assert(parsed->assertions.size() == 1);

    const auto* comparison =
        dynamic_cast<const vnnlib::query::TLessEqual*>(parsed->assertions[0]->cond.get());
    assert(comparison);
    assert(dynamic_cast<const vnnlib::query::TPlus*>(comparison->lhs.get()));
    assert(dynamic_cast<const vnnlib::query::TFloat*>(comparison->rhs.get()));
}

void testTransformations() {
    auto parsed = vnnlib::query::parseQueryString(queryText);
    const auto* comparison =
        dynamic_cast<const vnnlib::query::TCompare*>(parsed->assertions[0]->cond.get());
    assert(comparison);

    auto linear = vnnlib::query::linearize(comparison->lhs.get());
    assert(linear);
    assert(linear->getConstant() == 3.0);
    assert(linear->getNumTerms() == 1);
    assert(linear->getCoefficient("X[0]") == 2.0);

    const vnnlib::query::DNF dnf = vnnlib::query::toDNF(parsed->assertions[0]->cond.get());
    assert(dnf.size() == 1);
    assert(dnf[0].size() == 1);
    assert(dnf[0][0] == comparison);

    vnnlib::query::CompatTransformer transformer(parsed.get());
    const auto cases = transformer.transform();
    assert(!cases.empty());
}

void testEnumsHelpersAndExceptions() {
    assert(vnnlib::query::dtypeToString(vnnlib::query::TDataType::F32) == "F32");
    assert(vnnlib::query::sameType(vnnlib::query::TDataType::Real, vnnlib::query::TDataType::Real));
    assert(vnnlib::query::SymbolKind::Input != vnnlib::query::SymbolKind::Output);
    assert(vnnlib::query::checkQueryString(queryText).empty());

    bool caught = false;
    try {
        (void)vnnlib::query::parseQueryString("not a query");
    } catch (const vnnlib::query::VNNLibException&) {
        caught = true;
    }
    assert(caught);
}

} // namespace

int main() {
    testPublicTypesAndFunctions();
    testParsingAndAstTypes();
    testTransformations();
    testEnumsHelpersAndExceptions();
}
