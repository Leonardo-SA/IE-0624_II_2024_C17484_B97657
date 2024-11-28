#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2fDlUBZiC"
#define BLYNK_TEMPLATE_NAME "Smart Car Parking System"
#define BLYNK_AUTH_TOKEN "B6y6kA5cnLk05QZRepvevwRepjjRDhXS"

#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char ssid[] = "<Nombre de la red>";
char pass[] = "<Contraseña de la red>";

Servo myservo1;
Servo myservo2;
int servoPin1 = 18;
int servoPin2 = 19;

// Variables para almacenar el estado de los slots
bool slot1State = false;
bool slot2State = false;
bool slot3State = false;

// Variables para rastrear activación manual
bool slot1Manual = false;
bool slot2Manual = false;
bool slot3Manual = false;

// Función para actualizar la pantalla LCD
void updateLCD()
{
  lcd.setCursor(0, 0);
  lcd.print("Slot1:");
  lcd.setCursor(6, 0);
  lcd.print(slot1State ? "NA" : "A ");

  lcd.setCursor(8, 0);
  lcd.print("Slot2:");
  lcd.setCursor(14, 0);
  lcd.print(slot2State ? "NA" : "A ");

  lcd.setCursor(0, 1);
  lcd.print("Slot3:");
  lcd.setCursor(6, 1);
  lcd.print(slot3State ? "NA" : "A ");

  int totalAvailable = 3 - (slot1State + slot2State + slot3State);
  lcd.setCursor(9, 1);
  lcd.print("Total:");
  lcd.setCursor(15, 1);
  lcd.print(totalAvailable);
}

// Manejo de switches en Blynk
BLYNK_WRITE(V0)
{
  slot1Manual = param.asInt(); // Registrar activación manual
  slot1State = slot1Manual;   // Forzar estado a NA si manualmente se activa
  updateLCD();
}

BLYNK_WRITE(V1)
{
  slot2Manual = param.asInt();
  slot2State = slot2Manual;
  updateLCD();
}

BLYNK_WRITE(V2)
{
  slot3Manual = param.asInt();
  slot3State = slot3Manual;
  updateLCD();
}

void setup()
{
  Serial.begin(115200);
  pinMode(25, INPUT);
  pinMode(26, INPUT);
  pinMode(27, INPUT);

  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);

  myservo1.attach(servoPin1);
  myservo2.attach(servoPin2);

  lcd.init();
  lcd.backlight();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  updateLCD();
}

void loop()
{
  // Leer los sensores de los slots solo si no están en modo manual
  if (!slot1Manual)
    slot1State = digitalRead(25) == HIGH;

  if (!slot2Manual)
    slot2State = digitalRead(26) == HIGH;

  if (!slot3Manual)
    slot3State = digitalRead(27) == HIGH;

  updateLCD();

  int In = digitalRead(32);
  int OUT = digitalRead(33);

  if (In == LOW)
  {
      myservo1.write(90);
      delay(1000);
  }
  else
  {
      myservo1.write(180);
      delay(1000);
  }

  if (OUT == LOW)
  {
      myservo2.write(0);
      delay(1000);
  }
  else
  {
      myservo2.write(90);
      delay(1000);
  }

  // Enviar datos a Blynk
  Blynk.virtualWrite(V0, slot1State);
  Blynk.virtualWrite(V1, slot2State);
  Blynk.virtualWrite(V2, slot3State);

  int totalAvailable = 3 - (slot1State + slot2State + slot3State);
  Blynk.virtualWrite(V3, totalAvailable);

  Blynk.run();
}
