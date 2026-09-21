# System architecture

Status: acquisition prototype; the remaining firmware layers are scaffolds.

## Electrical and power flow

12 V battery → approximately 5 V buck → ESP32 VIN/5V and servo power rail.
The ESP32 3.3 V rail supplies the MyoWare 2.0. MyoWare ENV connects to the
planned ESP32 ADC1 GPIO 34 input. Servo power returns and signal/logic returns
need separate physical routing with a common electrical reference.

TODO: Verify the exact battery, buck, wiring, servo driver, rail behavior, and
body-connected USB/debug arrangement before integration.

## Firmware signal flow

MyoWare ENV → `emg_sensor` → `emg_processing` → `command_classifier` →
`state_machine` → `servo_controller` → mechanical hand.

`calibration` supplies user/session parameters to processing. `safety_manager`
will supervise faults and stop behavior. `main.cpp` coordinates the modules.
The current firmware only reads and prints ADC data; downstream modules do
not command motion yet.

TODO: Agree on startup, loss-of-signal, fault, and recovery behavior with the
mechanical and electrical design before enabling servo control.
