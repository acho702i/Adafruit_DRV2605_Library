#include <Wire.h>
#include "Adafruit_DRV2605.h"

#define DRV2605_PRINT_A_CAL_RESULT 1

Adafruit_DRV2605 drv;

void setup() {
  Serial.begin(9600);
  
  drv.begin();

  drv.useLRA();
  while(!drv.autoCal(0x3E, 0x8C, 3));
  drv.selectLibrary(6);
  
  // I2C trigger by sending 'go' command 
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG); 
}

uint8_t effect = 1;

void loop() {
  Serial.print("Effect #"); Serial.println(effect);

  // set the effect to play
  drv.setWaveform(0, effect);  // play effect 
  drv.setWaveform(1, 0);       // end waveform

  // play the effect!
  drv.go();

  // wait a bit
  delay(500);

  effect++;
  if (effect > 117) effect = 1;
}
