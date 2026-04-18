package sensor

import (
	"encoding/json"
	"log"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/versenilvis/templog-monitoring/internal/hub"
)

type MQTTPayload struct {
	Temp float64 `json:"temp"`
	Hum  float64 `json:"hum"`
}

func ReadMQTT(h *hub.Hub) {
	opts := mqtt.NewClientOptions()
	opts.AddBroker("tcp://localhost:1883")
	opts.SetClientID("templog_backend")
	opts.SetAutoReconnect(true)
	opts.SetConnectRetry(true)
	opts.SetConnectRetryInterval(time.Second * 5)

	opts.OnConnectionLost = func(c mqtt.Client, err error) {
		log.Printf("[MQTT] Connection lost: %v", err)
	}

	client := mqtt.NewClient(opts)

	client.AddRoute("room/sensor/data", func(c mqtt.Client, m mqtt.Message) {
		var payload MQTTPayload
		if err := json.Unmarshal(m.Payload(), &payload); err != nil {
			log.Printf("[MQTT] Payload parse error: %v | Raw: %s", err, string(m.Payload()))
			return
		}
		h.Broadcast(hub.SensorData{
			Temperature: payload.Temp,
			Humidity:    payload.Hum,
			Timestamp:   time.Now(),
		})
	})

	opts.OnConnect = func(c mqtt.Client) {
		log.Println("[MQTT] Connected to broker at localhost:1883")
		// esp_mqtt_client_publish(..., 1, 0)
		if token := c.Subscribe("room/sensor/data", 1, nil); token.Wait() && token.Error() != nil {
			log.Printf("[MQTT] Subscribe error: %v", token.Error())
		}
	}

	if token := client.Connect(); token.Wait() && token.Error() != nil {
		log.Printf("[MQTT] Initial connect error: %v", token.Error())
	}

	select {}
}