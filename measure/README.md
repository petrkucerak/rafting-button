# Meření

Tato složka obsahuje výsledky měření vlastností protokolu ESP NOW na zařízeních ESP32-S2.

## Infrastruktura

Měření je prováděno mezi 2 zařízeními, konkrétně `ESP32-S2-LCD` a `ESP32-S2-pico`.

## Software

Na obou dvou zařízení běží program, který se snaží simulovat v budoucnu žádaný provoz.

Konkrétně:
1. odeslat data pomocí *broadcastu/unicastu* `(device 1)`
2. počkat na data `(device 2)`
3. uložit data pomoci *handleru* `(device 2)`
4. odeslat odpověď `(device 2)`
5. počkat na odpověď `(device 1)`
6. změřit dobu celého procesu `(device 1)`

*Přesná implementace viz: [3.0.2](https://github.com/petrkucerak/rafting-button/tree/3.0.2).*

## Scénáře

Měření byly sledovány následující paramtery:
- prostředí, v němž bylo měření prováděno
- překážka mezi zařízeními
- vzdálenost mezi zařízeními
- velikost zprávy *(payload)* - maximální veliksot zprávy je 255 bajtů
- počet odeslaných zpráv
- typ odesílání: ***broadcast*** / ***unicast***
- počet chybných zpráv (až u pozdějších měření)

## Skripty

```pw
C:\.platformio\penv\Scripts\platformio.exe device monitor > measure/logs/log-<A1>.txt
```

*Skripty nejsou univerzální, záleží na místě instalace PlatformIO.*
### A scénaře

Scénáře typu A se snaží zmapovat vliv velikosti zprávy na zpoždění odeslání a typu odesílání (*broadcastu/uniscastu*).

#### Broadcast

| SCÉNÁŘ         | A11                 | A12                 | A13                 | A14                 | A15                 |
| -------------- | ------------------- | ------------------- | ------------------- | ------------------- | ------------------- |
| **prostředí**  | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) |
| **překážka**   | volný vzduch        | volný vzduch        | volný vzduch        | volný vzduch        | volný vzduch        |
| **vzdálenost** | 50 cm               | 50 cm               | 50 cm               | 50 cm               | 50 cm               |
| **velikost**   | **1 bajt**          | **10 bajt**         | **50 bajt**         | **120 bajt**        | **250 bajt**        |
| **počet**      | **10 000 zpráv**    | **10 000 zpráv**    | **10 000 zpráv**    | **1 000 zpráv**     | **5 000 zpráv**     |
| **typ**        | **broadcast**       | **broadcast**       | **broadcast**       | **broadcast**       | **broadcast**       |

#### Unicast

| SCÉNÁŘ         | A21                 | A22                 | A23                 | A24                 | A25                 |
| -------------- | ------------------- | ------------------- | ------------------- | ------------------- | ------------------- |
| **prostředí**  | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) | byt (s WiFi sítěmi) |
| **překážka**   | volný vzduch        | volný vzduch        | volný vzduch        | volný vzduch        | volný vzduch        |
| **vzdálenost** | 50 cm               | 50 cm               | 50 cm               | 50 cm               | 50 cm               |
| **velikost**   | **1 bajt**          | **10 bajt**         | **50 bajt**         | **120 bajt**        | **250 bajt**        |
| **počet**      | 10 000 zpráv        | 10 000 zpráv        | 10 000 zpráv        | 10 000 zpráv        | 10 000 zpráv        |
| **typ**        | **unicast**         | **unicast**         | **unicast**         | **unicast**         | **unicast**         |

### B scénaře
Scénáře typu **B** se snaží analyzovat dobou odeslání v závislosti na překážce v cestě 2 modulů.

#### Scénář B1

| PARAMETR       | HODNOTA                                         |
| -------------- | ----------------------------------------------- |
| **prostředí**  | byt, v kterém se je rušení několika WiFi sítěmi |
| **překážka**   | zeď se skříní                                   |
| **vzdálenost** | cca 6 m                                         |
| **velikost**   | 120 bajtů                                       |
| **počet**      | 10 000 zpráv                                    |
| **typ**        | unicast                                         |

### C scénaře
Scénáře typu **C** se snaží analyzovat dobou odeslání v závislosti na vzdálenosti 2 modulů v čistém prostředí v obou typech odesílání (*unicast/brodcast*).

| SCÉNÁŘ                   | C1                | C2                | C3                | C4                | C5                | C6                |
| ------------------------ | ----------------- | ----------------- | ----------------- | ----------------- | ----------------- | ----------------- |
| **prostředí**            | les (bez 2.4 GHz) | les (bez 2.4 GHz) | les (bez 2.4 GHz) | les (bez 2.4 GHz) | les (bez 2.4 GHz) | les (bez 2.4 GHz) |
| **překážka**             | vzduch            | vzduch            | vzduch            | vzduch            | vzduch            | vzduch            |
| **vzdálenost**           | **0,5 m**         | **25 m**          | **50 m**          | **100 m**         | **100 m**         | **50 m**          |
| **velikost**             | 125 bajtů         | 125 bajtů         | 125 bajtů         | 125 bajtů         | 125 bajtů         | 125 bajtů         |
| **počet**                | 5 000 zpráv       | 5 000 zpráv       | 5 000 zpráv       | 5 000 zpráv       | 5 000 zpráv       | 5 000 zpráv       |
| **počet chybných zpráv** | 0 zpráv           | 3 zpráv           | 15 zpráv          | 35 zpráv          | 15 zpráv          | 6 zpráv           |
| **typ**                  | **unicast**       | **unicast**       | **unicast**       | **unicast**       | **brodcast**      | **brodcast**      |

### D scénář
V rámci scénáře typu D jsem se snažil otestovat technologii při maximální vzdálenosti a poměřit úspěch úspěšně odeslaných a neodeslaných zpráv.

| SCÉNÁŘ                   | D1               |
| ------------------------ | ---------------- |
| **prostředí**            | pole (s 2.4 GHz) |
| **překážka**             | vzduch           |
| **vzdálenost**           | **577 m**        |
| **velikost**             | 125 bajtů        |
| **počet**                | 1 000 zpráv      |
| **počet chybných zpráv** | 50 zpráv         |
| **typ**                  | **unicast**      |