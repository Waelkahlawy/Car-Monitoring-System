# 🚗 AI Driver Behavior Monitor (ESP32-CAM)

![Project Status](https://img.shields.io/badge/Status-Active-success)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino-green)
![ML Model](https://img.shields.io/badge/Model-MobileNetV1-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

A real-time edge AI system for monitoring driver behavior using **ESP32-CAM** and **TensorFlow Lite Micro**. The system detects unsafe driving patterns such as drowsiness, phone usage, and other distractions, providing live video streaming with intelligent alerts through an intuitive web interface.

Built with a modular **AUTOSAR-inspired** architecture for enhanced maintainability and scalability.

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Key Features](#-key-features)
- [Detected Behaviors](#-detected-behaviors)
- [Hardware Requirements](#-hardware-requirements)
- [Software Architecture](#-software-architecture)
- [AI Model Pipeline](#-ai-model-pipeline)
- [Installation Guide](#-installation-guide)
- [Configuration](#-configuration)
- [Usage Instructions](#-usage-instructions)
- [Troubleshooting](#-troubleshooting)
- [Performance Metrics](#-performance-metrics)
- [Future Enhancements](#-future-enhancements)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🎯 Overview

This project implements an intelligent driver monitoring system that leverages edge computing to detect dangerous driving behaviors in real-time. By running AI inference directly on the ESP32 microcontroller, the system ensures:

- **Low Latency:** Instant response without cloud dependency
- **Privacy:** All processing happens on-device
- **Cost-Effective:** No recurring cloud service fees
- **Offline Operation:** Works without internet connectivity

---

## ✨ Key Features

### Core Capabilities

- **Real-Time AI Inference:** TensorFlow Lite models run directly on ESP32 (Edge Computing)
- **Live Video Streaming:** MJPEG stream with overlaid detection results
- **Web Dashboard:** Intuitive interface displaying confidence scores and behavior status
- **Modular Architecture:** Clean separation of HAL, AI Engine, and Network layers
- **Flexible Model Storage:** Supports both SD Card and internal flash storage
- **Low Power Consumption:** Optimized for embedded deployment
- **High Accuracy:** MobileNet-based model with transfer learning

### Technical Highlights

- **Quantized INT8 Model:** Optimized for microcontroller inference
- **PSRAM Utilization:** Efficient memory management for large models
- **Async Processing:** Non-blocking inference and streaming
- **OTA Ready:** Partition scheme supports future updates

---

## 🎭 Detected Behaviors

The system classifies driver activity into **10 categories**:

| Category | Description | Risk Level |
|----------|-------------|------------|
| **c0** | Safe Driving | ✅ Safe |
| **c1** | Texting - Right Hand | 🔴 High Risk |
| **c2** | Phone Call - Right Hand | 🟡 Medium Risk |
| **c3** | Texting - Left Hand | 🔴 High Risk |
| **c4** | Phone Call - Left Hand | 🟡 Medium Risk |
| **c5** | Operating Radio | 🟡 Medium Risk |
| **c6** | Drinking | 🟠 Moderate Risk |
| **c7** | Reaching Behind | 🔴 High Risk |
| **c8** | Hair & Makeup | 🟠 Moderate Risk |
| **c9** | Talking to Passenger | 🟢 Low Risk |

---

## 🛠 Hardware Requirements

### Essential Components

| Component | Specification | Notes |
|-----------|--------------|-------|
| **Microcontroller** | ESP32-CAM (AI-Thinker) | Must have PSRAM |
| **Camera** | OV2640 (included) | 2MP resolution |
| **SD Card** | Micro SD (8GB+) | FAT32 formatted |

### Optional Components

- External antenna for better WiFi range
- External LED for status indication
- Case/enclosure for mounting



## 📂 Software Architecture

### Project Structure

```text

main/
│   ├── main.ino                    # Entry point & setup
│   ├── app_httpd.cpp               # HTTP server & MJPEG streaming
│   ├── camera_pins.h               # Board-specific pin definitions
│   ├── board_config.h              # Hardware configuration
│   └── src/                        # Modular source code
│       ├── Config.h                # System-wide configuration
│       ├── Camera/
│       │   ├── Camera_HAL.h        # Camera hardware abstraction
│       │   └── Camera_HAL.cpp
│       ├── AI_Engine/
│       │   ├── AI_Engine.h         # TensorFlow Lite inference
│       │   └── AI_Engine.cpp
│       └── Network_Manager/
│           ├── Network_Manager.h   # WiFi & web server management
│           └── Network_Manager.cpp
```

### Architecture Layers

**Hardware Abstraction Layer (HAL)**
- Encapsulates camera and SD card operations
- Provides hardware-agnostic interfaces
- Simplifies hardware replacement or upgrades

**AI Engine Layer**
- Manages TensorFlow Lite Micro interpreter
- Handles model loading and preprocessing
- Executes inference and post-processing

**Network Manager Layer**
- Manages WiFi connectivity
- Serves web interface
- Streams video with detection overlays

---

## 🧠 AI Model Pipeline

### Training Dataset

**Source:** State Farm Distracted Driver Detection Dataset
- **Total Images:** ~22,000+ labeled images
- **Split:** 80% training, 20% validation
- **Resolution:** 128×128 pixels (resized)

### Data Preprocessing

**Normalization**
```python
rescale=1./255  # Pixel values normalized to [0, 1]
```

**Augmentation Techniques**
- Rotation: ±10 degrees
- Width/Height Shift: ±10%
- Brightness Range: 0.6 to 1.4
- Horizontal Flip: Random
- Fill Mode: Nearest neighbor

### Model Architecture

**Backbone: MobileNetV1 (α=0.5)**
- Pretrained on ImageNet
- Frozen base layers preserve learned features
- Lightweight design for embedded deployment

**Custom Classification Head**
```
Input (128×128×3)
    ↓
MobileNet Base (frozen)
    ↓
GlobalAveragePooling2D
    ↓
Dense(128, activation='relu')
    ↓
Dropout(0.3)
    ↓
Dense(10, activation='softmax')
    ↓
Output (10 classes)
```

**Total Parameters:** ~470K (after quantization: ~120KB)

### Training Configuration

| Parameter | Value |
|-----------|-------|
| **Optimizer** | Adam |
| **Loss Function** | Categorical Crossentropy |
| **Initial Learning Rate** | 0.001 |
| **Batch Size** | 32 |
| **Total Epochs** | 45 (3 cycles × 15) |
| **Validation Split** | 20% |

### Model Optimization for ESP32

**INT8 Quantization Process**
1. **Full Integer Quantization:** Converts all weights and activations to 8-bit integers
2. **Representative Dataset:** Uses validation images to calibrate quantization ranges
3. **Input/Output Types:** Forced to UINT8 for TFLite Micro compatibility

**Conversion Code**
```python
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = representative_data_gen
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.uint8
converter.inference_output_type = tf.uint8
```

**Performance Impact**
- Model Size: ~4.224MB → ~1.03MB (74% reduction)
- Inference Time: ~180ms per frame
- Accuracy Loss: <3% compared to float32

---

## 📥 Installation Guide

### Step 1: Prepare SD Card

1. Format SD card to **FAT32** (not exFAT)
2. Copy your trained model file to the root directory
3. Rename the file to: `model.tflite` (exact name required)
4. Insert SD card into ESP32-CAM slot

### Step 2: Install Arduino IDE

**Download Arduino IDE**
- Version 1.8.19+ or Arduino IDE 2.x
- Download from: [arduino.cc](https://www.arduino.cc/en/software)

**Install ESP32 Board Support**
1. Open Arduino IDE
2. Go to: `File → Preferences`
3. Add to "Additional Boards Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to: `Tools → Board → Boards Manager`
5. Search: "esp32"
6. Install: **esp32 by Espressif Systems** (v2.0.11 recommended)

### Step 3: Install Required Libraries

All libraries are included with ESP32 core:
- `WiFi` - WiFi connectivity
- `FS` - File system operations
- `SD_MMC` - SD card interface
- `esp_camera` - Camera driver

### Step 4: Configure Board Settings

In Arduino IDE, select:

| Setting | Value |
|---------|-------|
| **Board** | AI Thinker ESP32-CAM |
| **Upload Speed** | 115200 |
| **CPU Frequency** | 240MHz (WiFi/BT) |
| **Flash Frequency** | 80MHz |
| **Flash Mode** | QIO |

⚠️ **Critical Settings:**
- **Partition Scheme** must be "Huge APP" to fit the AI model
- **PSRAM** must be enabled for model inference

---

## 🔧 Configuration

### WiFi Settings

Edit `src/Config.h`:

```cpp
// WiFi Credentials
#define CFG_WIFI_SSID    "YourNetworkName"
#define CFG_WIFI_PASS    "YourPassword"

// Connection Settings
#define CFG_WIFI_TIMEOUT_MS  10000  // 10 seconds
#define CFG_WIFI_RETRY_COUNT 3
```

### Model Loading Configuration

**Option 1: Load from SD Card (Recommended)**
```cpp
#define CFG_MODEL_LOAD_FROM_SD  STD_ON
#define CFG_MODEL_PATH          "/model.tflite"
```

**Option 2: Load from Internal Flash**
```cpp
#define CFG_MODEL_LOAD_FROM_SD  STD_OFF
// Include model_data.h with uint8_t model_data[] array
```

### Camera Settings

```cpp
// Image Configuration
#define CFG_CAMERA_FRAME_SIZE   FRAMESIZE_QVGA  // 320x240
#define CFG_CAMERA_JPEG_QUALITY 12              // 0-63, lower = better

// Processing Configuration
#define CFG_INFERENCE_INTERVAL_MS  500  // Run AI every 500ms
```

### Advanced Settings

```cpp
// Memory Management
#define CFG_ENABLE_PSRAM        STD_ON
#define CFG_FRAMEBUFFER_COUNT   2

// Web Server
#define CFG_WEBSERVER_PORT      80
#define CFG_STREAM_PATH         "/stream"

// Debug Options
#define CFG_ENABLE_SERIAL_DEBUG STD_ON
#define CFG_SERIAL_BAUD_RATE    115200
```

---

## 🚀 Usage Instructions

### Initial Upload Process

**1. Enter Upload Mode**
- Connect **GPIO 0 to GND** using jumper wire
- Connect FTDI programmer as shown in Hardware section
- Ensure 5V power supply is stable

**2. Upload Firmware**
```
1. Open CameraWebServer.ino in Arduino IDE
2. Verify board settings (see Installation Guide)
3. Click Upload button (→)
4. Wait for "Connecting..." message
5. Press RESET button on ESP32-CAM
6. Wait for "Writing at 0x..." messages
7. Upload complete when "Hard resetting..." appears
```

**3. Enter Run Mode**
- Disconnect GPIO 0 from GND
- Press RESET button
- Device will now run normally

### Running the System

**1. Monitor Serial Output**
```
1. Open Serial Monitor (Tools → Serial Monitor)
2. Set baud rate to 115200
3. Press RESET on ESP32-CAM
```

**Expected Output:**
```
[CAMERA] Initializing camera...
[CAMERA] Camera initialized successfully
[SD] Mounting SD card...
[SD] SD card mounted
[AI] Loading model from SD card...
[AI] Model loaded: 589312 bytes
[AI] Input tensor: 128x128x3, type: UINT8
[AI] Output tensor: 10 classes
[WiFi] Connecting to YourNetwork...
[WiFi] Connected! IP: 192.168.1.15
[WEB] Server started on http://192.168.1.15
```

**2. Access Web Dashboard**
- Open browser
- Navigate to the IP address shown (e.g., `http://192.168.1.15`)
- Click **"Start Stream"** button

**3. View Detection Results**
- Live video feed displays in browser
- Detected behavior shown with confidence percentage
- Color-coded status indicators:
  - 🟢 Green: Safe driving
  - 🟡 Yellow: Medium risk
  - 🔴 Red: High risk

### Web Interface Features

- **Live Stream:** Real-time MJPEG video at ~10-15 FPS
- **Detection Overlay:** Bounding boxes and labels
- **Confidence Score:** Percentage for each prediction
- **Status Bar:** Current behavior and risk level
- **Settings Panel:** Adjust inference frequency and quality

---

## ❓ Troubleshooting

### Common Issues & Solutions

#### Camera Initialization Failed

**Symptoms:**
```
[CAMERA] Camera init failed with error 0x20001
```

**Solutions:**
1. Check camera ribbon cable is fully inserted
2. Verify PSRAM is enabled in board settings
3. Try different partition scheme
4. Restart ESP32-CAM
5. Test with known-good camera module

---

#### SD Card Mount Failed

**Symptoms:**
```
[SD] Card Mount Failed
```

**Solutions:**
1. Format SD card to FAT32 (not exFAT or NTFS)
2. Use SD card ≤8GB (better compatibility)
3. Check SD card is fully inserted
4. Clean SD card contacts
5. Try different SD card
6. Check SD_MMC pins are not damaged

---


#### Model Loading Error

**Symptoms:**
```
[AI] Failed to load model from SD
```

**Solutions:**
1. Verify file is named exactly `model.tflite`
2. Check file is in SD card root directory
3. Ensure model is TFLite INT8 format
4. Try re-converting model with correct settings
5. Check file is not corrupted (redownload)

---

#### WiFi Connection Failed

**Symptoms:**
```
[WiFi] Connection failed. Retrying...
```

**Solutions:**
1. Verify SSID and password in Config.h
2. Check 2.4GHz WiFi (ESP32 doesn't support 5GHz)
3. Move closer to WiFi router
4. Attach external antenna if available
5. Check router allows new device connections
6. Disable MAC filtering temporarily

---

#### Low Frame Rate / Lag

**Symptoms:**
- Stream updates slowly
- High latency between frames

**Solutions:**
1. Reduce JPEG quality in Config.h
2. Decrease frame size (use QVGA instead of VGA)
3. Increase inference interval (run AI less frequently)
4. Use stronger WiFi signal
5. Reduce number of connected clients
6. Disable debug serial output

---

#### Compilation Errors

**Error: Undefined Reference**
```
undefined reference to `AI_Engine_Init'
```

**Solutions:**
1. Verify folder structure matches documentation
2. Ensure all .cpp files are in correct directories
3. Close and reopen Arduino IDE
4. Clear Arduino cache: Delete `/tmp/arduino_*` folders
5. Reinstall ESP32 board support

**Error: Not Enough Memory**
```
Sketch uses 120% of program storage space
```

**Solutions:**
1. Select "Huge APP" partition scheme
2. Remove unused libraries
3. Reduce model size if possible
4. Optimize code to reduce size

---

#### Model Accuracy Issues

**Symptoms:**
- Incorrect classifications
- Low confidence scores
- Erratic predictions

**Solutions:**
1. Ensure proper lighting conditions
2. Position camera at correct angle
3. Verify model is properly quantized
4. Check input preprocessing matches training
5. Retrain model with more diverse data
6. Increase inference interval to reduce noise

---

### Debug Mode

Enable detailed logging by editing `Config.h`:

```cpp
#define CFG_ENABLE_SERIAL_DEBUG  STD_ON
#define CFG_DEBUG_LEVEL          3  // 0=None, 1=Error, 2=Warning, 3=Info
```

This will print detailed information about each stage of operation.

---

## 📊 Performance Metrics

### Inference Performance

| Metric | Value |
|--------|-------|
| **Inference Time** | ~180ms per frame |
| **FPS (with AI)** | ~5-6 FPS |
| **FPS (stream only)** | ~15 FPS |
| **Model Size** | 589KB (INT8) |
| **Memory Usage** | ~2MB PSRAM |

### Accuracy Metrics

| Metric | Value |
|--------|-------|
| **Training Accuracy** | 87.03% |
| **Validation Accuracy** | 91.25% |
| **Test Accuracy** | 88% |

### Power Consumption

| Mode | Current Draw |
|------|--------------|
| **Idle** | ~180mA |
| **Streaming Only** | ~300mA |
| **AI + Streaming** | ~450mA |

---

## 🚀 Future Enhancements

### Planned Features

- [ ] **Multi-face Detection:** Track multiple occupants
- [ ] **Alert System:** Buzzer/LED warnings for dangerous behaviors
- [ ] **Data Logging:** Save detection events to SD card
- [ ] **OTA Updates:** Wireless firmware updates
- [ ] **Mobile App:** Companion Android/iOS app
- [ ] **Cloud Integration:** Optional telemetry upload
- [ ] **Advanced Models:** Emotion detection, gaze tracking
- [ ] **Night Vision:** IR LED support for low-light operation

### Model Improvements

- [ ] Implement temporal smoothing (LSTM/GRU)
- [ ] Add attention mechanisms
- [ ] Experiment with EfficientNet-Lite
- [ ] Multi-task learning (behavior + attention level)

---

## 🤝 Contributing

We welcome contributions! Please follow these guidelines:

### How to Contribute

1. **Fork the Repository**
2. **Create Feature Branch:** `git checkout -b feature/AmazingFeature`
3. **Commit Changes:** `git commit -m 'Add AmazingFeature'`
4. **Push to Branch:** `git push origin feature/AmazingFeature`
5. **Open Pull Request**

### Code Standards

- Follow existing code style and architecture
- Add comments for complex logic
- Update documentation for new features
- Test thoroughly on actual hardware

### Reporting Issues

When reporting bugs, please include:
- ESP32 board model
- Arduino IDE version
- Full serial monitor output
- Steps to reproduce

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

### MIT License Summary

- ✅ Commercial use allowed
- ✅ Modification allowed
- ✅ Distribution allowed
- ✅ Private use allowed
- ⚠️ No liability
- ⚠️ No warranty

---

## 🙏 Acknowledgments

- **Dataset:** State Farm Distracted Driver Detection (Kaggle)
- **TensorFlow Team:** TensorFlow Lite Micro framework
- **ESP32 Community:** esp32-camera library and examples
- **Espressif Systems:** ESP32 platform and documentation

---

## 📞 Support

For questions and support:

- **Issues:** [GitHub Issues](https://github.com/Enmo7)

---

