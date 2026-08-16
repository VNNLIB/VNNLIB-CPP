#pragma once

#include <string>

#include "VNNLibExport.h"


namespace vnnlib::solver {

class VNNLIB_API Solver {
    private:
    std::string executable_;


    public:
    explicit Solver(const std::string& executable);
};
}