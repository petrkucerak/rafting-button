### Rafting button with dynamic neighbors count

Cílem této implementace je změnit statický počet sousedů na počet dynamický a umožnit zapojit tak do sítě až 20 zařízení, což je limit daný technologií ESP-NOW (konkrétně funkcí `esp_now_add_peer()`).

> [!NOTE]
> Při určité konfiguraci by určitě bylo možné vymyslet alternativní řešení pro využití více sousedů, např. vytvořit více subsíti. Pro tuto aplikaci to ovšem nedává smysl, proto jsem se rozhodl respektovat tento limit a zohlednit ho při následném výpočtu paměťové náročnosti.

## Návrh realizace

1. **ukládání**: V podstatě jsou 2 možnosti pro ukládání dat do paměti - statická nebo dynamická cesta. Statická by znamenalo mít neustále alokováno $20$ x $16$ bytes, kde by se do pole zapisovala aktuální zařízení. Druhým způsobem je paměť dynamická - např. linked list. Limitací technologie ESP-NOW jsem zvolil první možnost, tedy statický styl ukládání a to z důvodu bezpečnější implemtnace (resp. méně náchylné na chybu) a z důvodu menší výpočetní zátěže pro práci s paměti. 
2. **změna struktury packetu**: Součástí každé zprávy momenátlně byl i seznam sousedů. To nyní změním a to z důvodu omezení paměti. Místo části framu, kde doteď byli uložení sousedé (neighbor), uložím jednoduchý checksum, který bude typu `uint32_t` a bude preprezentovat sousedy následujícím způsobem. Za každý node ve stavu
   - `NOT_INITIALIZED` bude přičtena hodnota $1$
   - `INACTIVE` bude přičtena hodnota $100$
   - `ACTIVE` bude přičtena hodnota $10 000$
3. **nový typ zprávy**: Vytvořím nový typ zprávy, který bude odesílát seznam sousedů. Jelikož je velikost zprávy maximálně 250 bytes, budu muset seznam rozdělit do dvou zpráv. To ale ničemu nevadí. Tato zpráva se bude odesílat vždy, po:
   - změně datové struktury se sousedy
   - zachycení zprávy `HELLO_DS`
   - přijmutí chybného čísla počtu sousedů
   - naktivním zařízení
4. **aktualizace sezanamu sousedů**: Je třeba implementovat, resp. aktualizovat funkci pro příjem počtu sousedů. Vždy vyhrává zpráva, která má nojvější číslo epochy.
