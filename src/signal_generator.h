#pragma once
#include <vector>
#include <complex>

namespace RadarToolkit {

class SignalGenerator {
public:
    static std::vector<std::complex<double>> generateChirp(double f0, double bandwidth, double duration, double fs);
    static std::vector<std::complex<double>> generateBarkerCode(int code_length, double fs);
    static void addNoise(std::vector<std::complex<double>>& signal, double snr_db);
    static void addClutter(std::vector<std::complex<double>>& signal, double cnr_db);
};

}