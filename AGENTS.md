# AGENTS.md

# Prosthetic Hand Development Instructions

This repository contains the firmware, hardware documentation, testing,
and optional machine-learning work for an EMG-controlled prosthetic hand.

Read `PROJECT_PLAN.md` before making architectural changes.

---

## Engineering Roles

When working on this repository, evaluate tasks through the following
engineering perspectives.

### 1. Firmware Architect

Maintain clear separation between:

MyoWare acquisition
→ EMG processing
→ command classification
→ state machine
→ servo control

Do not allow hardware-specific implementation details to leak between layers.

Examples:

- `main.cpp` should coordinate modules, not contain their implementations.
- ADC operations belong in the EMG sensor module.
- EMG thresholds and normalization belong in processing/classification.
- Servo PWM/I2C details belong in the servo controller.
- The state machine should operate on commands and states rather than GPIOs.

Prefer small interfaces between modules.

---

### 2. Embedded Systems Engineer

The primary embedded platform is:

- ESP32 DOIT DevKit V1
- PlatformIO
- Arduino framework
- Embedded C++

Prefer embedded-friendly implementations.

Guidelines:

- Avoid unnecessary dynamic memory allocation.
- Prefer `constexpr` for compile-time constants.
- Prefer `enum class` over unscoped enums.
- Avoid unnecessary global mutable state.
- Keep functions small and focused.
- Avoid blocking `delay()` calls in final control logic where practical.
- Prefer `millis()` / `micros()` based timing for periodic behavior.
- Do not add dependencies unless they provide a clear benefit.
- Do not introduce an RTOS architecture unless the project actually requires it.

Simple solutions are preferred over unnecessarily sophisticated ones.

---

### 3. Hardware-Aware Firmware Engineer

Software decisions must respect the actual hardware.

Known hardware:

- ESP32 DOIT DevKit V1
- MyoWare 2.0 EMG sensor
- MyoWare ENV output for the initial implementation
- approximately three MG90S servos
- PCA9685-based external servo controller; exact breakout TBD
- final battery TBD
- LM2596-based buck converter planned for the approximately 5 V servo rail;
  exact module TBD
- MyoWare powered from the ESP32 3.3 V rail

The ESP32 power-input path remains TBD. Do not assume that the approximately
5 V servo rail is a supported input to the DOIT DevKit `VIN` pin.

Do not invent hardware specifications.

If a hardware value is unknown, use:

- a named configuration value,
- a TODO,
- or a clearly documented placeholder.

Do not fabricate:

- servo angles
- current limits
- EMG thresholds
- calibration values
- pin assignments
- timing constants
- servo-driver model
- battery characteristics
- buck-converter specifications

unless those values have already been established in the repository.

---

### 4. Safety Reviewer

This system combines:

- electronics connected to a human through an EMG sensor,
- moving mechanical components,
- battery power,
- servo motors capable of stalling.

Treat safety-related behavior explicitly.

Flag designs that could cause:

- uncontrolled servo motion,
- excessive mechanical travel,
- indefinite servo stall,
- unsafe startup movement,
- failure to stop during a fault,
- ADC/sensor failures being interpreted as valid commands,
- power instability causing unpredictable behavior.

Do not silently choose a "safe position" for the hand.

If safe behavior depends on mechanical design decisions that have not yet
been made, mark the behavior TBD.

Do not assume successful compilation implies safe hardware behavior.

---

### 5. Test Engineer

Pure software logic should be testable without hardware whenever practical.

Prioritize tests for:

- EMG normalization
- threshold logic
- hysteresis
- timing/debounce logic
- command classification
- state-machine transitions
- invalid transitions
- fault behavior

Hardware-interface modules may require hardware-in-loop testing.

Use synthetic EMG sequences for logic tests when hardware is unavailable.

Example:

0.05 → 0.08 → 0.11 → 0.65 → 0.72 → 0.42 → 0.28 → 0.08

Tests should verify expected commands and state transitions.

---

## Sources of Truth

Use information in this priority order:

1. Existing working code
2. `PROJECT_PLAN.md`
3. Hardware documentation stored in the repository
4. `docs/`
5. Explicit TODO/TBD values

Do not replace experimentally verified behavior with assumed behavior.

If the plan and working code conflict, explain the conflict before making
a major architectural change.

---

## Repository Architecture

Expected high-level layout:

```text
include/
    config.h
    system_types.h
    emg_sensor.h
    emg_processing.h
    calibration.h
    command_classifier.h
    state_machine.h
    servo_controller.h
    safety_manager.h

src/
    main.cpp
    emg_sensor.cpp
    emg_processing.cpp
    calibration.cpp
    command_classifier.cpp
    state_machine.cpp
    servo_controller.cpp
    safety_manager.cpp

test/

docs/

hardware/

ml/
