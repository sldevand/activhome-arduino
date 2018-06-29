/*
 * Project Name : Bridge  and sensor 
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
#include "Timer.h"
#include "nodeBridge2promini_config.h"

Timer t;
int timerEvent;
long refreshTime=120000; //millisecondes

unsigned int m_r1=1200;
const int probePin=A0;


  Sensor stemp;

void setup() { 


  
  //Serial and pins init
  Serial.begin(9600); 
  while(!Serial);  
  Serial.println("Starting Node Bridge..."); 
  delay(DEFAULT_DELAY);
  sensors24 = new RF24Sensors(CE_PIN, CSN_PIN);  
  delay(SHORT_DELAY);  


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
  
  char id[MAX_LEN] = "sensor24ctn10id4\0";
  for(byte i=0;i<MAX_LEN;i++){
    stemp.id[i]=id[i];
  }
  stemp.temp=-273.0;
  stemp.humi=-1.0;
  
  startTimer(); 

  measureAndSend();
}

void loop(){    
  t.update();
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
void startTimer(){
  timerEvent= t.every(refreshTime,measureAndSend);
}
void stopTimer(){
  t.stop(timerEvent); 
}

void measureAndSend(){
  sensors24->stopListening();
  delay(5);
  uint16_t val = analogRead(probePin);   
  stemp.temp=rawToDegCTN(val,m_r1,10000,3.3);
  delay(1);

  sensors24->radioTransmit(stemp);

  sensors24->startListening();
}

float rawToDegCTN(uint16_t raw,unsigned int r1,unsigned int r0,float V){


  unsigned int B = 4300;
  float temperatureExt=0.0;
  float S=0.0;
  float rT=0.0;
  float T0=298.15;
  float resultln=0.0;
      
        S = (float)(raw*V)/1023.0;
        
        if (S==0){
              
                temperatureExt=-273.0;
        }
        else{
                rT=((r1*V)/S)-r1;  
                resultln =rT/r0;
                temperatureExt = temperatureExt + (1/((log(resultln)/B)+1/T0))-273.15;
        }
        
  return temperatureExt;      
}




