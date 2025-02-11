import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import json
from numba import njit
import asyncio

class RadarSignalProcessor:
    def __init__(self, fc=24e9, B=250e6, T=50e-6, fs=10e6):
        """
        Initialize the radar signal processor.
        :param fc: Carrier frequency (Hz)
        :param B: Bandwidth (Hz)
        :param T: Chirp duration (s)
        :param fs: Sampling frequency (Hz)
        """
        self.fc = fc
        self.B = B
        self.T = T
        self.fs = fs
        self.c = 3e8  # Speed of light (m/s)

    def generate_chirp(self):
        """
        Generate a linear frequency-modulated chirp signal.
        :return: Time vector and transmitted signal
        """
        t = np.linspace(0, self.T, int(self.fs * self.T), endpoint=False)
        tx_signal = np.exp(1j * 2 * np.pi * (self.fc * t + (self.B / (2 * self.T)) * t**2))
        return t, tx_signal

    def simulate_received_signal(self, tx_signal, t, R, v):
        """
        Simulate the received signal with delay and Doppler shift.
        :param tx_signal: Transmitted signal
        :param t: Time vector
        :param R: Target range (m)
        :param v: Target velocity (m/s)
        :return: Received signal
        """
        tau = 2 * R / self.c  # Time delay due to range
        f_doppler = 2 * v * self.fc / self.c  # Doppler frequency shift
        rx_signal = np.exp(1j * 2 * np.pi * (self.fc * (t - tau) + (self.B / (2 * self.T)) * (t - tau)**2 + f_doppler * t))
        return rx_signal

    def mix_signals(self, tx_signal, rx_signal):
        """
        Mix transmitted and received signals to produce the beat signal.
        :param tx_signal: Transmitted signal
        :param rx_signal: Received signal
        :return: Beat signal
        """
        return tx_signal * np.conj(rx_signal)

    def compute_fft(self, signal):
        """
        Compute the FFT of a signal.
        :param signal: Input signal
        :return: FFT result and frequency vector
        """
        fft_result = np.fft.fft(signal)
        frequencies = np.fft.fftfreq(len(signal), d=1/self.fs)
        return fft_result, frequencies

    def plot_fft(self, fft_result, frequencies):
        """
        Plot the FFT result.
        :param fft_result: FFT of the signal
        :param frequencies: Frequency vector
        """
        plt.figure()
        plt.plot(frequencies, np.abs(fft_result))
        plt.title("FFT of Beat Signal")
        plt.xlabel("Frequency (Hz)")
        plt.ylabel("Magnitude")
        plt.grid()
        plt.show()

    def add_noise(self, signal, snr_db):
        """
        Add Gaussian noise to the signal.
        :param signal: Input signal
        :param snr_db: Signal-to-Noise Ratio (dB)
        :return: Noisy signal
        """
        signal_power = np.mean(np.abs(signal)**2)
        noise_power = signal_power / (10 ** (snr_db / 10))
        noise = np.sqrt(noise_power / 2) * (np.random.randn(len(signal)) + 1j * np.random.randn(len(signal)))
        return signal + noise

    def add_clutter(self, signal, clutter_power):
        """
        Add clutter to the signal.
        :param signal: Input signal
        :param clutter_power: Power of the clutter
        :return: Signal with clutter
        """
        clutter = np.sqrt(clutter_power / 2) * (np.random.randn(len(signal)) + 1j * np.random.randn(len(signal)))
        return signal + clutter

    def cfar_detection(self, fft_result, guard_cells, training_cells, threshold_factor):
        """
        Perform CFAR detection on the FFT result.
        :param fft_result: FFT of the beat signal
        :param guard_cells: Number of guard cells
        :param training_cells: Number of training cells
        :param threshold_factor: Threshold multiplier
        :return: Detected targets
        """
        threshold = np.zeros_like(fft_result)
        for i in range(guard_cells + training_cells, len(fft_result) - guard_cells - training_cells):
            noise_level = np.mean(np.abs(fft_result[i - training_cells - guard_cells:i - guard_cells]) +
                          np.abs(fft_result[i + guard_cells:i + guard_cells + training_cells]))
            threshold[i] = threshold_factor * noise_level

        detected_targets = np.abs(fft_result) > threshold
        return detected_targets

    def process_multiple_targets(self, targets, snr_db=20, clutter_power=0.1):
        """
        Process multiple targets and generate a Range-Doppler Map.
        :param targets: List of (range, velocity) tuples
        :param snr_db: Signal-to-Noise Ratio (dB)
        :param clutter_power: Power of the clutter
        """
        t, tx_signal = self.generate_chirp()
        beat_signals = []

        for R, v in targets:
            rx_signal = self.simulate_received_signal(tx_signal, t, R, v)
            rx_signal = self.add_noise(rx_signal, snr_db)
            rx_signal = self.add_clutter(rx_signal, clutter_power)
            beat_signal = self.mix_signals(tx_signal, rx_signal)
            beat_signals.append(beat_signal)

        # Stack beat signals and compute 2D FFT
        beat_matrix = np.vstack(beat_signals)
        range_doppler_map = np.fft.fft2(beat_matrix)

        # Plot Range-Doppler Map
        self.plot_3d_range_doppler(range_doppler_map)

    def plot_3d_range_doppler(self, range_doppler_map):
        """
        Plot a 3D Range-Doppler Map.
        :param range_doppler_map: 2D FFT result
        """
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        X, Y = np.meshgrid(np.arange(range_doppler_map.shape[1]), np.arange(range_doppler_map.shape[0]))
        ax.plot_surface(X, Y, np.abs(range_doppler_map), cmap='jet')
        plt.title("3D Range-Doppler Map")
        plt.show()

    def save_config(self, filename):
        """
        Save radar configuration to a file.
        :param filename: Name of the file
        """
        config = {
            "fc": self.fc,
            "B": self.B,
            "T": self.T,
            "fs": self.fs
        }
        with open(filename, 'w') as f:
            json.dump(config, f)

    def load_config(self, filename):
        """
        Load radar configuration from a file.
        :param filename: Name of the file
        """
        with open(filename, 'r') as f:
            config = json.load(f)
        self.fc = config["fc"]
        self.B = config["B"]
        self.T = config["T"]
        self.fs = config["fs"]

    async def real_time_processing(self, data_stream):
        """
        Process radar data in real-time.
        :param data_stream: Generator yielding radar data chunks
        """
        for chunk in data_stream:
            fft_result, frequencies = self.compute_fft(chunk)
            self.plot_fft(fft_result, frequencies)
            await asyncio.sleep(0.1)  # Simulate processing delay


def main():
    # Initialize radar with default parameters
    radar = RadarSignalProcessor()

    while True:
        print("\n--- Radar Signal Processing App ---")
        print("1. Set Radar Parameters")
        print("2. Simulate Single Target")
        print("3. Simulate Multiple Targets")
        print("4. Save Configuration")
        print("5. Load Configuration")
        print("6. Real-Time Processing")
        print("7. Exit")
        choice = input("Enter your choice: ")

        if choice == "1":
            # Set radar parameters
            fc = float(input("Enter carrier frequency (Hz): "))
            B = float(input("Enter bandwidth (Hz): "))
            T = float(input("Enter chirp duration (s): "))
            fs = float(input("Enter sampling frequency (Hz): "))
            radar = RadarSignalProcessor(fc, B, T, fs)
            print("Radar parameters updated successfully!")

        elif choice == "2":
            # Simulate single target
            R = float(input("Enter target range (m): "))
            v = float(input("Enter target velocity (m/s): "))
            radar.process_target(R, v)

        elif choice == "3":
            # Simulate multiple targets
            targets = []
            while True:
                R = float(input("Enter target range (m) (or -1 to stop): "))
                if R == -1:
                    break
                v = float(input("Enter target velocity (m/s): "))
                targets.append((R, v))
            radar.process_multiple_targets(targets)

        elif choice == "4":
            # Save configuration
            filename = input("Enter filename to save configuration: ")
            radar.save_config(filename)
            print("Configuration saved successfully!")

        elif choice == "5":
            # Load configuration
            filename = input("Enter filename to load configuration: ")
            radar.load_config(filename)
            print("Configuration loaded successfully!")

        elif choice == "6":
            # Real-time processing
            async def data_stream():
                while True:
                    yield np.random.randn(1024)  # Simulate radar data
            asyncio.run(radar.real_time_processing(data_stream()))

        elif choice == "7":
            print("Exiting the app. Goodbye!")
            break

        else:
            print("Invalid choice. Please try again.")


if __name__ == "__main__":
    main()