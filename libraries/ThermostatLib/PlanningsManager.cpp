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
		for (int k = 0; k < HOUR_PLAN_LEN; k++)
		{
			Serial.print("H:");
			Serial.print(dp.hourPlans[k].minute);
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

	dp.hourPlans[0].minute = 300;
	dp.hourPlans[0].modeId = 1;
	dp.hourPlans[1].minute = 480;
	dp.hourPlans[1].modeId = 2;
	dp.hourPlans[2].minute = 1020;
	dp.hourPlans[2].modeId = 1;
	dp.hourPlans[3].minute = 1380;
	dp.hourPlans[3].modeId = 2;
	for (int i = 4; i < HOUR_PLAN_LEN; i++) {
		dp.hourPlans[i].minute = 1999;
		dp.hourPlans[i].modeId = 0;
	}

	return dp;
}

WeekPlan PlanningsManager::getDefaultWeekPlan()
{
	DayPlan dp = getDefaultDayPlan();

	WeekPlan weekPlan;
	weekPlan.id = 1;
	for (int day = 1; day <= WEEK_LEN; day++) {
		if (day > 0 && day <= WEEK_LEN) {
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
	for (int day = 0; day < WEEK_LEN; day++) {
		if (this->weekPlan.dayPlans[day].jour == jour) {
			return this->weekPlan.dayPlans[day];
		}
	}

	return dp;
}

bool PlanningsManager::setDayPlan(DayPlan dp)
{
	for (int day = 0; day < WEEK_LEN; day++) {
		if (this->weekPlan.dayPlans[day].jour == dp.jour) {
			for (int i = 0; i < HOUR_PLAN_LEN; i++) {
				this->weekPlan.dayPlans[day].hourPlans[i].minute = 1999;
				this->weekPlan.dayPlans[day].hourPlans[i].modeId = 0;
			}
			this->weekPlan.dayPlans[day] = dp;
			return true;
		}
	}

	return false;
}
