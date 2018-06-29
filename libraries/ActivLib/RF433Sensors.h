    #ifndef RF433SENSORS_H
    #define RF433SENSORS_H
     
    #include <Arduino.h>
	#include <VirtualWire.h>
	#include <math.h>
	#include "domusbox_defines.h"
	struct Sensor433{
		
		char id[MAX_LEN];		
		union Temp{	   
			uint8_t valChar[4];
			float valFloat;
		};
		Temp temp;
	};
   
	  
    class RF433Sensors {
    public:
	/*****CTOR DTOR*****/
            RF433Sensors();	
			RF433Sensors(int txPin,int rxPin, int baudRate);				
            ~RF433Sensors();
			
			/*****METHODS****/
			void setupRadio(int txPin,int rxPin, int baudRate);
			void stopRadio();
			void startRadio();			
			bool radioReceive();
			bool radioRawReceive();
			void radioTransmit(uint8_t*buf,uint8_t buflen);						
			void flush433Sensor();				
			
			/*****MEMBERS****/
			int m_txPin;
			int m_rxPin;			
			int m_baudRate;    
			Sensor433 m_sensor;
			bool bufIsOk=false;
			uint8_t* bufRaw;
			
    };
     
    #endif
