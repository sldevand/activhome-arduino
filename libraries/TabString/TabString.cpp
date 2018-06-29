#include "TabString.h"

TabString::TabString(){
	
		flushTabString();

}

void TabString::splitString(String str, char separator){	
	Serial.println("m_inputString"+str);
	 for(int i=0;i<str.length();i++){

		  if (str.charAt(i) == separator){  // Handle delimiter
            
			   m_tabString[m_counter] = String(m_inputString);  
			   m_inputString = String("");
			   m_counter++;
			   	  delay(3);
			 
		} else{
		   m_inputString += str.charAt(i); 
		   
		 	  delay(1);
		}
	
		
        
    } 

		
}

void TabString::flushTabString(){
  for(byte i=0;i<MAX_ARRAY_SIZE;i++){
	m_tabString[i]=String("");
	 delay(1);
  }
  m_inputString=String("");
  m_counter=0;
}


void TabString::showTabstring(){
	 for(int i=0;i<MAX_ARRAY_SIZE;i++){
		 Serial.print(i);		 
		 Serial.print(" : ");		 
		 Serial.println(m_tabString[i]);
		 delay(10);
	 }
	
}