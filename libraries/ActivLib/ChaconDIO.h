#ifndef CHACONDIO_H
#define CHACONDIO_H
#define LISTENER_MODE 

#include <Arduino.h>

class ChaconDIO
{
public:
	/*****CTOR DTOR*****/
	ChaconDIO();
	ChaconDIO(uint8_t p_pinTx, uint8_t p_pinRx, unsigned long p_sender);
	~ChaconDIO();

	/*****METHODS*******/

	void setParameters(uint8_t p_pinTx, uint8_t p_pinRx, unsigned long p_sender);
	void go(int interruptor, byte onoff);
	void go(long sender, int interruptor, byte onoff);
	void sendBit(bool b);
	unsigned long power2(int power);
	void itob(unsigned long integer, int length);
	void itobInterruptor(unsigned long integer, int length);
	void sendPair(bool b);
	void transmit(int blnOn);

#ifdef LISTENER_MODE
	bool listenSignal();
	bool checkSignal();
	unsigned long getPulse();
#endif

	/*****MEMBERS*******/
	uint8_t pinTx;
	uint8_t pinRx;
	bool bit2[26]; // 26 bit Identifiant emetteur
	bool bit2Interruptor[4];
	int m_interruptor;
	unsigned long m_sender;
	float coeffDelay;
	float coeffDelayRcv;
	float increment;
	float tries;

#ifdef LISTENER_MODE
	struct signal_t
	{
		long sender;
		int receptor;
		boolean isSignal;
		boolean state;
	} receivedSignal;
#endif
	//String m_onoff;
};

#endif
