    #ifndef TINYDIO_H
    #define TINYDIO_H
     
    #include <Arduino.h>
	
	#define Remote
     
    class TinyDIO {
    public:
	/*****CTOR DTOR*****/
            TinyDIO();
			TinyDIO(int p_pinTx,int p_pinRx,unsigned long p_sender);
            ~TinyDIO(){}
			
	/*****METHODS*******/
				
			void setParameters(int p_pinTx,int p_pinRx,unsigned long p_sender);
			void go(int interruptor,String onoff);
			void sendBit(bool b);
            unsigned long power2(int power);
			void itob(unsigned long integer, int length);
			void itobInterruptor(unsigned long integer, int length);			
			void sendPair(bool b);
			void transmit(int blnOn);
			#ifdef Receiver
			bool listenSignal();	
			#endif
			
			
			
	/*****MEMBERS*******/		
			int pinTx;
			int pinRx;
			bool bit2[26];              // 26 bit Identifiant emetteur
			bool bit2Interruptor[4]; 
			int m_interruptor;
			unsigned long sender ;
			float coeffDelay;			
			
			struct signal_t
			{
			  long sender;
			  int receptor;
			  boolean isSignal;
			  boolean state;
			} receivedSignal;

			//String m_onoff;
           
    };
     
    #endif
