#include "emg_sensor.h"

// Constructor
EMGSensor::EMGSensor(uint8_t pin)
    : pin_(pin)
{
}

// Configure the ESP32 ADC for the MyoWare sensor
void EMGSensor::begin()
{
    pinMode(pin_, INPUT);

    // ESP32 ADC uses 12-bit resolution:
    // 0 to 4095
    analogReadResolution(12);

    // Increase measurable voltage range for the ADC input.
    analogSetPinAttenuation(pin_, ADC_11db);
}

// Read raw ADC value (0-4095)
int EMGSensor::readRaw() const
{
    return analogRead(pin_);
}

// Read approximate input voltage in millivolts
uint32_t EMGSensor::readMilliVolts() const
{
    return analogReadMilliVolts(pin_);
}