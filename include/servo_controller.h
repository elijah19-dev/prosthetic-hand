#pragma once

#include <cstdint>

enum class ServoCommunicationStatus : std::uint8_t
{
    NOT_STARTED,
    READY,
    BUS_INITIALIZATION_FAILED,
    DEVICE_NOT_FOUND,
    BUS_ERROR
};

class ServoController
{
public:
    ServoController();

    // Initializes I2C and performs an address-only probe. This method does not
    // configure PWM, write PCA9685 registers, or command servo motion.
    ServoCommunicationStatus beginCommunication();

    ServoCommunicationStatus communicationStatus() const;
    std::uint8_t lastWireStatus() const;

private:
    ServoCommunicationStatus communicationStatus_;
    std::uint8_t lastWireStatus_;
};
