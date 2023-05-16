Cílem tohoto projektu je odsimulovat reálné užití navrženého algoritmu pro synchronizaci času.

## Příkazy

```
idf.py build
idf.py -p <PORT> flash
idf.py monitor
```

## Schéma zapojení

![schema zapojeni](schema/schematic.svg)
![foto zapojeni](schema/zapojeni.jpg)

## MAC adresy modulu
  | NO  | type  | MAC address         | Note |
  | --- | ----- | ------------------- | ---- |
  | 1   | small | `70:b8:f6:5b:d3:24` | COM4 |
  | 2   | small | `cc:db:a7:1d:c4:08` | COM6 |
  | 3   | large | `c8:f0:9e:7b:10:8c` | COM7 |