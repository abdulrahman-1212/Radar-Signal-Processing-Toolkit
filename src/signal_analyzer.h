#pragma once
#include <vector>
#include <complex>

namespace RadarToolkit {

class SignalAnalyzer {
public:
    static void displayStats(const std::vector<std::complex<double>>& signal);
    static void plotWaveform(const std::vector<std::complex<double>>& signal, size_t width = 80, size_t height = 20);
    static void displayRangeDopplerMap(const std::vector<std::vector<double>>& map);
};

}