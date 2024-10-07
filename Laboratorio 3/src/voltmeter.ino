#include <PCD8544.h>

static PCD8544 lcd;
const float VREF = 5.0;
const int ADC_MAX_VALUE = 1023;

float global_max_voltageA0 = 0, global_max_voltageA1 = 0, global_max_voltageA2 = 0, global_max_voltageA3 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT_PULLUP); pinMode(9, INPUT_PULLUP);
  pinMode(10, OUTPUT); pinMode(11, OUTPUT); pinMode(12, OUTPUT); pinMode(13, OUTPUT);
  lcd.begin(84, 48);
}

float reverse_TF(int port) {
  return analogRead(port) * VREF / (ADC_MAX_VALUE * 0.104) - 2.5 / 0.104;
}

float get_max_voltage(int port) {
  float max_voltage = abs(reverse_TF(port));
  while (true) {
    float voltage = abs(reverse_TF(port));
    if (voltage > max_voltage) max_voltage = voltage;
    else return max_voltage;
  }
}

void update_dc(float voltage, int pin) {
  digitalWrite(pin, (voltage > 20 || voltage < -20) ? HIGH : LOW);
}

void loop() {
  int switch_DC = digitalRead(9), switch_Serial = digitalRead(8);

  float voltageA0 = reverse_TF(A0), voltageA1 = reverse_TF(A1);
  float voltageA2 = reverse_TF(A2), voltageA3 = reverse_TF(A3);

  if (switch_DC) {
    update_dc(voltageA0, 13); update_dc(voltageA1, 12);
    update_dc(voltageA2, 11); update_dc(voltageA3, 10);

    lcd.setCursor(0, 0); lcd.print("DC Voltages");
    lcd.setCursor(0, 1); lcd.print("-----V-----");
  } else {
    voltageA0 = get_max_voltage(A0) / sqrt(2);
    voltageA1 = get_max_voltage(A1) / sqrt(2);
    voltageA2 = get_max_voltage(A2) / sqrt(2);
    voltageA3 = get_max_voltage(A3) / sqrt(2);

    update_dc(global_max_voltageA0, 13); update_dc(global_max_voltageA1, 12);
    update_dc(global_max_voltageA2, 11); update_dc(global_max_voltageA3, 10);

    lcd.setCursor(0, 0); lcd.print("AC Voltages");
    lcd.setCursor(0, 1); lcd.print("----rms----");
  }

  if (!switch_Serial) {
    Serial.print("V1: "); Serial.println(voltageA3);
    Serial.print("V2: "); Serial.println(voltageA2);
    Serial.print("V3: "); Serial.println(voltageA1);
    Serial.print("V4: "); Serial.println(voltageA0);
  }

  lcd.setCursor(0, 2); lcd.print("V1: "); lcd.print(voltageA3, 2);
  lcd.setCursor(0, 3); lcd.print("V2: "); lcd.print(voltageA2, 2);
  lcd.setCursor(0, 4); lcd.print("V3: "); lcd.print(voltageA1, 2);
  lcd.setCursor(0, 5); lcd.print("V4: "); lcd.print(voltageA0, 2);
}
