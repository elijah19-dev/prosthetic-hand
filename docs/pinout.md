# Pinout

| Device | Signal | ESP32 connection | Status |
| --- | --- | --- | --- |
| MyoWare 2.0 | ENV | GPIO 34, ADC1 | Planned in `config.h`; verify wiring on hardware |
| MyoWare 2.0 | Power | ESP32 3.3 V rail | Planned; verify rail and grounding |
| PCA9685-based controller | SDA | TBD | Exact breakout and ESP32 pin TBD |
| PCA9685-based controller | SCL | TBD | Exact breakout and ESP32 pin TBD |
| PCA9685-based controller | Servo channels | TBD | Mechanical mapping TBD |
| Calibration button | Signal | TBD | Optional, not selected |

TODO: Update this table and `config.h` together after the physical connections
are verified. Do not assign controller pins before identifying the exact
PCA9685 breakout and reviewing its electrical requirements.
