# Single-channel wrist/thumb prototype

One MyoWare ENV channel drives a coordinated pair of poses. This is a deliberate
control mapping, not recognition of independent wrist and thumb intentions.
There is no machine learning or extra signal filter; the existing ENV
normalization is reused. Hysteresis and dwell reject threshold chatter and
short excursions, subject to the sampling interval.

## Behavior

1. Remain in STARTUP until activation stays at or below `off` for `releaseMs`.
2. Request the relaxed pose (OPEN). Activation at or above `on` for
   `activationMs` requests the contracted pose (CLOSE).
3. Retain the current command inside the hysteresis band. Interrupting a
   pending transition restarts its dwell. A sustained release requests OPEN.
4. Ramp one progress value between 0 and 1. Interpolate each servo between its
   own endpoints; opposite mounting directions are supported. A new command
   reverses from the current calculated position. EMG magnitude does not set
   intermediate positions in this first version.
5. Invalid input, either ADC rail, stale samples or an excessive continuous
   CLOSE duration latch FAULT. No further targets are valid until reset.

OPEN/CLOSED describe calculated ramp completion, not measured joint position.
`ServoController` only generates targets. It sends no I2C/PWM and cannot stop
an already energized motor. A future driver must handle fault output explicitly;
silence on I2C must never be treated as a physical stop.

## Required configuration

All settings live in `include/config.h`. Zero/-1 placeholders are intentionally
invalid. They must be replaced with measurements before previewing targets.

| Setting | Required meaning |
| --- | --- |
| `EMG_CALIBRATION` | Resting baseline and greater contraction reference, both raw ADC counts |
| `EMG_THRESHOLDS` | `0 <= off < on <= 1`, positive activation/release dwell in ms |
| `SAFETY_CONFIG.maxSampleGapMs` | Maximum sample age, greater than sampling period plus measured scheduling margin |
| `SAFETY_CONFIG.maxContractionMs` | Maximum continuous CLOSE command duration, established by bench testing |
| `SERVO_MOTION.wrist`, `.thumb` | Measured relaxed/contracted pulse endpoints within mechanical limits |
| `SERVO_MOTION.fullTravelMs` | Positive time for the full coordinated trajectory |
| `SERVO_MOTION.initialPosition` | Explicit initial progress in [0, 1]; the corresponding physical pose must be established before future motor enable |

The existing 100 ms acquisition interval is retained without blocking delay;
the final sampling rate still needs measurement. Dwell times are quantized by
sampling. Missed samples do not generate catch-up motion after a detected
timeout. The raw and millivolt telemetry fields come from separate ADC
conversions; only raw counts enter this controller. ADC rail rejection is
conservative and is not a complete sensor-disconnect detector.

## Bench integration still required

Identify the PCA9685 breakout, I2C pins/address, wrist/thumb channels and OE
wiring. Establish mechanical endpoints, initial alignment, motion timing,
power behavior and the physical fault response. No safe fault position is
assumed. A plausible floating ADC input or a stalled servo cannot be reliably
detected by this software alone. CLOSE timeout limits commanded contraction,
not all possible motor stalls or holding loads.

Then implement the isolated PCA9685 backend and verify disable/stop behavior
on communication failure and controller reset before enabling a loaded hand.
The original three-servo project plan remains a later milestone.

## Verification and telemetry

Run `python tools/run_tests.py` for the three host suites and `pio run` for the
ESP32 build. Test constants are fictional and must not be copied into hardware
configuration. No hardware safety claim follows from a successful build.

Serial prints raw counts, millivolts, activation, numeric command/state/fault,
target validity and both pulse targets. Enum order is in `system_types.h`:
commands NONE/OPEN/CLOSE/INVALID = 0/1/2/3; faults NONE/INVALID_CONFIGURATION/
INVALID_EMG/STALE_EMG/ACTIVATION_TIMEOUT/SERVO_COMMUNICATION = 0/1/2/3/4/5.
Defaults report configuration fault 1 and `targets_valid=0` while raw readings
remain available for calibration.
