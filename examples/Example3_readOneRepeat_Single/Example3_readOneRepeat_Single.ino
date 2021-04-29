//**************************************************************
//
//    Demo code for the FDC1004 capacitance sensor breakout board
//
//    This example repeatedly measures raw capacitance across CHANNEL1 and Gnd and
//    prints on serial terminal
//
//    This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//   For information on how to use, visit
//**************************************************************
#include <Wire.h>
#include <FDC1004.h>

//settings to configure measurement channel

uint8_t rate = 3;        //1 = 100 Hz, 2 = 200 Hz, 3 = 400 Hz

#define NUM_SENSORS 1
//measurement channel and sensor channel in different arrays for clarify
//measurement channel and sensor channel do not need to match
uint8_t measurements[NUM_SENSORS] = {1};
uint8_t sensors[NUM_SENSORS] = {1};
uint8_t capdac[NUM_SENSORS] =  {0};
uint8_t measurements_flag = 0b1000; //set bit to 1 for sensors [1][2][3][4]

//variables to hold output data
uint32_t rawdata[NUM_SENSORS];
float capacitance[NUM_SENSORS];

//object to access library functions
FDC1004 myFDC1004;

void setup() {
  //start i2c bus on your arduino
  Wire.begin(); 

  //start serial port and wait for user to open
  Serial.begin(115200); 
  while(!Serial){delay(5);}
  
  
  //set up all four sensors for single measurement mode
  for(int i=0; i < NUM_SENSORS; i++)
  {
    myFDC1004.setupSingleMeasurement(measurements[i], sensors[i], capdac[i]);

  }
  
  //start repeatedly measured all four measurements at given rate
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
     //Serial.print(rawdata[i]);
     Serial.println(capacitance[i]);

  }
  delay(100);

}
