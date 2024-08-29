// Leonardo Serrano Arias
// C17484
// Laboratorio 1. Introducción a microcontroladores y manejo de GPIOS
// En este laboratorio se realizará un simulador de bingo con el microcontrolador PIC12F683

#include <pic14/pic12f683.h>  // Archivo de cabecera para el PIC12F683

#define _XTAL_FREQ 4000000 // Frecuencia del oscilador

// Definición de los pines
#define SDI GP0
#define SCLK GP1
#define LOAD GP2
#define BUTTON GP3

unsigned char prevButtonState = 1; // Estado previo del botón
unsigned int lfsr = 0xACE1u; // Semilla inicial del LFSR
unsigned char firstPress = 0; // Bandera para detectar la primera presión del botón
unsigned char drawnNumbers[10] = {0}; // Array para registrar los números sacados
unsigned char drawCount = 0; // Contador de números sacados

// Función para generar un retardo en microsegundos
void delay_us(unsigned int us) {
    while (us--) {
        __asm nop __endasm; // Instrucción en ensamblador para no hacer nada (equivale a un ciclo)
    }
}

// Función para generar un retardo en milisegundos
void delay_ms(unsigned int ms) {
    while (ms--) {
        delay_us(1000); // Cada milisegundo son 1000 microsegundos
    }
}

// Función para enviar datos al display
void sendData(unsigned char data) {
    for (int i = 0; i < 8; i++) {
        SDI = (data >> (7 - i)) & 0x01;
        SCLK = 1;
        delay_us(10); // Pequeño retardo para la señal de reloj
        SCLK = 0;
        delay_us(10);
    }
}

// Función para cargar los datos en el display
void latchData() {
    LOAD = 1;
    delay_us(10);
    LOAD = 0;
}

// Función para mostrar números en el display
void showNumber(int number) {
    // Codificación de los números para el display
    const unsigned char numbers[] = {
        0xC0, // 0
        0xF9, // 1
        0xA4, // 2
        0xB0, // 3
        0x99, // 4
        0x92, // 5
        0x82, // 6
        0xF8, // 7
        0x80, // 8
        0x90  // 9
    };

    int tens = number / 10; // Obtener las decenas
    int units = number % 10; // Obtener las unidades

    sendData(numbers[tens]); // Mostrar dígito de las decenas
    latchData();
    sendData(numbers[units]); // Mostrar dígito de las unidades
    latchData();
}

// Función para apagar el display
void clearDisplay() {
    sendData(0xFF); // Todos los segmentos apagados
    latchData();
    sendData(0xFF); // Todos los segmentos apagados
    latchData();
}

// Función para generar un número pseudoaleatorio usando un LFSR con más taps
unsigned int generateRandomNumber() {
    unsigned lfsr_bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ^ (lfsr >> 7) ^ (lfsr >> 13)) & 1; // Añadidos más taps
    lfsr = (lfsr >> 1) | (lfsr_bit << 15); // Desplazamiento con la nueva entrada

    // Mezcla adicional utilizando un XOR con un contador de tiempo
    lfsr ^= TMR0;

    return lfsr % 100; // Retornar un valor entre 00 y 99
}

// Función para verificar si el número ya ha sido sacado
unsigned char isNumberDrawn(unsigned int number) {
    for (unsigned char i = 0; i < drawCount; i++) {
        if (drawnNumbers[i] == number) {
            return 1; // Número ya ha sido sacado
        }
    }
    return 0; // Número no ha sido sacado
}

// Función para hacer parpadear el número 99
void blink99() {
    for (unsigned char i = 0; i < 3; i++) {
        showNumber(99);
        delay_ms(100);
        clearDisplay();
        delay_ms(100);
    }
}

void main() {
    TRISIO = 0b00001000; // Configura GP3 como entrada (botón) y los demás como salida
    ANSEL = 0; // Desactiva los módulos analógicos
    CMCON0 = 0x07; // Desactiva los comparadores

    // Inicializar TMR0 para obtener un valor variable
    OPTION_REG = 0b11010111; // Configura TMR0: reloj interno, prescaler 1:256, asignado a TMR0

    // Apagar el display al inicio
    clearDisplay();

    while (1) {
        // Detección del cambio de estado del botón (de no presionado a presionado)
        if (prevButtonState == 1 && BUTTON == 0) { // Si antes no estaba presionado y ahora lo está
            if (!firstPress) {
                firstPress = 1; // Marcar que el botón ha sido presionado por primera vez
                delay_ms(30); // Pequeño retardo para evitar rebotes
            } else {
                if (drawCount < 10) {
                    unsigned int randomNumber;
                    do {
                        randomNumber = generateRandomNumber(); // Generar un nuevo número pseudoaleatorio
                    } while (isNumberDrawn(randomNumber)); // Repetir hasta obtener un número no repetido

                    drawnNumbers[drawCount++] = randomNumber; // Guardar el número sacado
                    showNumber(randomNumber); // Mostrar el número
                } else {
                    blink99(); // Parpadear 3 veces con el número 99
                    drawCount = 0; // Reiniciar el contador de números sacados
                    firstPress = 1; // Reiniciar para esperar la siguiente pulsación del botón
                }
            }
        }

        prevButtonState = BUTTON; // Actualizar el estado previo del botón

        // Agregar un pequeño retardo para evitar rebotes
        delay_ms(10);
    }
}
