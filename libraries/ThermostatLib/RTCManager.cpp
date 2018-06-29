#include "RTCManager.h"

void RTCManager::display(){
#ifdef RTC_DISPLAY   
    Serial.print(dayName[tm.Wday]);
    Serial.write(' ');
    this->print2digits(this->tm.Day);
    Serial.write('/');
    this->print2digits(this->tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(this->tm.Year));
    Serial.write(' ');
    this->print2digits(this->tm.Hour); Serial.write(':');
    this->print2digits(this->tm.Minute);Serial.write(':');    
    this->print2digits(this->tm.Second);
    Serial.print(" Time_t: ");
    Serial.println(  RTC.get());

#endif
}

tmElements_t RTCManager::read(){

    if (RTC.read(this->tm)) {
        return this->tm;
    } else {
        if (RTC.chipPresent()) {
          Serial.println(F("DS1307 stopped"));
          Serial.println();
        } else {
          Serial.println(F("DS1307 read error!"));
          Serial.println();
        }
        delay(1000);
    }
}

bool RTCManager::write(tmElements_t tm){
    tm.Wday = this->computeDayOfWeek(tm.Month,tm.Day,tmYearToCalendar(tm.Year));
    return RTC.write(tm);
}

uint8_t RTCManager::computeDayOfWeek(uint8_t month, uint8_t day, int year){
    uint8_t day_of_week;
  
    if (month < 3){
        month += 12;
        --year;
    }

    day_of_week = day + (13 * month - 27) / 5 + year + year/4 - year/100 + year/400;
    return ((uint8_t)(day_of_week % 7) + 1);
}

void RTCManager::print2digits(int number) {
    if (number >= 0 && number < 10) {
        Serial.print('0');
    }
    Serial.print(number);
}

