#include <BluetoothAT8.h>

BluetoothAt8::BluetoothAt8(){
	setup();
}

BluetoothAt8::BluetoothAt8(int rxPin,int txPin){
	
	m_rxPin=rxPin;m_txPin=txPin;
	setup();	
}

BluetoothAt8::BluetoothAt8(long baudRate){
	
	m_baudRate=baudRate;
	setup();	
}
BluetoothAt8::BluetoothAt8(int rxPin,int txPin,long baudRate){
	
	m_rxPin=rxPin;m_txPin=txPin;m_baudRate=baudRate;
	setup();	
}

void BluetoothAt8::setup(){
	
    Serial.begin(9600);	
	
	bluetooth = new SoftwareSerial(m_rxPin, m_txPin);
	delay(20);
	bluetooth->begin(m_baudRate);
	delay(50);
	Serial.println("init Bt...");
	delay(3);
	m_serialPrint=false;
}

void BluetoothAt8::log(const char* data){
	
	char* dataBuf=(char*)data;
	
	if(m_serialPrint)Serial.print((char*)dataBuf);
	send(dataBuf);	
}

void BluetoothAt8::logInt(const int data){
	
	if(m_serialPrint) Serial.print(data,DEC);
	String str;
	str+=data;
	char charBuf[10];
	str.toCharArray(charBuf,10);
	send(charBuf);
	
	
	
}

void BluetoothAt8::logString(const String data){
	if(m_serialPrint) Serial.print(data);
	char buf[60];
	data.toCharArray(buf,60);	
	send(buf);	

}

void BluetoothAt8::logFloat(const float data){
	if(m_serialPrint) Serial.print(data,2);
	String str;
	str+=data;
	char charBuf[10];
	str.toCharArray(charBuf,10);
	send(charBuf);
	
	
}

void BluetoothAt8::CR(){
	if(m_serialPrint) Serial.println("");
	send("\r\n");
}

void BluetoothAt8::send(char* data){
		
		bluetooth->write(data);
		bluetooth->flush();
		

}
void BluetoothAt8::sendChar(char data){
		
		bluetooth->write(data);

}

void BluetoothAt8::sendInt(int data){
		
		bluetooth->write(data);

}

String BluetoothAt8::receive(){
	
	m_buffer="";
	m_lgt=0;
	
	while (bluetooth->available()) {
        char tmp=bluetooth->read();
        delay(5);          
		m_buffer+=tmp;  
	    m_lgt++;
       
    }		
	
	return m_buffer;
}
