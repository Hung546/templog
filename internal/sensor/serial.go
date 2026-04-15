package sensor

import (
	"bufio"
	"fmt"
	"log"
	"strings"
	"time"

	"github.com/versenilvis/templog-monitoring/internal/hub"
	"go.bug.st/serial"
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
