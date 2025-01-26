Softwarová implementace.

## Seznam projektů

- `dynamic-neighbors` - implementac v rámci předmětu DSV, která umožňuje dynamický počet sousedů
- `esp-now-parameters` - měření latence ESP-NOW na ESP32
- `hello-world` - testovací kód na ESP32S2
- `measure-esp-now` - měření síťové infratruktury na ESP32S2
- `rafting-button` - výsledný kód
- `simulation` - simulace synchronizace času
- `sync-time` - měření synchronizace času

## Snippets

```sh
sudo usermod -a -G dialout, tty $USER # eneable read and write access to the serial console, after set need computer restart

idf.py --help # help manual
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -p <PORT> flash
idf.py monitor # display serial console

ls /dev/tty* # to find the port on linux device
```

## Coponents manager

https://components.espressif.com/

```sh
idf.py add-dependency "espressif/esp-now^2.1.1" # install the current version of esp-now
idf.py reconfigure # install defined dependencies
```