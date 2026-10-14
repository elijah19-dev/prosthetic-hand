# Hardware test plan

Tests below require the actual components and an agreed way to remove power.
Record parts, wiring, instruments, observations, and results for each run.

## Acquisition

- Verify GPIO 34 wiring and stable serial output.
- Record relaxed, moderate, and strong contraction raw ADC and millivolt values.
- Check clipping, repeatability, electrode repositioning, and motion artifacts.
- Compare battery-only and USB-connected development setups.

## Power and integration

- Measure buck output unloaded, with ESP32, with one servo, and with three.
- Observe 5 V rail at startup and during simultaneous motion; check resets.
- Observe MyoWare ENV while servos move and with carefully controlled load.
- Verify return routing keeps servo current away from the ADC reference path.

## PCA9685 communication-only gate

- Leave V+, the terminal block, and all servo headers unconnected.
- Power PCA9685 VCC from ESP32 3.3 V and connect the logic grounds.
- Connect SDA to GPIO 21 and SCL to GPIO 22.
- Hold OE HIGH at ESP32 3.3 V so PWM outputs remain disabled.
- Run `test/test_servo_communication` and require an ACK at expected address
  `0x40` with Wire status zero.
- Treat an ACK only as confirmation of I2C communication. It does not verify
  PWM output, servo power, channel mapping, or safe mechanical motion.
- Do not proceed to motion testing after any NACK, bus error, reset, unexpected
  heating, or unstable logic supply.

## Servos

- Select the driver and verify one-servo movement before three-servo operation.
- Determine each servo's mapping, travel limits, mechanical end stops, current,
  motion timing, and thermal behavior experimentally.
- Agree on startup, stop, and fault behavior before enabling hand motion.
