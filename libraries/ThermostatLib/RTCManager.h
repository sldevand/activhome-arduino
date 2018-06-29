#ifndef RTCMANAGER_H
#define RTCMANAGER_H

#include"structures.h"
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#define RTC_DISPLAY
 class RTCManager {
    public:
/*****CTOR DTOR*****/
    RTCManager(){}
    virtual ~RTCManager(){}    
/*****METHODS*******/
    void display();
    tmElements_t read();
    bool write(tmElements_t tm);
	uint8_t computeDayOfWeek(uint8_t month, uint8_t day, int year);
	void print2digits(int number);
	/*****MEMBERS*******/  
	tmElements_t tm;  
	const char *dayName[7] = 
	{ "Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};

};

#endif
