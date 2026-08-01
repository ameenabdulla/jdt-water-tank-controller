# 🌊 ESP32 Smart Water Tank Monitoring System & Web Dashboard

An end-to-end IoT Water Tank Level Monitoring and Automatic Motor Control System powered by ESP32, featuring an ultrasonic distance sensor, relay pump driver, NVS memory storage, and a real-time responsive web dashboard served directly from the ESP32 in standalone AP (Access Point) Mode or Router (STA) Mode.

---

## 🛠️ System Features

1. **Standalone Wi-Fi Access Point (AP Mode)**:
   - Connect directly to the ESP32's Wi-Fi network (`ESP32-WaterTank`) from any mobile phone, tablet, or laptop.
   - Access the dashboard at `http://192.168.4.1` with zero internet or router required!
2. **Optional Home Router Connection (STA Mode)**:
   - Save your home Wi-Fi SSID and Password in the Web Dashboard settings drawer for local network monitoring.
3. **HC-SR04 Ultrasonic Distance Sensor**:
   - Continuous water level measurement with Exponential Moving Average (EMA) digital noise filtering.
4. **Automatic Motor Relay Controller**:
   - **AUTO Mode**: Automatically switches motor ON when water drops below lower threshold (default `20%`) and OFF when reaching full threshold (default `95%`).
   - **MANUAL Mode**: Remote start/stop motor override directly from the web interface.
   - **Dry-Run Protection**: Safety cut-off if motor runs continuously for 5 minutes without water level rising (protects pump motor from burning out).
5. **Modern Glassmorphism Web UI**:
   - Fluid liquid wave visualizer matching exact water level percentage.
   - Live metrics: Water Level %, Depth (cm), Sensor Air Gap (cm), Estimated Volume (Liters), and Wi-Fi Signal Strength (dBm).
   - Dynamic threshold sliders & tank height calibration panel.

---

## 🔌 Hardware Circuit & Pinout

| Component | ESP32 Pin | Details |
| :--- | :--- | :--- |
| **HC-SR04 Trigger** | `GPIO 12` | Ultrasonic pulse trigger output |
| **HC-SR04 Echo** | `GPIO 13` | Ultrasonic echo input |
| **5V Relay In** | `GPIO 14` | High-current relay trigger pin (Active HIGH) |
| **Status LED** | `GPIO 2` | On-board LED indicator (Blinks in AP mode, Solid in STA mode) |
| **Factory Reset Button** | `GPIO 4` | Push button to GND (Hold 5s to clear saved Wi-Fi settings) |
| **VCC** | `5V / VIN` | Power supply for HC-SR04 & Relay module |
| **GND** | `GND` | Common Ground connection |

> **Note on HC-SR04 Echo Pin**: HC-SR04 operates at 5V logic. It is recommended to use a 1kΩ / 2kΩ resistor voltage divider on `Echo (GPIO 13)` to step 5V down to 3.3V for ESP32 pin safety.

---

## 🚀 How to Flash Firmware (Arduino IDE)

1. Open **Arduino IDE** (v2.x recommended).
2. Install **ESP32 Board Package**:
   - Go to `File` -> `Preferences`.
   - Add URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Go to `Tools` -> `Board` -> `Boards Manager`, search `esp32` and click **Install**.
3. Install required library:
   - Go to `Tools` -> `Manage Libraries`, search and install **`ArduinoJson`** (v6.x or v7.x).
4. Open the sketch:
   - Open `esp32_firmware/esp32_smart_water_tank/esp32_smart_water_tank.ino`.
5. Select Board & Port:
   - Board: `ESP32 Dev Module`
   - Upload Speed: `921600`
   - Port: Select your ESP32 COM port.
6. Click **Upload** ⚡.

---

## 📱 How to Connect & Use (AP Mode)

1. Once flashed, power up your ESP32.
2. Open Wi-Fi on your phone or laptop.
3. Connect to the network:
   - **SSID**: `ESP32-WaterTank`
   - **Password**: `12345678`
4. Open any browser (Chrome, Safari, Edge) and go to:
   - **`http://192.168.4.1`**
5. You will see the live liquid water tank dashboard!

---

## ⚙️ Calibration & Settings

- Click the **Settings (⚙️)** icon in the header.
- **Total Tank Height**: Set total distance from bottom of tank to sensor face (e.g. `100 cm`).
- **Sensor Offset**: Gap distance between sensor face and maximum full water level (e.g. `5 cm`).
- **Auto Min %**: Level at which pump automatically turns ON in AUTO mode.
- **Auto Max %**: Level at which pump automatically turns OFF in AUTO mode.
- Click **Save & Apply Settings**.
