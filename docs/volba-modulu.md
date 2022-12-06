# Volba modulu

## Požadavky

- podpora standardů (b/g/n)
- knihovna pro práci s MAC frames
- pin pro zapojení tlačítka

## Typy zařízení

### ESP32

- možnost využití Arduino wifi knihovny, treba modifikace pro praci s MAC frames
- https://github.com/espressif/esp-idf

### ESP8266

- https://github.com/espressif/ESP8266_RTOS_SDK
- [https://www.espressif.com/en/products/socs/esp8266](https://www.espressif.com/en/products/socs/esp8266)

### Raspberry PI Pico

### **CC3200**

| PARAMETRS | ESP32 | ESP8266 | PI Pico | CC3200 | ATSAMW25 |
| --- | --- | --- | --- | --- | --- |
| Processor | Tensilica
 Xtensa LX6
 (240 MHz) | Tensilica
 Xtensa L106
 (80 or 160 MHz) | Cortex-M0+ | ARM Cortex-M4
 (80 MHz) | ARM Cortex-M0+
 (48 MHz) |
| Protocols | IEEE 802.11b/g/n | IEEE 802.11b/g/n | IEEE 802.11b/g/n | IEEE 802.11b/g/n | IEEE 802.11b/g/n |
| Flash |  |  |  |  |  |