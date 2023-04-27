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