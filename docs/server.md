# Backend Server

The backend is built with **Go** using the **Fiber v3** web framework. It handles real-time data broadcasting and historical data persistence via WebSockets.

## Tech Stack
- **Framework**: [Fiber v3](https://docs.gofiber.io/)
- **WebSocket**: [Fiber Contrib WebSocket](https://github.com/gofiber/contrib)
- **Runtime**: Go 1.26+

## Core Modules

### 1. The Hub (`internal/hub`)
The `Hub` manages all active WebSocket connections and maintains a sliding window of historical data.
- **Persistence**: Stores the last 60 sensor readings in memory.
- **Broadcast**: When new data arrives, it:
  1. Updates the internal history slice.
  2. Prints the value to the server console.
  3. JSON marshals the data and sends it to all connected clients.
- **Registration**: When a new client connects, the Hub immediately sends the current history slice before starting live updates.

### 2. Sensor Simulation (`internal/sensor`)
Currently, the server uses a `Faker` sensor module to simulate SHT30 readings:
- Uses sinusoidal functions with random noise to mimic realistic environmental drift.
- Broadcasts updates every 1 second.
- **TODO**: Replace with a real Serial/UART reader to consume data from the physical ESP32.

### 3. API Routes
- `GET /health`: Basic health check returning `{"status":"ok"}`.
- `GET /ws`: WebSocket endpoint. Upgrades HTTP connections to persistent WebSocket tunnels.

## Data Persistence Strategy
To prevent the frontend chart from clearing on page reloads:
1. The server maintains a `[]SensorData` slice.
2. The initial Message Type `history` is sent to the client upon connection.
3. Subsequent updates use the Type `live`.

## Running the Server
```bash
make server  # Runs the Go backend
make app     # Runs both backend and Svelte frontend
```
