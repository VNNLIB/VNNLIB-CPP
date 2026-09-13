#include "Error.hpp"
#include "ProcessRunner.h"
#include <iostream>

using namespace vnnlib::solver;

vnnlib::solver::ProcessResult res;

bool checkResult(const std::string& expected_stdout, const std::string& expected_stderr, std::optional<int> expected_exitcode, bool expected_exit) {
    if (res.stdoutText != expected_stdout) {
        std::cerr << "Unexpected result for stdout. Got " << res.stdoutText << " but expected " << expected_stdout << '\n';
        return false;
    }

    if (res.stderrText != expected_stderr) {
        std::cerr << "Unexpected result for stderr. Got " << res.stderrText << " but expected " << expected_stderr << '\n';
        return false;
    }

    if (res.exitCode != expected_exitcode) {
        std::cerr << "Unexpected result for exit code. Got " << res.exitCode.value_or(255) << " but expected " << expected_exitcode.value_or(255) << '\n';
        return false;
    }

    if (res.exitedNormally != expected_exit) {
        std::cerr << std::boolalpha << "Unexpected result for expected exit. Got " << res.exitedNormally << " but expected " << expected_exit << '\n';
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Expected process path\n";
        return 1;
    }

    std::string process = argv[1];

    res = vnnlib::solver::runProcess("NotAProcess", {});
    if (!checkResult("", "", std::nullopt, false)) return 1;

    res = vnnlib::solver::runProcess(process, {});
    if (!checkResult("", "Usage: " + process + " <exit_code>\n", 1, true)) return 1;

    res = vnnlib::solver::runProcess(process, {"invalidargument"});
    if (!checkResult("This is a test process\n", "This is some error text", 1, true)) return 1;

    res = vnnlib::solver::runProcess(process, {"2", "these", "are", "arguments"});
    if (!checkResult("This is a test processthese are arguments \n", "This is some error text", 2, true)) return 1;

    res = vnnlib::solver::runProcess(process, {"100", "these", "are", "arguments"});
    if (!checkResult("This is a test processthese are arguments \n", "This is some error text", 100, true)) return 1;
}