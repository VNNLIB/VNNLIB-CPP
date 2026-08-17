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
}