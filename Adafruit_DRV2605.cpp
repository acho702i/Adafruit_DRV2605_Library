/*************************************************** 
  This is a library for the Adafruit DRV2605L Haptic Driver

  ----> http://www.adafruit.com/products/2306
 
  Check out the links above for our tutorials and wiring diagrams
  This motor/haptic driver uses I2C to communicate

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#include <Adafruit_DRV2605.h>

// Remove comment to print auto calibration results
//#define DRV2605_PRINT_A_CAL_RESULT

/**************************************************************************/
/*! 
    @brief  Instantiates a new DRV2605 class
*/
/**************************************************************************/
// I2C, no address adjustments or pins
Adafruit_DRV2605::Adafruit_DRV2605() {
}


/**************************************************************************/
/*! 
    @brief  Setups the HW
*/
/**************************************************************************/
boolean Adafruit_DRV2605::begin() {
  Wire.begin();
  uint8_t id = readRegister8(DRV2605_REG_STATUS);
  //Serial.print("Status 0x"); Serial.println(id, HEX);
  
  writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby
  
  writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback
  
  writeRegister8(DRV2605_REG_WAVESEQ1, 1); // strong click
  writeRegister8(DRV2605_REG_WAVESEQ2, 0);
  
  writeRegister8(DRV2605_REG_OVERDRIVE, 0); // no overdrive
  
  writeRegister8(DRV2605_REG_SUSTAINPOS, 0);
  writeRegister8(DRV2605_REG_SUSTAINNEG, 0);
  writeRegister8(DRV2605_REG_BREAK, 0);
  writeRegister8(DRV2605_REG_AUDIOMAX, 0x64);
  
  // ERM open loop
  
  // turn off N_ERM_LRA
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
  // turn on ERM_OPEN_LOOP
  writeRegister8(DRV2605_REG_CONTROL3, readRegister8(DRV2605_REG_CONTROL3) | 0x20);

  return true;
}

void Adafruit_DRV2605::setWaveform(uint8_t slot, uint8_t w) {
  writeRegister8(DRV2605_REG_WAVESEQ1+slot, w);
}

void Adafruit_DRV2605::selectLibrary(uint8_t lib) {
  writeRegister8(DRV2605_REG_LIBRARY, lib);
}

void Adafruit_DRV2605::go() {
  writeRegister8(DRV2605_REG_GO, 1);
}

void Adafruit_DRV2605::stop() {
  writeRegister8(DRV2605_REG_GO, 0);
}

void Adafruit_DRV2605::setMode(uint8_t mode) {
  writeRegister8(DRV2605_REG_MODE, mode);
}

void Adafruit_DRV2605::setRealtimeValue(uint8_t rtp) {
  writeRegister8(DRV2605_REG_RTPIN, rtp);
}

/********************************************************************/

uint8_t Adafruit_DRV2605::readRegister8(uint8_t reg) {
  uint8_t x ;
   // use i2c
    Wire.beginTransmission(DRV2605_ADDR);
    Wire.write((byte)reg);
    Wire.endTransmission();
    Wire.requestFrom((byte)DRV2605_ADDR, (byte)1);
    x = Wire.read();

  //  Serial.print("$"); Serial.print(reg, HEX); 
  //  Serial.print(": 0x"); Serial.println(x, HEX);
  
  return x;
}

void Adafruit_DRV2605::writeRegister8(uint8_t reg, uint8_t val) {
   // use i2c
    Wire.beginTransmission(DRV2605_ADDR);
    Wire.write((byte)reg);
    Wire.write((byte)val);
    Wire.endTransmission();
}

/****************/


// Allow users to use ERM motor or LRA motors

void Adafruit_DRV2605::useERM ()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
}

void Adafruit_DRV2605::useLRA ()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) | 0x80);
}


bool Adafruit_DRV2605::autoCal(uint8_t ratedVoltage, uint8_t overdriveClamp, uint8_t driveTime)
{
  bool isFailed = true;
  // Write required Registers
  writeRegister8(DRV2605_REG_RATEDV, ratedVoltage);
  writeRegister8(DRV2605_REG_CLAMPV, overdriveClamp);
  writeRegister8(DRV2605_REG_CONTROL1, driveTime & (0xE0 | readRegister8(DRV2605_REG_CONTROL1)));
  
  setMode(DRV2605_MODE_AUTOCAL);
  go();

  do{
    delay(100);
    }while(readRegister8(DRV2605_REG_GO));

  isFailed = readRegister8(DRV2605_REG_STATUS) & 0x08;

#ifdef DRV2605_PRINT_A_CAL_RESULT
  Serial.print("Auto calibration ");
  isFailed ? Serial.println("failed") : Serial.println("succeeded");
  Serial.print("A_CAL_COMP:"); Serial.println(readRegister8(DRV2605_REG_AUTOCALCOMP), HEX);
  Serial.print("A_CAL_BEMF:"); Serial.println(readRegister8(DRV2605_REG_AUTOCALEMP), HEX);
  Serial.print("FEEDBACK:"); Serial.println(readRegister8(DRV2605_REG_FEEDBACK), HEX);
#endif

  return !isFailed;
}

