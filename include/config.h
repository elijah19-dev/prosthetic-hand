#pragma once

#include <cstdint>

// ============================================================
// Prosthetic Hand Hardware Configuration
// ============================================================

// Serial communication
constexpr unsigned long SERIAL_BAUD = 115200;

// MyoWare 2.0 EMG Sensor
// GPIO 34 is an ADC1-capable input on the ESP32 DevKit V1.
constexpr int EMG_PIN = 34;

// PCA9685 I2C communication
// These are the default Arduino-ESP32 I2C pins and do not conflict with the
// EMG ADC input or ESP32 boot-strapping pins.
constexpr int SERVO_DRIVER_SDA_PIN = 21;
constexpr int SERVO_DRIVER_SCL_PIN = 22;

// Expected address with all PCA9685 A0-A5 address jumpers open. The embedded
// communication test must verify this address on the physical board.
constexpr std::uint8_t SERVO_DRIVER_I2C_ADDRESS = 0x40U;

// Conservative standard-mode bus rate for initial communication testing.
constexpr std::uint32_t SERVO_DRIVER_I2C_FREQUENCY_HZ = 100000U;
