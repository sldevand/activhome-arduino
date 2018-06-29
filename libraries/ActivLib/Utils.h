#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define MAX_TOKEN_SIZE 9


class Utils
{
	
	public:

		void splitString(char strBuf[],char separator[]);				
		void showTabstring();			
		void decomposeRaw(char*str);

		template <typename T>
		void logv(T str,bool ln){
		  #ifdef SERIAL_PRINT
		    Serial.print(str);
		    if(ln) Serial.println("");
		  #endif
		  
		}

		//MEMBERS
 	
		int m_counter;	
		char* m_tabString[MAX_TOKEN_SIZE];		
};	
#endif
