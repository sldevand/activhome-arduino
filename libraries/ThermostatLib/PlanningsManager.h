#ifndef PLANNINGSMANAGER_H
#define PLANNINGSMANAGER_H

#include "EEPROMAnything.h"
#include"structures.h"


 class PlanningsManager {
    public:
/*****CTOR DTOR*****/
    PlanningsManager(){}
    virtual ~PlanningsManager(){}    
/*****METHODS*******/
    void initPlanning();
    void setDefaultPlanning();
    int savePlanningInEEPROM();
    int loadPlanningFromEEPROM();
    void displayPlanning();
    DayPlan getDefaultDayPlan();
    WeekPlan getDefaultWeekPlan();    
   

    DayPlan getDayPlan(uint8_t jour);
    void setDayPlan(DayPlan dp);  

    /*****MEMBERS****/

    WeekPlan weekPlan;
   
 
};

#endif
