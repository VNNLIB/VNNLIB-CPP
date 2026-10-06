#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "VNNLib.h"

namespace query = vnnlib::query;

namespace {
void testHiddenNodeTheory() {
    const std::string nh = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [1])
    (declare-output Y float32 [1])
)
(assert (<= X[0] 1.0))
)";
    auto parsed = query::parseQueryString(nh);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));
    
    const std::string h = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [1])
    (declare-hidden H float32 [1] "hidden")
    (declare-output Y float32 [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(h);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));
}

void testMultipleInputOutputTheory() {
    const std::string sio = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [1])
    (declare-output Y float32 [1])
)
(assert (<= X[0] 1.0))
)";
    auto parsed = query::parseQueryString(sio);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));

    const std::string mio = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X1 float32 [1])
    (declare-input X2 float32 [1])
    (declare-output Y float32 [1])
)
(assert (<= X1[0] 1.0))
)";
    parsed = query::parseQueryString(mio);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));
}

void testMultipleNetworksTheory() {
    const std::string snet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [1])
    (declare-output Y float32 [1])
)
(assert (<= X[0] 1.0))
)";
    auto parsed = query::parseQueryString(snet);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));

    const std::string mnet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [1])
    (declare-output Y float32 [1])
)
(declare-network g
    (declare-input U float32 [1])
    (declare-output W float32 [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(mnet);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));

    const std::string minet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input A float32 [1,10])
    (declare-output B float32 [1,2])
)
(declare-network g
    (isomorphic-to f)
    (declare-input C float32 [1,10])
    (declare-output D float32 [1,2])
)
(assert (<= A[0, 0] 1.0))
)";
    parsed = query::parseQueryString(minet);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MINET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));

    const std::string menet = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input A float32 [1,10])
    (declare-output B float32 [1,2])
)
(declare-network f_copy
    (equal-to f)
    (declare-input C float32 [1,10])
    (declare-output D float32 [1,2])
)
(assert (<= A[0, 0] 1.0))
)";
    parsed = query::parseQueryString(menet);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MENET, vnnlib::query::TMultipleNetworks::MINET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));
}

void testMultipleNodeComparisonsTheory() {
    const std::string snc = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [2])
    (declare-output Y float32 [1])
)
(declare-network g
    (declare-input A float32 [2])
    (declare-hidden H float32 [1] "hidden")
    (declare-output B float32 [1])
)
(assert (<= (+ X[0] X[1]) 0.1))
(assert (<= Y[0] 0.1))
(assert (<= H[0] 0.5))
(assert (== Y[0] A[0]))
)";
    auto parsed = query::parseQueryString(snc);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));

    const std::string mnc = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float32 [2])
    (declare-output Y float32 [1])
)
(declare-network g
    (declare-input A float32 [2])
    (declare-hidden H float32 [1] "hidden")
    (declare-output B float32 [1])
)
(assert (<= (+ X[0] Y[0]) 0.1))
(assert (<= H[0] A[1]))
(assert (== B[0] H[0]))
)";
    parsed = query::parseQueryString(mnc);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F32}));
}

namespace {
    const std::string arithmeticNet = R"(
(vnnlib-version <2.0>)
(declare-network test
    (declare-input X real [2])
    (declare-output Y real [2])
)
)";

    std::vector<vnnlib::query::TArithmeticComplexity> arithmetic(const std::string& assertions) {
        return query::parseQueryString(arithmeticNet + assertions)->arithmeticComplexityTheory();
    }

    using AC = vnnlib::query::TArithmeticComplexity;
}

void testArithmeticBoundsOnly() {
    assert((arithmetic("(assert (<= X[0] 1.0)) (assert (>= Y[0] 0.5))") == std::vector<AC>{AC::BND, AC::OUTC, AC::LIN, AC::POLY}));
}

void testArithmeticOutputComparison() {
    assert((arithmetic("(assert (<= X[0] 1.0)) (assert (>= Y[0] Y[1]))") == std::vector<AC>{AC::OUTC, AC::LIN, AC::POLY}));
}

void testArithmeticLinear() {
    assert((arithmetic("(assert (<= (+ (* 0.5 X[0]) (* 0.75 X[1])) 1.0)) (assert (>= (+ Y[0] Y[1]) 0.5))") == std::vector<AC>{AC::LIN, AC::POLY}));
}

void testArithmeticPolynomial() {
    assert((arithmetic("(assert (<= (* X[0] X[1]) 1.0)) (assert (>= (+ Y[0] Y[1]) 0.5))") == std::vector<AC>{AC::POLY}));
}

void testArithmeticHighestLevelWins() {
    assert((arithmetic("(assert (<= X[0] 1.0)) (assert (<= (* X[0] X[1]) 1.0))") == std::vector<AC>{AC::POLY}));
}

// A negated number is still a constant, so this is a bound
void testArithmeticNegatedConstantIsBound() {
    assert((arithmetic("(assert (>= X[0] (- 1.0)))") == std::vector<AC>{AC::BND, AC::OUTC, AC::LIN, AC::POLY}));
}

// A tiny coefficient must not hide a product of two variables
void testArithmeticTinyCoefficientProductIsPolynomial() {
    assert((arithmetic("(assert (<= (* (* 0.0000000001 X[0]) X[1]) 1.0))") == std::vector<AC>{AC::POLY}));
}

// Comparisons inside and/or count the same as top-level ones
void testArithmeticNestedComparison() {
    assert((arithmetic("(assert (and (<= X[0] 1.0) (>= (+ Y[0] Y[1]) 0.5)))") == std::vector<AC>{AC::LIN, AC::POLY}));
}

void testElementTypeTheories() {
    const std::string single = R"(
(vnnlib-version <2.0>)
(declare-network f
    (declare-input X float16 [2])
    (declare-output Y float16 [1])
)
(assert (<= X[0] 1.0))
)";
    auto parsed = query::parseQueryString(single);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F16}));

    const std::string mixed = R"(
(vnnlib-version <2.0>)
(declare-network a
    (declare-input X float16 [2])
    (declare-output Y float32 [1])
)
(assert (<= X[0] 1.0))
)";
    parsed = query::parseQueryString(mixed);
    assert((parsed->hiddenNodeTheory() == std::vector<vnnlib::query::THiddenNode>{vnnlib::query::THiddenNode::NH, vnnlib::query::THiddenNode::H}));
    assert((parsed->inputOutputTheory() == std::vector<vnnlib::query::TInputOutput>{vnnlib::query::TInputOutput::SIO, vnnlib::query::TInputOutput::MIO}));
    assert((parsed->multipleNetworksTheory() == std::vector<vnnlib::query::TMultipleNetworks>{vnnlib::query::TMultipleNetworks::SNET, vnnlib::query::TMultipleNetworks::MNET}));
    assert((parsed->multipleNodeComparisonsTheory() == std::vector<vnnlib::query::TMultipleNodeComparisons>{vnnlib::query::TMultipleNodeComparisons::SNC, vnnlib::query::TMultipleNodeComparisons::MNC}));
    assert((parsed->arithmeticComplexityTheory() == std::vector<vnnlib::query::TArithmeticComplexity>{vnnlib::query::TArithmeticComplexity::BND, vnnlib::query::TArithmeticComplexity::OUTC, vnnlib::query::TArithmeticComplexity::LIN, vnnlib::query::TArithmeticComplexity::POLY}));
    assert((parsed->elementTypeTheories() == std::vector<vnnlib::query::TDataType>{vnnlib::query::TDataType::F16, vnnlib::query::TDataType::F32}));
}
} // namespace

int main() {
    testHiddenNodeTheory();
    testMultipleInputOutputTheory();
    testMultipleNetworksTheory();
    testMultipleNodeComparisonsTheory();
    testArithmeticBoundsOnly();
    testArithmeticOutputComparison();
    testArithmeticLinear();
    testArithmeticPolynomial();
    testArithmeticHighestLevelWins();
    testArithmeticNegatedConstantIsBound();
    testArithmeticTinyCoefficientProductIsPolynomial();
    testArithmeticNestedComparison();
    testElementTypeTheories();
}