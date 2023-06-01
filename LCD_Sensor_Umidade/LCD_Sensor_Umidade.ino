/**
 * Este código mostra a umidade e a temperatura usando um sensor de umidade do ar
 */

#include <LiquidCrystal.h>
#include <DHT.h>

// Pinos do LCD
#define PIN_RS 2
#define PIN_E 4
#define PIN_DB4 10
#define PIN_DB5 11
#define PIN_DB6 12
#define PIN_DB7 13

// Pino do Sensor de Umidade (Analógico)
#define PIN_DHT11 = 6

LiquidCrystal lcd(PIN_RS, PIN_E, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
DHT dht(PIN_DHT11, DHT11);

// Ícone de Grau (°)
byte grau[8] {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16,2);
  lcd.clear();
  lcd.createChar(0, grau);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  lcd.clear();
  lcd.print("Umidade: ");
  lcd.print(h);
  lcd.print("%");
  lcd.setCursor(0, 1);

  lcd.print("Temp.: ");
  lcd.print(t);
  lcd.write(byte(0));
  lcd.print("C");

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.println("%");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C ");
}
