#ifndef MODESMANAGER_H
#define MODESMANAGER_H

#include"structures.h"
#include "EEPROMAnything.h"

#define MODE_NOT_FOUND 20

 class ModesManager {
    public:
/*****CTOR DTOR*****/
    ModesManager(){}
    virtual ~ModesManager(){}    
/*****METHODS*******/
    void initModes();
    void setDefaultModes();
    int saveModesInEEPROM();
    int loadModesFromEEPROM();
    void displayModes();
    Mode getMode(uint8_t id);    
    uint8_t getModePos(uint8_t id);
    void addMode(Mode md);
    Mode limitsTest(Mode mode);


/*****MEMBERS*******/    
    Mode modes[MODES_LEN];
    uint8_t mode_iter=0;
};

#endif
