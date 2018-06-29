/*   
*   MCU = Arduino Pro-mini
*   Radio = NRF24L01+
*   NodeID="5Node"
*   Sensor = CTN10
*          
*   RadioId = sensor24ctn10id3*   
*/

#include <LowPower.h>

#define CE_PIN 9
#define CSN_PIN 10
#include "RF24.h"

RF24 radio(CE_PIN, CSN_PIN);

byte address[6] = "5Node";

const int MAX_LEN=20;
unsigned int m_r1=13200;
byte refreshTime=120;
const int probePin=A0;
uint16_t val=0;

typedef struct{
   char id[MAX_LEN]="sensor24ctn10id3\0";
   float temp;
   float humi;
}Sensor;

  Sensor sensor;


void setup() {

   
  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(address); // Write to device address 'SimpleNode'
  radio.setPALevel( RF24_PA_HIGH);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();
  radio.stopListening();
  sensor.temp=-273.0;
  sensor.humi=-1.0;
  
}

void loop(void){  

  measureAndSend();
  for(int i=0;i<refreshTime/4;i++){
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); 
  }
    
  
}


void measureAndSend(){

  ADCSRA |= (1 << ADEN);   
  delay(5);
  val = analogRead(probePin);  
  sensor.temp=rawToDegCTN(val,m_r1,10000,3.3);
  delay(1);    
  radio.setPayloadSize(sizeof(sensor));
  radio.write( &sensor, sizeof(sensor) );
  
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
