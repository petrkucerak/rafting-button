### Rafting button with dynamic neighbors count

Cílem této implementace je změnit statický počet sousedů na počet dynamický a umožnit zapojit tak do sítě až 20 zařízení, což je limit daný technologií ESP-NOW (konkrétně funkcí `esp_now_add_peer()`).

> [!NOTE]
> Při určité konfiguraci by určitě bylo možné vymyslet alternativní řešení pro využití více sousedů, např. vytvořit více subsíti. Pro tuto aplikaci to ovšem nedává smysl, proto jsem se rozhodl respektovat tento limit a ti u při následném výpočtu paměťové náročnosti.