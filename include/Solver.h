#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "VNNLibExport.h"


namespace vnnlib::solver {

enum class VerificationResult {
    Sat,
    Unsat,
    Unknown,
    TimedOut
};

enum class Capability {
    OnnxOpsetVersions,
    OnnxElementTypes,
    OnnxOperators,
    VNNLibVersions,
    HiddenNodeTheories,
    MultipleInputOutputTheories,
    MultipleNetworkTheories,
    MultipleNodeComparisonTheories,
    ArithmeticComplexityTheories,
    OptimisedDisjunctiveReasoning,
    SerialiseAssignments
};

struct VNNLIB_API VersionRange {
    std::string minimum;
    std::string maximum;
};

// empty element types means all reported types
struct VNNLIB_API OperatorSupport {
    std::string name;
    std::vector<std::string> elementTypes;
};

using SupportResult = std::variant<
    VersionRange,
    std::vector<std::string>,
    std::vector<OperatorSupport>,
    bool
>;



class VNNLIB_API Solver {
    private:
    std::string executable_;

    public:
    explicit Solver(const std::string& executable);

    VerificationResult verify(
    const std::string& query,
    const std::unordered_map<std::string, std::string>& networks,
    std::optional<int> timeout = std::nullopt);

    SupportResult supports(Capability capability);

};

}
