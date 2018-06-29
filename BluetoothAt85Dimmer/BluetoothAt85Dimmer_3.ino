//  by Brandon Plumbo AKA Riftliger
//  original version from Techbitar http://www.instructables.com/id/Modify-The-HC-05-Bluetooth-Module-Defaults-Using-A/step1/Components-Wiring/, and Phillipe Cantin http://phillipecantin.blogspot.com/2014/08/hc-05-bluetooth-link-with-zero-code.html
//  HC-05 universal test sketch to enter AT-commands
//  Uses hardware serial to talk to the host computer and software serial for communication with the bluetooth module
//
//  Pins
//  BT VCC to Arduino 5V out.
//  BT GND to GND
//  Arduino D11 to BT RX
//  Arduino D10 BT TX
//  Key Pin D9
//
//  When a command is entered in the serial monitor on the computer
//  the Arduino will relay it to the bluetooth module and display the result.
//
//  The HC-05 modules require both CR and NL

#include<SoftwareSerial.h>
#include <EEPROMAnything.h>

SoftwareSerial BTSerial(1, 2);
const uint8_t pwmPin = 0;

const byte countMax=5;
byte count=0;
int value=0;
void setup() {
 
  BTSerial.begin(9600);
  EEPROM_readAnything(0,value); 
  pinMode(pwmPin,OUTPUT);
  digitalWrite(pwmPin,LOW);
  analogWrite(pwmPin,value);

}

void loop() {
  
  // Read from the Bluetooth module and send to the Arduino Serial Monitor
  while (BTSerial.available()) {    
    value=BTSerial.read(); 
      if(value>255) value=255;  
      if(value<0) value=0;               
      analogWrite(pwmPin,value);
      if(count<countMax){
        count++;
      }else{
        count=0;
        EEPROM_writeAnything(0,value);  
     
      }  
} 
  
/*
  // Read from the Serial Monitor and send to the Bluetooth module
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }*/
}
