#include <Arduino.h>
#include <unity.h>

#include "servo_controller.h"

namespace
{
void testPca9685AcknowledgesExpectedAddress()
{
    ServoController controller;
    const ServoCommunicationStatus status = controller.beginCommunication();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(
        0U,
        controller.lastWireStatus(),
        "PCA9685 did not ACK at 0x40 (Wire 2=address NACK; other values=bus error)");
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(ServoCommunicationStatus::READY),
                            static_cast<std::uint8_t>(status));
}
} // namespace

void setup()
{
    delay(2000U);
    UNITY_BEGIN();
    RUN_TEST(testPca9685AcknowledgesExpectedAddress);
    UNITY_END();
}

void loop()
{
}
