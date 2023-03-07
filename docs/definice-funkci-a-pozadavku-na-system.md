# Definice funkcí a požadavků na systém

*Tento dokument popisuje funkce a dílčí požadavky na systém z pohledu: uživatele, softwaru, hardwaru a snaží se popsat základní koncepty k splnění jednotlivých požadavků.*

## Úvod

Cílem je vytvořit hlasovací zařízení pro aplikace jako je například hospodký kvíz, které bude řešit problematiku konsezu v distribuované síti.

### Požadované vlastnosti

**Zařízení by mělo:**

- být schopné fungovat na baterii
- být odolné vůči základnímu mechanickému poškození jako je pád ze stolu či nadměrné uhození do hlasovacího tlačítka
- mít jednoduché a intuitivní ovládání
- fungovat autonomně bez nutnosti uživatelské konfigurace
- moci aktivovat mód pro zobrazení výsledků
- fungovat v místnosti plné lidí
- zvládnout komunikovat s 1 až 10 zařízeními
- řešit problematiku konseznu v distribuovaném systému pomocí vhodného algoritmu

## Hardwarové požadavky

Zařízení by mělo obsahovat:

- **MCU**, který bude řídit celý systém,
- **baterii** pro bezdrátové fungování,
- **hlasovací tlačítko**, které bude odolné úhozu a bude sloužit k:
  - hlasování (krátký stisk),
  - přejití do/z prezentačního módu pro zobrazení výsledků (dlouhý stisk),
- **LED diodu** indikující stav daného zařízení,
- **ochrané pouzdro** pro uchranu před pádem či jiným poškozením.

### MCU

- Espressif architektura - využití protokolu ESP-NOW
- požadavky na PINs
  - *pro jednoduchý vývoj USB*
  - napájení přes baterii
  - digital IN - tlačítko
  - out (ideálně PWM) pro ovládání EGB LED sloužící pro signalizaci

### baterie

- fungování při rozmezí teplot teplotě 0-25˚C -> pokojová teplota i venekovní prostředí
- výdrž min. 3h plného fungování

### hlasovací tlačítko

- odolné mechanickému poškození
- *otestování - vyrobit prototyp (malý x velý - dát lidem do ruky)*

### RGB LED

- schopnost měnit barvy
- ovládání dle možností MCU (I2C, SPI, PWM)
- minimální schopnost 2 barev dle módu (lépe ale více i na ukázku fáze konfgurace a připojení)
- examples: [KY-016 RGB LED modul pro Arduino AVR, PIC, Raspberry - 3 barvy](https://dratek.cz/arduino/1403-ky-016-rgb-led-modul-3-barvy-pro-arduino-avr-pic-raspberry.html) - spíše nevhodné *(požadováno 5V)*

### ochranné pouzdro

- umožňuje stabilně položit na stůl či pařez stromu
- odolný vůči pádu
- odolný vůči dešti či střiku z vodní pistole
- snadno vyměnitelné bateri

## Softwarové požadavky

