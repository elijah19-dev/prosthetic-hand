#include "servo_controller.h"

#include <cassert>

#include "Wire.h"
#include "config.h"

TwoWire Wire;

namespace
{
void resetWire()
{
    Wire = TwoWire{};
}

void testSuccessfulAddressProbeUsesConfiguredBus()
{
    resetWire();
    Wire.endTransmissionResult = 0U;
    ServoController controller;

    assert(controller.communicationStatus() == ServoCommunicationStatus::NOT_STARTED);
    assert(controller.beginCommunication() == ServoCommunicationStatus::READY);
    assert(controller.communicationStatus() == ServoCommunicationStatus::READY);
    assert(controller.lastWireStatus() == 0U);
    assert(Wire.receivedSdaPin == SERVO_DRIVER_SDA_PIN);
    assert(Wire.receivedSclPin == SERVO_DRIVER_SCL_PIN);
    assert(Wire.receivedFrequencyHz == SERVO_DRIVER_I2C_FREQUENCY_HZ);
    assert(Wire.receivedAddress == SERVO_DRIVER_I2C_ADDRESS);
}

void testBusInitializationFailureIsReported()
{
    resetWire();
    Wire.beginResult = false;
    ServoController controller;

    assert(controller.beginCommunication() ==
           ServoCommunicationStatus::BUS_INITIALIZATION_FAILED);
    assert(controller.lastWireStatus() == 0xFFU);
}

void testAddressNackIsReportedAsDeviceNotFound()
{
    resetWire();
    Wire.endTransmissionResult = 2U;
    ServoController controller;

    assert(controller.beginCommunication() == ServoCommunicationStatus::DEVICE_NOT_FOUND);
    assert(controller.lastWireStatus() == 2U);
}

void testOtherWireFailuresAreReportedAsBusErrors()
{
    resetWire();
    Wire.endTransmissionResult = 4U;
    ServoController controller;

    assert(controller.beginCommunication() == ServoCommunicationStatus::BUS_ERROR);
    assert(controller.lastWireStatus() == 4U);
}
} // namespace

int main()
{
    testSuccessfulAddressProbeUsesConfiguredBus();
    testBusInitializationFailureIsReported();
    testAddressNackIsReportedAsDeviceNotFound();
    testOtherWireFailuresAreReportedAsBusErrors();
}
