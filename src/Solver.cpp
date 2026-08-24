#include "ProcessRunner.h"
#include "Error.hpp"
#include "Solver.h"
#include <vector>


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
    vnnlib::solver::Capability capability)
{
    using vnnlib::solver::Capability;

    std::string argument;

    switch (capability) {
        case Capability::OnnxOpsetVersions:
            argument = "--onnx-opset-versions";
            break;
        case Capability::OnnxElementTypes:
            argument = "--onnx-element-types";
            break;
        case Capability::OnnxOperators:
            argument = "--onnx-operators";
            break;
        case Capability::VNNLibVersions:
            argument = "--vnnlib-versions";
            break;
        case Capability::HiddenNodeTheories:
            argument = "--hidden-node-theories";
            break;
        case Capability::MultipleInputOutputTheories:
            argument = "--multiple-input-output-theories";
            break;
        case Capability::MultipleNetworkTheories:
            argument = "--multiple-network-theories";
            break;
        case Capability::MultipleNodeComparisonTheories:
            argument = "--multiple-node-comparison-theories";
            break;
        case Capability::ArithmeticComplexityTheories:
            argument = "--arithmetic-complexity-theories";
            break;
        case Capability::OptimisedDisjunctiveReasoning:
            argument = "--optimised-disjunctive-reasoning";
            break;
        case Capability::SerialiseAssignments:
            argument = "--serialise-assignments";
            break;
        default:
            throw VNNLibException("Unknown solver capability");
    }

    return {"supports", argument};
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

    return parseVerificationResult(result.stdoutText);
}

}