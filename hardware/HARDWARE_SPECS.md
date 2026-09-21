# Hardware Specifications

## 1. Purpose

This document is the current hardware source of truth for the prosthetic-hand electrical and embedded-control system.

Only values supported by the available hardware documentation or explicitly confirmed project decisions are included as specifications. Unknown values are marked **TBD**, **NOT SPECIFIED**, or **REQUIRES VERIFICATION** rather than estimated.

This document should be updated whenever:
- a hardware component is changed,
- an exact part/module is confirmed,
- a new datasheet is added,
- a hardware measurement resolves a TBD,
- or the electrical architecture changes.

---

## 2. Current Hardware Status

| Subsystem | Current Part / Status | Confidence |
|---|---|---|
| Microcontroller | DOIT ESP32 DevKit V1; board-level reference identifies an ESP-WROOM-32 module | Board/model documentation now available; exact physical board/module marking still **REQUIRES PHYSICAL VERIFICATION** |
| EMG sensor | MyoWare 2.0 Muscle Sensor | Confirmed |
| Servo motors | 3 × MG90S metal-gear micro servos | Confirmed model name; exact manufacturer not documented |
| Servo PWM controller | PCA9685-based controller | PCA9685 IC confirmed by available datasheet; exact breakout-board implementation **TBD** |
| Buck regulator | LM2596-based buck converter | LM2596 IC documentation available; exact converter module implementation **REQUIRES VERIFICATION** |
| Battery | Final battery not selected | **TBD** |
| Temporary test source | Six alkaline cells or another suitable external source may be used for early testing | Experimental only; exact configuration **TBD** |

---

## 3. System-Level Planned Electrical Architecture

Current planned architecture:

```text
                      BATTERY / TEST SOURCE
                               |
                               v
                         LM2596 BUCK
                               |
                         ~5 V OUTPUT
                               |
                +--------------+------------------+
                |                                 |
                v                                 v
       SERVO POWER PATH                  ESP32 POWER INPUT
                |                          **TBD / VERIFY**
         PCA9685 BOARD                            |
                |                                ESP32
      +---------+---------+                       |
      |         |         |                      3.3 V
    MG90S     MG90S     MG90S                     |
                                                   +------ MyoWare 2.0
                                                             |
                                                            ENV
                                                             |
                                                             v
                                                       ESP32 ADC1
```

### Important ESP32 power-path update

The newly added DOIT ESP32 DevKit V1 board reference states:

- the board can be powered from the onboard USB Micro-B connector or the `VIN` pin,
- external supply operation is stated as **6 V to 20 V**,
- the recommended external input range is **7 V to 12 V**,
- and operation above 12 V may overheat the onboard regulator and damage the device.

Therefore, the previously planned connection:

```text
5 V buck output -> ESP32 VIN
```

is **not supported by the currently available DOIT board-level reference** and must remain an open design issue.

Do not assume that the board can be powered from the 5 V buck through `VIN` until one of the following is obtained:

1. an exact board schematic or manufacturer document explicitly supporting that connection,
2. identification of a separate 5 V input pin and its intended use on the exact board revision,
3. or direct hardware verification performed safely.

The servo power rail may still remain approximately 5 V because the MG90S operating range supports that voltage, but the ESP32 power-input path must be treated separately until resolved.

### Planned grounding

The project intends to use:
- a common electrical ground reference,
- separate physical high-current servo return routing,
- separate signal/logic return routing,
- and a controlled connection between those return paths.

The goal is to reduce servo-current noise coupled into the MyoWare/ADC measurement path.

The final grounding layout is **not yet validated on hardware**.

---

# 4. ESP32 Controller

## 4.1 Current identification

### Planned development board
**DOIT ESP32 DevKit V1**

### Available board-level reference
A board-level reference for the DOIT ESP32 DevKit V1 is now available.

Source file:

```text
hardware/datasheets/esp32_datasheet.pdf
```

The document states that the DOIT ESP32 DevKit V1 was created by DOIT to evaluate the **ESP-WROOM-32** module.

### Available module-level reference
The existing Espressif module datasheet remains useful for chip/module electrical details:

```text
hardware/datasheets/esp32_wroom-32_datasheet_v3.8.pdf
```

### How these two sources should be used

Use the **DOIT board-level reference** for:
- board power-entry information,
- the DevKit pinout/header mapping,
- USB/VIN power-source information,
- board-level input-voltage guidance.

Use the **Espressif ESP32-WROOM-32 datasheet** for:
- module supply requirements,
- ADC behavior,
- GPIO capabilities,
- processor/memory details,
- module electrical characteristics.

### Physical verification status

The physical development board should still be checked for:
- the exact text printed on the RF module,
- board revision/variant,
- any differences from the documented DOIT layout.

The board-level reference supports the current belief that the board uses an ESP-WROOM-32 module, but physical confirmation is still required before treating the hardware identity as final.

### Important document scope warning

Only the portion of `esp32_datasheet.pdf` describing the **DOIT ESP32 DevKit V1** should be used for this project.

The later portion of that PDF contains a separate **Joy-IT NodeMCU ESP32 manual**. Those NodeMCU pages describe a different development board and must **not** be used as specifications for the DOIT DevKit V1.

---

## 4.2 DOIT ESP32 DevKit V1 board-level specifications

The DOIT board reference lists the following:

### Board operating voltage
- **3.3 V**

This refers to the board/module logic operating voltage, not the external VIN recommendation.

### External input voltage
Device summary:
- **7 V to 12 V**

Power section:
- external supply operation stated as **6 V to 20 V**
- recommended range: **7 V to 12 V**
- using more than **12 V** may overheat the voltage regulator and damage the device

### Power-entry methods
The board reference states that power can be supplied through:
- the onboard **USB Micro-B** connector,
- or the **VIN** pin.

The power source is described as being selected automatically.

### Board-level processor/memory summary
The board reference lists:
- Tensilica Xtensa LX6 32-bit single-/dual-core CPU
- clock speed: **240 MHz**
- flash memory: **4 MB**
- SRAM: **520 KB**

### Board-level peripheral summary
The board reference lists:
- digital I/O pins: **25**
- analog input pins: **6**
- analog output pins (DAC): **2**
- UARTs: **3**
- SPIs: **2**
- I2Cs: **3**

These counts are retained as values from the board-level reference. For firmware-level peripheral capability and exact GPIO functionality, prefer the Espressif module datasheet and the actual board pin mapping.

---

## 4.3 ESP32-WROOM-32 module specifications

The module-level Espressif datasheet documents:

### Processor
- ESP32-D0WDQ6
- Xtensa dual-core 32-bit LX6 processor
- maximum CPU frequency: **240 MHz**

### Memory
- ROM: **448 KB**
- SRAM: **520 KB**
- RTC SRAM: **8 KB**
- integrated SPI flash: **4 MB**

### Module power supply
- recommended operating voltage: **3.0 V to 3.6 V**
- typical supply voltage: **3.3 V**
- datasheet recommended minimum external supply capability: **0.5 A**

### Operating temperature
- **-40 °C to +85 °C**

### Module dimensions
- **18.0 mm × 25.5 mm × 3.1 mm** nominal

The module datasheet is marked:

```text
NOT RECOMMENDED FOR NEW DESIGNS (NRND)
```

This does not prevent use of an existing board. It does mean the exact installed module/revision should be documented.

---

## 4.4 ADC

The ESP32 module contains:
- two 12-bit SAR ADCs,
- supporting measurements on up to 18 analog-enabled channels at the module/chip level.

### Planned EMG input pin

```text
GPIO34
```

The ESP32-WROOM-32 module datasheet identifies:

```text
GPIO34 = ADC1_CH6
```

The DOIT board-level pin mapping also exposes the corresponding GPIO34 connection on the development-board header.

GPIO34 is an input-only GPIO at the module level.

### ADC calibration and accuracy limitations

The module datasheet states that measured ADC results can differ between chips by approximately **±6% by default**.

Calibration support is therefore relevant to any attempt to convert ADC readings into accurate voltage.

For the highest attenuation setting documented in the module datasheet:

- effective calibrated measurement range: approximately **150 mV to 2450 mV**
- listed total error after calibration: **-60 mV to +60 mV**
- measurements above approximately **2450 mV** have reduced accuracy relative to the listed ADC specifications.

### Design implication

The MyoWare ENV output may extend up to its supply voltage.

If the MyoWare is supplied from 3.3 V, the possible sensor output range can extend above the ESP32 datasheet's highest listed effective calibrated ADC range.

Therefore:

**The direct MyoWare ENV -> ESP32 GPIO34 connection must be experimentally characterized before the design is considered final.**

Items requiring validation:
- actual maximum ENV voltage at the selected MyoWare gain,
- whether the normal signal remains below the ESP32's useful ADC range,
- whether ADC saturation occurs,
- whether additional attenuation or analog conditioning is required.

No divider or additional conditioning network has been selected yet.

---

## 4.5 I2C

The ESP32-WROOM-32 module datasheet documents:
- two I2C bus interfaces,
- standard mode support: **100 kbit/s**
- fast mode support: **400 kbit/s**
- higher programmable rates subject to electrical constraints.

The separate DOIT board-level reference lists three I2Cs in its device summary. Because this differs from the Espressif module-level description, firmware design should follow the Espressif module documentation for actual hardware capability unless a DOIT-specific implementation is independently verified.

The PCA9685 servo controller is intended to communicate with the ESP32 over I2C.

### Planned I2C pins
- SDA: **TBD**
- SCL: **TBD**

Do not assign final pins until the complete board pinout and system requirements are reviewed.

---

## 4.6 DevKit-level specifications still missing

The new DOIT board reference resolves the general VIN guidance and power-entry methods, but it does **not** establish all board-level electrical details.

The following remain unresolved:

- exact onboard 3.3 V regulator model,
- exact onboard regulator current limit,
- maximum recommended current available from the board's 3.3 V output,
- onboard regulator thermal performance for this application,
- exact USB-to-serial IC on the physical board,
- protection circuitry on VIN/USB,
- whether a separate 5 V board pin may be used as a regulated 5 V input,
- exact board schematic,
- exact physical board revision.

These values require:
1. more specific board documentation,
2. component identification,
3. or hardware measurement.

---

# 5. MyoWare 2.0 Muscle Sensor

## 5.1 Identification

Part:
**MyoWare 2.0 Muscle Sensor**

Documented product identifier:

```text
DEV-18977
```

Source file:

```text
hardware/datasheets/myoware_2.0.pdf
```

The currently available document states that its information should be used as a guideline.

---

## 5.2 Supply specifications

- Minimum supply voltage: **2.27 V**
- Typical supply range: **3.3 V to 5 V**
- Maximum supply voltage: **5.47 V**
- Reverse-polarity protection: documented

### Planned supply

Current planned MyoWare supply:

```text
ESP32 3.3 V rail
```

This is within the documented MyoWare supply range.

The current capability of the specific ESP32 DevKit 3.3 V regulator must still be verified at the board level.

---

## 5.3 Signal outputs

Three documented output modes:
- Raw EMG
- Rectified EMG
- Envelope (ENV)

### Initial project signal

The initial prosthetic-hand implementation will use:

```text
ENV
```

The available MyoWare documentation describes the sensor output range as:

```text
0 V to VIN
```

where `VIN` is the MyoWare supply voltage.

The ENV signal is intended to represent muscle activation level.

### Planned interface

```text
MyoWare ENV
    |
    v
ESP32 GPIO34 / ADC1_CH6
```

This interface requires hardware validation because the possible MyoWare output range can exceed the ESP32 ADC's highest documented effective calibrated range.

---

## 5.4 Other documented characteristics

- Adjustable gain: yes
- Input bias current: **250 pA**
- Maximum input bias current: **1 nA**
- Board dimensions: **37.57 mm × 35.90 mm**
- VIN LED indicator: yes
- ENV LED indicator: yes
- Reference-electrode jumper: yes

---

## 5.5 MyoWare specifications not provided by the current document

The available document does **not** establish all parameters required for a final signal-chain design.

The following remain **NOT SPECIFIED** by the currently stored document:

- normal operating current consumption,
- maximum supply current,
- ENV output impedance,
- detailed ENV filter bandwidth,
- detailed RAW-path bandwidth,
- rectified-output bandwidth,
- exact adjustable-gain range,
- gain-setting procedure in electrical terms,
- absolute output accuracy,
- output noise specification,
- recommended source impedance when driving an MCU ADC,
- maximum recommended ADC load,
- settling time,
- exact response time.

If these values become necessary, obtain a more detailed manufacturer technical document or characterize them experimentally.

---

# 6. MG90S Servo Motors

## 6.1 Identification

Planned quantity:

```text
3
```

Servo model:

```text
MG90S
```

Source file:

```text
hardware/datasheets/mg90s_servo.pdf
```

The exact manufacturer of the purchased MG90S units is not established by the current documentation.

Because MG90S products may be sold by multiple suppliers, specifications should not be combined from unrelated MG90S listings without verification.

---

## 6.2 Documented electrical specifications

### Operating voltage

```text
4.8 V to 6.0 V
```

A nominal 5 V servo rail is therefore within the documented operating range.

### Control signal

The provided document shows:
- PWM period: **20 ms**
- corresponding frequency: **50 Hz**
- pulse width approximately **1 ms to 2 ms**
- approximately **1.5 ms** represents the middle position

Approximate positions shown in the document:
- ~1 ms: one end of travel
- 1.5 ms: center
- ~2 ms: opposite end of travel

These values describe the servo itself and do **not** establish safe motion limits for the prosthetic hand.

Final mechanical pulse limits must be measured with the actual hand.

### Dead band

- **5 µs**

---

## 6.3 Mechanical performance

### Approximate rotation
- approximately **180 degrees**
- approximately 90 degrees in each direction from center

### Stall torque
At **4.8 V**:
- **1.8 kgf·cm**

At **6.0 V**:
- **2.2 kgf·cm**

### Operating speed
At **4.8 V**:
- **0.1 s / 60°**

At **6.0 V**:
- **0.08 s / 60°**

### Weight
- **13.4 g**

### Approximate dimensions
- **22.5 mm × 12 mm × 35.5 mm**

---

## 6.4 Critical missing servo electrical data

The current MG90S document does **NOT** specify:

- stall current,
- startup/inrush current,
- no-load current,
- running current under load,
- idle holding current,
- motor winding resistance,
- allowable continuous stall duration,
- thermal limits,
- connector current rating.

### Design consequence

The total servo power requirement cannot yet be calculated from the available datasheet.

Therefore, the suitability of the LM2596 regulator for three simultaneous MG90S servos **cannot currently be confirmed**.

Required action:
- obtain authoritative current specifications for the exact purchased servo,
- or measure current experimentally under representative and worst-case conditions.

Do not assign a final power-supply current requirement until this gap is resolved.

---

# 7. PCA9685 PWM Controller

## 7.1 Identification

Controller IC:

```text
PCA9685
```

Document:

```text
hardware/datasheets/PCA9685_servo_driver.pdf
```

Important distinction:

The current document is the **NXP PCA9685 IC datasheet**.

It does not document the complete electrical design of the specific breakout/servo-driver board being used.

---

## 7.2 PCA9685 documented specifications

### Interface
- I2C controlled

### PWM channels
- **16 channels**

### PWM resolution
- **12 bit**
- **4096 steps**

### Programmable PWM frequency
Typical programmable range:

```text
24 Hz to 1526 Hz
```

The MG90S documented control frequency of 50 Hz lies within this programmable range.

### Internal oscillator
- Typical internal oscillator: **25 MHz**

### External clock
- External clock supported
- Maximum external clock: **50 MHz**

### Logic supply

```text
2.3 V to 5.5 V
```

### Input/output tolerance
- Inputs and outputs documented as **5.5 V tolerant**

### I2C capability
- Fast-mode Plus support
- up to **1 MHz** I2C bus operation

### Hardware addressing
- six hardware address inputs: A0 through A5
- supports up to 62 PCA9685 devices on a bus, subject to address restrictions described in the datasheet

### Output enable
- `OE` is active LOW
- can asynchronously control output enable state

### Power-on state
- PCA9685 PWM output pins default LOW at power-on

---

## 7.3 PCA9685 output-driver electrical limits

The PCA9685 IC outputs are logic/PWM outputs, not servo-power outputs.

At 5 V, documented output-driver capability includes:
- open-drain mode: up to **25 mA sink**
- totem-pole mode: up to **25 mA sink**
- totem-pole source capability: up to **10 mA source**

These limits apply to the PCA9685 output pins.

The servos must receive motor power from an appropriate external power rail rather than from a PCA9685 PWM output pin.

---

## 7.4 Planned interface

Planned logic connection:

```text
ESP32
 | \
 |  \
SDA  SCL
 |    |
 +----+
   |
   v
PCA9685
```

### Planned PCA9685 logic voltage

A 3.3 V logic supply is electrically within the PCA9685 IC's documented 2.3 V to 5.5 V operating range.

However, the exact breakout board circuitry must be verified before finalizing the connection.

### PWM target for MG90S

Initial intended PCA9685 output frequency:

```text
50 Hz
```

This comes from the MG90S servo document.

Final pulse-width limits for each mechanical servo channel remain **TBD**.

---

## 7.5 PCA9685 breakout-board information still missing

The current IC datasheet does not establish the following characteristics of the actual servo-driver board:

- exact board manufacturer,
- exact board model/revision,
- servo V+ allowable voltage range,
- total servo-rail current rating,
- PCB trace current capability,
- terminal-block current rating,
- onboard bulk capacitance,
- onboard decoupling,
- reverse-polarity protection,
- fuse/protection features,
- logic-voltage pull-up network,
- SDA/SCL pull-up resistor values,
- whether pull-ups connect to board VCC,
- default A0-A5 strap configuration,
- default board I2C address,
- OE pin wiring on the breakout,
- any regulator present on the board.

Obtain the specific breakout-board schematic or manufacturer documentation if available.

---

# 8. LM2596 Buck Regulator

## 8.1 Identification

Regulator family:

```text
LM2596
```

Source file:

```text
hardware/datasheets/lm2596_buck_converter.pdf
```

Important distinction:

The current document is the **Texas Instruments LM2596 regulator IC datasheet**.

It does not define the complete performance of an assembled third-party LM2596 converter module.

The exact physical buck-converter board/module must still be documented.

---

## 8.2 LM2596 documented specifications

### Regulator type
- nonsynchronous step-down (buck) switching regulator

### Nominal output-current rating

```text
3 A
```

This is the LM2596 IC load-current rating under appropriate circuit and thermal conditions.

It must not automatically be treated as the guaranteed continuous current capability of an unknown converter module.

### General operating input range

```text
4.5 V to 40 V
```

### Absolute maximum supply voltage

```text
45 V
```

### Available versions
- 3.3 V fixed
- 5 V fixed
- 12 V fixed
- adjustable

### Adjustable-version output range

```text
1.2 V to 37 V
```

### Switching frequency

```text
150 kHz nominal
```

### Protection
- current limiting
- thermal shutdown

### Typical operating quiescent current
- **5 mA typical**
- **10 mA maximum** under the datasheet's specified conditions

### Standby current
- approximately **80 µA typical** under the documented standby condition

---

## 8.3 5 V version characteristics

For the fixed 5 V version, the datasheet specifies output performance under:

```text
VIN = 7 V to 40 V
ILOAD = 0.2 A to 3 A
```

At 25 °C:
- nominal output: **5.0 V**
- listed range under the specified conditions: **4.8 V to 5.2 V**

Across the listed junction-temperature range:
- **4.75 V to 5.25 V**

Typical efficiency at:

```text
VIN = 12 V
VOUT = 5 V
ILOAD = 3 A
```

is:

```text
80%
```

---

## 8.4 External component requirements

The LM2596 IC requires external power-stage components.

The datasheet identifies the importance of:
- input bypass capacitance,
- output capacitance,
- inductor selection,
- fast catch diode,
- low-ESR capacitors,
- short PCB traces,
- appropriate thermal layout.

For low-voltage buck applications, the datasheet recommends a fast diode and discusses Schottky diodes as a preferred option.

### Design implication

A third-party module's actual performance depends on its:
- inductor,
- catch diode,
- capacitors,
- PCB copper,
- thermal design,
- connector ratings,
- and component authenticity/quality.

Therefore:

**The LM2596 IC datasheet alone is not enough to establish the continuous output capability of the purchased buck-converter module.**

---

## 8.5 LM2596 information still requiring verification

- exact converter module manufacturer,
- exact module model,
- whether the installed IC is fixed-5-V or adjustable LM2596,
- inductor value and saturation-current rating,
- diode part number/current rating,
- input capacitor specification,
- output capacitor specification,
- module connector ratings,
- module PCB current capability,
- heatsinking/thermal performance,
- guaranteed continuous module current,
- transient-load response with the three servos,
- output ripple of the actual module.

---

# 9. Battery / Primary Power Source

## 9.1 Final battery

```text
TBD
```

No final battery chemistry, voltage, capacity, form factor, or discharge rating has been selected.

Do not add assumed battery values to firmware or design calculations.

---

## 9.2 Temporary test source

A temporary test pack using six alkaline cells is under consideration.

The following are currently **TBD**:
- exact cell format,
- exact cell model,
- pack voltage under load,
- capacity,
- allowable discharge current,
- internal resistance,
- holder/contact resistance,
- ability to support simultaneous servo transients.

A larger battery or laboratory power source may be used instead.

The temporary source should not be treated as the final prosthetic-hand battery.

---

## 9.3 Final battery requirements to establish

The final battery must be selected only after system current is measured or reliably established.

Required specifications include:
- chemistry,
- series/parallel configuration,
- nominal voltage,
- full-charge voltage,
- minimum usable voltage,
- capacity in Ah or mAh,
- continuous discharge-current rating,
- peak discharge-current rating,
- internal protection/BMS requirements,
- fuse/protection requirements,
- charging method,
- connector,
- size,
- mass,
- desired runtime,
- safe mounting location.

---

# 10. Current Confirmed Voltage Domains

The following voltage information is supported by current component documentation:

| Component | Documented Voltage |
|---|---|
| DOIT ESP32 DevKit V1 external VIN | external operation stated as 6–20 V; recommended 7–12 V |
| DOIT ESP32 DevKit V1 logic/operating voltage | 3.3 V |
| ESP32-WROOM-32 module | 3.0–3.6 V supply; 3.3 V typical |
| MyoWare 2.0 | 2.27–5.47 V; 3.3–5 V typical |
| MG90S | 4.8–6.0 V |
| PCA9685 IC | 2.3–5.5 V |
| LM2596 family | 4.5–40 V operating input |
| LM2596 fixed 5 V version | performance characterized for 7–40 V input at 0.2–3 A load |
| Final battery | TBD |

### Current planned rails

```text
Servo power: approximately 5 V
ESP32 DevKit power input: TBD
    - 5 V into VIN is NOT supported by the current DOIT board-level reference
    - DOIT reference recommends 7–12 V on external VIN
ESP32 module rail: 3.3 V
MyoWare: 3.3 V planned
PCA9685 logic VDD: 3.3 V planned, pending breakout-board verification
```

### Power-architecture consequence

The current single-buck concept of:

```text
battery -> ~5 V buck -> servos + ESP32 VIN
```

must not be treated as finalized.

The servo rail can remain approximately 5 V based on the MG90S voltage specification, but the ESP32 input path requires a supported board-level connection or a revised power architecture.

No revised ESP32 power solution is selected in this document.

---

# 11. Signal Interfaces

## 11.1 MyoWare → ESP32

```text
MyoWare ENV
    |
    v
ESP32 GPIO34
ADC1_CH6
```

Status:
- software interface prepared,
- hardware validation pending.

Key unresolved issue:
- actual ENV maximum versus useful ESP32 ADC measurement range.

---

## 11.2 ESP32 → PCA9685

```text
ESP32 I2C
   |
   +-- SDA --> PCA9685 SDA
   |
   +-- SCL --> PCA9685 SCL
```

Status:
- I2C architecture selected,
- final GPIO assignments TBD,
- breakout-board electrical details TBD.

---

## 11.3 PCA9685 → MG90S

```text
PCA9685 PWM channel
        |
        v
MG90S signal wire
```

Initial intended frequency:

```text
50 Hz
```

Final channel assignments:
- Servo 1: TBD
- Servo 2: TBD
- Servo 3: TBD

Final mechanical pulse-width limits:
- Servo 1: TBD
- Servo 2: TBD
- Servo 3: TBD

---

# 12. Major Compatibility Checks

## 12.1 MyoWare output vs ESP32 ADC

**OPEN ISSUE**

MyoWare:
- output range documented as 0 V to VIN.

Planned MyoWare VIN:
- 3.3 V.

ESP32:
- highest documented effective calibrated ADC range reaches approximately 2.45 V.

Required:
- characterize actual ENV signal,
- confirm gain setting,
- determine whether full expected signal fits useful ADC range,
- add attenuation/conditioning only if required.

---

## 12.2 Planned 5 V buck output vs DOIT ESP32 VIN

**OPEN ISSUE / CURRENT PLAN NOT SUPPORTED BY AVAILABLE BOARD REFERENCE**

The DOIT board-level reference states:
- external operation from 6 V to 20 V,
- recommended VIN range of 7 V to 12 V.

The current buck-converter concept targets approximately 5 V for the servo rail.

Therefore:

```text
5 V buck -> ESP32 VIN
```

must not be considered validated or documented as a supported connection.

Required resolution:
- identify a documented 5 V input path on the exact board revision, **or**
- revise the ESP32 power architecture, **or**
- obtain additional exact-board documentation and validate safely.

---

## 12.3 MG90S voltage vs planned servo rail

**DOCUMENTED COMPATIBILITY**

MG90S:
- 4.8–6.0 V.

Planned rail:
- approximately 5 V.

Final voltage regulation and transient behavior still require measurement.

---

## 12.4 MG90S PWM vs PCA9685

**DOCUMENTED COMPATIBILITY**

MG90S:
- 50 Hz documented control period.

PCA9685:
- programmable PWM range includes 50 Hz.

Final pulse widths must be constrained based on the actual mechanical hand.

---

## 12.5 ESP32 logic vs PCA9685

**IC-LEVEL COMPATIBILITY SUPPORTED; BOARD-LEVEL VERIFICATION REQUIRED**

ESP32:
- 3.3 V logic system.

PCA9685 IC:
- operates from 2.3–5.5 V,
- inputs are 5.5 V tolerant.

Planned:
- PCA9685 VDD = 3.3 V.

Must verify the actual breakout board's pull-ups and wiring.

---

## 12.6 Three MG90S servos vs LM2596

**UNRESOLVED**

Known:
- LM2596 IC nominal output load capability: 3 A.

Unknown:
- MG90S current draw,
- simultaneous startup current,
- simultaneous stall current,
- exact LM2596 module continuous capability.

Therefore:
- no claim can currently be made that one LM2596 module can safely power all three servos plus the ESP32 system.

This must be resolved before final power-system approval.

---

# 13. Hardware Measurements Required

When hardware is available, record the following.

## 13.1 MyoWare

Measure:
- MyoWare supply voltage,
- resting ENV voltage,
- moderate contraction ENV voltage,
- strong contraction ENV voltage,
- maximum observed ENV voltage,
- ADC counts for each condition,
- noise at rest,
- response when servos operate,
- effect of gain adjustment,
- ADC clipping/saturation.

---

## 13.2 ESP32 power

Measure:
- 5 V/VIN input voltage,
- 3.3 V rail,
- 3.3 V rail while MyoWare is connected,
- rail behavior during servo movement,
- reset/brownout behavior.

---

## 13.3 Each MG90S servo

Measure:
- idle current,
- unloaded movement current,
- representative mechanical-load current,
- startup transient current,
- carefully controlled worst-case current,
- voltage at the servo during movement,
- safe mechanical pulse-width limits.

Do not intentionally leave a servo stalled for an uncontrolled duration.

---

## 13.4 Three-servo system

Measure:
- simultaneous operating current,
- worst expected transient current,
- 5 V rail minimum during simultaneous motion,
- regulator temperature,
- wiring/connector temperature,
- noise coupled into MyoWare ENV,
- ESP32 stability.

---

## 13.5 Buck converter

Measure:
- input voltage,
- output voltage at no load,
- output voltage with ESP32 only,
- output voltage with one servo,
- output voltage with all servos,
- transient droop,
- ripple,
- temperature,
- input current,
- output current.

---

# 14. Critical Hardware Gaps

The following items must remain explicitly unresolved until evidence is available.

### ESP32
- exact module marking on physical board
- exact DOIT DevKit board revision
- onboard regulator model and current limit
- available 3.3 V output current
- exact board schematic
- exact USB-to-serial IC on the physical board
- whether the exact board provides a documented regulated 5 V input path separate from the recommended 7–12 V VIN path
- final method for powering the ESP32 from the prosthetic-hand power system

### MyoWare
- supply current
- detailed ENV electrical/bandwidth characteristics
- actual selected gain
- actual maximum ENV signal in this application

### MG90S
- exact manufacturer
- operating current
- stall current
- startup current
- thermal limits
- safe mechanical range in the prosthetic

### PCA9685 board
- exact breakout manufacturer/model
- servo power-rail rating
- board trace/connector current rating
- pull-up values
- default address straps
- protection circuitry

### LM2596 module
- exact converter module model
- fixed or adjustable regulator variant
- actual external component ratings
- real continuous-current capability
- thermal behavior
- transient response

### Battery
- all final battery specifications

---

# 15. Source Documents

Current hardware references:

```text
hardware/datasheets/esp32_datasheet.pdf
hardware/datasheets/esp32_wroom-32_datasheet_v3.8.pdf
hardware/datasheets/myoware_2.0.pdf
hardware/datasheets/mg90s_servo.pdf
hardware/datasheets/PCA9685_servo_driver.pdf
hardware/datasheets/lm2596_buck_converter.pdf
```

### ESP32 source-use note

`esp32_datasheet.pdf` contains more than one board manual.

For this project:
- use the **DOIT ESP32 DevKit V1** portion as the board-level reference,
- do **not** use the later **Joy-IT NodeMCU ESP32** portion as specifications for the DOIT board.

The Espressif `esp32_wroom-32_datasheet_v3.8.pdf` remains the preferred source for module-level electrical and ADC specifications.

## Source hierarchy

For engineering decisions, prefer:

1. exact manufacturer datasheet for the exact physical component,
2. manufacturer schematic/manual for the exact breakout/module,
3. direct hardware measurement,
4. project documentation derived from those sources.

Do not fill a missing specification with a value from a different manufacturer or an unrelated product listing without clearly documenting the source and confirming applicability.

---

# 16. Rules for Codex

When using this file:

- Treat values marked **TBD**, **NOT SPECIFIED**, **OPEN ISSUE**, or **REQUIRES VERIFICATION** as unknown.
- Do not invent missing hardware values.
- Do not substitute common internet values for missing current, voltage, timing, or thermal specifications.
- Do not assume the IC datasheet fully specifies a third-party breakout/module.
- Use `esp32_datasheet.pdf` only for its DOIT ESP32 DevKit V1 portion; ignore the later Joy-IT NodeMCU section for DOIT-specific decisions.
- Use the Espressif ESP32-WROOM-32 datasheet for module-level ADC/GPIO/electrical behavior.
- Do not assume the current ESP32-WROOM-32 document is correct until the physical module marking is verified.
- Do not treat 5 V applied to the DOIT `VIN` pin as supported; the current board-level reference recommends 7–12 V external VIN.
- Do not choose final servo positions without mechanical testing.
- Do not choose final EMG thresholds without real MyoWare measurements.
- Do not claim the LM2596 power system is adequate until the servo-current requirement and actual module capability are established.
- Update this document when verified hardware information changes.
