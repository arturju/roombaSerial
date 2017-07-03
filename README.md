# Roomba 650 serial communication 
I've had an old Roomba 650 lying for a while and thought it was time to give it an upgrade. At the time of writing this, Roomba's that are phone app compatible cost near $500 or higher. Unwilling to pay this much money for an upgrade, I decided to venture down the road of building something that would let me communicate with it and let me add some of the features the newer models have.

## Parts list and circuit diagram
![schem](/img/schematics.png)
* Arduino Nano
* [RF transmitter/receiver modules](https://www.amazon.com/dp/B01CJDKHIA/ref=cm_sw_r_cp_dp_T1_yFWuzbNCM1KP3)
* [Mini 7 Pin Din connector](https://www.amazon.com/dp/B01CJDKHIA/ref=cm_sw_r_cp_dp_T1_yFWuzbNCM1KP3)
* [Boost converter](https://www.amazon.com/dp/B01GJ0SC2C/ref=cm_sw_r_cp_dp_T1_t5Hnzb928J4AM)

## Software
Fortunately, iRobot provides a good set of instructions on how to communicate with a Roomba. I used this document to write the commands and sequences: [iRobot Create 2 Open interface](https://cdn-shop.adafruit.com/datasheets/create_2_Open_Interface_Spec.pdf). 
The code runs as a state machine with a timer to check if the roomba has gone to sleep. A state is changed whenever it receives a command from the RF module, the arduino receives a serial command or when the roomba goes to sleep.


## References
* [Room_BT_Ctrl](https://github.com/Mjrovai/Roomba_BT_Ctrl)
* [RF Intro](http://arduinobasics.blogspot.com/2014/06/433-mhz-rf-module-with-arduino-tutorial.html)

