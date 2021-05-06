//**************************************************************
//
//    Demo code for the FDC1004 capacitance sensor breakout board
//
//    This example measures raw capacitance across all four sensors and Gnd and
//    prints on serial terminal, single measurement mode, must request data before reading.
//
//    Copyright (c) Facebook, Inc. and its affiliates.
//    This source code is licensed under the MIT license found in the
//    LICENSE file in the root directory of this source tree.
//
//**************************************************************

#include <Wire.h>
#include <FDC1004.h>

#define NUM_SENSORS 4

//settings to configure measurement channel
uint8_t measurements[NUM_SENSORS] = {1,2,3,4}; 
uint8_t sensors[NUM_SENSORS] = {1,2,3,4}; 
uint8_t capdacs[NUM_SENSORS] = {0,0,0,0};
uint8_t rate = 1;        //1 = 100 Hz, 2 = 200 Hz, 3 = 400 Hz

//variable to hold output data
float cap = 0;

//object to access library functions
FDC1004 myFDC1004;

void setup() {
  //start i2c bus on your arduino
  Wire.begin(); 

  //start serial port and wait for user to open
  Serial.begin(115200); 
  while(!Serial){delay(5);}

  //set each measurement channel to read from each sensor
  for(int i=0; i < NUM_SENSORS; i++)
  {
    myFDC1004.setupSingleMeasurement(measurements[i], sensors[i], capdacs[i]);
    
  }
  Serial.println("Ready");
}

void loop() {

  for(int i=0; i < NUM_SENSORS; i++)
  {
    cap = myFDC1004.getCapacitance(measurements[i], rate);
    Serial.print(cap);
    Serial.print(" ");
  }
  Serial.println(" pf");
  
  delay(100);

}
