# Sistema de Parqueo Inteligente

## Lorena Solís Extteny | Leonardo Serrano Arias

---

Este proyecto implementa un sistema automatizado de parqueo inteligente utilizando un microcontrolador ESP32, sensores infrarrojos, servomotores, una pantalla LCD y la plataforma Blynk. El sistema permite monitorear, gestionar y reservar espacios de parqueo en tiempo real mediante una aplicación móvil, mejorando la eficiencia y comodidad para los usuarios.

---

## Hardware Utilizado

- **ESP32**: Microcontrolador principal para la gestión del sistema.
- **Sensores infrarrojos (5x)**: Detectan la ocupación de los espacios de parqueo.
- **Servomotores (2x)**: Controlan las barreras de entrada y salida.
- **Pantalla LCD (I2C, 16x2)**: Muestra localmente la disponibilidad de espacios.
- **Wi-Fi**: Conexión a la plataforma Blynk para monitoreo remoto.

---

## Software Utilizado

- **Arduino IDE**: Desarrollo del firmware del ESP32.
- **Blynk**: Plataforma de IoT para monitoreo y control remoto.
- **Librerías**:
  - `BlynkSimpleEsp32.h`
  - `Wire.h`
  - `LiquidCrystal_I2C.h`
  - `ESP32Servo.h`

---

## Instalación y Configuración

1. **Clonar el repositorio**:
~~~
   ```bash
   git clone https://github.com/Leonardo-SA/IE 0624_II_2024_C17484_B97657/tree/9b995f69561d0bf21dc99eecd3e3977756ab1c37/Proyecto
   cd src
~~~
2. **Configurar el ESP32**:
~~~
char ssid[] = "Nombre de la red";
char pass[] = "Contraseña";
~~~
3. **Configurar el token de autenticación de Blynk**:
~~~
#define BLYNK_AUTH_TOKEN "TuTokenDeAutenticación"
~~~



