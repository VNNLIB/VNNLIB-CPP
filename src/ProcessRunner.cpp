#include "ProcessRunner.h"
#include <stdexcept>
#include <thread>

using namespace vnnlib::solver;

#ifdef _WIN32
#include <windows.h>

    namespace {
        void readFromPipe(HANDLE source, std::string& out) {
            char buffer[100]; // Read 100 bytes at a time
            DWORD bytes;
            while (true) {
                BOOL success = ReadFile(source, buffer, sizeof(buffer), &bytes, NULL);
                if (!success || bytes <= 0) break;
                out.append((char *) buffer, bytes);
            }
            CloseHandle(source);
        }
    }

    vnnlib::solver::ProcessResult vnnlib::solver::runProcess(
        const std::string& executable, 
        const std::vector<std::string>& arguments
    ) {
        // Set security attributes for pipes so that handles are inherited by the child process
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        // Create the pipes for communication between the processes
        HANDLE stdout_read = NULL;
        HANDLE stdout_write = NULL;
        HANDLE stderr_read = NULL;
        HANDLE stderr_write = NULL;
        if (!CreatePipe(&stdout_read, &stdout_write, &sa, 0) || !CreatePipe(&stderr_read, &stderr_write, &sa, 0)) {
            throw std::runtime_error("Error creating pipe.");
        }

        // Ensure the read pipe for each stream is not inherited by the parent
        if (!SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0) || !SetHandleInformation(stderr_read, HANDLE_FLAG_INHERIT, 0)) {
            throw std::runtime_error("Error closing pipe.");
        }

        // Initialise the result
        vnnlib::solver::ProcessResult result;
        result.exitedNormally = false;

        // Create a string for the command line arguments
        std::string args = executable;
        for (const auto& argument : arguments) {
            args += " ";
            args += argument.c_str();
        }

        // Create the child process
        PROCESS_INFORMATION pi{};
        STARTUPINFO si{};
        si.cb = sizeof(STARTUPINFO);
        si.hStdError = stderr_write;
        si.hStdOutput = stdout_write;
        si.dwFlags |= STARTF_USESTDHANDLES;
        BOOL success = CreateProcess(
            NULL,
            args.data(),
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        );

        // Ensure the process was successfully executed
        int error = 0;
        if (!success) {
            error = 1;
        }

        // Close the write handles from the parent
        CloseHandle(stdout_write);
        CloseHandle(stderr_write);

        // Create a thread for each data stream to prevent deadlocks
        std::string stdoutText;
        std::string stderrText;
        std::thread t1(readFromPipe, stdout_read, std::ref(stdoutText));
        std::thread t2(readFromPipe, stderr_read, std::ref(stderrText));
        WaitForSingleObject(pi.hProcess, INFINITE);
        t1.join();
        t2.join();

        // Set the fields of the result object
        result.stdoutText = stdoutText;
        result.stderrText = stderrText;

        // Check if the program exited normally
        DWORD exitCode = 0;
        if (error == 0 && GetExitCodeProcess(pi.hProcess, &exitCode)) {
            result.exitedNormally = true;
            result.exitCode = exitCode;
        }

        // Close the process handles
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        return result;
    }
#else
    #include <unistd.h>
    #include <sys/wait.h>

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
        int error_pipe[2];
        if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1 || pipe(error_pipe) == -1) {
            throw std::runtime_error("Error creating pipe.");
        }

        // Initialise the result
        vnnlib::solver::ProcessResult result;
        result.exitedNormally = false;

        // Create a fork of the current process
        pid_t pid = fork();

        if (pid < 0) {
            throw std::runtime_error("Error forking process.");
        } else if (pid == 0) { // Child process
            // Close the read end of pipes
            if (close(stdout_pipe[0]) == -1 || close(stderr_pipe[0]) == -1 || close(error_pipe[0]) == -1) {
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

            // If the execvp call fails, write errno to the pipe for the parent to interpret
            write(error_pipe[1], &errno, sizeof(int));
            _exit(1);
        } else { // Parent process
            // Close the write end of pipes
            if (close(stdout_pipe[1]) == -1 || close(stderr_pipe[1]) == -1 || close(error_pipe[1]) == -1) {
                throw std::runtime_error("Error closing pipe.");
            }

            // Create a thread for each data stream to prevent deadlocks
            std::string stdoutText;
            std::string stderrText;
            std::thread t1(readFromPipe, stdout_pipe[0], std::ref(stdoutText));
            std::thread t2(readFromPipe, stderr_pipe[0], std::ref(stderrText));
            t1.join();
            t2.join();

            // Check that the child process has successfully finished
            int status;
            if (wait(&status) == -1) {
                throw std::runtime_error("Error waiting for solver " + executable + " to finish.");
            }

            // Read the error code from the error pipe
            int error = 0;
            read(error_pipe[0], &error, sizeof(int));

            // Set the fields of the result object
            result.stdoutText = stdoutText;
            result.stderrText = stderrText;

            // Check if the program exited normally
            if (error == 0 && WIFEXITED(status)) {
                result.exitedNormally = true;
                result.exitCode = WEXITSTATUS(status);
            }
        }

        return result;
    }
#endif