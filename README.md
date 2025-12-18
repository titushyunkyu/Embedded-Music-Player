# Embedded Music Player – MSPM0G3507

An embedded music player implemented on the TI MSPM0G3507 LaunchPad using the BOOSTXL-EDUMKII BoosterPack. The system generates audio using PWM to drive the onboard buzzer, supports play/pause control via push buttons, and displays real-time note information on the LCD. The project was developed using bare-metal C and hardware timers, with peripheral configuration guided directly by microcontroller and BoosterPack datasheets.

This project was completed as a final project for an Embedded Systems course and is shared here as a public source-code mirror for portfolio and code review purposes.

---

## Hardware Platform

- **Microcontroller:** TI MSPM0G3507 (80-MHz ARM Cortex-M0+)
- **Development Board:** MSPM0G3507 LaunchPad
- **BoosterPack:** BOOSTXL-EDUMKII
- **Peripherals Used:**
  - PWM timer (audio generation)
  - Push buttons (play / pause control)
  - LCD display (note and state output)
  - GPIO and hardware timers

---

## Tech Stack & Skills

- **Language:** C (bare-metal)
- **Architecture:** Finite State Machine (FSM)
- **Peripherals:** PWM, timers, GPIO, LCD
- **Core Skills:**  
  - Datasheet-driven peripheral configuration  
  - Timer and prescaler setup  
  - Real-time embedded control  
  - Low-level microcontroller programming  

---

## Features

- PWM-based audio generation to play a predefined song  
- Hardware timer–driven note timing and duration control  
- Finite State Machine for play/pause behavior  
- Push-button input handling (Start / Pause / Resume)  
- Real-time LCD updates showing current playback state and note  
- Looping song playback with precise timing  

---

## System Design Overview

- Notes are stored as frequency values and durations in arrays  
- A hardware timer generates PWM output corresponding to each note  
- A finite state machine manages playback state transitions  
- Button inputs control transitions between STOP and PLAY states  
- LCD output reflects system state and current note being played
- Pictorial description of project:
<img width="2523" height="1167" alt="image" src="https://github.com/user-attachments/assets/e0331ef1-f6b2-4739-a1cb-b99cfd5773c8" />


Peripheral configuration and timing calculations were implemented by referencing the MSPM0G3507 microcontroller datasheet and TI BoosterPack documentation.
