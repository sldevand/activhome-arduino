#define CE_PIN 3
#define CSN_PIN 3 //Since we are using 3 pin configuration we will use same pin for both CE and CSN
#include <avr/wdt.h> 
#include <avr/sleep.h>
#include <avr/power.h>
#include "RF24.h"

RF24 radio(CE_PIN, CSN_PIN);

byte address[6] = "2Node";

const int MAX_LEN=20;
unsigned int m_r1=12800;
const int refreshTime=120;
// Incremented by watchdog interrupt
volatile uint8_t wdt_count;


const int probePin=A2;
uint16_t val=0;

typedef struct{
   char id[MAX_LEN]="sensor24ctn10id1\0";
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
  sensor.humi=-1.00;

 
  ACSR  |= _BV(ACD);     // switch Analog Compartaror OFF
  // Configure attiny85 sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Reset watchdog interrupt counter
  wdt_count = 255; //max value
  
}

void loop(void){  

  
  if( wdt_count < refreshTime) goToSleep();
  else{  
    wakeUp();
    measureAndSend();
    wdt_count = 0;
  }
}

void goToSleep(){
  radio.powerDown();
//  ADCSRA &= ~_BV(ADEN);  // switch ADC OFF
  ADCSRA &= ~(1<<ADEN);  
  watchdog_start_interrupt(6);      // prescale of 6 ~= 1sec
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();

  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
 
  
}

void wakeUp(){
   wdt_count = 0;  
   watchdog_stop();     
   radio.powerUp(); 
   
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
