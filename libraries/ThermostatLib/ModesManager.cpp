#include "ModesManager.h"

void ModesManager::initModes(){
   #ifdef FACTORY_RESET
      setDefaultModes();
      delay(10); 

    #ifdef EEPROM_SAVING
      saveModesInEEPROM();
      delay(10);   
    #endif
  #else
    #ifdef  EEPROM_SAVING
      loadModesFromEEPROM(); 
      delay(10); 
    #endif
  #endif

  displayModes();
  delay(10);
}

void ModesManager::setDefaultModes(){

  float consignes[MODES_LEN]={21.0,18.0,19.0,9.0};
  float deltas[MODES_LEN]={0.5,1.0,0.5,1.0};
  for(byte i=0;i<MODES_LEN;i++){
    this->modes[i].id=i+1; 
    this->modes[i].consigne=consignes[i];
    this->modes[i].delta=deltas[i];
  } 

}
int ModesManager::saveModesInEEPROM(){
  Serial.print("-svModEEP: ");
  int bytesWritten = EEPROM_writeAnything(EEP_MOD_IDX, this->modes);
  Serial.println(bytesWritten);  
  return bytesWritten;
}

int ModesManager::loadModesFromEEPROM(){
  Serial.print("-ldModEEP: ");
  int bytesRead = EEPROM_readAnything(EEP_MOD_IDX, this->modes);  
  Serial.println(bytesRead);  
  return bytesRead;
}

void ModesManager::displayModes(){  
  Serial.println("-Mod-");
  for(byte i=0;i<MODES_LEN;i++){
    Serial.print("ID : ");
    Serial.print(this->modes[i].id);
    Serial.print(" cons: ");
    Serial.print(this->modes[i].consigne);
    Serial.print(" delta: ");
    Serial.println(this->modes[i].delta); 
  }
  Serial.println("-End-");  
}

Mode ModesManager::getMode(uint8_t id){
  
  for(byte i=0;i<MODES_LEN;i++){
    if(this->modes[i].id==id){
 
      return this->modes[i];
    }
  }

  Mode mode;
  mode.id=0;
  mode.consigne=0;
  mode.delta=0;
  return mode;
}

uint8_t ModesManager::getModePos(uint8_t id){
  
  for(byte i=0;i<MODES_LEN;i++){
    if(this->modes[i].id==id){
      return i;
    }
  }
  return MODE_NOT_FOUND;
}

void ModesManager::addMode(Mode md){

  Mode mode = limitsTest(md);

  if(mode_iter>=MODES_LEN) mode_iter=0;
    uint8_t pos = getModePos(mode.id);
  if(pos==MODE_NOT_FOUND){
    this->modes[mode_iter]=mode;
    mode_iter++;
  }else{

    this->modes[pos]=mode;
  }
  
}

Mode ModesManager::limitsTest(Mode mode){
  if(mode.consigne < MIN_CONS) mode.consigne=MIN_CONS;
  if(mode.consigne > MAX_CONS) mode.consigne=MAX_CONS;

  if(mode.delta < MIN_DELTA) mode.delta=MIN_DELTA;
  if(mode.delta > MAX_DELTA) mode.delta=MAX_DELTA;  
  return mode;
}


  

