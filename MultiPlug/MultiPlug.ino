#include <RF24.h>
#define NUMWAYS 9
#define BTDELAY 2

const byte LED_RUN=4,CE_PIN=14,CSN_PIN=10;
byte addressRead[6] = "1Nodw";
byte addressWrite[6] = "2Nodw";

RF24 radio(CE_PIN, CSN_PIN);

String inputString = "";         // a string to hold incoming data

byte wayPins[NUMWAYS]={5,6,7,8,15,16,17,18,19};

struct multipStruct{
    char buf[4];    
}mymultip;

struct Lampe{ 
    byte numWay;
    bool etat;
}lampe;

void setup() { 
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(100);
  pinMode(LED_RUN,OUTPUT);
  
  initRadio();

  blinkLed(4,LED_RUN);

  setAllWays(0); 
}

void loop() { 
if(radioRead()) checkBuffer();
 checkLed();  
}

void setAllWays(byte onoff){
if(onoff>=1)onoff=1;
else onoff=0;
  
 for(byte i=0;i<NUMWAYS;i++){
    pinMode(wayPins[i],OUTPUT);
    digitalWrite(wayPins[i],!onoff);
  }  
}

void checkWays(){

  for(byte i=0;i<NUMWAYS;i++){    
    Serial.print(digitalRead(wayPins[i]));
    delay(BTDELAY);
    Serial.print(' ');
    delay(BTDELAY);
  }  
  Serial.println(' ');
}

void checkBuffer(){

   if(inputString.length()>=4) {

      if(inputString.charAt(0)=='r' && inputString.charAt(1)=='m'){     
       if(inputString.charAt(2)=='a'){
          setAllWays(inputString.charAt(3)-'0');  
          checkWays();      
       }else{ 
          if(inputString.charAt(2)=='c'&&inputString.charAt(3)=='y'){
              checkWays();
            }else{             
              lampe.numWay=inputString.charAt(2)-'0';   
              lampe.etat=inputString.charAt(3)-'0';    
              verifyData();   
            }  
      }  
     inputString = "";
    
    } else {
      Serial.println("Bad Cmd");  
       delay(BTDELAY);
    Serial.println(inputString); 
      }
     
  }else{
    Serial.println("Too Short Cmd"); 
    delay(BTDELAY);
    Serial.println(inputString); 
  }
}

void serialEvent() {
    inputString = "";
    while (Serial.available()){ 
      char inChar = (char)Serial.read();
      delay(BTDELAY);  
      inputString += inChar;       
    }    
      inputString.trim();   
      checkBuffer();
}

bool verifyData(){ 
  if(lampe.numWay>=0 && lampe.numWay<NUMWAYS && lampe.etat>=0 && lampe.etat<=1){
     digitalWrite(wayPins[lampe.numWay],!lampe.etat);    
  }else{
    Serial.println("Bad Way");
  }
}

bool radioRead(){
  if(radio.available()){
    radio.read( &mymultip, sizeof(mymultip));  
    delay(BTDELAY);
    inputString=String(mymultip.buf);    
    inputString.trim(); 
    Serial.println( inputString);       
    return true;      
  }
  return false;
}

void initRadio(){

  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openReadingPipe(1,addressRead);
  radio.openWritingPipe(addressWrite);
  radio.setPALevel( RF24_PA_HIGH);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening(); 
}

/******Run LED Manage******/
unsigned long previousMillis = 0;        // will store last time LED was updated
// constants won't change :
const long interval = 1000;           // interval at which to blink (milliseconds)
bool ledState = LOW; 

void checkLed(){
   // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
     // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
    // set the LED with the ledState of the variable:
    digitalWrite(LED_RUN, ledState);

 
  }
}

void blinkLed(int repeat,int pin){
  for(int i=0;i<repeat;i++){
     digitalWrite(pin,HIGH);
     delay(50);
     digitalWrite(pin,LOW);
     delay(50);
  }
}
