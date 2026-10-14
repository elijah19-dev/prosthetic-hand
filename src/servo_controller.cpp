#include "servo_controller.h"

#include <Wire.h>

#include "config.h"

namespace
{
constexpr std::uint8_t NO_WIRE_STATUS = 0xFFU;
constexpr std::uint8_t WIRE_SUCCESS = 0U;
constexpr std::uint8_t WIRE_ADDRESS_NACK = 2U;
} // namespace

ServoController::ServoController()
    : communicationStatus_(ServoCommunicationStatus::NOT_STARTED),
      lastWireStatus_(NO_WIRE_STATUS)
{
}

ServoCommunicationStatus ServoController::beginCommunication()
{
    lastWireStatus_ = NO_WIRE_STATUS;

    if (!Wire.begin(SERVO_DRIVER_SDA_PIN,
                    SERVO_DRIVER_SCL_PIN,
                    SERVO_DRIVER_I2C_FREQUENCY_HZ))
    {
        communicationStatus_ = ServoCommunicationStatus::BUS_INITIALIZATION_FAILED;
        return communicationStatus_;
    }

    // Address-only transaction: no PCA9685 register or PWM data is written.
    Wire.beginTransmission(SERVO_DRIVER_I2C_ADDRESS);
    lastWireStatus_ = Wire.endTransmission();

    if (lastWireStatus_ == WIRE_SUCCESS)
    {
        communicationStatus_ = ServoCommunicationStatus::READY;
    }
    else if (lastWireStatus_ == WIRE_ADDRESS_NACK)
    {
        communicationStatus_ = ServoCommunicationStatus::DEVICE_NOT_FOUND;
    }
    else
    {
        communicationStatus_ = ServoCommunicationStatus::BUS_ERROR;
    }

    return communicationStatus_;
}

ServoCommunicationStatus ServoController::communicationStatus() const
{
    return communicationStatus_;
}

std::uint8_t ServoController::lastWireStatus() const
{
    return lastWireStatus_;
}
