#include "Utils.h"

void Utils::splitString(char strBuf[], char separator[]){	
	 
	int i=0;
	char * pch;
	pch = strtok (strBuf,separator);
	m_tabString[i]=pch;
	i++;
	while (pch != NULL)	{
		pch = strtok (NULL, separator);
		m_tabString[i]=pch;	
		i++;
	}
	delete pch;
	m_counter=i-1;
}	

void Utils::showTabstring(){
	 for(int i=0;i<m_counter;i++){		 
	 	Serial.println(m_tabString[i]);		
		delay(10);
	 }
}

void Utils::decomposeRaw(char*str){
	#ifdef SERIAL_PRINT

		for(int j=0;j<strlen(str);j++){
	      Serial.print(str[j],HEX);     
	      Serial.print(" ");  
    	}  

    #endif
}


