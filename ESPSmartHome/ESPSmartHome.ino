/*
 * ======================================================================================
 * CYBERHOME OS — UNIFIED SINGLE-FILE ESP32 SMART HOME FIRMWARE
 * Built with standard WebServer.h & WebSocketsServer.h for 100% stable ESP32 web hosting
 * Includes: LittleFS Web Server, PROGMEM Fallback, 16x2 I2C LCD, Digital Gas Sensor
 * ======================================================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LittleFS.h>
#include "web_assets.h"

// --------------------------------------------------------------------------------------
// Hardware Pin Definitions
// --------------------------------------------------------------------------------------
static const uint8_t RELAY_LIGHTS[5] = { 26, 27, 14, 19, 13 }; // Active-LOW Relays
static const uint8_t RELAY_PUMP      = 25;                       // Active-LOW Pump Relay
static const uint8_t MOTOR_IN1       = 32;                       // Gate Motor IN1
static const uint8_t MOTOR_IN2       = 33;                       // Gate Motor IN2
static const uint8_t SERVO_PIN       = 18;                       // Gate Servo PWM
static const uint8_t SERVO2_PIN      = 17;                       // Auxiliary Servo 2 PWM
static const uint8_t TRIG_PIN        = 5;                        // Ultrasonic Trig
static const uint8_t ECHO_PIN        = 4;                        // Ultrasonic Echo
static const uint8_t IR_PIN          = 35;                       // IR Sensor (Digital Input)

// I2C LCD Pins for ESP32: SDA = GPIO 21, SCL = GPIO 22
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// --------------------------------------------------------------------------------------
// Network & System Configuration
// --------------------------------------------------------------------------------------
const char* AP_SSID = "SmartHome-IoT";
const char* AP_PASS = "12345678";
IPAddress   AP_IP(192, 168, 4, 1);
IPAddress   AP_SUBNET(255, 255, 255, 0);

DNSServer         dnsServer;
WebServer         server(80);
WebSocketsServer  webSocket(81);
Preferences       prefs;
Servo             gateServo;
Servo             servo2;

// Global System States
bool    lightStates[5]   = { false, false, false, false, false };
bool    pumpState        = false;
bool    pumpAutoMode     = true;
int     servoAngle       = 0;
int     servo2Angle      = 0;
int     waterDepth       = 100; // cm
int     waterOffset      = 5;   // cm
int     lowThreshold     = 20;  // %
int     highThreshold    = 90;  // %

// Sensor Reading Buffers
int     waterDistanceCm  = 0;
int     waterHeightCm    = 0;
int     waterPercentage  = 0;
bool    irDetected       = false;

// Event Timers
unsigned long irDetectTimeSec    = 0;
unsigned long lastSensorRead     = 0;
unsigned long lastBroadcast      = 0;
unsigned long gateAutoCloseTime  = 0;
bool          hasLCD             = false;

// Ultrasonic on Core 0 (FreeRTOS task - never blocks main loop)
float         smoothedDistance   = 0.0f; // Exponentially smoothed distance
SemaphoreHandle_t ultraMutex     = NULL; // Thread-safe access

// --------------------------------------------------------------------------------------
// Telemetry & WebSocket Broadcasting
// --------------------------------------------------------------------------------------
void broadcastState() {
  JsonDocument doc;
  doc["type"]            = "state";
  doc["pump_state"]      = pumpState;
  doc["pump_auto_mode"]  = pumpAutoMode;
  doc["servo_angle"]     = servoAngle;
  doc["water_distance"]  = waterDistanceCm;
  doc["water_height"]    = waterHeightCm;
  doc["water_percentage"]= waterPercentage;
  doc["ir_detected"]     = irDetected;
  // Lights array
  JsonArray lightsArr = doc["lights"].to<JsonArray>();
  for (int i = 0; i < 5; i++) lightsArr.add(lightStates[i]);
  // Nested objects
  JsonObject gateObj = doc["gate"].to<JsonObject>();
  gateObj["pos"]    = servoAngle;
  gateObj["status"] = (servoAngle > 10) ? "OPEN" : "CLOSED";
  JsonObject pumpObj = doc["pump"].to<JsonObject>();
  pumpObj["on"]   = pumpState;
  pumpObj["mode"] = pumpAutoMode ? "AUTO" : "MANUAL";
  JsonObject waterObj = doc["water"].to<JsonObject>();
  waterObj["distance"] = waterDistanceCm;
  waterObj["level"]    = waterPercentage;
  waterObj["height"]   = waterHeightCm;
  JsonObject irObj = doc["ir"].to<JsonObject>();
  irObj["val"]    = irDetected ? 1 : 0;
  irObj["status"] = irDetected ? "warning" : "safe";
  JsonObject settingsObj = doc["settings"].to<JsonObject>();
  settingsObj["waterDepth"]    = waterDepth;
  settingsObj["waterOffset"]   = waterOffset;
  settingsObj["lowThreshold"]  = lowThreshold;
  settingsObj["highThreshold"] = highThreshold;
  settingsObj["systemName"]    = "Smart Home";
  String output;
  serializeJson(doc, output);
  webSocket.broadcastTXT(output);
}

void updateLCDDisplay() {
  if (!hasLCD) return;
  lcd.setCursor(0, 0);
  lcd.print("Water: ");
  lcd.print(waterPercentage);
  lcd.print("%       ");
  lcd.setCursor(0, 1);
  lcd.print(pumpAutoMode ? "AUTO " : "MANU ");
  lcd.print(pumpState ? "PUMP:ON " : "PUMP:OFF");
}

// --------------------------------------------------------------------------------------
// Ultrasonic Task - Runs on Core 0 so it NEVER blocks Core 1 (WebSocket/Relay/HTTP)
// Uses exponential smoothing like JDT reference firmware: 60% old + 40% new
// --------------------------------------------------------------------------------------
void ultrasonicTask(void* param) {
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
  for (;;) {
    // Trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    // Measure echo (blocks Core 0 ONLY, max 30ms)
    long dur = pulseIn(ECHO_PIN, HIGH, 30000);
    if (dur > 0) {
      float dist = (dur * 0.0343f) / 2.0f;
      if (dist >= 2.0f && dist <= 450.0f) {
        xSemaphoreTake(ultraMutex, portMAX_DELAY);
        if (smoothedDistance < 1.0f) smoothedDistance = dist; // first read
        else smoothedDistance = (smoothedDistance * 0.6f) + (dist * 0.4f); // smooth
        xSemaphoreGive(ultraMutex);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(250)); // Measure every 250ms
  }
}

void readSensors() {
  // 1. Read smoothed distance from Core 0 ultrasonic task
  xSemaphoreTake(ultraMutex, portMAX_DELAY);
  float dist = smoothedDistance;
  xSemaphoreGive(ultraMutex);
  if (dist > 1.0f) {
    waterDistanceCm = (int)dist;
    waterHeightCm   = max(0, waterDepth - waterDistanceCm + waterOffset);
    waterPercentage = (int)constrain(map(waterHeightCm, 0, waterDepth, 0, 100), 0, 100);
  }

  // 2. IR Sensor (Active LOW when motion detected) - debounced
  static uint8_t irCount = 0;
  if (digitalRead(IR_PIN) == LOW) {
    if (irCount < 5) irCount++;
  } else {
    if (irCount > 0) irCount--;
  }
  bool currentIr = (irCount >= 5);
  if (currentIr && !irDetected) irDetectTimeSec = millis() / 1000;
  irDetected = currentIr;

  updateLCDDisplay();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %s\n", num, ip.toString().c_str());
      broadcastState();
      break;
    }
    case WStype_TEXT: {
      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, payload, length);
      if (!err) {
        String action = doc.containsKey("type") ? doc["type"].as<String>() : (doc.containsKey("action") ? doc["action"].as<String>() : "");

        if (action == "light") {
          int idx = doc.containsKey("id") ? (int)doc["id"] : (doc.containsKey("index") ? (int)doc["index"] : -1);
          bool state = doc.containsKey("state") ? doc["state"].as<bool>() : (doc.containsKey("on") ? doc["on"].as<bool>() : false);
          if (idx >= 0 && idx < 5) {
            lightStates[idx] = state;
            digitalWrite(RELAY_LIGHTS[idx], state ? LOW : HIGH); // INSTANT relay
            yield(); // yield to let WebSocket stack breathe
            Serial.printf("Relay %d (GPIO %d) -> %s\n", idx + 1, RELAY_LIGHTS[idx], state ? "ON" : "OFF");
          }
        } else if (action == "servo" || action == "servo2") {
          servo2Angle = constrain((int)doc["angle"], 0, 180);
          servo2.write(servo2Angle);
          Serial.printf("Auxiliary Servo 2 (GPIO 17) -> %d°\n", servo2Angle);
        } else if (action == "gate") {
          String cmd = doc.containsKey("cmd") ? doc["cmd"].as<String>() : (doc.containsKey("command") ? doc["command"].as<String>() : (doc.containsKey("action") ? doc["action"].as<String>() : ""));
          int pos = doc.containsKey("pos") ? (int)doc["pos"] : (doc.containsKey("position") ? (int)doc["position"] : -1);

          if (cmd == "open" || pos == 100) {
            servoAngle = 180;
            gateServo.write(180);
            gateAutoCloseTime = millis() + 8000; // Auto-close gate 8 seconds after opening!
            Serial.println("Smart Gate Servo -> OPEN (180°) [Auto-close in 8s]");
          } else if (cmd == "close" || pos == 0) {
            servoAngle = 0;
            gateServo.write(0);
            gateAutoCloseTime = 0;
            Serial.println("Smart Gate Servo -> CLOSED (0°)");
          } else if (cmd == "hold_open") {
            servoAngle = min(180, servoAngle + 10);
            gateServo.write(servoAngle);
            gateAutoCloseTime = 0;
            Serial.printf("Smart Gate Servo -> HOLD OPEN (%d°)\n", servoAngle);
          } else if (cmd == "hold_close") {
            servoAngle = max(0, servoAngle - 10);
            gateServo.write(servoAngle);
            gateAutoCloseTime = 0;
            Serial.printf("Smart Gate Servo -> HOLD CLOSE (%d°)\n", servoAngle);
          } else if (pos >= 0 && pos <= 100) {
            servoAngle = map(pos, 0, 100, 0, 180);
            gateServo.write(servoAngle);
            gateAutoCloseTime = 0;
            Serial.printf("Smart Gate Servo -> Position %d%% (%d°)\n", pos, servoAngle);
          }
        } else if (action == "pump" || action == "pump_mode" || action == "pump_toggle") {
          // Step 1: Update mode if provided (ALWAYS)
          if (doc.containsKey("mode")) {
            String m = doc["mode"].as<String>();
            bool newAuto = (m.equalsIgnoreCase("AUTO"));
            if (newAuto != pumpAutoMode) {
              pumpAutoMode = newAuto;
              prefs.putBool("pumpAuto", pumpAutoMode);
              Serial.printf("Pump Mode -> %s (saved permanently)\n", pumpAutoMode ? "AUTO" : "MANUAL");
            }
          }
          // Step 2: If MANUAL mode and a state/on key is present, toggle relay immediately
          if (!pumpAutoMode) {
            if (doc.containsKey("state")) {
              pumpState = doc["state"].as<bool>();
              digitalWrite(RELAY_PUMP, pumpState ? LOW : HIGH);
              Serial.printf("Pump Relay -> %s [MANUAL]\n", pumpState ? "ON" : "OFF");
            } else if (doc.containsKey("on")) {
              pumpState = doc["on"].as<bool>();
              digitalWrite(RELAY_PUMP, pumpState ? LOW : HIGH);
              Serial.printf("Pump Relay -> %s [MANUAL]\n", pumpState ? "ON" : "OFF");
            }
          }
        } else if (action == "settings") {
          if (doc.containsKey("waterDepth")) waterDepth = doc["waterDepth"];
          if (doc.containsKey("waterOffset")) waterOffset = doc["waterOffset"];
          if (doc.containsKey("lowThreshold")) lowThreshold = doc["lowThreshold"];
          if (doc.containsKey("highThreshold")) highThreshold = doc["highThreshold"];
        }
        broadcastState();
      }
      break;
    }
  }
}

void sendProgmemInChunks(const char* data, size_t len, const char* mimeType) {
  server.setContentLength(len);
  server.send(200, mimeType, "");
  size_t chunkSize = 1460;
  for (size_t i = 0; i < len; i += chunkSize) {
    size_t sz = (len - i < chunkSize) ? (len - i) : chunkSize;
    server.sendContent_P(data + i, sz);
  }
}

void handleRoot() {
  if (LittleFS.exists("/index.html") && LittleFS.open("/index.html", "r").size() > 500) {
    File f = LittleFS.open("/index.html", "r");
    server.streamFile(f, "text/html");
    f.close();
  } else {
    sendProgmemInChunks(INDEX_HTML, sizeof(INDEX_HTML) - 1, "text/html");
  }
}

void handleStyle() {
  if (LittleFS.exists("/style.css") && LittleFS.open("/style.css", "r").size() > 500) {
    File f = LittleFS.open("/style.css", "r");
    server.streamFile(f, "text/css");
    f.close();
  } else {
    sendProgmemInChunks(STYLE_CSS, sizeof(STYLE_CSS) - 1, "text/css");
  }
}

void handleAppJs() {
  if (LittleFS.exists("/app.js") && LittleFS.open("/app.js", "r").size() > 500) {
    File f = LittleFS.open("/app.js", "r");
    server.streamFile(f, "application/javascript");
    f.close();
  } else {
    sendProgmemInChunks(APP_JS, sizeof(APP_JS) - 1, "application/javascript");
  }
}

void handleWsJs() {
  if (LittleFS.exists("/esp32-ws.js") && LittleFS.open("/esp32-ws.js", "r").size() > 100) {
    File f = LittleFS.open("/esp32-ws.js", "r");
    server.streamFile(f, "application/javascript");
    f.close();
  } else {
    sendProgmemInChunks(ESP32_WS_JS, sizeof(ESP32_WS_JS) - 1, "application/javascript");
  }
}

void handleNotFound() {
  String path = server.uri();
  if (path.endsWith(".css")) {
    handleStyle();
  } else if (path.endsWith(".js")) {
    if (path.indexOf("esp32-ws") >= 0) handleWsJs();
    else handleAppJs();
  } else if (path == "/" || path.indexOf("index") >= 0) {
    handleRoot();
  } else {
    // Redirect captive portal requests (msftconnecttest.com, etc) to http://192.168.4.1/
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  }
}

// --------------------------------------------------------------------------------------
// Setup & Main Execution Loop
// --------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Smart Home IoT Controller Boot ===");

  prefs.begin("smarthome", false);
  pumpAutoMode = prefs.getBool("pumpAuto", true);

  // Initialize LCD Screen safely
  Wire.begin(21, 22);
  Wire.setTimeOut(100);
  Wire.beginTransmission(0x27);
  if (Wire.endTransmission() == 0) {
    hasLCD = true;
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CyberHome OS");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    Serial.println("LCD: Detected (0x27)");
  } else {
    Serial.println("LCD: Not Detected (0x27) - Proceeding");
  }

  // Initialize Output Relays
  for (int i = 0; i < 5; i++) {
    pinMode(RELAY_LIGHTS[i], OUTPUT);
    digitalWrite(RELAY_LIGHTS[i], HIGH);
  }
  pinMode(RELAY_PUMP, OUTPUT);
  digitalWrite(RELAY_PUMP, HIGH);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // TRIG/ECHO managed by FreeRTOS ultrasonicTask on Core 0
  pinMode(IR_PIN, INPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  gateServo.setPeriodHertz(50);
  gateServo.attach(SERVO_PIN, 500, 2400);
  gateServo.write(servoAngle);

  servo2.setPeriodHertz(50);
  servo2.attach(SERVO2_PIN, 500, 2400);
  servo2.write(servo2Angle);

  // Mount LittleFS
  if (LittleFS.begin(true)) {
    Serial.println("LittleFS: OK");
  } else {
    Serial.println("LittleFS: FAIL (Using Fallback)");
  }

  // Reset & Start AP Network
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(100);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(200);
  Serial.print("Wi-Fi AP: OK  IP: "); Serial.println(WiFi.softAPIP());

  dnsServer.start(53, "*", WiFi.softAPIP());

  // Set up HTTP WebServer routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/index.html", HTTP_GET, handleRoot);
  server.on("/style.css", HTTP_GET, handleStyle);
  server.on("/app.js", HTTP_GET, handleAppJs);
  server.on("/esp32-ws.js", HTTP_GET, handleWsJs);

  server.on("/generate_204", HTTP_GET, handleRoot);
  server.on("/fwlink", HTTP_GET, handleRoot);
  server.on("/canonical.html", HTTP_GET, handleRoot);
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot);
  server.on("/nui", HTTP_GET, handleRoot);
  server.on("/connecttest.txt", HTTP_GET, handleRoot);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("WebServer: OK (Port 80)");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocketsServer: OK (Port 81)");

  // Launch ultrasonic measurement on Core 0 (Core 1 = main loop, never blocked)
  ultraMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(
    ultrasonicTask,   // Task function
    "UltrasonicTask", // Name
    2048,             // Stack size
    NULL,             // Parameter
    1,                // Priority
    NULL,             // Handle
    0                 // Core 0
  );
  Serial.println("Ultrasonic: Core 0 Task Started (250ms, EXP smoothed)");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  webSocket.loop();

  // --- Auto Pump Logic: runs every loop tick for instant relay response ---
  if (pumpAutoMode) {
    if (waterPercentage > 0 && waterPercentage <= lowThreshold && !pumpState) {
      pumpState = true;
      digitalWrite(RELAY_PUMP, LOW);
      Serial.printf("Auto Pump ON  (Water %d%% <= Low %d%%)\n", waterPercentage, lowThreshold);
      broadcastState();
    } else if (waterPercentage >= highThreshold && pumpState) {
      pumpState = false;
      digitalWrite(RELAY_PUMP, HIGH);
      Serial.printf("Auto Pump OFF (Water %d%% >= High %d%%)\n", waterPercentage, highThreshold);
      broadcastState();
    }
  }

  // --- Auto-close gate 8 seconds after visitor entry ---
  if (gateAutoCloseTime > 0 && millis() > gateAutoCloseTime) {
    gateAutoCloseTime = 0;
    servoAngle = 0;
    gateServo.write(0);
    broadcastState();
    Serial.println("Smart Gate Servo -> Auto-Closed after 8s");
  }

  // --- Read IR/Gas/Flame sensors every 500ms ---
  if (millis() - lastSensorRead > 500) {
    lastSensorRead = millis();
    readSensors();
  }

  // --- Live broadcast every 250ms for smooth tank level updates ---
  if (millis() - lastBroadcast > 250) {
    lastBroadcast = millis();
    if (webSocket.connectedClients() > 0) {
      broadcastState();
    }
  }
}
