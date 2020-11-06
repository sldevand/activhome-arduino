#define VERSION "0.1.0"
#include "domusbox_config.h"
#include "structures.h"
#include "Timer.h"

/****OBJECTS*****/
MyCC1101 *cc1101;

void setup() {
  pinMode(CSNCC11_PIN, OUTPUT);
  digitalWrite(CSNCC11_PIN, HIGH);

  //Serial and pins init
  Serial.begin(9600);
  while (!Serial);
  Serial.print(F("TestCC1101Sensors "));
  Serial.print("v");
  Serial.println(VERSION);
  delay(SHORT_DELAY);;

        
 cc1101 = new MyCC1101();
  delay(SHORT_DELAY);
  cc1101->initChacon(14549858);
  delay(LONG_DELAY);
  cc1101->initRF433Sensors();
  delay(LONG_DELAY);
  cc1101->SetReceive();
  Serial.println(F("OK!"));
  delay(DEFAULT_DELAY);
}

void loop() {
   if (cc1101->radioReceive()) {

    Serial.println((char*)cc1101->bufRaw);
  }
}
