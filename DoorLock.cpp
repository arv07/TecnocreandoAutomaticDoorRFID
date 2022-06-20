#include <Arduino.h>
#include <Servo.h>


Servo ServoMotorDoorLock; //open door lock
Servo ServoMotorSpringDeadBolt; //spring door lock
Servo ServoMotorSlideDeadBolt; //push/hall door look

#define pinDoorLockServo 15 //D8 //open door lock
#define pinSpringDoorLockServo 13 //D7 //spring door 
#define pinSlideDeadBolt 0 //push/hall door look
#define pinButton 16 //To lock door from button

int stateButton = 0;
int angle = 0; //angle for servo motors

void setupDoorLock() {
  pinMode(pinButton, OUTPUT);
}


void openDoor(String validation)
{
  
  if(validation == "Enroll Succesed")
  {
    
    //Serial.println(validation);
    Serial.println("5");//Green Led on Auxiliar
    ServoMotorDoorLock.attach(pinDoorLockServo);
    ServoMotorDoorLock.write(90);
    delay(1000); 
    ServoMotorDoorLock.write(40);
    delay(500);
    ServoMotorDoorLock.detach();  
 
  }
  else if(validation == "Card does not exist")
  {
    //Serial.println(validation);
    Serial.println("6");//Red Led on Auxiliar
  }
  else
  {
    //s.write("3");
    //Serial.println("6");
  }
 
}


void secureDoor(boolean lock)
{
  ServoMotorSpringDeadBolt.attach(pinSpringDoorLockServo);
  ServoMotorSpringDeadBolt.write(30);
  delay(500);

  /*Move the deadbolt to lock the door*/
  if(lock == true)
  {
    ServoMotorSlideDeadBolt.attach(pinSlideDeadBolt);
    
    for (angle = 80; angle >= 0; angle -= 1) 
    {
      ServoMotorSlideDeadBolt.write(angle); // tell servo to go to position in variable 'pos'
      delay(5); // waits 20ms between servo commands
    }
    
    ServoMotorSlideDeadBolt.detach();
    
  }

  /*Move the deadbolt to unlock the door*/
  else
  {
    ServoMotorSlideDeadBolt.attach(pinSlideDeadBolt);
   
    for (angle = 0; angle < 80; angle += 1) 
    {
      ServoMotorSlideDeadBolt.write(angle); // tell servo to go to position in variable 'pos'
      delay(5); // waits 20ms between servo commands
    }
    ServoMotorSlideDeadBolt.detach();
    
  }
  
  /*Return spring deadbolt to normal position*/
  delay(300);
  for (angle = 30; angle >= 0; angle -= 1) 
  {
    ServoMotorSpringDeadBolt.write(angle); // tell servo to go to position in variable 'pos'
    delay(20); // waits 20ms between servo commands
  }

  ServoMotorSpringDeadBolt.detach();
  
}


boolean getStateLockButton() {

  stateButton = digitalRead(pinButton);
  boolean state = false;
  if(stateButton == HIGH)
  {
    state = true;
  }
  else if(stateButton == LOW)
  {
    state = false;
  }
  else {
    state = false;
  }

  return state;
  state = false;
}
