#pragma once

#include <Arduino.h>

class EMGSensor
{
public:
    // Create an EMG sensor connected to the specified ESP32 ADC pin.
    explicit EMGSensor(uint8_t pin);

    // Configure the ESP32 ADC for the sensor.
    void begin();

    // Return the raw ESP32 ADC reading.
    int readRaw() const;

    // Return the measured signal in millivolts.
    uint32_t readMilliVolts() const;

private:
    uint8_t pin_;
};