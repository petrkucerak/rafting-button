# Changelog

## Samostatný projekt

### Etapa 1. - studium problematiky

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

### Etapa 2. - volba modulu

- viz: [./docs/volba-modulu.md](./docs/volba-modulu.md)
- vitezem je ESP32-S2

### Etapa 3. - otestovani infrastruktury
#### Zprovozneni modulu

##### 3.0.1 Bootovani a spusteni mikrontroleru

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


### Etapa 4. - závěrečný report

Závěrečný report k samostatnému proejtku je možný ke stažení v Issues [#9](https://github.com/petrkucerak/rafting-button/issues/9) na odkaze: https://github.com/petrkucerak/rafting-button/files/10468332/kucerp28_samostatny-projekt.zip

---

## Bakalářská práce

### Etapa 5. - 1. Definujte funkcní a dalsí pozadavky na systém

#### jaký hardware

[Volba modulu 2.0](docs/volba-modulu.md)

#### multi modes

- ESP může fungovat 2 módech: AP (Access Point), STA (Station Mode)
- [Článek, jak aktivovat ESP-NOW a Wifi zároveň](https://www.electrosoftcloud.com/en/esp32-wifi-and-esp-now-simultaneously/)
  - v módu STA pravděpodobně nefungovalo, protože při neaktivitě modul přecházel do spánku
- [Diskuze na Arudion Foru](https://forum.arduino.cc/t/use-esp-now-and-wifi-simultaneously-on-esp32/1034555/16)
- Jednoduse ano: tento mod by mel fungovat, otazka vypocetni sily MC -> třeba provést testy

#### protokol

##### Algoritmus pro vytvoření infrastruktury

- bude třeba nějaký implementovat nebo jednoduše spojím všechny zařízení mezi sebou? v specifikaci limit 10 zařízení, bude stačit?
- pokud ne, bylo by třeba hledat alogritmus pro vytvoření clusterů

##### Algoritmus pro konsenzus

- Z předmětu PDV známo několik algoritmů a způsobů, které by šlo aplikovat
- Idea ukládat pořadí jako pole, které bude replikované na všechny *nodes*

| řešení                              | zjednodušený model     | garance             | algritmy                    |
| ----------------------------------- | ---------------------- | ------------------- | --------------------------- |
| zavedení logického času             | asynchronost           | bezpečnost a živost | Lamportovy vektorové hodiny |
| globální spatshot                   | asynchronost           | bezpečnost a živost | Chandy-Lamport              |
| propracovaný alogritmus na konsezus | asynchronost a selhání | bezpečnost          | RAFT                        |

- PTP? -> spíše ne, nevidím výhodu v ladění přesné časové značky, stačí logický čas -> synchronizace na zaklade logickych casovych znacek

###### Lamportovy vektorové hodiny

- každé zařízení má své logické hodiny
- při události inkemetuje lokální vektorové hodiny a každá odeslaná zpráva jedny své hodiny má, při přijetí zvyšuje hodnotu na vyšší
- vektorové, aby determinovali souběžnost
- mohu pomocí tohto alogoritmu vůbec garantovat?

###### Chandy Lamport - globální snapshot

- není řešení problému, spíše se možné inspirovat při navržení vlastního alogritmu

###### RAFT

- možná až moc komplikovaný
- na druhou stranu zajišťuje i detekci chyby (toto ale možná lepší řešit na úrovni algorimu pro propojení)
- absence živosti - to by mohl byt problem

### Otázky k diskuzi
- pro ovládání: display nebo RGB LED?
- display i na zobrazování výsledků
- joystik - pro masivnejsi ovladani
- baterie vs akumuator (nabijeni z USB)