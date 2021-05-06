//**************************************************************
//
//    Demo code for the FDC1004 capacitance sensor breakout board
//
//    This example measures raw capacitance across CHANNEL1 and Gnd and
//    prints on serial terminal
//
//    Copyright (c) Facebook, Inc. and its affiliates.
//    This source code is licensed under the MIT license found in the
//    LICENSE file in the root directory of this source tree.
//
//**************************************************************

#include <Wire.h>
#include <FDC1004.h>

//settings to configure measurement channel
uint8_t rate = 4;        //1 = 100 Hz, 2 = 200 Hz, 3 = 400 Hz


#define NUM_SENSORS 4
//measurement channel and sensor channel in different arrays for clarify
//measurement channel and sensor channel do not need to match
uint8_t measurements[NUM_SENSORS] = {1,2,3,4};
uint8_t sensors[NUM_SENSORS] = {1,2,3,4};
uint8_t capdacs[4] = {0,0,0,0};

uint8_t measurements_flag = 0b1111; //set bit for each of the sensors [1][2][3][4]
//variables to hold output data

uint32_t rawdata[NUM_SENSORS];
float capacitance[NUM_SENSORS];

//object to access library functions
FDC1004 myFDC1004;

void setup() {
  //start i2c bus on your arduino
  Wire.begin(); 
  Wire.setClock(400000);

  //start serial port and wait for user to open
  Serial.begin(115200); 
  while(!Serial){delay(5);}
  
  
  //set up all four sensors for single measurement mode
  for(int i=0; i < NUM_SENSORS; i++)
  {
    myFDC1004.setupSingleMeasurement(measurements[i], sensors[i], capdacs[i]);
    
  }
  
  //start repeatedly measured all four measuremens at given rate
  myFDC1004.startRepeatMeasurement(measurements_flag, rate);
  
  Serial.println("Ready");
  delay(1000);
}

void loop() {

  //myFDC1004.getRepeatRawData(measurements_flag, rawdata);
  myFDC1004.getRepeatCapacitance(measurements_flag, capacitance);
  Serial.print("Capacitance (pF): ");

  for(int i=0; i < NUM_SENSORS; i++)
  {  
     Serial.print(" ");
     Serial.print(capacitance[i]);
//     Serial.print(" ");
//     Serial.print(myFDC1004.getGainCalibration(sensors[i]));
//     Serial.print(" ");
//     Serial.print(myFDC1004.getOffsetCalibration(sensors[i]));
  }
  Serial.println();
  delay(100);

}
