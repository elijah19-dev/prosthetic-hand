# Firmware test plan

Pure logic should be tested with synthetic EMG sequences before hardware
integration. The implemented test directories are under `test/`. Run all suites
with `python tools/run_tests.py`, using g++/clang++ on PATH (or the local validation
compiler under `.pio`). Generated binaries remain in ignored `.pio/host-tests`.

- Processing: invalid readings, baseline subtraction, clamped normalization.
- Classification: on/off hysteresis, debounce, release, and timeout behavior.
- State machine: startup, valid and invalid transitions, motion completion,
  interruption, fault entry, and latched faults that cannot recover on good input.
- Safety: sensor failure and implausible input must not become motion commands.

Example input sequence: `0.05 → 0.08 → 0.11 → 0.65 → 0.72 → 0.42 → 0.28 → 0.08`.
Expected commands depend on measured thresholds and timing, both TBD.

Current tests cover normalization, threshold equality, noise spikes,
interrupted debounce, hysteresis, startup release, reversed servo mounting,
paired motion/reversal, bounded targets, invalid configuration, invalid input,
late samples, contraction timeout and unsigned clock rollover. Test settings
are synthetic. State completion means the requested ramp reached its endpoint,
not that the physical mechanism reached it; hardware validation remains required.
