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


#endif