#include <Bluetooth.h>

Bluetooth::Bluetooth(){
	setup();
}

Bluetooth::Bluetooth(int rxPin,int txPin){
	
	m_rxPin=rxPin;m_txPin=txPin;
	setup();	
}

Bluetooth::Bluetooth(long baudRate){
	
	m_baudRate=baudRate;
	setup();	
}
Bluetooth::Bluetooth(int rxPin,int txPin,long baudRate){
	
	m_rxPin=rxPin;m_txPin=txPin;m_baudRate=baudRate;
	setup();	
}
Bluetooth::Bluetooth(int rxPin,int txPin,int atPin,long baudRate){
	
	m_rxPin=rxPin;m_txPin=txPin;m_baudRate=baudRate;m_atPin=atPin;	
	setup();	
}

Bluetooth::Bluetooth(int rxPin,int txPin,int atPin,int powerPin,long baudRate){
	
	m_rxPin=rxPin;m_txPin=txPin;m_atPin=atPin;m_powerPin=powerPin;m_baudRate=baudRate;	
	setup();	
}

void Bluetooth::setup(){
	
    Serial.begin(9600);	

	pinMode(m_atPin,OUTPUT);
	digitalWrite(m_atPin,m_atMode);	
	
	m_masterRole=true;
	   
	powerOn();
	
	blueTooth = new SoftwareSerial(m_rxPin, m_txPin);
	delay(20);
	blueTooth->begin(m_baudRate);
	delay(50);
	Serial.println(F("init Bt..."));
	delay(3);
	m_serialPrint=false;
}

void Bluetooth::log(const char* data){
	
	char* dataBuf=(char*)data;
	
	if(m_serialPrint)Serial.print((char*)dataBuf);
	if(!m_masterRole){send(dataBuf);}	
}

void Bluetooth::logInt(const int data){
	
	if(m_serialPrint) Serial.print(data,DEC);
	
	if(!m_masterRole){
		String str;
		str+=data;
		char charBuf[10];
		str.toCharArray(charBuf,10);
		send(charBuf);
	}
	
}

void Bluetooth::logString(const String data){
	if(m_serialPrint) Serial.print(data);
	
	
	if(!m_masterRole){
		char buf[60];
		data.toCharArray(buf,60);	
		send(buf);	
	}
}

void Bluetooth::logFloat(const float data){
	if(m_serialPrint) Serial.print(data,2);
	
	if(!m_masterRole){
		String str;
		str+=data;
		char charBuf[10];
		str.toCharArray(charBuf,10);
		send(charBuf);
	}
	
	
}

void Bluetooth::CR(){
	if(m_serialPrint) Serial.println("");
	if(!m_masterRole)send("\r\n");
}

void Bluetooth::setRxPin(int rxPin){
	m_rxPin=rxPin;
	restart();
}
void Bluetooth::setTxPin(int txPin){
	m_txPin=txPin;
	restart();
}
void Bluetooth::setAtPin(int atPin){
	m_atPin=atPin;
	restart();
}
void Bluetooth::setPowerPin(int powerPin){
	m_powerPin=powerPin;
	restart();
}
void Bluetooth::setBaudRate(long baudRate){
	m_baudRate=baudRate;
	restart();
}

int Bluetooth::getRxPin(){return m_rxPin;}
int Bluetooth::getTxPin(){return m_txPin;}
int Bluetooth::getAtPin(){return m_atPin;}
int Bluetooth::getPowerPin(){return m_powerPin;}
long Bluetooth::getBaudRate(){return m_baudRate;}
bool Bluetooth::getAtMode(){return m_atMode;}

void Bluetooth::restart(){

	log("Bt rstrt");	
	delay(3);
	delete blueTooth;
	digitalWrite(m_powerPin,LOW);  
	delay(400);
	setup();
	
}


void Bluetooth::send(char* data){
		
		blueTooth->write(data);
		blueTooth->flush();
		

}
void Bluetooth::sendChar(char data){
		
		blueTooth->write(data);

}

void Bluetooth::sendInt(int data){
		
		blueTooth->write(data);

}

String Bluetooth::receive(){
	
	m_buffer="";
	m_lgt=0;
	
	while (blueTooth->available()) {
        char tmp=blueTooth->read();
        delay(5);          
		m_buffer+=tmp;  
	    m_lgt++;
       
    }		
	
	return m_buffer;
}

void Bluetooth::powerOn(){
	
	if(m_serialPrint) Serial.println(F("Bt Pwr On"));
	
	pinMode(m_powerPin,OUTPUT);
	digitalWrite(m_powerPin,HIGH); 
}

void Bluetooth::powerOff(){	
	
	Serial.println(F("Bt Pwr Off"));
	digitalWrite(m_powerPin,LOW);  	
	
}
#ifdef AT_MODE_ON
void Bluetooth::setAtMode(bool mode){
	
	m_atMode=mode;
	
	digitalWrite(m_atPin,mode);
	
	if(mode) {
		Serial.println("Bt AT");
		m_previousBaudRate=m_baudRate;
		m_baudRate=38400;		
	}
	else {
		Serial.println("Bt Normal");
		m_baudRate=m_previousBaudRate;
	}
	delay(5);
	restart();
	
	
}


void Bluetooth::setRole(bool role){	

	

	if(role){
		blueTooth->write("at+role=1\r\n");		
	}else{
		blueTooth->write("at+role=0\r\n");			
	}
		m_masterRole=role;
		
	
	
}




void Bluetooth::getRole(){
	blueTooth->write("at+role\r\n");	

}

 void Bluetooth::getState(){
	blueTooth->write("at+state\r\n");	 
 }
 
  void Bluetooth::getName(){
	blueTooth->write("at+name\r\n");
	delay(5);
	 
 }
 
  void Bluetooth::getAddr(){
	blueTooth->write("at+addr\r\n");
	delay(5);
	 
 }
 
 

void Bluetooth::reset(){
	blueTooth->write("at+reset\r\n");
	log("Bt rst");
	delay(5);
}

void Bluetooth::initSPPLib(){

	blueTooth->write("at+init\r\n");
	Serial.println("SPP Init");
	delay(100);
}


void Bluetooth::inquireDevicesAround(){
	
	blueTooth->write("at+inq\r\n");
	Serial.println("Bt inq");
	delay(100);
}

 
 void Bluetooth::pairWithDevice(String address,int time){
	 
	 String timeStr = String(time);
	 blueTooth->print("at+pair="+address+","+timeStr+"\r\n");	 
	
	Serial.print("Bt Pair = ");
	Serial.println(address);
		
	 
	 delay(50);
	 
 }

 void Bluetooth::atModeToString(){
	  
	if(m_serialPrint) Serial.println("<-Bt At->");		
	getName();	
	getAddr();
	getRole();
	getState();	
	
	  
  }

#endif

  
  void Bluetooth::toString(){
	  
	m_serialPrint=true;
	log("<-Bt Infos->");
	CR();
	delay(2);
	log("RxPin=");	
	logInt(getRxPin());
	CR();
	delay(2);
	log("TxPin=");
	logInt(getTxPin());
	CR();
	delay(2);
	log("AtPin=");
	logInt(getAtPin());
	CR();
	delay(2);
	log("PwrPin=");
	logInt(getPowerPin());
	CR();
	delay(2);
	log("BdRate=");
	logInt(getBaudRate());
	CR();
	delay(2);
	log("<------->");
	CR();
	delay(2); 
	m_serialPrint=false;
	  
  }
  
   



