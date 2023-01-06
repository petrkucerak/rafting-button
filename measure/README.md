# Meření

Tato složka obsahuje výsledky měření vlastností protokolu ESP NOW na zařízeních ESP32-S2.

## Infrastruktura

Měření je prováděno mezi 2 zařízeními, konkrétně `ESP32-S2-LCD` a `ESP32-S2-pico`.

## Software

Na obou dvou zařízení běží program, který se snaží simulovat v budoucnu žádaný provoz.

Konkrétně:
1. odeslat data pomocí *broadcastu* `(device 1)`
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

### Scénář A1

| PARAMETR       | HODNOTA                                         |
| -------------- | ----------------------------------------------- |
| **prostředí**  | byt, v kterém se je rušení několika WiFi sítěmi |
| **překážka**   | volný vzduch                                    |
| **vzdálenost** | 50 cm                                           |
| **velikost**   | 25 bajtů                                        |
| **počet**      | 10 000 zpráv                                    |

