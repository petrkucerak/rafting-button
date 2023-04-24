# Scenarios

Předpoklady pro simulaci jsou takové, že:

- zpoždění odesílání signálů je rozdílné a náhodné v rozmezí od 1 ms do 6 ms (tyto hodnoty byly zjištěny předchozím experimentem)
- zpomalení osciálátoru procesoru zanedbáváme jelikož se jedná o ±10 ppm [[1]](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html)

## B1

Simulace v tomto případě osciluje okolo nulového rozdílu v rozmezí až do 2.5 ms zpoždění. Vhodné v dalších simulacích otestovat přesnost střední hodnoty.

- **náhodně zpoždění na všech uzlech v rozmezí od 1 - 6 ms**
- synchronizační zpráva odesílaná každých 50 ms
- doba simulace je 6 s

![simulace B1](B1.png)

## B2

Pokud je zpoždění konstatní, alogritmus funguje perfektně. V praxi tato situace neexistuje.

- **konstatní zpoždění na všech uzlech stejné**
- synchronizační zpráva odesílaná každých 50 ms
- doba simulace je 6 s

![simulace B2](B2.png)

## B3

Zpoždění jsou konstatní ale na každém uzlu rozdílné. V tomto případě alogritmus nefunguje správně, jelikož se liší doba odesílá v závilsoti na směrech.

- **konstatní zpoždění na rozdílné na uzlech**
  - A (MATER) 1 ms
  - B (SALVE) 1.2 ms
  - C (SLAVE) 0.8 ms
- synchronizační zpráva odesílaná každých 50 ms
- doba simulace je 6 s

![simulace B3](B3.png)

## C1

Simulace navíc počítá střední hodnotu z naměřených hodnot. Výsledná synchronizovaná hodnota je přesnější. Rozdíl pro s B uzlem je 0.048 ms a s C uzlm 0.390 při ěření po dobu 6 s.

- **náhodně zpoždění na všech uzlech v rozmezí od 1 - 6 ms**
- synchronizační zpráva odesílaná každých 50 ms
- doba simulace je 6 s

![simulace C1](C1.png)

## C2

Scénář C2 oproti předchozímu prodlužuje dobu simulace na 2 minuty. Střední hodnota se ustálila na hodnotách 0.141 ms pro C a 0.188 ms pro B.

- **náhodně zpoždění na všech uzlech v rozmezí od 1 - 6 ms**
- synchronizační zpráva odesílaná každých 50 ms
- doba simulace je 120 s

![simulace C2](C2.png)

Simulaci se stejnými parametry jsem nechal běžet po dobu jedné hodiny. Střední hodnota vyšla:

- 0.2141 pro B
- 0.1881 pro C

## D1

Do scénářů jsem implementoval mechanismu balanceru, který ukládá historii času a je tak schopen odfiltrovat extrémy. Měření tak ve střední hodnotě dosahuje větší přesnosti (B 0.112 ms a C 0.459 ms).

V případě větší velikosti balanceru, dochází k nesrovnalostem a algoritmus již nefunguje efektivně.

- **náhodně zpoždění na všech uzlech v rozmezí od 1 - 6 ms**
- synchronizační zpráva odesílaná každých 50 ms
- doba simulace je 60 s
- velikost balanceru 5

![simulace D1](D1.png)

Pokud simulaci nechám běžet delší čas, dosahuje o trochu nižší kvality výsledků ovšem opět se vejdou do limitu 1 ms, konkrétně:
- pro B je hodnota 0.5595 ms
- pro C je hodnota 0.1551 ms