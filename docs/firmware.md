# Firmware Documentation (WiFi & MQTT Version)

The firmware is designed for the **ESP32** using **ESP-IDF**. It has been upgraded from a simple Serial logger to a wireless node that uses mDNS for zero-config connection to the MQTT broker.

## Hardware Components
- **Microcontroller**: ESP32
- **Sensor**: SHT30 (I2C)
- **Display**: SSD1306 OLED (I2C)

## Connectivity Features
- **WiFi**: Connects to a local Access Point.
- **mDNS Discovery**: Instead of a hardcoded IP, the ESP32 looks for a service named `_mqtt._tcp` on the local network to find the Mosquitto broker.
- **MQTT**: Publishes sensor data to the broker using the Paho/ESP-MQTT client.

## Pin Configuration (I2C)
| Function | GPIO Pin |
|----------|----------|
| SDA      | 21       |
| SCL      | 22       |
| Frequency| 100 kHz  |

## Software Tasks

### 1. Networking Task
- Initializes WiFi and starts mDNS searching.
- Once the broker's IP is resolved via `_mqtt._tcp.local`, it establishes an MQTT connection.

### 2. Sensor & Display Task
- Reads **SHT30** every 2 seconds.
- Updates the **OLED** display with real-time temperature and humidity.
- **MQTT Publish**: Sends a JSON payload to the topic `room/sensor/data`.

## Data Transmission
The ESP32 sends data in the following JSON format:
```json
{"temp": 25.50, "hum": 60.00}
```

## Build and Flash
```bash
make build  # Compile the firmware
make flash  # Flash to ESP32
make monitor # View logs and verify mDNS resolution
```
