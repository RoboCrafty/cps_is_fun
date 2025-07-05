# CPS Embedded Systems Lab Projects

## Overview
This repository contains a comprehensive collection of embedded systems laboratory projects for the dsPIC33FJ256MC710 microcontroller as part of the Concepts and Software Design for Cyber-Physical Systems module at TUM. The labs progress from basic I/O operations to advanced topics including interrupts, timers, DAC (Digital-to-Analog Conversion), and touchscreen/servo control.

## Repository Structure
```
cps_is_fun/
├── Lab00/                    # Hello World - Basic LCD and LED control
├── Lab01/                    # LCD and LED counter demonstration
├── Lab02/                    # Interrupts and Timers
├── Lab03/                    # DAC (Digital-to-Analog Converter)
├── Lab05/                    # Touchscreen and Servo Control
└── reference_documents/      # Datasheets and documentation
```

## Hardware Requirements
- **Microcontroller:** dsPIC33FJ256MC710
- **Display:** Sparkfun Serial Graphic LCD 128x64
- **DAC:** MCP4822 (12-bit Digital-to-Analog Converter)
- **Touchscreen:** Resistive touchscreen interface
- **Servos:** Standard RC servos (X and Y axis control)
- **Programmer/Debugger:** ICD3 or compatible
- **FLEX UI Board:** Custom development board

## Software Requirements
- **Compiler:** Microchip XC16 (tested with v1.36)
- **IDE:** MPLAB X or NetBeans (optional)
- **Make:** GNU Make for command-line builds
- **MATLAB:** For Lab03 signal processing (optional)

## Lab Descriptions

### Lab00: Hello World
**Purpose:** Introduction to basic I/O operations
- **Features:**
  - LCD initialization and text display
  - LED control and initialization
  - Basic microcontroller configuration
- **Key Files:**
  - `src/main.c`: Displays "Hello Wld!" on LCD
  - `src/led.c`: LED initialization routines
  - `include/lcd.h`: LCD control library
  - `include/led.h`: LED control macros

### Lab01: LCD and LED Counter
**Purpose:** Demonstrate real-time counter with visual feedback
- **Features:**
  - Real-time counter display on LCD
  - Binary LED counter (5 LEDs)
  - Group member information display
- **Key Files:**
  - `src/main.c`: Main counter loop with LED patterns
  - Uses shared LCD and LED libraries

### Lab02: Interrupts and Timers
**Purpose:** Introduction to interrupt-driven programming and timer management
- **Features:**
  - Timer1: 1-second interrupt (external 32.768 kHz clock)
  - Timer2: 2-millisecond interrupt (internal clock)
  - Timer3: Cycle counting for performance measurement
  - Real-time clock display (MM:SS:mmm format)
  - LED indicators for different timer events
- **Key Files:**
  - `src/main.c`: Main initialization and timer loop
  - `src/lab02.c`: Timer configuration and interrupt handlers
  - `include/lab02.h`: Timer function prototypes

### Lab03: DAC (Digital-to-Analog Converter)
**Purpose:** Digital-to-analog conversion and signal generation
- **Features:**
  - MCP4822 12-bit DAC control via SPI
  - Voltage output control (0-4.095V range)
  - Timer-based signal generation
  - Real-time voltage display on LCD
- **Key Files:**
  - `src/main.c`: DAC and timer initialization
  - `src/lab03.c`: DAC control functions and timer ISR
  - `include/lab03.h`: DAC function prototypes
  - `Lab03_Matlab_Signal_Processing/`: MATLAB analysis tools
- **Additional Tools:**
  - MATLAB scripts for signal analysis
  - Signal data export/import capabilities

### Lab05: Touchscreen and Servo Control
**Purpose:** Advanced I/O with touchscreen input and servo motor control
- **Features:**
  - Resistive touchscreen reading (X/Y coordinates)
  - Dual servo control (X and Y axis)
  - PWM generation for servo positioning
  - Real-time position feedback
  - Touchscreen calibration and mapping
- **Key Files:**
  - `src/main.c`: Touchscreen and servo initialization
  - `src/lab05.c`: Touchscreen reading and servo control functions
  - `include/lab05.h`: Function prototypes

## Common Components

### Shared Libraries
All labs use common header files and libraries:
- `include/lcd.h`: LCD control library for Sparkfun Serial Graphic LCD
- `include/led.h`: LED control macros and functions
- `include/types.h`: Common type definitions and bit manipulation macros
- `lib/libLCD.a`: Precompiled LCD library

### Configuration
All projects are configured for:
- **Target Device:** dsPIC33FJ256MC710
- **Clock:** XT Crystal Oscillator with PLL (12.8 MHz)
- **Watchdog:** Disabled
- **Compiler:** XC16 with legacy libc

## Build Instructions

### Prerequisites
1. Install Microchip XC16 compiler
2. Ensure XC16 is in your system PATH
3. Install GNU Make (usually included with XC16)

### Building Individual Labs
```bash
# Navigate to specific lab directory
cd LabXX/LabXX_Project.X

# Build the project
make

# Clean build artifacts
make clean
```

### Output Files
Each lab produces:
- `.hex` file for programming
- `.cof` file for debugging
- Map file with memory usage information

## Programming the Device
1. Connect ICD3 or compatible programmer
2. Use MPLAB X or your preferred tool to flash the `.hex` file
3. Ensure proper hardware connections as per lab documentation

## Reference Documents
The `reference_documents/` directory contains:
- `dsPIC-datasheet.pdf`: Microcontroller datasheet
- `dsPIC-C-compiler.pdf`: XC16 compiler documentation
- `LCD-datasheet.pdf`: LCD display specifications
- `MCP4822-datasheet.pdf`: DAC chip documentation
- `Lab_manual.pdf`: General laboratory manual
- `dsPIC33F_components.zip`: Additional component documentation

## Notes
- All labs are designed for educational use
- Solutions (`.hex` files) are provided for reference
- MATLAB tools are included for Lab03 signal analysis
- Projects use consistent coding standards and naming conventions
- Each lab builds upon concepts from previous labs

## License
This project is intended for educational use. See individual source files for specific attributions and licensing information. 
