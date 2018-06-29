#define CE_PIN 3
#define CSN_PIN 3 //Since we are using 3 pin configuration we will use same pin for both CE and CSN
#include <avr/wdt.h> 
#include <avr/sleep.h>
#include <avr/power.h>
#include "RF24.h"
#include "DHTXXAt85.h"

RF24 radio(CE_PIN, CSN_PIN);

DHTXXAt85 *dhtxx;

byte address[6] = "1Node";

const int MAX_LEN=20;
const int refreshTime=120;
// Incremented by watchdog interrupt
volatile uint8_t wdt_count;

const byte dataPin=4;
uint16_t val=0;

struct Sensor{
   char id[MAX_LEN];
   float temp;
   float humi;
}sensor;
  
void setup() {
  
  dhtxx = new DHTXXAt85(dataPin);
  String id = "sensor24dht11id1\0";  
  for(int i=0;i<MAX_LEN;i++){sensor.id[i]=id[i];}
   
  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(address);
  radio.setPALevel( RF24_PA_HIGH);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();  
  radio.stopListening();
 
   sensor.temp=-273.0;
   sensor.humi=-1.0;
 
  ACSR  |= _BV(ACD);     // switch Analog Compartaror OFF
  // Configure attiny85 sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Reset watchdog interrupt counter
  wdt_count = 255; //max value  
}

void loop(void){  
  if( wdt_count < refreshTime) goToSleep();
  else{    
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
   delay(100);
   watchdog_stop();   
   delay(10);  
   radio.powerUp(); 
   delay(10);     
}

void measureAndSend(){
  wakeUp();  

   delay(10);
  dhtxx->readValues();
  delay(400);
  sensor.humi=dhtxx->m_humidity;
  sensor.temp=dhtxx->m_temperature-2.0;
  radio.setPayloadSize(sizeof(sensor));
  radio.write( &sensor, sizeof(sensor) );  
  wdt_count = 0;
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

