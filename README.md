# Prosthetic Hand

Embedded control system for an EMG-controlled prosthetic hand. Intended for use by the Helping Hand Project at UNC-Chapel Hill

## Current Hardware

- ESP32 DOIT DevKit V1
- MyoWare 2.0 EMG sensor
- 3x MG90S servo motors
- PCA9685-based servo controller; exact breakout TBD
- Final battery TBD
- LM2596-based buck converter; exact module TBD

The ESP32 power-input path is unresolved. Do not assume that the planned
approximately 5 V servo rail is a supported input to the DOIT DevKit `VIN` pin.

## Planned Control Architecture

MyoWare ENV signal  
→ ESP32 ADC  
→ EMG processing  
→ command classification  
→ state machine  
→ servo control

## Development

Firmware is written in embedded C++ using PlatformIO and the Arduino framework.

Python may later be used for EMG data analysis and machine-learning experiments.
