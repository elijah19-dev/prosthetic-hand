# Prosthetic Hand Project Plan

## 1. Project Purpose

This repository will contain the electrical-control software, hardware documentation, test plans, and optional machine-learning work for an EMG-controlled prosthetic hand.

The mechanical hand already exists. This project is responsible for the electrical and embedded-control system that will:

1. Read muscle activation from a MyoWare 2.0 EMG sensor.
2. Convert the EMG signal into a reliable control command.
3. Maintain the hand's operating state.
4. Command approximately three MG90S servos through a servo driver.
5. Provide safe, predictable behavior during startup, normal operation, faults, and loss of valid EMG input.
6. Leave room for future gesture classification or machine learning without requiring a rewrite of the firmware.

The first version should be intentionally simple: use the MyoWare **ENV** output and conventional signal thresholds/hysteresis. Machine learning is an optional later phase.

---

## 2. Known Hardware

### Controller
- ESP32 DOIT DevKit V1
- Programmed with PlatformIO
- Arduino framework
- Embedded C++

### EMG Sensor
- MyoWare 2.0 Muscle Sensor
- Initial signal: **ENV (envelope) output**
- ENV connected to an **ADC1-capable ESP32 pin**
- Current planned ADC pin: GPIO 34
- MyoWare powered from the ESP32 3.3 V rail

### Actuators
- Approximately 3 × MG90S servos
- Exact mechanical mapping of each servo is still TBD
- Exact open/closed servo positions must be determined experimentally

### Servo Driver
- PCA9685-based external servo controller planned
- The PCA9685 IC is confirmed; the exact breakout-board implementation is TBD
- The driver implementation should be isolated inside `servo_controller.*` so the rest of the firmware does not depend on a specific driver

### Power
- Final battery chemistry, voltage, and capacity are TBD
- An LM2596-based buck converter is planned for the approximately 5 V servo rail; the exact module requires verification
- The ESP32 power-input path is TBD. Do not assume that the approximately 5 V servo rail can power the DOIT DevKit through `VIN`; the available board reference recommends 7–12 V for external `VIN` operation
- During development, the ESP32 may be powered through its onboard USB connector
- ESP32 onboard regulator supplies 3.3 V to:
  - ESP32 logic
  - MyoWare sensor

### Grounding
The design should use separate **high-current servo return routing** and **signal/logic return routing**, while maintaining a common electrical reference.

Conceptually:

```text
                 BATTERY / TEST SOURCE (TBD)
                     |                 |
                     v                 v
             LM2596-BASED BUCK    ESP32 POWER INPUT
                     |                 TBD / USB DURING DEVELOPMENT
                     v                 |
          APPROXIMATELY 5 V            v
          SERVO POWER RAIL            ESP32
                     |                 |
           PCA9685-BASED BOARD     onboard 3.3 V
                     |                 |
           +---------+---------+       +------ MyoWare
           |         |         |                  |
         Servo1    Servo2    Servo3              ENV
                                                  |
                                                  v
                                              ADC1 pin
```

Return-current routing should prevent high servo currents from flowing through the same physical ground path used as the MyoWare/ADC reference.

The final power design must be validated using the exact battery, buck module,
PCA9685 breakout board, ESP32 power path, wiring, and MG90S units.

---

## 3. Core Design Philosophy

The firmware should be modular.

The intended information flow is:

```text
MyoWare ENV
    |
    v
EMG acquisition
    |
    v
EMG processing
    |
    v
command classification
    |
    v
hand state machine
    |
    v
servo controller
    |
    v
mechanical hand
```

Each layer should know as little as possible about the implementation of the layer below it.

For example:

- `main.cpp` should not directly call `analogRead()`.
- the state machine should not know ESP32 GPIO numbers.
- EMG processing should not directly command servos.
- servo control should not decide what a muscle contraction means.
- future machine learning should replace or extend classification without requiring changes to ADC acquisition or servo control.

---

## 4. Planned Repository Structure

```text
prosthetic-hand/
|
|-- platformio.ini
|-- README.md
|-- AGENTS.md
|-- PROJECT_PLAN.md
|-- .gitignore
|
|-- include/
|   |-- config.h
|   |-- emg_sensor.h
|   |-- emg_processing.h
|   |-- command_classifier.h
|   |-- state_machine.h
|   |-- servo_controller.h
|   |-- safety_manager.h
|   |-- calibration.h
|   `-- system_types.h
|
|-- src/
|   |-- main.cpp
|   |-- emg_sensor.cpp
|   |-- emg_processing.cpp
|   |-- command_classifier.cpp
|   |-- state_machine.cpp
|   |-- servo_controller.cpp
|   |-- safety_manager.cpp
|   `-- calibration.cpp
|
|-- test/
|   |-- test_emg_processing/
|   |-- test_command_classifier/
|   `-- test_state_machine/
|
|-- docs/
|   |-- system_architecture.md
|   |-- pinout.md
|   |-- power_budget.md
|   |-- calibration_procedure.md
|   |-- hardware_test_plan.md
|   `-- firmware_test_plan.md
|
|-- hardware/
|   |-- README.md
|   |-- schematics/
|   |-- pcb/
|   |-- datasheets/
|   `-- bom/
|
|-- ml/
|   |-- README.md
|   |-- requirements.txt
|   |-- scripts/
|   |   |-- collect_serial_data.py
|   |   |-- preprocess_data.py
|   |   |-- train_model.py
|   |   |-- evaluate_model.py
|   |   `-- export_model.py
|   |
|   |-- notebooks/
|   |-- models/
|   `-- data/
|       |-- raw/
|       `-- processed/
|
`-- tools/
    `-- README.md
```

Not every file must contain full implementation immediately. The initial goal is to create a clean architecture and then implement one subsystem at a time.

---

# 5. Firmware Files

## `src/main.cpp`

### Responsibility
Coordinate the application.

### It should:
- initialize each subsystem in `setup()`
- execute the top-level control loop
- read the sensor
- process EMG
- obtain a command
- update the state machine
- update servo outputs
- check safety/fault conditions
- emit useful debug telemetry

### It should NOT:
- contain ADC implementation details
- contain EMG filtering math
- contain servo-driver-specific code
- contain large switch statements implementing all system behavior
- contain hard-coded pins or thresholds

Target conceptual loop:

```cpp
void loop()
{
    // 1. Read EMG
    // 2. Process signal
    // 3. Classify intended command
    // 4. Check safety
    // 5. Update hand state
    // 6. Update servo positions
    // 7. Output telemetry when appropriate
}
```

Production logic should eventually avoid blocking `delay()` calls and use non-blocking timing where practical.

---

## `include/config.h`

### Responsibility
Central location for hardware constants and tunable firmware parameters.

### Examples
- serial baud rate
- EMG ADC pin
- sampling period
- servo channels
- servo min/max positions
- servo motion timing
- EMG thresholds
- hysteresis thresholds
- calibration constants
- timeout values
- debug/telemetry flags

Do not scatter tunable numbers throughout `.cpp` files.

Hardware-derived values should be documented.

Experimentally derived values should be clearly marked as calibration values.

---

## `include/system_types.h`

### Responsibility
Define types shared across multiple modules.

Potential types:

```cpp
struct EMGReading;
struct EMGProcessed;
enum class HandCommand;
enum class HandState;
enum class FaultCode;
struct ServoTargets;
```

This avoids duplicating enums or structs across modules.

Possible early definitions:

```cpp
enum class HandCommand
{
    NONE,
    OPEN,
    CLOSE
};

enum class HandState
{
    STARTUP,
    OPEN,
    OPENING,
    CLOSED,
    CLOSING,
    STOPPED,
    FAULT
};
```

These may change after mechanical testing.

---

## `include/emg_sensor.h`
## `src/emg_sensor.cpp`

### Responsibility
Hardware interface for the MyoWare sensor.

### Initial functions
- initialize ADC
- read raw ADC count
- read approximate millivolts

### Current planned hardware
- MyoWare ENV output
- GPIO 34
- ESP32 ADC1

### This module should NOT:
- determine whether a contraction occurred
- command the hand
- normalize the user's muscle strength
- contain servo logic

The existing implementation can remain the first hardware-acquisition layer.

---

## `include/emg_processing.h`
## `src/emg_processing.cpp`

### Responsibility
Turn sensor readings into stable, normalized muscle-activation information.

Because the MyoWare ENV output is already rectified and smoothed, the first version should remain simple.

Potential pipeline:

```text
raw ADC / voltage
       |
       v
validity check
       |
       v
baseline subtraction
       |
       v
optional light smoothing
       |
       v
normalization
       |
       v
0.0 - 1.0 activation estimate
```

Potential processed type:

```cpp
struct EMGProcessed
{
    float activation;
    bool valid;
};
```

### Initial processing features
- resting baseline
- calibrated contraction level
- clamped normalization
- optional moving average / low-pass smoothing if needed

Do not choose final filter constants until real hardware data is collected.

---

## `include/calibration.h`
## `src/calibration.cpp`

### Responsibility
Calibrate the EMG system to the user and electrode placement.

Possible workflow:

1. measure relaxed baseline
2. measure baseline variation/noise
3. record controlled contractions
4. estimate usable contraction range
5. calculate normalization parameters
6. save or apply the parameters for the session

Initial calibration may occur at every startup.

Future options:
- store values in ESP32 nonvolatile storage
- provide a button or serial command to recalibrate
- support multiple profiles

Calibration should be separate from normal signal processing.

---

## `include/command_classifier.h`
## `src/command_classifier.cpp`

### Responsibility
Convert processed EMG into a semantic hand command.

The first implementation should **not use machine learning**.

Initial approach:

```text
activation
   |
   v
threshold + hysteresis
   |
   v
HandCommand
```

Possible first commands:
- `NONE`
- `OPEN`
- `CLOSE`

Features to consider:
- separate ON/OFF thresholds
- minimum activation duration
- minimum release duration
- refractory/debounce period
- short contraction vs long contraction
- double contraction in a later version

This module is deliberately named `command_classifier` so a future ML classifier can replace the internal algorithm without changing the rest of the firmware.

---

## `include/state_machine.h`
## `src/state_machine.cpp`

### Responsibility
Control valid hand behavior over time.

Initial states:

```text
STARTUP
OPEN
OPENING
CLOSED
CLOSING
STOPPED
FAULT
```

Conceptual transitions:

```text
OPEN
 |
 | CLOSE command
 v
CLOSING
 |
 | motion complete
 v
CLOSED
 |
 | OPEN command
 v
OPENING
 |
 | motion complete
 v
OPEN
```

The state machine should handle:
- command acceptance
- invalid transitions
- motion completion
- command interruption if supported
- startup behavior
- safe stop
- fault behavior

The state machine should output desired behavior, not manipulate servo pins directly.

---

## `include/servo_controller.h`
## `src/servo_controller.cpp`

### Responsibility
Own all servo-specific behavior.

This module should isolate the rest of the firmware from:
- the selected servo driver
- I2C details
- PWM frequency
- channel assignments
- pulse-width mapping
- MG90S position limits

Potential public interface:

```cpp
class ServoController
{
public:
    void begin();
    void commandOpen();
    void commandClose();
    void stop();
    void update();
    bool motionComplete() const;
};
```

Potential future features:
- individual finger/servo target positions
- trajectory/ramp control
- grip presets
- maximum rate of movement
- position limits
- startup neutral position

Do not finalize servo angles until the mechanical hand is available.

---

## `include/safety_manager.h`
## `src/safety_manager.cpp`

### Responsibility
Centralize system-level safety and fault handling.

Potential faults:
- invalid EMG reading
- ADC saturation for an extended period
- sensor disconnected or implausible signal
- command timeout
- servo communication failure
- brownout/reset detection where available
- impossible state-machine condition

Potential behaviors:
- stop motion
- return to safe position if mechanically appropriate
- enter `FAULT`
- emit a fault code over Serial
- require explicit reset/recalibration

Exact safe behavior must be chosen with the mechanical team.

---

# 6. Timing Architecture

The final firmware should use explicit update rates instead of placing arbitrary `delay()` calls throughout the system.

Possible tasks:

```text
EMG sampling          faster periodic task
signal processing     every EMG sample or window
command classifier    periodic
state machine         periodic / event driven
servo update          periodic
telemetry             slower periodic task
```

Exact frequencies are TBD and should be selected after hardware testing.

Because MyoWare ENV is already smoothed, very high raw-EMG sampling rates are probably unnecessary for the first version.

Use `millis()` / `micros()` or another non-blocking timing method where practical.

---

# 7. Hardware Documentation

## `docs/system_architecture.md`

Should describe:
- electrical block diagram
- firmware block diagram
- signal flow
- power flow
- module responsibilities
- current design decisions
- unresolved design decisions

---

## `docs/pinout.md`

Single source of truth for ESP32 connections.

Example:

| Device | Signal | ESP32 Pin | Notes |
|---|---|---:|---|
| MyoWare 2.0 | ENV | GPIO 34 | ADC1 |
| Servo Driver | SDA | TBD | I2C |
| Servo Driver | SCL | TBD | I2C |
| Calibration Button | Signal | TBD | Optional |

Update this whenever wiring changes.

---

## `docs/power_budget.md`

Track the actual power design.

Include:

| Load | Voltage | Typical Current | Peak/Stall Current | Source |
|---|---:|---:|---:|---|
| ESP32 DevKit V1 | Input path TBD | TBD | TBD | TBD; USB during development |
| MyoWare 2.0 | 3.3 V | TBD | TBD | ESP32 3.3 V |
| MG90S #1 | ~5 V | TBD | verify | buck |
| MG90S #2 | ~5 V | TBD | verify | buck |
| MG90S #3 | ~5 V | TBD | verify | buck |
| PCA9685-based servo controller | Logic/servo supplies depend on breakout | TBD | TBD | Exact breakout TBD |

The final buck converter must be sized using measured or verified peak current, not average current alone.

Also document:
- fuse/protection choice
- wire gauge
- connector current ratings
- bulk capacitance
- regulator thermal performance
- measured 5 V droop during simultaneous servo movement/stall testing

---

## `docs/calibration_procedure.md`

Eventually document:
- electrode placement
- startup procedure
- relaxed calibration
- contraction calibration
- acceptance criteria
- recalibration conditions

---

## `docs/hardware_test_plan.md`

Planned bench tests should include:

### Power
- verify buck output with no load
- verify buck output with ESP32 load
- verify buck output with one servo
- verify with three moving servos
- observe 5 V rail during simultaneous startup
- test worst-case mechanical resistance carefully
- observe ESP32 reset/brownout behavior
- observe MyoWare ENV while servos move

### EMG
- relaxed signal
- voluntary contraction
- repeatability
- saturation
- electrode repositioning
- motion artifact
- servo-noise coupling
- battery-only operation
- USB-connected development configuration

### Servo
- one-servo movement
- all-servo movement
- safe angle limits
- mechanical end stops
- motion timing
- repeatability
- thermal/current behavior

---

## `docs/firmware_test_plan.md`

Test:
- normalization math
- threshold hysteresis
- command timing
- state transitions
- startup state
- invalid command rejection
- fault transitions
- recovery behavior
- servo target generation

---

# 8. Hardware Directory

## `hardware/schematics/`
Store exported schematics and source files when the electrical design moves beyond Tinkercad.

## `hardware/pcb/`
Store PCB source/export files if a custom board is created.

## `hardware/datasheets/`
Store or link to documentation for:
- ESP32 DevKit
- MyoWare 2.0
- MG90S
- servo driver
- buck converter
- battery
- connectors

Avoid committing copyrighted datasheets if distribution is not appropriate; a README with official links may be better.

## `hardware/bom/`
Bill of materials:
- part
- manufacturer
- part number
- quantity
- voltage/current ratings
- source
- notes

---

# 9. Machine Learning Plan

Machine learning is **not required for Version 1**.

With one MyoWare ENV channel, a threshold/hysteresis controller should be implemented and characterized first.

ML becomes more useful if:
- multiple EMG channels are added
- multiple gestures need classification
- signal patterns contain useful information beyond simple activation magnitude
- conventional rules become unreliable

The firmware should nevertheless be structured so classification can later be replaced.

---

## `ml/scripts/collect_serial_data.py`

Future script to:
- read ESP32 Serial output
- timestamp samples
- attach labels
- save CSV recordings

Possible columns:

```text
timestamp_ms
raw_adc
millivolts
activation
label
```

---

## `ml/scripts/preprocess_data.py`

Future tasks:
- clean recordings
- segment trials/windows
- normalize features
- handle missing samples
- create training datasets

---

## `ml/scripts/train_model.py`

Possible models to evaluate:
- logistic regression
- linear discriminant analysis
- small decision tree
- SVM
- other lightweight classifiers
- small neural network only if justified

The smallest model that provides reliable performance should be preferred.

---

## `ml/scripts/evaluate_model.py`

Should report:
- accuracy
- confusion matrix
- per-class precision/recall
- false activations
- missed commands
- latency
- user/session generalization

For a prosthetic controller, false activations and control latency may be more important than aggregate accuracy.

---

## `ml/scripts/export_model.py`

Future responsibility:
- convert trained parameters into a form usable on ESP32
- generate coefficients/header file if possible
- document exact feature ordering
- preserve model version metadata

Do not commit a large ML runtime to the ESP32 unless the model actually requires it.

---

# 10. Testing Strategy

## Pure logic should be testable without hardware

Modules such as:
- EMG normalization
- hysteresis
- timing logic
- state-machine transitions

should be written so they can be tested with synthetic values.

Example synthetic sequence:

```text
0.05
0.08
0.10
0.62
0.70
0.71
0.40
0.30
0.10
```

Expected behavior can then be checked before connecting the MyoWare.

## Hardware-dependent code

`emg_sensor.cpp` and the final servo-driver implementation require ESP32/hardware-in-loop testing.

---

# 11. Logging and Debugging

Serial telemetry should initially make development easy.

Example output:

```text
t=1250 raw=487 mv=391 activation=0.08 cmd=NONE state=OPEN
t=1350 raw=1520 mv=1210 activation=0.71 cmd=CLOSE state=CLOSING
```

Do not continuously print at a rate high enough to interfere with control timing.

Eventually support a configurable debug mode.

---

# 12. Git Workflow

Use small commits corresponding to functional milestones.

Examples:

```text
Add MyoWare ADC acquisition
Add EMG normalization
Add activation hysteresis
Add EMG calibration routine
Add hand state machine
Add servo driver interface
Add single-servo bench test
Add three-servo hand control
Add safety fault handling
Add EMG data logger
```

Before major experimental work, create a feature branch.

Examples:

```text
feature/emg-processing
feature/servo-control
feature/state-machine
feature/calibration
feature/ml-classifier
fix/adc-noise
fix/servo-jitter
```

Do not commit generated PlatformIO build output.

Potential `.gitignore` additions later:

```gitignore
# Python
__pycache__/
*.pyc
.venv/
venv/

# macOS
.DS_Store

# Large/local experimental data if desired
ml/data/raw/*
!ml/data/raw/.gitkeep

# Generated model artifacts if desired
ml/models/*
!ml/models/.gitkeep
```

Decide whether experimental data/models belong in Git before enabling those rules.

---

# 13. Development Milestones

## Milestone 0 — Repository and Toolchain
Status: largely complete

- PlatformIO project created
- ESP32 DOIT DevKit V1 configured
- project builds
- Git initialized
- GitHub remote configured
- initial code committed

---

## Milestone 1 — EMG Acquisition
Status: code started; hardware validation pending

Goals:
- read MyoWare ENV on ADC1
- print raw ADC
- print measured millivolts
- verify values change with muscle activation

Acceptance:
- stable serial communication
- no ADC clipping during normal contraction
- clear difference between relaxed and contracted muscle

---

## Milestone 2 — EMG Calibration and Processing

Goals:
- estimate resting baseline
- estimate contraction range
- normalize activation to approximately 0.0–1.0
- characterize noise
- add smoothing only if necessary

Acceptance:
- repeatable activation estimate
- no large false activation when relaxed
- reasonable response latency

---

## Milestone 3 — Command Classifier

Goals:
- threshold-based activation
- hysteresis
- activation timing/debounce
- produce `HandCommand`

Acceptance:
- deliberate contractions produce commands
- hovering near threshold does not chatter
- short noise spikes do not trigger commands

---

## Milestone 4 — Single Servo Control

Goals:
- identify/finalize the PCA9685 breakout implementation
- command one MG90S
- establish safe pulse/angle limits
- verify power rail behavior

Acceptance:
- repeatable motion
- no ESP32 resets
- no dangerous mechanical overtravel
- EMG reading remains stable during servo motion

---

## Milestone 5 — Three Servo Control

Goals:
- map all three servos
- define open/closed target sets
- coordinate motion
- characterize current draw

Acceptance:
- hand opens/closes reliably
- current draw remains within power design
- servo motion does not corrupt control signal

---

## Milestone 6 — State Machine

Goals:
- implement all required hand states
- connect EMG commands to servo targets
- support startup and stop behavior

Acceptance:
- only valid state transitions occur
- commands produce deterministic results
- fault state prevents unsafe movement

---

## Milestone 7 — Safety and Robustness

Goals:
- sensor validity checks
- timeouts
- fault codes
- safe startup
- brownout/noise testing
- wiring/power validation

Acceptance:
- failure modes are documented
- system fails safely for agreed scenarios

---

## Milestone 8 — Usability / Calibration Improvements

Potential additions:
- startup calibration routine
- status LED
- calibration button
- serial command interface
- adjustable sensitivity
- persistent settings

---

## Milestone 9 — Optional ML

Only start after collecting sufficient labeled data and establishing a conventional baseline.

Goals:
- collect labeled dataset
- train/evaluate simple models
- compare against threshold controller
- deploy only if ML provides meaningful improvement

---

# 14. Open Engineering Decisions

Do not let Codex invent these. They require hardware/mechanical decisions or testing.

- exact PCA9685 breakout board
- exact battery chemistry/capacity
- exact buck converter part/module
- verified MG90S current draw for the units used
- servo channel assignments
- servo mechanical angle limits
- open/closed positions
- whether all three servos move simultaneously
- final EMG ADC pin
- final EMG sample rate
- final smoothing algorithm
- EMG activation thresholds
- calibration procedure
- safe hand position on fault
- whether a physical emergency-stop or power switch is required
- whether current sensing will be added
- whether position feedback beyond the hobby-servo internal loop is required
- whether more than one MyoWare channel will be used
- whether ML is necessary

Any code depending on these decisions should expose placeholders/configuration rather than inventing values.

---

# 15. Safety Notes

This is a body-connected sensor and moving electromechanical system.

Development should include:
- current-limited bench power when practical
- conservative servo travel limits during early tests
- easy means to remove power
- careful handling of stalled servos
- power-rail measurements during motion
- body-connected electrical-safety considerations when USB/debug equipment is attached

Do not assume that successful software compilation proves safe hardware behavior.

---

# 16. Instructions for Codex

When Codex receives this file, it should first inspect the repository before changing anything.

## Phase A — Analyze

Codex should:

1. Read `PROJECT_PLAN.md`.
2. Read `AGENTS.md` if it exists.
3. Inspect the entire current repository.
4. Identify:
   - files already present
   - files already implemented
   - files that partially overlap this plan
   - missing planned files
   - any architecture conflicts
5. Explain the current architecture.
6. Explain the proposed final architecture.
7. Produce a concise implementation plan.
8. Do **not** modify files until the analysis is complete.

## Phase B — Scaffold

After analysis, Codex may create the missing project structure.

Requirements:

- Preserve existing working code.
- Do not replace working implementations with placeholders.
- Do not invent hardware-specific values that are marked TBD.
- Add header/source pairs only where appropriate.
- Keep interfaces minimal.
- Prefer comments/TODOs over fabricated behavior.
- Keep `main.cpp` small.
- Centralize constants in `config.h`.
- Create documentation files from this plan.
- Do not add unnecessary dependencies.
- Do not implement machine learning yet.
- Do not implement a specific servo-driver library until the board is selected.
- Do not change GPIO assignments without explaining why.
- Build the PlatformIO project after scaffolding.
- Fix any compile errors caused by the scaffolding.
- Summarize every file created or modified.

## Phase C — Implement incrementally

Implementation order should be:

1. preserve and validate existing EMG acquisition
2. EMG processing
3. calibration
4. command classifier
5. state machine
6. servo-controller interface
7. hardware-specific servo implementation after driver selection
8. safety manager
9. test coverage
10. optional ML tooling later

Do not implement multiple hardware-dependent subsystems in one uncontrolled change.

---

# 17. Suggested Codex Prompt

Paste the following into Codex after committing this plan:

> Read `PROJECT_PLAN.md` and follow the repository-level instructions in `AGENTS.md`. Inspect the entire repository. First, do not edit anything. Explain the current project structure, identify what has already been implemented, compare it against the plan, and describe the target architecture and implementation sequence. Then propose the exact files that should be created, preserved, or modified. Clearly identify any hardware-dependent values that must remain TBD. After presenting that analysis, scaffold the missing project files in the correct directories without overwriting working code or inventing hardware values. Keep interfaces minimal, keep `main.cpp` small, and add TODO comments where hardware testing is required. Do not add a servo-driver dependency or machine-learning implementation yet. Build the PlatformIO project after scaffolding and fix only errors introduced by the scaffold. Finally, summarize all created/modified files and recommend the next single implementation milestone.

---

# 18. Immediate Next Hardware Session

When hardware becomes available, the first objective is still:

```text
MyoWare ENV
    |
    v
ESP32 ADC1
    |
    v
Serial Monitor
```

Record:
- relaxed raw ADC
- relaxed millivolts
- moderate contraction
- strong contraction
- maximum observed value
- variability over repeated trials
- whether servo activity changes the EMG reading

Do not choose final thresholds before collecting these measurements.

---

# 19. Definition of Version 1 Complete

Version 1 is complete when:

- the user can intentionally command the hand to open and close using the MyoWare ENV signal
- three servos move the hand through safe, repeatable positions
- EMG processing is calibrated and robust enough for repeated use
- the controller does not chatter around thresholds
- servo activity does not cause false EMG commands
- power remains stable during expected loads
- startup and fault behavior are defined
- the code is modular and documented
- the project can be built from a clean checkout using PlatformIO
- the GitHub repository contains the documentation necessary for another club member to understand and continue the project

Machine learning is **not** required for Version 1.
