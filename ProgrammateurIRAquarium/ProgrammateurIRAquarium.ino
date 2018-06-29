/*
    MCU = Arduino Pro-mini
    Radio = NRF24L01+
    NodeIDWrite="3Mast"
    NodeIDRead="3Nodw"


    RadioId = aquarium
    Version : 1.0 (01/03/18)
      - Création.
*/

#include "structures.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "Timer.h"
#include "RTCManager.h"
#include "Utils.h"
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <IRremote.h>
#include "EEPROMAnything.h"
#include "Utils.h"

#define CE_PIN 9
#define CSN_PIN 10
#define LDR_PIN A0


#define VERSION "1.0"


#define METH_GET "get"
#define METH_SET "set"
#define KEY_INFO "info"
#define KEY_PLAN "plan"
#define KEY_RTC "rtc"
#define KEY_LEDS "leds"
#define STATE_FAILURE 254

#define R_PIPE 1

#define ONOFF 0xFFFD02
#define RED 0xFFF807
#define BLUE 0xFFF609
#define GREEN 0xFFF40B
#define WHITE 0xFFF708



Timer t;
int timerEvent;
int refreshTime = 1000;
bool delayedInfos = false;

RF24 radio(CE_PIN, CSN_PIN);
ComStruct commande;
RTCManager clkMan;
IRsend irsend;
int sensitivity = 40;
uint8_t etatLampe=STATE_FAILURE;
DayPlan dp;

void(* resetFunc) (void) = 0;//declare reset function at address 0

void setup() {

  Serial.begin(115000);
  Serial.print(F("- Aquarium ID Prog ver. "));
  Serial.print(VERSION);
  Serial.println(F(" -"));
  flushCommande();
  initRadio();
  startTimer();
  readRTC();
  Serial.println(F("Setup OK"));
  EEPROM_readAnything(0,dp);
}

void loop(void) {
  t.update();
  radioRead();
}
void checkPlanTime() {

    readRTC();
   DayPlan dayPlan = dp;

    long tm[2];   
  
    tm[0] = hoursToSeconds(dayPlan.heure1Start);
    tm[1] = hoursToSeconds(dayPlan.heure1Stop);

    
    long rtcTime = tmToSeconds(clkMan.tm);
   
   if ((rtcTime == tm[0] || rtcTime == tm[1])) {
      switchOnOff();
    }
}

tmElements_t strToHours(char* str) {
  Utils utils;
  tmElements_t tm;
  char separator[] = ":";
  utils.splitString(str, separator);
  tm.Hour = atoi(utils.m_tabString[0]);
  tm.Minute = atoi(utils.m_tabString[1]);
  tm.Second = 0;
  return tm;
}

long tmToSeconds(tmElements_t tm) {  
  long result = tm.Hour*3600L + tm.Minute*60L + tm.Second * 1L;  
  return result;
}

long hoursToSeconds(char* str) {
  tmElements_t tm = strToHours(str);
  return tmToSeconds(tm);
}

bool radioRead() {
  if (radio.available()) {
    uint8_t payloadSize = radio.getDynamicPayloadSize();
    radio.setPayloadSize(payloadSize);
    Serial.print("received smthing : ");
    Serial.println(payloadSize);
    flushCommande();
    switch (payloadSize) {
      case sizeof(DayPlan):
        return bindDayplan();
      case sizeof(ComStruct):
        return bindCommande();
      case sizeof(tmElements_t):
        return bindRTC();
      case 2:
        resetFunc();
      default :
        radio.flush_rx();
        flushCommande();
        return false;
    }


  }
}

bool bindCommande() {
  radio.read( &commande, sizeof(commande));
  int id;
  //GET
  if (strcmp(commande.meth, METH_GET) == 0) {
    
    //INFO
    if (strcmp(commande.key, KEY_INFO) == 0) {     
      delayedInfos = true;
      flushCommande();
      return true;
    }

    //RTC
    if (strcmp(commande.key, KEY_RTC) == 0) {    
      sendRTC(readRTC());
    }

     //PLAN
    if (strcmp(commande.key, KEY_PLAN) == 0) {    
      t.after(200,sendDayplan);
    }
  }

  //SET
  if (strcmp(commande.meth, METH_SET) == 0) {
    if (strcmp(commande.key, KEY_LEDS) == 0) {    
      Serial.println("Comm switchOnoff");
      switchOnOff();
    }
  }

  flushCommande();
  return false;
}

bool bindRTC() {
  tmElements_t tm;
  radio.read( &tm, sizeof(tmElements_t));
  clkMan.display();
  if (clkMan.write(tm)) sendRTC(tm);
  return true;
}

bool bindDayplan() {
  stopTimer();
  radio.read( &dp, sizeof(DayPlan));  
  t.after(200,sendDayplan);
  EEPROM_writeAnything(0,dp);
  startTimer();
  return true;
}

void sendDayplan() {

  radio.stopListening();
  radio.setPayloadSize(sizeof(DayPlan));
  radio.write( &dp, sizeof(DayPlan));
  radio.startListening();
}
void sendRTC(tmElements_t tm) {
  radio.stopListening();
  delay(100);
  radio.setPayloadSize(sizeof(tmElements_t));
  radio.write( &tm, sizeof(tmElements_t));
  radio.startListening();
}

void sendMessage(char* str) {
  radio.stopListening();
  delay(100);
  Message message;
  strcpy(message.buf, str);
  radio.setPayloadSize(sizeof(Message));
  radio.write( &message, sizeof(Message));
  Serial.println(message.buf);
  radio.startListening();
}


void initRadio() {
  byte addressWrite[6] = "3Mast";
  byte addressRead[6] = "3Nodw";

  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15, 15); // Max delay between retries & number of retries
  radio.openWritingPipe(addressWrite);
  radio.openReadingPipe(R_PIPE, addressRead);
  radio.setPALevel( RF24_PA_HIGH);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.startListening();
}

void startTimer() {
  timerEvent = t.every(refreshTime, checkPlanTime);
}
void stopTimer() {
  t.stop(timerEvent);

}

void flushCommande() {
  for (byte i = 0; i < BUF_LEN; i++) {
    commande.buf[i] = 0x00;
  }
}

tmElements_t readRTC() {

  tmElements_t tm = clkMan.read();
  clkMan.display();
  return tm;
}



uint8_t switchOnOff() {

  int diffLum, prevLum, postLum;
  uint8_t etat = STATE_FAILURE;

  
  uint8_t retries_cnt = 0;
  const uint8_t retries = 2;

  do{  
  uint8_t timeout_cnt = 0;
  const uint8_t timeout = 6;
    do {
      prevLum = readLDR();    
      
      sendONOFF();
      delay(100);
      
      postLum = readLDR();   
      diffLum = postLum - prevLum;
      Serial.println(diffLum);
      timeout_cnt++;
      if (timeout_cnt > timeout){
        Serial.println(F("timeout overflow"));
        break;
      }
  
    } while (abs(diffLum) < sensitivity);
  
    if (diffLum > sensitivity) etat = 1;
    if (diffLum < -sensitivity) etat = 0;

      retries_cnt++;
      if (retries_cnt > retries){
        Serial.println(F("retries overflow"));
        break;
      }
  }while(etat == STATE_FAILURE || etat == etatLampe);

  etatLampe = etat;  
  Serial.print("Etat = ");
  Serial.println(etatLampe);  
  char buf[MESSAGE_LEN];
  
  char buf2[3];
  itoa (etatLampe,buf2,10);  
  
  strcpy(buf,"aqua ");  
  strcat(buf,buf2);
  sendMessage(buf);
 
  return etat;
}

void sendONOFF() {
  sendIR(ONOFF);
}

void sendIR(unsigned long data) {
  irsend.sendNEC(data, 32);
}

int readLDR() {
  int ldr = 0;
  for(uint8_t i=0;i<5;i++){
    ldr=analogRead(LDR_PIN);
    delay(2);    
  }
  return 1023 - ldr;

}


