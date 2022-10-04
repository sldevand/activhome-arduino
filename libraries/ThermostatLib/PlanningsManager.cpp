#include "PlanningsManager.h"

void PlanningsManager::initPlanning()
{
#ifdef FACTORY_RESET
	setDefaultPlanning();
	delay(10);
#ifdef EEPROM_SAVING
	savePlanningInEEPROM();
	delay(10);
#endif
#else
#ifdef EEPROM_SAVING
	loadPlanningFromEEPROM();
	delay(10);
#endif
#endif
	displayPlanning();
	delay(10);
}

void PlanningsManager::setDefaultPlanning()
{
	this->weekPlan = getDefaultWeekPlan();
}

int PlanningsManager::savePlanningInEEPROM()
{
	Serial.print("- svPlanEEP: ");
	int bytesWritten = EEPROM_writeAnything(EEP_PLA_IDX, this->weekPlan);
	Serial.println(bytesWritten);
	return bytesWritten;
}

int PlanningsManager::loadPlanningFromEEPROM()
{
	Serial.print("-ldPlanEEP: ");
	int bytesRead = EEPROM_readAnything(EEP_PLA_IDX, this->weekPlan);
	Serial.println(bytesRead);
	return bytesRead;
}

void PlanningsManager::displayPlanning()
{
	Serial.print("-PLAN ");
	Serial.println(this->weekPlan.id);
	for (int j = 0; j < WEEK_LEN; j++)
	{
		DayPlan dp = this->weekPlan.dayPlans[j];
		Serial.print("J:");
		Serial.print(dp.jour);
		Serial.print(" | ");
		for (int k = 0; k < HOUR_PLAN_LEN; k++) {
			Serial.print("H:");
			Serial.print(dp.hourPlans[k].hour);
			Serial.print(" m:");
			Serial.print(dp.hourPlans[k].modeId);
			Serial.print(" | ");
		}	
		Serial.println("");
	}
}

DayPlan PlanningsManager::getDefaultDayPlan()
{
	DayPlan dp;
	dp.jour = 1;

	strcpy(dp.hourPlans[0].hour, "05:00");
	dp.hourPlans[0].modeId=1;
	strcpy(dp.hourPlans[1].hour, "08:00");
	dp.hourPlans[1].modeId=2;
	strcpy(dp.hourPlans[2].hour, "17:00");
	dp.hourPlans[2].modeId=1;
	strcpy(dp.hourPlans[3].hour, "23:00");
	dp.hourPlans[3].modeId=2;
	strcpy(dp.hourPlans[4].hour, "XX:XX");
	dp.hourPlans[4].modeId=3;
	strcpy(dp.hourPlans[5].hour, "XX:XX");
	dp.hourPlans[5].modeId=3;

	return dp;
}

WeekPlan PlanningsManager::getDefaultWeekPlan()
{
	DayPlan dp = getDefaultDayPlan();

	WeekPlan weekPlan;
	weekPlan.id = 1;
	for (int day = 1; day <= WEEK_LEN; day++)
	{
		if (day > 0 && day <= WEEK_LEN)
		{
			dp.jour = day;
			weekPlan.dayPlans[day - 1] = dp;
		}
	}
	return weekPlan;
}

DayPlan PlanningsManager::getDayPlan(uint8_t jour)
{
	DayPlan dp;
	dp.jour = 7;

	if (jour == 0)
		jour = 7;
	for (int day = 0; day < WEEK_LEN; day++)
	{
		if (this->weekPlan.dayPlans[day].jour == jour)
		{
			return this->weekPlan.dayPlans[day];
		}
	}

	return dp;
}

void PlanningsManager::setDayPlan(DayPlan dp)
{
	for (int day = 0; day < WEEK_LEN; day++)
	{
		if (this->weekPlan.dayPlans[day].jour == dp.jour)
		{
			this->weekPlan.dayPlans[day] = dp;
			Serial.println("* dp set * ");
		}
	}
}
