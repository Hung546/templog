# Firmware

The firmware is built for the **ESP32** microcontroller using the **ESP-IDF** framework. It interfaces with an **SHT30** temperature and humidity sensor via the I2C protocol and displays data on an OLED screen.

## Hardware Components
- **Microcontroller**: ESP32
- **Sensor**: SHT30 (I2C)
- **Display**: SSD1306 OLED (I2C)

## Pin Configuration (I2C)
| Function | GPIO Pin |
|----------|----------|
| SDA      | 21       |
| SCL      | 22       |
| Frequency| 100 kHz  |

## Software Architecture

### 1. Initialization
- **I2C Master Init**: Configures GPIO pins and installs the I2C driver.
- **OLED Init**: Initializes the display and draws the static UI elements (icons for temperature and humidity).

### 2. Main Sensor Task (`sensor_display_task`)
The core logic runs in a dedicated FreeRTOS task:
1. **SHT30 Read**: Polled every 1000ms.
2. **Data Formatting**: Converts float values to strings.
3. **Display Update**: 
   - Renders temperature numbers and the °C symbol.
   - Renders humidity numbers and the % symbol.
4. **Serial Output**: The measurement data is printed to the UART console (Standard Output), which can be consumed by the Go server.

## Data Output Format
Currently, measurements are logged to the console. For production usage with the Go server, the UART output serves as the data bridge.

## Build and Flash
Using the provided `Makefile`:
```bash
make build  # Compile the firmware
make flash  # Flash to ESP32
```
