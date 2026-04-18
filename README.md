<div align="center">
  <img width=10% alt="ce" src="https://github.com/user-attachments/assets/b98ff0ac-1f45-4ff7-aa04-ab652528cbdd" />
  <h1>Templog-monitoring</h1>
  <p><b>A real-time temperature and humidity monitor ESP32 over MQTT, Go backend, WebSocket frontend</b></p>
  <p><a href="#installation"><strong>Installation »</strong></a></p>
  <img src="https://camo.githubusercontent.com/b16ecdcac9c3d21ec3a49459430f747b46b3a37acc95ee468d87d0ec61ff2392/68747470733a2f2f692e696d6775722e636f6d2f576d4d6e5352742e706e67">
</div>

>[!IMPORTANT]
> This branch uses WiFi and MQTT. ESP32 has built-in WiFi, no extra hardware needed  
> Switch to the [`firmware-usb`](https://github.com/versenilvis/templog-monitoring/tree/firmware-usb) branch if you prefer the USB/UART version

## Under the hood
The ESP32 boots and connects to WiFi. It then uses mDNS to discover any `_mqtt._tcp` service on the local network, so no hardcoded hostname is needed. Once it finds the broker, it connects and publishes `{"temp": xx.xx, "hum": xx.xx}` to the topic room/sensor/data every 2 seconds

The Go server subscribes to that topic, parses the JSON payload, and broadcasts the data over WebSocket to the frontend

## Dependencies
- [Bun](https://bun.com/)
- [Golang](https://go.dev/)
- Makefile
- [EIM](https://docs.espressif.com/projects/idf-im-ui/en/latest/)
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v3.1.5/get-started/linux-setup.html)

> [!NOTE]
> After installing ESP-IDF, you should make an alias for it in your shell config file  
> Because if you source it everytime you open a new terminal, it will take a short time to load it

E.g.
```bash
alias idf="source /opt/esp-idf/export.sh"
```

## Installation
> [!WARNING]
> I'm using Arch Linux, so my commands will use `pacman`, it won't work the same as your distro  
> If you use another distro, remember to change the command to your distro's package manager

- Install all necessary packages
```bash
make setup
```

- Install Avahi

`sudo pacman -S mosquitto avahi`

- Configure Mosquitto (MQTT Broker)


Open file: `sudo nano /etc/mosquitto/mosquitto.conf` (or use your own editor)

```
listener 1883
allow_anonymous true
```

- Configure Avahi (mDNS Service Discovery)

Open file: `sudo nano /etc/avahi/services/mqtt.service`

```xml
<?xml version="1.0" standalone='no'?>
<!DOCTYPE service-group SYSTEM "avahi-service.dtd">
<service-group>
  <name>MQTT Broker</name>
  <service>
    <type>_mqtt._tcp</type>
    <port>1883</port>
  </service>
</service-group>
```

- Start services
```bash
sudo systemctl start mosquitto
sudo systemctl start avahi-daemon
```

- Check

```bash
# Check broker is running
sudo systemctl status mosquitto

# Check avahi is advertising
avahi-browse _mqtt._tcp
```
Expected:
```
+ wlp1s0 IPv4 MQTT Broker _mqtt._tcp local
```

## How to run

<br>

- First, build the firmware
```bash
make build
```
- Then, flash the firmware (you only need to do this once)
```bash
make flash
```
- Then you need to monitor it via ESP-IDF (you only need to do this once)
```bash
make monitor
```
## Monitoring via the web
- To run both web and server
```bash
make app
```

<div align="center">
  
</div>
