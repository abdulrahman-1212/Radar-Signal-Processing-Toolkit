#pragma once
#include <vector>
#include <complex>
#include <string>

namespace RadarToolkit {

class RadarCLI {
public:
    void run();
    void runBatch(const std::string& config_file);

private:
    double f0 = 1e9;
    double bandwidth = 100e6;
    double duration = 1e-6;
    double fs = 1e9;
    double snr_db = 10.0;
    double cnr_db = 10.0;
    size_t guard_cells = 5;
    size_t training_cells = 10;
    double pfa = 1e-6;
    int barker_length = 7;
    size_t num_pulses = 10;
    std::vector<std::complex<double>> signal;
    std::vector<std::complex<double>> reference;
    std::vector<std::vector<std::complex<double>>> pulse_train;

    void clearInputBuffer();
};

}