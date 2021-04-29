//**************************************************************
//
//    Demo code for the FDC1004 capacitance sensor breakout board
//
//    This example measures raw capacitance across Sensor and Gnd and
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
uint8_t measurement = 1;    //must be 1,2,3,or 4
uint8_t sensor = 1;         //must be 1,2,3,or 4
uint8_t capdac = 0;         //pF offset for selected measurement
uint8_t rate = 1;           //1 = 100 Hz, 2 = 200 Hz, 3 = 400 Hz

//variables to hold output data
uint32_t data;
float cap = 0;

//object to access library functions
FDC1004 myFDC1004;

void setup() {
  //start i2c bus on your arduino
  Wire.begin(); 

  //start serial port and wait for user to open
  Serial.begin(115200); 
  while(!Serial){delay(5);}
  
  //set up one measurement channel to measure channel A (CHA)
  //measurement channels hold the settings for what you want to measure
  //channels are the physical pins for measurement on the chip/board
  //read more about it here: 
  myFDC1004.setupSingleMeasurement(measurement, sensor, capdac);
  Serial.println("Ready");
}

void loop() {

  cap = myFDC1004.getCapacitance(measurement, rate);
 
  Serial.print("Measurement Channel #");
  Serial.print(measurement);
  Serial.print(" Capacitance (pF): ");
  Serial.println(cap);

  delay(1000);

}
