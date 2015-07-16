/*
      RoboCore - Serial Relay example
                    (19/06/2015)

  Written by François.
  
  Example of how to use the Serial Relay library.
  The relays will be energized one by one and then
  de-energized one by one.

*/

#include <SerialRelay.h>

const int NumModules = 2;    // maximum of 10
const int PauseTime = 1000;  // [ms]

SerialRelay relays(4,5,NumModules); // (data, clock, number of modules)

// --------------------------------------------------------------

void setup(){
  Serial.begin(19200);
  relays.Info(&Serial,BIN);
  Serial.println();
}

// --------------------------------------------------------------

void loop(){
  // turn on one by one starting from relay 1 & module 1
  for(int i=1 ; i <= NumModules ; i++){
    for(int j=1 ; j <= 4 ; j++){
      relays.SetRelay(j, SERIAL_RELAY_ON, i);
      delay(PauseTime);
    }
  }
  
  // turn off one by one starting from relay 1 & module 1
  for(int i=1 ; i <= NumModules ; i++){
    for(int j=1 ; j <= 4 ; j++){
      relays.SetRelay(j, SERIAL_RELAY_OFF, i);
      delay(PauseTime);
    }
  }
}

// --------------------------------------------------------------




