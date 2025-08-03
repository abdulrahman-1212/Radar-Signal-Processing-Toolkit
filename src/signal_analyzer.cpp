#include "signal_analyzer.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace RadarToolkit {

void SignalAnalyzer::displayStats(const std::vector<std::complex<double>>& signal) {
    double power = std::accumulate(signal.begin(), signal.end(), 0.0,
        [](double sum, const auto& s) { return sum + std::norm(s); }) / signal.size();
    double peak = std::abs(*std::max_element(signal.begin(), signal.end(),
        [](const auto& a, const auto& b) { return std::abs(a) < std::abs(b); }));
    std::cout << "Signal Power: " << power << " W\n";
    std::cout << "Peak Amplitude: " << peak << "\n";
}

void SignalAnalyzer::plotWaveform(const std::vector<std::complex<double>>& signal, size_t width, size_t height) {
    std::vector<double> magnitudes(signal.size());
    std::transform(signal.begin(), signal.end(), magnitudes.begin(),
        [](const auto& s) { return std::abs(s); });
    double max_mag = *std::max_element(magnitudes.begin(), magnitudes.end());
    if (max_mag == 0) max_mag = 1.0;

    std::vector<std::string> plot(height, std::string(width, ' '));
    for (size_t i = 0; i < signal.size(); ++i) {
        size_t col = i * width / signal.size();
        size_t row = height - 1 - static_cast<size_t>(magnitudes[i] / max_mag * (height - 1));
        if (row < height) plot[row][col] = '*';
    }

    std::cout << "Waveform Plot:\n";
    for (const auto& line : plot) {
        std::cout << line << "\n";
    }
}

void SignalAnalyzer::displayRangeDopplerMap(const std::vector<std::vector<double>>& map) {
    std::cout << "Range-Doppler Map (Intensity):\n";
    for (const auto& range_bin : map) {
        for (double val : range_bin) {
            std::cout << (val > 0.5 ? "*" : " ") << " ";
        }
        std::cout << "\n";
    }
}

}