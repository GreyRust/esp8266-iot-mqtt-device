# ESP8266 IoT MQTT Device

A comprehensive ESP8266-based IoT device that provides multi-LED control, environmental monitoring, and intelligent lighting automation through MQTT protocol. Perfect for smart home applications and IoT learning projects.

## Features

### 🔌 Connectivity
- **WiFi Connection**: Robust WiFi connectivity with auto-reconnection
- **MQTT Integration**: Full MQTT client with authentication support
- **Real-time Communication**: Bi-directional MQTT messaging
- **Connection Monitoring**: Automatic connection status reporting

### 💡 LED Control System
- **Triple LED Control**: Independent control of 3 LEDs (1 built-in + 2 external)
- **Group Operations**: Simultaneous control of all LEDs
- **Blink Patterns**: Customizable blinking sequences
- **State Synchronization**: Real-time LED state feedback

### 🌟 Smart Lighting Features
- **Auto-Light Mode**: Automatic LED control based on ambient light
- **LDR Integration**: Light-dependent resistor for ambient light detection
- **Configurable Threshold**: Adjustable light sensitivity settings
- **Light Condition Analysis**: Intelligent light level categorization
- **LDR Calibration**: Built-in sensor calibration functionality

### 📊 Environmental Monitoring
- **Temperature Sensing**: DHT11/DHT22 temperature readings
- **Humidity Monitoring**: Real-time humidity measurements
- **Light Level Detection**: Continuous ambient light monitoring
- **System Health**: Memory usage, WiFi signal strength, and uptime tracking

## Hardware Requirements

### Essential Components
- **ESP8266 NodeMCU** (or compatible board)
- **DHT11/DHT22** temperature and humidity sensor
- **LDR (Light Dependent Resistor)** with 10kΩ pull-down resistor
- **LEDs** (2 external + built-in LED)
- **220Ω Resistors** for LED current limiting
- **Breadboard and jumper wires**

### Pin Configuration

| Component | Pin | GPIO | Description |
|-----------|-----|------|-------------|
| LED 1 (Built-in) | D4 | GPIO2 | NodeMCU built-in LED |
| LED 2 (External) | D5 | GPIO14 | External LED 1 |
| LED 3 (External) | D6 | GPIO12 | External LED 2 |
| DHT Sensor | D2 | GPIO4 | Temperature/Humidity sensor |
| LDR Sensor | A0 | ADC0 | Light sensor (analog input) |

## Circuit Diagram

```
ESP8266 NodeMCU
    │
    ├── D4 (GPIO2) ──── Built-in LED
    │
    ├── D5 (GPIO14) ──── 220Ω ──── LED2 ──── GND
    │
    ├── D6 (GPIO12) ──── 220Ω ──── LED3 ──── GND
    │
    ├── D2 (GPIO4) ──── DHT11/DHT22
    │                    │
    │                    └── 3.3V, GND
    │
    └── A0 ──── LDR ──── 3.3V
                 │
                10kΩ
                 │
                GND
```

## Software Requirements

### Arduino IDE Setup
1. **ESP8266 Board Package**: Install ESP8266 board package in Arduino IDE
2. **Required Libraries**:
   - `ESP8266WiFi` (included with ESP8266 package)
   - `PubSubClient` for MQTT
   - `ArduinoJson` for JSON handling
   - `DHT sensor library` for temperature/humidity

### Library Installation
```cpp
// Install these libraries via Arduino IDE Library Manager:
// 1. PubSubClient by Nick O'Leary
// 2. ArduinoJson by Benoit Blanchon
// 3. DHT sensor library by Adafruit
```

## Configuration

### Network Settings
```cpp
// WiFi Configuration
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// MQTT Configuration
const char* mqtt_server = "192.168.1.100";  // Your MQTT broker IP
const int mqtt_port = 1883;
const char* mqtt_username = "your_username";
const char* mqtt_password = "your_password";
```

### MQTT Topics
| Topic | Direction | Purpose | Message Format |
|-------|-----------|---------|----------------|
| `esp8266/led` | Subscribe | LED control commands | `{"led1": true, "command": "all_on"}` |
| `esp8266/sensor` | Publish | Environmental data | `{"temperature": 25.5, "humidity": 60.2}` |
| `esp8266/status` | Publish | Device status | `{"status": "online", "uptime": 3600}` |
| `esp8266/ldr` | Publish | Light sensor data | `{"ldr_raw": 512, "light_percentage": 50.0}` |

## Installation & Setup

### 1. Hardware Assembly
1. Connect components according to the circuit diagram
2. Ensure proper power supply (3.3V for sensors)
3. Verify all connections before powering on

### 2. Software Upload
1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/esp8266-iot-mqtt-device.git
   ```

2. **Open in Arduino IDE**:
   - Open `esp8266-iot-mqtt-device.ino`
   - Install required libraries
   - Select correct board: "NodeMCU 1.0 (ESP-12E Module)"

3. **Configure settings**:
   - Update WiFi credentials
   - Set MQTT broker details
   - Adjust pin assignments if needed

4. **Upload to device**:
   - Connect ESP8266 via USB
   - Select correct COM port
   - Upload the code

### 3. MQTT Broker Setup
Ensure you have an MQTT broker running:
- **Local**: Mosquitto, Eclipse Mosquitto
- **Cloud**: HiveMQ, AWS IoT, Google Cloud IoT
- **Testing**: Use public brokers for initial testing

## API Reference

### LED Control Commands

#### Individual LED Control
```json
{
  "led1": true,
  "led2": false,
  "led3": true
}
```

#### Group Commands
```json
{
  "command": "all_on"     // Turn all LEDs on
}
{
  "command": "all_off"    // Turn all LEDs off
}
{
  "command": "blink"      // Blink all LEDs 3 times
}
```

#### Auto-Light Control
```json
{
  "auto_light": true,     // Enable auto-light mode
  "ldr_threshold": 300    // Set light threshold (0-1024)
}
```

#### LDR Calibration
```json
{
  "command": "calibrate_ldr"  // Start 10-second calibration
}
```

### Published Data Formats

#### Sensor Data
```json
{
  "temperature": 25.5,
  "humidity": 60.2,
  "led1_state": true,
  "led2_state": false,
  "led3_state": true,
  "ldr_value": 512,
  "auto_light_mode": false,
  "timestamp": 12345678
}
```

#### Device Status
```json
{
  "status": "online",
  "uptime": 3600,
  "free_heap": 45000,
  "wifi_rssi": -45,
  "led1_state": true,
  "led2_state": false,
  "led3_state": true,
  "auto_light_mode": false,
  "ldr_threshold": 300
}
```

#### LDR Data
```json
{
  "ldr_raw": 512,
  "light_percentage": 50.0,
  "light_condition": "Normal",
  "auto_mode": false,
  "threshold": 300,
  "timestamp": 12345678
}
```

## Features in Detail

### Auto-Light System
- **Threshold-based Control**: LEDs automatically turn on when ambient light falls below threshold
- **Hysteresis Prevention**: Smart switching to prevent flickering
- **Manual Override**: Manual control always takes precedence
- **Customizable Sensitivity**: Adjustable threshold from mobile app

### Light Condition Categories
| Condition | LDR Range | Description |
|-----------|-----------|-------------|
| Very Bright | > 700 | Direct sunlight/bright artificial light |
| Bright | 500-700 | Well-lit room |
| Normal | 300-500 | Average room lighting |
| Dim | 150-300 | Low light conditions |
| Dark | < 150 | Very dark/night conditions |

### System Monitoring
- **Memory Usage**: Free heap monitoring for stability
- **WiFi Signal**: RSSI reporting for connection quality
- **Uptime Tracking**: Device operational time since last boot
- **Connection Status**: Real-time connectivity monitoring

## Troubleshooting

### Common Issues

#### 1. WiFi Connection Problems
```
Symptoms: Device not connecting to WiFi
Solutions:
- Verify SSID and password
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP8266 doesn't support 5GHz)
- Reset WiFi credentials
```

#### 2. MQTT Connection Failures
```
Symptoms: MQTT connection errors in serial monitor
Solutions:
- Verify broker IP address and port
- Check username/password credentials
- Ensure broker is accessible from device network
- Check firewall settings
```

#### 3. Sensor Reading Issues
```
Symptoms: NaN values or incorrect readings
Solutions:
- Verify sensor wiring and power supply
- Check pull-up resistors for I2C sensors
- Replace faulty sensors
- Verify pin assignments in code
```

#### 4. LED Control Not Working
```
Symptoms: LEDs not responding to commands
Solutions:
- Check LED wiring and current limiting resistors
- Verify pin assignments
- Test with manual digitalWrite commands
- Check power supply capacity
```

### Debug Mode
Enable detailed logging by adding to setup():
```cpp
Serial.begin(115200);
// Add debug prints throughout code
```

### Serial Monitor Output
Monitor device status via Serial Monitor (115200 baud):
- WiFi connection status
- MQTT connection attempts
- Sensor readings
- Command acknowledgments
- Error messages

## Performance Specifications

- **WiFi Range**: Up to 50m indoors (depends on environment)
- **MQTT Update Rate**: 
  - Sensor data: Every 2 seconds
  - LDR data: Every 1 second
  - Status updates: Every 30 seconds
- **Response Time**: < 100ms for LED commands
- **Power Consumption**: ~80mA during normal operation
- **Memory Usage**: ~30KB RAM, ~300KB Flash

## Customization Options

### Timing Adjustments
```cpp
const unsigned long sensorInterval = 2000;    // Sensor reading interval
const unsigned long statusInterval = 30000;   // Status update interval
const unsigned long ldrInterval = 1000;       // LDR reading interval
```

### Pin Reassignment
```cpp
#define LED1_PIN D4    // Change pin assignments as needed
#define LED2_PIN D5
#define LED3_PIN D6
#define DHT_PIN D2
```

### Sensor Configuration
```cpp
#define DHT_TYPE DHT22    // Change to DHT11 if using DHT11 sensor
```

## Advanced Features

### Calibration System
- **10-second calibration period**: Exposes LDR to various light conditions
- **Automatic threshold calculation**: Sets optimal threshold based on environment
- **Calibration data publishing**: Shares results with connected applications

### Error Handling
- **WiFi reconnection**: Automatic WiFi reconnection on connection loss
- **MQTT reconnection**: Robust MQTT reconnection with exponential backoff
- **Sensor error handling**: Graceful handling of sensor read failures
- **Watchdog protection**: Prevents system hangs

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines
- Follow Arduino coding standards
- Add comments for complex logic
- Test thoroughly before submitting
- Update documentation for new features

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Related Projects

- **Flutter Mobile App**: [flutter-iot-mqtt-controller](https://github.com/GreyRust/flutter-iot-mqtt-controller)

## Acknowledgments

- ESP8266 Community for excellent documentation
- Arduino IDE team for the development platform
- PubSubClient library maintainers
- DHT sensor library contributors
