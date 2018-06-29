#ifndef STRUCTURES_H
#define STRUCTURES_H

#define BUF_LEN 10
#define KEY_LEN 5
#define METH_LEN 5

#define MODES_LEN 4
#define WEEK_LEN 7
#define HOUR_LEN 6
#define MAX_LEN 20

#define MESSAGE_LEN 10

#define MIN_CONS 9.0
#define MAX_CONS 24.0
#define MIN_DELTA 0.3
#define MAX_DELTA 2.0

#define EEP_MOD_IDX 0
#define EEP_THR_IDX 64
#define EEP_PLA_IDX 128

#define VERBOSE_MODE
//#define FACTORY_RESET
#define EEPROM_SAVING
//#define RTC_DISPLAY
//#define RTC_REFRESH 1000

#define THERMOSTAT_TYPE 1
#define PROG_TYPE 2

#include <stdint.h>
#include <Arduino.h>

struct Message{
  char buf[MESSAGE_LEN];

};

struct DayPlan{ 
  uint8_t jour;
  uint8_t modeId;
  uint8_t defaultModeId;
  char heure1Start[HOUR_LEN];
  char heure1Stop[HOUR_LEN];
  char heure2Start[HOUR_LEN];
  char heure2Stop[HOUR_LEN]; 
};

struct WeekPlan{
  uint8_t id;
  DayPlan dayPlans[WEEK_LEN];
};

struct Mode{
  uint8_t id;
  float consigne;
  float delta;
};

struct ComStruct{  
  char meth[METH_LEN];
  char key[KEY_LEN];
  char buf[BUF_LEN]; 
};

struct Sensor{
   char id[MAX_LEN];
   float temp;
   float hygro;
};

struct Thermostat{
    float consigne;
    float delta;
    float tempExt;    
    bool interne; 
    bool etat;
    uint8_t mode;
    uint8_t plan;
};

struct Lampe{
    char id[MAX_LEN];
    int numSer;
    int etat;
};
  

#endif