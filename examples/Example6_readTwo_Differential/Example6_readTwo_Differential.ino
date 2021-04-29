//**************************************************************
//
//    Demo code for the FDC1004 capacitance sensor breakout board
//
//    This example measures raw capacitance across SensorA and SensorB and
//    prints on serial terminal (differential mode)
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

#define NUM_SENSORS 2
//settings to configure measurement channel
uint8_t measurements[NUM_SENSORS] = {1,2};    //must be 1,2,3,or 4
uint8_t sensorA[NUM_SENSORS] = {1,3};         //must be 1,2,3,or 4
uint8_t sensorB[NUM_SENSORS] = {2,4};         //must be 1,2,3,or 4 and NOT same as sensor A
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
  
  for(int i=0; i < NUM_SENSORS; i++)
  {
    myFDC1004.setupDifferentialMeasurement(measurements[i], sensorA[i], sensorB[i]);
  }
  Serial.println("Ready");
}

void loop() {
  Serial.print("Capacitance (pF): ");
  for(int i=0; i < NUM_SENSORS; i++)
  {
    cap = myFDC1004.getCapacitance(measurements[i], rate);
    Serial.print(cap);
    Serial.print(" ");
  }

  Serial.println();
 
  delay(1000);

}
