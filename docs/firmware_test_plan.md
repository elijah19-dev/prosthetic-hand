# Firmware test plan

Pure logic should be tested with synthetic EMG sequences before hardware
integration. Processing, contraction-event classification, target-toggle state
logic, and classifier-to-state integration tests are implemented under `test/`
and run on a host compiler.

- Processing: invalid readings, baseline subtraction, clamped normalization.
- Classification: on/off hysteresis, minimum contraction time, release/rearm,
  sustained contraction, invalid input, and `millis()` wraparound.
- State machine: explicit startup target, both toggle directions, invalid
  inputs, repeated command sequences, both fault types from relevant states,
  and stop/fault command gating.
- Control pipeline: two deliberate contractions separated by a valid release
  produce exactly two target changes; sustained activation does not repeat.
- Servo communication host logic: configured I2C pins/address and correct
  reporting of ACK, address NACK, initialization failure, and other bus errors.

The separate embedded `test/test_servo_communication` test requires the ESP32
and logic-powered PCA9685 board. It performs an address-only probe and must not
be confused with a host unit test or proof of servo operation.
- Safety: sensor failure and implausible input must not become motion commands.

Example input sequence: `0.05 → 0.08 → 0.11 → 0.65 → 0.72 → 0.42 → 0.28 → 0.08`.
The checked-in classifier values are synthetic test fixtures only. Real
thresholds and timing remain TBD pending measured user/session data.

Motion completion, interruption, and fault recovery remain untested because
their hardware and safety policies are still TBD. No test implies that a
commanded target was physically reached.
