#define CE_PIN 8
#define CSN_PIN 9 

#include "RF24.h"

RF24 radio(CE_PIN, CSN_PIN);

byte address[6] = "1Node";

const int MAX_LEN=20;
unsigned int m_r1=12500;


uint16_t val=0;

typedef struct{
   char id[MAX_LEN]="sensor24dht11idX\0";
   float temp;
   float humi;
}Sensor;

  Sensor sensor;


void setup() {

  Serial.begin(9600);
  sensor.humi=-1.00;

   
  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(address); // Write to device address 'SimpleNode'
  radio.setPALevel( RF24_PA_HIGH);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  
  radio.stopListening();
  sensor.temp=-273.0;
  radio.enableDynamicPayloads();
  Serial.print("Fake Temp probe ");
  Serial.print((char*)sensor.id);
  Serial.print(" on ");
  Serial.println((char*)address);

    randomSeed(analogRead(0));
   
}

void loop(void){  

    measureAndSend();
    delay(5000);  
}


void measureAndSend(){

 
  
  //val = 750; 
  //sensor.temp=rawToDegCTN(val,m_r1,10000,3.3);
  
radio.setPayloadSize(sizeof(sensor));
  sensor.temp=random(10.0,30.0);
  delay(1);    
  
  
  radio.write( &sensor, sizeof(sensor) );
  Serial.print("Sending ");
  Serial.print((char*)sensor.id);
  Serial.print(" ");
  Serial.println(sensor.temp);
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
