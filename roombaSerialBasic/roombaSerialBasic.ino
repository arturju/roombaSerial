/*------------------------------------------------------------------
Summary of Roomba Open Interface Spec:

3 operating modes: Passive, Safe, and Full
  -Passive(128): Sleeps after 5 min inactive. Other modes drain battery
  -Safe(131):    Allows user control but safety condition reverts it to Passive mode
  -Full(132):    Complete complete, ignores cliff, wheel-drop, charger, etc
-------------------------------------------------------------------*/
#include <SoftwareSerial.h>   // for mini din-7 connector
#include <RH_ASK.h>           // RadioHead library
#include <SPI.h>              // not used, needed to compile RH_ASK.h
#include "inc/roomba.h"     

/*-----------------------------------------------------------------------------
 ** Global Defines/Typedefs/Enums/Macros.
 **---------------------------------------------------------------------------*/
din7Connector roombaDin =  {.pin1Vpwr =0, .pin2Vpwr=0, .pin3Rxd= 6, .pin4Txd= 7, .pin5Brc = 5, .pin6Gnd=0, .pin7Gnd=0 };
roombaObj     artuRoomba=  {.roombaState = ASLEEP, .roombaUpdateFunc = roombaCmdFromSleep};

// Roomba OI Commands. Each command starts with one byte opcode
#define START_OI  128
#define RESET_OI  7
#define STOP_OI   173
#define BAUD      129
#define SAFE_MODE 131
#define FULL_MODE 132
#define CLEAN     135
#define SEEK_DOCK 143

/*------------------- Initialize variables ----------------------*/
RH_ASK         driver;        // pins 11, 12, 10 for Rx,Tx,Ptt 
SoftwareSerial Roomba(roombaDin.pin3Rxd, roombaDin.pin4Txd);   //rxPin, txPin; 6,7

unsigned long int roombaIOtimeout = 240000;   // 4 min
unsigned long     roombaTimeMark  = 0;
int               ledState        = 0;
int               roombaCmd       = 0;

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);
char rfMsg[sizeof(buf)+1];              // + EOL
  
//---------------------Setup()---------------------------------
void setup() 
{
  Roomba.begin(19200);                      // changed to 115200, could be 19200
  Serial.begin(19200);                      // for debugging with serial monitor
  pinMode(roombaDin.pin5Brc, OUTPUT);       // Roomba's BRC pin. Used to wake it up
  pinMode(13,OUTPUT);                       // use built in led for feedback
  roombaTimeMark = millis();                // start timer

  // RadioHead initialization...
  if (!driver.init()) {Serial.println("init failed");}
  else                {Serial.println("init success! \n");}
}


//--------------------Main Loop-------------
void loop() {
  
/*------------ Receive commands from RF chip------------*/
if (driver.recv(buf, &buflen)){
  for (int i=0; i<buflen; i++){    rfMsg[i]= char(buf[i]);  }
  rfMsg[5]= 0;                     // add NUL  
  String tmpString(rfMsg);
  roombaCmd = tmpString.toInt();
}

/*------------Do command ------------*/
if(roombaCmd != 0){
  Serial.print("--Received: "); Serial.println(roombaCmd);
  digitalWrite(13, ledState);
  switch(artuRoomba.roombaState){    
    case ASLEEP:
        artuRoomba.roombaUpdateFunc= &roombaCmdFromSleep;
        break;
     case CLEANING:
        artuRoomba.roombaUpdateFunc = &roombaCmdFromClean;
        break;
     case DOCKING:
        artuRoomba.roombaUpdateFunc = &roombaCmdFromSeek;
        break;
  }
  artuRoomba.roombaUpdateFunc(&artuRoomba.roombaState, roombaDin, roombaCmd);
  roombaTimeMark= millis();           // reset sleep timers
  
}// End if(roombaCmd)


/*-----------Update timers and other states ------------*/
if(IsTime(&roombaTimeMark, roombaIOtimeout) && artuRoomba.roombaState != ASLEEP  ) {
  Serial.println("Roomba went to sleep!");  
  artuRoomba.roombaState=ASLEEP;
}
roombaCmd = 0; 
ledState = !ledState;  

}

