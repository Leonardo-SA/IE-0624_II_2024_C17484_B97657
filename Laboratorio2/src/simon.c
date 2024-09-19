#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// Variables globales
int selected_color = 5;         // Color seleccionado por el usuario
int current_state;              // Estado actual de la FSM (Máquina de Estados Finitos)
int color_sequence[4] = {2, 1, 0, 1};  // Secuencia de colores simplificada
int sequence_position = 0;      // Índice actual en la secuencia
int milliseconds = 0;           // Cuenta milisegundos

// Definición de estados para la FSM
#define WAIT_TIME 0
#define GAME_SEQUENCE 1
#define USER_SEQUENCE 2
#define END 3

// Funciones para controlar los LEDs
void TurnOnGreen(void) {
    PORTB = (1 << PB1);  // Enciende luz verde
}

void TurnOnBlue(void) {
    PORTB = (1 << PB3);  // Enciende luz azul
}

void TurnOnRed(void) {
    PORTB = (1 << PB2);  // Enciende luz roja
}

void TurnOffAll(void) {
    PORTB = 0x00;  // Apaga todas las luces
}

// Configuración de los puertos de luces y temporizador
void SetupLights(void) {
    DDRB = 0x0F;  // Configuración del puerto B
    TCCR0B = 0x5; // Configuración del reloj del temporizador
}

// Inicialización del temporizador
void SetupTimer(void) {
    TCCR0A = 0x00;               // Modo de operación normal
    TCCR0B |= (1 << CS00) | (1 << CS02);  // Prescaler para el temporizador
    sei();                        // Habilita interrupciones globales
    TIMSK |= (1 << TOIE0);        // Activa la interrupción por desbordamiento del Timer0
}

// Función de la FSM (Máquina de Estados Finitos)
void FSMStatusChange(void) {
    switch (current_state) {
        case WAIT_TIME:
            if (selected_color < 5) {
                current_state = GAME_SEQUENCE;
                milliseconds = 0;
                sequence_position = 0;
                selected_color = 5;  // Desactiva el botón
            }
            break;

        case GAME_SEQUENCE:
            // Muestra la secuencia de colores
            if (color_sequence[sequence_position] == 0) {
                TurnOnGreen();
            } else if (color_sequence[sequence_position] == 1) {
                TurnOnRed();
            } else if (color_sequence[sequence_position] == 2) {
                TurnOnBlue();
            }

            if (milliseconds == 10) {
                if (sequence_position >= 3) {
                    current_state = USER_SEQUENCE;
                    milliseconds = 0;
                    TurnOffAll();  // Apaga todas las luces
                    sequence_position = 0;
                } else {
                    sequence_position++;
                    milliseconds = 0;
                    TurnOffAll();  // Apaga todas las luces
                }
            }
            break;

        case USER_SEQUENCE:
            // Espera la secuencia del usuario
            if (selected_color == 5) {
                current_state = USER_SEQUENCE;
            } else {
                if (selected_color == color_sequence[sequence_position]) {
                    selected_color = 5;

                    if (sequence_position == 3) {
                        current_state = END;
                    } else {
                        sequence_position++;
                    }
                } else {
                    current_state = END;
                }
                TurnOffAll();  // Apaga todas las luces
            }
            break;

        case END:
            current_state = WAIT_TIME;
            selected_color = 5;
            break;

        default:
            break;
    }
}

int main(void) {
    SetupLights();  // Configura las luces
    SetupTimer();   // Configura el temporizador
    current_state = WAIT_TIME;  // Estado inicial

    while (1) {
        FSMStatusChange();  // Actualiza el estado de la FSM
    }
}

// Interrupciones
ISR(TIMER0_OVF_vect) {  // Interrupción por desbordamiento del temporizador
    milliseconds++;
}

ISR(INT1_vect) {  // Interrupción - Amarillo
    selected_color = 3;
}

ISR(PCINT1_vect) {  // Interrupción - Rojo
    selected_color = 1;
}

ISR(PCINT2_vect) {  // Interrupción - Verde
    selected_color = 0;
}

ISR(INT0_vect) {  // Interrupción - Azul
    selected_color = 2;
}
