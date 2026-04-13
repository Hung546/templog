package sensor

import (
	"math"
	"math/rand"
	"time"

	"github.com/versenilvis/templog-monitoring/internal/hub"
)

// Fake simulates realistic SHT30 output with slow drift + noise
// when you're ready to use real serial, swap this out for a serial reader
func Faker(h *hub.Hub) {
	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()

	var (
		baseTemp = 27.0
		baseHum  = 60.0
		tick     = 0.0
	)

	for range ticker.C {
		tick++

		temp := baseTemp + 2.5*math.Sin(tick/30) + (rand.Float64()-0.5)*0.4
		hum := baseHum + 5.0*math.Cos(tick/45) + (rand.Float64()-0.5)*0.8

		h.Broadcast(hub.SensorData{
			Temperature: math.Round(temp*100) / 100,
			Humidity:    math.Round(hum*100) / 100,
			Timestamp:   time.Now(),
		})
	}
}
