#ifndef TABSTRING_H
#define TABSTRING_H
	

#include <Arduino.h>
#define MAX_ARRAY_SIZE 3

class TabString
{
	
	public:
	
		TabString();
		void splitString(String str,char separator);	
		void flushTabString();	
		void showTabstring();
		
		String m_inputString;
		int m_counter;
		String m_tabString[MAX_ARRAY_SIZE];
	
};



	
#endif
