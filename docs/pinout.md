# Pinout

| Device | Signal | ESP32 connection | Status |
| --- | --- | --- | --- |
| MyoWare 2.0 | ENV | GPIO 34, ADC1 | Planned in `config.h`; verify wiring on hardware |
| MyoWare 2.0 | Power | ESP32 3.3 V rail | Planned; verify rail and grounding |
| Servo driver | Control signals | TBD | Driver board and pins TBD |
| Servo driver | Servo channels | TBD | Mapping TBD |
| Calibration button | Signal | TBD | Optional, not selected |

TODO: Update this table and `config.h` together after the physical connections
are verified. Do not assign servo-driver pins before selecting the board.
