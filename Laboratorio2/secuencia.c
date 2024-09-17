#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
  // Configurar los pines de los LEDs RGB como salida
  DDRD |= (1 << PD2);  // Rojo LED1
  DDRA |= (1 << PA0) | (1 << PA1);  // Verde y Azul LED1

  DDRD |= (1 << PD5) | (1 << PD4) | (1 << PD3);  // LED2 (Rojo, Verde, Azul)

  DDRB |= (1 << PB3) | (1 << PB4) | (1 << PB5);  // LED3 (Rojo, Verde, Azul)

  DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);  // LED4 (Rojo, Verde, Azul)

  // Configurar los pines de los botones como entrada
  DDRD &= ~(1 << PD1);  // Botón 1
  DDRD &= ~(1 << PD0);  // Botón 2
  DDRB &= ~(1 << PB6);  // Botón 3
  DDRB &= ~(1 << PB7);  // Botón 4

  // Activar resistencias pull-up internas para los botones
  PORTD |= (1 << PD1);
  PORTD |= (1 << PD0);
  PORTB |= (1 << PB6);
  PORTB |= (1 << PB7);

  // Asegurarse de que todos los LEDs comiencen apagados
  PORTD &= ~(1 << PD2);  // Apagar Rojo LED1
  PORTA &= ~(1 << PA0) & ~(1 << PA1);  // Apagar Verde y Azul LED1
  PORTD &= ~(1 << PD5) & ~(1 << PD4) & ~(1 << PD3);  // Apagar LED2
  PORTB &= ~(1 << PB3) & ~(1 << PB4) & ~(1 << PB5);  // Apagar LED3
  PORTB &= ~(1 << PB0) & ~(1 << PB1) & ~(1 << PB2);  // Apagar LED4

  // Hacer que cada LED parpadee en su color correspondiente
  while (1) {
    // Parpadear LED1 en Rojo
    PORTD |= (1 << PD2);  // Encender Rojo LED1
    _delay_ms(2000);
    PORTD &= ~(1 << PD2); // Apagar Rojo LED1
    _delay_ms(3000);

    // Parpadear LED2 en Verde
    PORTD |= (1 << PD4);  // Encender Verde LED2
    _delay_ms(2000);
    PORTD &= ~(1 << PD4); // Apagar Verde LED2
    _delay_ms(3000);

    // Parpadear LED3 en Azul
    PORTB |= (1 << PB5);  // Encender Azul LED3
    _delay_ms(2000);
    PORTB &= ~(1 << PB5); // Apagar Azul LED3
    _delay_ms(3000);

    // Parpadear LED4 en Amarillo (Rojo + Verde)
    PORTB |= (1 << PB0);  // Encender Rojo LED4
    PORTB |= (1 << PB1);  // Encender Verde LED4
    _delay_ms(2000);
    PORTB &= ~(1 << PB0); // Apagar Rojo LED4
    PORTB &= ~(1 << PB1); // Apagar Verde LED4
    _delay_ms(3000);
  }
}
