
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

Sensor RF433Sensors::radioRawReceive() {
	startRadio();
    Sensor sensor;
    byte sensorSize = sizeof(Sensor);
	if(hasMessage()) {
		vw_get_message((byte *) &sensor, &sensorSize);
        return sensor;
	}    
}

bool RF433Sensors::hasMessage() {
    return vw_have_message();
}

void RF433Sensors::radioTransmit(uint8_t*buf,uint8_t buflen) {   		
			
		stopRadio();
		vw_send(buf,buflen);
		vw_wait_tx();	
}

			
