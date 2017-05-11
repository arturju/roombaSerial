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


extern SoftwareSerial Roomba;

int motorSpeed = 100;

char command = 0; // variable to store command received from IR or BT remote control
char state = 0;

int buttonPin = 12;
int ledPin=13;
int ddPin=5; //device detect

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



/*---------------------------------------------------------------
Serial sequence: [139] [LED Bits] [Power Color] [Power Intensity]
LED Bits:		
Power Color: 	 0 = green, 255 = red
Power Intensity: 0= off, 	255 = full intensity
*/
void setWarningLED(bool enable)
{
  warningLED = enable;
  Roomba.write(139);
  Roomba.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Roomba.write((byte)color);
  Roomba.write((byte)intensity);
}




/*-----------------------------------------------------------------------------
 ** Function: 
 ** Wakes up the robot
 **---------------------------------------------------------------------------*/
void wakeUp (void)
{
  setWarningLED(ON);
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
 This command Play song
 [140] [song #] [lenght] [note#] [note duration]
 [141] [song #]                                      */
void playSound (int num) 
{
  switch (num)
  { 
    case 1: 
      Roomba.write("\x8c\x01\x04\x42\x20\x3e\x20\x42\x20\x3e\x20"); // [140] [1] [4] [68] [32] ... place "start sound" in slot 1
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




#endif
