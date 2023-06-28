#include <MFRC522.h>

//RFID-Pins
#define RFID_SS_PIN 7
#define RST_PIN 6
MFRC522 rfid(RFID_SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;


// Init array that will store new NUID
char validUUIDs[][9]={"F1A8641B","14AB7B2C","D015371D","D015371D","F1B1141B","B308C132","D028061D","93C04E32","C04B371D","F16A9B1B","93B17232","93800432"};

void initRFID(){
  rfid.PCD_Init();  // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}


void rfidTimerEvent(){
// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));


  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    displayMessage("Falscher Karten-Typ", true);
    return;
  }
  char cardUid[8] = "";
  array_to_string(rfid.uid.uidByte, 4, cardUid);
  Serial.print("ID: ");
  Serial.println(cardUid);

  // Prüfen, ob Karte zugelassen ist
  bool cardValid=false;
  for(uint8_t i=0;i< sizeof(validUUIDs);i++){
    char* uuid=validUUIDs[i];
    if(strcmp(cardUid,uuid)==0){
      cardValid=true;
      break;
    }
  }
    if(cardValid){
      Serial.println("Karte gültig");
      //Karte valide -> Relais für Ventil für BEER_VALVE_OPEN_TIME Sekunden öffnen
      startBier();
    }else{
      displayMessage("Karte ungueltig", true);
      abortBier();
    }
  
 
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[len * 2] = '\0';
}
