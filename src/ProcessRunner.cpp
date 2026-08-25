#include "ProcessRunner.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>

using namespace vnnlib::solver;

vnnlib::solver::ProcessResult vnnlib::solver::runProcess(
    const std::string& executable, 
    const std::vector<std::string>& arguments
) {
    // Create the pipes for communication between the processes
    int stdout_pipe[2];
    int stderr_pipe[2];
    if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1) {
        throw std::runtime_error("Error creating pipe.");
    }

    // Initialise the result
    ProcessResult result;

    // Create a fork of the current process
    pid_t pid = fork();

    if (pid < 0) {
        throw std::runtime_error("Error forking process.");
    } else if (pid == 0) { // Child process
        // Close the read end of pipes
        if (close(stdout_pipe[0]) == -1 || close(stderr_pipe[0]) == -1) {
            throw std::runtime_error("Error closing pipe.");
        }

        // Set the stdout of the child process to the write end of the pipe
        if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1) {
            throw std::runtime_error("Error duplicating stdout file descriptor.");
        }

        // Set the stderr of the child process to the write end of the pipe
        if (dup2(stderr_pipe[1], STDERR_FILENO) == -1) {
            throw std::runtime_error("Error duplicating stderr file descriptor.");
        }

        // Convert arguments into array of char *
        std::vector<char *> args;
        args.push_back(const_cast<char *>(executable.c_str())); // Arguments are in the form {executable, arg1, arg2, ..., null}
        for (const auto& argument : arguments) args.push_back(const_cast<char *>(argument.c_str()));
        args.push_back(nullptr);

        execvp(args[0], args.data());
    } else { // Parent process
        // Close the write end of pipes
        if (close(stdout_pipe[1]) == -1 || close(stderr_pipe[1]) == -1) {
            throw std::runtime_error("Error closing pipe.");
        }

        // Read the solver's stdout
        std::string output;
        char buffer[100]; // Read 100 bytes at a time
        while (true) {
            ssize_t bytes = read(stdout_pipe[0], buffer, sizeof(buffer));
            if (bytes <= 0) break;
            output.append((char *) buffer, bytes);
        }
        close(stdout_pipe[0]);

        // Read the solver's stderr
        std::string error;
        while (true) {
            ssize_t bytes = read(stderr_pipe[0], buffer, sizeof(buffer));
            if (bytes <= 0) break;
            error.append((char *) buffer, bytes);
        }
        close(stderr_pipe[0]);

        // Check that the child process has successfully finished
        int status;
        if (wait(&status) == -1) {
            throw std::runtime_error("Error waiting for solver " + executable + " to finish.");
        }

        // Set the fields of the result object
        result.stdoutText = output;
        result.stderrText = error;

        // Check if the program exited normally
        if (WIFEXITED(status)) {
            result.exitedNormally = true;
        } else {
            result.exitedNormally = false;
        }
        result.exitCode = WEXITSTATUS(status);
    }

    return result;
}