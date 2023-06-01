/**
 * Este código usa o sensor de movimento
 * Conecte o Relé (OPCIONAL) e o pino IN do sensor
 * O LED do Arduino também acenderá quando houver movimento
 * O sensor deve ser ligado no 5V, caso contrário, não funciona
 */

// Pinos do sensor e Relé
#define PIN_SENSOR 2
#define PIN_RELE 4

void setup() {
  pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_RELE,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(PIN_SENSOR) == HIGH) {
    // Há movimento
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_RELE, HIGH);
    Serial.println("Movimento");
  } else {
    // Não há movimento
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(PIN_RELE, LOW);
    Serial.println("Parado");
  }
}
