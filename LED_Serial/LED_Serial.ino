/**
 * Desenvolvido por Jefferson Dantas
 * 
 * Este código acende/apaga o LED pela porta serial
 * Digite "lamp1-on" para ligar
 * Digite "lamp1-off" para desligar
 * 
 * Este código pode ser usado para acender o LED pelo celular/computador
 * https://github.com/josejefferson/arduino-serial-node
 */

/** Pino da lâmpada */
#define LAMP1_PIN 4

void setup() {
  pinMode(LAMP1_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(10);
}

/** Lâmpada está acesa */
bool LAMP1_SWITCHED_ON = false;

void loop() { 
  if (Serial.available() > 0) {
    String content = Serial.readString();
    content.trim();
    
    if (content == "lamp1-on") {
      // Acende a lâmpada
      digitalWrite(LAMP1_PIN, HIGH);
      LAMP1_SWITCHED_ON = true;
      Serial.println("lamp1-on");
    } else if (content == "lamp1-off") {
      // Apaga a lâmpada
      digitalWrite(LAMP1_PIN, LOW);
      LAMP1_SWITCHED_ON = false;
      Serial.println("lamp1-off");
    }
  }
}