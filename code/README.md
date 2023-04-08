# Code

Softwarová implementace.

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