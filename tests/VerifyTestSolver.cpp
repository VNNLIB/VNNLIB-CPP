#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 3 || std::string(argv[1]) != "verify") {
        return 1;
    }

    std::string query = argv[2];

    if (query == "sat.vnnlib") {
        std::cout << "sat\n";
        return 0;
    }

    if (query == "unsat.vnnlib") {
        std::cout << "unsat\n";
        return 0;
    }

    if (query == "unknown.vnnlib") {
        std::cout << "unknown\n";
        return 0;
    }

    if (query == "timed-out.vnnlib") {
        std::cout << "timed-out\n";
        return 0;
    }

    if (query == "extra.vnnlib") {
        std::cout << "sat\nextra output\n";
        return 0;
    }

    if (query == "malformed.vnnlib") {
        std::cout << "invalid\n";
        return 0;
    }

    if (query == "stderr.vnnlib") {
        std::cerr << "solver warning\n";
        std::cout << "sat\n";
        return 0;
    }

    if (query == "nonzero.vnnlib") {
        std::cout << "sat\n";
        return 7;
    }

    if (query == "arguments.vnnlib") {
        bool networkFound = false;
        bool timeoutFound = false;

        for (int i = 3; i < argc; ++i) {
            std::string argument = argv[i];

            if (argument == "--network" && i + 1 < argc &&
                std::string(argv[i + 1]) == "N=model.onnx") {
                networkFound = true;
            }

            if (argument == "--timeout" && i + 1 < argc &&
                std::string(argv[i + 1]) == "60") {
                timeoutFound = true;
            }
        }

        std::cout << (networkFound && timeoutFound ? "sat\n" : "invalid\n");
        return 0;
    }

    std::cout << "invalid\n";
    return 0;
}
