#include "structures.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "Timer.h"
#include "ModesManager.h"
#include "PlanningsManager.h"
#include "ThermostatManager.h"
#include "RTCManager.h"
#include "Utils.h" ~
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <DHT.h>
#include <DHT_U.h>

#define CE_PIN 9
#define CSN_PIN 10
#define DHTPIN 4
#define DHTTYPE DHT22
#define R_PIPE 1
#define VERSION 1.3

#define METH_GET "get"
#define METH_SEL "sel"
#define METH_SET "set"
#define METH_PUT "put"
#define METH_SAVE "save"
#define KEY_MODE "mode"
#define KEY_INFO "info"
#define KEY_PLAN "plan"
#define KEY_RTC "rtc"
#define KEY_PWR "pwr"

#define CHK_PL_TIME 30000

RF24 radio(CE_PIN, CSN_PIN);
DHT dht(DHTPIN, DHTTYPE);

long refreshTime = 15000;                      // millisecondes
byte sendingTime = 120 / (refreshTime / 1000); // envoi de la temperature par radio
// tous les 'sendingTime * refreshTime'
byte currentTime = 1; // Compteur actuel de 'sendingTime'

const int entreeChaudiere = 3;
const int sortieChaudiere = 2;

bool etatChaudiere; // Etat du relais de la chaudiere
bool dernierEtatChaudiere;
bool power = true;

const float tempOffset = -0.1;
bool delayedInfos = false;

Timer t;
int timerEvent;
int timerEventCheckPlan;

Sensor sensor;
ComStruct commande;

ModesManager modMan;
PlanningsManager plMan;
ThermostatManager thMan;
RTCManager clkMan;

void setup()
{
  strcpy(sensor.id, "sensor24thermid1\0");

  Serial.begin(115000);
  Serial.print("- Thermo ver. ");
  Serial.print(VERSION);
  Serial.println(" -");
  pinMode(entreeChaudiere, INPUT);
  pinMode(sortieChaudiere, OUTPUT);
  digitalWrite(sortieChaudiere, HIGH);
  etatChaudiere = false;
  dernierEtatChaudiere = false;
  sensor.temp = -273.0;
  flushCommande();
  modMan.initModes();
  plMan.initPlanning();
  thMan.init();

  readRTC();
  initRadio();
  startTimer();
  dht.begin();
  checkPlanTime();
  Serial.println("Setup OK");
}

void loop(void)
{

  t.update();
  etatChaudiere = digitalRead(entreeChaudiere);
  if (etatChaudiere != dernierEtatChaudiere) {
    envoiEtatChaudiere();
  }

  if (thMan.getInterne() == 1) {
    thMan.setTempActuelle(sensor.temp);
  } else {
    thMan.setTempActuelle(thMan.getTempExt());
  }

  if (thMan.getTempActuelle() < (thMan.getConsigne() - thMan.getDelta() / 2)) {
    allumerChaudiere();
  } else {
    if (thMan.getTempActuelle() > (thMan.getConsigne() + thMan.getDelta() / 2)) {
      eteindreChaudiere();
    }
  }
  dernierEtatChaudiere = etatChaudiere;

  radioRead();

  if (delayedInfos) {
    t.after(300, sendAllCallback);
    delayedInfos = false;
  }
}

void checkPlanTime()
{
  if (thMan.getPlan() > 0) {
    readRTC();
    DayPlan dp;

    if (clkMan.tm.Wday >= 0 && clkMan.tm.Wday <= 7) {
      dp = plMan.getDayPlan(clkMan.tm.Wday);
    } else {
      Serial.println(F("invalid weekDay !"));
      return;
    }

    int rtcTime = tmToMinutes(clkMan.tm);
    uint8_t mode = 1;
    for (int hPlan = 0; hPlan < HOUR_PLAN_LEN; hPlan++) {
      if (rtcTime >= dp.hourPlans[hPlan].minute) {
        mode = dp.hourPlans[hPlan].modeId;
        break;
      }
    }

    bindThermostatWithMode(mode);
  }
}

tmElements_t strToHours(char *str)
{
  Utils utils;
  tmElements_t tm;
  char separator[] = ":";
  utils.splitString(str, separator);
  tm.Hour = atoi(utils.m_tabString[0]);
  tm.Minute = atoi(utils.m_tabString[1]);
  tm.Second = 0;
  return tm;
}

int tmToMinutes(tmElements_t tm)
{
  return tm.Hour * 60 + tm.Minute;
}

bool radioRead()
{
  if (!radio.available()) {
    return false;
  }
  uint8_t payloadSize = radio.getDynamicPayloadSize();
  radio.setPayloadSize(payloadSize);
  flushCommande();
  switch (payloadSize) {
  case sizeof(Thermostat):
    return bindThermostat();
  case sizeof(Sensor):
    return bindSensor();
  case sizeof(ComStruct):
    return bindCommande();
  case sizeof(Mode):
    return bindMode();
  case sizeof(DayPlan):
    return bindDayplan();
  case sizeof(tmElements_t):
    return bindRTC();
  default:
    radio.flush_rx();
    flushCommande();
    return false;
  }  
}

bool bindThermostat()
{
  Thermostat thermostat;
  radio.read(&thermostat, sizeof(thermostat));
  thMan.setThermostat(thermostat);
  t.after(20, sendThermoCallback);
#ifdef VERBOSE_MODE
  Serial.println("*New par rcv*");
#endif
  return true;
}

void bindThermostatWithMode(uint8_t id)
{
  if (id <= 0) {
    return;
  }
  Mode mode;
  mode = modMan.getMode(id);
 if (mode.id <= 0) {
    return;
  }
  bool changeMode = false;
  if (mode.id != thMan.getMode()) {
    #ifdef VERBOSE_MODE
    Serial.print("chg Mod from = ");
    Serial.print(thMan.getMode());
    Serial.print(" to ");
    Serial.println(id);
    #endif
    changeMode = true;
  }
 
  delayedInfos = thMan.hasChanged(mode);
  if (delayedInfos) {
  #ifdef VERBOSE_MODE
    Serial.print("thermo has changed");
  #endif
  }
  thMan.setConsigne(mode.consigne);
  thMan.setDelta(mode.delta);
  if (changeMode) {
    thMan.setMode(mode.id);
  }
  thMan.display();  
}

bool bindMode()
{
  Mode mode;
  radio.read(&mode, sizeof(mode));
  modMan.addMode(mode);

  return true;
}

bool bindSensor()
{
  Sensor sensorRecu;
  radio.read(&sensorRecu, sizeof(sensorRecu));

  radio.stopListening();
  radio.setPayloadSize(sizeof(sensorRecu));
  radio.write(&sensorRecu, sizeof(sensorRecu));
  radio.startListening();

  return true;
}

bool bindCommande()
{
  radio.read(&commande, sizeof(commande));

  int id;
  delay(2);
  // GET
  if (strcmp(commande.meth, METH_GET) == 0) {

    // INFO
    if (strcmp(commande.key, KEY_INFO) == 0) {
      delayedInfos = true;
      flushCommande();
      return true;
    }

    // PWR
    if (strcmp(commande.key, KEY_PWR) == 0) {
      sendPowerState();
      return true;
    }

    // MODE
    if (strcmp(commande.key, KEY_MODE) == 0) {
      delayedInfos = false;
      id = atoi(commande.buf);
      if (id < 254) {
        Mode mode = modMan.getMode(id);
        if (mode.id > 0) {
          sendMode(mode);
        }
      } else {
        for (byte i = 0; i < MODES_LEN; i++) {
          sendMode(modMan.modes[i]);
        }
      }
    }

    // PLAN
    if (strcmp(commande.key, KEY_PLAN) == 0) {
      delayedInfos = false;

      id = atoi(commande.buf);

      stopTimer();
      if (id < 254) {
        DayPlan dp = plMan.getDayPlan(id);
        if (dp.jour > 0) {
          sendDayplan(dp);
        }
      } else {
        for (byte i = 0; i < WEEK_LEN; i++) {
          sendDayplan(plMan.weekPlan.dayPlans[i]);
          delay(100);
        }
      }
      startTimer();
    }
    // RTC
    if (strcmp(commande.key, KEY_RTC) == 0) {
      sendRTC(clkMan.read());
    }
  }
  // SEL
  if (strcmp(commande.meth, METH_SEL) == 0) {
    // MODE
    if (strcmp(commande.key, KEY_MODE) == 0) {
      delayedInfos = true;
      int id = atoi(commande.buf);
      if (id > 0 && thMan.getPlan() <= 0)
        bindThermostatWithMode(id);
      else
        sendMessage("tht pon");
    }
  }

  // SET
  if (strcmp(commande.meth, METH_SET) == 0) {
    // PLAN
    if (strcmp(commande.key, KEY_PLAN) == 0) {
      delayedInfos = false;
      int id = atoi(commande.buf);
      if (id > 0)
        thMan.setPlan(id);
    }
    // PWR
    if (strcmp(commande.key, KEY_PWR) == 0) {
      delayedInfos = false;
      int id = atoi(commande.buf);
      if (id >= 0 && id <= 1) {
        power = !!id;
        if (!power) {
          eteindreChaudiere();
        }
        sendPowerState();
      }
    }
  }

  // SAVE
  if (strcmp(commande.meth, METH_SAVE) == 0) {
    // PLAN
    if (strcmp(commande.key, KEY_PLAN) == 0) {
      delayedInfos = true;
      plMan.savePlanningInEEPROM();
      plMan.displayPlanning();
      sendMessage("tht spok");
    }

    // MODE
    if (strcmp(commande.key, KEY_MODE) == 0) {
      delayedInfos = true;
      modMan.saveModesInEEPROM();
      modMan.displayModes();
      sendMessage("tht smok");
    }
  }

  flushCommande();
  return false;
}

void sendPowerState()
{
  char *str;
  if (power)
    str = "tht pow 1\0";
  else
    str = "tht pow 0\0";

  sendMessage(str);
}

bool bindDayplan()
{
  DayPlan dp;
  stopTimer();
  radio.read(&dp, sizeof(DayPlan));
  plMan.setDayPlan(dp);

  startTimer();
  return true;
}

bool bindRTC()
{
  tmElements_t tm;
  radio.read(&tm, sizeof(tmElements_t));
  clkMan.display();
  if (clkMan.write(tm))
    sendRTC(tm);
  return true;
}

void sendMode(Mode mode)
{
  radio.stopListening();
  delay(100);
  radio.setPayloadSize(sizeof(Mode));
  radio.write(&mode, sizeof(Mode));
  radio.startListening();
}

void sendDayplan(DayPlan dayPlan)
{
  radio.stopListening();
  radio.setPayloadSize(sizeof(DayPlan));
  radio.write(&dayPlan, sizeof(DayPlan));
  radio.startListening();
}

void sendRTC(tmElements_t tm)
{
  radio.stopListening();
  delay(100);
  radio.setPayloadSize(sizeof(tmElements_t));
  radio.write(&tm, sizeof(tmElements_t));
  radio.startListening();
}

void sendMessage(char *str)
{
  radio.stopListening();
  delay(100);
  Message message;
  strcpy(message.buf, str);
  radio.setPayloadSize(sizeof(Message));
  radio.write(&message, sizeof(Message));
  Serial.println(message.buf);
  radio.startListening();
}

void capteurToString()
{
  Serial.print(F("ID: "));
  Serial.print(sensor.id);
  Serial.print(F(" Tmp: "));
  Serial.print(sensor.temp);
  Serial.print(F(" Hyg: "));
  Serial.println(sensor.hygro);
}

void initRadio()
{
  byte addressWrite[6] = "2Mast";
  byte addressRead[6] = "2Nodw";

  radio.begin();            // Start up the radio
  radio.setAutoAck(1);      // Ensure autoACK is enabled
  radio.setRetries(15, 15); // Max delay between retries & number of retries
  radio.openWritingPipe(addressWrite);
  radio.openReadingPipe(R_PIPE, addressRead);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.startListening();
}

void radioSend()
{
  radio.stopListening();
  radio.setPayloadSize(sizeof(sensor));
  radio.write(&sensor, sizeof(sensor));
  delay(300);
  sendInfos(false);
#ifdef VERBOSE_MODE
  Serial.println(F("--- Sens snd : "));
  capteurToString();
  Serial.println(F("---"));
#endif
  radio.startListening();
}

void sendThermoCallback()
{
  sendInfos(true);
}

void sendAllCallback()
{
  checkPlanTime();
  radioSend();
}

void sendInfos(bool stopStart)
{
  if (stopStart)
    radio.stopListening();
  Thermostat thermostat = thMan.getThermostat();

#ifdef VERBOSE_MODE
  Serial.println(F("--- Ther snd : "));
  thMan.display();
  Serial.println(F("---"));
#endif
  radio.setPayloadSize(sizeof(Thermostat));
  radio.write(&thermostat, sizeof(Thermostat));
  if (stopStart)
    radio.startListening();
}

void allumerChaudiere()
{
  if (power) {
    digitalWrite(sortieChaudiere, LOW);
  }
}

void eteindreChaudiere()
{
  digitalWrite(sortieChaudiere, HIGH);
}

void envoiEtatChaudiere()
{
  measure();
  radioSend();
}

void startTimer()
{
  timerEvent = t.every(refreshTime, measureAndSend);
  timerEventCheckPlan = t.every(CHK_PL_TIME, checkPlanTime);
}
void stopTimer()
{
  t.stop(timerEvent);
  t.stop(timerEventCheckPlan);
}

void measureAndSend()
{
  measure();
  if (currentTime >= sendingTime) {
    radioSend();
    currentTime = 1;
  } else {
    currentTime++;
  }
}

void measure()
{
  if (etatChaudiere)
    thMan.setEtat(0.0);
  else
    thMan.setEtat(1.0);

  stopTimer();
  radio.stopListening();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t) || isnan(h)) {
    Serial.println(F("DHT read fail"));
    radio.startListening();
    startTimer();
    return;
  }

#ifdef VERBOSE_MODE
  Serial.print(F("Tmp: "));
  Serial.print(t);
  Serial.print(F(" *C,"));
  Serial.print(F("Hyg: "));
  Serial.print(h);
  Serial.println(F(" %\t"));
#endif

  sensor.temp = t + tempOffset;
  sensor.hygro = h;
  radio.startListening();
  startTimer();
}

void flushCommande()
{
  for (byte i = 0; i < BUF_LEN; i++) {
    commande.buf[i] = 0x00;
  }
}

tmElements_t readRTC()
{
  tmElements_t tm = clkMan.read();
  clkMan.display();
  return tm;
}
