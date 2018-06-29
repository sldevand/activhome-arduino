//Sonde de Température RF433
#include <avr/wdt.h> 

#include <VirtualWire.h>

#define NOP __asm__ __volatile__ ("nop\n\t")

const uint8_t outPin = 2;
const uint8_t probePin = 2;
const uint8_t MAX_LEN=20;
const uint8_t refreshTime=120;

// Incremented by watchdog interrupt
volatile uint8_t wdt_count;

struct Sensor{
   uint8_t id[MAX_LEN]="sensor43ctn10id1\0";

  union Temp
  {   
    uint8_t valChar[4];
    float valFloat;
  }temp;
 
 };

void setup() {

const uint8_t txPin = 0;
const uint8_t rxPin = 1;

// Serial.begin(9600);
 pinMode(outPin,OUTPUT);
 digitalWrite(outPin,LOW); 
 vw_set_tx_pin(txPin);
 vw_set_rx_pin(rxPin);
 vw_setup(2000); 

 blinkLed(outPin,3);

  watchdog_start_interrupt(6);

    // Reset watchdog interrupt counter
  wdt_count = 255; //max value
 
}

void loop() {
   
if( wdt_count >= refreshTime) { 
    
    measureAndSend();
    wdt_count = 0;
  }
  
   
   
}

void measureAndSend(){
  unsigned int m_r1=12000;
  Sensor sensor;  
  
  uint16_t val = analogRead(probePin);  
    personalDelayMs(5);
  val = analogRead(probePin);
   personalDelayMs(5);  
  sensor.temp.valFloat=rawToDegCTN(val,m_r1,10000,3.3);
   personalDelayMs(5); 
   uint8_t buf[MAX_LEN+4];  

  for(int i=0; i< MAX_LEN;i++) {buf[i] = sensor.id[i];}
  for(int j= MAX_LEN+1;j< MAX_LEN+4;j++){
    buf[j]=sensor.temp.valChar[j];
  }
  
  //radioChainedTransmit(buf,sizeof(buf),1);   
  radioChainedTransmit(sensor.id,sizeof(sensor.id),1);
  radioChainedTransmit(sensor.temp.valChar,sizeof(sensor.temp.valChar),1);
  
 
}

/* Turn off WDT */
void watchdog_stop() {
  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = 0x00;
}
/* Turn onn WDT (with interupt) */
void watchdog_start_interrupt(uint8_t wd_prescaler) {
  if(wd_prescaler > 9) wd_prescaler = 9;
  byte _prescaler = wd_prescaler & 0x7;
  if (wd_prescaler > 7 ) _prescaler |= _BV(WDP3); 
  // ^ fourth bit of the prescaler is somewhere else in the register...
  // set new watchdog timer prescaler value
  WDTCR = _prescaler;

  // start timed sequence
  WDTCR |= _BV(WDIE) | _BV(WDCE) | _BV(WDE);
}


// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  wdt_count++;
  WDTCR |= _BV(WDIE);           // Watchdog goes to interrupt not reset
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


void blinkLed(uint8_t pin,uint8_t nb){
  for(uint8_t i=0;i<nb;i++){
    digitalWrite(pin,HIGH); 
    personalDelayMs(5);
    digitalWrite(pin,LOW);   
    personalDelayMs(5);  
  } 
}

void personalDelayMs(int dl){
 for(int i=0;i<dl;i++){
  personalDelayUs(1000);
  }
  
}

void personalDelayUs(int dl){
  dl=dl/2.13;
  for(int i=0;i<dl;i++){
    NOP;    
  }
}


void radioChainedTransmit(uint8_t *c,uint8_t len,uint8_t num){

for(int i = 0;i<num;i++){
    
     vw_send(c,len);
      vw_wait_tx();
     blinkLed(outPin,1);
     personalDelayUs(200); 
  
    
  }
}







