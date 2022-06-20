/*Library for sensor PN532 sensor with I2C communication*/
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);
//#define PN532_IRQ   (2)
//#define PN532_RESET (3)
//#include <Adafruit_PN532.h>
//Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#define WHITE_LED 3
#define GREEN_LED 5
#define RED_LED 4

int button = 4;

int str_len = 0;

String card = "";

int stateButton = 0;


void setup() {

   /*----------------Settings for NFC Sensor---------------------*/
  /*Start NFC*/
 
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
 
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
 
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  //This value could stuck loop block code
  nfc.setPassiveActivationRetries(0x05);


  // configure board to read RFID tags
  nfc.SAMConfig();
  /*----------------End Settings for NFC Sensor---------------------*/
 
  Serial.begin(9600);
  pinMode(WHITE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
}


void readCard()
{
  uint8_t my_str[8];
  byte ActualUID[4];
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
 
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);


 
  if (success) {
    //Serial.println("Found a card!");
    //digitalWrite(led, HIGH);
    /*Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");*/
    //openDoor();
    digitalWrite(WHITE_LED, HIGH);
    //Authenticating
    //s.write("1");//Send data to Arduino to show in screen
   
    delay(1000);
    for (uint8_t i=0; i < uidLength; i++)
    {
      //Serial.print(" 0x");Serial.print(uid[i], HEX);
      my_str[i] = uid[i];
      card += my_str[i];
    }

    //Send data to NodeMCU
    Serial.println(card);
    /*url_t2 = "{\"id_card\":\""+card+"\",\"token\":\""+token+"\"}";
 
    Serial.println("\n url_tr2" + url_t2);
    str_len = url_t2.length() + 1 ;
    Serial.println("len: "+ String(str_len));
 
    char url_t[str_len];
    url_t2.toCharArray(url_t, str_len);
    Serial.println("url_tr2 after" + url_t2);
    Serial.println(url_t);*/

    //Blynk.virtualWrite(V4, 5);

   
      /*---------------EMIT DATA-----------------------*/
      String str((char*)my_str);
      //webSocket.emit("rfid:enrollEntry", "{\"id_card\":\"0xF7 0x10 0x1E 0xD\",\"token\":\"614f35b7afefaf58e070\"}");
      //webSocket.emit("rfid:enrollEntry", url_t);
      //Blynk.virtualWrite(V4, card);
      card = "";
      //digitalWrite(led, LOW);
     
     

     
   
   
   
   
    //Serial.println(str, HEX);
    //submitData(str);
    //openDoor();
    // Wait 1 second before continuing
    //delay(1000);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    //Serial.println("Timed out waiting for a card");
  }

  digitalWrite(WHITE_LED, LOW);
  
}





void loop() {

   int opt = 0;  
 
  if(Serial.available() > 0)
  {
    String opcion = "";
   
    //opcion = String(Serial.readString());
    opcion = Serial.readStringUntil('\n');
    opt = opcion.toInt();
    //opt = opt+"2";
    //Serial.println(opcion.length());
    //Serial.println(opt);

     
    str_len = opcion.length() + 1;
    //Serial.println(str_len);
 
    char url_t[str_len];
    opcion.toCharArray(url_t, str_len);
   
    //Serial.write(url_t);
    if(opt == 5)//Card was validated successful
    {  
        digitalWrite(GREEN_LED, HIGH);
        delay(500);
        digitalWrite(GREEN_LED, LOW);
        opt = 0;
       
    }
    else if(opt == 6)//Card is invalid
    {
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
      opt = 0;
    }
    else if(opt == 8)//
    {
      digitalWrite(GREEN_LED, HIGH);
      opt = 0;
    }
    else if(opt == 7)
    {
      digitalWrite(GREEN_LED, LOW);
      opt = 0;
    }
    else if(opt == 9)//Conrim card was add
    {
      digitalWrite(GREEN_LED, HIGH);
      delay(500);
      digitalWrite(GREEN_LED, LOW);
      delay(500);
      digitalWrite(GREEN_LED, HIGH);
      delay(500);
      digitalWrite(GREEN_LED, LOW);
      opt = 0;
    }
    else if(opt == 10)//Conrim card wasn't add
    {
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
      delay(500);
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
      opt = 0;
    }
    else if(opt == 11)//Button lock pressed, waiting for card to validate and lock
    {
      digitalWrite(RED_LED, HIGH);
      opt = 0;
    }
    else if(opt == 12)//TURN OFF waiting for card to validate and lock
    {
      digitalWrite(RED_LED, LOW);
      opt = 0;
    }
  else
  {
    /*Serial.println(opcion);
    Serial.println("Invalido");*/
  }
 
}

readCard();

//stateButton = digitalRead(button);

//Serial.println(String(stateButton));
//delay(2000);


}