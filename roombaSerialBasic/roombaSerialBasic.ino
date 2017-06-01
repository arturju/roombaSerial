/*------------------------------------------------------------------
Summary of Roomba Open Interface Spec:

4 operating modes: Off, Passive, Safe, and Full
  -Passive(128): Sleeps after 5 min inactive. Other modes drain battery
  -Safe(131):    Allows user control but safety condition reverts it to Passive mode
  -Full(132):    Complete complete, ignores cliff, wheel-drop, charger, etc
-------------------------------------------------------------------*/
#include <SoftwareSerial.h>
#include <RH_ASK.h>   // RadioHead library
#include <SPI.h>      // not used, needed to compile

RH_ASK driver;        // pins 11, 12, 10 for Rx,Tx,Ptt 

#define START_OI  128
#define RESET_OI  7
#define STOP_OI   173
#define SAFE_MODE 131
#define FULL_MODE 132
#define CLEAN     135
#define SEEK_DOCK 143

#include "inc/roomba.h"
SoftwareSerial Roomba(6,7); //rxPin, txPin. Nano can 

#define ASLEEP    1
#define AWAKE     2
#define MOVING    3

int roombaState = ASLEEP;
int serialIn = 0;

unsigned long int roombaIOtimeout = 240000; // 4 min
unsigned long     roombaTimeMark  = 0;


// adding stuff for XY-MK-5V 
#define rfReceivePin A0  //RF Receiver pin = Analog pin 0
 #define ledPin 13        //Onboard LED = digital pin 13

 const unsigned int upperThreshold = 70;  //upper threshold value
 const unsigned int lowerThreshold = 50;  //lower threshold value

#define RED_LED   3
#define GREEN_LED 2
int   ledState = 0;
  
//---------------------------------------------
void setup() 
{
  Roomba.begin(115200); // changed to 115200
  Serial.begin(19200);
  pinMode(ddPin, OUTPUT);  //pin5
  pinMode(RED_LED, OUTPUT);   // led
  pinMode(GREEN_LED, OUTPUT);   // led
  digitalWrite(RED_LED, HIGH);
  roombaTimeMark = millis();

  // initialize RF stuff
  if (!driver.init()) {Serial.println("init failed");}
  else                {Serial.println("init success! \n");}
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
}

void loop() 
{
  
// RF chip stuff
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);
char rfMsg[sizeof(buf)+1];

if (driver.recv(buf, &buflen)){
  int i;    //???
  for (int i=0; i<buflen; i++){
    rfMsg[i]= char(buf[i]);
//   Serial.print(i);  Serial.print("="); Serial.print(char(buf[i])); Serial.print(" ");
  }
  rfMsg[5]= 0;   //NUL
  String tmpString(rfMsg);
  
  serialIn = tmpString.toInt()+86;    // use RF msg instead for serialIn
//  Serial.print(serialIn);
//  Serial.println(" ");
  
}



  
/* Add 86 from serial Input
 *  clean:  135 -> 49. Chr 1
 *  sDock:  143 -> 57. Chr 9
*/
//if (Serial.available()>0)   serialIn= Serial.read()+86;  

if(serialIn != 0){
  Serial.print("Cmd: "); Serial.println(serialIn);
  digitalWrite(GREEN_LED, !ledState);
  digitalWrite(RED_LED, ledState);
  ledState = !ledState;
  
  if      (roombaState == MOVING && (serialIn== 135 ||serialIn==143) ){
    cmdRoomba(CLEAN);       //stop current cmd  first...
    Serial.print("Stopping current cmd...");
  }
  else if (roombaState == MOVING){
    Serial.println("Uknown cmd while moving...");
    cmdRoomba(CLEAN);       // stop moving
    roombaState = AWAKE;
  }
  
  else if (roombaState == AWAKE && (serialIn== 135 ||serialIn==143)){
    roombaState = MOVING;
    Serial.print("State is now Moving...");
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
    Serial.println("Roomba went to sleep!");  
    roombaState=ASLEEP;
  }

serialIn = 0; 
}

