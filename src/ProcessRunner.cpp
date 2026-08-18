#include "ProcessRunner.h"
#include <unistd.h>
#include <sys/wait.h>

using namespace vnnlib::solver;

vnnlib::solver::ProcessResult runProcess(
    const std::string& executable, 
    const std::vector<std::string>& arguments
) {
    // Create the pipes for communication between the processes
    int stdout_pipe[2];
    if (pipe(stdout_pipe) == -1) {
        // Pipe creation failed
    }

    // Create a fork of the current process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed - abort
    } else if (pid == 0) { // Child process
        // Close the read end of pipe
        if (close(stdout_pipe[0]) == -1) {
            // Close failed
        }

        // Set the stdout of the child process to the write end of the pipe
        if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1) {
            // Duplication failed
        }

        // Convert arguments into array of char *
        std::vector<char *> args;
        args.push_back(const_cast<char *>(executable.c_str())); // Arguments are in the form {executable, arg1, arg2, ...}
        for (const auto& argument : arguments) args.push_back(const_cast<char *>(argument.c_str()));
        args.push_back(nullptr);

        // Run the process (using an empty environment)
        execvp(args[0], args.data());
    } else { // Parent process
        // Close the write end of pipe
        if (close(stdout_pipe[1]) == -1) {
            // Close failed
        }

        // Check that the child process has successfully finished
        int status;
        if (wait(&status) == -1) {
            // The waiting failed so exit as soon as possible (program is in an unknown state)
        }

        // Initialise the result
        vnnlib::solver::ProcessResult result;

        result.stdoutText = stdout_pipe[0]; // still an FD so need to read from this
        result.stderrText = nullptr;

        // Check if the program exited normally
        if (WIFEXITED(status)) {
            result.exitedNormally = true;
        } else {
            result.exitedNormally = false;
        }
        result.exitCode = WEXITSTATUS(status);

        return result;
    }
}