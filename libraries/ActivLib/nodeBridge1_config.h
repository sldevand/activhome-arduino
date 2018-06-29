/******INCLUDES********/

#include "RF24Sensors.h"

#include "domusbox_defines.h"

/****PINOUT SETTING****/
const byte txCC1101Pin = 2;
const byte rxCC1101Pin = 3;
const byte CE_PIN=8;
const byte CSN_PIN=9;
const byte CSNCC11_PIN=10;
const byte SENSOR_PAYLOAD=28;
const byte THERM_PAYLOAD=13;
#define READ_PIPES_BRIDGE {(byte*)"1Node",(byte*)"2Node",(byte*)"3Node",(byte*)"4Node",(byte*)"5Node"}
#define WRITE_PIPE_BRIDGE {(byte*)"1Mast"}
 
/****OBJECTS*****/

RF24Sensors *sensors24;


