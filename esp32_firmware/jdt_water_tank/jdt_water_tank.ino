/*
  ╔══════════════════════════════════════════════════════╗
  ║   JDT Islam Institute - Water Tank Controller        ║
  ║   Board    : ESP32 Dev Module                        ║
  ║   Reset Pin: GPIO 4 (Hold 5s to Reset WiFi)          ║
  ║   Logic    : Below 20cm = Lock at 20.0cm            ║
  ║              Above 20cm = Show Exact Distance        ║
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
const float SENSOR_OFFSET = 20.0; // Min 20cm offset for 100% Full
const float AUTO_PUMP_ON   = 20.0;
const float AUTO_PUMP_OFF  = 90.0;

// Objects
WebSocketsClient webSocket;
WebServer        server(80);
Preferences      preferences;
DNSServer        dnsServer;

const byte DNS_PORT = 53;

// State Variables
String deviceName;
String ssid;
String password;
bool isAPMode = false;
float smoothedDistance = 0;
float lastValidDistance = 20.0;
bool pumpState = false;
bool sensorError = false;
int failedReadings = 0;

unsigned long lastTelemetryTime = 0;
unsigned long apBlinkTimer = 0;
bool ledState = false;
unsigned long buttonPressTime = 0;
bool buttonHeld = false;

// Async WiFi Scanner State
int scanResultsCount = -1;
bool isScanning = false;

// Captive Portal HTML with Zero-Lag Async Scanner
const char INDEX_HTML[] PROGMEM = R"html(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>JDT Tank Setup</title>
  <style>
    body { font-family: sans-serif; background-color: #030712; color: #e0e0e0; text-align: center; margin: 0; padding: 20px; }
    h1 { color: #0ea5e9; margin-bottom: 5px; }
    p.sub { color: #64748b; font-size: 0.85rem; margin-top: 0; margin-bottom: 20px; }
    form { background-color: #0f172a; padding: 25px; border-radius: 16px; display: inline-block; border: 1px solid rgba(56,189,248,0.2); max-width: 90%; width: 320px; box-shadow: 0 20px 40px rgba(0,0,0,0.8); }
    label { display: block; text-align: left; font-size: 0.8rem; color: #94a3b8; margin-top: 12px; margin-bottom: 4px; font-weight: bold; }
    input, select { display: block; margin: 0 auto 12px auto; padding: 12px; width: 100%; border: 1px solid #334155; border-radius: 8px; box-sizing: border-box; background-color: #1e293b; color: #fff; font-size: 0.95rem; }
    input:focus, select:focus { outline: 2px solid #0ea5e9; }
    input[type=submit] { background: linear-gradient(135deg, #0ea5e9, #6366f1); color: #fff; font-weight: bold; cursor: pointer; margin-top: 20px; border: none; }
    .scan-status { font-size: 0.78rem; color: #38bdf8; text-align: left; margin-top: -6px; margin-bottom: 10px; }
  </style>
</head>
<body>
  <h1>JDT Tank Setup</h1>
  <p class="sub">Instant Zero-Lag Configuration</p>

  <form action="/save" method="post">
    <label>Device Name</label>
    <input type="text" name="name" placeholder="e.g. Rooftop Tank" required>

    <label>Select WiFi Network</label>
    <select id="wifiList" name="ssid" required>
      <option value="">Scanning for networks...</option>
    </select>
    <div id="scanStatus" class="scan-status">⚡ Instant WiFi Scan...</div>

    <label>WiFi Password</label>
    <input type="password" name="pass" placeholder="Enter WiFi Password">

    <input type="submit" value="Save & Connect">
  </form>

  <script>
    function fetchWiFi() {
      fetch('/scan')
        .then(res => res.json())
        .then(data => {
          const list = document.getElementById('wifiList');
          const status = document.getElementById('scanStatus');
          if (data.status === 'scanning') {
            setTimeout(fetchWiFi, 500);
            return;
          }
          list.innerHTML = '';
          if (!data.networks || data.networks.length === 0) {
            list.innerHTML = '<option value="">No WiFi Found (Refreshing...)</option>';
            status.innerHTML = '❌ No networks found';
            setTimeout(fetchWiFi, 2000);
          } else {
            status.innerHTML = '✅ Found ' + data.networks.length + ' WiFi networks';
            data.networks.forEach(item => {
              const opt = document.createElement('option');
              opt.value = item.ssid;
              opt.innerHTML = item.ssid + ' (' + item.rssi + ' dBm)';
              list.appendChild(opt);
            });
          }
        })
        .catch(err => {
          document.getElementById('scanStatus').innerHTML = '⚠️ Tap to select network';
        });
    }
    fetchWiFi();
  </script>
</body>
</html>
)html";

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleScan() {
  if (scanResultsCount == -1 && !isScanning) {
    WiFi.scanNetworks(true); // Start Async non-blocking scan
    isScanning = true;
    server.send(200, "application/json", "{\"status\":\"scanning\"}");
    return;
  }
  
  scanResultsCount = WiFi.scanComplete();
  if (scanResultsCount < 0) {
    server.send(200, "application/json", "{\"status\":\"scanning\"}");
  } else {
    String json = "{\"status\":\"complete\",\"networks\":[";
    for (int i = 0; i < scanResultsCount; ++i) {
      if (i > 0) json += ",";
      json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
    }
    json += "]}";
    WiFi.scanDelete();
    scanResultsCount = -1;
    isScanning = false;
    server.send(200, "application/json", json);
  }
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
  delay(1000);
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
  
  long duration = pulseIn(ECHO_PIN, HIGH, 35000);
  if (duration == 0) return -1.0;
  
  float distance = (duration * 0.0343) / 2.0;
  return distance;
}

float toPercent(float dist) {
  float usableDepth = TANK_DEPTH_CM - SENSOR_OFFSET;
  float waterLevel  = usableDepth - (dist - SENSOR_OFFSET);
  if (waterLevel < 0) waterLevel = 0;
  if (waterLevel > usableDepth) waterLevel = usableDepth;
  return (waterLevel / usableDepth) * 100.0;
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

String getJsonValue(String json, String key) {
  String searchKey = "\"" + key + "\":\"";
  int startKey = json.indexOf(searchKey);
  if (startKey == -1) {
    searchKey = "\"" + key + "\": \"";
    startKey = json.indexOf(searchKey);
  }
  if (startKey == -1) return "";
  startKey += searchKey.length();
  int endKey = json.indexOf("\"", startKey);
  if (endKey == -1) return "";
  return json.substring(startKey, endKey);
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

      // WiFi config update from Web Dashboard
      if (msg.indexOf("\"wificonfig\"") >= 0 || msg.indexOf("\"ssid\"") >= 0) {
        String newSsid = getJsonValue(msg, "ssid");
        String newPass = getJsonValue(msg, "pass");
        if (newSsid.length() > 0) {
          Serial.print(F("Updating WiFi Credentials -> SSID: "));
          Serial.println(newSsid);
          preferences.begin("cfg", false);
          preferences.putString("ssid", newSsid);
          preferences.putString("pass", newPass);
          preferences.end();
          Serial.println(F("Saved to preferences! Rebooting to connect to new WiFi..."));
          delay(1000);
          ESP.restart();
        }
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
    WiFi.mode(WIFI_AP);
    WiFi.softAP("JDT-Tank-Setup", "12345678");
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    
    server.on("/", HTTP_GET, handleRoot);
    server.on("/scan", HTTP_GET, handleScan);
    server.on("/save", HTTP_POST, handleSave);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println(F("Portal started at 192.168.4.1"));
  } else {
    isAPMode = false;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print(F("Connecting to WiFi: "));
    Serial.println(ssid);
    
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
      delay(200);
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
    webSocket.setReconnectInterval(2000);
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
  
  // Send telemetry every 500ms for smooth live updates
  if (millis() - lastTelemetryTime > 500) {
    lastTelemetryTime = millis();
    
    float newDist = measureDistance();
    
    // STRICT RULE:
    // Any reading below 20cm (or blind zone echo spike) is locked to exactly 20.0 cm!
    // Any reading above 20cm shows its exact measured distance!
    if (newDist > 0.0 && newDist < 20.0) {
      newDist = 20.0;
    }

    if (lastValidDistance <= 22.0 && newDist > 80.0) {
      // Blind zone secondary echo jump -> lock to 20.0 cm
      newDist = 20.0;
    }

    if (newDist > 0.0) {
      failedReadings = 0;
      sensorError = false;
      lastValidDistance = newDist;

      if (smoothedDistance == 0.0) {
        smoothedDistance = newDist;
      } else {
        smoothedDistance = (smoothedDistance * 0.7) + (newDist * 0.3);
      }
    } else {
      failedReadings++;
      if (failedReadings >= 15) {
        sensorError = true;
      }
    }
    
    // Ensure display distance is locked to 20.0cm if below 20cm
    float activeDistance = (smoothedDistance > 0.0) ? smoothedDistance : lastValidDistance;
    if (activeDistance < 20.0) {
      activeDistance = 20.0;
    }

    float pct = toPercent(activeDistance);
    
    if (webSocket.isConnected()) {
      String json = "{\"type\":\"telemetry\",\"distanceCm\":";
      json += String(sensorError ? -1.0 : activeDistance, 2);
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
