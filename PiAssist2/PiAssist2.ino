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
#define VERSION "2.1.4"
#include "domusbox_config.h"
#include "structures.h"
#include "Timer.h"

#define MAX_BUF_LEN 64


//const String blueLampAddr = "98d3,31,fb18f4";
//void(* resetFunc) (void) = 0; //declare reset function @ address 0

/****OBJECTS*****/
Bluetooth *bt;
RF24Sensors *sensors24;
MyCC1101 *cc1101;
Utils *utils;
Timer t;
char lastRTCasked[5];

void setup() {
  utils = new Utils();
  pinMode(CSN_PIN, OUTPUT);
  digitalWrite(CSN_PIN, HIGH);
  pinMode(CSNCC11_PIN, OUTPUT);
  digitalWrite(CSNCC11_PIN, HIGH);

  //Serial and pins init
  Serial.begin(9600);
  while (!Serial);
  Serial.print(F("PiAssist "));
  Serial.print("v");
  Serial.println(VERSION);
  delay(SHORT_DELAY);
  bt = new Bluetooth();
  delay(LONG_DELAY);

  delay(DEFAULT_DELAY);
  sensors24 = new RF24Sensors(CE_PIN, CSN_PIN);
  delay(SHORT_DELAY);

  cc1101 = new MyCC1101();
  delay(SHORT_DELAY);
  cc1101->initChacon(14549858);
  delay(LONG_DELAY);
  cc1101->initRF433Sensors();
  delay(LONG_DELAY);
  digitalWrite(CSNCC11_PIN, HIGH);
  Serial.println(F("OK!"));
  delay(DEFAULT_DELAY);

  displayFreemem();
}

void loop() {

  checkBluetoothBuffer();
  sensors433RadioReceive();
  sensors24RadioReceive();
  t.update();
}

void displayFreemem() {
#ifdef DEBUG_MEM
  int freeMem = freeMemory();

  delay(10);
  Serial.print(F("freeMemory()="));
  Serial.println(freeMem);
  delay(10);

#endif
}

void serialEvent() {
  String inputString = String("");
  while (Serial.available()) {
    inputString += (char)Serial.read();
    delay(1);
  }
  char buf[MAX_BUF_LEN];
  inputString.toCharArray(buf, MAX_BUF_LEN);
  checkCommands(buf);
}

void checkBluetoothBuffer() {

  String btReceive = bt->receive();
  if ( btReceive != "") {
    Serial.println(btReceive);
    char buf[MAX_BUF_LEN];
    btReceive.toCharArray(buf, MAX_BUF_LEN);
    checkCommands(buf);
  }
}
void checkCommands(char* buf) {

  char separator[] = "/";
  
  utils->splitString(buf, separator);

 

  //--------->CC1101<-------------
  if (strcmp(utils->m_tabString[0], MDL_CC1101) == 0) {
 //   utils->logv("CC1101 ", false);
    //--------->CHACON<-------------
    // cc1101/chacon/14549858/2/0/
    if (strcmp(utils->m_tabString[1], PRT_CHACON) == 0) {
      long sender = atol(utils->m_tabString[2]);
      int interruptor = atoi(utils->m_tabString[3]);
      byte onoff = (byte) atoi(utils->m_tabString[4]);
   /*   utils->logv(sender, false);
      utils->logv(" ", false);
      utils->logv(interruptor, false);
      utils->logv(" ", false);
      utils->logv(onoff, true);*/

      cc1101->stopRadio();
      cc1101->sendDataChacon(sender, interruptor, onoff);


      cc1101->startRadio();
    }
    //--------->VIRTUAL WIRE<-------------
    if (strcmp(utils->m_tabString[1], PRT_VWIRE) == 0) {
      if (utils->m_tabString[2] == KEY_TEST) {
  //      utils->logv("RF433 transmit test", true);
        uint8_t buf[6] = "hello";
        for (int i = 0; i < 5; i++) {
          cc1101->RF433Transmit(buf, 20);
        }
       // utils->logv("sent", true);
        cc1101->startRadio();
      }
    }
  } else {

    //--------->NRF24L01<-----------
    if (strcmp(utils->m_tabString[0], MDL_NRF24) == 0) {
      ComStruct command;
    //  utils->logv("nrf24 ", false);
      if (strcmp(utils->m_tabString[1], KEY_INIT) == 0) {
        sensors24->setupRadio();

      }
      if (strcmp(utils->m_tabString[1], KEY_TEST) == 0) {
        printf_begin();
        sensors24->printDetails();
      }
      if (strcmp(utils->m_tabString[1], PRT_NODE) == 0 && utils->m_counter > 4) {
    //    utils->logv(" Node : ", false);

        byte buf[6];
        for (int i = 0; i < 6; i++) {
          buf[i] = utils->m_tabString[2][i];
        }
#ifdef SERIAL_PRINT
        utils->decomposeRaw(buf);
#endif
        sensors24->openWritingPipe(buf);
        //nrf24/node/1Nodw/multi/1/1/
        if (strcmp(utils->m_tabString[3], PRT_MULTIPLUG) == 0) {
          //utils->logv(" multi", false);
          command.buf[0] = 'r';
          command.buf[1] = 'm';
          command.buf[2] = utils->m_tabString[4][0];
          command.buf[3] = utils->m_tabString[5][0];
          sensors24->radioTransmit(command);
        }
        if (strcmp(utils->m_tabString[3], PRT_LAMP) == 0) {
          //utils->logv(" lamp", false);
          sensors24->m_multiprise.etat = atoi(utils->m_tabString[5]);
          sensors24->radioTransmit(sensors24->m_multiprise);
        }
        //nrf24/node/2Nodw/ther/
        if (strcmp(utils->m_tabString[3], PRT_THERM) == 0) {
          //utils->logv(" ther", false);
           strcpy(lastRTCasked, PRT_THERM);
          //nrf24/node/2Nodw/ther/set/
          if (strcmp(utils->m_tabString[4], METH_SET) == 0) {
            //nrf24/node/2Nodw/ther/set/cons/19/
            if (strcmp(utils->m_tabString[5], KEY_CONSIGN) == 0) {
            //  utils->logv(" cons", false);
              sensors24->m_thermostat.consigne = atof(utils->m_tabString[6]);
             // utils->logv(sensors24->m_thermostat.consigne , false);
              sensors24->radioTransmit(sensors24->m_thermostat);
            }
            //nrf24/node/2Nodw/ther/set/delta/0.7/
            if (strcmp(utils->m_tabString[5], KEY_DELTA) == 0) {
           //   utils->logv(" delta", false);
              sensors24->m_thermostat.delta = atof(utils->m_tabString[6]);
              sensors24->radioTransmit(sensors24->m_thermostat);
            }
            //nrf24/node/2Nodw/ther/set/temp/19.5/
            if (strcmp(utils->m_tabString[5], KEY_TEMP) == 0) {
           //   utils->logv(" temp", false);
              sensors24->m_thermostat.tempExt = atof(utils->m_tabString[6]);
              sensors24->radioTransmit(sensors24->m_thermostat);
            }
            //nrf24/node/2Nodw/ther/set/int/1/
            if (strcmp(utils->m_tabString[5], KEY_INTERNE) == 0) {
             // utils->logv(" int", false);
              sensors24->m_thermostat.interne = atoi(utils->m_tabString[6]);
              sensors24->radioTransmit(sensors24->m_thermostat);
            }

            //nrf24/node/2Nodw/ther/set/plan/1/
            if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0) {
            //  utils->logv(" plan", false);
              sensors24->m_thermostat.plan = atoi(utils->m_tabString[6]);
              sensors24->radioTransmit(sensors24->m_thermostat);
            }
          }
          //nrf24/node/2Nodw/ther/get/
          if (strcmp(utils->m_tabString[4], METH_GET) == 0) {
          //  utils->logv(" get", false);
            strcpy(command.meth, METH_GET);

            //nrf24/node/2Nodw/ther/get/info/
            if (strcmp(utils->m_tabString[5], KEY_INFO) == 0) {
            //  utils->logv(" info", false);
              strcpy(command.key, KEY_INFO);
              sensors24->radioTransmit(command);
            }
            //nrf24/node/2Nodw/ther/get/mode/1/
            if (strcmp(utils->m_tabString[5], KEY_MODE) == 0) {
             // utils->logv(" mode ", false);
              strcpy(command.key, KEY_MODE);
              strcpy(command.buf, utils->m_tabString[6]);
              sensors24->radioTransmit(command);
            }

            //nrf24/node/2Nodw/ther/get/plan/1/
            if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0) {
           //   utils->logv(" plan ", false);
              strcpy(command.key, KEY_PLAN);
              strcpy(command.buf, utils->m_tabString[6]);
              sensors24->radioTransmit(command);
            }
            //nrf24/node/2Nodw/ther/get/rtc/
            if (strcmp(utils->m_tabString[5], KEY_RTC) == 0) {
            //  utils->logv(" rtc ", false);
              strcpy(command.key, KEY_RTC);
              sensors24->radioTransmit(command);
            }
          }
          //nrf24/node/2Nodw/ther/sel/
          if (strcmp(utils->m_tabString[4], METH_SEL) == 0) {

            strcpy(command.meth, METH_SEL);
          //  utils->logv(" sel", false);
            //nrf24/node/2Nodw/ther/sel/mode/1/
            if (strcmp(utils->m_tabString[5], KEY_MODE) == 0) {
             // utils->logv(" mode", false);
              strcpy(command.key, KEY_MODE);
              strcpy(command.buf, utils->m_tabString[6]);
              sensors24->radioTransmit(command);
            }
          }

          //nrf24/node/2Nodw/ther/put/
          if (strcmp(utils->m_tabString[4], METH_PUT) == 0) {
           // utils->logv(" put", false);
            //nrf24/node/2Nodw/ther/put/mode/4/23.5/0.7/
            if (strcmp(utils->m_tabString[5], KEY_MODE) == 0) {
             // utils->logv(" mode ", false);
              Mode mode;
              mode.id = atoi(utils->m_tabString[6]);
              mode.consigne = atof(utils->m_tabString[7]);
              mode.delta = atof(utils->m_tabString[8]);
              sensors24->radioTransmit(mode);
            }

            //nrf24/node/2Nodw/ther/put/plan/2/1/3/06:00/23:00/XX:XX/XX:XX/
            if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0) {
            //  utils->logv(" plan ", false);
             dayPlanPut(THERMOSTAT_TYPE);
            }

            //nrf24/node/2Nodw/ther/put/rtc/6/2018/02/15/15/34/22/
            if (strcmp(utils->m_tabString[5], KEY_RTC) == 0) {             
               RTCTherPut();
            }
          }
          //nrf24/node/2Nodw/ther/save/
          if (strcmp(utils->m_tabString[4], METH_SAVE) == 0) {
           // Serial.println(" save");
            strcpy(command.meth, METH_SAVE);
            //nrf24/node/2Nodw/ther/save/plan/
            if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0) {
            //  Serial.println(" plan ");
              strcpy(command.key, KEY_PLAN);
              sensors24->radioTransmit(command);
            }

            //nrf24/node/2Nodw/ther/save/mode/
            if (strcmp(utils->m_tabString[5], KEY_MODE) == 0) {
           //   Serial.println(" mode ");
              strcpy(command.key, KEY_MODE);
              sensors24->radioTransmit(command);
            }
          }
        }

        //nrf24/node/3Nodw/aqua/
        if (strcmp(utils->m_tabString[3], PRT_AQUA) == 0) {
            strcpy(lastRTCasked, PRT_AQUA);
         // utils->logv(" aqua", false);
          //nrf24/node/3Nodw/aqua/get/
          if (strcmp(utils->m_tabString[4], METH_GET) == 0) {
             strcpy(command.meth, METH_GET);
            //nrf24/node/3Nodw/aqua/get/rtc/
            if (strcmp(utils->m_tabString[5], KEY_RTC) == 0) {
              //utils->logv(" rtc ", false);
              strcpy(command.key, KEY_RTC);
              sensors24->radioTransmit(command);
            }
            //nrf24/node/3Nodw/aqua/get/plan/
            if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0) {
              //utils->logv(" plan ", false);
              strcpy(command.key, KEY_PLAN);
              sensors24->radioTransmit(command);
            }
          }
    
          //nrf24/node/3Nodw/aqua/put/
          if (strcmp(utils->m_tabString[4], METH_PUT) == 0) {
            //nrf24/node/3Nodw/aqua/put/rtc/2018/03/02/15/49/22/
            if (strcmp(utils->m_tabString[5], KEY_RTC) == 0) {            
              RTCPut();
            } 
            //nrf24/node/3Nodw/aqua/put/plan/06:00/23:00/
            if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0) {            
              dayPlanPut(PROG_TYPE);
            }                             
          }          
        }
       //nrf24/node/3Nodw/aqua/set/
       if (strcmp(utils->m_tabString[4], METH_SET) == 0) {
            strcpy(command.meth, METH_SET);
            //nrf24/node/3Nodw/aqua/set/leds/
            if (strcmp(utils->m_tabString[5], KEY_LEDS) == 0) {
              //utils->logv(" leds ", false);
              strcpy(command.key, KEY_LEDS);
              sensors24->radioTransmit(command);
            }
       }


        
      }
    } else {
      //BLUETOOTH
      if (strcmp(utils->m_tabString[0], MDL_BT) == 0) {

        //CONNEXION
       /* if (strcmp(utils->m_tabString[1], KEY_CONNECT) == 0) {
        //  utils->logv("connect", true);
          bt->setAtMode(true);
        }
        if (strcmp(utils->m_tabString[1], KEY_INIT) == 0) {
          bt->setAtMode(false);
        }
        if (strcmp(utils->m_tabString[1], KEY_MASTER) == 0) {
          bt->setRole(true);
        }
        if (strcmp(utils->m_tabString[1], KEY_SLAVE) == 0) {
          bt->setRole(false);
        }
        if (strcmp(utils->m_tabString[1], KEY_ROLE) == 0) {
          bt->getRole();
        }*/
        if (strcmp(utils->m_tabString[1], KEY_VAL) == 0) {

          int buf;

          buf = atoi(utils->m_tabString[2]);
          bt->sendInt(buf);
        }
      }
    }

  }
  //utils->showTabstring();
  displayFreemem();
}

void RTCTherPut(){
  tmElements_t tm;
  tm.Wday = atoi(utils->m_tabString[6]);  
  tm.Year = CalendarYrToTm(atoi(utils->m_tabString[7]));
  tm.Month = atoi(utils->m_tabString[8]);
  tm.Day = atoi(utils->m_tabString[9]);
  tm.Hour = atoi(utils->m_tabString[10]);
  tm.Minute = atoi(utils->m_tabString[11]);
  tm.Second = atoi(utils->m_tabString[12]);
  sensors24->radioTransmit(tm);  
}

void RTCPut(){            
  tmElements_t tm;
  tm.Year = CalendarYrToTm(atoi(utils->m_tabString[6]));
  tm.Month = atoi(utils->m_tabString[7]);
  tm.Day = atoi(utils->m_tabString[8]);
  tm.Hour = atoi(utils->m_tabString[9]);
  tm.Minute = atoi(utils->m_tabString[10]);
  tm.Second = atoi(utils->m_tabString[11]);
  sensors24->radioTransmit(tm);  
}

void dayPlanPut(uint8_t type){ 

  switch(type){
    case THERMOSTAT_TYPE:{
      sensors24->m_dayplan.jour = atoi(utils->m_tabString[6]);
      sensors24->m_dayplan.modeId = atoi(utils->m_tabString[7]);
      sensors24->m_dayplan.defaultModeId = atoi(utils->m_tabString[8]);
      strcpy(sensors24->m_dayplan.heure1Start, utils->m_tabString[9]);
      strcpy(sensors24->m_dayplan.heure1Stop, utils->m_tabString[10]);
      strcpy(sensors24->m_dayplan.heure2Start, utils->m_tabString[11]);
      strcpy(sensors24->m_dayplan.heure2Stop, utils->m_tabString[12]);      
      break;
    }
    case PROG_TYPE:{
      sensors24->m_dayplan.jour = 0;
      sensors24->m_dayplan.modeId = 0;
      sensors24->m_dayplan.defaultModeId = 0;
      strcpy(sensors24->m_dayplan.heure1Start, utils->m_tabString[6]);
      strcpy(sensors24->m_dayplan.heure1Stop, utils->m_tabString[7]);
      strcpy(sensors24->m_dayplan.heure2Start, " ");
      strcpy(sensors24->m_dayplan.heure2Stop, " ");      
      break;
    }
    default:{
      return;
    }   
  }
  
  sensors24->radioTransmit(sensors24->m_dayplan);
}


void sensors24RadioReceive() {
  uint8_t payloadSize = sensors24->radioReceive();
  switch (payloadSize) {
    case sizeof(Sensor):
      bindSensor();
      break;
    case sizeof(Thermostat):
      bindThermostat();
      break;
    case sizeof(Mode):
      bindMode();
      break;
    case sizeof(DayPlan):
      bindDayplan();
      break;
    case sizeof(tmElements_t):
      bindRTC();
      break;
    case sizeof(Message):
      bindMessage();
      break;
  }

}

void bindSensor() {
  String mergedString = "";
  bt-> m_serialPrint = true;
  mergedString = sensors24->m_sensor.id;
  mergedString += " ";
  mergedString += String(sensors24->m_sensor.temp);

  if (sensors24->m_sensor.hygro >= 0) {
    mergedString += " ";
    mergedString += String(sensors24->m_sensor.hygro);

  }
  bt->logString(mergedString);
  bt->CR();
  delay(100);
  bt-> m_serialPrint = false;
  displayFreemem();

}

void bindThermostat() {
  String mergedString = "";
  bt-> m_serialPrint = true;
  mergedString = "thermostat ";
  mergedString += String(sensors24->m_thermostat.consigne);
  mergedString += " ";
  mergedString += String(sensors24->m_thermostat.delta);
  mergedString += " ";
  mergedString += String(sensors24->m_thermostat.tempExt);
  mergedString += " ";
  mergedString += String(sensors24->m_thermostat.interne);
  mergedString += " ";
  mergedString += String(sensors24->m_thermostat.etat);
  bt->logString(mergedString);
  bt->CR();
  t.after(300, bindThermostat2);
}

void bindThermostat2() {
  String mergedString = "";

  mergedString = "thersel ";
  mergedString += String(sensors24->m_thermostat.mode);
  mergedString += " ";
  mergedString += String(sensors24->m_thermostat.plan);
  bt->logString(mergedString);
  bt->CR();
  delay(100);
  bt-> m_serialPrint = false;
  displayFreemem();
}

void bindMode() {
  String mergedString = "";
  bt-> m_serialPrint = true;
  mergedString = "thermode ";
  mergedString += String(sensors24->m_mode.id);
  mergedString += " ";
  mergedString += String(sensors24->m_mode.consigne);
  mergedString += " ";
  mergedString += String(sensors24->m_mode.delta);
  bt->logString(mergedString);
  bt->CR();
  delay(100);
  bt-> m_serialPrint = false;
  displayFreemem();
}

void bindDayplan() {

  String mergedString = "";
  bt-> m_serialPrint = true;
  mergedString = lastRTCasked;
  mergedString += "plan ";
  if (strcmp(lastRTCasked, PRT_THERM) == 0) {

    mergedString += String(sensors24->m_dayplan.jour);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.modeId);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.defaultModeId);
    mergedString += " ";
  }
  mergedString += String(sensors24->m_dayplan.heure1Start);
  mergedString += " ";
  mergedString += String(sensors24->m_dayplan.heure1Stop);
  
  if (strcmp(lastRTCasked, PRT_THERM) == 0) {
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.heure2Start);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.heure2Stop);
  }
  

  bt->logString(mergedString);
  bt->CR();
  delay(100);
  bt-> m_serialPrint = false;
  displayFreemem();

}
void bindRTC() {
  tmElements_t tm = sensors24->m_rtc;
  Serial.print(lastRTCasked);
  Serial.print(F("clock "));
  Serial.print(tm.Wday);  
  Serial.write(' ');
  Serial.print(tmYearToCalendar(tm.Year));
  Serial.write('/');
  print2digits(tm.Month);
  Serial.write('/');
  print2digits(tm.Day);
  Serial.write(' ');
  print2digits(tm.Hour); Serial.write(':');
  print2digits(tm.Minute);
  Serial.write(':');
  print2digits(tm.Second);
  Serial.println("");
  delay(100);

}
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.print('0');
  }
  Serial.print(number);
}
void bindMessage() {

  String mergedString = "";
  bt-> m_serialPrint = true;

  mergedString = "message ";
  mergedString += String( sensors24->m_message.buf);

  bt->logString(mergedString);
  bt->CR();
  delay(100);
  bt-> m_serialPrint = false;
}

void sensors433RadioReceive() {
  if (cc1101->radioReceive() && cc1101->bufIsOk) {
    bt->m_serialPrint = true;
    String bufString = String(cc1101->m_sensor.id) + " " + String(cc1101->m_sensor.temp.valFloat);
    bt->logString(bufString);
    bt->m_serialPrint = false;
    bt->CR();
  }
}
void sensors433RadioRawReceive() {

  if (cc1101->radioRawReceive()) {

    Serial.println((char*)cc1101->bufRaw);
  }
}
