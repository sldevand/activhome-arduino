/*
   Project Name : PiAssist2
   By : Sébastien LORRAIN
   Date : 02/03/2018
   Version : 2.1.5
   Description :
   A brancher en USB sur le Raspberry Pi de la box domotique DIY DomusBox
   Sert à gérer la RF pour le projet de box domotique DIY DomusBox
   ------------------------------------------------------------------------------------------------
   - Gestion du CC1101
     librairies utilisées : ELECHOUSE_cc1101->h  (+ libs perso : ChaconDIO.h RF433Sensors.h)
     source :  https://github.com/simonmonk/CC1101_arduino
      ------------------------------------------------------------------------------------------------
      - Gestion des sondes de température RF433
      librairies utilisées : VirtualWire.h
      source : http://www.airspayce.com/mikem/arduino/VirtualWire/VirtualWire-1.27.zip
      ------------------------------------------------------------------------------------------------
      - Gestion des sondes de température RF24l01
      librairies utilisées : nRF24L01.h RF24.h et RF24_config.h
      source : http://tmrh20.github.io/RF24
   ------------------------------------------------------------------------------------------------
   - Gestion des prises RF433 Chacon IO
     librairies utilisées : adaptée du fichier radioEmission.cpp dans le zip 'hcc.zip' de idleman :
     source : http://idleman.fr/ressources/PHP/hcc/hcc.zip
   ------------------------------------------------------------------------------------------------
   - Gestion du Bluetooth
      librairies utilisées : SoftwareSerial.h (officielle Arduino IDE)
      source : https://www.arduino.cc/en/Reference/softwareSerial
   ------------------------------------------------------------------------------------------------
   - Gestion de Servo-Moteurs

*/
//cc1101/chacon/14549858/2/1/
//cc1101/chacon/14549858/2/0/
//#define DEBUG_MEM
//#define SERIAL_PRINT
#define VERSION "2.1.5"
#include "domusbox_config.h"
#include "structures.h"
#include "Timer.h"

#define MAX_BUF_LEN 64

/****OBJECTS*****/
Bluetooth *bt;
RF24Sensors *sensors24;
MyCC1101 *cc1101;
Utils *utils;
Timer t;
char lastRTCasked[5];

void displayFreemem();
void serialEvent();
void checkBluetoothBuffer();
void checkCommands(char *buf);
void RTCTherPut();
void RTCPut();
void dayPlanPut(uint8_t type);
void sensors24RadioReceive();
void bindSensor();
void bindThermostat();
void bindThermostat2();
void bindMode();
void bindDayplan();
void bindRTC();
void print2digits(int number);
void bindMessage();
void sensors433RadioReceive();