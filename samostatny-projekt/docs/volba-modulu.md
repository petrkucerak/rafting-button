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


## Volba modulu 2.0

- z rodiny ESP32 nebo ESP8266
- preferencie ESP32 - výkonejší
- výherce klasická ESP32S - nove naskladněná oproti času před Vánocemi
- dostupná na Eshops:
  - https://rpishop.cz/esp32-a-esp8266/1500-esp32-vyvojova-deska.html
  - https://www.briv.cz/p/5349/esp-wroom-32-esp32-esp-32s-2-4ghz-vyvojarska-deska-s-wifi-bt
  - https://e-shop.prokyber.cz/esp32/esp32-devkitv1/ (200)
- porovnani verzi
  - https://docs.espressif.com/projects/esp-idf/en/v4.3.1/esp32/hw-reference/chip-series-comparison.html
  - https://gist.githubusercontent.com/sekcompsci/2bf39e715d5fe47579fa184fa819f421/raw/df6314887307e656470fbcf072ff4c516dd4afff/Comparison%2520Espressif%2520ESP%2520MCUs.md
- final eshops:
  - https://dratek.cz/arduino/1581-esp-32s-esp32-esp8266-development-board-2.4ghz-dual-mode-wifi-bluetooth-antenna-module.html
  - https://www.briv.cz/p/5349/