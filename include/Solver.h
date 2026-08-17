#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include "VNNLibExport.h"



namespace vnnlib::solver {

enum class VerificationResult {
    Sat,
    Unsat,
    Unknown,
    TimedOut
};



class VNNLIB_API Solver {
    private:
    std::string executable_;

    public:
    explicit Solver(const std::string& executable);

    VerificationResult verify(
    const std::string& query,
    const std::unordered_map<std::string, std::string>& networks,
    std::optional<int> timeout = std::nullopt);


};

}
