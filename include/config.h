#pragma once

#include "command_classifier.h"
#include "emg_processing.h"
#include "safety_manager.h"
#include "servo_controller.h"

// ============================================================
// Prosthetic Hand Hardware Configuration
// ============================================================

// Serial communication
constexpr unsigned long SERIAL_BAUD = 115200;

// MyoWare 2.0 EMG Sensor
// GPIO 34 is an ADC1-capable input on the ESP32 DevKit V1.
constexpr int EMG_PIN = 34;
constexpr int EMG_ADC_BITS = 12;
constexpr int EMG_ADC_MAX = (1 << EMG_ADC_BITS) - 1;

// Preserve the acquisition prototype's 100 ms interval; final rate is TBD.
constexpr uint32_t EMG_SAMPLE_PERIOD_MS = 100;
constexpr uint32_t TELEMETRY_PERIOD_MS = 100;
// Documented MG90S control period, NOT calibrated mechanical endpoints.
constexpr uint32_t SERVO_PWM_PERIOD_US = 20000;

// TODO: Replace invalid placeholders with measured settings. EMG calibration
// uses raw ADC units; thresholds use normalized activation, timings use ms.
// Defaults intentionally cannot produce valid motion targets.
constexpr EMGCalibrationValues EMG_CALIBRATION = {0.0f, 0.0f};
constexpr EMGThresholds EMG_THRESHOLDS = {0.0f, 0.0f, 0, 0};
constexpr SafetyConfig SAFETY_CONFIG = {0, 0};
constexpr ServoMotionConfig SERVO_MOTION = {
    {0, 0}, // Wrist: relaxed/contracted pulse widths, measured on mechanism.
    {0, 0}, // Thumb: relaxed/contracted pulse widths, measured on mechanism.
    0,      // Full-travel time in ms, TBD.
    -1.0f   // Known initial pose in [0, 1], TBD; never assume a safe position.
};
