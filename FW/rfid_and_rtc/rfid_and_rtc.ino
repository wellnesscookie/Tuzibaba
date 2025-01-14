#include <MFRC522.h>  // MFRC522 RFID module library.
#include <SPI.h>      // SPI device communication library.
#include <EEPROM.h>   // EEPROM (memory) library.
#include <ThreeWire.h>  
#include <RtcDS1302.h>

#define pinRST 9      // Defines pins for RST, SS conncetions respectively.
#define pinSS 10
#define RX 0
#define TX 1

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

byte readCard[4];     // Array that will hold UID of the RFID card.
int successRead;
byte message[64];

MFRC522 mfrc522(pinSS, pinRST);   // Creates MFRC522 instance.
MFRC522::MIFARE_Key key;          // Creates MIFARE key instance.
// UID white card   33EDD215
void setup()
{  
  pinMode(RX, OUTPUT);
  pinMode(TX, OUTPUT);
  digitalWrite(RX, 0);
  digitalWrite(TX, 0);
  Serial.begin(1000000);     // Starts the serial connection at 9600 baud rate.
  SPI.begin();            // Initiates SPI connection between RFID module and Arduino.
  mfrc522.PCD_Init();    // Initiates MFRC522 RFID module.

  Serial.println("Please scan your RFID card to the reader.");

  do {
    successRead = getID();     // Loops getID library function until reading process is done.
  }

  while (!successRead);
  for ( int i = 0; i < mfrc522.uid.size; i++ )  // You can add multiple cards to read in the for loop.
  {
    EEPROM.write(i, readCard[i]);     // Saves RFID cards UID to EEPROM.
  }
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);
}

void loop()
{
  getID(); 
}

int getID() // Function that will read and print the RFID cards UID.
{
  if ( ! mfrc522.PICC_IsNewCardPresent())  // If statement that looks for new cards.
  {
    return 0;
  }

  if ( ! mfrc522.PICC_ReadCardSerial())    // If statement that selects one of the cards.
  {
    return 0;
  }
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  Serial.print("UID: ");    
  for (int i = 0; i < mfrc522.uid.size; i++) {  
    readCard[i] = mfrc522.uid.uidByte[i];   // Reads RFID cards UID.
    Serial.print(readCard[i], HEX);     // Prints RFID cards UID to the serial monitor.
  }
  
  mfrc522.PICC_HaltA();     // Stops the reading process.
  Serial.println();
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u-%02u-%02u_%02u/%02u/%04u"),
            dt.Hour(),
            dt.Minute(),
            dt.Second(),
            dt.Month(),
            dt.Day(),
            dt.Year());
    Serial.print(datestring);
    Serial.println();
}
