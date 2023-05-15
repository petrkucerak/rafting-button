# Simulace alogritmu pro synchronizaci času v distribuovaném systému

## Scripts

```
make clean && make && ./main
make clean && make && ./main > output.txt && python3 visualize.py 
```

## Problematika synchronizace času

Požadavek na přesnost synchronizace času 1 ms. Tato hodnota vychází z reakční doby člověka, která se u špičkových atletů při startu pohybuje okolo 0,1 až 0,25 s. Pokud tedy stanovíme hodnotu o dva řády nižší, nemělo by dojít ke kolizím.

### Detailní popis zvoleného algoritmu

Doba přenosu zprávy z uzlu $N_A$ do uzlu $N_B$ trvá dobu $D_n$ s chybou $O_n$, přičemž platí, že $T_A$ je hodnota hodin v uzlu $N_A$ a $T_B$ je hodnota hodin v uzlu $N_B$. Pak o přenosu zprávy platí, že:

$$
T_A = T_B + D_n + O_n.
$$

Při fungování systému nejsme schopni zjistit absolutní hodnotu $D_n$ a $O_n$, proto počítáme s průměrovanou hodnotou *aritmetický průměr* zpoždění $\bar{D}$ a chybou počítanou klouzavým průměrem $\bar{O}$. Toto zjednodušení způsobuje nepřesnosti při synchronizaci času. Po přidání průměrných hodnot tedy platí, že:

$$
T_A = T_B + \bar{D} + \bar{O},
$$

kde doba přenosu $D_n$ je počítána jako

$$
D_n = {{RTT}\over{2}}
$$
a velikost chyby $O_n$ jako

$$
O_n = T_B - T_A - \bar{D}.
$$

Celkově algoritmus funguje tak, že z uzlu *MASTER* je jednou za 100 ms odešle do všech *SLAVE* uzlů zpráva inicializující synchronizaci doby přenosu a jednou za 500 ms zprávu inicializující synchronizaci času.

Průběh zprávy pro synchronizaci doby přenosu je takový, že:


* Z uzlu *MASTER* se odešle zpráva s lokálním časem do uzlu *SLAVE*.
* *SLAVE* uzel přijme zprávu a se stejným obsahem ji okamžitě odešle zpět do uzlu, z kterého zprávu obdržel. Tedy do uzlu typy *MASTER*.
* Po obdržení času se v uzlu *MASTER* se z přijaté hodnoty a aktuálního lokálního času vypočítá doba přenosu jako
$$
D_n = {{T_B - T_A}\over{2}},
$$
kde $T_A$ je čas odeslaný v situaci {\sbf [A]}, tedy na začátku celého procesu a $T_B$ je čas v situaci {\sbf [C]}, tedy po přijetí zprávy od uzlu *SLAVE*. Po vypočítání se hodnota odešle do uzlu *SLAVE*.
* Uzel *SLAVE* přijatou hodnotu zapíše do pole, aby mohla být později použita k výpočtu průměrné doby přenosu $\bar{D}$.


Výpočet doby zpoždění předpokládá že je doba přenosu symetrická. Tedy, že průměrné odesílání z *MASTER* do *SLAVE* trvá stejnou dobu jako odesílání z *SLAVE* do *MASTER.

Průběh zprávu pro synchronizaci času je takový, že:


* Z uzlu *MASTER* se odešle zpráva s lokálním časem do uzlu *SLAVE*.
* Po přijetí zprávy dojde k výpočtu průměrné chyby $\bar{O}$ a nastavení lokálního času na uzlu *SLAVE* podle reference z uzlu *MASTER*. Průměrná chyba se počítá s užitím klouzavého průměru jako 
$$
\bar{O} = \bar{O} O_n = \bar{O} (T_S - T_M - \bar{D}),
$$
kde $T_M$ je čas odeslaný z uzlu *MASTER* a $T_S$ je aktulaní hodnota času v uzlu *SLAVE*.

Čas se následně nastaví v závislosti na velikosti odchylky. Pokud je větší než konstanta $\bar{O} > | k |$,

> Chyba je způsobena především rozdílem rychlosti běhu oscilátorů v zařízeních. Podle výrobce může odchylka dosahovat až ±10 ppm. Pokud je tedy hodnota chyby větší než cca 100 µs, jedná se o chybu v době přenosu. Proto v takové situaci nastavíme maximální hodnotu rovnou na velikost konstanty. V našem případě tedy 100 µs nebo -100 µs, podle orientace.

hodnota hodin je nastavena jako:

$$
T_S = T_M + \bar{D} \pm k.
$$

V případě, že $\bar{O} \leq | k |$, pak je čas nastaví jako:

$$
T_S = T_M + \bar{D} + \bar{O}.
$$

### Simulace fungování algoritmu

Pro ověření toho, zdali navržený algoritmus splňuje definovanou podmínku přesnosti synchronizace (1 ms), jsem připravil simulaci. Simulace je naprogramovaná v jazyce C. Implementaci si je možné prohlédnout v repozitáři projektu.
> Tato simulace je uložena konkrétně na adrese {https://github.com/petrkucerak/rafting-button/tree/main/code/simulation}.

Simulace se skládá z hlavních třech částí:

* inicializace prostředku a konfigurace parametrů simulace,
* smyčka realizující samotný běh simulace
* a rutina pro vyčištění alokovaných prostředků.

V simulaci je možné měnit parametry nastavující:

* dobu simulace (udávaná v µs),
* počet uzlů (maximální počet je 255),
* počáteční čas simulace,
* status uzlu,
> Standartě je jeden z uzlů v režimu *MASTER* a ostatní v režimu *SLAVE*.
* chybu oscilátoru na každém z uzlů,
* počáteční čas času uzlů, (udávaný v µs),
* dobu přenosu zprávy mezi uzly,
* velikost pole pro výpočet průměrného zpoždění,
* konstantu pro určení maximální odchylky.

Běh simulace pak probíhá jako smyčka omezená dobou. Běh jednoho cyklu představuje dobu 1 µs. V prvním kroku se nastaví náhodné zpoždění pro daný běh na všech uzlech. Druhý inkrementuje lokální hodiny na všech uzlech a realizuje připadnou chyby oscilátoru. Třetí krok na všech uzlech spustí funkci `process\_pipe()`, která realizuje zpoždění při odesílání zpráv, resp. zkontroluje jestli nějaká ze zpráv nedosáhla požadované doby odesílání. Pokud ano, odešle ji do fronty na cílový uzel. Čtvrtý krok tvoří stavový automat, který se stará o odbavení příchozích zpráv na každém z uzlů. Tento proces funguje podle algoritmu pro synchronizaci času popsaného v předchozí kapitole. Pátý krok odesílá inicializační zprávy pro konkrétní procesy, tedy synchronizaci doby přenosu a času. Synchronizace času se spouští jednou za 500 ms a synchronizace doby zpoždění jednou za 100 ms. V případě definovaní makra `BUILD\_REPORT` se v předposledním kroku vypíší hodnoty do log souboru, z kterého jsou následně generovaný grafy. Pro efektivitu velikosti logovacího souboru jsou vypisovány pouze cykly, které obsahují změnu. Pro každý uzel krom uzlu *MASTER* se vypisují se tři hodnoty, konkrétně průměrné zpoždění, velikost chyby a rozdíl času v uzlu *MASTER* a daného *SLAVE* uzlu. Na závěr každého běhy cyklu je inkrementovaný čas běhu simulace.


Odesílání zpráv se zpožděním je realizováno pomocí prioritní fronty nazývané *pipe* a fronty obsahující příchozí zprávy. Každý uzel má své tyto dvě struktury. Celý proces je ilustrován v schématu. Probíhá tak, že dojde k odeslání zprávy pomocí funkce `send_message()`. Tato funkce alokuje paměť pro novou zprávu a vloží zprávu do prioritní fronty *pipe*. Prioritu určuje doba odesílání, konkrétně čím nižší hodnota, tím dříve bude zpráva zpracována. Pokud už uplynula doba odesílání odesílání, zpráva je vyjmuta z *pipe* a vložena do *queue* na cílovém uzlu, na které jsou uchovávány příchozí zprávy.

Výsledky simulace je možné vizualizovat pomocí třech python skriptů, které se nescházejí v repozitáři simulace. 

První `visualizeO.py` zobrazuje velikost chyby $\bar{O}$ v závislosti na čase pro 3 *SLAVE* uzly, umožňuje zobrazit maximální a minimální akceptovatelnou chybu a střední hodnotu všech vypsaných hodnot.

Druhý skript `visualizeRTT.py` zobrazuje průměrnou hodnotu doby přenosu $\bar{D}$ v závislosti na čase pro 3 *SLAVE* uzly a umožňuje zobrazit střední hodnotu všech vypsaných hodnot.

Třetí soubor se souborem `visualizeTIME.py` zobrazuje rozdíl uzlu *MASTER* a *SLAVE* v závislosti na čase simulace pro 3 *SLAVE* uzly a umožňuje zobrazit střední hodnotu vypsaných hodnot.
