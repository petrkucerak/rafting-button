# Changelog

## Etapa 1. - studium problematiky

- studium problematiky WiFi a IoT
  - typicke problemy: spotreba energie, vyuziti jiz postavene site, lze se spolehnout na rychlost (napr u MQTT)
- dava smysls smerovani na urovni MAC address?
  - teoreticky ano, nikde se neuziva, proce?
  - jak smerovat? Vyuziti modelu DNS?
- otazky
  - nebudou problem, kdyz budou 2 zarizeni moc blizko u sebe?
  - bude samotne vysilani ovlivnovat polarizace a volba kanalu?
  - jake specialni moznosti nabizi IEEE 802.11 pro IoT?
  - 

## Etapa 2. - volba modulu

- viz: [./docs/volba-modulu.md](volba-modulu.md)
- vitezem je ESP32-S2

## Etapa 3. - otestovani infrastruktury
### Zprovozneni modulu

#### 3.0.1 Bootovani a spusteni mikrontroleru

- treba pouzit nasledujici konfiguraci pro PlatformIO
```ini
[env:esp32dev]
platform = espressif32
board = esp32-s2-saola-1
framework = arduino

; config microcontroller
board_build.mcu = esp32s2
upload_protocol = esptool
board_build.f_cpu = 240000000L ; change MCU frequency
board_build.flash_mode = dio ; this option is important for ESP32-S2
```
- a nakonfigurovat `usbEventCallback` (*viz [3.0.1](https://github.com/petrkucerak/rafting-button/releases/tag/3.0.1)*)