#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <exit_code>\n";
        return 1;
    }

    std::cout << "This is a test process";
    std::cerr << "This is some error text";

    for (int i = 2; i < argc; i++) {
        std::cout << argv[i] << ' ';
    }
    std::cout << '\n';

    int exitCode;
    try {
        exitCode = std::stoi(argv[1]);
    } catch (...) {
        return 1;
    }

    return exitCode;
}