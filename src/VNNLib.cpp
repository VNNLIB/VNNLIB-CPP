#include "VNNLib.h"
#include "LinearArithExpr.h"
#include <set>

namespace {
    void getVariables(const vnnlib::query::TNode *node, std::vector<const vnnlib::query::TVarExpr *>& variables) {
        // Retrieve the children of the current node
        std::vector<const vnnlib::query::TNode *> children;
        node->children(children);

        // Recursively go through each child of the current node
        for (const vnnlib::query::TNode *child : children) {
            if (auto c = dynamic_cast<const vnnlib::query::TVarExpr *>(child)) variables.push_back(c); //std::cout << child->toString() << ' ' << c->symbol->name << ' ' << typeid(c->indices).name() << '\n';
            getVariables(child, variables);
        }
    }

    // Collect every comparison under a node, including ones nested inside and/or
    void getComparisons(const vnnlib::query::TNode *node, std::vector<const vnnlib::query::TCompare *>& out) {
        std::vector<const vnnlib::query::TNode *> children;
        node->children(children);
        for (const vnnlib::query::TNode *child : children) {
            if (auto c = dynamic_cast<const vnnlib::query::TCompare *>(child)) out.push_back(c);
            getComparisons(child, out);
        }
    }

    // True if the expression is just a variable, with nothing done to it
    bool isBareVariable(const vnnlib::query::TArithExpr *expr) {
        return dynamic_cast<const vnnlib::query::TVarExpr *>(expr) != nullptr;
    }

    // True if the expression is just a number
    bool isConstant(const vnnlib::query::TArithExpr *expr) {
        return dynamic_cast<const vnnlib::query::TLiteral *>(expr) != nullptr;
    }

    // True if the variable is a hidden or output node (not an input)
    bool isHiddenOrOutput(const vnnlib::query::TArithExpr *expr) {
        auto var = dynamic_cast<const vnnlib::query::TVarExpr *>(expr);
        return var && (var->symbol->kind == vnnlib::query::SymbolKind::Hidden
                    || var->symbol->kind == vnnlib::query::SymbolKind::Output);
    }

    // Level of one comparison: 0 = BND, 1 = OUTC, 2 = LIN, 3 = POLY
    int comparisonLevel(const vnnlib::query::TCompare *cmp) {
        const auto *lhs = cmp->lhs.get();
        const auto *rhs = cmp->rhs.get();

        // Variable against a number, either way round
        if ((isBareVariable(lhs) && isConstant(rhs)) || (isConstant(lhs) && isBareVariable(rhs))) return 0;

        // Two plain hidden or output variables compared with each other
        if (isBareVariable(lhs) && isBareVariable(rhs) && isHiddenOrOutput(lhs) && isHiddenOrOutput(rhs)) return 1;

        // Anything else is linear if both sides linearize, otherwise polynomial
        try {
            vnnlib::query::linearize(lhs);
            vnnlib::query::linearize(rhs);
            return 2;
        } catch (const vnnlib::query::VNNLibException&) {
            return 3;
        }
    }
}

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
    // Process each assertion in the query
    for (const auto& assertion : query.assertions) {
        // Create a vector to hold all the found variables
        std::vector<const vnnlib::query::TVarExpr *> variables;

        // Find all variables in the assertion
        getVariables(assertion.get(), variables);

        // Check every pair of variables in the assertion, and if there are any two which are in the same network it is MNC
        for (size_t i = 0; i < variables.size(); i++) {
            for (size_t j = i + 1; j < variables.size(); j++) {
                if (variables[i]->symbol->name != variables[j]->symbol->name && variables[i]->symbol->networkName == variables[j]->symbol->networkName) return "MNC";
            }
        }
    }

    // If no assertion has multiple variables in the same network, it is SNC
    return "SNC";
}

std::string arithmeticComplexityTheory(const TQuery& query) {
    static const char* names[] = {"BND", "OUTC", "LIN", "POLY"};
    int highest = 0;

    for (const auto& assertion : query.assertions) {
        std::vector<const TCompare *> comparisons;
        getComparisons(assertion.get(), comparisons);

        for (const TCompare *cmp : comparisons) {
            int level = comparisonLevel(cmp);
            if (level > highest) highest = level;
            // POLY is the top level, nothing can beat it
            if (highest == 3) return names[3];
        }
    }
    return names[highest];
}

std::vector<std::string> elementTypeTheories(const TQuery& query) {
    // A set so each element type appears once, in a fixed order
    std::set<std::string> found;

    for (const auto& network : query.networks) {
        if (!network) continue;
        for (const auto& decl : network->inputs)  found.insert(dtypeToString(decl->symbol->dtype));
        for (const auto& decl : network->hidden)  found.insert(dtypeToString(decl->symbol->dtype));
        for (const auto& decl : network->outputs) found.insert(dtypeToString(decl->symbol->dtype));
    }
    return std::vector<std::string>(found.begin(), found.end());
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


