#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// Variables globales
int timer_overflow_count = 0;   // Cuenta las sobrecargas del temporizador
int selected_color = 5;         // Color seleccionado por el usuario
int current_state;              // Estado actual de la FSM (Máquina de Estados Finitos)
int color_sequence[15] = {2, 1, 0, 1, 3, 2, 0, 2, 1, 0, 3, 2, 1, 0, 1};  // Secuencia de colores
int sequence_length = 4;        // Longitud de la secuencia de colores
int sequence_position = 0;      // Índice actual en la secuencia
int index;
int display_time = 10;          // Tiempo de visualización de cada color
int cycle_counter = 0;          // Contador de ciclos
int temp_variable;              // Variable temporal para intercambio
int seconds = 0;                // Cuenta segundos
int milliseconds = 0;           // Cuenta milisegundos

// Definición de estados para la FSM
#define START 0 
#define GAME_SEQUENCE 1 
#define USER_SEQUENCE 2 
#define LEVEL 3 
#define END 4 
#define WAIT_TIME 5

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

void TurnOnYellow(void) {
    PORTB = (1 << PB0);  // Enciende luz amarilla
}

void TurnOffAll(void) {
    PORTB = 0x00;  // Apaga todas las luces
}

// Función para generar una nueva secuencia de colores
void GenerateSequence(void) {
    for (index = 0; index < sequence_length; index++) {
        temp_variable = color_sequence[index];
        color_sequence[index] = color_sequence[cycle_counter % 13];
        color_sequence[cycle_counter % 13] = temp_variable;
    }
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
                current_state = START;
                milliseconds = 0;
                sequence_position = 0;
                selected_color = 5;  // Desactiva el botón
            }
            break;

        case START:
            if (milliseconds == 5) {
                current_state = GAME_SEQUENCE;
                milliseconds = 0;
                sequence_position = 0;
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
            } else if (color_sequence[sequence_position] == 3) {
                TurnOnYellow();
            }

            if (milliseconds == display_time) {
                if (sequence_position >= sequence_length - 1) {
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
            if (selected_color == 5) {
                current_state = USER_SEQUENCE;
            } else {
                if (selected_color == color_sequence[sequence_position]) {
                    selected_color = 5;

                    if (sequence_position == sequence_length - 1) {
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

        case LEVEL:
            // Subir de nivel: incrementa la secuencia
            current_state = GAME_SEQUENCE;
            sequence_position = 0;
            sequence_length++;
            if (display_time > 1) {
                display_time--;  // Reduce el tiempo de visualización
            }
            selected_color = 5;
            GenerateSequence();  // Genera nueva secuencia
            break;

        case END:
            // Fin del juego
            if (milliseconds == 6) {
                current_state = WAIT_TIME;
                selected_color = 5;
            }
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
