#include "test_suite.h"
#include "signal_generator.h"
#include "signal_processor.h"
#include <iostream>
#include <cassert>

namespace RadarToolkit {

void TestSuite::runTests() {
    std::cout << "Running tests...\n";
    auto chirp = SignalGenerator::generateChirp(1e9, 100e6, 1e-6, 1e9);
    assert(chirp.size() == 1000 && "Chirp size mismatch");

    auto barker = SignalGenerator::generateBarkerCode(7, 1e9);
    assert(!barker.empty() && "Barker code generation failed");

    auto noisy = chirp;
    SignalGenerator::addNoise(noisy, 10.0);
    assert(noisy.size() == chirp.size() && "Noise addition failed");

    auto filtered = SignalProcessor::matchedFilter(chirp, chirp);
    assert(filtered.size() == chirp.size() && "Matched filter size mismatch");

    std::cout << "All tests passed!\n";
}

}