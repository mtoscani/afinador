//// ****** THIS FILE IS AUTOGENERATED ******
////
////          >>>> DO NOT CHANGE <<<<
////
/// 
///  Filename; C:\Users\Axel Lacapmesure\Google Drive\Facultad\2018_InstrumentacionYControl\afinador\prueba.py
///  Source class: Afinador2
///  Generation timestamp: 2018-12-10T15:34:52.209893
///  Class code hash: d8b86d2c295de2e888b64d479673cf2192e638e3
///
/////////////////////////////////////////////////////////////

#define LENGTH 256 // Define el largo de la adquisición. 256 es lo maximo que se banca usando floats.
#define FREQ 38461 // Frecuencia de sampleo medida 
#define BAUD_RATE 115200



// Import libraries
#include <Arduino.h>

// PERSONALIZE: Add global dependencies here.

#include "inodriver_bridge.h"
#include "inodriver_user.h"

// PERSONALIZE: Add local dependencies here.

void setup() {
  bridge_setup();
  
  user_setup();

  analogRead(A0);

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  // PERSONALIZE: your setup code her

  Serial.begin(BAUD_RATE);
  Serial.println("READY");
}

void loop() {
  
  bridge_loop();
  
  user_loop();

}
