#include "radar_cli.h"
#include "signal_generator.h"
#include "signal_processor.h"
#include "signal_analyzer.h"
#include "file_io.h"
#include "test_suite.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

namespace RadarToolkit {

void RadarCLI::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void RadarCLI::run() {
    while (true) {
        std::cout << "\n=== Autonomy Radar Toolkit ===\n";
        std::cout << "1. Generate Chirp Signal\n";
        std::cout << "2. Generate Barker Code\n";
        std::cout << "3. Add Noise\n";
        std::cout << "4. Add Clutter\n";
        std::cout << "5. Apply Matched Filter\n";
        std::cout << "6. Perform CFAR Detection\n";
        std::cout << "7. Compute Doppler Spectrum\n";
        std::cout << "8. Generate Range-Doppler Map\n";
        std::cout << "9. Display Signal Stats\n";
        std::cout << "10. Plot Waveform\n";
        std::cout << "11. Save Signal\n";
        std::cout << "12. Load Signal\n";
        std::cout << "13. Set Parameters\n";
        std::cout << "14. Run Batch Mode\n";
        std::cout << "15. Run Tests\n";
        std::cout << "16. Exit\n";
        std::cout << "Select an option: ";

        int choice;
        std::cin >> choice;
        clearInputBuffer();

        switch (choice) {
            case 1:
                signal = SignalGenerator::generateChirp(f0, bandwidth, duration, fs);
                reference = signal;
                pulse_train = std::vector<std::vector<std::complex<double>>>(num_pulses, signal);
                std::cout << "Chirp signal generated.\n";
                SignalAnalyzer::displayStats(signal);
                break;
            case 2:
                signal = SignalGenerator::generateBarkerCode(barker_length, fs);
                reference = signal;
                pulse_train = std::vector<std::vector<std::complex<double>>>(num_pulses, signal);
                std::cout << "Barker code generated.\n";
                SignalAnalyzer::displayStats(signal);
                break;
            case 3:
                if (signal.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                SignalGenerator::addNoise(signal, snr_db);
                for (auto& pulse : pulse_train) SignalGenerator::addNoise(pulse, snr_db);
                std::cout << "Noise added (SNR: " << snr_db << " dB).\n";
                SignalAnalyzer::displayStats(signal);
                break;
            case 4:
                if (signal.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                SignalGenerator::addClutter(signal, cnr_db);
                for (auto& pulse : pulse_train) SignalGenerator::addClutter(pulse, cnr_db);
                std::cout << "Clutter added (CNR: " << cnr_db << " dB).\n";
                SignalAnalyzer::displayStats(signal);
                break;
            case 5:
                if (signal.empty() || reference.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                signal = SignalProcessor::matchedFilter(signal, reference);
                for (auto& pulse : pulse_train) pulse = SignalProcessor::matchedFilter(pulse, reference);
                std::cout << "Matched filter applied.\n";
                SignalAnalyzer::displayStats(signal);
                break;
            case 6:
                if (signal.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                {
                    auto detections = SignalProcessor::cfarDetection(signal, guard_cells, training_cells, pfa);
                    std::cout << "CFAR detections at indices: ";
                    for (const auto& idx : detections) std::cout << idx << " ";
                    std::cout << "\n";
                }
                break;
            case 7:
                if (signal.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                {
                    double velocity;
                    std::cout << "Enter target velocity (m/s): ";
                    std::cin >> velocity;
                    clearInputBuffer();
                    auto doppler = SignalProcessor::computeDoppler(signal, f0, fs, velocity);
                    SignalAnalyzer::displayStats(doppler);
                }
                break;
            case 8:
                if (pulse_train.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                {
                    auto map = SignalProcessor::rangeDopplerMap(pulse_train, f0, fs);
                    SignalAnalyzer::displayRangeDopplerMap(map);
                }
                break;
            case 9:
                if (signal.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                SignalAnalyzer::displayStats(signal);
                break;
            case 10:
                if (signal.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                SignalAnalyzer::plotWaveform(signal);
                break;
            case 11:
                if (signal.empty()) {
                    std::cout << "Generate a signal first!\n";
                    break;
                }
                {
                    std::string filename;
                    std::cout << "Enter filename to save: ";
                    std::getline(std::cin, filename);
                    if (FileIO::saveSignal(signal, filename)) {
                        std::cout << "Signal saved to " << filename << "\n";
                    } else {
                        std::cout << "Failed to save signal.\n";
                    }
                }
                break;
            case 12:
                {
                    std::string filename;
                    std::cout << "Enter filename to load: ";
                    std::getline(std::cin, filename);
                    signal = FileIO::loadSignal(filename);
                    if (!signal.empty()) {
                        reference = signal;
                        pulse_train = std::vector<std::vector<std::complex<double>>>(num_pulses, signal);
                        std::cout << "Signal loaded from " << filename << "\n";
                    } else {
                        std::cout << "Failed to load signal.\n";
                    }
                }
                break;
            case 13:
                std::cout << "Enter center frequency (Hz): ";
                std::cin >> f0;
                std::cout << "Enter bandwidth (Hz): ";
                std::cin >> bandwidth;
                std::cout << "Enter pulse duration (s): ";
                std::cin >> duration;
                std::cout << "Enter sampling frequency (Hz): ";
                std::cin >> fs;
                std::cout << "Enter SNR (dB): ";
                std::cin >> snr_db;
                std::cout << "Enter CNR (dB): ";
                std::cin >> cnr_db;
                std::cout << "Enter guard cells: ";
                std::cin >> guard_cells;
                std::cout << "Enter training cells: ";
                std::cin >> training_cells;
                std::cout << "Enter probability of false alarm: ";
                std::cin >> pfa;
                std::cout << "Enter Barker code length (7 or 13): ";
                std::cin >> barker_length;
                std::cout << "Enter number of pulses for Doppler: ";
                std::cin >> num_pulses;
                clearInputBuffer();
                std::cout << "Parameters updated.\n";
                break;
            case 14:
                {
                    std::string config_file;
                    std::cout << "Enter config file name: ";
                    std::getline(std::cin, config_file);
                    runBatch(config_file);
                }
                break;
            case 15:
                TestSuite::runTests();
                break;
            case 16:
                std::cout << "Exiting...\n";
                return;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    }
}

void RadarCLI::runBatch(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        std::cout << "Failed to open config file: " << config_file << "\n";
        return;
    }

    std::string line;
    int run_count = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double batch_snr, batch_cnr;
        ss >> batch_snr >> batch_cnr;

        signal = SignalGenerator::generateChirp(f0, bandwidth, duration, fs);
        reference = signal;
        pulse_train = std::vector<std::vector<std::complex<double>>>(num_pulses, signal);

        SignalGenerator::addNoise(signal, batch_snr);
        for (auto& pulse : pulse_train) SignalGenerator::addNoise(pulse, batch_snr);

        SignalGenerator::addClutter(signal, batch_cnr);
        for (auto& pulse : pulse_train) SignalGenerator::addClutter(pulse, batch_cnr);

        signal = SignalProcessor::matchedFilter(signal, reference);
        for (auto& pulse : pulse_train) pulse = SignalProcessor::matchedFilter(pulse, reference);

        auto detections = SignalProcessor::cfarDetection(signal, guard_cells, training_cells, pfa);
        auto map = SignalProcessor::rangeDopplerMap(pulse_train, f0, fs);

        std::string output_file = "batch_result_" + std::to_string(run_count++) + ".csv";
        FileIO::saveSignal(signal, output_file);
        std::cout << "Batch run " << run_count << " completed. Results saved to " << output_file << "\n";
    }
}

}