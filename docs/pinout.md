# Pinout

| Device | Signal | ESP32 connection | Status |
| --- | --- | --- | --- |
| MyoWare 2.0 | ENV | GPIO 34, ADC1 | Planned in `config.h`; verify wiring on hardware |
| MyoWare 2.0 | Power | ESP32 3.3 V rail | Planned; verify rail and grounding |
| PCA9685-based controller | SDA | GPIO 21 | Selected in `config.h`; Arduino-ESP32 default SDA |
| PCA9685-based controller | SCL | GPIO 22 | Selected in `config.h`; Arduino-ESP32 default SCL |
| PCA9685-based controller | VCC | ESP32 3.3 V rail | Logic power only |
| PCA9685-based controller | GND | ESP32 GND | Required common logic reference |
| PCA9685-based controller | OE | ESP32 3.3 V rail for initial communication test | Hold HIGH to disable outputs; GPIO control remains TBD |
| PCA9685-based controller | V+ / terminal block | Not connected for initial communication test | Servo power rail; do not power during ACK-only test |
| PCA9685-based controller | Servo channels | TBD | Mechanical mapping TBD |
| Calibration button | Signal | TBD | Optional, not selected |

GPIO 21 and GPIO 22 are bidirectional, are not ESP32 boot-strapping pins, and
do not conflict with the GPIO 34 EMG input. Verify the physical header labels
and wiring before applying power.

TODO: Select an OE control GPIO only after startup and fault-output behavior is
defined. Servo channels and their mechanical limits remain TBD.
