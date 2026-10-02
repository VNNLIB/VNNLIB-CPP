# Check compiler diagnostics for the migrated public API.
set(public_types
    Shape
    Indices
    TDataType
    SymbolKind
    SymbolInfo
    TNode
    TElementType
    TArithExpr
    TVarExpr
    TLiteral
    TFloat
    TInt
    TNegate
    TPlus
    TMinus
    TMultiply
    TBoolExpr
    TCompare
    TGreaterThan
    TLessThan
    TGreaterEqual
    TLessEqual
    TEqual
    TNotEqual
    TConnective
    TAnd
    TOr
    TAssertion
    TInputDefinition
    THiddenDefinition
    TOutputDefinition
    TNetworkDefinition
    TVersion
    TQuery
    LinearArithExpr
    Literal
    Clause
    DNF
    Polytope
    Box
    PolyUnion
    SpecCase
    CompatTransformer
    VNNLibException
)
set(parser_functions parseQueryFile parseQueryString checkQueryFile checkQueryString)
set(shared_functions dtypeToString isConstant sameFamily sameType linearize toDNF dnfOf dnfOr dnfAnd distrib)

set(headers "#include \"CompatTransformer.h\"\n#include \"DNFConverter.h\"\n#include \"LinearArithExpr.h\"\n#include \"VNNLib.h\"\n")
set(old_source "${headers}")
set(new_source "#define VNNLIB_NO_DEPRECATED_QUERY_API\n${headers}")
foreach(name IN LISTS public_types)
    string(APPEND old_source "using Used${name} = ::${name};\n")
    string(APPEND new_source "using Used${name} = vnnlib::query::${name};\n")
endforeach()
foreach(name IN LISTS parser_functions shared_functions)
    string(APPEND old_source "auto used_${name} = &::${name};\n")
    string(APPEND new_source "auto used_${name} = &vnnlib::query::${name};\n")
endforeach()

set(generator_args -G "${GENERATOR}")
if(NOT "${GENERATOR_PLATFORM}" STREQUAL "")
    list(APPEND generator_args -A "${GENERATOR_PLATFORM}")
endif()
if(NOT "${GENERATOR_TOOLSET}" STREQUAL "")
    list(APPEND generator_args -T "${GENERATOR_TOOLSET}")
endif()

foreach(mode IN ITEMS old new)
    set(probe_dir "${BINARY_DIR}/query-deprecation/${mode}")
    file(MAKE_DIRECTORY "${probe_dir}")
    file(WRITE "${probe_dir}/probe.cpp" "${${mode}_source}")
    set(options "-Wdeprecated-declarations")
    set(msvc_options "/w14996")
    if(mode STREQUAL "new")
        set(options "-Werror=deprecated-declarations")
        set(msvc_options "/we4996")
    endif()
    file(WRITE "${probe_dir}/CMakeLists.txt"
        "cmake_minimum_required(VERSION 3.16)\n"
        "project(QueryDeprecationProbe LANGUAGES CXX)\n"
        "set(CMAKE_CXX_STANDARD 17)\n"
        "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n"
        "add_library(probe OBJECT probe.cpp)\n"
        "target_include_directories(probe PRIVATE \"${SOURCE_DIR}/include\" \"${SOURCE_DIR}/include/util\" \"${SOURCE_DIR}/src/generated\")\n"
        "if(MSVC)\n"
        "  target_compile_options(probe PRIVATE ${msvc_options})\n"
        "else()\n"
        "  target_compile_options(probe PRIVATE ${options})\n"
        "endif()\n"
    )
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -S "${probe_dir}" -B "${probe_dir}/build"
            ${generator_args} "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
        RESULT_VARIABLE configured OUTPUT_VARIABLE output ERROR_VARIABLE errors
    )
    if(NOT configured EQUAL 0)
        message(FATAL_ERROR "${mode} diagnostic probe configuration failed:\n${output}\n${errors}")
    endif()
    # Rebuild on every run so cached object files cannot hide the diagnostics.
    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${probe_dir}/build" --config Debug --clean-first
        RESULT_VARIABLE built OUTPUT_VARIABLE output ERROR_VARIABLE errors
    )
    set(diagnostics "${output}\n${errors}")
    file(WRITE "${probe_dir}/diagnostics.txt" "${diagnostics}")
    if(NOT built EQUAL 0)
        message(FATAL_ERROR "${mode} diagnostic probe compilation failed:\n${diagnostics}")
    endif()
    if(mode STREQUAL "old")
        foreach(name IN LISTS public_types parser_functions)
            string(FIND "${diagnostics}" "use vnnlib::query::${name}" found)
            if(found EQUAL -1)
                message(FATAL_ERROR "Missing replacement-path warning for ${name}:\n${diagnostics}")
            endif()
        endforeach()
    endif()
endforeach()

message(STATUS "All 44 legacy type aliases and 4 parser/check functions warn with replacement paths; the new API compiles with deprecations treated as errors.")
