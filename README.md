# Rafting button

Repozitář s bakalářskou prací, která si klade za cíl vytvořit autonomní zařízení řešící problém konsenzu v distribuovaném systému.

Výslednou softwarovou implementaci je možné nalézt ve složce [code/rafting-button](/code/rafting-button/).

Dokumentace finální implementace je dostupná na adrese: https://petrkucerak.github.io/rafting-button/.

## O projektu
### Motivace
Už od útlých dětských let jezdím na letní tábory a přes rok organizuji mládežnické akce. Na *Hudebním týdnu*, jedné z akcí pro mladé, tradičně hráváme hru, ve které jsou různé týmy, které mezi sebou soutěží. Cílem je se jako první přihlásit o slovo a zodpovědět otázku. Doposud se nám nepovedlo vymyslet efektivní způsob, jak určit pořadí týmů. Ve výsledku vždy použijeme zvoneček, který při odpovědí více týmů ve stejný čas není vhodným řešením.

### Popis cíle bakalářské práce
Cílem bakalářské práce je vytvořit koncept hlasovacího tlačítka, které bude fungovat
jako autonomní zařízení v distribuovaném systému. Systém pomocí vhodných algoritmů
bude řešit problematiku konsenzu, respektive kauzálního uspořádaání stisku tlačítek.

### Popis hlasovacího zařízení

Z uživatelského pohledu by zařízení mělo být kompaktní krabička, kterou si bude moci skupina položit před sebe na stůl.2 Krabička by na sobě měla mít tlačítko, které bude odolné i vůči veliké síle způsobené bouchnutí v zápalu hry. Zařízení bude možné přepínat mezi dvěma módy, tj. *PRESENTER* a *NORMAL*. V módu *NORMAL* bude zařízení řešit,jakém pořadí se týmy přihlásily o slovo. V módu *PRESENTER* navíc pomocí webového serveru vykreslí výsledky.

Samotná hra pak bude probíhat tak, že bude položena otázka, hráči se přihlásí o slovo
stiskem tlačítka. Informace se rozdistribuujesíti a zařízenímódu *PRESENTER* zobrazí
výsledky. Hlasování se vyresetuje dlouhým stisknutím hlasovacího tlačítka na jakémkoliv zařízení

## Struktura

- [samostatny-projekt](/samostatny-projekt/) - složka obsahující veškeré soubory týkající se samostatného projektu
- [thesis](/thesis/) - složka s bakalářskou prací
- [code](/code/) - složka s softwarovými implementacemi
- [assets](/assets/) - složka s relevantními zdroji
- [slideshow](/slideshow/) - složka s slideshow k závěrečné prezentaci