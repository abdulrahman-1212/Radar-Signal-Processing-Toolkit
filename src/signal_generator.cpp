#include "signal_generator.h"
#include <random>
#include <cmath>
#include <algorithm>

namespace RadarToolkit {

constexpr double PI = 3.14159265358979323846;

std::vector<std::complex<double>> SignalGenerator::generateChirp(double f0, double bandwidth, double duration, double fs) {
    size_t num_samples = static_cast<size_t>(duration * fs);
    std::vector<std::complex<double>> chirp(num_samples);
    double t_step = 1.0 / fs;
    double k = bandwidth / duration;

    for (size_t i = 0; i < num_samples; ++i) {
        double t = i * t_step;
        double phase = 2 * PI * (f0 * t + 0.5 * k * t * t);
        chirp[i] = std::complex<double>(std::cos(phase), std::sin(phase));
    }
    return chirp;
}

std::vector<std::complex<double>> SignalGenerator::generateBarkerCode(int code_length, double fs) {
    std::vector<int> barker;
    if (code_length == 7) barker = {1, 1, 1, -1, -1, 1, -1};
    else if (code_length == 13) barker = {1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1};
    else return {};

    std::vector<std::complex<double>> signal;
    double chip_duration = 1.0 / fs;
    for (int phase : barker) {
        for (size_t i = 0; i < static_cast<size_t>(chip_duration * fs); ++i) {
            signal.emplace_back(phase, 0.0);
        }
    }
    return signal;
}

void SignalGenerator::addNoise(std::vector<std::complex<double>>& signal, double snr_db) {
    std::random_device rd;
    std::mt19937 gen(rd());
    double snr = std::pow(10.0, snr_db / 10.0);
    double signal_power = std::accumulate(signal.begin(), signal.end(), 0.0,
        [](double sum, const auto& s) { return sum + std::norm(s); }) / signal.size();
    double noise_power = signal_power / snr;
    double noise_std = std::sqrt(noise_power / 2.0);

    std::normal_distribution<double> dist(0.0, noise_std);
    for (auto& sample : signal) {
        sample += std::complex<double>(dist(gen), dist(gen));
    }
}

void SignalGenerator::addClutter(std::vector<std::complex<double>>& signal, double cnr_db) {
    std::random_device rd;
    std::mt19937 gen(rd());
    double cnr = std::pow(10.0, cnr_db / 10.0);
    double signal_power = std::accumulate(signal.begin(), signal.end(), 0.0,
        [](double sum, const auto& s) { return sum + std::norm(s); }) / signal.size();
    double clutter_power = signal_power * cnr;
    double clutter_std = std::sqrt(clutter_power / 2.0);

    std::normal_distribution<double> dist(0.0, clutter_std);
    for (auto& sample : signal) {
        sample += std::complex<double>(dist(gen), dist(gen));
    }
}

}