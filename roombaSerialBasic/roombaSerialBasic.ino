/*------------------------------------------------------------------
  Roomba Control via BT/Android
  ==> Main commands according IRobot Open Interface doc:  http://www.irobotweb.com/~/media/MainSite/PDFs/About/STEM/Create/iRobot_Roomba_600_Open_Interface_Spec.pdf?la=enBasic 
  ==> Using Android app: https://play.google.com/store/apps/details?id=appinventor.ai_mjrovai.MJRoBot_II_BT_Command_V2&hl=en
  ==> Several commands based on Create2 library developed by Dom Amato: https://github.com/brinnLabs/Create2
  Marcelo Jose Rovai - 30 June, 2016 - Visit: http://mjrobot.org
-------------------------------------------------------------------*/

#include <SoftwareSerial.h>

#include "roomba.h"
//#include "roombaDefines.h"
#include "sensors.h"

// Roomba Create2 connection
int rxPin=10;
int txPin=11;
SoftwareSerial Roomba(rxPin,txPin);


//---------------------------------------------
void setup() 
{
  Roomba.begin(115200); // change to 115200 ?
  Serial.begin(19200);
  
  pinMode(ddPin, OUTPUT);	//pin5
  pinMode(ledPin, OUTPUT); //spare if needed for test purpose
  pinMode(buttonPin, INPUT_PULLUP); //used for "start" for test purposes

  delay(2000);
  Serial.println("Roomba Remote Control");
  
  wakeUp ();   // Wake-up Roomba
  startSafe(); // Start Roomba in Safe Mode

  // turn-off all LEDs and Display
  setPowerLED(128,0);
  setDebrisLED(OFF);
  setDockLED(OFF);
  setSpotLED(OFF);
  setWarningLED(OFF);
  cleanDigitLED ();

  playSound (1); // start sound
  Serial.println("Playing sound1");
/*   while (digitalRead(buttonPin))
  {  
    setDebrisLED(ON);
    writeLEDs ('-', '-', '-', '-');
  }
  setDebrisLED(OFF);
  writeLEDs ('s', 't', 'o', 'p');
   */
   delay(5000);
   playSound (3);
   Serial.println("Playing sound3");

  //motorSquareTest(); // un-comment if you want to test Roomba doing a square 
  //turnCW (40, 180);  //un-comment if you want to test Roomba spin clock-wise 180 degrees and stop
  //driveWheels(20, -20); //un-comment if you want to test Roomba spin
  //driveLeft(20); //un-comment if you want to test Roomba turning left 

}

void loop() 
{
  int serialIn;
  if (Serial.available()>0){
    serialIn= Serial.read();
    command = serialIn;
    Serial.println(serialIn);    
//    manualCmd();
  }
  
}

