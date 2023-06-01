/**
 * Este código mostra a distância em cm usando um sensor ultrassônico
 */

#include <Ultrasonic.h>
#include <LiquidCrystal.h>

// Pinos do LCD
#define PIN_RS 2
#define PIN_E 4
#define PIN_DB4 10
#define PIN_DB5 11
#define PIN_DB6 12
#define PIN_DB7 13

// Pinos do Sensor Ultrassônico
#define PIN_TRIG 5
#define PIN_ECHO 6

LiquidCrystal lcd(PIN_RS, PIN_E, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
Ultrasonic ultrasonic(PIN_TRIG, PIN_ECHO);

void setup() {
  Serial.begin(9600);
  Serial.println("Lendo dados do sensor...");
  lcd.begin(16,2);
  lcd.clear();
}

void loop() {
  float cm;
  long microsec = ultrasonic.timing();
  cm = ultrasonic.convert(microsec, Ultrasonic::CM);

  Serial.print("Distancia em cm: ");
  Serial.println(cm);
  delay(100);

  lcd.clear();
  lcd.print("Distancia:");
  lcd.setCursor(0, 1);
  lcd.print(cm);
  lcd.print(" cm");
}
