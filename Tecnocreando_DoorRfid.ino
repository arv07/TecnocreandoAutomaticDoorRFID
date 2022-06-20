
#include "Global.h"
#include "DoorLock.h"
#include "MagneticSensor.h"


//#define BLYNK_PRINT Serial
#include<SoftwareSerial.h> //Included SoftwareSerial Library

#define BLYNK_HEARTBEAT 5
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;


/*WIFI Libraries*/
#include <FS.h>     
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h> //Json
#include <EEPROM.h>  //To write in EEPROM

//Initialize library
WiFiManager wifiManager;



DynamicJsonDocument doc(256); //Json length


//flag for saving data, for Wifi Manager
boolean shouldSaveConfig = false;
String saveParams = "1";



/*To sotre data in EEPROM*/
char auth[32] = "";
char parameterToken[32] = ""; //To receive token in WifiManager app when user connects to NodeMCU server
String token = "";
//char *token = "";


int opt = 0;

//true: enrroll entry, false: enrroll card
boolean flagEventCard = true;

boolean flagWaitingCardToLock = false;
boolean oldStateButtonLock = false;
boolean stateButtonLock = false;


unsigned long previousMillis = 0;//will store last time LED was updated

//constants won't change :
const long interval = 5000;// interval at which to blink (milliseconds)
int flag = 1;


String option = "";

//callback notifying us of the need to save config, apply for configure Wifi
void saveConfigCallback () {
  Serial.println("Should save config...");
  shouldSaveConfig = true;
  saveParams = "2";
  Serial.println(saveParams);
}

//Blynk variables ------------------------------------------>


//WidgetLED ledApp(V1); //Led in the app for magnetic
WidgetLED ledDeadboltState(V0); //Led in the app for dead bolt
WidgetLED ledDoorState(V1);
BlynkTimer timerStateDoor; //Magnetic Time
BlynkTimer timerStateDeadbolt; //DeadBolt Timer
BlynkTimer timerBtnLock; //Button to lock door

BlynkTimer timerResetESP; //Reconnect

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
//char auth[] = "";
//char auth[] = "HQqAu3IAaq8CLivujbKt4MSVX3oKT9X7";
  char ssid[] = "Intranet";
char pass[] = "admin#2020*";

//Blynk variables ------------------------------------------>


void setup() {

  
  setupDoorLock();
  setupMagneticSensor();



  Serial.begin(9600);
  EEPROM.begin(512);
 
  
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // setup custom parameters
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  //Blynk token is long 32
  WiFiManagerParameter tokenUser("token", "token", "", 32);

  //Add parameters
  wifiManager.addParameter(&tokenUser);
  
  
  /*To configure Access Point. A Wifi network will appear and you can connect
  When credentials are stored in ESP this connect automatically, otherwise start
  configuration portal*/
  if(!wifiManager.autoConnect("AutoConnectAP", "123456"))
  {
    Serial.println("failed to connect and hit timeout");    
  }
  else
  {
    Serial.println("Local IP Assigned");
    Serial.println(WiFi.localIP());  
  }
  
  //Read updated parameters
  strcpy(parameterToken,tokenUser.getValue());
  Serial.println(saveParams);
  Serial.println(strlen(parameterToken));
  Serial.println(parameterToken);
  Serial.println(tokenUser.getValue());
  Serial.println(shouldSaveConfig);

  //Save token in EEPROM
  if(shouldSaveConfig == true)
  {
    for(int x = 0; x <= strlen(parameterToken); x++)
    {
      Serial.print(parameterToken[x]);
      Serial.print(x);
      EEPROM.write(x, parameterToken[x]);
    }
      EEPROM.commit();
      Serial.println("Token saved");
      Serial.println(strlen(parameterToken));
      Serial.println(parameterToken);
      Serial.println(tokenUser.getValue());
  }

  /*------------------End Settingd for WIFi-------------------------*/


  //Load token in variable
  char tok [32] = "";
  //char auth[32] = "";

  /*Read data from EEPROM*/
  //32 token long of Blynk
  for(int x = 0; x <= 32; x++)
  {
    auth[x] = char(EEPROM.read(x));
  }

  token = String(auth);
  Serial.println(auth);

  //Blynk connections
  Blynk.begin("HQqAu3IAaq8CLivujbKt4MSVX3oKT9X7", ssid, pass);
  //Blynk.begin(auth, ssid, pass);

  //Blynk Timers
  timerStateDoor.setInterval(1000L, getStateDoorEvent);
  timerStateDeadbolt.setInterval(1000L, getStateDeadboltEvent);
  timerBtnLock.setInterval(1000L, secureFromButtonEvent);
  
  timerResetESP.setInterval(1000000L, resetESP);
  
}

//This methods are executed from Blynk timer ------------------------------------------>


void resetESP()
{
  //ESP.reset();
}

void getStateDoorEvent() {
  
  boolean state = getStateDoor();
  
  if(state == true) {
    ledDoorState.on();
  }
  else if(state == false) {
    ledDoorState.off();
  }
  
}

void getStateDeadboltEvent() {
  
  boolean state = getStateDeadbolt();
  
  if(state == true) {
    ledDeadboltState.on();
  }
  else if(state == false) {
    ledDeadboltState.off();
  }
}

void secureFromButtonEvent()
{
  boolean state = false;
  state = getStateLockButton();
  if(state)
  {
    Serial.println("11");
    flagWaitingCardToLock = true;
 
  }

 
 
  /*Authentication for secure door*/
  
}

//This methods are executed from Blynk timer ------------------------------------------>


//Blynk methods to wirte from app to hardware ---------------------------------------->

//Open door
BLYNK_WRITE(V7)
{
  int pinData = param.asInt();
  String validation = "Enroll Succesed";

  if(pinData == 1)
  {
    
    //lock = true;
    openDoor(validation);
  }
  else
  {
    //Serial.println("End V7");
  }
  
}



//Lock door
BLYNK_WRITE(V2)
{
  int pinData = param.asInt();
  //Serial1.println("V2: " + String(pinData));
  
  if(pinData == 1)
  {
    lock = true;
    secureDoor(lock);
  }
  else
  {
    //Serial1.println("End V2");
  }
  
}

//Unlock door
BLYNK_WRITE(V3)
{
  int pinData = param.asInt();
  //Serial1.println("V3: " + String(pinData));

  if(pinData == 1)
  {
    lock = false;
    secureDoor(lock);
  }
  else
  {
    //Serial1.println("End V3");
  }
 
}

//To receive the response from API Enroll Entry
BLYNK_WRITE(V4)
{
  String webhookdata = param.asStr();
  deserializeJson(doc, webhookdata);

  if(flagWaitingCardToLock == true and doc["message"] == "Enroll Succesed")
  {
    secureDoor(true);
    Serial.println("12");//Turn off Led waiting card
  }
  else if(flagWaitingCardToLock == true and doc["message"] == "Card does not exist")
  {
    Serial.println("10");
    
  }
  else if(flagWaitingCardToLock == false and doc["message"] == "Enroll Succesed")
  {
    Serial.println("5");//Turn on Green led on Arduino
    secureDoor(false);
    openDoor(doc["message"]);  
  }
  else {
    openDoor(doc["message"]);
  }
  
  
  //Serial.println(webhookdata);
  //Serial.printf(doc[0]);
  flagWaitingCardToLock = false;
}


//To change flag card
BLYNK_WRITE(V5)
{
  int pinData = param.asInt();

  if(pinData == 1)
  {
    //To Enroll Card
    flagEventCard = false;  
    Serial.println("8");//Turn on Led on Arudino Auxiliar
  }
  else
  {
    //To Enroll Entry
    flagEventCard = true;
    Serial.println("7");//Turn off Led on Arudino Auxiliar
    
  }

}

//To receive the response from API Enroll Card
BLYNK_WRITE(V6)
{
  String webhookdata = param.asStr();

  deserializeJson(doc, webhookdata);
  //openDoor(doc["message"]);

  if(doc["message"] == "card enroll")
  {
     Serial.println("9");//Turn off Led on Arudino Auxiliar
     Blynk.virtualWrite(V5, 0);//Turn off button on App
     flagEventCard = true;
  }
  else
  {
    Serial.println("10");//Turn off Led on Arudino Auxiliar
  }
 
  Serial.println(webhookdata);
  //Serial.printf(doc[0]);
}



void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timerStateDeadbolt.run();
  timerStateDoor.run();
  timerBtnLock.run();
  
  
  if(Serial.available() > 0)
    {
      

      option = Serial.readStringUntil('\n');
      //Blynk.virtualWrite(V4, option);
     
      opt = option.toInt();
     
      //int opcion =  s.read() -;
      //Serial.println(comingByte);
      //s.write("9");
      //s.write(opcion);
      //s.println(opcion);
      //s.println(opt);
      //Serial.println("-" + option);
      if(flagEventCard == true)
      {
        Blynk.virtualWrite(V4, option);  
      }
      else if(flagEventCard == false)
      {
        Blynk.virtualWrite(V6, option);  
      }
      
      /*if(opcion == 2471630217){
        lock = false;
        secureDoor();  
      }
      else{
        //s.write(opt);
      }*/
    }

  if(flagWaitingCardToLock == true)
  {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= interval and flag >= 1 and flag <=6 ) {
  
      previousMillis = currentMillis;
      flag = flag + 1;

      if(flag == 6)
      {
        Serial.println("12");//Turn off Led waiting card
        flagWaitingCardToLock = false;
        flag = 1;  
      }
      /*Serial.println("12");//Turn off Led waiting card
      flagWaitingCardToLock = false;  */
      //Serial.println("C:" + String(flag));
           
    }
  }
  
  
    
}
