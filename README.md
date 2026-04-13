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

- Install all necessary packages for Go server
```bash
go mod tidy
```
or
```bash
make pkg
```
- Install all necessary packages for website
```bash
cd web/ && bun i
```
## Build
```bash
make build
```

## Flash
```bash
make flash
```

## Web
- To run both web and server
```bash
make app
```
- Or run every single one
```bash
make web
```
or
```bash
make server
```
