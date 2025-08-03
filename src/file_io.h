#pragma once
#include <vector>
#include <complex>
#include <string>

namespace RadarToolkit {

class FileIO {
public:
    static bool saveSignal(const std::vector<std::complex<double>>& signal, const std::string& filename);
    static std::vector<std::complex<double>> loadSignal(const std::string& filename);
};

}