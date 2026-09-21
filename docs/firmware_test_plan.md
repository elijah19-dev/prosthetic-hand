# Firmware test plan

Pure logic should be tested with synthetic EMG sequences before hardware
integration. The planned test directories are under `test/`; no logic or
tests have been implemented by the scaffold.

- Processing: invalid readings, baseline subtraction, clamped normalization.
- Classification: on/off hysteresis, debounce, release, and timeout behavior.
- State machine: startup, valid and invalid transitions, motion completion,
  interruption, fault entry, and recovery after its policy is agreed.
- Safety: sensor failure and implausible input must not become motion commands.

Example input sequence: `0.05 → 0.08 → 0.11 → 0.65 → 0.72 → 0.42 → 0.28 → 0.08`.
Expected commands depend on measured thresholds and timing, both TBD.
