#ifndef ROOMBA_H
#define ROOMBA_H

/*-----------------------------------------------------------------------------
 ** Global Defines/Typedefs/Enums/Macros.
 **---------------------------------------------------------------------------*/
#define ASLEEP    1
#define CLEANING  2
#define DOCKING   3
 
#define START_OI  128
#define RESET_OI  7
#define STOP_OI   173
#define BAUD      129
#define SAFE_MODE 131
#define FULL_MODE 132
#define CLEAN     135
#define SEEK_DOCK 143 
 
extern SoftwareSerial Roomba;

typedef struct miniDin7 {
	int pin1Vpwr;
	int pin2Vpwr;
	int pin3Rxd;
	int pin4Txd;
	int pin5Brc;
	int pin6Gnd;
	int pin7Gnd;
}din7Connector;

typedef struct roombaInfo {
	int roombaState;
	void (*roombaUpdateFunc)(int* , din7Connector, int);	
}roombaObj;



/*-----------------------------------------------------------------------------
 ** Function: IsTime()
 **
 ** Arguments:
 ** *timeMark   : pointer to value of timeMark variable. Gets compared with millis()
 ** timeInterval: time that must elapse before function returns true
 **---------------------------------------------------------------------------*/
boolean IsTime(unsigned long *timeMark, unsigned long timeInterval) {
    if (millis() - *timeMark >= timeInterval) {
        *timeMark = millis();
        return true;
    }
    return false;
}


/*-----------------------------------------------------------------------------
 ** Functions: 
 ** wake up and general cmd functions including wait-time
 **---------------------------------------------------------------------------*/
void wakeUp (din7Connector miniDin)
{
  digitalWrite(miniDin.pin5Brc, HIGH);
  delay(100);
  digitalWrite(miniDin.pin5Brc, LOW);
  delay(500);
  digitalWrite(miniDin.pin5Brc, HIGH);
  delay(1000);
}

void cmdRoomba(int command){
	Roomba.write(command);
	delay(1000);
}

/*-----------------------------------------------------------------------------
 ** Function: 
 ** Updates roomba states if currently in sleep mode
 **---------------------------------------------------------------------------*/
void roombaCmdFromSleep(int* roombaState, din7Connector roombaDin, int cmdOpCode){
	Serial.println("Waking up roomba...Starting IO");
	wakeUp(roombaDin);	
	cmdRoomba(START_OI);
	if(cmdOpCode == CLEAN)		*roombaState = CLEANING;
	if(cmdOpCode == SEEK_DOCK)	*roombaState = DOCKING;
	cmdRoomba(cmdOpCode);	
}

/*-----------------------------------------------------------------------------
 ** Function: 
 ** Updates roomba states if currently awake and Cleaning
 **---------------------------------------------------------------------------*/
void roombaCmdFromClean(int* roombaState, din7Connector roombaDin, int cmdOpCode){
	Serial.println("Received cmd while cleaning...");
	if(cmdOpCode == CLEAN)		Serial.println("Already cleaning...");
	if(cmdOpCode == SEEK_DOCK)	{
		Serial.println("Stopping cleaning to dock...");
		cmdRoomba(CLEAN);			// stops cleaning		
		cmdRoomba(cmdOpCode);
		*roombaState = DOCKING; 
	};
}

/*-----------------------------------------------------------------------------
 ** Function: 
 ** Updates roomba states if currently Seeking Dock
 **---------------------------------------------------------------------------*/
void roombaCmdFromSeek(int* roombaState, din7Connector roombaDin, int cmdOpCode){
	Serial.println("Received cmd while docking...");
	if(cmdOpCode == SEEK_DOCK)		Serial.println("Already seeking dock...");
	if(cmdOpCode == CLEAN)			{
		Serial.println("Stopping docking to clean...");
		cmdRoomba(CLEAN);			// stops seeking dock		
		cmdRoomba(cmdOpCode);
		*roombaState = CLEANING; 
	};
}
 
/*----------------------------------------------------------------------------
 ** Function: 
 ** This command starts the default cleaning mode. Same as pressing Roomba's Clean button
 **---------------------------------------------------------------------------*/
void clean()
{
	Roomba.write(135);
	delay(1000);
}

/*-----------------------------------------------------------------------------
 ** Function: 
 ** Same as pressing Roomba's Dock button. Will pause cleaning cycle if in progress
 **---------------------------------------------------------------------------*/
void seekDock()
{
	Roomba.write(143);
	delay(1000);
}

/********************************************

Functions below are not used. Maybe for later update?

*********************************************/

/*-----------------------------------------------------------------------------
 ** Function: 
 ** Enters this mode after sending Start, Spot, Clean, or Seek Dock command. Can request and receive sensor data but can't change parameters. Goes into power saving mode after 5 mins of inactivity. 
 **---------------------------------------------------------------------------*/
void startPassive()
{  
  Roomba.write(128);  //Start OI
  delay(1000);
}

/*-----------------------------------------------------------------------------
 ** Function: 
 ** This command gives you control over Roomba turning on the cliff, wheel-drop and internal charger safety features
 **---------------------------------------------------------------------------*/
void startSafe()
{  
  Roomba.write(128);  //Starts the OI, must always send before sending OI cmds
  Roomba.write(131);  //Safe mode
  delay(1000);
}

/*-----------------------------------------------------
Song (pg 19)
- Available in modes: Passive, Safe, or Full
- Serial sequence: [140] [song #] [length] [note#] [note duration]
- Song number is 0-4; if second song command is sent, the old one is overwritten
- Song length 1-16; number of musical notes in the song
- Song bytes, Note Number   31-127; everything else is a rest note
- Song bytes, Note Duration 0-255 ; in incrementes of 1/64th of second. .5s = 32
                              
							  
Play (pg20)
- Available in modes: Safe or Full	
- [141] [song #] 
------------------------------------------------*/
void playSound (int num) 
{
  switch (num)
  { 
    case 1: 
		// [140] [1] [4] [68] [32] ... place "start sound" in slot 1
      Roomba.write("\x8c\x01\x04\x42\x20\x3e\x20\x42\x20\x3e\x20"); 
      Roomba.write("\x8d\x01"); // [141] [1] play it (in slot 1)
      break;
 
    case 2: 
      Roomba.write("\x8c\x01\x01\x3c\x20"); // place "low freq sound" in slot 2
      Roomba.write("\x8d\x01"); // play it (in slot 2)
      break;

    case 3:
      Roomba.write("\x8c\x01\x01\x48\x20"); // place "high freq sound" in slot 3
      Roomba.write("\x8d\x01"); // play it (in slot 3)
      break;
  }
}


/*--------------------------------- LED controls
- Available in Safe or Full Mode
- Serial sequence: [139] [LED Bits] [Power Color] [Power Intensity]
- LED Bits:		
- Power Color: 	 0 = green, 255 = red
- Power Intensity: 0= off, 	255 = full intensity
---------------------------------*/

/*  
 This command controls the Power LED by two data bytes: color and intensity.
 color: Green = 0; 128 = Orange; red=255 
 intensity low=0; max=255 */



/*-----------------------------------------------------
Roomba Open Interface Sensor Packets
- 58 sensor data packets. 

-PacketID: 22: Voltage; 	dataBytes: 2,unsigned. range: 0 - 65535 mV
-PacketID: 23: Current; 	dataBytes: 2   signed. range: -32768 to 32767 mA
-PacketID: 24: Temperature; dataBytes: 1   signed. range: -128 to 127
-PacketID: 25: BattCharge;  dataBytes: 2,unsigned. range: 0-65535 mAh
-PacketID: 25: BattCapacty; dataBytes: 2,unsigned. range: 0-65535 mAh

------------------------------------------------------*/

#endif
