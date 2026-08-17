#pragma once

#include <string>

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
};
}