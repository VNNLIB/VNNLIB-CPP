#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>

#include "VNNLib.h"

namespace {

constexpr const char* queryText = R"(
(vnnlib-version <2.0>)
(declare-network test
    (declare-input X real [1])
    (declare-output Y real [1])
)
(assert (<= X[0] 1.0))
)";

void testFileParsing(const std::string& path) {
    auto oldQuery = ::parseQueryFile(path);
    auto newQuery = vnnlib::query::parseQueryFile(path);
    auto stringQuery = vnnlib::query::parseQueryString(queryText);
    assert(oldQuery);
    assert(newQuery);
    assert(stringQuery);
    assert(newQuery->networks.size() == 1);
    assert(newQuery->assertions.size() == 1);
    assert(oldQuery->toString() == newQuery->toString());
    assert(newQuery->toString() == stringQuery->toString());
}

void testFileChecking(const std::string& path) {
    const auto oldResult = ::checkQueryFile(path);
    const auto newResult = vnnlib::query::checkQueryFile(path);
    assert(oldResult == newResult);
    assert(newResult.empty());
}

} // namespace

int main(int argc, char* argv[]) {
    assert(argc == 2);
    const std::string path = argv[1];
    {
        std::ofstream file(path);
        assert(file);
        file << queryText;
        file.close();
        assert(file);
    }
    testFileParsing(path);
    testFileChecking(path);
    const int removed = std::remove(path.c_str());
    assert(removed == 0);
}
