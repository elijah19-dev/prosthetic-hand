#pragma once

#include <cstdint>

class TwoWire
{
public:
    bool beginResult = true;
    std::uint8_t endTransmissionResult = 0U;
    int receivedSdaPin = -1;
    int receivedSclPin = -1;
    std::uint32_t receivedFrequencyHz = 0U;
    std::uint8_t receivedAddress = 0U;

    bool begin(int sdaPin, int sclPin, std::uint32_t frequencyHz)
    {
        receivedSdaPin = sdaPin;
        receivedSclPin = sclPin;
        receivedFrequencyHz = frequencyHz;
        return beginResult;
    }

    void beginTransmission(std::uint8_t address)
    {
        receivedAddress = address;
    }

    std::uint8_t endTransmission()
    {
        return endTransmissionResult;
    }
};

extern TwoWire Wire;
