#ifndef STRUCTURES_H
#define STRUCTURES_H

#define BUF_LEN 10
#define KEY_LEN 5
#define METH_LEN 5

#define MODES_LEN 4
#define HOUR_PLAN_LEN 6
#define WEEK_LEN 7
#define HOUR_PLAN_MINUTE_UNDEFINED 1999
#define HOUR_PLAN_MODE_UNDEFINED 0
#define MAX_LEN 20

#define MESSAGE_LEN 15

#define MIN_CONS 9.0
#define MAX_CONS 30.0
#define MIN_DELTA 0.3
#define MAX_DELTA 2.0

#define EEP_MOD_IDX 0
#define EEP_THR_IDX 64
#define EEP_PLA_IDX 128

#define THERMOSTAT_TYPE 1
#define PROG_TYPE 2

//#define FACTORY_RESET
#define VERBOSE_MODE
#define EEPROM_SAVING

//#define RTC_DISPLAY
//#define RTC_REFRESH 1000

#include <stdint.h>
#include <Arduino.h>

struct Message
{
  char buf[MESSAGE_LEN];
};

struct HourPlan
{
  uint8_t modeId;
  int minute;
};

struct DayPlan
{
  uint8_t jour;
  HourPlan hourPlans[HOUR_PLAN_LEN];
};

struct WeekPlan
{
  uint8_t id;
  DayPlan dayPlans[WEEK_LEN];
};

struct Mode
{
  uint8_t id;
  float consigne;
  float delta;
};

struct ComStruct
{
  char meth[METH_LEN] = "";
  char key[KEY_LEN] = "";
  char buf[BUF_LEN] = "";
};

struct Sensor
{
  char id[MAX_LEN]="";
  float temp=0;
  float hygro=0;
};

struct Thermostat
{
  float consigne;
  float delta;
  float tempExt;
  bool interne;
  bool etat;
  uint8_t mode;
  uint8_t plan;
};

#endif
