//**************************************************************
//
//    Demo code for the FDC1004 capacitance sensor breakout board
//
//    This example measures raw capacitance across SensorA and SensorB and
//    prints on serial terminal (differential mode)
//
//    Copyright (c) Facebook, Inc. and its affiliates.
//    This source code is licensed under the MIT license found in the
//    LICENSE file in the root directory of this source tree.
//
//**************************************************************

#include <Wire.h>
#include <FDC1004.h>

#define NUM_SENSORS 2
//settings to configure measurement channel
uint8_t measurements[NUM_SENSORS] = {1,2};    //must be 1,2,3,or 4
uint8_t sensorA[NUM_SENSORS] = {1,3};         //must be 1,2,3,or 4
uint8_t sensorB[NUM_SENSORS] = {2,4};         //must be 1,2,3,or 4 and NOT same as sensor A
uint8_t rate = 1;           //1 = 100 Hz, 2 = 200 Hz, 3 = 400 Hz

uint8_t measurements_flag = 0b1100; //set bit for each of the sensors [1][2][3][4]
//variables to hold output data
float capacitance[NUM_SENSORS];

//object to access library functions
FDC1004 myFDC1004;

void setup() {
  //start i2c bus on your arduino
  Wire.begin(); 

  //start serial port and wait for user to open
  Serial.begin(115200); 
  while(!Serial){delay(5);}
  
  for(int i=0; i < NUM_SENSORS; i++)
  {
    myFDC1004.setupDifferentialMeasurement(measurements[i], sensorA[i], sensorB[i]);
  }
  
  myFDC1004.startRepeatMeasurement(measurements_flag, rate);

  Serial.println("Ready");
}

void loop() {
  
  myFDC1004.getRepeatCapacitance(measurements_flag, capacitance);
  Serial.print("Capacitance (pF): ");

  for(int i=0; i < NUM_SENSORS; i++)
  {  
     Serial.print(" ");
     Serial.print(capacitance[i]);
  }
  Serial.println();
  delay(100);

}
