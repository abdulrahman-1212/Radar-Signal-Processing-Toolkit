#include "file_io.h"
#include <fstream>
#include <sstream>

namespace RadarToolkit {

bool FileIO::saveSignal(const std::vector<std::complex<double>>& signal, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (const auto& sample : signal) {
        file << sample.real() << "," << sample.imag() << "\n";
    }
    return true;
}

std::vector<std::complex<double>> FileIO::loadSignal(const std::string& filename) {
    std::vector<std::complex<double>> signal;
    std::ifstream file(filename);
    if (!file.is_open()) return signal;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double real, imag;
        char comma;
        ss >> real >> comma >> imag;
        signal.emplace_back(real, imag);
    }
    return signal;
}

}
