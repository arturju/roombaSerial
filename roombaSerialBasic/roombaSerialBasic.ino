/*------------------------------------------------------------------
Summary of Roomba Open Interface Spec:

4 operating modes: Off, Passive, Safe, and Full
  -Passive(128): Sleeps after 5 min inactive. Other modes drain battery
  -Safe(131):    Allows user control but safety condition reverts it to Passive mode
  -Full(132):    Complete complete, ignores cliff, wheel-drop, charger, etc
-------------------------------------------------------------------*/
#include <SoftwareSerial.h>
#define START_OI  128
#define RESET_OI  7
#define STOP_OI   173
#define SAFE_MODE 131
#define FULL_MODE 132
#define CLEAN     135
#define SEEK_DOCK 143

#include "inc/roomba.h"
SoftwareSerial Roomba(10,11); //rxPin, txPin

#define ASLEEP    1
#define AWAKE     2
#define MOVING    3

int roombaState = ASLEEP;
int serialIn = 0;

unsigned long int roombaIOtimeout = 240000; // 4 min
unsigned long     roombaTimeMark  = 0;
  
//---------------------------------------------
void setup() 
{
  Roomba.begin(115200); // changed to 115200
  Serial.begin(19200);
  pinMode(ddPin, OUTPUT);  //pin5

  Serial.println("Setup done");
  roombaTimeMark = millis();
}

void loop() 
{
/* Add 86 from serial Input
 *  clean:  135 -> 49. Chr 1
 *  sDock:  143 -> 57. Chr 9
*/
if (Serial.available()>0)   serialIn= Serial.read()+86;  

if(serialIn != 0){
  Serial.print("Cmd: "); Serial.println(serialIn);   
  
  if      (roombaState == MOVING && (serialIn== 135 ||serialIn==143) ){
    cmdRoomba(CLEAN);       //stop current cmd  first...
  }
  else if (roombaState == MOVING){
    Serial.println("Uknown cmd while moving...");
    cmdRoomba(CLEAN);       // stop moving
    roombaState = AWAKE;
  }
  
  else if (roombaState == AWAKE && (serialIn== 135 ||serialIn==143)){
    roombaState = MOVING;
  }
  else if (roombaState == AWAKE){
    Serial.println("Uknown command while awake...");
  }
  
  else if (roombaState == ASLEEP && (serialIn== 135 ||serialIn==143)){
    Serial.println("Waking up"); wakeUp();
    roombaState = MOVING;
  }
  else if (roombaState == ASLEEP){
    wakeUp();
    Serial.println("Unkown cmd while asleep...");
    roombaState = AWAKE;
  }

  delay(500);
  cmdRoomba(serialIn);      // issue command received
  roombaTimeMark= millis(); // reset timers

}// End if(serialIn)


// Update Timers:
  if(IsTime(&roombaTimeMark, roombaIOtimeout)) {
    Serial.println("Roomba went to sleep!");  roombaState=ASLEEP;
  }

serialIn = 0; 
}

