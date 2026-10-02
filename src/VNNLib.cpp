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

void getChildren(const TNode *node, std::vector<const TVarExpr *>& expressions) {
    // Retrieve the children of the current node
    std::vector<const vnnlib::query::TNode *> children;
    node->children(children);

    // Recursively go through each child of the current node
    for (const TNode *child : children) {
        if (auto c = dynamic_cast<const TVarExpr *>(child)) expressions.push_back(c); //std::cout << child->toString() << ' ' << c->symbol->name << ' ' << typeid(c->indices).name() << '\n';
        getChildren(child, expressions);
    }
}

std::string multipleNodeComparisonsTheory(const TQuery& query) {
    // Process each assertion in the query
    for (const auto& assertion : query.assertions) {
        // Create a vector to hold all the found expressions
        std::vector<const TVarExpr *> expressions;

        // Find all variables in the assertion
        getChildren(assertion.get(), expressions);

        // Check every pair of variables in the assertion, and if there are any two which are in the same network it is MNC
        for (size_t i = 0; i < expressions.size(); i++) {
            for (size_t j = i + 1; j < expressions.size(); j++) {
                if (expressions[i]->symbol->name != expressions[j]->symbol->name && expressions[i]->symbol->networkName == expressions[j]->symbol->networkName) return "MNC";
            }
        }
    }

    // If no assertion has multiple variables in the same network, it is SNC
    return "SNC";
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


