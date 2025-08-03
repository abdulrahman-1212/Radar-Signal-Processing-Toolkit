# Autonomy Radar Toolkit

## Overview

The **Autonomy Radar Toolkit** is a terminal-based radar signal processing toolkit implemented in modern C++20. Designed for radar engineers and researchers, it provides a modular, dependency-free solution for generating, processing, and analyzing radar signals. The toolkit supports core radar functionalities such as signal generation, filtering, target detection, Doppler processing, and visualization, all accessible via an intuitive command-line interface (CLI).

## Features

- **Signal Generation**:
  - Linear Frequency Modulated (LFM) chirp signals with customizable parameters (center frequency, bandwidth, duration, sampling frequency).
  - Barker code waveforms (lengths 7 and 13) for phase-coded pulse compression.
  - Noise addition with configurable Signal-to-Noise Ratio (SNR).
  - Clutter simulation with configurable Clutter-to-Noise Ratio (CNR).

- **Signal Processing**:
  - Matched filtering for pulse compression.
  - Constant False Alarm Rate (CFAR) detection with adjustable guard and training cells.
  - Doppler processing to estimate target velocity using a basic Fast Fourier Transform (FFT) implementation.
  - Range-Doppler map generation for simultaneous range and velocity analysis.

- **Signal Analysis**:
  - Statistical summaries (signal power, peak amplitude).
  - ASCII-based waveform plotting for terminal visualization.
  - Text-based range-Doppler map visualization.

- **File I/O**:
  - Save and load signals to/from CSV files for persistence and reproducibility.

- **Batch Processing**:
  - Automated processing of multiple scenarios from a configuration file (SNR and CNR settings).

- **Unit Testing**:
  - Basic test suite to verify core functionalities (signal generation, noise addition, matched filtering).

## Installation

### Prerequisites
- C++20-compliant compiler (e.g., `g++` version 10 or later).
- No external dependencies are required.

### Compilation
1. Clone or download the source code to a local directory.
2. Navigate to the project directory containing the source files:
   - `signal_generator.h`, `signal_generator.cpp`
   - `signal_processor.h`, `signal_processor.cpp`
   - `signal_analyzer.h`, `signal_analyzer.cpp`
   - `file_io.h`, `file_io.cpp`
   - `test_suite.h`, `test_suite.cpp`
   - `radar_cli.h`, `radar_cli.cpp`
   - `main.cpp`
3. Compile the toolkit using:
   ```bash
   g++ -std=c++20 signal_generator.cpp signal_processor.cpp signal_analyzer.cpp file_io.cpp test_suite.cpp radar_cli.cpp main.cpp -o radar_toolkit
   ```

### Output
- The executable `radar_toolkit` will be generated in the current directory.

## Usage

### Interactive Mode
Run the toolkit in interactive mode to access the CLI menu:
```bash
./radar_toolkit
```

The CLI provides the following options:
1. **Generate Chirp Signal**: Create an LFM chirp signal.
2. **Generate Barker Code**: Create a Barker code waveform (length 7 or 13).
3. **Add Noise**: Add Gaussian noise with specified SNR.
4. **Add Clutter**: Add Rayleigh-distributed clutter with specified CNR.
5. **Apply Matched Filter**: Perform pulse compression.
6. **Perform CFAR Detection**: Detect targets with configurable parameters.
7. **Compute Doppler Spectrum**: Estimate target velocity.
8. **Generate Range-Doppler Map**: Visualize range and velocity.
9. **Display Signal Stats**: Show signal power and peak amplitude.
10. **Plot Waveform**: Display an ASCII waveform plot.
11. **Save Signal**: Save the current signal to a CSV file.
12. **Load Signal**: Load a signal from a CSV file.
13. **Set Parameters**: Adjust radar parameters (frequency, bandwidth, etc.).
14. **Run Batch Mode**: Process multiple scenarios from a config file.
15. **Run Tests**: Execute the unit test suite.
16. **Exit**: Quit the program.

### Batch Mode
Run the toolkit in batch mode to process multiple scenarios:
```bash
./radar_toolkit --batch config.txt
```

#### Configuration File Format
The configuration file (`config.txt`) should contain one line per scenario, with space-separated SNR and CNR values (in dB). Example:
```
10.0 5.0
20.0 10.0
```
Each scenario generates a chirp signal, adds noise and clutter, applies matched filtering, performs CFAR detection, and saves results to `batch_result_N.csv` (where `N` is the run index).

### Example Workflow
1. Start the toolkit: `./radar_toolkit`
2. Select option `13` to set parameters (e.g., center frequency = 1 GHz, bandwidth = 100 MHz).
3. Select option `1` to generate a chirp signal.
4. Select option `3` to add noise (e.g., SNR = 10 dB).
5. Select option `5` to apply matched filtering.
6. Select option `6` to perform CFAR detection.
7. Select option `11` to save the processed signal.
8. Select option `7` to compute the Doppler spectrum for a specified velocity.
9. Select option `8` to generate and view a range-Doppler map.

## Technical Details

### Architecture
The toolkit is organized into modular components within the `RadarToolkit` namespace:
- **SignalGenerator**: Handles LFM chirp and Barker code generation, noise, and clutter addition.
- **SignalProcessor**: Implements matched filtering, CFAR detection, Doppler processing, and range-Doppler map generation.
- **SignalAnalyzer**: Provides statistical analysis and terminal-based visualization.
- **FileIO**: Manages signal saving/loading in CSV format.
- **TestSuite**: Runs unit tests for core functionalities.
- **RadarCLI**: Provides the CLI interface and batch processing logic.

### Key Algorithms
- **Chirp Generation**: Generates LFM signals using a quadratic phase model.
- **Barker Code**: Supports phase-coded waveforms with lengths 7 and 13.
- **Matched Filter**: Performs convolution with the complex conjugate of the reference signal.
- **CFAR Detection**: Uses cell-averaging CFAR with configurable guard and training cells.
- **Doppler Processing**: Implements a basic Cooley-Tukey FFT for velocity estimation.
- **Range-Doppler Map**: Applies FFT across pulses for each range bin, displayed as a text grid.

### Design Choices
- **C++20**: Utilizes modern features like `constexpr`, `std::complex`, and range-based loops for performance and clarity.
- **Dependency-Free**: Includes a custom FFT implementation to avoid external libraries.
- **Terminal-Based**: Uses ASCII plots and text outputs for visualization, ensuring compatibility with any terminal.
- **Modular Structure**: Separates concerns into distinct classes for maintainability and extensibility.

### Limitations
- The FFT implementation is basic for simplicity; performance may be limited for large datasets.
- Range-Doppler maps are displayed as text grids due to terminal constraints.
- Batch mode assumes a fixed processing pipeline; custom pipelines require code modification.
- No support for real-time radar data input; signals are simulated or loaded from files.

## Future Improvements
- Optimize FFT implementation or integrate an external library (e.g., FFTW) if dependencies are allowed.
- Support additional waveform types (e.g., polyphase codes).
- Enhance visualization with more detailed ASCII plots or optional graphical output.
- Add advanced CFAR variants (e.g., Order-Statistic CFAR).
- Implement parallel processing for batch mode using `std::thread`.

## Contributing
Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a feature branch (`git checkout -b feature-name`).
3. Commit changes (`git commit -m "Add feature"`).
4. Push to the branch (`git push origin feature-name`).
5. Submit a pull request.

Please ensure code follows C++20 standards and includes unit tests for new features.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contact
For questions or feedback, contact the maintainers at [insert contact email or repository issue tracker].
