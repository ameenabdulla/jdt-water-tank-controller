/*
 * JDT Water Tank Controller Firmware
 * Built for ESP32 Dev Module
 * Designed by Easy Electro Tech
 */

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <WebSocketsClient.h>
#include <DNSServer.h>

// Pins
#define TRIG_PIN 12
#define ECHO_PIN 13
#define RELAY_PIN 14
#define LED_PIN 2
#define RESET_PIN 4

// Server
const char* WS_HOST = "jdt-water-tank-controller.onrender.com";
const uint16_t WS_PORT = 443;
const char* WS_PATH = "/ws";

// Tank Config
const float TANK_DEPTH_CM = 100.0;
const float SENSOR_OFFSET = 25.0;
const float AUTO_PUMP_ON = 20.0;
const float AUTO_PUMP_OFF = 90.0;

// State
String deviceName;
String ssid;
String password;
bool isAPMode = false;
float smoothedDistance = 0;
bool pumpState = false;
unsigned long lastTelemetryTime = 0;
unsigned long apBlinkTimer = 0;
bool ledState = false;

// Button state
unsigned long buttonPressTime = 0;
bool buttonHeld = false;

Preferences preferences;
WebServer server(80);
WebSocketsClient webSocket;
DNSServer dnsServer;
const byte DNS_PORT = 53;

// AP Portal HTML
const char INDEX_HTML[] PROGMEM = R"html(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>EET Device Setup</title>
  <style>
    body { font-family: sans-serif; background-color: #121212; color: #e0e0e0; text-align: center; margin: 0; padding: 20px; }
    h1 { color: #03dac6; }
    form { background-color: #1e1e1e; padding: 20px; border-radius: 10px; display: inline-block; box-shadow: 0 4px 8px rgba(0,0,0,0.5); max-width: 90%; width: 300px;}
    input { display: block; margin: 15px auto; padding: 12px; width: calc(100% - 24px); border: none; border-radius: 5px; box-sizing: border-box; background-color: #2d2d2d; color: #fff;}
    input:focus { outline: 2px solid #bb86fc; }
    input[type=submit] { background-color: #bb86fc; color: #000; font-weight: bold; cursor: pointer; margin-top: 20px; }
    input[type=submit]:hover { background-color: #9c27b0; }
  </style>
</head>
<body>
  <h1>EET Device Setup</h1>
  <form action="/save" method="post">
    <input type="text" name="name" placeholder="Device Name" required>
    <input type="text" name="ssid" placeholder="WiFi SSID" required>
    <input type="password" name="pass" placeholder="WiFi Password" required>
    <input type="submit" value="Save & Restart">
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
  html += F("<style>body{font-family:sans-serif;background-color:#121212;color:#03dac6;text-align:center;padding:50px;}</style></head>");
  html += F("<body><h1>Saved successfully!</h1><p style=\"color:#e0e0e0\">Rebooting device...</p></body></html>");
  
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
  
  return distance + 2.0; // Added 2 cm calibration offset
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
    } else if (millis() - buttonPressTime > 5000) {
      Serial.println(F("Resetting preferences..."));
      preferences.begin("cfg", false);
      preferences.clear();
      preferences.end();
      
      for (int i = 0; i < 10; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        delay(50);
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
      Serial.println(F("[WS] Connected"));
      digitalWrite(LED_PIN, HIGH);
      webSocket.sendTXT("{\"type\":\"ping\"}");
      break;
    case WStype_TEXT: {
      String msg = (char*)payload;
      Serial.print(F("[WS] Rx: "));
      Serial.println(msg);
      
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
  
  preferences.begin("cfg", true);
  deviceName = preferences.getString("name", "");
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("pass", "");
  preferences.end();
  
  if (ssid == "") {
    isAPMode = true;
    Serial.println(F("Starting AP Mode"));
    WiFi.softAP("EET-Tank-Setup", "12345678");
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    
    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println(F("AP Portal started at 192.168.4.1"));
  } else {
    isAPMode = false;
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print(F("Connecting to WiFi"));
    
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
      delay(500);
      Serial.print(F("."));
      checkResetButton();
    }
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println(F(" Failed! Will retry in loop."));
    } else {
      Serial.println(F(" Connected!"));
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
  
  if (millis() - lastTelemetryTime > 100) { // Fast 100ms interval for zero-delay live updates
    lastTelemetryTime = millis();
    
    float newDist = measureDistance();
    if (newDist > 0) {
      if (smoothedDistance == 0) {
        smoothedDistance = newDist;
      } else {
        // Fast real-time response: 80% new data + 20% old data
        smoothedDistance = (smoothedDistance * 0.2) + (newDist * 0.8);
      }
    }
    
    float pct = toPercent(smoothedDistance);
    
    if (webSocket.isConnected()) {
      String json = "{\"type\":\"telemetry\",\"distanceCm\":";
      json += String(smoothedDistance, 2);
      json += ",\"rssi\":";
      json += WiFi.RSSI();
      json += "}";
      webSocket.sendTXT(json);
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
