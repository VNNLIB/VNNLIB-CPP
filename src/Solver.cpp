#include "ProcessRunner.h"
#include "Error.hpp"
#include "Solver.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include "ProcessRunner.h"


namespace {
//Build verify command args
std::vector<std::string> buildVerifyArguments(
    const std::string& query,
    const std::unordered_map<std::string, std::string>& networks,
    std::optional<int> timeout) 
{
    std::vector<std::string> arguments = {"verify", query};

    for (const auto& [name, path] : networks) {
        arguments.push_back("--network");
        arguments.push_back(name + "=" + path);
    }

    if (timeout.has_value()) {
        arguments.push_back("--timeout");
        arguments.push_back(std::to_string(*timeout));
    }
    return arguments;



    
}



//Build supports command args
std::vector<std::string> buildSupportsArguments(
    const std::string& capability)
{
    return {"supports", capability};
}


//Split output into lines
std::vector<std::string> splitLines(const std::string& output)
{
    std::vector<std::string> lines;
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        lines.push_back(line);
    }

    return lines;
}



//Parse integer
int parseInteger(
    const std::string& value,
    const std::string& errorMessage)
{
    if (value.empty()) {
        throw vnnlib::query::VNNLibException(errorMessage);
    }

    for (char character : value) {
        if (character < '0' || character > '9') {
            throw vnnlib::query::VNNLibException(errorMessage);
        }
    }

    try {
        return std::stoi(value);
    } catch (...) {
        throw vnnlib::query::VNNLibException(errorMessage);
    }
}

//Parse semantic version
vnnlib::solver::SemanticVersion parseSemanticVersion(
    const std::string& value)
{
    const std::string errorMessage = "Malformed version range output";

    size_t firstDot = value.find('.');

    if (firstDot == std::string::npos) {
        throw vnnlib::query::VNNLibException(errorMessage);
    }

    std::string majorText = value.substr(0, firstDot);

    size_t minorStart = firstDot + 1;
    size_t minorEnd = minorStart;

    while (minorEnd < value.size() &&
           value[minorEnd] >= '0' &&
           value[minorEnd] <= '9') {
        minorEnd++;
    }

    if (minorEnd == minorStart) {
        throw vnnlib::query::VNNLibException(errorMessage);
    }

    std::string minorText =
        value.substr(minorStart, minorEnd - minorStart);

    std::optional<int> patch = std::nullopt;
    std::string extra;

    if (minorEnd < value.size() && value[minorEnd] == '.') {
        size_t patchStart = minorEnd + 1;
        size_t patchEnd = patchStart;

        while (patchEnd < value.size() &&
               value[patchEnd] >= '0' &&
               value[patchEnd] <= '9') {
            patchEnd++;
        }

        if (patchEnd == patchStart) {
            throw vnnlib::query::VNNLibException(errorMessage);
        }

        patch = parseInteger(
            value.substr(patchStart, patchEnd - patchStart),
            errorMessage);

        extra = value.substr(patchEnd);
    } else {
        extra = value.substr(minorEnd);
    }

    return {
        parseInteger(majorText, errorMessage),
        parseInteger(minorText, errorMessage),
        patch,
        extra
    };
}

//Parse version range
vnnlib::solver::VersionRange parseVersionRange(
    const std::string& output)
{
    std::vector<std::string> lines = splitLines(output);

    if (lines.size() != 2 || lines[0].empty() || lines[1].empty()) {
        throw vnnlib::query::VNNLibException("Malformed version range output");
    }

    return {
        parseSemanticVersion(lines[0]),
        parseSemanticVersion(lines[1])
    };
}

//Parse ONNX opset range
vnnlib::solver::OpsetRange parseOpsetRange(
    const std::string& output)
{
    std::vector<std::string> lines = splitLines(output);

    if (lines.size() != 2 || lines[0].empty() || lines[1].empty()) {
        throw vnnlib::query::VNNLibException("Malformed opset range output");
    }

    return {
        parseInteger(lines[0], "Malformed opset range output"),
        parseInteger(lines[1], "Malformed opset range output")
    };
}



//Parse list output
std::vector<std::string> parseSupportList(
    const std::string& output)
{
    std::vector<std::string> lines = splitLines(output);

    for (const std::string& line : lines) {
        if (line.empty()) {
            throw vnnlib::query::VNNLibException("Malformed support list output");
        }
    }

    return lines;
}




//Parse element type
vnnlib::query::TDataType parseElementType(const std::string& value)
{
    if (value == "real") return vnnlib::query::TDataType::Real;
    if (value == "float16") return vnnlib::query::TDataType::F16;
    if (value == "float32") return vnnlib::query::TDataType::F32;
    if (value == "float64") return vnnlib::query::TDataType::F64;
    if (value == "bfloat16") return vnnlib::query::TDataType::BF16;
    if (value == "float8e4m3fn") return vnnlib::query::TDataType::F8E4M3FN;
    if (value == "float8e5m2") return vnnlib::query::TDataType::F8E5M2;
    if (value == "float8e4m3fnuz") return vnnlib::query::TDataType::F8E4M3FNUZ;
    if (value == "float8e5m2fnuz") return vnnlib::query::TDataType::F8E5M2FNUZ;
    if (value == "float4e2m1") return vnnlib::query::TDataType::F4E2M1;
    if (value == "int8") return vnnlib::query::TDataType::I8;
    if (value == "int16") return vnnlib::query::TDataType::I16;
    if (value == "int32") return vnnlib::query::TDataType::I32;
    if (value == "int64") return vnnlib::query::TDataType::I64;
    if (value == "uint8") return vnnlib::query::TDataType::U8;
    if (value == "uint16") return vnnlib::query::TDataType::U16;
    if (value == "uint32") return vnnlib::query::TDataType::U32;
    if (value == "uint64") return vnnlib::query::TDataType::U64;
    if (value == "complex64") return vnnlib::query::TDataType::C64;
    if (value == "complex128") return vnnlib::query::TDataType::C128;
    if (value == "bool") return vnnlib::query::TDataType::Bool;

    throw vnnlib::query::VNNLibException("Malformed element type support output");
}

//Parse element type list
std::vector<vnnlib::query::TDataType> parseElementTypes(
    const std::string& output)
{
    std::vector<std::string> values = parseSupportList(output);
    std::vector<vnnlib::query::TDataType> elementTypes;

    for (const std::string& value : values) {
        elementTypes.push_back(parseElementType(value));
    }

    return elementTypes;
}


//Parse boolean output
bool parseSupportBoolean(const std::string& output)
{
    std::vector<std::string> lines = splitLines(output);

    if (lines.size() != 1) {
        throw vnnlib::query::VNNLibException("Malformed boolean support output");
    }

    if (lines[0] == "true") {
        return true;
    }

    if (lines[0] == "false") {
        return false;
    }

    throw vnnlib::query::VNNLibException("Malformed boolean support output");
}




//Parse operator output
std::vector<vnnlib::solver::OperatorSupport> parseOperatorSupport(
    const std::string& output)
{
    std::vector<vnnlib::solver::OperatorSupport> operators;
    std::vector<std::string> lines = splitLines(output);

    for (const std::string& line : lines) {
        if (line.empty()) {
            throw vnnlib::query::VNNLibException("Malformed operator support output");
        }

        std::istringstream stream(line);
        vnnlib::solver::OperatorSupport operatorSupport;

        if (!(stream >> operatorSupport.name)) {
            throw vnnlib::query::VNNLibException("Malformed operator support output");
        }

        std::string elementType;

        while (stream >> elementType) {
            operatorSupport.elementTypes.push_back(
                parseElementType(elementType));
        }

        operators.push_back(operatorSupport);
    }

    return operators;
}



//Parse hidden node theories
std::vector<vnnlib::solver::HiddenNodeTheory> parseHiddenNodeTheories(
    const std::string& output)
{
    std::vector<std::string> values = parseSupportList(output);
    std::vector<vnnlib::solver::HiddenNodeTheory> theories;

    for (const std::string& value : values) {
        if (value == "NH") {
            theories.push_back(vnnlib::solver::HiddenNodeTheory::NH);
        } else if (value == "H") {
            theories.push_back(vnnlib::solver::HiddenNodeTheory::H);
        } else {
            throw vnnlib::query::VNNLibException("Malformed theory support output");
        }
    }

    return theories;
}

//Parse multiple input output theories
std::vector<vnnlib::solver::MultipleInputOutputTheory> parseMultipleInputOutputTheories(
    const std::string& output)
{
    std::vector<std::string> values = parseSupportList(output);
    std::vector<vnnlib::solver::MultipleInputOutputTheory> theories;

    for (const std::string& value : values) {
        if (value == "SIO") {
            theories.push_back(vnnlib::solver::MultipleInputOutputTheory::SIO);
        } else if (value == "MIO") {
            theories.push_back(vnnlib::solver::MultipleInputOutputTheory::MIO);
        } else {
            throw vnnlib::query::VNNLibException("Malformed theory support output");
        }
    }

    return theories;
}

//Parse multiple network theories
std::vector<vnnlib::solver::MultipleNetworkTheory> parseMultipleNetworkTheories(
    const std::string& output)
{
    std::vector<std::string> values = parseSupportList(output);
    std::vector<vnnlib::solver::MultipleNetworkTheory> theories;

    for (const std::string& value : values) {
        if (value == "SNET") {
            theories.push_back(vnnlib::solver::MultipleNetworkTheory::SNET);
        } else if (value == "MENET") {
            theories.push_back(vnnlib::solver::MultipleNetworkTheory::MENET);
        } else if (value == "MINET") {
            theories.push_back(vnnlib::solver::MultipleNetworkTheory::MINET);
        } else if (value == "MNET") {
            theories.push_back(vnnlib::solver::MultipleNetworkTheory::MNET);
        } else {
            throw vnnlib::query::VNNLibException("Malformed theory support output");
        }
    }

    return theories;
}

//Parse multiple node comparison theories
std::vector<vnnlib::solver::MultipleNodeComparisonTheory> parseMultipleNodeComparisonTheories(
    const std::string& output)
{
    std::vector<std::string> values = parseSupportList(output);
    std::vector<vnnlib::solver::MultipleNodeComparisonTheory> theories;

    for (const std::string& value : values) {
        if (value == "SNC") {
            theories.push_back(vnnlib::solver::MultipleNodeComparisonTheory::SNC);
        } else if (value == "MNC") {
            theories.push_back(vnnlib::solver::MultipleNodeComparisonTheory::MNC);
        } else {
            throw vnnlib::query::VNNLibException("Malformed theory support output");
        }
    }

    return theories;
}

//Parse arithmetic complexity theories
std::vector<vnnlib::solver::ArithmeticComplexityTheory> parseArithmeticComplexityTheories(
    const std::string& output)
{
    std::vector<std::string> values = parseSupportList(output);
    std::vector<vnnlib::solver::ArithmeticComplexityTheory> theories;

    for (const std::string& value : values) {
        if (value == "BND") {
            theories.push_back(vnnlib::solver::ArithmeticComplexityTheory::BND);
        } else if (value == "OUTC") {
            theories.push_back(vnnlib::solver::ArithmeticComplexityTheory::OUTC);
        } else if (value == "LIN") {
            theories.push_back(vnnlib::solver::ArithmeticComplexityTheory::LIN);
        } else if (value == "POLY") {
            theories.push_back(vnnlib::solver::ArithmeticComplexityTheory::POLY);
        } else {
            throw vnnlib::query::VNNLibException("Malformed theory support output");
        }
    }

    return theories;
}



//Run supports command
std::string runSupports(
    const std::string& executable,
    const std::string& capability)
{
    std::vector<std::string> arguments =
        buildSupportsArguments(capability);

    vnnlib::solver::ProcessResult result =
        vnnlib::solver::runProcess(executable, arguments);

    if (!result.exitedNormally || !result.exitCode.has_value() || result.exitCode.value() != 0) {
        throw vnnlib::query::VNNLibException("Solver process failed");
    }

    return result.stdoutText;
}


//Read verify result
vnnlib::solver::VerificationResult parseVerificationResult(
    const std::string& output)
{
    std::string result = output.substr(0, output.find('\n'));

    if (!result.empty() && result.back() == '\r') {
        result.pop_back();
    }

    if (result == "sat") {
        return vnnlib::solver::VerificationResult::Sat;
    }

    if (result == "unsat") {
        return vnnlib::solver::VerificationResult::Unsat;
    }

    if (result == "unknown") {
        return vnnlib::solver::VerificationResult::Unknown;
    }

    if (result == "timed-out") {
        return vnnlib::solver::VerificationResult::TimedOut;
    }

    throw vnnlib::query::VNNLibException("Malformed solver output: "+ result);
}
}



namespace vnnlib::solver {
Solver::Solver(const std::string& executable)
    : executable_(executable) {
}


VerificationResult Solver::verify(
    const std::string& query,
    const std::unordered_map<std::string, std::string>& networks,
    std::optional<int> timeout)
{
    std::vector<std::string> arguments =
        buildVerifyArguments(query, networks, timeout);

    ProcessResult result = runProcess(executable_, arguments);

    if (!result.exitedNormally || !result.exitCode.has_value() || result.exitCode.value() != 0) {
        throw vnnlib::query::VNNLibException("Solver process failed");
    }

    return parseVerificationResult(result.stdoutText);
}

OpsetRange Solver::supportsOnnxOpsetVersions()
{
    return parseOpsetRange(
        runSupports(executable_, "--onnx-opset-versions"));
}

std::vector<vnnlib::query::TDataType> Solver::supportsOnnxElementTypes()
{
    return parseElementTypes(
        runSupports(executable_, "--onnx-element-types"));
}

std::vector<OperatorSupport> Solver::supportsOnnxOperators()
{
    return parseOperatorSupport(
        runSupports(executable_, "--onnx-operators"));
}

VersionRange Solver::supportsVNNLibVersions()
{
    return parseVersionRange(
        runSupports(executable_, "--vnnlib-versions"));
}

std::vector<HiddenNodeTheory> Solver::supportsHiddenNodeTheories()
{
    return parseHiddenNodeTheories(
        runSupports(executable_, "--hidden-node-theories"));
}

std::vector<MultipleInputOutputTheory> Solver::supportsMultipleInputOutputTheories()
{
    return parseMultipleInputOutputTheories(
        runSupports(executable_, "--multiple-input-output-theories"));
}

std::vector<MultipleNetworkTheory> Solver::supportsMultipleNetworkTheories()
{
    return parseMultipleNetworkTheories(
        runSupports(executable_, "--multiple-network-theories"));
}

std::vector<MultipleNodeComparisonTheory> Solver::supportsMultipleNodeComparisonTheories()
{
    return parseMultipleNodeComparisonTheories(
        runSupports(executable_, "--multiple-node-comparison-theories"));
}

std::vector<ArithmeticComplexityTheory> Solver::supportsArithmeticComplexityTheories()
{
    return parseArithmeticComplexityTheories(
        runSupports(executable_, "--arithmetic-complexity-theories"));
}

bool Solver::supportsOptimisedDisjunctiveReasoning()
{
    return parseSupportBoolean(
        runSupports(executable_, "--optimised-disjunctive-reasoning"));
}

bool Solver::supportsSerialiseAssignments()
{
    return parseSupportBoolean(
        runSupports(executable_, "--serialise-assignments"));
}
}
