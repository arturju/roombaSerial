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

#define IDLING         1
#define DOING_STUFF    2
int roombaState = IDLING;
int serialIn;
  
//---------------------------------------------
void setup() 
{
  Roomba.begin(115200); // changed to 115200
  Serial.begin(19200);
  pinMode(ddPin, OUTPUT);  //pin5

  Serial.println("Setup done");
}

void loop() 
{
/* Add 86 from serial Input
 *  clean:  135 -> 49. Chr 1
 *  sDock:  143 -> 57. Chr 9
 *  startOI:128 -> 42. Chr * 
*/
if (Serial.available()>0)    serialIn= Serial.read()+86;  

if(serialIn != 0){
  Serial.print("Cmd: "); Serial.println(serialIn);   
  
  //If we're doing something, stop current cmd then issue new one
  if (roombaState == DOING_STUFF){
    cmdRoomba(CLEAN);   
    delay(200);
    cmdRoomba(serialIn);
    roombaState = DOING_STUFF;
  }else{
    cmdRoomba(START_OI);  // must start OI again
    delay(500);
    cmdRoomba(serialIn);
    roombaState = DOING_STUFF;
  }

  
}// End if(serialIn)

// timeOut roombaState to IDLE. After 5 min OI stops

serialIn = 0; 
}

