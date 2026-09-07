#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <memory>

#include "TypedAST.h"
#include "Absyn.H"
#include "VNNLibExport.h"

namespace vnnlib::query {

using Literal = const TCompare*;                // A Boolean literal is a comparison (e.g., x <= 5)
using Clause = std::vector<Literal>;            // A clause is a conjunction of literals 
using DNF    = std::vector<Clause>;             // DNF is a disjunction of clauses

VNNLIB_API DNF toDNF(const TBoolExpr* node);
DNF dnfOf(const TBoolExpr* node);
DNF dnfOr(const std::vector<std::unique_ptr<TBoolExpr>>& args);
DNF dnfAnd(const std::vector<std::unique_ptr<TBoolExpr>>& args);
DNF distrib(const DNF& left, const DNF& right);

} // namespace vnnlib::query

#ifndef VNNLIB_NO_DEPRECATED_QUERY_API
using Literal [[deprecated("use vnnlib::query::Literal")]] = vnnlib::query::Literal;
using Clause [[deprecated("use vnnlib::query::Clause")]] = vnnlib::query::Clause;
using DNF [[deprecated("use vnnlib::query::DNF")]] = vnnlib::query::DNF;

using vnnlib::query::toDNF;
using vnnlib::query::dnfOf;
using vnnlib::query::dnfOr;
using vnnlib::query::dnfAnd;
using vnnlib::query::distrib;
#endif
