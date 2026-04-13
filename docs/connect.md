# Connect to physical device

To connect to the real firmware, we need to modify both the **Firmware** (to print data to the Serial port in the correct format) and the **Go Server** (to read the Serial port instead of generating fake data).

I will guide you through the modification in 2 steps:

### Step 1: Modify Firmware (C)
In the file `firmware/main/templogger.c`, you need to add the `printf` command to push data to the USB port.

```c
// Find the section that updates the measurement in sensor_display_task
if (sht30_read(I2C_MASTER_NUM, &temp, &hum) == ESP_OK) {
    // ... OLED drawing code remains ...
    // ADD THIS LINE: Print to Serial port in the correct CSV format for easy server reading
    // Format: TEMP:xx.xx,HUM:yy.yy
    printf("DATA:%.2f,%.2f\n", temp, hum);

}
```

---
### Step 2: Modify Server Go
We need a library to read the Serial port. You run this command:

```bash
go get go.bug.st/serial
```

Then, we create the file `internal/sensor/serial.go` to replace `faker.go`:

```go
package sensor

import (
	"bufio"
	"fmt"
	"log"
	"strings"

	"go.bug.st/serial"
	"github.com/versenilvis/templog-monitoring/internal/hub"
)

func ReadSerial(h *hub.Hub, portName string) {
	mode := &serial.Mode{
		BaudRate: 115200,
	}
	port, err := serial.Open(portName, mode)
	if err != nil {
		log.Fatal("Cannot open Serial port: ", err)
	}

	scanner := bufio.NewScanner(port)
	for scanner.Scan() {
		line := scanner.Text()
		if strings.HasPrefix(line, "DATA:") {
			var temp, hum float64
			_, err := fmt.Sscanf(line, "DATA:%f,%f", &temp, &hum)
			if err == nil {
				h.Broadcast(hub.SensorData{
					Temperature: temp,
					Humidity:    hum,
					Timestamp:   time.Now(),
				})
			}
		}
	}
}
```

---

### Step 3: Modify `cmd/main.go`
Instead of calling to generate fake data, you call the function to read Serial:

```go

// cmd/main.go
func main() {
    h := hub.New(60)
    // Replace sensor.Faker(h) with:
    // Note: /dev/ttyUSB0 is the name The port on Linux and Windows will be COM3, COM4...
    go sensor.ReadSerial(h, "/dev/ttyUSB0")
    // ... Fiber code remains unchanged ...
}
```

*(Note: You need to know the exact name of the ESP32 port plugged into the machine, for example `/dev/ttyUSB0` or `/dev/ttyACM0`)*