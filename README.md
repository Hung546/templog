# Templog-monitoring
A real-time monitoring dashboard using a Go (Fiber v3) server, connected via WebSockets and UART for an ESP32-based temperature and humidity display on a LED screen

> UIT-CE103 project

## Dependencies
- [Bun](https://bun.com/)
- [Golang](https://go.dev/)
- Makefile
- [EIM](https://docs.espressif.com/projects/idf-im-ui/en/latest/)
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v3.1.5/get-started/linux-setup.html)
<br>
- After installing ESP-IDF, you should make an alias for it in your shell config file. Because if you source it everytime you open a new terminal, it will take a short time to load it

E.g.
```bash
alias idf="source /opt/esp-idf/export.sh"
```

- Install all necessary packages
```bash
make setup
```

## How to run
>[!IMPORTANT]
> Make sure you have the ESP32 connected to your computer via USB

<br>

- First, build the firmware
```bash
make build
```
- Then, flash the firmware (you only need to do this once)
```bash
make flash
```
- Then you need to monitor it via ESP-IDP (you only need to do this once)
```bash
make monitor
```
## Monitoring via the web
- To run both web and server
```bash
make app
```
