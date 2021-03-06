#define VERSION "0.1.2"
#define CSNCC11_PIN 10
#define MAX_LEN 20
#define SHORT_DELAY 20
#define DEFAULT_DELAY 50
#define LONG_DELAY 200

#include "MyCC1101.h"
#include <DHT.h>
#include <DHT_U.h>
#include <LowPower.h>

#define DHTPIN 8
#define DHTTYPE DHT22
#define REF_TIME 120

/****OBJECTS*****/
MyCC1101 *cc1101;
DHT dht(DHTPIN, DHTTYPE);

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

  char id[MAX_LEN] = "sensor43dht22id1\0";

  for (int i = 0; i < sizeof(id) ; i++ ) {
    sensor.id[i] = id[i];
  }
  sensor.temp = -273.2;
  sensor.hygro = 2.2;

  dht.begin();
}

void loop() {
  sensor.temp = dht.readTemperature();
  sensor.hygro = dht.readHumidity();
  if (isnan( sensor.temp) || isnan(sensor.hygro)) {
    return;
  }

  delay(SHORT_DELAY);
  cc1101->initChacon(14549858);
  cc1101->setChannel(0x01);
  delay(LONG_DELAY);
  cc1101->RF433Transmit((uint8_t*) &sensor, sizeof(sensor));
  delay(1000);
  cc1101->SetIdle();
  for (int i = 0; i < REF_TIME / 4; i++) {
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  }
}
