#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "TypedAST.h"
#include "VNNLibExport.h"


namespace vnnlib::solver {

enum class VerificationResult {
    Sat,
    Unsat,
    Unknown,
    TimedOut
};

enum class HiddenNodeTheory {
    NH,
    H
};

enum class MultipleInputOutputTheory {
    SIO,
    MIO
};

enum class MultipleNetworkTheory {
    SNET,
    MENET,
    MINET,
    MNET
};

enum class MultipleNodeComparisonTheory {
    SNC,
    MNC
};

enum class ArithmeticComplexityTheory {
    BND,
    OUTC,
    LIN,
    POLY
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
    std::vector<vnnlib::query::TDataType> elementTypes;
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
    std::vector<vnnlib::query::TDataType> supportsOnnxElementTypes();
    std::vector<OperatorSupport> supportsOnnxOperators();
    VersionRange supportsVNNLibVersions();
    std::vector<HiddenNodeTheory> supportsHiddenNodeTheories();
    std::vector<MultipleInputOutputTheory> supportsMultipleInputOutputTheories();
    std::vector<MultipleNetworkTheory> supportsMultipleNetworkTheories();
    std::vector<MultipleNodeComparisonTheory> supportsMultipleNodeComparisonTheories();
    std::vector<ArithmeticComplexityTheory> supportsArithmeticComplexityTheories();
    bool supportsOptimisedDisjunctiveReasoning();
    bool supportsSerialiseAssignments();

};

}
