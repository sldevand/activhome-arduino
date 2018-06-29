
#include "RF433Sensors.h"

RF433Sensors::RF433Sensors(){
	
	
}

RF433Sensors::RF433Sensors(int txPin,int rxPin, int baudRate){	

	setupRadio(txPin,rxPin,baudRate);	
	
	
}


RF433Sensors::~RF433Sensors(){	
	
}

void RF433Sensors::setupRadio(int txPin,int rxPin, int baudRate){
	
	Serial.begin(9600);
	
	m_txPin=txPin;
	m_rxPin=rxPin;
	m_baudRate=baudRate;	
	vw_set_tx_pin(txPin);
	vw_set_rx_pin(rxPin);
	vw_setup(baudRate);
	startRadio();
	flush433Sensor();
	Serial.println(F("init RF433"));	


}

void RF433Sensors::stopRadio(){	
	vw_rx_stop(); 
	delay(1);
}
void RF433Sensors::startRadio(){	
	vw_rx_start(); 
	delay(1);
}		
bool RF433Sensors::radioReceive(){

	startRadio(); 		


	//Buffer length is set to the maximum that WirtualWire can support.
	uint8_t buflen = VW_MAX_MESSAGE_LEN;
	//Received data buffer with max length.
	uint8_t buf[buflen];  
   
	if(vw_have_message()){ 
		if(vw_get_message(buf,&buflen)){ 		
			 
		
			if(buflen>10){
				flush433Sensor();
				
				for(int i=0;i<MAX_LEN;i++){
					m_sensor.id[i]=buf[i];
				}	
				bufIsOk=false;				
			}
			else if(buflen==4) {
				for(int i=0;i<4;i++) m_sensor.temp.valChar[i]=buf[i];
				bufIsOk=true;
			}
			
						
				 			   
			return true;
		}	
	} 
	
	return false;
		
}

bool RF433Sensors::radioRawReceive(){

	startRadio(); 		


	//Buffer length is set to the maximum that WirtualWire can support.
	uint8_t buflen = VW_MAX_MESSAGE_LEN;
	//Received data buffer with max length.
	uint8_t buf[buflen];  
   
	if(vw_have_message()){ 
		if(vw_get_message(buf,&buflen)){ 		
			 
		
			bufRaw=buf;						
				 			   
			return true;
		}	
	} 
	
	return false;
		
}
void RF433Sensors::radioTransmit(uint8_t*buf,uint8_t buflen){   		
			
		stopRadio();
		vw_send(buf,buflen);
		vw_wait_tx();	
}

void RF433Sensors::flush433Sensor(){
	m_sensor.temp.valFloat=0.0;
 for(int i=0;i<MAX_LEN;i++) m_sensor.id[i]=' ';
	
	
}

			