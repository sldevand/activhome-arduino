#define VERSION "0.1.0"
#define CSNCC11_PIN 10
#define MAX_LEN 20
#define SHORT_DELAY 20
#define DEFAULT_DELAY 50
#define LONG_DELAY 200

#include "MyCC1101.h"

/****OBJECTS*****/
MyCC1101 *cc1101;

/****STRUCTURES*****/
struct Sensor{
   char id[MAX_LEN];
   float temp;
   float hygro;
};


Sensor sensor;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.print(F("Conformetre "));
  Serial.print("v");  
  Serial.println(VERSION);
      delay(SHORT_DELAY);

      
 cc1101 = new MyCC1101();
  delay(SHORT_DELAY);
  cc1101->initChacon(14549858);
  delay(LONG_DELAY);
  cc1101->initRF433Sensors();
  delay(LONG_DELAY);
  Serial.println(F("OK!"));
  delay(DEFAULT_DELAY);

 
}

void loop() {

   uint8_t buf[6] = "hello";
     Serial.println("Sending");
        for (int i = 0; i < 5; i++) {
   
          cc1101->RF433Transmit(buf, 20);
        }
    delay(1000);
          Serial.println("Sent");


}
