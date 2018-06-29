#include "ThermostatManager.h"
  
void ThermostatManager::init(){

    this->tempActuelle=-273.0;  

     #ifdef FACTORY_RESET
      setDefault();
      delay(10); 

    #ifdef EEPROM_SAVING
      saveInEEPROM();
      delay(10);   
    #endif
  #else
    #ifdef  EEPROM_SAVING
      loadFromEEPROM(); 
      delay(10); 
    #endif
  #endif

  display();
  delay(10);
    
}
void ThermostatManager::setDefault(){
    this->thermostat.consigne=19.00;
    this->thermostat.delta=1.0;
    this->thermostat.tempExt=20.00;
    this->thermostat.interne=true;  
    this->thermostat.mode=1;  
    this->thermostat.plan=1;  
}

int ThermostatManager::saveInEEPROM(){
  Serial.print("- svTherEEP: ");
  int bytesWritten = EEPROM_writeAnything(EEP_THR_IDX,this->thermostat);
  Serial.println(bytesWritten);  
  return bytesWritten;
}

int ThermostatManager::loadFromEEPROM(){

  Serial.print("-ldTherEEP: ");
  int bytesRead = EEPROM_readAnything(EEP_THR_IDX,this->thermostat);  
  Serial.println(bytesRead);  
  return bytesRead;
}

void ThermostatManager::display(){
  #ifdef VERBOSE_MODE   
  
    Serial.print(F("Cons: "));    
    Serial.print( this->thermostat.consigne);   
    Serial.print(F( " Delta: "));       
    Serial.print( this->thermostat.delta);   
    Serial.print(F(" TmpAct: "));  
    Serial.print(this->tempActuelle); 
    Serial.print( F(" Int: "));   
    Serial.print( this->thermostat.interne);     
    Serial.print( F(" Etat: "));   
    Serial.print( this->thermostat.etat); 
    Serial.print( F(" Mod: "));   
    Serial.print( this->thermostat.mode); 
    Serial.print( F(" Plan: "));   
    Serial.println( this->thermostat.plan); 
    
 #endif 
}

void ThermostatManager::limitsTest(){

    if(this->thermostat.consigne < MIN_CONS) this->thermostat.consigne=MIN_CONS;
    if(this->thermostat.consigne > MAX_CONS) this->thermostat.consigne=MAX_CONS;

    if(this->thermostat.delta < MIN_DELTA) this->thermostat.delta=MIN_DELTA;
    if(this->thermostat.delta > MAX_DELTA) this->thermostat.delta=MAX_DELTA;   
}

bool ThermostatManager::hasChanged(Mode mode){

  if( this->thermostat.consigne != mode.consigne ||
      this->thermostat.delta != mode.delta || 
      this->thermostat.mode != mode.id){
      return true;
  }else{
    return false;
  }  
}

//GETTERS
float ThermostatManager::getConsigne(){return this->thermostat.consigne;}
float ThermostatManager::getDelta(){return this->thermostat.delta;}
float ThermostatManager::getTempExt(){return this->thermostat.tempExt;}
bool ThermostatManager::getInterne(){return this->thermostat.interne;}
uint8_t ThermostatManager::getMode(){return this->thermostat.mode;}
uint8_t ThermostatManager::getPlan(){return this->thermostat.plan;}
float ThermostatManager::getTempActuelle(){return this->tempActuelle;}
Thermostat ThermostatManager::getThermostat(){return this->thermostat;}

//SETTERS
void ThermostatManager::setConsigne(float consigne){
    this->thermostat.consigne=consigne;
    limitsTest();
}

void ThermostatManager::setDelta(float delta){
    this->thermostat.delta=delta;
    limitsTest();
}

void ThermostatManager::setTempExt(float tempExt){
    this->thermostat.tempExt=tempExt;
}

void ThermostatManager::setInterne(bool interne){
    this->thermostat.interne=interne;
}

void ThermostatManager::setEtat(bool etat){
    this->thermostat.etat=etat;
}

void ThermostatManager::setMode(uint8_t mode){
    this->thermostat.mode=mode;
    saveInEEPROM();
}

void ThermostatManager::setPlan(uint8_t plan){
    this->thermostat.plan=plan;
    saveInEEPROM();
}

void ThermostatManager::setTempActuelle(float temp){
    this->tempActuelle=temp;
}

void ThermostatManager::setThermostat(Thermostat thermostat){
    
    if(thermostat.mode != this->thermostat.mode || 
      thermostat.plan !=  this->thermostat.plan){
      this->thermostat=thermostat;
      limitsTest();
      saveInEEPROM();
    }else{
      limitsTest();
      this->thermostat=thermostat;
    }
}




