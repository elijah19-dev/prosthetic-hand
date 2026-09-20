#pragma once

// ============================================================
// Prosthetic Hand Hardware Configuration
// ============================================================

// Serial communication
constexpr unsigned long SERIAL_BAUD = 115200;

// MyoWare 2.0 EMG Sensor
// GPIO 34 is an ADC1-capable input on the ESP32 DevKit V1.
constexpr int EMG_PIN = 34;