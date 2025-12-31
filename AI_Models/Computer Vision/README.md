# 🚗 **AI Driver Behavior Monitor (ESP32-CAM)**

![Project Status](https://img.shields.io/badge/Status-Active-success)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino-green)
![ML Model](https://img.shields.io/badge/Model-MobileNetV1-orange)
![License](https://img.shields.io/badge/License-MIT-orange)

A smart, real-time driver monitoring system based on **ESP32-CAM** and **TensorFlow Lite Micro**. This project uses Edge AI to detect unsafe driving behaviors (drowsiness, phone usage, drinking, etc.) and provides a live video stream with status alerts via a Web Interface.

The code follows a modular **AUTOSAR-style** architecture for better maintainability and scalability.

---

## 📋 Table of Contents
- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Project Structure](#-project-structure)
- [Model Training & Architecture](#-model-training--architecture)
- [Installation & Setup](#-installation--setup)
- [Configuration](#-configuration)
- [How to Run](#-how-to-run)
- [Troubleshooting](#-troubleshooting)

---

## ✨ Features
* **Real-time Inference:** Runs TensorFlow Lite models directly on the ESP32 (Edge Computing).
* **10 Detected Behaviors:**
    * Safe driving
    * Texting (Left/Right)
    * Phone Call (Left/Right)
    * Operating Radio
    * Drinking
    * Reaching Behind
    * Hair & Makeup
    * Talking to Passenger
* **Web Dashboard:** Live MJPEG video stream with bounding boxes and prediction confidence.
* **Modular Code:** Organized into HAL (Hardware Abstraction Layer), AI Engine, and Network Manager.
* **Flexible Model Loading:** Supports loading the `.tflite` model from **SD Card** or Internal Flash.

---

## 🛠 Hardware Requirements
1.  **ESP32-CAM Board** (AI-Thinker Model recommended).
2.  **FTDI Programmer** (USB to TTL) for uploading code.
3.  **Micro SD Card** (Formatted to FAT32) - *Required if loading model from SD*.
4.  **Power Supply** (5V, at least 2A stable source).
5.  Jumper Wires.

---

## 📂 Project Structure
To compile successfully, your folder structure **must** look like this:

```text
CameraWebServer/
├── main/
│   ├── main.ino                  # Entry point
│   ├── app_httpd.cpp             # Web Server & Streaming Logic
│   ├── camera_pins.h             # Camera Pin Definitions
│   ├── board_config.h            # Board specific config
│   └── src/                      # Source Folder (Modules)
│       ├── Config.h              # System Configuration Macros
│       ├── Camera/
│       │   ├── Camera_HAL.h
│       │   └── Camera_HAL.cpp
│       ├── AI_Engine/
│       │   ├── AI_Engine.h
│       │   └── AI_Engine.cpp
│       └── Network_Manager/
│           ├── Network_Manager.h
│           └── Network_Manager.cpp