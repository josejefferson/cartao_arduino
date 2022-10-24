#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 sensorDeCartao(SS_PIN, RST_PIN);

String cartaoMestre = "80 81 B8 79";
String cartaoAnterior;
String cartoesCadastrados[10];
int numeroDeCartoesCadastrados = 0;
#define adicionarCartao(d) cartoesCadastrados[numeroDeCartoesCadastrados++] = d
bool modo_cadastrar = false;

int VERMELHO = 6;
int VERDE = 7;

void setup() {
  pinMode(VERDE, OUTPUT); // LED Verde
  pinMode(VERMELHO, OUTPUT); // LED Vermelho

  Serial.begin(9600);
  SPI.begin();
  sensorDeCartao.PCD_Init();

  Serial.println("Aproxime o seu cartao do leitor...\n");
}

void loop() {
  int temCartao = sensorDeCartao.PICC_IsNewCardPresent();
  int leuCartao = sensorDeCartao.PICC_ReadCardSerial();

  if (!temCartao || !leuCartao) {
    // Se nenhum cartão for detectado
    cartaoAnterior = "";

    // Apaga o LED verde
    digitalWrite(VERDE, LOW);
  } else {
    // Cartão detectado
    String cartao = "";
    byte letra;
    for (byte i = 0; i < sensorDeCartao.uid.size; i++) {
      cartao.concat(String(sensorDeCartao.uid.uidByte[i] < 0x10 ? " 0" : " "));
      cartao.concat(String(sensorDeCartao.uid.uidByte[i], HEX));
    }
    cartao.toUpperCase();
    Serial.println("UID do cartão: " + cartao + "\n");

    // Somente executa se o cartão foi trocado
    if (cartao != cartaoAnterior) {
      // Se estiver no modo de cadastro
      if (modo_cadastrar) {
        modo_cadastrar = false;
        adicionarCartao(cartao);
        Serial.print("Cartão cadastrado: " + cartao);
        piscarLED(VERMELHO);
      }

      // Se o cartão for o mestre
      else if (cartao.substring(1) == cartaoMestre) {
        modo_cadastrar = true;
        Serial.println("Modo de cadastro iniciado");
        digitalWrite(VERMELHO, HIGH);
      }

      // Se o cartão for outro
      else {
        for (int i = 0; i < numeroDeCartoesCadastrados; i++) {
          if (cartao == cartoesCadastrados[i]) {
            digitalWrite(VERDE, HIGH);
          }
        }
      }

      cartaoAnterior = cartao;
    }

    sensorDeCartao.PICC_IsNewCardPresent();
    sensorDeCartao.PICC_ReadCardSerial();
  }
}

void piscarLED(int led) {
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
}
