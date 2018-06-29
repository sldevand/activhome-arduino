#include <avr/io.h>
#include <avr/sleep.h> 
#include <avr/power.h>

//                +-\/-+
// Vcc      PB5  1|    |8  Vcc
// NC (D 3) PB3  2|    |7  PB2 (D 2) Bp2
// Tx (D 4) PB4  3|    |6  PB1 (D 1) Bp1 
//          GND  4|    |5  PB0 (D 0)NC
//                +----+

unsigned long sender = 14549858;
int pinTx = 4;
int inter1Pin=1,inter2Pin=2;
float coeffDelay=1.0f;  
bool bit2[26];              // 26 bit Identifiant emetteur
bool bit2Interruptor[4]; 
volatile bool state=false;

ISR (PCINT0_vect)
{ 
  state=true;
}

void setup() {

  pinMode(inter1Pin,INPUT);
  pinMode(inter2Pin,INPUT);
  pinMode(pinTx,OUTPUT);
  digitalWrite(pinTx,LOW);  
  PCMSK  |= bit (PCINT2);  
  PCMSK  |= bit (PCINT1); 
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts
  
}

void loop() {
  // put your main code here, to run repeatedly:
    if(state){
      
      if(digitalRead(inter1Pin)){
        
        go(2,true);    
      }
      if(digitalRead(inter2Pin)){
        go(2,false);    
      } 
     
    } else goToSleep();    
}

void goToSleep (){
    state=false;  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
    cli();
    sleep_enable();
    sleep_bod_disable();
    sei();       // Enable interrupts
    sleep_cpu();  
    sleep_disable();   
    sei();   
}  

void go(int interruptor,bool onoff){  
  cli();
 
  itob(sender,26);            // convertion du code de l'emetteur (ici 8217034) en code binaire
  itobInterruptor(interruptor,4);  
    
 
  int retries = 5;
  
  
  
   for(int i=0;i<retries;i++){
     transmit(onoff);           
              
   }  
    state=false;
  sei();
}
void sendBit(bool b) {
 if (b) {
   digitalWrite(pinTx, HIGH);
   delayMicroseconds(275*coeffDelay);   //275 orinally, but tweaked.
   digitalWrite(pinTx, LOW);
   delayMicroseconds(1275*coeffDelay);  //1225 orinally, but tweaked.
 }
 else {
   digitalWrite(pinTx, HIGH);
   delayMicroseconds(275*coeffDelay);   //275 orinally, but tweaked.
   digitalWrite(pinTx, LOW);
   delayMicroseconds(275*coeffDelay);   //275 orinally, but tweaked.
 }
}

unsigned long power2(int power){
unsigned long integer=1;
for (int i=0; i<power; i++){
  integer*=2;
}
return integer;
}

void itob(unsigned long integer, int length)
{
  for (int i=0; i<length; i++){
    if ((integer / power2(length-1-i))==1){
    integer-=power2(length-1-i);
    bit2[i]=1;
    }
    else bit2[i]=0;
  }
}

void itobInterruptor(unsigned long integer, int length)
{
  for (int i=0; i<length; i++){
    if ((integer / power2(length-1-i))==1){
    integer-=power2(length-1-i);
    bit2Interruptor[i]=1;
    }
    else bit2Interruptor[i]=0;
  }
}


//Envoie d'une paire de pulsation radio qui definissent 1 bit réel : 0 =01 et 1 =10
//c'est le codage de manchester qui necessite ce petit bouzin, ceci permet entre autres de dissocier les données des parasites
void sendPair(bool b) {
 if(b)
 {
   sendBit(true);
   sendBit(false);
 }
 else
 {
   sendBit(false);
   sendBit(true);
 }
}

void transmit(int blnOn)
{
 int i;

 // Sequence de verrou anoncant le départ du signal au recepeteur
 digitalWrite(pinTx, HIGH);
 delayMicroseconds(275*coeffDelay);     // un bit de bruit avant de commencer pour remettre les delais du recepteur a 0
 digitalWrite(pinTx, LOW);
 delayMicroseconds(9900*coeffDelay);     // premier verrou de 9900µs*/
 digitalWrite(pinTx, HIGH);   // high again
 delayMicroseconds(275*coeffDelay);      // attente de 275µs entre les deux verrous
 digitalWrite(pinTx, LOW);    // second verrou de 2675µs
 delayMicroseconds(2675*coeffDelay);
 digitalWrite(pinTx, HIGH);  // On reviens en état haut pour bien couper les verrous des données

 // Envoie du code emetteur (272946 = 1000010101000110010  en binaire)
 for(i=0; i<26;i++)
 {
   sendPair(bit2[i]);
 }

 // Envoie du bit définissant si c'est une commande de groupe ou non (26em bit)
 sendPair(false);

 // Envoie du bit définissant si c'est allumé ou eteint 27em bit)
 sendPair(blnOn);

 // Envoie des 4 derniers bits, qui représentent le code interrupteur, ici 0 (encode sur 4 bit donc 0000)
 // nb: sur  les télécommandes officielle chacon, les interrupteurs sont logiquement nommés de 0 à x
 // interrupteur 1 = 0 (donc 0000) , interrupteur 2 = 1 (1000) , interrupteur 3 = 2 (0100) etc...
  for(i=0; i<4;i++)
 {
  
 if(bit2Interruptor[i]==0){
  sendPair(false);
 }else{
  sendPair(true);
 }
}
 
 digitalWrite(pinTx, HIGH);   // coupure données, verrou
 delayMicroseconds(275*coeffDelay);      // attendre 275µs
 digitalWrite(pinTx, LOW);    // verrou 2 de 2675µs pour signaler la fermeture du signal
 //delayMicroseconds(2675*coeffDelay);      // attendre 275µs
}
