#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

char st[20];

void setup()
{
  pinMode(7, OUTPUT); // Verde
  pinMode(6, OUTPUT); // Amarelo

  Serial.begin(9600);   // Inicia comunicação Serial em 9600 baud rate
  SPI.begin();          // Inicia comunicação SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

String cartao_anterior = "";

void loop()
{
  int a = mfrc522.PICC_IsNewCardPresent();
  int b = mfrc522.PICC_ReadCardSerial();
  Serial.println(a);
  Serial.println(b);
  if (!a || !b) {
    // Se nenhum cartão for detectado
    cartao_anterior = "";
    Serial.println("Nenhum cartao");
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
  } else {
    
    // Tem cartão
    Serial.print("UID da tag :");
    String cartao = "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      cartao.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      cartao.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    cartao.toUpperCase();
    Serial.println();

    mfrc522.PICC_IsNewCardPresent();
    mfrc522.PICC_ReadCardSerial();

    if (cartao != cartao_anterior) {
      if (cartao.substring(1) == "50 DE CA A3") {
        Serial.println("Chaveiro 1 identificado!\n");
        digitalWrite(7, HIGH);
      }

      if (cartao.substring(1) == "80 81 B8 79") {
        Serial.println("Cartao 2 identificado\n");
        digitalWrite(6, HIGH);
      }

      cartao_anterior = cartao;
    }
  }
}
