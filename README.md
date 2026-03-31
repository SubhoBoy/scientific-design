# Applied Digital Design and Scientific Computing: Arduino and Embedded C

This repository contains the curriculum, source code, and hardware schematics for a 2-week training program focused on digital design and embedded systems. The course bridges high-level Arduino programming and bare-metal microcontroller engineering (AVR-GCC). 

The curriculum is structured around the development of two primary hardware projects: a multiplexed 6-digit hardware clock and a scientific calculator capable of evaluating complex functions using numerical methods (Runge-Kutta 4) and expression parsing (Shunting Yard algorithm). The toolchain and development environment are optimized for mobile platforms using Android, Termux, and OTG serial connections.

## Learning Objectives

Upon completion of this program, participants will be able to:
* Configure and utilize an embedded C compilation toolchain (AVR-GCC, avrdude) within a mobile Linux environment.
* Derive Boolean logic equations using Karnaugh Maps and implement hardware state machines using D-Flip-Flop integrated circuits.
* Transition from hardware abstraction layers (Arduino C++) to bare-metal Embedded C, including direct memory mapping, bitwise operations, and custom driver development.
* Implement fundamental data structures (Stacks, Queues) and algorithms in resource-constrained microcontroller environments.
* Apply numerical methods, specifically the Runge-Kutta (RK4) method, to approximate ordinary differential equations (ODEs) for hardware-based evaluation of trigonometric and logarithmic functions.

## Prerequisites and Requirements

* An Android device with USB OTG support.
* **Microcontroller:** Arduino Uno with a USB OTG adapter.
* **Logic ICs:** 74LS74 (Dual D-Flip-Flop), 74LS47 (BCD to 7-Segment Decoder), and basic logic gates (AND/OR/NOT).
* **Displays:** Common-Anode 7-Segment Displays, 16x2 Character LCD.
* **Input Devices:** Standard tactile pushbuttons.
* **Components:** Breadboards, 220Ω and 10kΩ resistors, jumper wires.

## Curriculum Structure

The program is divided into 10 daily modules, each designed for approximately 3.5 to 4 hours of active development.

### Week 1: Digital Logic and Hardware Multiplexing
* **Day 1: Mobile Toolchain and Core I/O** - Configuration of the Termux/ArduinoDroid environment and implementation of basic hardware inputs/outputs.
* **Day 2: Combinational Logic and State Machines** - Boolean derivation, K-Maps, and the construction of a physical 0-5 counter using D-Flip-Flops and logic gates.
* **Day 3: Multiplexing and Hardware Timers** - Matrix wiring for the 6-digit display and the configuration of Timer1 interrupts for 10Hz persistence-of-vision multiplexing.
* **Day 4: Timekeeping Logic and Debouncing** - Implementation of cascading boolean time logic, non-blocking debounce routines, and user interface state machines.
* **Day 5: Bare-Metal C Transition** - Introduction to AVR memory mapping, bitwise operations, and direct port manipulation (PORTB, DDRB), replacing the Arduino framework.

### Week 2: Embedded C and Scientific Computing
* **Day 6: AVR-GCC and Interrupts** - Compiling C code in Termux, matrix keypad wiring, and the development of bare-metal Pin Change Interrupt (PCINT) drivers.
* **Day 7: Custom LCD Drivers and 8-Bit Architecture** - Development of a 4-bit LCD driver from scratch, analysis of floating-point limitations on 8-bit systems, and modeling mathematical functions as ODEs.
* **Day 8: Embedded Numerical Methods** - C implementation of the fixed-step Runge-Kutta (RK4) algorithm for trigonometric and logarithmic approximation, and the Fast Inverse Square Root algorithm.
* **Day 9: Data Structures and Parsing** - Memory management, array-based stacks, and the implementation of the Shunting Yard algorithm for Reverse Polish Notation (RPN) evaluation.
* **Day 10: System Integration** - Integration of the custom keypad driver, math parser, and LCD driver, followed by accuracy benchmarking against standard scientific calculators.

## Repository Navigation

Daily modules are organized into sequential directories (e.g., `day1`, `day2`). Each directory contains:
1. `main.pdf` - Module instructions, theory, and checkpoints.
2. `codes/` - Source code for the day's exercises.
3. `figs/` - Hardware schematics and breadboard wiring guides.

## License

This project is licensed under the [MIT License](https://choosealicense.com/licenses/mit/).
