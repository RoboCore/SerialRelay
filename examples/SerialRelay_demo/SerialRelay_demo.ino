/*
      RoboCore - Serial Relay example
                    (20/05/2015)

  Written by François.
  
  Examples of how to use the Serial Relay library.
  
  
  Note: this example requires the String Functions
        v1.5 library (available at
        https://github.com/RoboCore/String_Functions)

*/

//#include <SoftwareSerial.h>
//void PCINT_Extension2(void){}

#include <String_Functions.h>
#include "SerialRelay.h"


SerialRelay relays(4,5,8); // (data, clock)
byte module = 1;

#define BUFFER_SIZE 3
char buffer[BUFFER_SIZE];

void setup(){
  Serial.begin(19200);
  relays.SetModule(0x00,1,false);
  relays.SetModule(0x55,2);
  relays.Info(&Serial);
}

void loop(){
  
  if(ReadFromSerial(&Serial, buffer, BUFFER_SIZE, '#')){
    
    if((buffer[0] == 'm') || (buffer[0] == 'M')){
      if((buffer[1] >= '1') && (buffer[1] <= '9')){
        module = buffer[1] - '0';
        Serial.print("Module ");
        Serial.println(module);
      }
    }
    
    if(buffer[0] == 'i')
      relays.Info(&Serial);
    
    
    if((buffer[0] == '+') || (buffer[0] == '-')){
      if((buffer[1] >= '1') && (buffer[1] <= '8')){
        byte state = SERIAL_RELAY_OFF;
        if(buffer[0] == '+'){
          state = SERIAL_RELAY_ON;
        }
        byte relay = (buffer[1] - '0');
        relays.SetRelay(relay, state, module);
        Serial.print("Set #");
        Serial.print(relay);
        Serial.print(" - ");
        Serial.println(state);
      }
    }
    
    if(((buffer[0] >= '0') && (buffer[0] <= '9')) || ((buffer[0] >= 'A') && (buffer[0] <= 'F'))){
      byte value = 0x00;
      if(buffer[0] <= '9')
        value = (buffer[0] - '0') * 16;
      else
        value = (buffer[0] - 'A' + 10) * 16;
      
      if(((buffer[1] >= '0') && (buffer[1] <= '9')) || ((buffer[1] >= 'A') && (buffer[1] <= 'F'))){
        if(buffer[1] <= '9')
          value += (buffer[1] - '0');
        else
          value += (buffer[1] - 'A' + 10);
        
        Serial.print("Value = ");
        Serial.println(value, HEX);
        relays.SetModule(value, module);
      }
    }
  }

}





