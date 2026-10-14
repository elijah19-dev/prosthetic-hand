# PCA9685 communication test

This embedded test initializes the ESP32 I2C bus on GPIO 21/22 and performs an
address-only transaction to the expected PCA9685 address `0x40`. It writes no
PCA9685 registers and cannot command PWM or servo motion.

## Required wiring

```text
ESP32 GPIO 21 -> PCA9685 SDA
ESP32 GPIO 22 -> PCA9685 SCL
ESP32 3.3 V   -> PCA9685 VCC
ESP32 GND     -> PCA9685 GND
ESP32 3.3 V   -> PCA9685 OE
PCA9685 V+    -> DISCONNECTED
Terminal block-> UNPOWERED
Servo headers -> EMPTY
```

Verify every connection against the labels on the physical boards before
applying USB power. The communication gate passes only when the device ACKs at
`0x40` and the reported Wire status is zero.

Run with the connected ESP32 upload port:

```sh
pio test -e esp32doit-devkit-v1 -f test_servo_communication \
    --upload-port <ESP32_UPLOAD_PORT>
```

Do not proceed to PWM or servo-power testing if this test fails.
