#include "Error.hpp"
#include "Solver.h"
#include <iostream>
#include <string>
#include <vector>

using vnnlib::solver::OperatorSupport;
using vnnlib::solver::OpsetRange;
using vnnlib::solver::Solver;
using vnnlib::solver::VersionRange;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Expected test solver paths\n";
        return 1;
    }

    Solver solver(argv[1]);

    OpsetRange opset = solver.supportsOnnxOpsetVersions();

    if (opset.minimum != 13 || opset.maximum != 21) {
        std::cerr << "Unexpected ONNX opset versions\n";
        return 1;
    }

    std::vector<std::string> elementTypes =
        solver.supportsOnnxElementTypes();

    if (elementTypes !=
        std::vector<std::string>{"real", "float32", "float64"}) {
        std::cerr << "Unexpected ONNX element types\n";
        return 1;
    }

    std::vector<OperatorSupport> operators =
        solver.supportsOnnxOperators();

    if (operators.size() != 2 ||
        operators[0].name != "Gemm" ||
        operators[0].elementTypes !=
            std::vector<std::string>{"float32", "float64"} ||
        operators[1].name != "Relu" ||
        !operators[1].elementTypes.empty()) {
        std::cerr << "Unexpected ONNX operators\n";
        return 1;
    }

    VersionRange versions = solver.supportsVNNLibVersions();

    if (versions.minimum.major != 2 ||
        versions.minimum.minor != 0 ||
        versions.minimum.patch.has_value() ||
        !versions.minimum.extra.empty() ||
        versions.maximum.major != 2 ||
        versions.maximum.minor != 3 ||
        versions.maximum.patch != 1 ||
        versions.maximum.extra != "-beta") {
        std::cerr << "Unexpected VNN-LIB versions\n";
        return 1;
    }

    if (solver.supportsHiddenNodeTheories() !=
        std::vector<std::string>{"NH", "H"}) {
        std::cerr << "Unexpected hidden node theories\n";
        return 1;
    }

    if (solver.supportsMultipleInputOutputTheories() !=
        std::vector<std::string>{"SIO", "MIO"}) {
        std::cerr << "Unexpected input/output theories\n";
        return 1;
    }

    if (solver.supportsMultipleNetworkTheories() !=
        std::vector<std::string>{"SNET", "MNET", "MENET", "MINET"}) {
        std::cerr << "Unexpected network theories\n";
        return 1;
    }

    if (solver.supportsMultipleNodeComparisonTheories() !=
        std::vector<std::string>{"SNC", "MNC"}) {
        std::cerr << "Unexpected comparison theories\n";
        return 1;
    }

    if (solver.supportsArithmeticComplexityTheories() !=
        std::vector<std::string>{"BND", "OUTC", "LIN", "POLY"}) {
        std::cerr << "Unexpected arithmetic theories\n";
        return 1;
    }

    if (!solver.supportsOptimisedDisjunctiveReasoning()) {
        std::cerr << "Unexpected disjunctive reasoning support\n";
        return 1;
    }

    if (solver.supportsSerialiseAssignments()) {
        std::cerr << "Unexpected assignment support\n";
        return 1;
    }

    Solver edgeSolver(argv[2]);

    bool malformedThrown = false;

    try {
        edgeSolver.supportsOnnxOpsetVersions();
    } catch (const VNNLibException&) {
        malformedThrown = true;
    }

    if (!malformedThrown) {
        std::cerr << "Malformed supports output did not throw VNNLibException\n";
        return 1;
    }

    std::vector<std::string> stderrResult =
        edgeSolver.supportsOnnxElementTypes();

    if (stderrResult != std::vector<std::string>{"real", "float32"}) {
        std::cerr << "stderr affected supports result\n";
        return 1;
    }

    if (!edgeSolver.supportsOptimisedDisjunctiveReasoning()) {
        std::cerr << "Non-zero exit affected valid supports result\n";
        return 1;
    }


    bool crashThrown = false;

    try {
        edgeSolver.supportsSerialiseAssignments();
    } catch (const VNNLibException&) {
        crashThrown = true;
    }

    if (!crashThrown) {
        std::cerr << "Abnormal supports termination did not throw VNNLibException\n";
        return 1;
    }

    Solver missingSolver("definitely-not-a-real-vnnlib-solver");

    bool missingThrown = false;

    try {
        missingSolver.supportsOnnxOpsetVersions();
    } catch (const VNNLibException&) {
        missingThrown = true;
    }

    if (!missingThrown) {
        std::cerr << "Missing solver executable did not throw VNNLibException\n";
        return 1;
    }

    std::cout << "Solver supports tests passed\n";
    return 0;
}
