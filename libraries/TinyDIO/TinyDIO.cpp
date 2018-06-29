#include "TinyDIO.h"

TinyDIO::TinyDIO(){
	//Serial.begin(9600);	
	//bit2[26]={};              // 26 bit Identifiant emetteur
	//bit2Interruptor[4]={}; 		

}
TinyDIO::TinyDIO(int p_pinTx,int p_pinRx,unsigned long p_sender){
	
	
	setParameters(p_pinTx,p_pinRx,p_sender);
	coeffDelay=1.0f;
}



void TinyDIO::setParameters(int p_pinTx,int p_pinRx,unsigned long p_sender){
		coeffDelay=1.3f;
	pinTx=p_pinTx;	
	pinRx=p_pinRx;	
	sender=p_sender;
	pinMode(pinTx,OUTPUT);
	digitalWrite(pinTx,LOW);
	pinMode(pinRx,INPUT);
}

void TinyDIO::go(int interruptor,String onoff){
	
	
	m_interruptor=interruptor;
	itob(sender,26);            // convertion du code de l'emetteur (ici 8217034) en code binaire
	itobInterruptor(interruptor,4);  
    
	bool boolOnOff=false;
	int retries = 5;
	
	(onoff=="on") ? boolOnOff=true:boolOnOff=false;
  
   for(int i=0;i<retries;i++){
     transmit(boolOnOff);           
              
   }  
	
}

//Envois d'une pulsation (passage de l'etat haut a l'etat bas)
//1 = 310µs haut puis 1340µs bas
//0 = 310µs haut puis 310µs bas
void TinyDIO::sendBit(bool b) {
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

//Calcul le nombre 2^chiffre indiqué, fonction utilisé par itob pour la conversion decimal/binaire
unsigned long TinyDIO::power2(int power){
unsigned long integer=1;
for (int i=0; i<power; i++){
  integer*=2;
}
return integer;
} 

//Convertis un nombre en binaire, nécessite le nombre, et le nombre de bits souhaité en sortie (ici 26)
// Stocke le résultat dans le tableau global "bit2"
void TinyDIO::itob(unsigned long integer, int length)
{
  for (int i=0; i<length; i++){
    if ((integer / power2(length-1-i))==1){
    integer-=power2(length-1-i);
    bit2[i]=1;
    }
    else bit2[i]=0;
  }
}

void TinyDIO::itobInterruptor(unsigned long integer, int length)
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
void TinyDIO::sendPair(bool b) {
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


//Fonction d'envois du signal
//recoit en parametre un booleen définissant l'arret ou la marche du matos (true = on, false = off)
void TinyDIO::transmit(int blnOn)
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
#ifdef Receiver
bool  TinyDIO::listenSignal() {
  receivedSignal.sender = 0;
  receivedSignal.receptor = 0;
  receivedSignal.isSignal = false;
  //receivedSignal.state = false;

  int i = 0;
  unsigned long t = 0;

  byte prevBit = 0;
  byte bit = 0;

  unsigned long sender = 0;
  bool group = false;
  bool on = false;
  unsigned int recipient = 0;
  
    t = pulseIn(pinRx, LOW, 12000);
	
	//Latch1	
	if(t>coeffDelay*8000 && t<coeffDelay*12000){		

	  t = pulseIn(pinRx, LOW, 5000);
	  

	//Latch2 
	if(t>coeffDelay*2400 && t<coeffDelay*4000){
	// data
	
	  while (i < 64)
	  {
		t = pulseIn(pinRx, LOW, 6000);

		if (t > coeffDelay*200 && t < coeffDelay*1000)
		{ bit = 0;

		}
		else if (t > coeffDelay*1000 && t < coeffDelay*2000)
		{ bit = 1;

		}
		else
		{ i = 0;
		  break;
		  
		}

		if (i % 2 == 1)
		{
		  if ((prevBit ^ bit) == 0)
		  { // must be either 01 or 10, cannot be 00 or 11
			i = 0;
			break;
		  }

		  if (i < 53)
		  { // first 26 data bits
			sender <<= 1;
			sender |= prevBit;
		  }
		  else if (i == 53)
		  { // 26th data bit
			group = prevBit;
		  }
		  else if (i == 55)
		  { // 27th data bit
			on = prevBit;
		  }
		  else
		  { // last 4 data bits
			recipient <<= 1;
			recipient |= prevBit;
		  }
		}

		prevBit = bit;
		++i;
	  }


	  // interpret message
	  if (i > 0)
	  {
		receivedSignal.sender = sender;
		receivedSignal.receptor = recipient;
		receivedSignal.isSignal = true;
		if (on)
		{
		  receivedSignal.state = true;
		} else {
		  receivedSignal.state = false;
		}
	  }
  }
  }
  bool res = receivedSignal.state;
 
  return res;
}
#endif

  
  
  






