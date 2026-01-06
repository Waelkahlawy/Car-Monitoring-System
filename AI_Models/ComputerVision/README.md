# 🚗 Driver Distraction Detection using Edge AI


An **AIoT (Artificial Intelligence of Things)** system that combines computer vision and IoT capabilities to detect driver distraction in real-time using an ESP32-CAM module. The system runs entirely on-device (edge computing) and sends instant alerts when dangerous driving behavior is detected.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Model Training](#model-training)
- [Deployment](#deployment)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Performance](#performance)
- [Troubleshooting](#troubleshooting)

## 🎯 Overview

This project implements a real-time driver distraction detection system that runs AI inference directly on an ESP32-CAM microcontroller. It classifies driver behavior into 10 categories (1 safe + 4 dangerous + 5 dangerous) and sends instant alerts via Telegram and MQTT when dangerous behavior is detected.

### Key Highlights

- **Edge AI**: Runs TensorFlow Lite Micro on ESP32-CAM (limited resources)
- **Real-time Detection**: Processes frames and makes predictions in milliseconds
- **Multi-channel Alerts**: Telegram notifications + MQTT dashboard + Web streaming
- **Optimized Model**: Quantized MobileNet (Int8) for embedded deployment
- **Dual-core Architecture**: FreeRTOS task management for optimal performance

## ✨ Features

- ✅ Real-time driver behavior classification (10 classes)
- ✅ On-device AI inference (no cloud dependency for detection)
- ✅ Instant photo alerts via Telegram bot
- ✅ MQTT integration for dashboard monitoring
- ✅ Live MJPEG streaming via web server
- ✅ Configurable alert cooldown (prevents spam)
- ✅ Power-optimized WiFi settings (prevents brownout)
- ✅ Memory management for stable long-term operation

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32-CAM Module                      │
├─────────────────────────────────────────────────────────┤
│  Core 1 (App Core)          │  Core 0 (Pro Core)        │
│  ─────────────────          │  ─────────────────        │
│  • Image Capture            │  • WiFi Management        │
│  • Preprocessing            │  • Telegram Bot           │
│  • TFLite Inference         │  • MQTT Client            │
│  • Decision Making          │  • Web Server             │
│  • JPEG Compression         │  • Alert Sending          │
└─────────────────────────────────────────────────────────┘
         │                              │
         └─────────────┬────────────────┘
                       │
            ┌──────────▼──────────┐
            │   Shared Memory     │
            │   (Global Buffer)   │
            └──────────┬──────────┘
                       │
         ┌─────────────▼───────────────┐
         │   Alert Channels            │
         ├─────────────────────────────┤
         │  • Telegram (Photos+Text)   │
         │  • MQTT (Dashboard Data)    │
         │  • Web Stream (Live View)   │
         └─────────────────────────────┘
```

## 🔧 Hardware Requirements

- **ESP32-CAM** module (AI Thinker or compatible)
- **FTDI Programmer** (for initial upload)
- **Power Supply**: 5V/2A minimum (stable power is crucial)
- **MicroSD Card** (optional, for logging)

### Camera Configuration

- Resolution: CIF (400x296) - optimal balance for AI
- Format: RGB565
- Auto Gain Control: Enabled
- Saturation: Reduced for better AI clarity

## 💻 Software Requirements

### Development Tools

- **Arduino IDE** 1.8.x or 2.x
- **ESP32 Board Manager** (latest version)
- **TensorFlow Lite Micro** for ESP32

### Python Environment (for training)

```bash
python >= 3.8
tensorflow >= 2.8
keras >= 2.8
numpy
opencv-python
matplotlib
scikit-learn
```

### Libraries (ESP32)

- ESP32 Camera driver
- WiFi libraries
- Telegram Bot API wrapper
- PubSubClient (MQTT)
- TensorFlow Lite Micro

## 📦 Installation

### 1. Clone the Repository

```bash
git clone https://github.com/Waelkahlawy/NOVA-CAR.git
cd driver-distraction-detection
```

### 2. Install Python Dependencies

```bash
pip install -r requirements.txt
```

### 3. Setup Arduino IDE

1. Add ESP32 board manager URL in Arduino IDE preferences
2. Install ESP32 board support
3. Install required libraries via Library Manager

### 4. Configure Credentials

Create a `config.h` file:

```cpp
// WiFi credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// Telegram Bot
#define BOT_TOKEN "your_telegram_bot_token"
#define CHAT_ID "your_chat_id"

// MQTT Broker
#define MQTT_SERVER "your_mqtt_broker"
#define MQTT_PORT 1883
#define MQTT_USER "your_username"
#define MQTT_PASS "your_password"
```

## 🧠 Model Training

### Dataset Preparation

The system uses the **State Farm Distracted Driver Detection** dataset with 10 classes:

- **c0**: Safe driving
- **c1**: Texting (right hand)
- **c2**: Talking on phone (right hand)
- **c3**: Texting (left hand)
- **c4**: Talking on phone (left hand)
- **c5**: Operating radio
- **c6**: Drinking
- **c7**: Reaching behind
- **c8**: Hair and makeup
- **c9**: Talking to passenger

### Training Pipeline

```bash
cd training
python train_model.py --input_size 96 --epochs 50 --batch_size 32
```

### Model Optimization

```bash
python convert_to_tflite.py --model model.h5 --quantize int8
python generate_c_array.py --input model.tflite --output model_data.h
```

**Optimization Steps:**

1. **Transfer Learning**: MobileNetV2 backbone (pre-trained)
2. **Fine-tuning**: Custom head for 10 classes
3. **Conversion**: Keras → TensorFlow Lite
4. **Quantization**: Float32 → Int8 (4x size reduction)
5. **C-Array Generation**: Embedding in firmware

## 🚀 Deployment

### 1. Flash the Firmware

1. Open `driver_distraction_detection.ino` in Arduino IDE
2. Select **Board**: "AI Thinker ESP32-CAM"
3. **Upload** the code

### 2. Initial Boot

After first boot, the system will:
- Connect to WiFi
- Initialize camera with optimal settings
- Load AI model into memory
- Start dual-core tasks
- Begin monitoring

### 3. Monitor Serial Output

```
[BOOT] Driver Distraction Detection v1.0
[WIFI] Connecting to WiFi...
[WIFI] Connected! IP: 192.168.1.100
[CAM] Camera initialized: CIF (400x296)
[AI] Model loaded: 10 classes, Int8 quantized
[MQTT] Connected to broker
[READY] System operational
```

## 📱 Usage

### Telegram Bot Commands

- `/start` - Initialize bot
- `/status` - Check system status
- `/stream` - Get live stream URL
- `/sensitivity [1-10]` - Adjust detection threshold

### Web Interface

Access the live stream:
```
http://<ESP32_IP_ADDRESS>/stream
```

### MQTT Topics

- `driver/status` - System health
- `driver/prediction` - Real-time classification
- `driver/confidence` - Prediction confidence score
- `driver/alert` - Distraction alerts

## 🔬 Technical Details

### AI Inference Flow

```
Raw Frame (RGB565) 
    ↓
Resize to Model Input Size (128×128)
    ↓
Color Conversion & Normalization
    ↓
TFLite Micro Inference
    ↓
Softmax Probabilities [10 classes]
    ↓
ArgMax → Class Index
    ↓
Decision: Safe (c0) or Danger (c1-c9)
    ↓
If Danger → JPEG Compression → Alert
```

### Memory Management

- **PSRAM**: Enabled for frame buffers
- **Dynamic Allocation**: JPEG buffers freed immediately after sending
- **Shared Buffers**: Efficient inter-task communication
- **Stack Sizes**: Carefully tuned per task

### Power Management

```cpp
WiFi.setTxPower(WIFI_POWER_19dBm); // Prevent brownout
```

### Task Architecture (FreeRTOS)

| Task | Core | Priority | Stack | Function |
|------|------|----------|-------|----------|
| Task_AI | 1 | High | 8KB | AI inference & preprocessing |
| Task_Comm | 0 | Normal | 4KB | WiFi, alerts, streaming |

## 📊 Performance

- **Inference Time**: ~200-400ms per frame
- **Accuracy**: ~94.35% on validation set
- **Model Size**: 1.03MB (quantized)
- **RAM Usage**: ~180KB (peak)
- **Power Consumption**: ~500mA @ 5V
- **Alert Latency**: <1 second (detection to notification)

## 🐛 Troubleshooting

### Camera Initialization Failed

- Check power supply (needs stable 5V/2A)
- Verify camera ribbon cable connection
- Try different resolution (QVGA)

### Brownout Detector Reset

```cpp
// Reduce WiFi power
WiFi.setTxPower(WIFI_POWER_15dBm);
```

### Out of Memory Errors

- Reduce image resolution
- Decrease JPEG quality
- Increase alert cooldown time

### False Positives

- Retrain model with more diverse data
- Adjust confidence threshold
- Improve lighting conditions


