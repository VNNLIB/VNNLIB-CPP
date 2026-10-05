#define VNNLIB_NO_DEPRECATED_QUERY_API
#include "TypedAST.h"  
#include "LinearArithExpr.h"
#include <set>

namespace vnnlib::query {

// ----------- Utility Functions ----------

std::string dtypeToString(TDataType dt) {
  switch (dt) {
    case TDataType::Real: return "Real";
    case TDataType::F16: return "F16";
    case TDataType::F32: return "F32";
    case TDataType::F64: return "F64";
    case TDataType::BF16: return "BF16";
    case TDataType::F8E4M3FN: return "F8E4M3FN";
    case TDataType::F8E5M2: return "F8E5M2";
    case TDataType::F8E4M3FNUZ: return "F8E4M3FNUZ";
    case TDataType::F8E5M2FNUZ: return "F8E5M2FNUZ";
    case TDataType::F4E2M1: return "F4E2M1";
    case TDataType::I8: return "I8";
    case TDataType::I16: return "I16";
    case TDataType::I32: return "I32";
    case TDataType::I64: return "I64";
    case TDataType::U8: return "U8";
    case TDataType::U16: return "U16";
    case TDataType::U32: return "U32";
    case TDataType::U64: return "U64";
    case TDataType::C64: return "C64";
    case TDataType::C128: return "C128";
    case TDataType::Bool: return "Bool";
    case TDataType::String: return "String";
    case TDataType::FloatConstant: return "FloatConstant";
    case TDataType::NegativeIntConstant: return "NegativeIntConstant";
    case TDataType::PositiveIntConstant: return "PositiveIntConstant";
    default: return "Unknown";
  }
}

bool isConstant(TDataType dt) {
    return dt == TDataType::FloatConstant || dt == TDataType::NegativeIntConstant || dt == TDataType::PositiveIntConstant;
}

// Returns true if the data type of the expression is in the same family as a constant data type
bool sameFamily(TDataType exprType, TDataType constType) {
    if (isConstant(constType)) {
        switch (exprType) {
            case TDataType::Real:
            case TDataType::F16:
            case TDataType::F32:
            case TDataType::F64:
            case TDataType::BF16:
            case TDataType::F8E4M3FN:
            case TDataType::F8E5M2:
            case TDataType::F8E4M3FNUZ:
            case TDataType::F8E5M2FNUZ:
            case TDataType::F4E2M1:
              return constType == TDataType::FloatConstant;
            case TDataType::I8:
            case TDataType::I16:
            case TDataType::I32:
            case TDataType::I64:
              return constType == TDataType::NegativeIntConstant || constType == TDataType::PositiveIntConstant;
            case TDataType::U8:
            case TDataType::U16:
            case TDataType::U32:
            case TDataType::U64:
              return constType == TDataType::PositiveIntConstant;
            case TDataType::FloatConstant:
              return constType == TDataType::FloatConstant;
            case TDataType::NegativeIntConstant:
            case TDataType::PositiveIntConstant:
              return constType == TDataType::NegativeIntConstant || constType == TDataType::PositiveIntConstant;
            default:
                return false;
        }
    }
    return false; // If constType is not a constant data type
}

bool sameType(TDataType a, TDataType b) {
    return a == b;
}

std::string shapeToString(const Shape& s) {
	if (s.empty()) return "[]";
	std::ostringstream oss;
	oss << '[';
	for (size_t i = 0; i < s.size(); ++i) {
	if (i) oss << ',';
	oss << s[i];
	}
	oss << ']';
	return oss.str();
}

template <class T>
std::string bnfcPrint(const T* p) {
	if (!p) return "<null>";
	PrintAbsyn pr;
	return pr.print(const_cast<T*>(p));
}


// ---------- TElementType ----------

void TElementType::children(std::vector<const TNode*>& out) const {
	(void)out; // leaf
}

std::string TElementType::toString() const {
    return bnfcPrint(src_ElementType);
}

// ---------- TArithExpr ----------

std::string TArithExpr::toString() const {
    return bnfcPrint(src_ArithExpr);
}

void TVarExpr::children(std::vector<const TNode*>& out) const {
	(void)out;
}

void TLiteral::children(std::vector<const TNode*>& out) const {
	(void)out;
}

void TNegate::children(std::vector<const TNode*>& out) const {
	if (expr) out.push_back(expr.get());
}

void TPlus::children(std::vector<const TNode*>& out) const {
	for (auto const& a : args) if (a) out.push_back(a.get());
}

void TMinus::children(std::vector<const TNode*>& out) const {
	if (head) out.push_back(head.get());
	for (auto const& r : rest) if (r) out.push_back(r.get());
}

void TMultiply::children(std::vector<const TNode*>& out) const {
	for (auto const& a : args) if (a) out.push_back(a.get());
}

// ---------- TBoolExpr ----------

std::string TBoolExpr::toString() const {
    return bnfcPrint(src_BoolExpr);
}

void TCompare::children(std::vector<const TNode*>& out) const {
	if (lhs) out.push_back(lhs.get());
	if (rhs) out.push_back(rhs.get());
}

void TConnective::children(std::vector<const TNode*>& out) const {
	for (auto const& a : args) if (a) out.push_back(a.get());
}

// --- Assertion ---

void TAssertion::children(std::vector<const TNode*>& out) const {
	if (cond) out.push_back(cond.get());
}

std::string TAssertion::toString() const {
    return bnfcPrint(src_Assertion);
}

// --- Definitions ---

void TInputDefinition::children(std::vector<const TNode*>& out) const {
	(void)out; 
}

std::string TInputDefinition::toString() const {
    return bnfcPrint(src_InputDefinition);
}


void THiddenDefinition::children(std::vector<const TNode*>& out) const {
	(void)out;
}

std::string THiddenDefinition::toString() const {
    return bnfcPrint(src_HiddenDefinition);
}

void TOutputDefinition::children(std::vector<const TNode*>& out) const {
	(void)out; 
}

std::string TOutputDefinition::toString() const {
    return bnfcPrint(src_OutputDefinition);
}

// --- Network ---

void TNetworkDefinition::children(std::vector<const TNode*>& out) const {
    for (auto const& n : inputs)  if (n) out.push_back(n.get());
    for (auto const& n : hidden)  if (n) out.push_back(n.get());
    for (auto const& n : outputs) if (n) out.push_back(n.get());
}

std::string TNetworkDefinition::toString() const {
    return bnfcPrint(src_NetworkDefinition);
}

// --- Version ---

void TVersion::children(std::vector<const TNode*>& out) const {
	(void)out;
}

std::string TVersion::toString() const {
    return bnfcPrint(src_Version);
}

// --- Query ---

namespace {
    void getVariables(const vnnlib::query::TNode *node, std::vector<const vnnlib::query::TVarExpr *>& variables) {
        // Retrieve the children of the current node
        std::vector<const vnnlib::query::TNode *> children;
        node->children(children);

        // Recursively go through each child of the current node
        for (const vnnlib::query::TNode *child : children) {
            if (auto c = dynamic_cast<const vnnlib::query::TVarExpr *>(child)) variables.push_back(c);
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

std::vector<THiddenNode> TQuery::hiddenNodeTheory() {
    for (const auto& network : networks) {
        // A declaration counts even when no assertion mentions the hidden node.
        if (network && !network->hidden.empty()) 
            return std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::H};
    }
    return std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H};
}

std::vector<TInputOutput> TQuery::inputOutputTheory() {
    for (const auto& network : networks) {
        // Count declared nodes, not tensor elements or assertion references.
        if (network && (network->inputs.size() > 1 || network->outputs.size() > 1))
            return std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::MIO};
    }
    return std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO};
}

std::vector<TMultipleNetworks> TQuery::multipleNetworksTheory() {
    // If there is only one network, it is a single network
    if (networks.size() == 1) 
        return std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET};

    // Count the number of networks with equal-to or isomorphic-to declarations
    int equalCount = 0, isomorphicCount = 0;
    for (const auto& network : networks) {
        if (!network->equalTo.empty()) equalCount++;
        if (!network->isometricTo.empty() || !network->equalTo.empty()) isomorphicCount++;
    }

    // If there are multiple network declarations and all but one contains an equal-to, it is MENET
    if (equalCount == static_cast<int>(networks.size()) - 1) 
        return std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MENET, vnnlib::query::TMultipleNetworks::MINET, vnnlib::query::TMultipleNetworks::MNET};

    // If there are multiple network declarations and all but one contains an isomorphic-to, it is MINET
    if (isomorphicCount == static_cast<int>(networks.size()) - 1) 
        return std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MINET, vnnlib::query::TMultipleNetworks::MNET};

    // If the network does not match any of the other sets, it is MNET
    return std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MNET};
}

std::vector<TMultipleNodeComparisons> TQuery::multipleNodeComparisonsTheory() {
    // Process each assertion in the query
    for (const auto& assertion : assertions) {
        // Create a vector to hold all the comparisons in the assertion
        std::vector<const vnnlib::query::TCompare *> comparisons;

        // Find all the comparisons in the assertion
        getComparisons(assertion.get(), comparisons);

        // Identify the variables in each comparison
        for (const vnnlib::query::TCompare *comparison : comparisons) {
            std::vector<const vnnlib::query::TVarExpr *> variables;

            // Find all variables in the comparison
            getVariables(comparison, variables);

            // Check every pair of variables in the assertion, and if there are any two which are in the same network it is MNC
            for (size_t i = 0; i < variables.size(); i++) {
                for (size_t j = i + 1; j < variables.size(); j++) {
                    if (variables[i]->symbol->name != variables[j]->symbol->name && variables[i]->symbol->networkName == variables[j]->symbol->networkName) 
                        return std::vector<TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::MNC};
                }
            }
        }
    }

    // If no assertion has multiple variables in the same network, it is SNC
    return std::vector<TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC};
}

TArithmeticComplexity TQuery::arithmeticComplexityTheory() {
    static const vnnlib::query::TArithmeticComplexity names[] = {vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY};
    int highest = 0;

    for (const auto& assertion : assertions) {
        std::vector<const vnnlib::query::TCompare *> comparisons;
        getComparisons(assertion.get(), comparisons);

        for (const vnnlib::query::TCompare *comparison : comparisons) {
            int level = comparisonLevel(comparison);
            if (level > highest) highest = level;
            // POLY is the top level, nothing can beat it
            if (highest == 3) return names[3];
        }
    }
    return names[highest];
}

std::vector<TDataType> TQuery::elementTypeTheories() {
    // A set so each element type appears once, in a fixed order
    std::set<vnnlib::query::TDataType> found;

    for (const auto& network : networks) {
        if (!network) continue;
        for (const auto& decl : network->inputs)  found.insert(decl->symbol->dtype);
        for (const auto& decl : network->hidden)  found.insert(decl->symbol->dtype);
        for (const auto& decl : network->outputs) found.insert(decl->symbol->dtype);
    }
    return std::vector<vnnlib::query::TDataType>(found.begin(), found.end());
}

void TQuery::children(std::vector<const TNode*>& out) const {
	for (auto const& n : networks)   if (n) out.push_back(n.get());
	for (auto const& a : assertions) if (a) out.push_back(a.get());
}

std::string TQuery::toString() const {
    return bnfcPrint(src_Query);
}

} // namespace vnnlib::query



