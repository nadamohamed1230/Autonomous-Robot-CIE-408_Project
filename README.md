# 🚘 Autonomous 4WD Vehicle Platform

[![Python](https://img.shields.io/badge/Python-3.9+-blue.svg)](https://www.python.org/)
[![C](https://img.shields.io/badge/C-XC8-orange.svg)](https://www.microchip.com/)
[![ONNX](https://img.shields.io/badge/ONNX-Runtime-brightgreen.svg)](https://onnxruntime.ai/)
[![Firebase](https://img.shields.io/badge/Firebase-RTDB-yellow.svg)](https://firebase.google.com/)

> An enterprise-grade, dual-processor autonomous robotics platform featuring AI-driven obstacle avoidance, deterministic finite state machine (FSM) parallel parking, live 2D SLAM, and a fault-tolerant hardware safety layer.

<img width="1280" height="960" alt="WhatsApp Image 2026-05-21 at 7 10 01 PM" src="https://github.com/user-attachments/assets/cabf546e-0782-4257-a3ea-6be0abb12b22" />
<img width="1280" height="960" alt="WhatsApp Image 2026-05-21 at 7 10 03 PM" src="https://github.com/user-attachments/assets/7fce23da-6bd5-4668-a535-8f8158160a0b" />

Developed as the capstone project for **CIE 408: Embedded Systems** at **Zewail City of Science and Technology**.

---

## 📑 Table of Contents
- [Architecture Overview](#-architecture-overview)
- [Core Features](#-core-features)
- [Machine Learning Subsystems](#-machine-learning-subsystems)
- [Hardware Stack](#-hardware-stack)
- [Software Stack](#-software-stack)
-  [Team & Acknowledgments](#-team--acknowledgments)
- [Installation & Setup](#-installation--setup)


---

## 🏗 Architecture Overview

Autonomous systems require both high-level decision-making and microsecond-accurate hardware control. Standard Linux environments cannot guarantee the deterministic timing required for high-speed motor control. To solve this, the vehicle utilizes an asymmetrical **Dual-Processor Architecture**:

### 1. The Lower-Level Controller (PIC16F877A)
Operating entirely bare-metal in C, the PIC acts as the vehicle's "Spinal Cord." It handles microsecond-accurate ultrasonic echo timing, 10 kHz PWM generation, and hardware interrupts. Crucially, it manages **Un-interruptible Failsafes**: if the higher-level brain crashes, the PIC autonomously halts the chassis.

### 2. The High-Level Brain (Raspberry Pi 3)
A multi-threaded Python environment that acts as the strategic brain. It manages the Firebase cloud telemetry loop, processes NLP voice commands, runs the FSM for autonomous parking, and executes the spatial AI obstacle avoidance models.

---

## Core Features

* **Three-Tier Hardware Safety System:** * *Automatic Emergency Braking (AEB):* Overrides user commands if an object enters the 25cm critical zone.
  * *Dead Man's Switch:* An 800ms heartbeat timeout that instantly cuts motor power if the UART connection to the Raspberry Pi drops.
  * *Traction Control:* Wheel slip detection utilizing dual IR optocoupler encoders.
* **Autonomous Parallel Parking:** A precise 5-phase discrete Finite State Machine (Scanning, Overshoot, Angle Right, Reversing, Angle Left) optimized for differential-drive kinematics.
* **Voice-Controlled Mission Control:** A progressive web app (PWA) dashboard utilizing the Web Speech API and an NLP Intent Classifier to map conversational speech to vehicle commands.
* **Live 2D SLAM & Odometry:** Real-time dead-reckoning coordinate matrices tracking chassis displacement and plotting environmental boundaries on the remote dashboard.

---

##  Machine Learning Subsystems

This platform replaces hardcoded "if/then" driving logic with trained machine learning models:

1. **Isolation Forest (Anomaly Detection):** Monitors 5-dimensional feature matrices (wheel RPMs, voltages, command latency) to detect mechanical stalls or "kidnapped robot" scenarios.
2. **Random Forest Classifier (Obstacle Avoidance):** Trained on human driving telemetry, this model maps scaled proximity inputs from three ultrasonic arrays to optimal motion vectors.
3. **NLP Intent Classifier:** Parses unstructured audio transcripts from the dashboard, filtering out filler words to execute semantic movement targets (e.g., *"Car, please move forward"* → `CMD_FORWARD`).

---

##  Hardware Stack

* **Microcontroller:** Microchip PIC16F877A (Custom PCB implementation)
* **SBC:** Raspberry Pi 3 Model B (Running Raspberry Pi OS)
* **Actuators:** 4x 12V DC Gear Motors driven by an L298N Dual H-Bridge
* **Sensors:** * 3x HC-SR04 Ultrasonic Transducers (Front, Back, Right)
  * 2x LM393 IR Optocoupler Wheel Encoders
* **Power & Logic:** 12V Li-ion primary rail, LM2596 Buck Converter (5V logic), BSS138 Bi-directional Logic Level Converter (3.3V Pi ↔ 5V PIC).

---

## 💻 Software Stack

* **Embedded C:** XC8 Compiler, Bare-metal ISR management, Ring Buffers.
* **Python 3.9+:** `threading`, `pyserial`, `firebase-admin`, `curses` for terminal UI.
* **Data Science / ML:** `scikit-learn`, `numpy`, `onnxruntime` for edge inference.
* **Web Frontend:** HTML5, CSS3, Vanilla JavaScript, Firebase RTDB API, Web Speech API.

---

## Team & Acknowledgments
Project Lead:

Nada Mohamed — System Architecture, Cloud Integration, & ML Pipelines

Core Engineering Team:

Ashar Salama — AEB Systems & Motor Driver Development

Reem Tamer — Traction Control & PID Systems

Malak Osama — Communication Links & UART Protocols

Alya Marwan — Python Logic & Navigation FSM

Academic Advisor:

Dr. Hazem Fahmy — CIE 408 Course Instructor
## Installation & Setup

### 1. Clone the Repository
```bash
git clone [https://github.com/nadamohamed1230/Autonomous-Robot-CIE-408_Project.git](https://github.com/nadamohamed1230/Autonomous-Robot-CIE-408_Project.git)
cd Autonomous-Robot-CIE-408_Project
