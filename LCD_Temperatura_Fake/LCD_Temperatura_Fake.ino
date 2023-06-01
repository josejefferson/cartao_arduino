/**
 * Este código mostra uma temperatura Fake no display LCD
 * Esta temperatura aumenta 1 a cada 300ms e quando chega em 99, diminui 1
 */

#include <LiquidCrystal.h>

// Pinos do LCD
#define PIN_RS 2
#define PIN_E 4
#define PIN_DB4 10
#define PIN_DB5 11
#define PIN_DB6 12
#define PIN_DB7 13

LiquidCrystal lcd(PIN_RS, PIN_E, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);

// Parte 1 da figura do termômetro
byte termometro1[] = {
  B00000,
  B00001,
  B00010,
  B00100,
  B00100,
  B00100,
  B00100,
  B00111
};

// Parte 2 da figura do termômetro
byte termometro2[] = {
  B00000,
  B10000,
  B01011,
  B00100,
  B00111,
  B00100,
  B00111,
  B11100
};

// Parte 3 da figura do termômetro
byte termometro3[] = {
  B00111,
  B00111,
  B00111,
  B01111,
  B11111,
  B11111,
  B01111,
  B00011
};

// Parte 4 da figura do termômetro
byte termometro4[] = {
  B11111,
  B11100,
  B11100,
  B11110,
  B11111,
  B11111,
  B11110,
  B11000
};

void setup() {
  lcd.begin(16,2);
  lcd.createChar(5, termometro1);
  lcd.createChar(6, termometro2);
  lcd.createChar(7, termometro3);
  lcd.createChar(8, termometro4);
  lcd.clear();
  lcd.print("\x05\x06 Temperatura:");
  lcd.setCursor(0, 1);
  lcd.print("\x07\x08 26 graus");
}

void loop() {
  // Aumenta a temperatura até 99
  for (int i = 0; i <= 99; i++) {
    lcd.setCursor(3, 1);
    lcd.print("  ");
    lcd.setCursor(3, 1);
    lcd.print(i);
    delay(300);
  }

  // Diminui a temperatura até 0
  for (int i = 98; i > 0; i--) {
    lcd.setCursor(3, 1);
    lcd.print("  ");
    lcd.setCursor(3, 1);
    lcd.print(i);
    delay(300);
  }
}
