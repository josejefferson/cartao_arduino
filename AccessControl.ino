#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h> // --LCD--

#define COMMON_ANODE // Anodo comum do LED

#define redLed 14 // Pino do LED vermelho
#define greenLed 15 // Pino do LED verde
#define yellowLed 16 // Pino do LED amarelo

#define relay 1 // Pino do Relé
#define wipeB 8 // Pino do botão RESET

#ifdef COMMON_ANODE
#define LED_ON LOW
#define LED_OFF HIGH
#else
#define LED_ON HIGH
#define LED_OFF LOW
#endif

bool programMode = false; // initialize programming mode to false
uint8_t successRead; // Variable integer to keep if we have Successful Read from Reader

byte storedCard[4]; // Stores an ID read from EEPROM
byte readCard[4]; // Stores scanned ID read from RFID Module
byte masterCard[4]; // Stores master card's ID read from EEPROM

byte padLock1[] = { B00000, B00000, B00011, B00111, B01110, B01100, B01100, B01100 };
byte padLock2[] = { B00000, B00000, B11000, B11100, B01110, B00110, B00110, B00110 };
byte padLock1Open[] = { B00000, B00011, B00111, B01110, B01100, B01100, B01100, B01100 };
byte padLock2Open[] = { B00000, B11000, B11100, B01110, B00110, B00110, B00010, B00000 };
byte padLock3[] = { B11111, B11111, B11110, B11100, B11110, B11111, B11111, B11111 };
byte padLock4[] = { B11111, B11111, B01111, B00111, B01111, B11111, B11111, B11111 };

// Create MFRC522 instance.
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // --LCD--
void lcdprint(String line1); // --LCD--
void lcdprint(String line1, String line2); // --LCD--

///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  lcd.begin(16, 2); // --LCD--
  lcd.createChar(0, padLock1); // --LCD--
  lcd.createChar(1, padLock2); // --LCD--
  lcd.createChar(2, padLock3); // --LCD--
  lcd.createChar(3, padLock4); // --LCD--
  lcd.createChar(4, padLock1Open); // --LCD--
  lcd.createChar(5, padLock2Open); // --LCD--

  //Arduino Pin Configuration
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(wipeB, INPUT_PULLUP); // Enable pin's pull up resistor
  pinMode(relay, OUTPUT);
  //Be careful how relay circuit behave on while resetting or power-cycling your Arduino
  digitalWrite(relay, HIGH); // Make sure door is locked
  digitalWrite(redLed, LED_OFF); // Make sure led is off
  digitalWrite(greenLed, LED_OFF); // Make sure led is off
  digitalWrite(yellowLed, LED_OFF); // Make sure led is off

  //Protocol Configuration
  Serial.begin(9600); // Initialize serial communications with PC
  SPI.begin(); // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init(); // Initialize MFRC522 Hardware

  //If you set Antenna Gain to Max it will increase reading distance
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  Serial.println(F("Access Control Example v0.1")); // For debugging purposes
  ShowReaderDetails(); // Show details of PCD - MFRC522 Card Reader details

  //Wipe Code - If the Button (wipeB) Pressed while setup run (powered on) it wipes EEPROM
  if (digitalRead(wipeB) == LOW) { // when button pressed pin should get low, button connected to ground
    digitalWrite(redLed, LED_ON); // Red Led stays on to inform user we are going to wipe
    Serial.println(F("> LIMPAR MEMÓRIA"));
    Serial.println(F("Solte o botao dentro de 10s para cancelar"));
    lcdprint("LIMPAR MEMÓRIA", "10s para limpar"); // --LCD--
    bool buttonState = monitorWipeButton(10000); // Give user enough time to cancel operation
    if (buttonState == true && digitalRead(wipeB) == LOW) { // If button still be pressed, wipe EEPROM
      Serial.println(F("Limpando memoria..."));
      lcdprint("LIMPANDO MEMORIA", "Aguarde..."); // --LCD--
      for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) { //Loop end of EEPROM address
        if (EEPROM.read(x) != 0) EEPROM.write(x, 0); // if not write 0 to clear, it takes 3.3mS
      }
      lcdprint("A MEMORIA FOI", "LIMPA C/SUCESSO"); // --LCD--
      Serial.println(F("Memoria limpa com sucesso"));
      digitalWrite(redLed, LED_OFF); // visualize a successful wipe
      delay(200);
      digitalWrite(redLed, LED_ON);
      delay(200);
      digitalWrite(redLed, LED_OFF);
      delay(200);
      digitalWrite(redLed, LED_ON);
      delay(200);
      digitalWrite(redLed, LED_OFF);
    } else {
      Serial.println(F("Limpeza da memoria cancelada")); // Show some feedback that the wipe button did not pressed for 15 seconds
      digitalWrite(redLed, LED_OFF);
			lcdprint("LIMPEZA DA MEM.", "CANCELADA"); // --LCD--
			delay(2000); // --LCD--
    }
  }
  // Check if master card defined, if not let user choose a master card
  // This also useful to just redefine the Master Card
  // You can keep other EEPROM records just write other than 143 to EEPROM address 1
  // EEPROM address 1 should hold magical number which is '143'
  if (EEPROM.read(1) != 143) {
    Serial.println(F("Nenhum cartao mestre cadastrado"));
    Serial.println(F("Escaneie um cartao para defini-lo como mestre"));
    lcdprint("INSIRA UM CARTAO", "MESTRE"); // --LCD--
    do {
      successRead = getID(); // sets successRead to 1 when we get read from reader otherwise 0
      digitalWrite(yellowLed, LED_ON); // Visualize Master Card need to be defined
      delay(200);
      digitalWrite(yellowLed, LED_OFF);
      delay(200);
    }
    while (!successRead); // Program will not go further while you not get a successful read
    for (uint8_t j = 0; j < 4; j++) { // Loop 4 times
      EEPROM.write(2 + j, readCard[j]); // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1, 143); // Write to EEPROM we defined Master Card.
    Serial.println(F("Cartao mestre definido!"));
    lcdprint("CARTAO MESTRE", "CADASTRADO"); // --LCD--
		delay(2000); // --LCD--
  }
  Serial.println(F("-------------------"));
  Serial.println(F("ID do cartao mestre"));
  for (uint8_t i = 0; i < 4; i++) { // Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i); // Write it to masterCard
    Serial.print(masterCard[i], HEX);
  }
  Serial.println("");
  Serial.println(F("-------------------"));
  Serial.println(F("Aguardando cartoes para serem escaneados..."));
  cycleLeds(); // Everything ready lets give user some feedback by cycling leds
}

///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop() {
	lcdprint("\x01\x02", "\x03\x04"); // --LCD--
  do {
    successRead = getID(); // sets successRead to 1 when we get read from reader otherwise 0
    // When device is in use if wipe button pressed for 10 seconds initialize Master Card wiping
    if (digitalRead(wipeB) == LOW) { // Check if button is pressed
      // Visualize normal operation is iterrupted by pressing wipe button Red is like more Warning to user
      digitalWrite(redLed, LED_ON); // Make sure led is off
      digitalWrite(greenLed, LED_OFF); // Make sure led is off
      digitalWrite(yellowLed, LED_OFF); // Make sure led is off
      // Give some feedback
      Serial.println(F("> LIMPAR CARTAO MESTRE"));
      Serial.println(F("Solte o botao dentro de 10s para cancelar"));
      lcdprint("APAGAR MESTRE", "10s para apagar"); // --LCD--
      bool buttonState = monitorWipeButton(10000); // Give user enough time to cancel operation
      lcdprint("APAGANDO MESTRE", "Aguarde..."); // --LCD--
      if (buttonState == true && digitalRead(wipeB) == LOW) { // If button still be pressed, wipe EEPROM
        EEPROM.write(1, 0); // Reset Magic Number.
        Serial.println(F("Cartao mestre apagado!"));
        Serial.println(F("RESETE O ARDUINO"));
        lcdprint("MESTRE APAGADO!", "Reinicie Arduino"); // --LCD--
        while (1);
      }
      Serial.println(F("Exclusao do cartao mestre cancelada!"));
			lcdprint("EXCLUSAO MESTRE", "CANCELADA"); // --LCD--
			delay(2000); // --LCD--
    }
    if (programMode) {
      cycleLeds(); // Program Mode cycles through Red Green Blue waiting to read a new card
    } else {
      normalModeOn(); // Normal mode, blue Power LED is on, all others are off
    }
  }
  while (!successRead); //the program will not go further while you are not getting a successful read
  if (programMode) {
    if (isMaster(readCard)) { //When in program mode check First If master card scanned again to exit program mode
      Serial.println(F("Cartao mestre escaneado"));
      Serial.println(F("Saindo do modo de Programacao"));
      Serial.println(F("-----------------------------"));
      programMode = false;
			lcdprint("\x01\x02", "\x03\x04"); // --LCD--
      return;
    } else {
      if (findID(readCard)) { // If scanned card is known delete it
        Serial.println(F("Cartao ja cadastrado, removendo..."));
        lcdprint("CARTAO EXISTENTE", "Removendo..."); // --LCD--
        deleteID(readCard);
				lcdprint("CARTAO EXISTENTE", "REMOVIDO"); // --LCD--
				delay(1000);  // --LCD--
      } else { // If scanned card is not known add it
        Serial.println(F("Cartao nao cadastrado, adicionando..."));
        lcdprint("CARTAO NOVO", "Adicionando..."); // --LCD--
        writeID(readCard);
        lcdprint("CARTAO NOVO", "ADICIONADO"); // --LCD--
				delay(1000);  // --LCD--
      }
			Serial.println(F("--------------------------------------------"));
			Serial.println(F("Escaneie um cartao para ADICIONAR ou REMOVER"));
			lcdprint("MODO PROGRAMACAO", "[MESTRE] Sair   "); // --LCD--
    }
  } else {
    if (isMaster(readCard)) { // If scanned card's ID matches Master Card's ID - enter program mode
      programMode = true;
      Serial.println(F("> MODO DE PROGRAMACAO"));
			Serial.println(F("Escaneie um cartao para ADICIONAR ou REMOVER"));
			lcdprint("MODO PROGRAMACAO", "[MESTRE] Sair   "); // --LCD--
      // uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that
      // Serial.print(F("Existem ")); // stores the number of ID's in EEPROM
      // Serial.print(count);
      // Serial.print(F(" cartoes cadastrados"));
      // Serial.println("");
      // Serial.println(F("Escaneie um cartao para ADICIONAR ou REMOVER"));
      // Serial.println(F("Escaneie o cartao mestre novamente para SAIR"));
      // Serial.println(F("-----------------------------"));
      // lcdprint("* MODO PROGR. *", count + " cartoes cad."); // --LCD--
    } else {
      if (findID(readCard)) { // If not, see if the card is in the EEPROM
        Serial.println(F("BEM-VINDO, USUARIO AUTENTICADO!"));
        lcdprint("   BEM-VINDO"); // --LCD--
        granted(300); // Open the door lock for 300 ms
      } else { // If not, show that the ID was not valid
        Serial.println(F("BLOQUEADO! INVASOR!"));
        lcdprint("     CARTAO     ", "    RECUSADO    "); // --LCD--
        denied();
      }
    }
  }
}

/////////////////////////////////////////  Access Granted    ///////////////////////////////////
void granted(uint16_t setDelay) {
  digitalWrite(yellowLed, LED_OFF); // Turn off blue LED
  digitalWrite(redLed, LED_OFF); // Turn off red LED
  digitalWrite(greenLed, LED_ON); // Turn on green LED
  digitalWrite(relay, LOW); // Unlock door!
  delay(setDelay); // Hold door lock open for given seconds
  digitalWrite(relay, HIGH); // Relock door
  delay(1000); // Hold green LED on for a second
}

///////////////////////////////////////// Access Denied  ///////////////////////////////////
void denied() {
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_ON); // Turn on red LED
  delay(1000);
}

///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
uint8_t getID() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
	lcdprint("VERIFICANDO", "CARTAO"); // --LCD--
  Serial.println(F("ID do cartao escaneado:"));
  for (uint8_t i = 0; i < 4; i++) { //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    // Visualize system is halted
    digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
    digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
    digitalWrite(redLed, LED_ON); // Turn on red LED
    while (true); // do not go further
  }
}

///////////////////////////////////////// Cycle Leds (Program Mode) ///////////////////////////////////
void cycleLeds() {
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(yellowLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  delay(200);
}

//////////////////////////////////////// Normal Mode Led  ///////////////////////////////////
void normalModeOn() {
  digitalWrite(yellowLed, LED_ON); // Blue LED ON and ready to read card
  digitalWrite(redLed, LED_OFF); // Make sure Red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure Green LED is off
  digitalWrite(relay, HIGH); // Make sure Door is Locked
}

//////////////////////////////////////// Read an ID from EEPROM //////////////////////////////
void readID(uint8_t number) {
  uint8_t start = (number * 4) + 2; // Figure out starting position
  for (uint8_t i = 0; i < 4; i++) { // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i); // Assign values read from EEPROM to array
  }
}

///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
void writeID(byte a[]) {
  if (!findID(a)) { // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(0); // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = (num * 4) + 6; // Figure out where the next slot starts
    num++; // Increment the counter by one
    EEPROM.write(0, num); // Write the new count to the counter
    for (uint8_t j = 0; j < 4; j++) { // Loop 4 times
      EEPROM.write(start + j, a[j]); // Write the array values to EEPROM in the right position
    }
    successWrite();
    Serial.println(F("Succesfully added ID record to EEPROM"));
  } else {
    failedWrite();
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}

///////////////////////////////////////// Remove ID from EEPROM   ///////////////////////////////////
void deleteID(byte a[]) {
  if (!findID(a)) { // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite(); // If not
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  } else {
    uint8_t num = EEPROM.read(0); // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot; // Figure out the slot number of the card
    uint8_t start; // = ( num * 4 ) + 6; // Figure out where the next slot starts
    uint8_t looping; // The number of times the loop repeats
    uint8_t j;
    uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT(a); // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--; // Decrement the counter by one
    EEPROM.write(0, num); // Write the new count to the counter
    for (j = 0; j < looping; j++) { // Loop the card shift times
      EEPROM.write(start + j, EEPROM.read(start + 4 + j)); // Shift the array values to 4 places earlier in the EEPROM
    }
    for (uint8_t k = 0; k < 4; k++) { // Shifting loop
      EEPROM.write(start + j + k, 0);
    }
    successDelete();
    Serial.println(F("Succesfully removed ID record from EEPROM"));
  }
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
bool checkTwo(byte a[], byte b[]) {
  for (uint8_t k = 0; k < 4; k++) { // Loop 4 times
    if (a[k] != b[k]) { // IF a != b then false, because: one fails, all fail
      return false;
    }
  }
  return true;
}

///////////////////////////////////////// Find Slot   ///////////////////////////////////
uint8_t findIDSLOT(byte find[]) {
  uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that
  for (uint8_t i = 1; i <= count; i++) { // Loop once for each EEPROM entry
    readID(i); // Read an ID from EEPROM, it is stored in storedCard[4]
    if (checkTwo(find, storedCard)) { // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i; // The slot number of the card
    }
  }
}

///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
bool findID(byte find[]) {
  uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that
  for (uint8_t i = 1; i < count; i++) { // Loop once for each EEPROM entry
    readID(i); // Read an ID from EEPROM, it is stored in storedCard[4]
    if (checkTwo(find, storedCard)) { // Check to see if the storedCard read from EEPROM
      return true;
    } else { // If not, return false
    }
  }
  return false;
}

///////////////////////////////////////// Write Success to EEPROM   ///////////////////////////////////
// Flashes the green LED 3 times to indicate a successful write to EEPROM
void successWrite() {
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
}

///////////////////////////////////////// Write Failed to EEPROM   ///////////////////////////////////
// Flashes the red LED 3 times to indicate a failed write to EEPROM
void failedWrite() {
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
}

///////////////////////////////////////// Success Remove UID From EEPROM  ///////////////////////////////////
// Flashes the blue LED 3 times to indicate a success delete to EEPROM
void successDelete() {
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(yellowLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(yellowLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(yellowLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(yellowLed, LED_ON); // Make sure blue LED is on
  delay(200);
}

////////////////////// Check readCard IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
bool isMaster(byte test[]) {
  return checkTwo(test, masterCard);
}

bool monitorWipeButton(uint32_t interval) {
  uint32_t now = (uint32_t) millis();
  while ((uint32_t) millis() - now < interval) {
    // check on every half a second
    if (((uint32_t) millis() % 500) == 0) {
      if (digitalRead(wipeB) != LOW)
        return false;
    }
  }
  return true;
}

////////////////////////// PRINT TO LCD ///////////////////////////////
void lcdprint(String line1) { // --LCD--
  lcdprint(line1, ""); // --LCD--
} // --LCD--

void lcdprint(String line1, String line2) { // --LCD--
	line1.trim(); // --LCD--
	line2.trim(); // --LCD--
	uint8_t line1Len = line1.length(); // --LCD--
	uint8_t line2Len = line2.length(); // --LCD--
  for (uint8_t i = 0; i < (16 - line1Len) / 2; i++) line1 = " " + line1; // --LCD--
	for (uint8_t i = 0; i < (16 - line2Len) / 2; i++) line2 = " " + line2; // --LCD--
  lcd.clear(); // --LCD--
  lcd.print(line1); // --LCD--
  lcd.setCursor(0, 1); // --LCD--
  lcd.print(line2); // --LCD--
} // --LCD--