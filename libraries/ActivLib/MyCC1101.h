#ifndef MYCC1101_H
#define MYCC1101_H

#include <Arduino.h>

#include <ELECHOUSE_CC1101.h>
#include "ChaconDIO.h"
#include "RF433Sensors.h"

/**
 * RF STATES
 */
enum RFSTATE
{
  RFSTATE_IDLE = 0,
  RFSTATE_RX,
  RFSTATE_TX
};

/**
 * Carrier frequencies
 */
enum CFREQ
{
  CFREQ_868 = 0,
  CFREQ_915,
  CFREQ_433,
  CFREQ_LAST
};

// Carrier frequency = 868 MHz
#define CC1101_DEFVAL_FREQ2_868  0x21        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_868  0x62        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_868  0x76        // Frequency Control Word, Low Byte
// Carrier frequency = 902 MHz
#define CC1101_DEFVAL_FREQ2_915  0x22        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_915  0xB1        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_915  0x3B        // Frequency Control Word, Low Byte
// Carrier frequency = 433 MHz
#define CC1101_DEFVAL_FREQ2_433  0x10        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_433  0xA7        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_433  0x62        // Frequency Control Word, Low Byte

#define RF_TX 0x13
#define RF_RX 0x0D


class MyCC1101 : public ELECHOUSE_CC1101,public ChaconDIO,public RF433Sensors{
	
	public:
	
	MyCC1101();

	virtual ~MyCC1101(){}
	
	//CHACON METHODS
	void initChacon(unsigned long p_sender);
	void configChacon();
	boolean sendDataChacon(long sender,int interruptor,byte data);
	#ifdef LISTENER_MODE
		void startListeningChacon();
	    void readDataChacon();
    #endif
	
	//RF433SENSORS METHODS
	void initRF433Sensors();
	void RF433Transmit(uint8_t*buf,uint8_t buflen);
	
	
	//CC1101 METHODS
	void setCarrierFreq(byte freq);
	void setChannel(byte channel);	
	byte readMARCState();
	void SetTransmit();
	void SetIdle();
	void flushRxFIFO();
	void setTxPin(uint8_t txPin);
	void setRxPin(uint8_t rxPin);	
	
	//MEMBERS
	byte rfState;
	byte carrierFreq;
	
	
	//MEMBERS
	uint8_t m_pinTx;
	uint8_t m_pinRx;
	
	
	
	
};


#endif