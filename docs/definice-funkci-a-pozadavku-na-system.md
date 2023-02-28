# Definice funkcí a požadavků na systém

Tento dokument popisuje funkce a dílčí požadavky na systém z pohledu: uživatele, softwaru, hardwaru a snaží se popsat základní koncepty k splnění jednotlivých požadavků.

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

### Hardwarové komponenty

Zařízení by mělo obsahovat:

- **MCU**, který bude řídit celý systém,
- **baterii** pro bezdrátové fungování,
- **hlasovací tlačítko**, které bude odolné úhozu a bude sloužit k:
  - hlasování (krátký stisk),
  - přejití do/z prezentačního módu pro zobrazení výsledků (dlouhý stisk),
- **LED diodu** indikující stav daného zařízení,
- **ochrané pouzdro** pro uchranu před pádem či jiným poškozením.
