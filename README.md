# Prototyping-D4
Autonomous Line-Following & Obstacle-Avoidance Robot. Programmed in C++ with a focus on real-time sensor processing and custom 3D-printed hardware integration. HSHL Semester Project.

---

## Overview
This robot utilizes a combination of infrared and ultrasonic sensing to navigate a defined path while maintaining safety protocols for unexpected obstacles. A key highlight of this project is the **mechanical-electrical integration**, featuring custom 3D-printed components designed specifically for this sensor suite.

---

## Team Members
| Team Member |
|-------------|
| Ibrahim Abdelmalek|
| Ye Htet Kyaw |
| Dodly Forbi|
| Gabriel Tchakunte Tchouteng|
---

## Folders
| Name | Description |
|------------|------------|
| Lectures | Lectures and handout provided by the Professor|
| Firmware and Simulation | Arduino (.ino) source code & simulation files |
| CAD | .STL and .STEP files for 3D-printed parts |
| Diagrams| System Modelling Diagrams|
| Docs | Report and Presentation |
| Media | Project photos and demonstration videos |
---

## Tech Stack
* **Language:** C++ (Arduino Framework)
* **Hardware:** Arduino Uno, L298N Motor Driver
* **Sensors:** 2x IR Sensors (Line Tracking), 2x HC-SR04 (Ultrasonic)
* **Actuators:** 2x DC Geared Motors
* **Power:** LiPo Battery
* **Mechanical:** Custom CAD-designed 3D-printed holders

---

## Circuit Logic
The system operates on a feedback loop with the following logic priority:
1. **Safety:** If Ultrasonic sensors detect an object within range, the motor controller overrides navigation to stop/turn.
2. **Navigation:** If path is clear, IR sensors dictate differential steering to follow the line.
