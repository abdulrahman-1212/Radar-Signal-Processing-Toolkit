#pragma once
#include <vector>
#include <complex>

namespace RadarToolkit {

class SignalProcessor {
public:
    static std::vector<std::complex<double>> matchedFilter(const std::vector<std::complex<double>>& signal,
                                                          const std::vector<std::complex<double>>& reference);
    static std::vector<size_t> cfarDetection(const std::vector<std::complex<double>>& signal, size_t guard_cells,
                                            size_t training_cells, double pfa);
    static std::vector<std::complex<double>> computeDoppler(const std::vector<std::complex<double>>& signal,
                                                           double f0, double fs, double velocity);
    static std::vector<std::vector<double>> rangeDopplerMap(const std::vector<std::vector<std::complex<double>>>& pulses,
                                                           double f0, double fs);
private:
    static void fft(std::vector<std::complex<double>>& data, bool inverse = false);
};

}