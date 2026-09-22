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
