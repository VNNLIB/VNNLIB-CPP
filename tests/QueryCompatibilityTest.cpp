#include <cassert>
#include <type_traits>

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
(assert (<= X[0] 1.0))
)";

} // namespace

int main() {
    const ::TDataType oldType = ::TDataType::F32;
    assert(dtypeToString(oldType) == "F32");

    auto oldQuery = ::parseQueryString(queryText);
    auto newQuery = vnnlib::query::parseQueryString(queryText);
    assert(oldQuery);
    assert(newQuery);
    assert(oldQuery->toString() == newQuery->toString());

    const auto* oldComparison =
        dynamic_cast<const ::TCompare*>(oldQuery->assertions[0]->cond.get());
    assert(oldComparison);

    auto oldLinear = linearize(oldComparison->lhs.get());
    auto newLinear = vnnlib::query::linearize(oldComparison->lhs.get());
    assert(oldLinear->toString() == newLinear->toString());

    const ::DNF oldDnf = toDNF(oldQuery->assertions[0]->cond.get());
    const vnnlib::query::DNF newDnf =
        vnnlib::query::toDNF(oldQuery->assertions[0]->cond.get());
    assert(oldDnf.size() == newDnf.size());
}
