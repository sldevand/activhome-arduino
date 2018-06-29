#include <IRremote.h>
#include <IRremoteInt.h>
#include <BluetoothAt85.h>

const uint8_t btRxPin = 0;
const uint8_t btTxPin = 2;
const uint8_t irEmitPin = 4;

int speedBt=9600;

const unsigned long RemoteArray[7][4]={  
  {0x2FD18E7,0x2FD30CF,0x2FDA857,0x2FD38C7},
  {0x2FD8877,0x2FDF00F,0x2FDC837,0x2FD08F7},
  {0x2FD807F,0x2FD40BF,0x2FDC03F,0x2FD9867},
  {0x2FD20DF,0x2FDA05F,0x2FD609F,0x2FDF807},
  {0x2FDE01F,0x2FD10EF,0x2FD906F,0x2FDD827},
  {0x2FD50AF,0x2FDD02F,0x2FDE817,0x2FD7887},
  {0x2FDB04F,0x2FD708F,0x2FDB847,0x2FD58A7}
};
char coords[2];
BluetoothAt85 bt;
IRsend My_Sender;

void setup(){
  bt.setup(btRxPin,btTxPin,speedBt);
  pinMode(irEmitPin,OUTPUT);
  digitalWrite(irEmitPin,LOW);  
  coords[0]=-1;
  coords[1]=-1;

}

void loop() {
  
 String btCommand=bt.receive();

  if (btCommand!=""){     
    for(int j=0;j<2;j++){
      coords[j]=btCommand[j]-48;
    }        
  }
  
  if(coords[0]>=0 && coords[0]<=6 && coords[1]>=0 && coords[1]<=3){ 
    My_Sender.sendNEC(RemoteArray[coords[0]][coords[1]],32);    
    coords[0]=-1;coords[1]=-1;
    delay(100);    
   }
}
