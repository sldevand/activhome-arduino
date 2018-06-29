/*
   Code pour construction d'un recepteur "maison", recois un signal et ouvre ou ferme un port exterieur (relié par exemple a un relais)
   Fréquence : 433.92 mhz
   Protocole : home easy
   Licence : CC -by -sa
   Matériel associé : Atmega 328 (+résonateur associé) + récepteur RF AM 433.92 mhz + relais + led d'etat
   Auteur : Valentin CARRUESCO  aka idleman (idleman@idleman.fr - http://blog.idleman.fr)

   Basé sur le travail de :
   Barnaby Gray 12/2008
   Peter Mead   09/2009
*/

int recepteurPin = 1;
int relaiPin = 2;
int deviceId = 3;

struct config_t
{
  long sender;
  int receptor;
} signal;

struct signal_t
{
  long sender;
  int receptor;
  boolean isSignal;
  boolean state;
} receivedSignal;


void setup()
{ pinMode(recepteurPin, INPUT);
  pinMode(relaiPin, OUTPUT);
  digitalWrite(relaiPin, HIGH);

}


void loop()
{

  //Ecoute des signaux
  listenSignal();

  //Si un signal au protocol home easy est reçu...
  if (receivedSignal.isSignal) {

    //Récuperation du signal de référence en mémoire interne
    //EEPROM_readAnything(0, signal);
    signal.sender = 14549858;
    signal.receptor=deviceId;

    //Comparaison signal de référence, signal reçu
    if (receivedSignal.sender == signal.sender && receivedSignal.receptor == signal.receptor) {
     // Serial.println("Signal correspondant");

      //On ferme ou on ouvre le relais en fonction du bit d'etat (on/off) du signal
      if (receivedSignal.state)
      {
      //  Serial.println("Etat : on, fermeture du relais");
        digitalWrite(relaiPin, LOW);
      }
      else
      {
      //  Serial.println("Etat : off, ouverture du relais");
        digitalWrite(relaiPin, HIGH);
      }
    
     
    }
  }

}

void listenSignal() {
  receivedSignal.sender = 0;
  receivedSignal.receptor = 0;
  receivedSignal.isSignal = false;

  int i = 0;
  unsigned long t = 0;

  byte prevBit = 0;
  byte bit = 0;

  unsigned long sender = 0;
  bool group = false;
  bool on = false;
  unsigned int recipient = 0;
/*
  // latch 1
  while ((t < 9200 || t > 11350))
  { t = pulseIn(recepteurPin, LOW, 1000000);
  }
  Serial.println("latch 1");
*/
  // latch 2
  while (t < 2400 || t > 3200)
  { t = pulseIn(recepteurPin, LOW, 1000000);
  }

 // Serial.println("latch 2");

  // data
  while (i < 64)
  {
    t = pulseIn(recepteurPin, LOW, 1000000);

    if (t > 100 && t < 700)//200->700
    { bit = 0;

    }
    else if (t > 800 && t < 2000) //1000 -> 2000
    { bit = 1;

    }
    else
    { i = 0;
      break;
    //  Serial.println("bit mort" + t);
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

