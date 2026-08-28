#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3 || std::string(argv[1]) != "supports") {
        return 1;
    }

    std::string capability = argv[2];

    if (capability == "--onnx-opset-versions") {
        std::cout << "13\n";
        return 0;
    }

    if (capability == "--onnx-element-types") {
        std::cerr << "solver warning\n";
        std::cout << "real\nfloat32\n";
        return 0;
    }

    if (capability == "--optimised-disjunctive-reasoning") {
        std::cout << "true\n";
        return 7;
    }

    return 1;
}
