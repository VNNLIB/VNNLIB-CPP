#include "ProcessRunner.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <thread>

using namespace vnnlib::solver;

namespace {
    void readFromPipe(int source, std::string& out) {
        char buffer[100]; // Read 100 bytes at a time
        while (true) {
            ssize_t bytes = read(source, buffer, sizeof(buffer));
            if (bytes <= 0) break;
            out.append((char *) buffer, bytes);
        }
        close(source);
    }
}

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
    vnnlib::solver::ProcessResult result;
    result.exitedNormally = false;

    // Create a fork of the current process
    pid_t pid = fork();

    int error;
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
        error = errno;
    } else { // Parent process
        // Close the write end of pipes
        if (close(stdout_pipe[1]) == -1 || close(stderr_pipe[1]) == -1) {
            throw std::runtime_error("Error closing pipe.");
        }

        std::string stdoutText;
        std::string stderrText;

        // Create a thread for each data stream to prevent deadlocks
        std::thread t1(readFromPipe, stdout_pipe[0], std::ref(stdoutText));
        std::thread t2(readFromPipe, stderr_pipe[0], std::ref(stderrText));
        t1.join();
        t2.join();

        // Check that the child process has successfully finished
        int status;
        if (wait(&status) == -1) {
            throw std::runtime_error("Error waiting for solver " + executable + " to finish.");
        }

        // Set the fields of the result object
        result.stdoutText = stdoutText;
        result.stderrText = stderrText;

        // Check if the program exited normally
        if (error != 0 && WIFEXITED(status)) {
            result.exitedNormally = true;
            result.exitCode = WEXITSTATUS(status);
        }
    }

    return result;
}