#include "ProcessRunner.h"

#include <unistd.h>
#include <sys/wait.h>

vnnlib::solver::ProcessResult runProcess(
    const std::string& executable, 
    const std::vector<std::string>& arguments
) {
    // Create a fork of the current process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed - abort
    } else if (pid == 0) { // Child process
        // Convert arguments into array of char *
        std::vector<const char *> args;
        for (const auto& argument : arguments) args.push_back(argument.data());
        args.push_back(nullptr);

        // Run the process (using an empty environment)
        execve(executable.c_str(), args.data(), nullptr);
    } else {
        // Parent process - wait for solver to execute
    }
}