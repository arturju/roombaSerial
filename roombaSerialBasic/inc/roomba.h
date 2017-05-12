#ifndef ROOMBA_H
#define ROOMBA_H

/*-----------------------------------------------------------------------------
 ** Global Defines/Typedefs/Enums/Macros.
 **---------------------------------------------------------------------------*/
#define clamp(value, min, max) (value < min ? min : value > max ? max : value)

#define 	ON 			1
#define 	OFF 		0
#define 	MAX_SPEED 	220 // 70% of top speed ==> 256
#define 	MIN_SPEED 	70 


// variables for Roomba
extern SoftwareSerial Roomba;

int ddPin = 5; //device detect


// int buttonPin = 12;
// int ledPin=13;

//variables for LEDs
bool debrisLED;
bool spotLED;
bool dockLED;
bool warningLED;
byte color;
byte intensity;

char digit1;
char digit2;
char digit3;
char digit4;

//blocking sensor functions - these will request data and wait until a response is recieved, then return the response

int getSensorData(byte sensorID);
int * getSensorData(byte numOfRequests, byte requestIDs[]);

bool getSensorData(byte * buffer, byte bufferLength);

byte single_byte_packets[22] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 21, 24, 34, 35, 36, 37, 38, 45, 52, 53, 58};
bool is_in_array(byte val);



/** LED controls --------------------
- available in Safe or Full Mode


*/

/*---------------------------------------------------------------
Serial sequence: [139] [LED Bits] [Power Color] [Power Intensity]
LED Bits:		
Power Color: 	 0 = green, 255 = red
Power Intensity: 0= off, 	255 = full intensity
*/


/*-----------------------------------------------------------------------------
 ** Function: 
 ** Wakes up the robot
 **---------------------------------------------------------------------------*/
void wakeUp (void)
{
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
  delay(2000);
}


/*-----------------------------------------------------------------------------
 ** Function: 
 ** This command gives you control over Roomba turning on the cliff, wheel-drop and internal charger safety features
 **---------------------------------------------------------------------------*/
void startSafe()
{  
  Roomba.write(128);  //Start
  Roomba.write(131);  //Safe mode
  delay(1000);
}


/*-----------------------------------------------------------------------------
 ** Function: 
 ** Enters this mode after sending Start, Spot, Clean, or Seek Dock command. Can request and receive sensor data but can't change parameters. Goes into power saving mode after 5 mins of inactivity. 
 **---------------------------------------------------------------------------*/
void startPassive()
{  
  Roomba.write(128);  //Start OI
  delay(1000);
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






/*  
 This command controls the Power LED by two data bytes: color and intensity.
 color: Green = 0; 128 = Orange; red=255 
 intensity low=0; max=255 */
void setPowerLED(byte setColor, byte setIntensity)
{
  color = setColor;
  intensity = setIntensity;
  Roomba.write(139);
  Roomba.write((byte)0x00);
  Roomba.write((byte)color);
  Roomba.write((byte)intensity);
}

//---------------------------------------------------------------
void setDebrisLED(bool enable)
{
  debrisLED = enable;
  Roomba.write(139);
  Roomba.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Roomba.write((byte)color);
  Roomba.write((byte)intensity);
}

//---------------------------------------------------------------
void setSpotLED(bool enable)
{
  spotLED = enable;
  Roomba.write(139);
  Roomba.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Roomba.write((byte)color);
  Roomba.write((byte)intensity);
}

//---------------------------------------------------------------
void setDockLED(bool enable)
{
  dockLED = enable;
  Roomba.write(139);
  Roomba.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Roomba.write((byte)color);
  Roomba.write((byte)intensity);
}



/*--------------------------------------------------------------------------
This command controls the four 7 segment displays on the Roomba.*/
void setDigitLEDs(byte digit1, byte digit2, byte digit3, byte digit4)
{
    Roomba.write(163);
    Roomba.write(digit1);
    Roomba.write(digit2);
    Roomba.write(digit3);
    Roomba.write(digit4);
}

/*--------------------------------------------------------------------------
This command controls the four 7 segment displays on the Roomba using ASCII character codes.*/
void setDigitLEDFromASCII(byte digit, char letter)
{
  switch (digit){
  case 1:
    digit1 = letter;
    break;
  case 2:
    digit2 = letter;
    break;
  case 3:
    digit3 = letter;
    break;
  case 4:
    digit4 = letter;
    break;
  }
  Roomba.write(164);
  Roomba.write(digit1);
  Roomba.write(digit2);
  Roomba.write(digit3);
  Roomba.write(digit4);
}

//---------------------------------------------------------------
void cleanDigitLED (void)
{
  setDigitLEDFromASCII(1, ' ');
  setDigitLEDFromASCII(2, ' ');
  setDigitLEDFromASCII(3, ' ');
  setDigitLEDFromASCII(4, ' ');
}
//---------------------------------------------------------------
void writeLEDs (char a, char b, char c, char d)
{
  setDigitLEDFromASCII(1, a);
  setDigitLEDFromASCII(2, b);
  setDigitLEDFromASCII(3, c);
  setDigitLEDFromASCII(4, d);
}



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
