# Prosthetic Hand

Embedded control system for an EMG-controlled prosthetic hand. Intended for use by the Helping Hand Project at UNC-Chapel Hill

## Current Hardware

- ESP32 DOIT DevKit V1
- MyoWare 2.0 EMG sensor
- 3x MG90S servo motors
- Servo driver
- X V battery
- 12 V to 5 V buck converter

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