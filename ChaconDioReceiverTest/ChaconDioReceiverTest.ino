#define LISTENER_MODE
#include "domusbox_config.h"
#include "structures.h"
#include "Timer.h"

MyCC1101 *cc1101;
unsigned long t;

void setup() {
  pinMode(CSN_PIN, OUTPUT);
  digitalWrite(CSN_PIN, HIGH);
  pinMode(CSNCC11_PIN, OUTPUT);
  digitalWrite(CSNCC11_PIN, HIGH);

  cc1101 = new MyCC1101();
  delay(SHORT_DELAY);
  cc1101->initChacon(14549858);
  delay(LONG_DELAY);
  cc1101->initRF433Sensors();
  cc1101->startListeningChacon();
  delay(LONG_DELAY);
  digitalWrite(CSNCC11_PIN, HIGH);
  delay(DEFAULT_DELAY);
  t = 0;
  
}

void loop() {

  //  t = cc1101->getPulse();
  //  int i = 0;
  //
  //  if (t > 8000 && t < 12000) {
  //
  //    while (i < 64)
  //    {
  //      t = cc1101->getPulse();
  //      i++;
  //
  //
  //      Serial.println(t);
  //
  //    }
  //  }

  cc1101->listenSignal();

}
