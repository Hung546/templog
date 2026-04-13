# Dependencies
- [EIM](https://docs.espressif.com/projects/idf-im-ui/en/latest/)
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v3.1.5/get-started/linux-setup.html)
<br>
- After installing ESP-IDF, you should make an alias for it in your shell config file. Because if you source it everytime you open a new terminal, it will take a short time to load it
E.g.
```bash
    alias idf="source /opt/esp-idf/export.sh"
```
## Build
```bash
idf.py build
```

or

```bash
make build
```

## Flash
```bash
idf.py flash
```
or

```bash
make flash
```
