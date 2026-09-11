#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "VNNLibExport.h"


namespace vnnlib::solver {

enum class VerificationResult {
    Sat,
    Unsat,
    Unknown,
    TimedOut
};

struct VNNLIB_API SemanticVersion {
    int major;
    int minor;
    std::optional<int> patch;
    std::string extra;
};

struct VNNLIB_API VersionRange {
    SemanticVersion minimum;
    SemanticVersion maximum;
};

struct VNNLIB_API OpsetRange {
    int minimum;
    int maximum;
};

// empty element types means all reported types
struct VNNLIB_API OperatorSupport {
    std::string name;
    std::vector<std::string> elementTypes;
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

    OpsetRange supportsOnnxOpsetVersions();
    std::vector<std::string> supportsOnnxElementTypes();
    std::vector<OperatorSupport> supportsOnnxOperators();
    VersionRange supportsVNNLibVersions();
    std::vector<std::string> supportsHiddenNodeTheories();
    std::vector<std::string> supportsMultipleInputOutputTheories();
    std::vector<std::string> supportsMultipleNetworkTheories();
    std::vector<std::string> supportsMultipleNodeComparisonTheories();
    std::vector<std::string> supportsArithmeticComplexityTheories();
    bool supportsOptimisedDisjunctiveReasoning();
    bool supportsSerialiseAssignments();

};

}
