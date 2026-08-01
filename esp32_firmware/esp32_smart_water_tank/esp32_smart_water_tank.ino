/*
  ╔══════════════════════════════════════════════════════╗
  ║   JDT Islam Institute - Water Tank Controller        ║
  ║   Board    : ESP32 Dev Module                        ║
  ║   Reset Pin: GPIO 4 (Hold 5s to Reset WiFi)          ║
  ╚══════════════════════════════════════════════════════╝
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <WebSocketsClient.h>
#include <DNSServer.h>

// Pin Definitions
#define TRIG_PIN  12
#define ECHO_PIN  13
#define RELAY_PIN 14
#define LED_PIN   2
#define RESET_PIN 4   // Push button connected to GPIO 4 & GND

// Server Config
const char* WS_HOST = "jdt-water-tank-controller.onrender.com";
const uint16_t WS_PORT = 443;
const char* WS_PATH = "/ws";

// Tank Config
const float TANK_DEPTH_CM = 100.0;
const float SENSOR_OFFSET = 25.0;
const float AUTO_PUMP_ON   = 20.0;
const float AUTO_PUMP_OFF  = 90.0;

// Objects & State
WebSocketsClient webSocket;
WebServer        server(80);
Preferences      preferences;
DNSServer        dnsServer;

const byte DNS_PORT = 53;

String deviceName;
String ssid;
String password;
bool isAPMode = false;
float smoothedDistance = 0;
bool pumpState = false;
bool sensorError = false;
int failedReadings = 0;

unsigned long lastTelemetryTime = 0;
unsigned long apBlinkTimer = 0;
bool ledState = false;
unsigned long buttonPressTime = 0;
bool buttonHeld = false;

// Captive Portal HTML
const char INDEX_HTML[] PROGMEM = R"html(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>JDT Tank Setup</title>
  <style>
    body { font-family: sans-serif; background-color: #030712; color: #e0e0e0; text-align: center; margin: 0; padding: 20px; }
    h1 { color: #0ea5e9; }
    form { background-color: #0f172a; padding: 25px; border-radius: 16px; display: inline-block; border: 1px solid rgba(56,189,248,0.2); max-width: 90%; width: 320px; box-shadow: 0 20px 40px rgba(0,0,0,0.8); }
    input { display: block; margin: 15px auto; padding: 12px; width: calc(100% - 24px); border: 1px solid #334155; border-radius: 8px; box-sizing: border-box; background-color: #1e293b; color: #fff; }
    input:focus { outline: 2px solid #0ea5e9; }
    input[type=submit] { background: linear-gradient(135deg, #0ea5e9, #6366f1); color: #fff; font-weight: bold; cursor: pointer; margin-top: 20px; border: none; }
  </style>
</head>
<body>
  <h1>JDT Tank Setup</h1>
  <form action="/save" method="post">
    <input type="text" name="name" placeholder="Device Name" required>
    <input type="text" name="ssid" placeholder="WiFi SSID" required>
    <input type="password" name="pass" placeholder="WiFi Password" required>
    <input type="submit" value="Save & Connect">
  </form>
</body>
</html>
)html";

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleSave() {
  String newName = server.arg("name");
  String newSSID = server.arg("ssid");
  String newPass = server.arg("pass");
  
  preferences.begin("cfg", false);
  preferences.putString("name", newName);
  preferences.putString("ssid", newSSID);
  preferences.putString("pass", newPass);
  preferences.end();
  
  String html = F("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  html += F("<style>body{font-family:sans-serif;background-color:#030712;color:#22c55e;text-align:center;padding:50px;}</style></head>");
  html += F("<body><h1>Saved successfully!</h1><p style=\"color:#e0e0e0\">Connecting to WiFi...</p></body></html>");
  
  server.send(200, "text/html", html);
  delay(2000);
  ESP.restart();
}

void handleNotFound() {
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  server.send(302, "text/plain", "");
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 0.0;
  
  float distance = (duration * 0.0343) / 2.0;
  if (distance < 20.0 || distance > 450.0) return 0.0;
  return distance;
}

float toPercent(float dist) {
  float waterLevel = TANK_DEPTH_CM - (dist - SENSOR_OFFSET);
  if (waterLevel < 0) waterLevel = 0;
  if (waterLevel > TANK_DEPTH_CM) waterLevel = TANK_DEPTH_CM;
  return (waterLevel / TANK_DEPTH_CM) * 100.0;
}

void checkResetButton() {
  if (digitalRead(RESET_PIN) == LOW) {
    if (!buttonHeld) {
      buttonHeld = true;
      buttonPressTime = millis();
      Serial.println(F("Button on GPIO 4 held... Keep holding 5s to reset WiFi."));
    } else if (millis() - buttonPressTime > 5000) {
      Serial.println(F("Resetting WiFi preferences..."));
      preferences.begin("cfg", false);
      preferences.clear();
      preferences.end();
      
      for (int i = 0; i < 12; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(60);
        digitalWrite(LED_PIN, LOW);
        delay(60);
      }
      ESP.restart();
    }
  } else {
    buttonHeld = false;
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println(F("[WS] Disconnected"));
      digitalWrite(LED_PIN, LOW);
      break;
    case WStype_CONNECTED:
      Serial.println(F("[WS] Connected to Server!"));
      digitalWrite(LED_PIN, HIGH);
      webSocket.sendTXT("{\"type\":\"ping\"}");
      break;
    case WStype_TEXT: {
      String msg = (char*)payload;
      if (msg.indexOf("\"pumpOn\":true") > 0) {
        pumpState = true;
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println(F("Pump ON from Server"));
      } else if (msg.indexOf("\"pumpOn\":false") > 0) {
        pumpState = false;
        digitalWrite(RELAY_PIN, LOW);
        Serial.println(F("Pump OFF from Server"));
      }
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);
  
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  preferences.begin("cfg", true);
  deviceName = preferences.getString("name", "");
  ssid       = preferences.getString("ssid", "");
  password   = preferences.getString("pass", "");
  preferences.end();
  
  if (ssid == "") {
    isAPMode = true;
    Serial.println(F("Starting AP Mode: JDT-Tank-Setup"));
    WiFi.softAP("JDT-Tank-Setup", "12345678");
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    
    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println(F("Portal started at 192.168.4.1"));
  } else {
    isAPMode = false;
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print(F("Connecting to WiFi: "));
    Serial.println(ssid);
    
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
      delay(500);
      Serial.print(F("."));
      checkResetButton();
    }
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println(F("\nWiFi failed! Retrying in loop..."));
    } else {
      Serial.println(F("\nWiFi Connected!"));
      Serial.println(WiFi.localIP());
    }
    
    webSocket.beginSSL(WS_HOST, WS_PORT, WS_PATH);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(3000);
  }
}

void loop() {
  checkResetButton();
  
  if (isAPMode) {
    dnsServer.processNextRequest();
    server.handleClient();
    
    if (millis() - apBlinkTimer > 700) {
      apBlinkTimer = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);
    static unsigned long lastWiFiRetry = 0;
    if (millis() - lastWiFiRetry > 5000) {
      Serial.println(F("Reconnecting WiFi..."));
      WiFi.disconnect();
      WiFi.begin(ssid.c_str(), password.c_str());
      lastWiFiRetry = millis();
    }
  } else {
    webSocket.loop();
  }
  
  if (millis() - lastTelemetryTime > 250) {
    lastTelemetryTime = millis();
    
    float newDist = measureDistance();
    if (newDist > 0) {
      failedReadings = 0;
      sensorError = false;
      if (smoothedDistance == 0) {
        smoothedDistance = newDist;
      } else {
        smoothedDistance = (smoothedDistance * 0.6) + (newDist * 0.4);
      }
    } else {
      failedReadings++;
      if (failedReadings >= 5) {
        sensorError = true;
      }
    }
    
    float pct = toPercent(smoothedDistance);
    
    if (webSocket.isConnected()) {
      String json = "{\"type\":\"telemetry\",\"distanceCm\":";
      json += String(sensorError ? -1.0 : smoothedDistance, 2);
      json += ",\"sensorError\":";
      json += (sensorError ? "true" : "false");
      json += ",\"rssi\":";
      json += WiFi.RSSI();
      json += "}";
      webSocket.sendTXT(json);
    } else {
      if (sensorError) {
        if (pumpState) {
          pumpState = false;
          digitalWrite(RELAY_PIN, LOW);
          Serial.println(F("Local Safety: Sensor Disconnected! Emergency Pump OFF"));
        }
      } else {
        if (pct <= AUTO_PUMP_ON && !pumpState) {
          pumpState = true;
          digitalWrite(RELAY_PIN, HIGH);
          Serial.println(F("Local: Pump ON"));
        } else if (pct >= AUTO_PUMP_OFF && pumpState) {
          pumpState = false;
          digitalWrite(RELAY_PIN, LOW);
          Serial.println(F("Local: Pump OFF"));
        }
      }
    }
  }
}