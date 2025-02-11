from main import RadarSignalProcessor

import numpy as np
import asyncio


def test_initialization_default():
    radar = RadarSignalProcessor()
    assert radar.fc == 24e9  # Default carrier frequency
    assert radar.B == 250e6  # Default bandwidth
    assert radar.T == 50e-6  # Default chirp duration
    assert radar.fs == 10e6  # Default sampling frequency

def test_initialization_custom():
    radar = RadarSignalProcessor(fc=20e9, B=100e6, T=30e-6, fs=5e6)
    assert radar.fc == 20e9
    assert radar.B == 100e6
    assert radar.T == 30e-6
    assert radar.fs == 5e6


def test_generate_chirp():
    radar = RadarSignalProcessor()
    t, tx_signal = radar.generate_chirp()
    assert len(t) == int(radar.fs * radar.T)  # Ensure time vector length matches sampling rate and duration
    assert np.iscomplexobj(tx_signal)  # Check if the signal is a complex-valued signal
    assert np.allclose(np.abs(tx_signal), np.ones_like(tx_signal))  # Check if the magnitude of the chirp signal is 1


def test_simulate_received_signal():
    radar = RadarSignalProcessor()
    t, tx_signal = radar.generate_chirp()
    R = 1000  # Target range (m)
    v = 50  # Target velocity (m/s)
    rx_signal = radar.simulate_received_signal(tx_signal, t, R, v)
    assert len(rx_signal) == len(tx_signal)  # Ensure the received signal has the same length
    assert np.iscomplexobj(rx_signal)  # Check if the received signal is complex-valued


def test_mix_signals():
    radar = RadarSignalProcessor()
    t, tx_signal = radar.generate_chirp()
    R = 1000  # Target range (m)
    v = 50  # Target velocity (m/s)
    rx_signal = radar.simulate_received_signal(tx_signal, t, R, v)
    beat_signal = radar.mix_signals(tx_signal, rx_signal)
    assert len(beat_signal) == len(tx_signal)  # Beat signal should have the same length as the input signal
    assert np.iscomplexobj(beat_signal)  # Beat signal should be complex


def test_compute_fft():
    radar = RadarSignalProcessor()
    t, tx_signal = radar.generate_chirp()
    fft_result, frequencies = radar.compute_fft(tx_signal)
    assert len(fft_result) == len(tx_signal)  # FFT result should have the same length as the input signal
    assert len(frequencies) == len(tx_signal)  # Frequency vector length should match the signal length


def test_add_noise():
    radar = RadarSignalProcessor()
    t, tx_signal = radar.generate_chirp()
    snr_db = 20  # Signal-to-Noise Ratio in dB
    noisy_signal = radar.add_noise(tx_signal, snr_db)
    assert len(noisy_signal) == len(tx_signal)  # Noisy signal should have the same length
    assert not np.allclose(tx_signal, noisy_signal)  # The noisy signal should be different from the original signal


def test_add_clutter():
    radar = RadarSignalProcessor()
    t, tx_signal = radar.generate_chirp()
    clutter_power = 0.1  # Clutter power
    signal_with_clutter = radar.add_clutter(tx_signal, clutter_power)
    assert len(signal_with_clutter) == len(tx_signal)  # Cluttered signal should have the same length
    assert not np.allclose(tx_signal, signal_with_clutter)  # Cluttered signal should be different from the original signal



def test_cfar_detection():
    radar = RadarSignalProcessor()
    t, tx_signal = radar.generate_chirp()
    R = 1000  # Target range (m)
    v = 50  # Target velocity (m/s)
    rx_signal = radar.simulate_received_signal(tx_signal, t, R, v)
    rx_signal = radar.add_noise(rx_signal, snr_db=20)
    rx_signal = radar.add_clutter(rx_signal, clutter_power=0.1)
    beat_signal = radar.mix_signals(tx_signal, rx_signal)

    fft_result, frequencies = radar.compute_fft(beat_signal)
    detected_targets = radar.cfar_detection(fft_result, guard_cells=2, training_cells=5, threshold_factor=3)

    assert detected_targets.any()  # There should be some detected targets


def test_process_multiple_targets():
    radar = RadarSignalProcessor()
    targets = [(1000, 50), (1500, 30), (2000, 10)]  # List of (range, velocity) tuples
    radar.process_multiple_targets(targets)

    # Check that the range-doppler map is generated, which implies the function is working
    # There won't be direct output verification but visual check of Range-Doppler Map



def test_save_load_config():
    radar = RadarSignalProcessor(fc=20e9, B=100e6, T=30e-6, fs=5e6)
    filename = "test_config.json"
    radar.save_config(filename)

    radar_new = RadarSignalProcessor()
    radar_new.load_config(filename)

    assert radar_new.fc == radar.fc  # Ensure loaded configuration matches the saved one
    assert radar_new.B == radar.B
    assert radar_new.T == radar.T
    assert radar_new.fs == radar.fs



def test_real_time_processing():
    radar = RadarSignalProcessor()

    async def data_stream():
        for _ in range(5):  # Generate 5 chunks of random data
            yield np.random.randn(1024)

    async def run_real_time():
        await radar.real_time_processing(data_stream())

    asyncio.run(run_real_time())  # Check if no errors occur and the function completes
