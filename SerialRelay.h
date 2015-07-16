#ifndef RC_SERIAL_RELAY_H
#define RC_SERIAL_RELAY_H

/*
	   RoboCore Serial Relay Library
		    (v1.0 - 20/05/2015)

  Serial Relay functions for Arduino
    (tested with Arduino 1.0.1)

  Copyright 2015 RoboCore (François) ( http://www.RoboCore.net )
  
  ------------------------------------------------------------------------------
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  ------------------------------------------------------------------------------
  
*/

#include <Arduino.h>

#define SERIAL_RELAY_MAX_MODULES 10

#define SERIAL_RELAY_DELAY_DATA           5 // [µs]
#define SERIAL_RELAY_DELAY_CLOCK_HIGH     5 // [µs]
#define SERIAL_RELAY_DELAY_CLOCK_LOW     20 // [µs]
#define SERIAL_RELAY_DELAY_LATCH       1100 // [µs]

#define SERIAL_RELAY_OFF      0
#define SERIAL_RELAY_ON       1
#define SERIAL_RELAY_NONE  0xFF

#define SERIAL_RELAY_DEBUG

//#define ENABLE_INFINITE_STREAM

class SerialRelay {
  public:
    // Constructor
    SerialRelay(int pinData, int pinClock, byte qty = 1){
      _pinData = pinData;
      _pinClock = pinClock;
      pinMode(_pinData, OUTPUT);
      pinMode(_pinClock, OUTPUT);
      digitalWrite(_pinData, LOW);
      digitalWrite(_pinClock, LOW);
      
      _tosend = false; // default
      
      for(int i=0 ; i < SERIAL_RELAY_MAX_MODULES ; i++)
        _data[i] = 0; // default all LOW
      
      // check and set module qty
      _module_qty = qty;
      if(qty > SERIAL_RELAY_MAX_MODULES)
        _module_qty = SERIAL_RELAY_MAX_MODULES;
#ifndef ENABLE_INFINITE_STREAM
      if(qty == 0)
        _module_qty = 1;
#endif
    }
    
    // Get the state of the relay (1-based)
    byte GetState(byte relay, byte module = 1){
      // check index
      if((relay > 8) || (module > SERIAL_RELAY_MAX_MODULES))
        return SERIAL_RELAY_NONE;
      
      // check if whole mask
      if(relay == 0)
        return _data[module - 1];
      
      byte mask = (1 << (relay - 1));
      if(mask & _data[module - 1])
        return SERIAL_RELAY_ON;
      else
        return SERIAL_RELAY_OFF;
    }
    
#ifdef SERIAL_RELAY_DEBUG
    // Print all information
    void Info(HardwareSerial *stream, byte format = HEX){
      stream->print("Qty:");
      stream->println(_module_qty);
      for(int i=0 ; i < _module_qty ; i++){
        stream->print('[');
        stream->print(i+1);
        stream->print("] - ");
        if(format == HEX)
          stream->print("0x");
        else if(format == BIN)
          stream->print("0b");
        else if(format == OCT)
          stream->print('0');
        stream->println(_data[i], format);
      }
    }
#endif
    
    // Set all relays at once on one module (1-based)
    //  NOTE: set send to FALSE to manually write to the shift register
    boolean SetModule(byte mask, byte module = 1, boolean send = true){
      // check index
      if(module > SERIAL_RELAY_MAX_MODULES)
        return false;
      
      _data[module - 1] = mask;
      _tosend = true;
      
      if(send)
        Send();
      
      return true;
    }
    
    // Set the relay state on a module (2x 1-based)
    //  NOTE: set send to FALSE to manually write to the shift register
    boolean SetRelay(byte relay, byte state, byte module = 1, boolean send = true){
      // check index
      if((relay == 0) || (relay > 8) || (module > SERIAL_RELAY_MAX_MODULES))
        return false;
      
      if(state == SERIAL_RELAY_ON){
        byte mask = (1 << (relay - 1));
        _data[module - 1] |= mask;
      } else {
        byte mask = ~(1 << (relay - 1));
        _data[module - 1] &= mask;
      }
      _tosend = true;
      
      if(send)
        Send();
      
      return true;
    }
    
    // Write changes
    void Write(void){
      if(_tosend)
        Send();
    }
  
  
  private:
    int _pinClock;
    int _pinData;
    byte _module_qty;
    byte _data[SERIAL_RELAY_MAX_MODULES];
    boolean _tosend; // TRUE if data to send
    
    // Send the data
    void Send(void){
      byte mask;
      boolean last_relay = false;
      byte module_number = _module_qty; // start from MAX because the do loop will set it to 0-base at the beginning
      do {
        // loop infinite stream or increment
        if(_module_qty != 0){
          module_number--;
          
          // check for last relay
          if(module_number == 0)
            last_relay = true;
        }
        
        // send pulse
        mask = 0x80; // reset
        for(int i=1 ; i <= 8 ; i++){
          // set Data line
          if(_data[module_number] & mask)
            digitalWrite(_pinData, HIGH);
          else
            digitalWrite(_pinData, LOW);
          
          delayMicroseconds(SERIAL_RELAY_DELAY_DATA); // delay between Data and Clock signals
          
          // set Clock line
          digitalWrite(_pinClock, HIGH); // rising edge
          if((i == 8) && last_relay)
            delayMicroseconds(SERIAL_RELAY_DELAY_LATCH); // latch
          else
            delayMicroseconds(SERIAL_RELAY_DELAY_CLOCK_HIGH); // shift
          digitalWrite(_pinClock, LOW);
          delayMicroseconds(SERIAL_RELAY_DELAY_CLOCK_LOW); // it is acceptable to have 5µs delay after the last bit has been sent
          
          mask >>= 1; // update mask
        }
      } while(module_number > 0);
      
      _tosend = false; // reset
    }
  
};


#endif // RC_SERIAL_RELAY_H




