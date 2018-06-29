/*
 * Project Name : Bridge 
 * By : Sébastien LORRAIN
 * Date : 12/10/2017
 * Version : 2.0.2
 * Description : 
 * Pont nrf24l01 entre PiAssist et nodes 
 *  a) Addresses en entrée :
 *      1Node à 5Node
 *  b) Addresse de sortie :
 *      1Mast (Master)
 * ------------------------------------------------------------------------------------------------
 
 */

#include "nodeBridge1_config.h"
struct Sensor{      
    char id[MAX_LEN];
    float temp;
    float humi;
  };


void setup() { 

  pinMode(CSN_PIN,OUTPUT);

  digitalWrite(CSN_PIN,HIGH);

  
  //Serial and pins init
  Serial.begin(9600); 
  while(!Serial);  
  Serial.println("Starting Node Bridge..."); 
  delay(DEFAULT_DELAY);
  sensors24 = new RF24Sensors(CE_PIN, CSN_PIN);  
  delay(SHORT_DELAY);  

  digitalWrite(CSNCC11_PIN,HIGH);
  Serial.println("<--  Node Bridge Started!-->");
  delay(DEFAULT_DELAY); 

  sensors24->m_writeAdress[0]="1Mast";
  sensors24->openWritingPipe(sensors24->m_writeAdress[0]);
  sensors24->m_readAdress[0]="1Node";
  sensors24->m_readAdress[1]="2Node";
  sensors24->m_readAdress[2]="3Node";
  sensors24->m_readAdress[3]="4Node";
  sensors24->m_readAdress[4]="5Node";

  for(byte i=0;i<5;i++){
  
    sensors24->openReadingPipe(i+1,sensors24->m_readAdress[i]);
  }

  

 
}

void loop(){    
  
  if(sensors24->radioReceive()>=SENSOR_PAYLOAD){
      String mergedString=sensors24->m_sensor.id;
    mergedString+=" ";
    mergedString+=String(sensors24->m_sensor.temp);

    if(sensors24->m_sensor.humi>=0){  
      mergedString+=" ";
      mergedString+=String(sensors24->m_sensor.humi);      
    }  
    
    Serial.println(mergedString);
    sensors24->radioTransmit(sensors24->m_sensor);
  }
 
}




