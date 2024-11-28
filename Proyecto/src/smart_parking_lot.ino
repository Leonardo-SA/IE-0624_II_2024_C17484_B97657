#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2fDlUBZiC"
#define BLYNK_TEMPLATE_NAME "Smart Car Parking System"
#define BLYNK_AUTH_TOKEN "B6y6kA5cnLk05QZRepvevwRepjjRDhXS"

#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char ssid[] = "Sebachoserra";
char pass[] = "53b@ch04521";

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

  lcd.init();
  lcd.backlight();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  int Slot1 = digitalRead(25);
  int Slot2 = digitalRead(26);
  int Slot3 = digitalRead(27);

  int In = digitalRead(32);
  int OUT = digitalRead(33);

  if (Slot1 == 0)
  {
      Serial.println("slot1 available");
      lcd.setCursor(0, 0);
      lcd.print("Slot1:");
      lcd.setCursor(6, 0);
      lcd.print("A ");
  }
  else
  {
      Serial.println("slot1 not available");
      lcd.setCursor(0, 0);
      lcd.print("Slot1:");
      lcd.setCursor(6, 0);
      lcd.print("NA");
  }

  if (Slot2 == 0)
  {
      Serial.println("slot2 available");
      lcd.setCursor(8, 0);
      lcd.print("Slot2:");
      lcd.setCursor(14, 0);
      lcd.print("A ");
  }
  else
  {
      Serial.println("slot2 not available");
      lcd.setCursor(8, 0);
      lcd.print("Slot2:");
      lcd.setCursor(14, 0);
      lcd.print("NA");
  }

  if (Slot3 == 0)
  {
      Serial.println("slot3 available");
      lcd.setCursor(0, 1);
      lcd.print("Slot3:");
      lcd.setCursor(6, 1);
      lcd.print("A ");
  }
  else
  {
      Serial.println("slot3 not available");
      lcd.setCursor(0, 1);
      lcd.print("Slot3:");
      lcd.setCursor(6, 1);
      lcd.print("NA");
  }

  int Total = 3 - (Slot1 + Slot2 + Slot3);
  lcd.setCursor(9, 1);
  lcd.print("Total:");
  lcd.setCursor(15, 1);
  lcd.print(Total);
  Serial.print("Total Available Slots:");
  Serial.println(Total);
  Serial.println("****************************************************");

  Blynk.virtualWrite(V0, Slot1);
  Blynk.virtualWrite(V1, Slot2);
  Blynk.virtualWrite(V2, Slot3);
  Blynk.virtualWrite(V3, Total);
  Blynk.run();
}
