#include "PlanningsManager.h"

void PlanningsManager::initPlanning(){

	#ifdef FACTORY_RESET
      setDefaultPlanning();
      delay(10); 
	    #ifdef EEPROM_SAVING
	      savePlanningInEEPROM();
	      delay(10);   
	    #endif
  	#else
	    #ifdef  EEPROM_SAVING
	      loadPlanningFromEEPROM(); 
	      delay(10); 
	    #endif
  	#endif	

  	displayPlanning();
  	delay(10);
  	
}

void PlanningsManager::setDefaultPlanning(){
	this->weekPlan=getDefaultWeekPlan();	
}

int PlanningsManager::savePlanningInEEPROM(){

  Serial.print("- svPlanEEP: ");
  int bytesWritten = EEPROM_writeAnything(EEP_PLA_IDX,this->weekPlan);
  Serial.println(bytesWritten);  
  return bytesWritten;
}

int PlanningsManager::loadPlanningFromEEPROM(){

  Serial.print("-ldPlanEEP: ");
  int bytesRead = EEPROM_readAnything(EEP_PLA_IDX,this->weekPlan);  
  Serial.println(bytesRead);  
  return bytesRead;
  
}

void PlanningsManager::displayPlanning(){

	Serial.print("-PLAN ");
	Serial.println(this->weekPlan.id);
	for(int j=0;j<WEEK_LEN;j++){
		DayPlan dp = this->weekPlan.dayPlans[j];
		Serial.print("J : ");
		Serial.print(dp.jour);
		Serial.print(" mId : ");
		Serial.print(dp.modeId);
		Serial.print(" dfMId : ");
		Serial.print(dp.defaultModeId);
		Serial.print(" h1Stt : ");
		Serial.print(dp.heure1Start);
		Serial.print(" h1Stp : ");
		Serial.print(dp.heure1Stop);
		Serial.print(" h2Stt : ");
		Serial.print(dp.heure2Start);
		Serial.print(" h2Stp : ");
		Serial.println(dp.heure2Stop);
	}
	
  
}

DayPlan PlanningsManager::getDefaultDayPlan(){
		DayPlan dp;
		dp.jour=1;
		dp.modeId=1;
		dp.defaultModeId=2;
		strcpy(dp.heure1Start,"05:00");
		strcpy(dp.heure1Stop,"08:00");
		strcpy(dp.heure2Start,"17:00");
		strcpy(dp.heure2Stop,"23:00");

		return dp;
}

WeekPlan PlanningsManager::getDefaultWeekPlan(){

	DayPlan dp = getDefaultDayPlan();

	WeekPlan weekPlan;
	weekPlan.id=1;
  	for(int day=1;day<=WEEK_LEN;day++){
		if(day>0 && day<=WEEK_LEN){
		dp.jour=day;
		weekPlan.dayPlans[day-1]=dp;
		}  
	}
  	return weekPlan;
} 


DayPlan PlanningsManager::getDayPlan(uint8_t jour){
	
	DayPlan dp;	
	dp.jour=0;

	for(int day=0;day<WEEK_LEN;day++){
		if(this->weekPlan.dayPlans[day].jour==jour){
			return this->weekPlan.dayPlans[day];
		}
	}	
	return dp;
}

void PlanningsManager::setDayPlan(DayPlan dp){   

	for(int day=0;day<WEEK_LEN;day++){
		if(this->weekPlan.dayPlans[day].jour==dp.jour){
			this->weekPlan.dayPlans[day]=dp;
			Serial.println("* dp set * ");
		}
	}	
}



