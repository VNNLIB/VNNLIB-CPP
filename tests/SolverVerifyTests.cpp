#include "Error.hpp"
#include "Solver.h"
#include <iostream>
#include <string>
#include <unordered_map>

using vnnlib::solver::Solver;
using vnnlib::solver::VerificationResult;

bool checkResult(
    Solver& solver,
    const std::string& query,
    VerificationResult expected)
{
    if (solver.verify(query, {}) != expected) {
        std::cerr << "Unexpected result for " << query << '\n';
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Expected test solver path\n";
        return 1;
    }

    Solver solver(argv[1]);

    if (!checkResult(solver, "sat.vnnlib", VerificationResult::Sat)) {
        return 1;
    }

    if (!checkResult(solver, "unsat.vnnlib", VerificationResult::Unsat)) {
        return 1;
    }

    if (!checkResult(solver, "unknown.vnnlib", VerificationResult::Unknown)) {
        return 1;
    }

    if (!checkResult(solver, "timed-out.vnnlib", VerificationResult::TimedOut)) {
        return 1;
    }

    if (!checkResult(solver, "extra.vnnlib", VerificationResult::Sat)) {
        return 1;
    }

    if (!checkResult(solver, "stderr.vnnlib", VerificationResult::Sat)) {
        return 1;
    }

    if (!checkResult(solver, "nonzero.vnnlib", VerificationResult::Sat)) {
        return 1;
    }

    bool crashThrown = false;

    try {
        solver.verify("crash.vnnlib", {});
    } catch (const VNNLibException&) {
        crashThrown = true;
    }

    if (!crashThrown) {
        std::cerr << "Abnormal solver termination did not throw VNNLibException\n";
        return 1;
    }

    Solver missingSolver("definitely-not-a-real-vnnlib-solver");

    bool missingThrown = false;

    try {
        missingSolver.verify("query.vnnlib", {});
    } catch (const VNNLibException&) {
        missingThrown = true;
    }

    if (!missingThrown) {
        std::cerr << "Missing solver executable did not throw VNNLibException\n";
        return 1;
    }

    std::unordered_map<std::string, std::string> networks = {
        {"N", "model.onnx"}
    };

    if (solver.verify("arguments.vnnlib", networks, 60) !=
        VerificationResult::Sat) {
        std::cerr << "Verify arguments were not forwarded correctly\n";
        return 1;
    }

    bool malformedThrown = false;

    try {
        solver.verify("malformed.vnnlib", {});
    } catch (const VNNLibException&) {
        malformedThrown = true;
    }

    if (!malformedThrown) {
        std::cerr << "Malformed output did not throw VNNLibException\n";
        return 1;
    }

    std::cout << "Solver verify tests passed\n";
    return 0;
}
