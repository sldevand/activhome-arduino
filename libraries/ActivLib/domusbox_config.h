/******INCLUDES********/

#include "RF24Sensors.h"
#include "MyCC1101.h"
#include "Bluetooth.h"
#include "Utils.h"
#include <printf.h>
#include <MemoryFree.h>

#include "domusbox_defines.h"

/****PINOUT SETTING****/
const byte txCC1101Pin = 2;
const byte rxCC1101Pin = 3;
const byte tx433Pin = 7;
const byte rx433Pin = 5;
const byte servoXPin=4;  //declaration constante de broche axe X
const byte servoYPin=6;  //declaration constante de broche axe Y
const uint16_t CE_PIN=8;
const uint16_t CSN_PIN=9;
const byte CSNCC11_PIN=10;
