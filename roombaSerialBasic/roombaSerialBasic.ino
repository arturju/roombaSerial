/*------------------------------------------------------------------
Basic control for Roomba via serial cmds



Summary of Roomba Open Interface Spec:

4 operating modes: Off, Passive, Safe, and Full
  -Passive(128): Sleeps after 5 min inactive. Other modes drain battery
  -Safe(131):    Allows user control but safety condition reverts it to Passive mode
  -Full(132):    Complete complete, ignores cliff, wheel-drop, charger, etc

-------------------------------------------------------------------*/

#include <SoftwareSerial.h>

#include "inc/roomba.h"

//int rxPin=10;
//int txPin=11;
SoftwareSerial Roomba(10,11); //rxPin, txPin

//---------------------------------------------
void setup() 
{
  Roomba.begin(115200); // changed to 115200
  Serial.begin(19200);
  pinMode(ddPin, OUTPUT);  //pin5

  delay(2000);
  
//  wakeUp ();  
  startPassive();

  // turn-off all LEDs and Display
//  setPowerLED(128,0);
//  setDebrisLED(OFF);
//  setDockLED(OFF);
//  setSpotLED(OFF);
//  setWarningLED(OFF);
//  cleanDigitLED ();

  playSound (1); // start sound
  Serial.println("Playing sound1");

   delay(5000);
   playSound (3);
   Serial.println("Playing sound3");
   
}

void loop() 
{
  int serialIn;
  if (Serial.available()>0){
    serialIn= Serial.read();
    Serial.println(serialIn);    
//    manualCmd();
  }
  
}

