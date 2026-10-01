#include "VNNLib.h"

namespace vnnlib::query {

std::string hiddenNodeTheory(const TQuery& query) {
    for (const auto& network : query.networks) {
        // A declaration counts even when no assertion mentions the hidden node.
        if (network && !network->hidden.empty()) return "H";
    }
    return "NH";
}

std::string inputOutputTheory(const TQuery& query) {
    for (const auto& network : query.networks) {
        // Count declared nodes, not tensor elements or assertion references.
        if (network && (network->inputs.size() > 1 || network->outputs.size() > 1))
            return "MIO";
    }
    return "SIO";
}

std::string multipleNetworksTheory(const TQuery& query) {
    // If there is only one network, it is a single network
    if (query.networks.size() == 1) return "SNET";

    // Count the number of networks with equal-to or isomorphic-to declarations
    int equalCount = 0, isomorphicCount = 0;
    for (const auto& network : query.networks) {
        if (network->equalTo != "") equalCount++;
        if (network->isometricTo != "") isomorphicCount++;
    }

    // If there are multiple network declarations and all but one contains an equal-to, it is MENET
    if (equalCount == static_cast<int>(query.networks.size()) - 1) return "MENET";

    // If there are multiple network declarations and all but one contains an isomorphic-to, it is MINET
    if (isomorphicCount == static_cast<int>(query.networks.size()) - 1) return "MINET";

    // If the network does not match any of the other sets, it is MNET
    return "MNET";
}

std::string multipleNodeComparisonsTheory(const TQuery& query) {
    return "";
}

std::unique_ptr<TQuery> parseQueryFile(std::string path) {
    FILE *file = fopen(path.c_str(), "r");
    if (!file) {
        std::fprintf(stderr, "Error: Cannot open input file '%s': %s\n", path.c_str(), std::strerror(errno));
        return nullptr;
    }

    VNNLibQuery *parse_tree = nullptr;
    try {
        Query *query = pQuery(file);
        parse_tree = dynamic_cast<VNNLibQuery*>(query);
    } catch (const parse_error &e) {
        fclose(file);
        throw VNNLibException("Parse error: " + std::string(e.what()));
    }
    fclose(file);
    
    if (parse_tree == nullptr) {
        throw VNNLibException("Error: Failed to parse VNNLIB file: " + path);
    }

    TypedBuilder typeChecker;
    auto typed = typeChecker.build(parse_tree);
    if (typeChecker.getErrorCount() > 0) {
        throw VNNLibException(typeChecker.getErrorReport());
    } else if (typeChecker.getWarningCount() > 0) {
        std::cerr << "Warning(s) during type checking:\n" << typeChecker.getErrorReport() << std::endl;
    }
    return typed;
}

std::unique_ptr<TQuery> parseQueryString(std::string content) {
    VNNLibQuery *parse_tree = nullptr;
    try {
        Query *query = psQuery(content.c_str());
        parse_tree = dynamic_cast<VNNLibQuery*>(query);
    } catch (const parse_error &e) {
        throw VNNLibException("Parse error: " + std::string(e.what()));
    }
    
    if (parse_tree == nullptr) {
        throw VNNLibException("Error: Failed to parse VNNLIB file: " + content);
    }

    TypedBuilder typeChecker;
    auto typed = typeChecker.build(parse_tree);
    if (typeChecker.getErrorCount() > 0) {
        throw VNNLibException(typeChecker.getErrorReport());
    } else if (typeChecker.getWarningCount() > 0) {
        std::cerr << "Warning(s) during type checking:\n" << typeChecker.getErrorReport() << std::endl;
    }
    return typed;
}

std::string checkQueryFile(std::string path) {
    FILE *file = fopen(path.c_str(), "r");
    if (!file) {
        std::fprintf(stderr, "Error: Cannot open input file '%s': %s\n", path.c_str(), std::strerror(errno));
        return nullptr;
    }

    VNNLibQuery *parse_tree = nullptr;
    try {
        Query *query = pQuery(file);
        parse_tree = dynamic_cast<VNNLibQuery*>(query);
    } catch (const parse_error &e) {
        fclose(file);
        throw VNNLibException("Parse error: " + std::string(e.what()));
    }
    fclose(file);
    
    if (parse_tree == nullptr) {
        throw VNNLibException("Error: Failed to parse VNNLIB file: " + path);
    }

    TypedBuilder typeChecker;
    auto typed = typeChecker.build(parse_tree);
    if (typeChecker.getErrorCount() > 0) {
        return typeChecker.getErrorReport();
    }
    return "";
}

std::string checkQueryString(std::string content) {
    VNNLibQuery *parse_tree = nullptr;
    try {
        Query *query = psQuery(content.c_str());
        parse_tree = dynamic_cast<VNNLibQuery*>(query);
    } catch (const parse_error &e) {
        throw VNNLibException("Parse error: " + std::string(e.what()));
    }
    
    if (parse_tree == nullptr) {
        throw VNNLibException("Error: Failed to parse VNNLIB file: " + content);
    }

    TypedBuilder typeChecker;
    auto typed = typeChecker.build(parse_tree);
    if (typeChecker.getErrorCount() > 0) {
        return typeChecker.getErrorReport();
    }
    return "";
}

} // namespace vnnlib::query

std::unique_ptr<vnnlib::query::TQuery> parseQueryFile(std::string path) {
    return vnnlib::query::parseQueryFile(std::move(path));
}

std::unique_ptr<vnnlib::query::TQuery> parseQueryString(std::string content) {
    return vnnlib::query::parseQueryString(std::move(content));
}

std::string checkQueryFile(std::string path) {
    return vnnlib::query::checkQueryFile(std::move(path));
}

std::string checkQueryString(std::string content) {
    return vnnlib::query::checkQueryString(std::move(content));
}


