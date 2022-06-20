#include <Arduino.h>
#define magneticSensorDoor A0 //To indicate wheater is open or closed
#define magneticSensorDeadbolt 14 //To indicate wheater deadbolt is open or closed


int stateDoor = 0;
int oldStateDoor = 0;
int newStateDoor = 0;

int stateDeadbolt = 0;
int oldStateDeadbolt = 0;
int newStateDeadbolt = 0;



void setupMagneticSensor() {
  /*Settings for magnetic sensor*/
  pinMode(magneticSensorDoor, INPUT);
  pinMode(magneticSensorDeadbolt, INPUT);
}

boolean getStateDoor(){
  boolean state = false;
  oldStateDoor = stateDoor; 
  stateDoor = analogRead(magneticSensorDoor);
  newStateDoor = stateDoor;
  
  if(stateDoor <=60)
  {
    state = true;
  }
  else
  {
    state = false;
  }

  return state;
  
}




boolean getStateDeadbolt(){
  
  boolean state = false;
  oldStateDeadbolt = stateDeadbolt;
  stateDeadbolt = digitalRead(magneticSensorDeadbolt);
  newStateDeadbolt = stateDeadbolt;
  
  if(stateDeadbolt == 1 )
  {
    state = true;
  }
  else
  {
    state = false;
  }
  
  return state;
}
