Para ejecutar este laboratorio es necesario tener los archivos de código fuente en el directorio stm32f429i-discovery y seguir los siguientes pasos a través de comandos:

> make
> arm-none-eabi-objcopy -O binary main.elf firmware.bin
> st-flash --reset write firmware.bin 0x8000000


Para correr el script de Python que lee/escribe al puerto serial/USB y que envíe la informacion a un dashboard de Thingsboard se debe presionar el botón correspondiente de la placa y luego ejecutar el siguiente comando:

> python3 iot.py
