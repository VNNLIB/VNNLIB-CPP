#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cerrno>
#include <cstring>

#include "Absyn.H"
#include "Parser.H"
#include "TypedBuilder.h"
#include "Printer.H"
#include "ParserError.H"
#include "VNNLibExport.h"

#include "Error.hpp"

namespace vnnlib::query {

VNNLIB_API std::unique_ptr<TQuery> parseQueryFile(std::string path);
VNNLIB_API std::unique_ptr<TQuery> parseQueryString(std::string content);
VNNLIB_API std::string checkQueryFile(std::string path);
VNNLIB_API std::string checkQueryString(std::string content);

// Compute the least permissive theory on demand (VNN-LIB 2.0, 4.1.1-4.1.6).
VNNLIB_API std::string hiddenNodeTheory(const TQuery& query); // NH or H
VNNLIB_API std::string inputOutputTheory(const TQuery& query); // SIO or MIO
VNNLIB_API std::string multipleNetworksTheory(const TQuery& query); // SNET, MNET, MINET, or MENET
VNNLIB_API std::string multipleNodeComparisonsTheory(const TQuery& query); // SNC or MNC
VNNLIB_API std::string arithmeticComplexityTheory(const TQuery& query); // BND, OUTC, LIN, or POLY
VNNLIB_API std::vector<std::string> elementTypeTheories(const TQuery& query); // one entry per declared element type

} // namespace vnnlib::query

#ifndef VNNLIB_NO_DEPRECATED_QUERY_API
[[deprecated("use vnnlib::query::parseQueryFile")]]
VNNLIB_API std::unique_ptr<vnnlib::query::TQuery> parseQueryFile(std::string path);
[[deprecated("use vnnlib::query::parseQueryString")]]
VNNLIB_API std::unique_ptr<vnnlib::query::TQuery> parseQueryString(std::string content);
[[deprecated("use vnnlib::query::checkQueryFile")]]
VNNLIB_API std::string checkQueryFile(std::string path);
[[deprecated("use vnnlib::query::checkQueryString")]]
VNNLIB_API std::string checkQueryString(std::string content);
#endif
