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
        throw VNNLibException(errorMessage);
    }

    for (char character : value) {
        if (character < '0' || character > '9') {
            throw VNNLibException(errorMessage);
        }
    }

    try {
        return std::stoi(value);
    } catch (...) {
        throw VNNLibException(errorMessage);
    }
}

//Parse semantic version
vnnlib::solver::SemanticVersion parseSemanticVersion(
    const std::string& value)
{
    const std::string errorMessage = "Malformed version range output";

    size_t firstDot = value.find('.');

    if (firstDot == std::string::npos) {
        throw VNNLibException(errorMessage);
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
        throw VNNLibException(errorMessage);
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
            throw VNNLibException(errorMessage);
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
        throw VNNLibException("Malformed version range output");
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
        throw VNNLibException("Malformed opset range output");
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
            throw VNNLibException("Malformed support list output");
        }
    }

    return lines;
}




//Parse boolean output
bool parseSupportBoolean(const std::string& output)
{
    std::vector<std::string> lines = splitLines(output);

    if (lines.size() != 1) {
        throw VNNLibException("Malformed boolean support output");
    }

    if (lines[0] == "true") {
        return true;
    }

    if (lines[0] == "false") {
        return false;
    }

    throw VNNLibException("Malformed boolean support output");
}




//Parse operator output
std::vector<vnnlib::solver::OperatorSupport> parseOperatorSupport(
    const std::string& output)
{
    std::vector<vnnlib::solver::OperatorSupport> operators;
    std::vector<std::string> lines = splitLines(output);

    for (const std::string& line : lines) {
        if (line.empty()) {
            throw VNNLibException("Malformed operator support output");
        }

        std::istringstream stream(line);
        vnnlib::solver::OperatorSupport operatorSupport;

        if (!(stream >> operatorSupport.name)) {
            throw VNNLibException("Malformed operator support output");
        }

        std::string elementType;

        while (stream >> elementType) {
            operatorSupport.elementTypes.push_back(elementType);
        }

        operators.push_back(operatorSupport);
    }

    return operators;
}



//Parse theory list
std::vector<std::string> parseTheoryList(
    const std::string& output,
    const std::vector<std::string>& allowed)
{
    std::vector<std::string> values = parseSupportList(output);

    for (const std::string& value : values) {
        if (std::find(allowed.begin(), allowed.end(), value) == allowed.end()) {
            throw VNNLibException("Malformed theory support output");
        }
    }

    return values;
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

    if (!result.exitedNormally) {
        throw VNNLibException("Solver process terminated abnormally");
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

    throw VNNLibException("Malformed solver output: "+ result);
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

    if (!result.exitedNormally) {
        throw VNNLibException("Solver process terminated abnormally");
    }

    return parseVerificationResult(result.stdoutText);
}

OpsetRange Solver::supportsOnnxOpsetVersions()
{
    return parseOpsetRange(
        runSupports(executable_, "--onnx-opset-versions"));
}

std::vector<std::string> Solver::supportsOnnxElementTypes()
{
    return parseSupportList(
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

std::vector<std::string> Solver::supportsHiddenNodeTheories()
{
    return parseTheoryList(
        runSupports(executable_, "--hidden-node-theories"),
        {"NH", "H"});
}

std::vector<std::string> Solver::supportsMultipleInputOutputTheories()
{
    return parseTheoryList(
        runSupports(executable_, "--multiple-input-output-theories"),
        {"SIO", "MIO"});
}

std::vector<std::string> Solver::supportsMultipleNetworkTheories()
{
    return parseTheoryList(
        runSupports(executable_, "--multiple-network-theories"),
        {"SNET", "MNET", "MENET", "MINET"});
}

std::vector<std::string> Solver::supportsMultipleNodeComparisonTheories()
{
    return parseTheoryList(
        runSupports(executable_, "--multiple-node-comparison-theories"),
        {"SNC", "MNC"});
}

std::vector<std::string> Solver::supportsArithmeticComplexityTheories()
{
    return parseTheoryList(
        runSupports(executable_, "--arithmetic-complexity-theories"),
        {"BND", "OUTC", "LIN", "POLY"});
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
