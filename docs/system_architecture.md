# System architecture

Status: acquisition plus software control for coordinated wrist/thumb targets.
Physical servo output and automatic calibration remain unimplemented.

## Electrical and power flow

The final battery is TBD. An LM2596-based buck is planned to supply the
approximately 5 V servo rail through a PCA9685-based controller board. The
exact buck and PCA9685 breakout implementations still require verification.

The ESP32 power-input path is unresolved. The available DOIT board reference
recommends 7–12 V for external `VIN` operation, so the approximately 5 V servo
rail must not be documented as a supported `VIN` source. The ESP32 may use USB
during development while the final power path is selected and verified.

The ESP32 3.3 V rail supplies the MyoWare 2.0. MyoWare ENV connects to the
planned ESP32 ADC1 GPIO 34 input. Servo power returns and signal/logic returns
need separate physical routing with a common electrical reference.

TODO: Verify the exact battery, buck module, PCA9685 breakout, ESP32 power
path, wiring, rail behavior, and body-connected USB/debug arrangement before
integration.

## Firmware signal flow

MyoWare ENV → `emg_sensor` → `emg_processing` → `command_classifier` →
`state_machine` → `servo_controller` → mechanical hand.

Manual user/session parameters in `config.h` feed the existing normalization;
`calibration` remains a placeholder for automatic collection. The classifier
adds hysteresis, activation/release dwell and a startup release requirement.
The state machine selects the relaxed or contracted pose. The servo module
ramps a shared progress value and maps it to separate wrist/thumb pulse targets.
`safety_manager` latches invalid-input, stale-input and contraction-duration
faults. `main.cpp` coordinates sampling, control and serial target preview
without `delay()`. It does not write PWM/I2C.

The requested two-servo prototype narrows the original three-servo plan; no
existing servo implementation was replaced. See
[wrist/thumb control](wrist_thumb_control.md) for behavior and remaining hardware work.

TODO: Agree on startup, loss-of-signal, fault, and recovery behavior with the
mechanical and electrical design before enabling servo control.
