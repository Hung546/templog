# Communication Protocol

This document defines the data format used between the different layers of the TempLog system.

## 1. Server to Client (WebSocket)
All WebSocket messages are sent as JSON strings with the following structure:

```json
{
  "type": "string",
  "data": "any"
}
```

### Type: `history`
Sent immediately after a client successfully connects to `/ws`.
- **Data**: Array of `SensorData` objects.
- **Purpose**: Restores the chart state after a page reload.

### Type: `live`
Sent every second when new sensor data is available.
- **Data**: A single `SensorData` object.

### Data Object: `SensorData`
```json
{
  "temperature": 27.50,
  "humidity": 62.15,
  "timestamp": "2026-04-13T22:45:00Z"
}
```

---

## 2. Firmware to Server (UART/Serial) - Planned
The bridge from hardware to software.

### Format: CSV String
To minimize processing overhead on the ESP32, data should be sent as a comma-separated string followed by a newline:
`T:27.50,H:62.15\n`

### Processing Logic
1. Go server listens on `/dev/ttyUSB0` (or similar).
2. Parses strings using regex or `fmt.Sscanf`.
3. Creates a `SensorData` object with a server-side timestamp.
4. Passes the object to `hub.Broadcast()`.
