#include <PCD8544.h>  // Librería para controlar la pantalla LCD PCD8544 (Nokia 5110)

static PCD8544 display;  // Objeto para interactuar con la pantalla LCD
const float VOLTAGE_REFERENCE = 5.0;  // Referencia de voltaje para las lecturas ADC
const int ADC_RESOLUTION = 1023;  // Valor máximo del ADC (10 bits de resolución)

float max_voltage_A0 = 0;  // Máximo voltaje detectado en el pin A0
float max_voltage_A1 = 0;  // Máximo voltaje detectado en el pin A1
float max_voltage_A2 = 0;  // Máximo voltaje detectado en el pin A2
float max_voltage_A3 = 0;  // Máximo voltaje detectado en el pin A3

void setup() {
  // Inicialización del puerto serial
  Serial.begin(9600);

  // Configuración de pines
  pinMode(8, INPUT_PULLUP);  // Botón para alternar entre AC/DC
  pinMode(9, INPUT_PULLUP);  // Botón para mostrar valores en el monitor serial
  pinMode(10, OUTPUT);  // LED indicador para A3
  pinMode(11, OUTPUT);  // LED indicador para A2
  pinMode(12, OUTPUT);  // LED indicador para A1
  pinMode(13, OUTPUT);  // LED indicador para A0

  // Inicialización de la pantalla LCD
  display.begin(84, 48);  // Configuración de la resolución de la pantalla
}

// Función que convierte el valor del ADC a voltaje real utilizando la función de transferencia
float convert_adc_to_voltage(int analog_pin) {
  return analogRead(analog_pin) * VOLTAGE_REFERENCE / (ADC_RESOLUTION * 0.104) - 2.5 / 0.104;
}

// Función que obtiene el voltaje máximo de un puerto ADC
float get_peak_voltage(int analog_pin) {
  float peak_voltage = abs(convert_adc_to_voltage(analog_pin));  // Valor inicial
  while (true) {  // Bucle infinito para detectar el voltaje máximo
    float current_voltage = abs(convert_adc_to_voltage(analog_pin));  // Leer nuevo valor
    if (current_voltage > peak_voltage) {  // Actualizar el valor máximo si es necesario
      peak_voltage = current_voltage;
    } else {
      return peak_voltage;  // Retorna el valor máximo encontrado
    }
  }
}

void update_led_state(float voltage, int led_pin) {
  digitalWrite(led_pin, (voltage > 20 || voltage < -20) ? HIGH : LOW);  // Controla el LED según el voltaje
}

void loop() {
  // Leer el estado de los botones
  int is_dc_mode = digitalRead(9);  // Botón para alternar entre AC/DC
  int is_serial_enabled = digitalRead(8);  // Botón para habilitar la impresión serial

  // Leer los voltajes de los pines analógicos
  float voltage_A0 = convert_adc_to_voltage(A0);
  float voltage_A1 = convert_adc_to_voltage(A1);
  float voltage_A2 = convert_adc_to_voltage(A2);
  float voltage_A3 = convert_adc_to_voltage(A3);

  // Modo de medición de voltaje DC
  if (is_dc_mode) {
    // Actualizar el estado de los LEDs según el valor de los voltajes
    update_led_state(voltage_A0, 13);
    update_led_state(voltage_A1, 12);
    update_led_state(voltage_A2, 11);
    update_led_state(voltage_A3, 10);

    // Mostrar información en pantalla LCD
    display.setCursor(0, 0);  // Posicionar el cursor en la parte superior
    display.print("DC Voltages");  // Imprimir título
    display.setCursor(0, 1);  // Mover el cursor a la siguiente línea
    display.print("-----V-----");  // Separador
  }

  // Modo de medición de voltaje AC
  else {
    // Leer y actualizar los máximos voltajes RMS para cada canal
    float peak_voltage = get_peak_voltage(A0);
    if (peak_voltage > max_voltage_A0 || max_voltage_A0 - peak_voltage > 1) {
      max_voltage_A0 = peak_voltage;
    }
    voltage_A0 = max_voltage_A0 / sqrt(2);  // Calcular RMS

    peak_voltage = get_peak_voltage(A1);
    if (peak_voltage > max_voltage_A1 || max_voltage_A1 - peak_voltage > 1) {
      max_voltage_A1 = peak_voltage;
    }
    voltage_A1 = max_voltage_A1 / sqrt(2);

    peak_voltage = get_peak_voltage(A2);
    if (peak_voltage > max_voltage_A2 || max_voltage_A2 - peak_voltage > 1) {
      max_voltage_A2 = peak_voltage;
    }
    voltage_A2 = max_voltage_A2 / sqrt(2);

    peak_voltage = get_peak_voltage(A3);
    if (peak_voltage > max_voltage_A3 || max_voltage_A3 - peak_voltage > 1) {
      max_voltage_A3 = peak_voltage;
    }
    voltage_A3 = max_voltage_A3 / sqrt(2);

    // Actualizar el estado de los LEDs según el valor de los voltajes máximos
    update_led_state(max_voltage_A0, 13);
    update_led_state(max_voltage_A1, 12);
    update_led_state(max_voltage_A2, 11);
    update_led_state(max_voltage_A3, 10);

    // Mostrar información en pantalla LCD
    display.setCursor(0, 0);
    display.print("AC Voltages");
    display.setCursor(0, 1);
    display.print("----rms----");
  }

  // Imprimir voltajes en el monitor serial si el botón está activado
  if (!is_serial_enabled) {
    Serial.print("V1: "); Serial.println(voltage_A3);
    Serial.print("V2: "); Serial.println(voltage_A2);
    Serial.print("V3: "); Serial.println(voltage_A1);
    Serial.print("V4: "); Serial.println(voltage_A0);
  }

  // Mostrar los voltajes en la pantalla LCD
  display.setCursor(0, 2); display.print("V1: "); display.print(voltage_A3, 2);
  display.setCursor(0, 3); display.print("V2: "); display.print(voltage_A2, 2);
  display.setCursor(0, 4); display.print("V3: "); display.print(voltage_A1, 2);
  display.setCursor(0, 5); display.print("V4: "); display.print(voltage_A0, 2);
}
