# Communication Protocol

This document defines the data formats for the MQTT and WebSocket layers.

## 1. ESP32 to Broker (MQTT)
The firmware publishes data to the following topic:

**Topic**: `room/sensor/data`  
**Payload Format**: JSON  
**QoS**: 1 (At least once)

### Payload Example
```json
{
  "temp": 26.45,
  "hum": 58.20
}
```

---

## 2. Server to Client (WebSocket)
The Go server communicates with the Svelte frontend via WebSockets at `ws://localhost:8080/ws`.

### Message Structure
```json
{
  "type": "history | live",
  "data": "SensorData | []SensorData"
}
```

- **type: `history`**: Sent once upon connection. Contains an array of the last 60 readings.
- **type: `live`**: Sent every time the MQTT subscriber receives a message. Contains a single reading.

### SensorData Schema
```json
{
  "temperature": 26.45,
  "humidity": 58.20,
  "timestamp": "2026-04-18T22:00:00Z"
}
```

---

## 3. Service Discovery (mDNS)
The server uses **Avahi** to advertise its presence so the ESP32 doesn't need a hardcoded IP.

- **Service Type**: `_mqtt._tcp`
- **Port**: `1883`
- **Hostname**: `[machine_name].local`
