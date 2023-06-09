# Testování systému pomocí Arduino modulu

Složka obsahuje kód pro ovládání Arduino UNO boardu používaného pro testování vlastností algoritmu.

## Funkce

Soubour obashuje 2 funkce, pro krátký a dlouhý stisk.

```C
void pushButton(uint8_t device) {
  digitalWrite(device, HIGH);
  delay(10);
  digitalWrite(device, LOW);
}
```
```C
void longPushButton(uint8_t device) {
  digitalWrite(device, HIGH);
  delay(6000);
  digitalWrite(device, LOW);
}
```

## Testy

Během testování jsem se soustředil na 3 oblasti:

1. normální běh,
2. stanovení časového limitu, při kterém se zařízení stále zachovává kauzalitu událostí,
3. problematické scénáře jako je připojení a odpojení zařízení do sítě.

## Zapojení použité při měření

![Zapojení použité pro měření](../../../slideshow/measure-set.jpg)