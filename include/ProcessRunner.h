#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <optional>
#include "VNNLibExport.h"

namespace vnnlib::solver {
    struct ProcessResult {
        std::string stdoutText;
        std::string stderrText;
        std::optional<int> exitCode;
        bool exitedNormally;
    };

    ProcessResult runProcess(
        const std::string& executable,
        const std::vector<std::string>& arguments
    );
}