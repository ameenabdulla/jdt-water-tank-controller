/*
  JDT Water Tank Controller - ESP32 Firmware (HTTP Polling)
  
  Hardware Connections:
  - JSN-SR04T / HC-SR04 Ultrasonic Sensor:
      Trigger Pin → GPIO 12
      Echo Pin    → GPIO 13
      VCC         → 5V
      GND         → GND
  - Relay Module (Pump):
      Signal Pin  → GPIO 14
      VCC         → 5V (or VIN)
      GND         → GND
  - LED Indicator (Optional):
      Built-in LED → GPIO 2
*/

#include <WiFi.h>
#include <HTTPClient.h>

// ======================== CONFIGURATION ========================
// 1. WiFi Settings
const char* ssid     = "Airtel_juma_8616";
const char* password = "38725";

// 2. Server URL (Render cloud deployment)
// Replace with your actual Render URL once deployed!
const char* serverUrl = "https://YOUR-RENDER-URL.onrender.com/api/device";

// 3. Ultrasonic Sensor Pins
const int TRIG_PIN = 12;
const int ECHO_PIN = 13;

// 4. Relay Pin (Pump Motor)
const int RELAY_PIN = 14;

// 5. LED Indicator
const int LED_PIN = 2;

// 6. Fallback Thresholds (used when server is unreachable)
const float LOCAL_LOW_THRESHOLD = 20.0;   // % to turn pump ON
const float LOCAL_HIGH_THRESHOLD = 90.0;  // % to turn pump OFF
const float LOCAL_TANK_DEPTH = 100.0;     // cm
const float LOCAL_SENSOR_OFFSET = 5.0;    // cm
// ===============================================================

unsigned long lastPollTime = 0;
const unsigned long pollInterval = 2000; // Poll server every 2 seconds

unsigned long lastMeasureTime = 0;
const unsigned long measureInterval = 500; // Measure distance every 500ms

float currentDistanceCm = 0;
bool pumpOn = false;
bool serverReachable = false;
int consecutiveFailures = 0;
const int MAX_FAILURES = 10; // After 10 failures, switch to local mode

// Measure distance using ultrasonic sensor
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  
  if (duration == 0) {
    Serial.println("[SENSOR] Timeout - no echo received!");
    return -1; // Error
  }
  
  float distance = (duration * 0.0343) / 2.0;
  
  // Filter out clearly invalid readings
  if (distance < 2 || distance > 400) {
    Serial.println("[SENSOR] Out of range reading, ignoring.");
    return -1;
  }
  
  return distance;
}

// Calculate water level percentage locally (fallback)
float calculateLocalLevel(float distanceCm) {
  float effectiveDepth = LOCAL_TANK_DEPTH - LOCAL_SENSOR_OFFSET;
  float waterHeight = effectiveDepth - (distanceCm - LOCAL_SENSOR_OFFSET);
  float percent = (waterHeight / effectiveDepth) * 100.0;
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return percent;
}

// Local fallback pump control (runs when server is unreachable)
void localPumpControl(float levelPercent) {
  if (levelPercent <= LOCAL_LOW_THRESHOLD && !pumpOn) {
    pumpOn = true;
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("[LOCAL] Pump turned ON (level below threshold)");
  }
  if (levelPercent >= LOCAL_HIGH_THRESHOLD && pumpOn) {
    pumpOn = false;
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("[LOCAL] Pump turned OFF (level above threshold)");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Setup pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, LOW);  // Pump OFF at start
  digitalWrite(LED_PIN, LOW);
  
  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Measure distance periodically
  if (millis() - lastMeasureTime >= measureInterval) {
    lastMeasureTime = millis();
    
    float reading = measureDistance();
    if (reading > 0) {
      // Simple moving average filter (smooth out noise)
      currentDistanceCm = (currentDistanceCm * 0.7) + (reading * 0.3);
    }
    
    Serial.print("[SENSOR] Distance: ");
    Serial.print(currentDistanceCm, 1);
    Serial.println(" cm");
  }
  
  // Poll server periodically
  if ((millis() - lastPollTime >= pollInterval) && (WiFi.status() == WL_CONNECTED)) {
    lastPollTime = millis();
    
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    // Get WiFi signal strength
    int rssi = WiFi.RSSI();
    
    // Build JSON payload
    String payload = "{\"distanceCm\":" + String(currentDistanceCm, 1) + 
                     ",\"rssi\":" + String(rssi) + "}";
    
    int httpCode = http.POST(payload);
    
    if (httpCode > 0) {
      consecutiveFailures = 0;
      serverReachable = true;
      
      String response = http.getString();
      Serial.print("[SERVER] Response: ");
      Serial.println(response);
      
      // Parse pump command from server
      if (response.indexOf("\"pumpOn\":true") != -1) {
        if (!pumpOn) {
          pumpOn = true;
          digitalWrite(RELAY_PIN, HIGH);
          digitalWrite(LED_PIN, HIGH);
          Serial.println("[PUMP] Turned ON by server command");
        }
      } else if (response.indexOf("\"pumpOn\":false") != -1) {
        if (pumpOn) {
          pumpOn = false;
          digitalWrite(RELAY_PIN, LOW);
          digitalWrite(LED_PIN, LOW);
          Serial.println("[PUMP] Turned OFF by server command");
        }
      }
    } else {
      consecutiveFailures++;
      Serial.print("[SERVER] Error code: ");
      Serial.println(httpCode);
      
      if (consecutiveFailures >= MAX_FAILURES) {
        serverReachable = false;
        Serial.println("[FALLBACK] Server unreachable. Switching to local control.");
        
        // Calculate level locally and control pump
        float level = calculateLocalLevel(currentDistanceCm);
        localPumpControl(level);
      }
    }
    
    http.end();
  }
  
  // Handle WiFi reconnection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost. Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
    
    // While WiFi is down, run local pump control
    float level = calculateLocalLevel(currentDistanceCm);
    localPumpControl(level);
    
    delay(5000);
  }
}
