    #include "Solver.h"
    #include <vector>


    namespace {

//Building verify command arguments
    std::vector<std::string> buildVerifyArguments(
        const std::string& query,
        const std::unordered_map<std::string, std::string>& networks,
        std::optional<int> timeout
    ) 
    {
        std::vector<std::string> arguments = {"verify", query};

        for (const auto& [name, path] : networks) {
            arguments.push_back("--network");
            arguments.push_back(name + "=" + path);
        }

        if (timeout.has_value()) {
            arguments.push_back("--timeout");
            arguments.push_back(std::to_string(*timeout));
        }

        return arguments;

    }
    }


    namespace vnnlib::solver {

    Solver::Solver(const std::string& executable)
        : executable_(executable) {
    }
    }