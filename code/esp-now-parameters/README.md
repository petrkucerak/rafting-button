Cílem tohoto projektu je změřit **latenci protokolu esp-now** běžícího na modulech **esp32**. To takovým způsobem, že jsou všechna zařízení propojena vdičem, který synchronizuje jejich vnitřní hodiny a po odeslání se spočítá čas příjmu dané zprávy.

## Příkazy

```
idf.py build
idf.py -p <PORT> flash
idf.py monitor
```

## Schéma zapojení

![schema zapojeni](schema/schematic.svg)
![foto zapojeni](schema/zapojeni.jpg)

## Koncept měření

Měřící sestava obsahuje 3 ESP32 a jedno STM32 (STM32G431KB používá se pro výuku předmětu LPE). STM32 generuje pomocí PWM pulz, který slouží k syncrhonizaci hodin v DS. ESP32 mezi sebou vysílají zprávu pomocí níž se spočítá latence.

**Měření latence** probíhá přesně následujícím způsobem:
1. Zařízení A odešle do zařízení B a C zprávu se synchronizovaným časem odeslání
2. Zařízení B a C zprávu přijmou a spočítají latenci, tedy dobu od odeslání (zapasání zprávy se synchronziovaným časem) do přijmutím zprávy (spuštění callback funkce a provedení výpočtu latence). Latence se následně počítá jako
   $$
   t_l = T_{B,C}-T_A
   $$
   - $T_l$ ... velikost latence
   - $T_A$ ... čas ve zprávě, resp. čas odeslání
   - $T_{B,C}$ ... čas přijmutí zprávy

**Čas je** v ESP32 **synchronizován** pomocí pulzů z PWM generovaného STM32. To tak, že při registraci náběžné hrany na vstupním pinu (nastaveno na GPIO_NUM_21) se generuje přerušení (ISR), které uloží aktulání hodnotu globálního času od spuštění a prohlásí ji za výchozí čas synchronizace ($T_s$). Čas DS se následně počít jako

   $$
   T_{DS} = T_g - T_s
   $$
   - $T_{DS}$ ... čas synchronizovaný v DS
   - $T_g$ ... globální čas běhu daného zařízení
   - $T_s$ ... doba synchornizace času

**Veškeré naměření hodnoty jsou udávány v *µs*.**

### Technické poznámky

- Čas je synchronziován s průměrnou odchylkou okolo *173 µs*. Tato hodnota byla určena měřením kdy je generován pulz o frekvenci 1Hz, perioda je tedy 1 s. A ISR handler funkce vždy spočítá čas čas od předchozího běhu a vynuluje $T_s$. Zpoždění je dáno režijními náklady běhu procesoru. Maximální odchylka byla určená také měřením a při běhu 24 min a 18 s dosáhla maximální hodnoty *810 µs*.
  
  Původně jsem se domníval, že overhead je veliký především kvůlu funkci `esp_timer_get_time()` z důvodu dohledané diskuze https://esp32.com/viewtopic.php?t=16228. Experimentem jsem ale ověřil, že tato funkce není hlavní brzdou při měření. Prodluvu se mi ale povedlo snížit přetaktováním na 240 MHz a optimalizací kódu.
  
  *Nepřesnost v tomto měření může způsobit i skutečnost, že nevím s jakou přesností je generován referenční signál.*
- MAC adresy modulu
  | NO  | type  | MAC address         | Note |
  | --- | ----- | ------------------- | ---- |
  | 1   | small | `70:b8:f6:5b:d3:24` | COM4 |
  | 2   | small | `cc:db:a7:1d:c4:08` | COM6 |
  | 3   | large | `c8:f0:9e:7b:10:8c` | COM7 |


## Výsledky měření latence

Měření jsem spustil ve dvou scénářích. Lišily se pouze časem běhu. Výsledné hodnoty jsou totožené, pouze se liší množstvím odeslaných a přijatých zpráv.

| Měření            | Zařízení | střední hodnota | maximální hodnota | minimální hodnota |
| ----------------- | -------- | --------------- | ----------------- | ----------------- |
| A *(cca 6.5 min)* | COM6     | 1308.59 µs      | 14944 µs          | 899 µs            |
|                   | COM7     | 1256.00 µs      | 19471 µs          | 883 µs            |
| B *(cca 37 min)*  | COM6     | 1152.03 µs      | 13239 µs          | 893 µs            |
|                   | COM7     | 1134.67 µs      | 25143 µs          | 871 µs            |

![A scenario](measure/esp-latency/latency-A.png)
![B scenario](measure/esp-latency/latency-B.png)
