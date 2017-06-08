// ask_transmitter.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to transmit messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) transmitter with an TX-C1 module

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;
// RH_ASK driver(2000, 2, 4, 5); // ESP8266: do not use pin 11

void setup()
{
    Serial.begin(9600);	  // Debugging only
    if (!driver.init())
         Serial.println("init failed");
}

int serialIn = 0;


void loop()
{
  /* Add 86 from serial Input
 *  clean:  135 , - 86 -> 49 -> Chr 1
 *  sDock:  143 , - 86 -> 57 -> Chr 9
*/
  if (Serial.available()>0){
    serialIn = Serial.read() +86;
  }

  if (serialIn!=0){
   const char *msg = "49";

    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    Serial.print("Sent: "); Serial.println(serialIn);
    Serial.print("*msg: "); Serial.println(*msg);
    
    delay(5000);
    serialIn = 0;
  }

    
}
