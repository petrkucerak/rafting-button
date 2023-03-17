# Bakalářská práce

Samotná bakalářská práce.

## Jak sprovoznit lokální kompilaci

1. nainstaluj texlive včetně luatex (nutná por instalaci OpTeX)
2. stani si submodul s OpTeX
3. naistaluj OpTeX
   ```
   cd OpTeX/optex/base
   luatex -ini optex.ini
   ```
4. pro live vývoj spusť skritp `texloop.sh`
   ```
   ./texloop.sh optex ctustyle-doc
   ```

## Požadavky na závěrečnou práci

- [ ] minimálně 20 stran bez přílohy ve formátu A4 o velikosti textu 11pt
- [ ] svazek musí mít lepenou (ne kroužkovou) vazbu
- [ ] musí obsahovat
  - [ ] obsah včetně případných příloh
  - [ ] zadávací formulář
  - [ ] anotaci v češtine a angličtině
  - [ ] prohlášení o samostatnoti práce dle https://intranet.fel.cvut.cz/cz/rozvoj/MP_2009_01.pdf
  - [ ] korektní citace dle https://intranet.fel.cvut.cz/cz/rozvoj/MP_2009_01.pdf
  - [ ] úvodní rozbor dané problematiky
  - [ ] popis řešení zadaného úkolu
  - [ ] závěrečné hodnocení výsledků
  - [ ] seznam použité litartury

*viz https://intranet.fel.cvut.cz/cz/rozvoj/smerniceSZZ.pdf*

