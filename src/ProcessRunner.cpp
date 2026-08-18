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
        std::vector<char *> args;
        args.push_back(const_cast<char *>(executable.c_str())); // Arguments are in the form {executable, arg1, arg2, ...}
        for (const auto& argument : arguments) args.push_back(const_cast<char *>(argument.c_str()));
        args.push_back(nullptr);

        // Run the process (using an empty environment)
        execvp(args[0], args.data());
    } else { // Parent process
        // Check that the child process has successfully finished
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            // The waiting failed so exit as soon as possible (program is in an unknown state)
        }
    }

    vnnlib::solver::ProcessResult result = vnnlib::solver::ProcessResult();

    return result;
}