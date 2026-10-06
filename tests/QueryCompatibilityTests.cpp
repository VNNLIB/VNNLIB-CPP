#include <cassert>
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
    (declare-input X real [2])
    (declare-output Y real [1])
)
(assert (>= X[0] 0.0))
(assert (<= X[0] 1.0))
(assert (>= X[1] -1.0))
(assert (<= X[1] 2.0))
(assert (or (<= (+ (* 2.0 Y[0]) 3.0) 7.0) (>= Y[0] 3.0)))
)";

void testLegacyTypesAndFunctions() {
    static_assert(std::is_same_v<::Shape, vnnlib::query::Shape>);
    static_assert(std::is_same_v<::Indices, vnnlib::query::Indices>);
    static_assert(std::is_same_v<::TDataType, vnnlib::query::TDataType>);
    static_assert(std::is_same_v<::SymbolKind, vnnlib::query::SymbolKind>);
    static_assert(std::is_same_v<::SymbolInfo, vnnlib::query::SymbolInfo>);
    static_assert(std::is_same_v<::TNode, vnnlib::query::TNode>);
    static_assert(std::is_same_v<::TElementType, vnnlib::query::TElementType>);
    static_assert(std::is_same_v<::TArithExpr, vnnlib::query::TArithExpr>);
    static_assert(std::is_same_v<::TVarExpr, vnnlib::query::TVarExpr>);
    static_assert(std::is_same_v<::TLiteral, vnnlib::query::TLiteral>);
    static_assert(std::is_same_v<::TFloat, vnnlib::query::TFloat>);
    static_assert(std::is_same_v<::TInt, vnnlib::query::TInt>);
    static_assert(std::is_same_v<::TNegate, vnnlib::query::TNegate>);
    static_assert(std::is_same_v<::TPlus, vnnlib::query::TPlus>);
    static_assert(std::is_same_v<::TMinus, vnnlib::query::TMinus>);
    static_assert(std::is_same_v<::TMultiply, vnnlib::query::TMultiply>);
    static_assert(std::is_same_v<::TBoolExpr, vnnlib::query::TBoolExpr>);
    static_assert(std::is_same_v<::TCompare, vnnlib::query::TCompare>);
    static_assert(std::is_same_v<::TGreaterThan, vnnlib::query::TGreaterThan>);
    static_assert(std::is_same_v<::TLessThan, vnnlib::query::TLessThan>);
    static_assert(std::is_same_v<::TGreaterEqual, vnnlib::query::TGreaterEqual>);
    static_assert(std::is_same_v<::TLessEqual, vnnlib::query::TLessEqual>);
    static_assert(std::is_same_v<::TEqual, vnnlib::query::TEqual>);
    static_assert(std::is_same_v<::TNotEqual, vnnlib::query::TNotEqual>);
    static_assert(std::is_same_v<::TConnective, vnnlib::query::TConnective>);
    static_assert(std::is_same_v<::TAnd, vnnlib::query::TAnd>);
    static_assert(std::is_same_v<::TOr, vnnlib::query::TOr>);
    static_assert(std::is_same_v<::TAssertion, vnnlib::query::TAssertion>);
    static_assert(std::is_same_v<::TInputDefinition, vnnlib::query::TInputDefinition>);
    static_assert(std::is_same_v<::THiddenDefinition, vnnlib::query::THiddenDefinition>);
    static_assert(std::is_same_v<::TOutputDefinition, vnnlib::query::TOutputDefinition>);
    static_assert(std::is_same_v<::TNetworkDefinition, vnnlib::query::TNetworkDefinition>);
    static_assert(std::is_same_v<::TVersion, vnnlib::query::TVersion>);
    static_assert(std::is_same_v<::TQuery, vnnlib::query::TQuery>);
    static_assert(std::is_same_v<::LinearArithExpr, vnnlib::query::LinearArithExpr>);
    static_assert(std::is_same_v<::Literal, vnnlib::query::Literal>);
    static_assert(std::is_same_v<::Clause, vnnlib::query::Clause>);
    static_assert(std::is_same_v<::DNF, vnnlib::query::DNF>);
    static_assert(std::is_same_v<::Polytope, vnnlib::query::Polytope>);
    static_assert(std::is_same_v<::Box, vnnlib::query::Box>);
    static_assert(std::is_same_v<::PolyUnion, vnnlib::query::PolyUnion>);
    static_assert(std::is_same_v<::SpecCase, vnnlib::query::SpecCase>);
    static_assert(std::is_same_v<::CompatTransformer, vnnlib::query::CompatTransformer>);
    static_assert(std::is_same_v<::VNNLibException, vnnlib::query::VNNLibException>);
    static_assert(std::is_same_v<::LinearArithExpr::Term, vnnlib::query::LinearArithExpr::Term>);
    static_assert(std::is_same_v<decltype(&::parseQueryFile), decltype(&vnnlib::query::parseQueryFile)>);
    static_assert(std::is_same_v<decltype(&::parseQueryString), decltype(&vnnlib::query::parseQueryString)>);
    static_assert(std::is_same_v<decltype(&::checkQueryFile), decltype(&vnnlib::query::checkQueryFile)>);
    static_assert(std::is_same_v<decltype(&::checkQueryString), decltype(&vnnlib::query::checkQueryString)>);
    assert(&::dtypeToString == &vnnlib::query::dtypeToString);
    assert(&::isConstant == &vnnlib::query::isConstant);
    assert(&::sameFamily == &vnnlib::query::sameFamily);
    assert(&::sameType == &vnnlib::query::sameType);
    assert(&::linearize == &vnnlib::query::linearize);
    assert(&::toDNF == &vnnlib::query::toDNF);
    assert(&::dnfOf == &vnnlib::query::dnfOf);
    assert(&::dnfOr == &vnnlib::query::dnfOr);
    assert(&::dnfAnd == &vnnlib::query::dnfAnd);
    assert(&::distrib == &vnnlib::query::distrib);
}

void testParsingConsistency(const std::string& content) {
    auto oldString = ::parseQueryString(content);
    auto newString = vnnlib::query::parseQueryString(content);
    assert(oldString);
    assert(newString);
    assert(oldString->toString() == newString->toString());
    assert(::checkQueryString(content) == vnnlib::query::checkQueryString(content));
    assert(::checkQueryString(content).empty());
}

void testLinearizationConsistency(const std::string& content) {
    auto oldQuery = ::parseQueryString(content);
    auto newQuery = vnnlib::query::parseQueryString(content);
    const auto* oldOr = dynamic_cast<const ::TOr*>(oldQuery->assertions[4]->cond.get());
    const auto* newOr = dynamic_cast<const vnnlib::query::TOr*>(newQuery->assertions[4]->cond.get());
    assert(oldOr);
    assert(newOr);
    const auto* oldComparison = dynamic_cast<const ::TCompare*>(oldOr->args[0].get());
    const auto* newComparison = dynamic_cast<const vnnlib::query::TCompare*>(newOr->args[0].get());
    assert(oldComparison);
    assert(newComparison);
    auto oldLinear = ::linearize(oldComparison->lhs.get());
    auto newLinear = vnnlib::query::linearize(newComparison->lhs.get());
    assert(oldLinear);
    assert(newLinear);
    assert(oldLinear->getConstant() == newLinear->getConstant());
    assert(oldLinear->getNumTerms() == newLinear->getNumTerms());
    for (const auto& term : oldLinear->getTerms()) {
        assert(term.coeff == newLinear->getCoefficient(term.varName));
    }
    assert(oldLinear->getConstant() == 3.0);
    assert(oldLinear->getNumTerms() == 1);
    assert(oldLinear->getCoefficient("Y[0]") == 2.0);
}

void testDnfConsistency(const std::string& content) {
    auto oldQuery = ::parseQueryString(content);
    auto newQuery = vnnlib::query::parseQueryString(content);
    assert(oldQuery->assertions.size() == newQuery->assertions.size());
    for (size_t i = 0; i < oldQuery->assertions.size(); ++i) {
        const ::DNF oldDnf = ::toDNF(oldQuery->assertions[i]->cond.get());
        const vnnlib::query::DNF newDnf = vnnlib::query::toDNF(newQuery->assertions[i]->cond.get());
        assert(oldDnf.size() == newDnf.size());
        assert(!oldDnf.empty());
        for (size_t clause = 0; clause < oldDnf.size(); ++clause) {
            assert(oldDnf[clause].size() == newDnf[clause].size());
            assert(!oldDnf[clause].empty());
            for (size_t literal = 0; literal < oldDnf[clause].size(); ++literal) {
                assert(oldDnf[clause][literal]->toString() == newDnf[clause][literal]->toString());
            }
        }
    }
    const auto disjunction = vnnlib::query::toDNF(newQuery->assertions[4]->cond.get());
    assert(disjunction.size() == 2);
    assert(disjunction[0].size() == 1);
    assert(disjunction[1].size() == 1);
}

void testCompatConsistency(const std::string& content) {
    auto oldQuery = ::parseQueryString(content);
    auto newQuery = vnnlib::query::parseQueryString(content);
    ::CompatTransformer oldTransformer(oldQuery.get());
    vnnlib::query::CompatTransformer newTransformer(newQuery.get());
    const auto oldCases = oldTransformer.transform();
    const auto newCases = newTransformer.transform();
    assert(oldCases.size() == newCases.size());
    assert(oldCases.size() == 1);
    for (size_t i = 0; i < oldCases.size(); ++i) {
        assert(oldCases[i].inputBox == newCases[i].inputBox);
        assert(oldCases[i].outputConstraints.size() == newCases[i].outputConstraints.size());
        for (size_t j = 0; j < oldCases[i].outputConstraints.size(); ++j) {
            assert(oldCases[i].outputConstraints[j].coeffMatrix == newCases[i].outputConstraints[j].coeffMatrix);
            assert(oldCases[i].outputConstraints[j].rhs == newCases[i].outputConstraints[j].rhs);
        }
    }
    const vnnlib::query::Box expectedBox = {{0.0, 1.0}, {-1.0, 2.0}};
    assert(newCases[0].inputBox == expectedBox);
    assert(newCases[0].outputConstraints.size() == 2);
    assert(newCases[0].outputConstraints[0].coeffMatrix == std::vector<std::vector<double>>{{2.0}});
    assert(newCases[0].outputConstraints[0].rhs == std::vector<double>{4.0});
    assert(newCases[0].outputConstraints[1].coeffMatrix == std::vector<std::vector<double>>{{-1.0}});
    assert(newCases[0].outputConstraints[1].rhs == std::vector<double>{-3.0});
}

void testExceptionConsistency() {
    std::string oldMessage;
    std::string newMessage;
    try {
        (void)::parseQueryString("not a query");
    } catch (const ::VNNLibException& error) {
        oldMessage = error.what();
    }
    try {
        (void)vnnlib::query::parseQueryString("not a query");
    } catch (const vnnlib::query::VNNLibException& error) {
        newMessage = error.what();
    }
    assert(!oldMessage.empty());
    assert(oldMessage == newMessage);
}

} // namespace

int main() {
    const std::string content = queryText;
    testLegacyTypesAndFunctions();
    testParsingConsistency(content);
    testLinearizationConsistency(content);
    testDnfConsistency(content);
    testCompatConsistency(content);
    testExceptionConsistency();
}
