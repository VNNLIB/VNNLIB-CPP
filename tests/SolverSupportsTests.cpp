#include "Error.hpp"
#include "Solver.h"
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using vnnlib::solver::Capability;
using vnnlib::solver::OperatorSupport;
using vnnlib::solver::Solver;
using vnnlib::solver::VersionRange;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Expected test solver paths\n";
        return 1;
    }

    Solver solver(argv[1]);

    VersionRange opset = std::get<VersionRange>(
        solver.supports(Capability::OnnxOpsetVersions));

    if (opset.minimum != "13" || opset.maximum != "21") {
        std::cerr << "Unexpected ONNX opset versions\n";
        return 1;
    }

    std::vector<std::string> elementTypes =
        std::get<std::vector<std::string>>(
            solver.supports(Capability::OnnxElementTypes));

    if (elementTypes !=
        std::vector<std::string>{"real", "float32", "float64"}) {
        std::cerr << "Unexpected ONNX element types\n";
        return 1;
    }

    std::vector<OperatorSupport> operators =
        std::get<std::vector<OperatorSupport>>(
            solver.supports(Capability::OnnxOperators));

    if (operators.size() != 2 ||
        operators[0].name != "Gemm" ||
        operators[0].elementTypes !=
            std::vector<std::string>{"float32", "float64"} ||
        operators[1].name != "Relu" ||
        !operators[1].elementTypes.empty()) {
        std::cerr << "Unexpected ONNX operators\n";
        return 1;
    }

    VersionRange versions = std::get<VersionRange>(
        solver.supports(Capability::VNNLibVersions));

    if (versions.minimum != "2.0" || versions.maximum != "2.0") {
        std::cerr << "Unexpected VNN-LIB versions\n";
        return 1;
    }

    if (std::get<std::vector<std::string>>(
            solver.supports(Capability::HiddenNodeTheories)) !=
        std::vector<std::string>{"NH", "H"}) {
        std::cerr << "Unexpected hidden node theories\n";
        return 1;
    }

    if (std::get<std::vector<std::string>>(
            solver.supports(Capability::MultipleInputOutputTheories)) !=
        std::vector<std::string>{"SIO", "MIO"}) {
        std::cerr << "Unexpected input/output theories\n";
        return 1;
    }

    if (std::get<std::vector<std::string>>(
            solver.supports(Capability::MultipleNetworkTheories)) !=
        std::vector<std::string>{"SNET", "MNET", "MENET", "MINET"}) {
        std::cerr << "Unexpected network theories\n";
        return 1;
    }

    if (std::get<std::vector<std::string>>(
            solver.supports(Capability::MultipleNodeComparisonTheories)) !=
        std::vector<std::string>{"SNC", "MNC"}) {
        std::cerr << "Unexpected comparison theories\n";
        return 1;
    }

    if (std::get<std::vector<std::string>>(
            solver.supports(Capability::ArithmeticComplexityTheories)) !=
        std::vector<std::string>{"BND", "OUTC", "LIN", "POLY"}) {
        std::cerr << "Unexpected arithmetic theories\n";
        return 1;
    }

    if (!std::get<bool>(
            solver.supports(Capability::OptimisedDisjunctiveReasoning))) {
        std::cerr << "Unexpected disjunctive reasoning support\n";
        return 1;
    }

    if (std::get<bool>(
            solver.supports(Capability::SerialiseAssignments))) {
        std::cerr << "Unexpected assignment support\n";
        return 1;
    }

    Solver edgeSolver(argv[2]);

    bool malformedThrown = false;

    try {
        edgeSolver.supports(Capability::OnnxOpsetVersions);
    } catch (const VNNLibException&) {
        malformedThrown = true;
    }

    if (!malformedThrown) {
        std::cerr << "Malformed supports output did not throw VNNLibException\n";
        return 1;
    }

    std::vector<std::string> stderrResult =
        std::get<std::vector<std::string>>(
            edgeSolver.supports(Capability::OnnxElementTypes));

    if (stderrResult != std::vector<std::string>{"real", "float32"}) {
        std::cerr << "stderr affected supports result\n";
        return 1;
    }

    if (!std::get<bool>(
            edgeSolver.supports(Capability::OptimisedDisjunctiveReasoning))) {
        std::cerr << "Non-zero exit affected valid supports result\n";
        return 1;
    }


    bool crashThrown = false;

    try {
        edgeSolver.supports(Capability::SerialiseAssignments);
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
        missingSolver.supports(Capability::OnnxOpsetVersions);
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
