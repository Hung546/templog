# Backend Server

The backend is built with **Go** and **Fiber v3**. It acts as an MQTT subscriber and a WebSocket broadcaster.

## Infrastructure Requirements
The server expects the following services to be running on the host machine:
- **Mosquitto**: MQTT Broker listening on port 1883.
- **Avahi (mDNS)**: Advertises the `_mqtt._tcp` service so the ESP32 can find the server automatically.

## Core Modules

### 1. MQTT Subscriber (`internal/sensor/mqtt.go`)
- **Connection**: Connects to `tcp://localhost:1883`.
- **Subscription**: Listens to the topic `room/sensor/data`.
- **Parsing**: Unmarshals the incoming JSON (`temp`, `hum`) and converts it to the internal `SensorData` structure.
- **Integration**: Calls `h.Broadcast(data)` to push measurements to the frontend.

### 2. The Hub (`internal/hub/hub.go`)
- **WebSocket Management**: Handles browser connections.
- **Persistence**: Maintains a 60-point sliding window of sensor data in RAM.
- **Sync**: When a new browser tab opens, the Hub immediately sends the 60-second history so the chart isn't empty.

### 3. Web Framework (Fiber v3)
- **CORS**: Configured to allow connections from the Svelte dev server.
- **WebSocket Upgrade**: Handles the transition from HTTP to WS protocols.

## Linux Setup (Arch Linux Example)
```bash
# Install services
sudo pacman -S mosquitto avahi

# Start services
sudo systemctl start mosquitto avahi-daemon
```

## Running the Backend
```bash
make server  # Runs only the Go server
make app     # Runs both Go server and Svelte frontend using Bun
```
