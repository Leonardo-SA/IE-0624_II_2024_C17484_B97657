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
    PORTB = (0 << PB0) | (1 << PB1) | (0 << PB2) | (0 << PB3);  // Enciende luz verde
}

void TurnOnBlue(void) {
    PORTB = (0 << PB0) | (0 << PB1) | (0 << PB2) | (1 << PB3);  // Enciende luz azul
}

void TurnOnRed(void) {
    PORTB = (0 << PB0) | (0 << PB1) | (1 << PB2) | (0 << PB3);  // Enciende luz roja
}

void TurnOnYellow(void) {
    PORTB = (1 << PB0) | (0 << PB1) | (0 << PB2) | (0 << PB3);  // Enciende luz amarilla
}

void TurnOffAll(void) {
    PORTB = (0 << PB0) | (0 << PB1) | (0 << PB2) | (0 << PB3);  // Apaga todas las luces
}

void BlinkAll(void) {
    PORTB ^= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);  // Parpadeo de todas las luces
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
    GIMSK |= 0xD8;              // Habilita interrupciones para los pines de los puertos B y C
    PCMSK2 |= 0b00000001;        // Interrupción para el pin PD0
    PCMSK1 |= 0b00000001;        // Interrupción para el pin PA0
    DDRB = 0x0F;                 // Configuración del puerto B
    TCCR0B = 0x5;                // Configuración del reloj del temporizador
    MCUCR |= (1 << ISC11);       // Configuración para flanco negativo en INT1
}

// Inicialización del temporizador
void SetupTimer(void) {
    TCCR0A = 0x00;               // Modo de operación normal
    TCCR0B = 0x00;
    TCCR0B |= (1 << CS00) | (1 << CS02);  // Prescaler para el temporizador
    sei();                        // Habilita interrupciones globales

    TCNT0 = 0;                    // Inicializa el Timer0
    TIMSK |= (1 << TOIE0);        // Activa la interrupción por desbordamiento del Timer0
}

// Función de la FSM (Máquina de Estados Finitos)
void FSMStatusChange(void) {
    switch (current_state) {
        case WAIT_TIME:
            // Espera a que se presione un botón (color seleccionado)
            if (selected_color < 5) {
                current_state = START;
                timer_overflow_count = 0;  // Reinicia contadores
                seconds = 0;
                milliseconds = 0;
            }
            break;

        case START:
            // Inicia la secuencia del juego
            if (milliseconds == 4) {
                current_state = GAME_SEQUENCE;
                timer_overflow_count = 0;
                seconds = 0;
                milliseconds = 0;
                sequence_position = 0;
                sequence_length = 2;
                selected_color = 5;  // Desactiva el botón
                GenerateSequence();  // Genera nueva secuencia
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

            // Verifica si ha pasado el tiempo para mostrar el siguiente color
            if (milliseconds == display_time) {
                if (sequence_position >= sequence_length - 1) {
                    current_state = USER_SEQUENCE;
                    timer_overflow_count = 0;
                    seconds = 0;
                    milliseconds = 0;
                    TurnOffAll();  // Apaga todas las luces
                    sequence_position = 0;
                } else {
                    sequence_position++;
                    current_state = GAME_SEQUENCE;
                    seconds = 0;
                    milliseconds = 0;
                    TurnOffAll();  // Apaga todas las luces
                }
            }
            break;

        case USER_SEQUENCE:
            // Espera la secuencia del usuario
            if (selected_color == 5) {
                current_state = USER_SEQUENCE;
                milliseconds = 0;
            } else {
                if (selected_color == 0) {
                    TurnOnGreen();
                } else if (selected_color == 1) {
                    TurnOnRed();
                } else if (selected_color == 2) {
                    TurnOnBlue();
                } else if (selected_color == 3) {
                    TurnOnYellow();
                }

                if (milliseconds == 2) {
                    milliseconds = 0;
                    TurnOffAll();  // Apaga todas las luces

                    if (selected_color == color_sequence[sequence_position]) {
                        selected_color = 5;

                        if (sequence_position == sequence_length - 1) {
                            current_state = LEVEL;
                            seconds = 0;
                            milliseconds = 0;
                        } else {
                            current_state = USER_SEQUENCE;
                            sequence_position++;
                        }
                    } else {
                        TurnOffAll();  // Apaga todas las luces en caso de error
                        current_state = END;
                    }
                }
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
            // Fin del juego: vuelve al estado de espera
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
    if (timer_overflow_count % 12 == 0) {  // Medio segundo
        if (current_state == START || current_state == END) {
            BlinkAll();  // Parpadeo de todas las luces
        }
        ++milliseconds;  // Incrementa milisegundos
    }

    if (timer_overflow_count == 60) {  // Un segundo completo
        timer_overflow_count = 0;
        ++seconds;  // Incrementa el contador de segundos
    } else {
        timer_overflow_count++;
    }

    cycle_counter++;  // Incrementa el contador de ciclos
    if (cycle_counter > 1000) {
        cycle_counter = 0;
    }
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
