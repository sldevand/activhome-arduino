#include "Utils.h"
#define MAX_SIZE 50

Utils utils;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(20);


}

void loop() {
  char m_inputString[]="cc1101/chacon/14549858/2/1/18/44/55/66";
  utils.splitString(m_inputString,"/");

  utils.showTabstring();
  utils.showSizeOfTab();
  delay(1000);
  // put your main code here, to run repeatedly:

}




