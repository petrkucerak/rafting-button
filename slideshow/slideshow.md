---
marp: true
theme: gaia
_class: lead
style: |
  @font-face {
    font-family: Technika-bold;
    src: url(Technika-Bold.otf);
  }
  @font-face {
    font-family: Technika;
    src: url(Technika-Regular.otf);
  }
  section {
    background: white;
    font-family: "Technika";
    background-image: url("background.jpg");
    background-size: 112%;
    background-position: top left;
    padding-top: 50px;
    padding-bottom: 50px;
    padding-left: 50px;
    padding-right: 50px;
  }
  code {
   font-family: 'Roboto Mono', monospace;
  }
  a {
   color: gray;
  }
  h1 {
    font-family: "Technika-bold";
    font-weight: 900;
    color: black;
    text-transform: uppercase;
    font-size: 1.4rem;
    text-align:left;
  }
  h2 {
    font-size: 1.1rem;
    text-align: left;

  }
  ul {
    margin: 0;
  }
  header{
    top: 50px;
    left: 24px;
    height: 200px;
  }
  footer {
    font-size: .5rem;
  }
  footer strong {
    color: black;
  }
  section::after {
    content: attr(data-marpit-pagination) '/' attr(data-marpit-pagination-total);
  }


header: "![w:200](CVUT-logo.jpg)"
# footer: 
paginate: true
# backgroundColor: "#FFF"
size: 4:3

---

# Distribuovaný systém<br>IoT zařízení řešící<br>problém konsenzu
## Bakalářská práce

---
<!--
backgroundColor: "#FFF"
header: ""
 -->

# Postup
1. Rešerše
2. Požadavky
3. Síťová infrastruktura
4. Algoritmické řešení
5. Realizace a testování

![bg right:40%](UX.png)

---

# Požadavky<br>na systém

- řešení určení pořadí
- autonomnost zařízení
- distribuovaný systém
- bezdrátová komunikace
- maximální chyba 1 ms

![bg right:40%](push.png)

<!-- footer: "REŠERŠE | **POŽADAVKY** | SÍŤOVÁ INFRA | ALGORITMUS | REALIZCE A MĚŘENÍ" -->

---

# Síťová infrastruktura

- volba vhodného modulu

![w:850](moduly.png)

<!-- footer: "REŠERŠE | POŽADAVKY | **SÍŤOVÁ INFRA** | ALGORITMUS | REALIZCE A MĚŘENÍ" -->

---

# Protokol ESP-NOW

- 2. vrstva ISO/OSI modelu
- *callback* funkce, broad/unicast 
- limity protokolu

![w:850](callbacks.png)

---

<!--

# Protokol ESP-NOW

- měření
  - vzdálenost => chybovost
  - velikost => rychlost přenosu
-->

![bg 95%](infra-measurement.png)

---

<!-- footer: "REŠERŠE | POŽADAVKY | SÍŤOVÁ INFRA | **ALGORITMUS** | REALIZCE A MĚŘENÍ" -->

<!-- # Algoritmus

- požadavky
  - koncenzus - shoda na uspořádání
- distribuovaný systém (DS)
- kauzalita a čas, konsenzus

--- -->

# Rozbor problému

- určení pořadí událostí
  → kauzalita / **časová značka**
- dílčí problémy
  - synchronizace času DS
  - distribuce logů
  - distribuce seznamu zařízení DS

---

# Synchronizace času

- výpočet doby přenosu $D$
- $D = {RTT\over 2}$

![w:850](rtt.png)

<!-- ---

# Synchronizace času

## Odeslání času -->
- $T_M = T_S + \bar{D} + O$

<!-- ![w:850](time.png) -->


---

## Simulace

![bg 90%](time-simulation.png)

---

## Reálný hardware

![bg 90%](time-wrong.png)

---

# Synchronizace času

- na reálném zařízení negativní vliv velikých chyb a provozu FreeRTOS
<!-- - zlepšeno řešeno časovou značkou -->
- změna algoritmu:
  1. synchronizace je prováděna tradičně<br>dokud není chyba $O$ menší než $O_{lim}$
  2. jakmile je chyba $O$ minimalizovaná,<br>upravuj čas maximálně o konstantu $K$

<!-- konstanta K je chyba způsobená nepřeností osciálátoru 25 µs -->

---

## Reálný hardware

![bg 90%](time-measure.png)

<!-- chyba způsobena tím, že MASTER hodiny jsou rychlejší -->

<!-- ---

# Distribuce logů

![w:850](distribution.png)

- bez MASTER uzlu: $L= N(N-1)$
- s MASTER uzlem: $L=2(N-1)$ -->

---

# Celkový alogritmus

1. Registrace zařízení do DS
2. Běžný chod (epochy)
   1. volby lídra
   2. běžný provoz (logy, čas)
3. Terminace zařízení z DS

---

<!-- footer: "REŠERŠE | POŽADAVKY | SÍŤOVÁ INFRA | ALGORITMUS | **REALIZCE A MĚŘENÍ**" -->

# Realizace

- FreeRTOS, ESP-IDF
- základem je stavový automat
- unifikovaná velikost zprávy
- Doxygen dokumentace
  

---

<!-- footer: "" -->

<!-- # Testované scénáře
- běžný chod
- kauzalita 2 blízkých událostí<br/>*(do 1 ms)*
- odpojení zařízení a opětovné zapojení do sítě -->


![bg](measure-set.jpg)

---


# Shrnutí

- uspořádání dle časové značky
- splněna maximální chyba 1 ms
- ESP-NOW, ESP-IDF a FreeRTOS
- poškození modulů a časová náročnost při práci s hardwarem

---

# Budoucnost projektu

- webový server
- robustnější distribuce logů
- podsítě
- mechanický návrh tlačítka a celého zařízení

---

<!-- footer: "Petr Kučera | kucerp28@fel.cvut.cz | [github.com/petrkucerak/rafting-button](https://github.com/petrkucerak/rafting-button)"
backgroundColor: ""
-->

# Distribuovaný systém IoT zařízení řešící problém konsenzu
![w:220](qr.png)