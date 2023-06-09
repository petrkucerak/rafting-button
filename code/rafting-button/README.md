### Rafting button

**Dokumentace se věnuje pouze finální implementaci distribuovaného systému, která je v repositáři dostupná pod složkou [code/rafting-button](https://github.com/petrkucerak/rafting-button/tree/main/code/rafting-button).**

## O projektu
### Motivace
Už od útlých dětských let jezdím na letní tábory a přes rok organizuji mládežnické akce. Na *Hudebním týdnu*, jedné z akcí pro mladé, tradičně hráváme hru, ve které jsou různé týmy, které mezi sebou soutěží. Cílem je se jako první přihlásit o slovo a zodpovědět otázku. Doposud se nám nepovedlo vymyslet efektivní způsob, jak určit pořadí týmů. Ve výsledku vždy použijeme zvoneček, který při odpovědí více týmů ve stejný čas není vhodným řešením.

### Popis cíle bakalářské práce
Cílem bakalářské práce je vytvořit koncept hlasovacího tlačítka, které bude fungovat
jako autonomní zařízení v distribuovaném systému. Systém pomocí vhodných algoritmů
bude řešit problematiku konsenzu, respektive kauzálního uspořádaání stisku tlačítek.

### Popis hlasovacího zařízení

Z uživatelského pohledu by zařízení mělo být kompaktní krabička, kterou si bude moci skupina položit před sebe na stůl.2 Krabička by na sobě měla mít tlačítko, které bude odolné i vůči veliké síle způsobené bouchnutí v zápalu hry. Zařízení bude možné přepínat mezi dvěma módy, tj. *PRESENTER* a *NORMAL*. V módu *NORMAL* bude zařízení řešit,jakém pořadí se týmy přihlásily o slovo. V módu *PRESENTER* navíc pomocí webového serveru vykreslí výsledky.

Samotná hra pak bude probíhat tak, že bude položena otázka, hráči se přihlásí o slovo stiskem tlačítka. Informace se rozdistribuujesíti a zařízenímódu *PRESENTER* zobrazí výsledky. Hlasování se vyresetuje dlouhým stisknutím hlasovacího tlačítka na jakémkoliv zařízení.

## Implementace

Kód jsem implementoval v jazyce C s použitím *ESP-IDF*. Jedná se o oficiální vývojový framework pro procesory ze sérií ESP32, ESP32-S, ESP32-C a ESP32-H. Framework se skládá z jednotlivých komponent, jako je například RTOS kernel, který využívá *FreeRTOS*, ovladače periferií, wifi či komponenta pro optimalizaci spotřeby energie. Většina komponent je vyvíjena jako *open-source*. ESP-IDF také podporuje balíčkový systém,(balíčkový systém se nazývá *IDF Component Registry*) který umožňuje pokročilé verzování a efektivní zacházení s jednotlivými komponenty.

Jedním z požadavků na zařízení bylo, aby se jednalo o *autonomní systém*. Výsledný software je tedy na všech zařízeních z počátku ve stejné konfiguraci.

Z důvodu více jader procesoru a využití *callback* funkcí v implementaci využívám **multitaskové infrastruktury**, kterou zajišťuje *FreeRTOS*. Běh systému je složen z následujících procesů.

* Rodičovský proces `app_main` inicializuje nutné komponenty a stará se o správu celého systému. Také se využívá pro vypisování pravidelných hlášení o stavu systému.
* Proces `espnow_handler_task` slouží k zpracování *ESP-NOW udláostí*. To jsou události vyvolané *callbacky*, které signalizují odeslání nebo přijmutí zprávy. Proces funguje jako stavový automat a je detailněji popsán v další kapitole.
* Pokud je zařízení *lídrem*, pak proces `send_rtt_cal_master_task` rozesílá zprávu inicializující výpočet doby přenosu $D$. Pokud lídrem není, nic neprovádí.
* Proces `send_time_task` funguje podobně jako předchozí proces. Tedy pokud je zařízení *lídrem*, pak rozesílá zprávy synchronizující čas. Tím si *lídr* udržuje svoji autoritu.
* Pokud zařízení po dobu $t_{sync}$ neobdrželo zprávu s časem od *lídra*, proces  `send_request_vote_task` inicializuje nové volby a následně je obslouží.
* Posledním uživatelsky zavedeným procesem je `handle_ds_event_task`. Ten se stará o distribuci logů mezi zařízeními.
* V systému probíhají i jiné systémové obslužné rutiny, například zajišťující chod wifi.

Software implementuje 3 **přerušení**, respektive dvě obsluhy přerušení a jedno přerušení. Konkrétně se jedná o *callback* funkce `espnow_send_cb` a `espnow_recv_cb` obsluhující přerušení ESP-NOW a přerušení `gpio_handler_isr` pro obsluhu hardwarového tlačítka připojeného na bránu `GPIO_NUM_23`.

**Priorita** jednotlivých procesů je popsána tabulkou *(viz níže)*. Nejvyšší prioritu mají přerušení a rutiny obsluhující přerušení. Po nich následuje `espnow_handler_task`, protože se stará o časově kritické procesy jako je synchronizace času. Střední prioritu mají všechny ostatní procesy krom rodičovského, který má prioritu nejnižší.

| proces                     | priorita |
| -------------------------- | -------- |
| `app_main`                 | 1        |
| `espnow_handler_task"`     | 3        |
| `send_rtt_cal_master_task` | 2        |
| `send_time_task`           | 2        |
| `send_request_vote_task`   | 2        |
| `handle_ds_event_task`     | 2        |


Z měření síťové infrastruktury vyplývá, že **velikost zprávy má vliv na dobu odeslání**. Algoritmus v ideálním případě očekává symetrickou dobu odesílání zpráv. Proto jsem při implementaci využil maximální velikost rámce a nevyužité místo jsem zaplnil náhodnými daty. Díky tomu nebude mít rozdílná velikost vliv na případnou asymetričnost.

Z důvodu zjednodušení využívám pouze **jediný typ rámce** pro všechny typy zpráv. Rámce se skládají z následujících částí, které jsou vizualizovány obrázky přiloženými obrázky.

* **Typ zprávy** (`message_type_t`) ovlivňuje stavový automat v procesu, který zpracovává příchozí zprávy.
* **ID epochy** (`uint32_t`) říká o jakou epochu se jedná, a tak zajišťuje bezpečnost.
* **Číselný obsah** (`uint64_t`) používá se k přenášení různých typů zpráv. Ve většině případů obsahuje časovou značku.
* **Typ události DS** (`ds_event_t`) specifikuje, zdali se jednalo o stisk nebo reset. Využívá se k distribuování logu.
* **Mac adresa události** (`uint8_t[ESP_NOW_ETH_ALEN]`) se využívá k distribuování logu, konkrétně ke zdrojové adrese dané události.
* **Úkol události** (`ds_task_t`) se využívá k distribuci logů.
* **Sousedé** (`neighbour_t[NEIGHBOURS_COUNT]`) je pole obsahující seznam sousedů. Požadavek na systém definuje maximálně 10 připojených zařízení. Do pole se neukládá informace o zařízení, kterému seznam náleží. Proto je velikost pole 9.
    * **Titul** (`device_title_t`) určuje, zdali je zařízení *lídr* nebo *následovník*.
    * **Status** (`device_status_t`) je informace o stavu zařízení, tj. zdali je aktivní či nikoli.
    * **Mac adresa** (`uint8_t[ESP_NOW_ETH_ALEN]`) identifikuje souseda pomocí jeho mac adresy.
* **Výplň** (`uint8_t`) neboli payload vyplňuj nevyužitý prostor náhodnými daty.

### Proces zpracování zpráv

Zprávy zpracovává speciální mechanismus. To tak, že *callback* funkce zaznamená novou zprávu, pošle ji jako událost do fronty a z ní proces `espnow_handler_task` vyjme událost a tu následně odbaví.

Tento **proces je klíčovým** pro fungování celého systému. Funguje jako stavový automat. Přijatou událost, respektive zprávu zpracuje podle jejího typu. Pokud se jedná o zprávy typu:

* `HELLO_DS` zařízení ověří, zdali se jedná o neznámé zařízení. Pokud ano, přidá ho do svého seznamu. Pokud již zařízení zná, pouze nastaví jeho status jako aktivní. Jako odpověď mu pošle seznam všech zařízení v sítí.
* `NEIGHBOURS` zařízení si přidá všechny neznámé sousedy do svého listu a aktualizuje status a titul u jednotlivých zařízení.
* `RTT_CAL_MASTER` zařízení odešle zprávy zpět odesílateli se stejným obsahem. Tato zpráva se využívá k výpočtu doby přenosu $D$.
* `RTT_CAL_SLAVE` zařízení vypočítá dobu přenosu a odešle ji odesílateli zprávy.
* `RTT` zařízení uloží hodnotu přenosu do pole, aby bylo možné vypočítat průměrnou dobu přenosu. Pokud se jedná o první takovou informaci, pole se touto hodnotou vyplní celé.
* `TIME` zařízení spočítá chybu synchronizace času $O$ a nastaví proměnou konstantu $c$ pro výpočet času $T_{DS}$ podle algoritmu popsaného v bakalářské práci.

Zpráva tohoto typu se využívá i k tomu, aby si *lídr* udržel svoji autoritu. Pokud je tedy zařízení ve stavu *kandidáta* a přijme daný typ zprávy, ukončí svoji kandidaturu a přepne se do stavu *následovník*. 

Při každém zpracování této zprávy se ukládá časová značka. Ta se následně využívá k výpočtu *timeoutu* $t_{sync}$. Pokud je delší než stanovená konstanta, zařízení přechází do stavu *kandidát*, zvyšuje číslo epochy (*epoch ID*) a inicializuje volby.

* `REQUEST_VOTE` zařízení odešle odesílateli zprávu s hlasem.
* `GIVE_VOTE` zařízení zprávu uloží. Pokud má více odpovědí, než je polovina aktivních zařízení v síti, prohlásí se za *lídra* a ostatní zařízení za *následovníky*.
* `LOG2MASTER` zařízení pošle log do fronty, která předává tento typ událostí procesu `handle_ds_event_task`. Log je uložen a rozeslán všem *následovníkům*.
* `LOG2SLAVES` zařízení pošle log do fronty, která předává tento typ událostí procesu `handle_ds_event_task`. Log je uložen.

V případě, že proces obdrží události s informací o **neúspěchu odeslání zprávy**, se inkrementuje součet neodeslaných zpráv v daném zařízení. Pokud jich je více než počet povolený konstantou `COUNT_ERROR_MESSAGE_TO_INACTIVE`, proces je prohlášen za neaktivní a tato informace je rozdistribuována na všechny aktivní sousedy.

### Poznámky k implementaci

Kód je doplněn o ladící výpisy definované v ESP-IDF. Konkrétně se jedná o aplikaci funkcí `ESP_LOGI`, `ESP_LOGW` a `ESP_LOGE`.

Veškerý kód je dostupný v [repozitáři projektu](https://github.com/petrkucerak/rafting-button/).

Dokumentace kódu je dostupná na adrese https://petrkucerak.github.io/rafting-button/.

## Příkazy
### Práce s ESP-IDF
```
idf.py build
idf.py -p <PORT> flash
idf.py monitor
```
### Export dokumentace
```
cd doc
doxygen Doxyfile
```

## Seznam zarizeni
  | NO  | type        | MAC address         | Note |
  | --- | ----------- | ------------------- | ---- |
  | 1   | ESP32 small | `70:b8:f6:5b:d3:24` | COM4 |
  |     | STM32       |                     | COM5 |
  | 2   | ESP32 small | `cc:db:a7:1d:c4:08` | COM6 |
  | 3   | ESP32 large | `c8:f0:9e:7b:10:8c` | COM7 |
  | 4   | ESP32 small | `cc:db:a7:1d:c7:cc` | COM8 |
  | 5   | ESP32 large | `94:b5:55:f9:f2:f0` | COM9 |