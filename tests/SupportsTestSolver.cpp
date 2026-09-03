#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3 || std::string(argv[1]) != "supports") {
        return 1;
    }

    std::string capability = argv[2];

    if (capability == "--onnx-opset-versions") {
        std::cout << "13\n21\n";
    } else if (capability == "--onnx-element-types") {
        std::cout << "real\nfloat32\nfloat64\n";
    } else if (capability == "--onnx-operators") {
        std::cout << "Gemm float32 float64\nRelu\n";
    } else if (capability == "--vnnlib-versions") {
        std::cout << "2.0\n2.0\n";
    } else if (capability == "--hidden-node-theories") {
        std::cout << "NH\nH\n";
    } else if (capability == "--multiple-input-output-theories") {
        std::cout << "SIO\nMIO\n";
    } else if (capability == "--multiple-network-theories") {
        std::cout << "SNET\nMNET\nMENET\nMINET\n";
    } else if (capability == "--multiple-node-comparison-theories") {
        std::cout << "SNC\nMNC\n";
    } else if (capability == "--arithmetic-complexity-theories") {
        std::cout << "BND\nOUTC\nLIN\nPOLY\n";
    } else if (capability == "--optimised-disjunctive-reasoning") {
        std::cout << "true\n";
    } else if (capability == "--serialise-assignments") {
        std::cout << "false\n";
    } else {
        return 1;
    }

    return 0;
}
