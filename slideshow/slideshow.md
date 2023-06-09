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
    padding-top: 160px;
    padding-bottom: 120px;
    padding-left: 125px;
    padding-right: 125px;
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
  li{
    font-size: rem;
  }
  ul {
    margin: 0;
  }
  header{
    top: 10px;
    height: 200px;
  }
  footer {
    font-size: .5rem;
  }
  footer strong {
    color: black;
  }


header: "![w:200](CVUT-logo.jpg)"
# footer: 
paginate: true
# backgroundColor: "#FFF"
size: 4:3

---

# Distribuovaný systém IoT zařízení řešící problém konsenzu
## Bakalářská práce

---
<!-- backgroundColor: "#FFF" -->

# Postup
1. Rešerše
2. Požadavky
3. Síťová infrastruktura
4. Algoritmické řešení
5. Realizace a testování
---

# Rešerše

- trendy
- úspora energie
- hirearchická struktura

<!-- footer: "**REŠERŠE** | POŽADAVKY | SÍŤOVÁ INFRA | ALGORITMUS | REALIZCE A MĚŘENÍ" -->

---

# Požadavky

- určení pořadí
- variabilita prostředí
- autonomní a distribuovaný systém
- přesnost 1 ms
- bezdrátová komunikace

<!-- footer: "REŠERŠE | **POŽADAVKY** | SÍŤOVÁ INFRA | ALGORITMUS | REALIZCE A MĚŘENÍ" -->

---

# Síťová infrastruktura

- volba vhodného modulu

![w:690](moduly.png)

<!-- footer: "REŠERŠE | POŽADAVKY | **SÍŤOVÁ INFRA** | ALGORITMUS | REALIZCE A MĚŘENÍ" -->

---

# Protokol ESP-NOW

- 2. vrstva ISO/OSI modelu
- *callback* funkce, broad/unicast 

![w:690](callbacks.png)

---

# Protokol ESP-NOW

- limity protokolu
  - 250 bajtů
  - 10 zařízení
  - ESP-IDF / Arduino Framework
  - nekvalitní dokumentace

---

# Protokol ESP-NOW

- modifikovatelnost

![w:700](infra.png)

---

# Protokol ESP-NOW

- měření
  - vzdálenost => chybovost
  - velikost => rychlost přenosu

---

![w:700](infra-measurement.png)

---

<!-- footer: "REŠERŠE | POŽADAVKY | SÍŤOVÁ INFRA | **ALGORITMUS** | REALIZCE A MĚŘENÍ" -->

# Algoritmus

- požadavky
  - koncenzus - shoda na uspořádání
- distribuovaný systém (DS)
- kauzalita a čas, konsenzus

---

# Rozbor problému

- kauzalita / **časová značka**
- dílčí problémy
  - synchronizace času DS
  - distribuce logů
  - distribuce senzamu zařízení DS

---

# Synchronizace času

- ideální situace

$$
T_A = T_B + D_n + O_n,\:\:\: D_n = {RTT\over 2}
$$

- realita (předpoklad symetrie)

$$
T_A = T_B + \bar{D} + \bar{O}
$$

---

![w:700](time-simulation.png)

---

# Synchronizace času

- reálný hardware
  - časová značka
  - změna výpočtu

$$
T = T \pm K
$$

---

![w:700](time-measure.png)

---

# Distribuce logů

$$
L= N(N-1) \:\: | \:\: L=2(N-1)
$$

![w:726](distribution.png)