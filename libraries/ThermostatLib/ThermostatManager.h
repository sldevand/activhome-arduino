#ifndef THERMOSTATMANAGER_H
#define THERMOSTATMANAGER_H

#include"structures.h"
#include "EEPROMAnything.h"

 class ThermostatManager {
    public:
/*****CTOR DTOR*****/
    ThermostatManager(){}
    virtual ~ThermostatManager(){}    
/*****METHODS*******/
    void init();
    void setDefault();
    int saveInEEPROM();
    int loadFromEEPROM();
    void display();
    void limitsTest();
    bool hasChanged(Mode mode);


    //GETTERS
    float getConsigne();
    float getDelta();
    float getTempExt();
    bool getInterne();
    uint8_t getMode();
    uint8_t getPlan();
    float getTempActuelle();
    Thermostat getThermostat();

    //SETTERS
    void setConsigne(float consigne);
    void setDelta(float delta);
    void setTempExt(float tempExt);
    void setInterne(bool interne);
    void setEtat(bool etat);
    void setMode(uint8_t mode);
    void setPlan(uint8_t plan);
    void setTempActuelle(float temp);
    void setThermostat(Thermostat thermostat);


/*****MEMBERS*******/  
    protected:  
    Thermostat thermostat;
    float tempActuelle;
    
};

#endif
