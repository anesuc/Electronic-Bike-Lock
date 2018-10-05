/*
 * --------------------------------------------------------------------------------------------------------------------
 * Bike locker main control code.
 * --------------------------------------------------------------------------------------------------------------------
 * This is the main code of our bike locker. It essentially checks if the card being read is new and then checks if it matches what is currently stored in memory and acts accordingly.
 * 
 * By Group 37 Engineering Design 3B
 * 
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above
int greenLedPin = 7;               // Green LED output pin
int redLedPin = 8;                // Red LED output pin
int actuatorTriggerA = 2;          // Actuator trigger A pin
int actuatorTriggerB = 4;          // Actuator trigger B pin
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

//Initialize the lock as unlocked
bool lockLocked = false;

void setup() { 
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }


  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(actuatorTriggerA, OUTPUT);
  pinMode(actuatorTriggerB, OUTPUT);
  digitalWrite(greenLedPin, HIGH);
}
 
void loop() {

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    if (!lockLocked) {

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

   //Lock routine
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    digitalWrite(actuatorTriggerB, HIGH);
    //digitalWrite(actuatorTriggerB, HIGH);
    delay(1000);
    digitalWrite(actuatorTriggerB, LOW);
    //digitalWrite(actuatorTriggerB, LOW);
    
    
    lockLocked = true;
    } else {
      Serial.println(F("Currently locked with a different Card."));
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(redLedPin, HIGH);
      delay(100);
      digitalWrite(greenLedPin, LOW);
      digitalWrite(redLedPin, LOW);
      delay(100);
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(redLedPin, HIGH);
      delay(100);
      digitalWrite(greenLedPin, LOW);
      delay(100);
    }
  }
  else { 
    if (lockLocked) {
    Serial.println(F("Locked Bike owner card detected, unlocking..."));
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    //digitalWrite(actuatorTriggerA, LOW);
    //digitalWrite(actuatorTriggerB, LOW);
    //delay(1000);
    //digitalWrite(actuatorTriggerA, HIGH);

    digitalWrite(actuatorTriggerA, HIGH);
    delay(1000);
    //digitalWrite(actuatorTriggerA, LOW);
    digitalWrite(actuatorTriggerA, LOW);
    
    lockLocked = false;
    } else {
         //Lock routine (unnecessary leaving it here because there might be a reason I added it)
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    //digitalWrite(actuatorTriggerA, HIGH);
    digitalWrite(actuatorTriggerB, HIGH);
    delay(1000);
    //digitalWrite(actuatorTriggerA, LOW);
    digitalWrite(actuatorTriggerB, LOW);
    
    
    lockLocked = true;
    }
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
