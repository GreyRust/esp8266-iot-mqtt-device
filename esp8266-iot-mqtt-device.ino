#include <ESP8266WiFi.h> 
#include <PubSubClient.h> 
#include <ArduinoJson.h> 
#include <DHT.h> 
 
// WiFi Configuration 
const char* ssid = "Redmi 12"; 
const char* password = "11111111"; 
 
// MQTT Configuration 
const char* mqtt_server = "192.168.150.137"; 
const int mqtt_port = 1883; 
const char* mqtt_client_id = "nodemcu_client"; 
const char* mqtt_username = "megatron";  // Ganti dengan username MQTT Anda 
const char* mqtt_password = "megatron";  // Ganti dengan password MQTT Anda 
 
// MQTT Topics 
const char* led_topic = "esp8266/led"; 
const char* sensor_topic = "esp8266/sensor"; 
const char* status_topic = "esp8266/status"; 
const char* ldr_topic = "esp8266/ldr";
 
// Pin Configuration 
#define LED1_PIN D4       // GPIO2 (LED built-in pada NodeMCU)
#define LED2_PIN D5       // GPIO14 (LED eksternal 1)
#define LED3_PIN D6       // GPIO12 (LED eksternal 2)
#define DHT_PIN D2        // GPIO4 (ubah sesuai kebutuhan) 
#define DHT_TYPE DHT11    // DHT22 atau DHT11 
#define LDR_PIN A0        // Pin analog untuk LDR
 
// Objects 
WiFiClient espClient; 
PubSubClient client(espClient); 
DHT dht(DHT_PIN, DHT_TYPE); 
 
// Variables 
bool led1State = false; 
bool led2State = false; 
bool led3State = false; 
bool autoLightMode = false;  // Mode otomatis berdasarkan LDR
int ldrThreshold = 300;      // Threshold untuk mode otomatis (0-1024)
unsigned long lastSensorRead = 0; 
unsigned long lastStatusPublish = 0; 
unsigned long lastLdrRead = 0;
unsigned long startTime = 0;  // Untuk tracking uptime 
const unsigned long sensorInterval = 2000; 
const unsigned long statusInterval = 2000; 
const unsigned long ldrInterval = 1000;  // Baca LDR setiap 1 detik
 
void setup() { 
  Serial.begin(115200); 
  delay(10); 
   
  // Initialize all LED pins
  pinMode(LED1_PIN, OUTPUT); 
  pinMode(LED2_PIN, OUTPUT); 
  pinMode(LED3_PIN, OUTPUT); 
  
  // Turn off all LEDs initially
  digitalWrite(LED1_PIN, LOW); 
  digitalWrite(LED2_PIN, LOW); 
  digitalWrite(LED3_PIN, LOW); 
   
  dht.begin(); 
  startTime = millis();  // Record start time 
   
  setup_wifi(); 
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback); 
   
  Serial.println("NodeMCU MQTT IoT Device with 3 LEDs and LDR Ready!"); 
} 
 
void setup_wifi() { 
  Serial.println(); 
  Serial.print("Connecting to "); 
  Serial.println(ssid); 
   
  WiFi.begin(ssid, password); 
   
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  } 
   
  Serial.println(""); 
  Serial.println("WiFi connected"); 
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP()); 
} 

int readLDR() {
  return analogRead(LDR_PIN);
}

float getLightPercentage(int ldrValue) {
  // Konversi nilai LDR (0-1024) ke persentase cahaya (0-100%)
  // Nilai tinggi LDR = cahaya terang, nilai rendah = gelap
  return map(ldrValue, 0, 1024, 0, 100);
}

String getLightCondition(int ldrValue) {
  if (ldrValue > 700) return "Very Bright";
  else if (ldrValue > 500) return "Bright";
  else if (ldrValue > 300) return "Normal";
  else if (ldrValue > 150) return "Dim";
  else return "Dark";
}

void handleAutoLight(int ldrValue) {
  if (!autoLightMode) return;
  
  bool shouldTurnOn = (ldrValue < ldrThreshold);
  
  if (shouldTurnOn && (!led1State || !led2State || !led3State)) {
    // Nyalakan semua LED jika gelap
    led1State = led2State = led3State = true;
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    Serial.println("Auto Light: LEDs turned ON (Dark detected)");
  } else if (!shouldTurnOn && (led1State || led2State || led3State)) {
    // Matikan semua LED jika terang
    led1State = led2State = led3State = false;
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    Serial.println("Auto Light: LEDs turned OFF (Bright detected)");
  }
}
 
void callback(char* topic, byte* payload, unsigned int length) { 
  Serial.print("Message arrived ["); 
  Serial.print(topic); 
  Serial.print("] "); 
   
  String message = ""; 
  for (unsigned int i = 0; i < length; i++) { 
    message += (char)payload[i]; 
  } 
  Serial.println(message); 
   
  if (String(topic) == led_topic) { 
    DynamicJsonDocument doc(512); 
    deserializeJson(doc, message); 
     
    // Control LED 1
    if (doc.containsKey("led1")) { 
      led1State = doc["led1"]; 
      digitalWrite(LED1_PIN, led1State ? HIGH : LOW); 
      Serial.println("LED1 state: " + String(led1State ? "ON" : "OFF")); 
    } 
    
    // Control LED 2
    if (doc.containsKey("led2")) { 
      led2State = doc["led2"]; 
      digitalWrite(LED2_PIN, led2State ? HIGH : LOW); 
      Serial.println("LED2 state: " + String(led2State ? "ON" : "OFF")); 
    } 
    
    // Control LED 3
    if (doc.containsKey("led3")) { 
      led3State = doc["led3"]; 
      digitalWrite(LED3_PIN, led3State ? HIGH : LOW); 
      Serial.println("LED3 state: " + String(led3State ? "ON" : "OFF")); 
    } 
    
    // Control all LEDs at once (backward compatibility)
    if (doc.containsKey("led")) { 
      bool allLedState = doc["led"];
      led1State = led2State = led3State = allLedState;
      digitalWrite(LED1_PIN, allLedState ? HIGH : LOW); 
      digitalWrite(LED2_PIN, allLedState ? HIGH : LOW); 
      digitalWrite(LED3_PIN, allLedState ? HIGH : LOW); 
      Serial.println("All LEDs state: " + String(allLedState ? "ON" : "OFF")); 
    }
    
    // Auto Light Mode Control
    if (doc.containsKey("auto_light")) {
      autoLightMode = doc["auto_light"];
      Serial.println("Auto Light Mode: " + String(autoLightMode ? "ENABLED" : "DISABLED"));
    }
    
    // LDR Threshold Setting
    if (doc.containsKey("ldr_threshold")) {
      ldrThreshold = doc["ldr_threshold"];
      Serial.println("LDR Threshold set to: " + String(ldrThreshold));
    }
    
    // Special commands
    if (doc.containsKey("command")) {
      String command = doc["command"];
      if (command == "all_on") {
        led1State = led2State = led3State = true;
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, HIGH);
        digitalWrite(LED3_PIN, HIGH);
        Serial.println("All LEDs turned ON");
      } else if (command == "all_off") {
        led1State = led2State = led3State = false;
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
        Serial.println("All LEDs turned OFF");
      } else if (command == "blink") {
        blinkAllLeds(3, 500); // Blink 3 times, 500ms interval
      } else if (command == "calibrate_ldr") {
        calibrateLDR();
      }
    }
  } 
} 

void calibrateLDR() {
  Serial.println("Starting LDR calibration...");
  Serial.println("Please expose LDR to different light conditions for 10 seconds");
  
  int minVal = 1024, maxVal = 0;
  unsigned long calibrationStart = millis();
  
  while (millis() - calibrationStart < 10000) { // 10 seconds calibration
    int ldrValue = readLDR();
    if (ldrValue < minVal) minVal = ldrValue;
    if (ldrValue > maxVal) maxVal = ldrValue;
    delay(100);
  }
  
  // Set threshold to middle value
  ldrThreshold = (minVal + maxVal) / 2;
  
  Serial.println("LDR Calibration complete:");
  Serial.println("Min value: " + String(minVal));
  Serial.println("Max value: " + String(maxVal));
  Serial.println("New threshold: " + String(ldrThreshold));
  
  // Publish calibration results
  publishLDRCalibration(minVal, maxVal, ldrThreshold);
}

void publishLDRCalibration(int minVal, int maxVal, int threshold) {
  DynamicJsonDocument doc(256);
  doc["calibration"] = true;
  doc["min_value"] = minVal;
  doc["max_value"] = maxVal;
  doc["threshold"] = threshold;
  doc["timestamp"] = millis();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  if (client.publish(ldr_topic, jsonString.c_str())) {
    Serial.println("LDR Calibration data published");
  }
}

void blinkAllLeds(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    delay(delayMs);
  }
  
  // Restore previous states
  digitalWrite(LED1_PIN, led1State ? HIGH : LOW);
  digitalWrite(LED2_PIN, led2State ? HIGH : LOW);
  digitalWrite(LED3_PIN, led3State ? HIGH : LOW);
}
 
void reconnect() { 
  while (!client.connected()) { 
    Serial.print("Attempting MQTT connection..."); 
     
    // Coba koneksi dengan username dan password 
    bool connected = false; 
     
    // Jika username dan password tidak kosong, gunakan autentikasi 
    if (strlen(mqtt_username) > 0 && strlen(mqtt_password) > 0) { 
      Serial.print("with authentication..."); 
      connected = client.connect(mqtt_client_id, mqtt_username, mqtt_password); 
    } else { 
      // Koneksi tanpa autentikasi 
      Serial.print("without authentication..."); 
      connected = client.connect(mqtt_client_id); 
    } 
     
    if (connected) { 
      Serial.println("connected"); 
      client.subscribe(led_topic); 
      publishStatusOnline(); // Kirim status pertama setelah koneksi berhasil 
      Serial.println("Subscribed to: " + String(led_topic)); 
    } else { 
      Serial.print("failed, rc="); 
      Serial.print(client.state()); 
      Serial.println(" retry in 5 seconds"); 
       
      // Print error details 
      switch (client.state()) { 
        case -4: 
          Serial.println("Connection timeout"); 
          break; 
        case -3: 
          Serial.println("Connection lost"); 
          break; 
        case -2: 
          Serial.println("Connect failed"); 
          break; 
        case -1: 
          Serial.println("Disconnected"); 
          break; 
        case 1: 
          Serial.println("Bad protocol version"); 
          break; 
        case 2: 
          Serial.println("Bad client ID"); 
          break; 
        case 3: 
          Serial.println("Unavailable"); 
          break; 
        case 4: 
          Serial.println("Bad credentials"); 
          break; 
        case 5: 
          Serial.println("Unauthorized"); 
          break; 
      } 
       
      delay(5000); 
    } 
  } 
} 

void publishLDRData() {
  int ldrValue = readLDR();
  float lightPercentage = getLightPercentage(ldrValue);
  String lightCondition = getLightCondition(ldrValue);
  
  DynamicJsonDocument doc(512);
  doc["ldr_raw"] = ldrValue;
  doc["light_percentage"] = lightPercentage;
  doc["light_condition"] = lightCondition;
  doc["auto_mode"] = autoLightMode;
  doc["threshold"] = ldrThreshold;
  doc["timestamp"] = millis();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  if (client.publish(ldr_topic, jsonString.c_str())) {
    Serial.println("LDR Data published: " + String(ldrValue) + " (" + lightCondition + ")");
  } else {
    Serial.println("Failed to publish LDR data");
  }
}
 
void publishSensorData() { 
  // Uncomment baris berikut untuk menggunakan sensor DHT yang sesungguhnya 
  float temperature = dht.readTemperature(); 
  float humidity = dht.readHumidity(); 
   
  // Data dummy untuk testing 
  //float temperature = random(20, 50); 
  //float humidity = random(50, 80); 
   
  if (isnan(temperature) || isnan(humidity)) { 
    Serial.println("DHT sensor read failed!"); 
    return; 
  } 
   
  DynamicJsonDocument doc(512); 
  doc["temperature"] = temperature; 
  doc["humidity"] = humidity; 
  doc["led1_state"] = led1State;
  doc["led2_state"] = led2State;
  doc["led3_state"] = led3State;
  doc["ldr_value"] = readLDR();
  doc["auto_light_mode"] = autoLightMode;
  doc["timestamp"] = millis(); 
   
  String jsonString; 
  serializeJson(doc, jsonString); 
   
  if (client.publish(sensor_topic, jsonString.c_str())) { 
    Serial.println("Published Sensor Data:"); 
    Serial.println(jsonString); 
  } else { 
    Serial.println("Failed to publish sensor data"); 
  } 
} 
 
void publishStatusOnline() { 
  // Buat JSON status online 
  DynamicJsonDocument doc(256); 
  doc["status"] = "online"; 
  doc["uptime"] = (millis() - startTime) / 1000; // dalam detik dari start time 
  doc["free_heap"] = ESP.getFreeHeap(); // Memory yang tersedia 
  doc["wifi_rssi"] = WiFi.RSSI(); // Kekuatan sinyal WiFi 
  doc["led1_state"] = led1State;
  doc["led2_state"] = led2State;
  doc["led3_state"] = led3State;
  doc["auto_light_mode"] = autoLightMode;
  doc["ldr_threshold"] = ldrThreshold;
   
  String jsonString; 
  serializeJson(doc, jsonString); 
   
  if (client.publish(status_topic, jsonString.c_str())) { 
    Serial.println("Status sent:"); 
    Serial.println(jsonString); 
  } else { 
    Serial.println("Failed to publish status"); 
  } 
} 
 
void publishStatusOffline() { 
  // Buat JSON status offline untuk Last Will 
  DynamicJsonDocument doc(128); 
  doc["status"] = "offline"; 
  doc["uptime"] = (millis() - startTime) / 1000; 
   
  String jsonString; 
  serializeJson(doc, jsonString); 
   
  client.publish(status_topic, jsonString.c_str(), true); // Retain message 
  Serial.println("Status sent: offline"); 
} 
 
void loop() { 
  if (!client.connected()) { 
    reconnect(); 
  } 
  client.loop(); 
   
  unsigned long now = millis(); 
   
  // Sensor setiap 2 detik 
  if (now - lastSensorRead > sensorInterval) { 
    lastSensorRead = now; 
    publishSensorData(); 
  } 
   
  // LDR setiap 1 detik
  if (now - lastLdrRead > ldrInterval) {
    lastLdrRead = now;
    publishLDRData();
    
    // Handle auto light control
    int ldrValue = readLDR();
    handleAutoLight(ldrValue);
  }
   
  // Kirim status online setiap 2 detik 
  if (now - lastStatusPublish > statusInterval) { 
    lastStatusPublish = now; 
    publishStatusOnline(); 
  } 
   
  // Check WiFi connection 
  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println("WiFi connection lost! Reconnecting..."); 
    setup_wifi(); 
  } 
   
  delay(100); 
}