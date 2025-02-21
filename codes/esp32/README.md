## Instalar en una máquina linux, probada en un Ubuntu 22.04 LTS

Se sigue el [tutorial](https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/get-started/linux-setup.html)

$ sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/get-started/index.html#step-2-get-esp-idf

////////

Crear un proyecto

- hacer carpeta
- entrar carpeta
- $ idf.py set-target esp32

Mostrar menu esp32 para ver que funcione
- $ idf.py menuconfig

Compilar
- $ idf.py build

Cargar programa
- $ idf.py -p /dev/ttyUSB0 -b 115200 flash

- $ sudo usermod -a -G dialout $USER

mostrar console con screen (apt install screen)
- $ screen /dev/ttyUSB0 115200


## Ejemplo control motor con driver L298N

- [Motor L298N](https://esp32tutorials.com/esp32-dc-motor-l289n-esp-idf/)

## Ejemplo push button

- [Push button](https://esp32tutorials.com/esp32-push-button-esp-idf-digital-input/)

## Otros
- [Varios](https://github.com/SIMS-IOT-Devices)