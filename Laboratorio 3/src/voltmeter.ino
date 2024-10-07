#include <PCD8544.h>

PCD8544 lcd;
const float VREF = 5.0;
const int ADC_MAX_VALUE = 1023;

void setup() {
  Serial.begin(9600);
  lcd.begin(84, 48);

  pinMode(10, OUTPUT);
}

float read_voltage(int port) {
  return analogRead(port) * VREF / ADC_MAX_VALUE;
}

void loop() {
  float voltage = read_voltage(A0);

  if (voltage > 20 || voltage < -20) {
    digitalWrite(10, HIGH);
  } else {
    digitalWrite(10, LOW);
  }

  lcd.setCursor(0, 0);
  lcd.print("Voltage: ");
  lcd.print(voltage, 2);

  Serial.print("Voltage: ");
  Serial.println(voltage, 2);

  delay(1000);  // Small delay to avoid overwhelming updates
}
