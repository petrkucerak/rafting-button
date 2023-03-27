# Hello world project

Cílem tohoto prvního projektu je se pokusit zkompilovat základní kód na desku ESP-32 pomocí a seznámit se s ESP-IDF.

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

## Notes

- used version: `ESP-IDF v5.1-dev-4167-gab63aaa4a2`
- 