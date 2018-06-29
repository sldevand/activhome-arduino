/*   
*   MCU = Arduino Pro-mini
*   Radio = NRF24L01+
*   NodeID="2Nodw"
*   Sensor = Teleinfo with SFH
*          
*   RadioId = sensor24tinfoid1  
*/
#include "RF24.h"
#include <SoftwareSerial.h>

#define SDELAY 10

//Pin allocation
#define CE_PIN 9
#define CSN_PIN 10
#define RX_PIN 2

//Special characters
#define startFrame 0x02
#define endFrame 0x03
#define LF 0x0A
#define CR 0x0D

//Sensor id max length
#define MAX_LEN 20

//Comment this line to remove Serial Debug
#define SERIAL_DEBUG

SoftwareSerial* cptSerial;
RF24 radio(CE_PIN, CSN_PIN);

byte address[6] = "2Nodw";

byte valuesOK=1;
bool radioWrite=false;

typedef struct{
   char id[MAX_LEN]="sensor24tinfoid1";
   float base=-273;
   float papp=-273;
}Sensor;

Sensor ti;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete
byte countRadio=0;
const byte countRadioMax=40;


void setup() {
  Serial.begin(115200);
  cptSerial = new SoftwareSerial(2, 3);
  cptSerial->begin(1200);

 
 radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(address); // Write to device address 'SimpleNode'
  radio.setPALevel( RF24_PA_HIGH);
  radio.setChannel(108);
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_250KBPS);     
  sendRadioData();
  Serial.println("Demarrage Teleinfo");
 
   
}

void loop(void){  

 checkTeleinfo();
}


void checkTeleinfo(){
 char charIn = 0;

  while (charIn != startFrame){
    charIn = cptSerial->read() & 0x7F;
  }
  String command = "";
  while (charIn != endFrame)
  {    
    if (cptSerial->available())
    {
      charIn = cptSerial->read()& 0x7F;  
   
      if(charIn != CR && charIn != LF ){
         command+=charIn;       
      }else{
        parseCommand(command);
        command="";
      }
    }
  }
  Serial.println("");  
}

void parseCommand(String com){

  String infoIndex,infoValue,tempValue;
  infoIndex = com.substring(0, com.indexOf(" "));
  tempValue = com.substring(com.indexOf(" ")+1);
  infoValue = tempValue.substring(0, tempValue.indexOf(" "));

  logg(infoIndex + " " + infoValue);

  if(infoIndex=="BASE"){
    valuesOK=1; 
    ti.base=infoValue.toFloat()/1000.0;
  }

  if(infoIndex=="PAPP"){
    valuesOK++; 
    ti.papp=infoValue.toFloat();

  }

  if(valuesOK==2){     
     
    if(countRadio>=countRadioMax){
     sendRadioData();
     countRadio=0;  
    }else{
      countRadio++;
    }
    valuesOK=0;
  }
  
}


void sendRadioData(){
  radio.stopListening();
  radio.setPayloadSize(sizeof(ti));
  radio.write(&ti,sizeof(ti));
  radio.startListening();
}

template<typename T> void logg (T data){

  #ifdef SERIAL_DEBUG
    Serial.println(data);
  #endif
}








