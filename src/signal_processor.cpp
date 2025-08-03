#include "signal_processor.h"
#include <cmath>
#include <algorithm>

namespace RadarToolkit {

constexpr double PI = 3.14159265358979323846;
constexpr double C = 3.0e8;

void SignalProcessor::fft(std::vector<std::complex<double>>& data, bool inverse) {
    size_t n = data.size();
    if (n <= 1) return;

    std::vector<std::complex<double>> even(n / 2), odd(n / 2);
    for (size_t i = 0; i < n / 2; ++i) {
        even[i] = data[i * 2];
        odd[i] = data[i * 2 + 1];
    }

    fft(even, inverse);
    fft(odd, inverse);

    double angle = 2 * PI / n * (inverse ? -1 : 1);
    std::complex<double> w(1), wn(std::cos(angle), std::sin(angle));
    for (size_t i = 0; i < n / 2; ++i) {
        std::complex<double> t = w * odd[i];
        data[i] = even[i] + t;
        data[i + n / 2] = even[i] - t;
        w *= wn;
    }
}

std::vector<std::complex<double>> SignalProcessor::matchedFilter(const std::vector<std::complex<double>>& signal,
                                                                const std::vector<std::complex<double>>& reference) {
    std::vector<std::complex<double>> output(signal.size());
    size_t ref_size = reference.size();

    for (size_t i = 0; i < signal.size(); ++i) {
        output[i] = 0.0;
        for (size_t j = 0; j < ref_size && i >= j; ++j) {
            output[i] += signal[i - j] * std::conj(reference[j]);
        }
    }
    return output;
}

std::vector<size_t> SignalProcessor::cfarDetection(const std::vector<std::complex<double>>& signal, size_t guard_cells,
                                                  size_t training_cells, double pfa) {
    std::vector<size_t> detections;
    double threshold_factor = std::pow(pfa, -1.0 / (training_cells)) - 1;

    for (size_t i = guard_cells + training_cells; i < signal.size() - guard_cells - training_cells; ++i) {
        double noise_power = 0.0;
        size_t count = 0;

        for (size_t j = i - guard_cells - training_cells; j < i - guard_cells; ++j) {
            noise_power += std::norm(signal[j]);
            count++;
        }
        for (size_t j = i + guard_cells + 1; j <= i + guard_cells + training_cells; ++j) {
            noise_power += std::norm(signal[j]);
            count++;
        }

        noise_power /= count;
        double threshold = noise_power * threshold_factor;
        if (std::norm(signal[i]) > threshold) {
            detections.push_back(i);
        }
    }
    return detections;
}

std::vector<std::complex<double>> SignalProcessor::computeDoppler(const std::vector<std::complex<double>>& signal,
                                                                 double f0, double fs, double velocity) {
    std::vector<std::complex<double>> spectrum = signal;
    fft(spectrum);
    double doppler_shift = 2 * velocity * f0 / C;
    std::vector<std::complex<double>> result(spectrum.size());
    for (size_t i = 0; i < spectrum.size(); ++i) {
        double freq = i * fs / spectrum.size();
        result[i] = (std::abs(freq - doppler_shift) < fs / spectrum.size()) ? spectrum[i] : 0.0;
    }
    return result;
}

std::vector<std::vector<double>> SignalProcessor::rangeDopplerMap(const std::vector<std::vector<std::complex<double>>>& pulses,
                                                                 double f0, double fs) {
    std::vector<std::vector<double>> map(pulses.size(), std::vector<double>(pulses[0].size()));
    for (size_t i = 0; i < pulses.size(); ++i) {
        std::vector<std::complex<double>> spectrum = pulses[i];
        fft(spectrum);
        for (size_t j = 0; j < spectrum.size(); ++j) {
            map[i][j] = std::abs(spectrum[j]);
        }
    }
    return map;
}

}