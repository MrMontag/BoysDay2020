/*
   --------------------------------------------------------------------------------------------------------------------
   Example sketch/program showing An Arduino Door Access Control featuring RFID
   --------------------------------------------------------------------------------------------------------------------

 * **Security**
   To keep it simple we are going to use Tag's Unique IDs. It's simple and not hacker proof.

   @license Released into the public domain.

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices

#define RST_PIN     9          // Configurable, see typical pin layout above
#define SS_PIN      10         // Configurable, see typical pin layout above

#define RedLed      7
#define GreenLed    6
#define Buzzer      5
#define Delay       1000

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(Buzzer, OUTPUT);
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  byte detected[mfrc522.uid.size] = {0};
  Serial.print("UID (length ");
  Serial.print(mfrc522.uid.size);
  Serial.print("):");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    detected[i] = mfrc522.uid.uidByte[i];
  } 
  Serial.println();

  byte allowed[] = {0xF4, 0x18, 0x8E, 0xAB};
  
  if (sizeof(allowed) != sizeof(detected)) {
    accessDenied();
    return;
  }

  bool equal = true;
  for (int i=0; i<mfrc522.uid.size; i++) {
    if (allowed[i] != detected[i]) {
      equal = false;
      break;
    }
  }

  if (equal) {
    accessGranted();
  } else {
    accessDenied();
  }
}

void accessDenied() {
  digitalWrite(RedLed, HIGH);
  tone(Buzzer, 1500);
  delay(Delay);
  noTone(Buzzer);
  digitalWrite(RedLed, LOW);
}

void accessGranted() {
  digitalWrite(GreenLed, HIGH);
  tone(Buzzer, 250);
  delay(Delay);
  noTone(Buzzer);
  digitalWrite(GreenLed, LOW);
}
