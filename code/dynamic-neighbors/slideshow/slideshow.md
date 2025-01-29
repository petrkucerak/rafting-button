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
## DSVA

---
<!--
backgroundColor: "#FFF"
header: ""
 -->

# Požadavky<br>na systém

- řešení určení pořadí
- autonomnost zařízení
- distribuovaný systém
- bezdrátová komunikace
- maximální chyba 1 ms

![bg right:40%](push.png)

<!-- footer: "**POŽADAVKY** | SÍŤOVÁ INFRA | ALGORITMUS | REALIZCE" -->

---

# Protokol ESP-NOW

- 2. vrstva ISO/OSI modelu
- *callback* funkce, broad/unicast 
- limity protokolu

![w:850](callbacks.png)

<!-- footer: "POŽADAVKY | **SÍŤOVÁ INFRA** | ALGORITMUS | REALIZCE" -->

---

<!-- footer: "POŽADAVKY | SÍŤOVÁ INFRA | **ALGORITMUS** | REALIZCE" -->

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

# Celkový alogritmus

1. Registrace zařízení do DS
2. Běžný chod (epochy)
   1. volby lídra
   2. běžný provoz (logy, čas)
3. Terminace zařízení z DS

---

<!-- footer: "POŽADAVKY | SÍŤOVÁ INFRA | ALGORITMUS | **REALIZCE**" -->

# Realizace

- FreeRTOS, ESP-IDF
- základem je stavový automat
- unifikovaná velikost zprávy
- Doxygen dokumentace
  

---


<!-- footer: "POŽADAVKY | SÍŤOVÁ INFRA | ALGORITMUS | **REALIZCE**" -->

# Dynamický počet sousedů

- ukládání dat
- struktura packetu - checksum typu `uint32_t`
- nový typ zprávy 

---

<!-- footer: "" -->

<!-- # Testované scénáře
- běžný chod
- kauzalita 2 blízkých událostí<br/>*(do 1 ms)*
- odpojení zařízení a opětovné zapojení do sítě -->


![bg](measure-set.jpg)

---





<!-- footer: "Petr Kučera | kucerp28@fel.cvut.cz | [github.com/petrkucerak/rafting-button](https://github.com/petrkucerak/rafting-button)"
backgroundColor: ""
-->

# Distribuovaný systém IoT zařízení řešící problém konsenzu
![w:220](qr.png)