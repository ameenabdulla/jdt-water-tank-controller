# ESP32 Smart Home Controller — Complete Setup & Upload Guide

This directory contains the firmware (`ESPSmartHome.ino`) and web assets (`data/` folder) to host the Smart Home Control Center directly from an ESP32 micro-controller.

---

## 📁 Directory Structure
```
ESPSmartHome/
├── ESPSmartHome.ino        <-- Main Arduino Firmware Sketch
├── UPLOAD_INSTRUCTIONS.md  <-- Setup Guide
└── data/                   <-- Files to upload to LittleFS
    ├── index.html
    ├── style.css
    ├── app.js
    └── esp32-ws.js
```

---

## ⚙️ 1. Required Arduino Libraries
Open **Arduino IDE** -> **Sketch** -> **Include Library** -> **Manage Libraries...** and install:

1. **`ESPAsyncWebServer`** by *lacamera* (or *me-no-dev*)
2. **`AsyncTCP`** by *dvarrel* (or *me-no-dev*)
3. **`ArduinoJson`** by *Benoit Blanchon* (Version 7.x recommended)
4. **`ESP32Servo`** by *Kevin Harrington*

> *Note: `WiFi.h`, `LittleFS.h`, and `Preferences.h` are included automatically with the ESP32 Arduino Board Package.*

---

## 📌 2. Pin Assignments & Hardware Connections

| Function | Pin Name | ESP32 GPIO | Description |
| :--- | :--- | :--- | :--- |
| **Light 1 Relay** | RELAY_LIGHT_1 | `GPIO 26` | Active-LOW relay module |
| **Light 2 Relay** | RELAY_LIGHT_2 | `GPIO 27` | Active-LOW relay module |
| **Light 3 Relay** | RELAY_LIGHT_3 | `GPIO 14` | Active-LOW relay module |
| **Light 4 Relay** | RELAY_LIGHT_4 | `GPIO 19` | Active-LOW relay module |
| **Light 5 Relay** | RELAY_LIGHT_5 | `GPIO 13` | Active-LOW relay module |
| **Water Pump Relay**| RELAY_PUMP | `GPIO 25` | Active-LOW relay module |
| **Gate Motor IN1** | MOTOR_IN1 | `GPIO 32` | L298N / L293D direction pin 1 |
| **Gate Motor IN2** | MOTOR_IN2 | `GPIO 33` | L298N / L293D direction pin 2 |
| **Servo Motor** | SERVO_PIN | `GPIO 18` | PWM Signal line |
| **Ultrasonic TRIG**| TRIG_PIN | `GPIO 5` | HC-SR04 Trigger pin |
| **Ultrasonic ECHO**| ECHO_PIN | `GPIO 4` | HC-SR04 Echo pin |
| **IR Beam / Sensor**| IR_PIN | `GPIO 35` | Digital Input (LOW = Object Detected) |
| **Gas Sensor** | GAS_PIN | `GPIO 34` | ADC1 Analog Input (MQ-2 / MQ-5) |
| **Flame Sensor** | FLAME_PIN | `GPIO 36` | ADC1 Analog Input (IR Flame Module) |

> ⚠️ **GPIO 12 Note**: GPIO 12 is a strapping pin. If your ESP32 fails to boot when powered with a relay connected to GPIO 12, change `RELAY_LIGHT[3]` in `ESPSmartHome.ino` to `GPIO 2` or `GPIO 15`.

---

## 🔧 3. Arduino IDE Board Settings

Go to **Tools** menu in Arduino IDE:
* **Board**: `"ESP32 Dev Module"` (or your specific ESP32 variant)
* **Upload Speed**: `921600` (or `115200` if upload fails)
* **CPU Frequency**: `240MHz (WiFi/BT)`
* **Flash Frequency**: `80MHz`
* **Flash Mode**: `QIO`
* **Flash Size**: `4MB (32Mb)`
* **Partition Scheme**: `"Default 4MB with spiffs"` or `"No OTA (2MB APP/2MB SPIFFS)"`
* **PSRAM**: `Disabled`

---

## 📤 4. Uploading Firmware (`.ino`)
1. Connect your ESP32 to your PC via USB cable.
2. Select the correct **Port** under **Tools** -> **Port**.
3. Click the **Upload** button (`Ctrl + U`).

---

## 📂 5. Uploading Web Files to LittleFS

To serve the web dashboard, you must upload the `data/` folder to the ESP32's flash memory.

### Method A: Arduino IDE 1.8.x
1. Download **`arduino-esp32fs-plugin`**: [GitHub Releases](https://github.com/lorol/arduino-esp32fs-plugin/releases)
2. Extract `esp32fs.jar` into `<Arduino>/tools/ESP32FS/tool/esp32fs.jar`.
3. Restart Arduino IDE.
4. Go to **Tools** -> **ESP32 LittleFS Data Upload**.

### Method B: Arduino IDE 2.x
1. Install **`arduino-littlefs-upload`** extension: [GitHub Repo](https://github.com/earlephilhower/arduino-littlefs-upload)
2. Press `Ctrl + Shift + P` -> Select **Upload LittleFS to ESP32**.

---

## 📶 6. Connecting to Smart Home Hotspot

1. Power ON the ESP32.
2. On your phone, tablet, or laptop, open Wi-Fi settings and connect to:
   * **SSID**: `SmartHome-IoT`
   * **Password**: `12345678`
3. Open any web browser and navigate to:
   * **URL**: `http://192.168.4.1`

---

## 🌐 7. REST API & WebSocket Protocol

### WebSocket Endpoint
`ws://192.168.4.1/ws` (Receives live telemetry every second, handles instant remote actions).

### REST Endpoints
* **`GET /api/status`**: Get current system state
* **`GET /api/settings`**: Read stored NVS configuration
* **`POST /api/settings`**: Save settings (`{"tankDepth":150, "sensorOffset":5, "lowThreshold":20, "highThreshold":90, "dashName":"My Home"}`)
* **`POST /api/light`**: Toggle lights (`{"id":0, "state":true}`)
* **`POST /api/gate`**: Control gate (`{"action":"open"}` | `{"action":"close"}` | `{"action":"stop"}` | `{"pos":50}`)
* **`POST /api/servo`**: Control servo (`{"on":true, "angle":90}`)
* **`POST /api/pump`**: Control water pump (`{"mode":"manual", "on":true}`)
