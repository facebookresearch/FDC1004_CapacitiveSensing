/*!
 *  @file FDC1004.cpp

 *  This is a library for the FDC1004 4-Channel Capacitive Sensor
 *
 *  This library requires I2C connection to communicate
 *
 *  MIT license, all text above must be included in any redistribution
 */


// #include "Arduino.h"
// #include <Wire.h>
#include <FDC1004.h>

uint8_t MEAS_CONFIG[] = {0x08, 0x09, 0x0A, 0x0B};
uint8_t MEAS_MSB[] = {0x00, 0x02, 0x04, 0x06};
uint8_t MEAS_LSB[] = {0x01, 0x03, 0x05, 0x07};
uint8_t SAMPLE_DELAY[] = {11,11,6,3};

FDC1004::FDC1004(uint16_t rate)
{
  this->_addr = 0b1010000;
  this->_rate = rate;
}

void FDC1004::writeRegister16(uint8_t reg, uint16_t data)
{
  Wire.beginTransmission(_addr);
  Wire.write(reg); //send address
  Wire.write( (uint8_t) (data >> 8) );
  Wire.write( (uint8_t) data);
  //Wire.write( data, 2);
  Wire.endTransmission();
}

uint16_t FDC1004::readRegister16(uint8_t reg)
{
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.endTransmission();
  uint16_t value;
  Wire.beginTransmission(_addr);
  Wire.requestFrom(_addr, (uint8_t)2);
  value = Wire.read();
  value <<= 8;
  value |= Wire.read();
  Wire.endTransmission();
  return value;
}

/*!
 *  @brief      setup measurement channel to read specified channel in 
                single measurement mode (between specified channel and ground)
 *  @param      measurement channel to setup (1, 2, 3, or 4)
 *
 *  @param      input channel to read (1, 2, 3, or 4)
 * 
 *  @param      capdac, 5 bit number to add as offset capacitance. Offset is 3.125pfxcapdac
 * 
 */
uint8_t FDC1004::setupSingleMeasurement(uint8_t measurement, uint8_t channel, uint8_t capdac)
{
    //check if measurement and channel are between 1 and 4

    //check if capdac is between 00000 and 11111

    uint16_t configuration_data;
    configuration_data = (channel-1) << 13; // set CHA
    //Serial.println(configuration_data, BIN);
    configuration_data |=  (0x4) << 10; // disable CHB, enable CAPDAC
    configuration_data |= (capdac) << 5; //CAPDAC value
    //Serial.println(configuration_data, BIN);
    writeRegister16(FDC1004_CONF_MEAS1+(measurement-1), configuration_data);
    //writeRegister16(0x09, configuration_data);
    configuration_data = readRegister16(FDC1004_CONF_MEAS1+(measurement-1));
    //Serial.println(configuration_data, BIN);
    return 0;

}

/*!
 *  @brief      setup measurement channel to relative capacitance between sensor A and sensor B 
 *              (differential mode)
 * 
 *  @param      measurement
 *              measurement channel to setup (1, 2, 3, or 4)
 *
 *  @param      sensorA
 *              input sensor to read (1, 2, 3, or 4)
 * 
 *  @param      sensorB
 *              input sensor to read (1, 2, 3, or 4)
 * 
 */
uint8_t FDC1004::setupDifferentialMeasurement(uint8_t measurement, uint8_t sensorA, uint8_t sensorB)
{
    uint8_t CHA;
    uint8_t CHB;
    //check which sensor is smaller. CHA is lower CHB is higher
    //if they are equal throw an error
    if(sensorB > sensorA)
    {
        CHA = sensorA;
        CHB = sensorB;
    }
    else if (sensorA > sensorB)
    {
        CHA = sensorB;
        CHB = sensorA;
    }
    else{
      Serial.println("Sensor A and B cannot be the same in differential mode.");
    }

    uint16_t configuration_data;
    configuration_data = (CHA-1) << 13; // set CHA (channels are 1 indexed in the datasheet, binary register values are 0-indexed >:( )
    //Serial.println(configuration_data, BIN);
    configuration_data |=  (CHB-1) << 10; // set CHB, disable CAPDAC
    configuration_data &= (0xFC1F); //make sure capdac is 0

    //Serial.println(configuration_data, BIN);
    writeRegister16(FDC1004_CONF_MEAS1+(measurement-1), configuration_data);
    //writeRegister16(0x09, configuration_data);

    return 0;

}
/*!
 *  @brief      get raw 24-bit data from the chip (no capdac offset)

 *  @param      measurement 
 *              measurement channel to read from (1, 2, 3, or 4). Must be setup first
 *
 * 
 */
uint32_t FDC1004::getRawData(uint8_t measurement, uint8_t rate)
{
  //check if measurement is 1-4
  uint32_t data = 0;
  //check is measurement is ready
  //read FDC_conf, check if DONE_X is 1
  //return DATA_NOT_READY
  //trigger single measurement
  uint16_t trigger_data;
  trigger_data = 0;             //repeat disabled
  trigger_data |= (rate) << 10; // sample rate

  //only one channel can be set to trigger at a time.
  trigger_data |= (1 << (8-measurement)); 
  writeRegister16(FDC1004_CONFIG, trigger_data);
  // trigger_data = readRegister16(FDC1004_CONFIG);
  // Serial.println(trigger_data, BIN);
  delay(15); //wait for data conversion
  
  // trigger_data = readRegister16(FDC1004_CONFIG);
  // Serial.println(trigger_data, BIN);

  uint16_t msb = readRegister16(FDC1004_MEAS1_MSB+2*(measurement-1));
  uint16_t lsb = readRegister16(FDC1004_MEAS1_LSB+2*(measurement-1));
  // Serial.println(msb, BIN);
  // Serial.println(lsb, BIN);
  data |= msb;
  data = (data << 16);
  data |= lsb;
  data = (data >> 8);

  //trigger_data = readRegister16(FDC1004_CONFIG);
  //Serial.println(trigger_data, BIN);
  return data;

}

float FDC1004::getCapacitance(uint8_t measurement, uint8_t rate)
{
  float data;
  uint32_t rawdata = getRawData(measurement, rate);

  //Serial.println(rawdata, BIN);
  rawdata = (rawdata & 0xFFFFFF); //make sure it is only 24 bit data
  //Serial.println(rawdata, BIN);
  
  //get capdac value for this measurement channel
  uint16_t capdac = readRegister16(FDC1004_CONF_MEAS1+(measurement-1));
  capdac = (capdac >> 5) & 0x1F; //isolate 5 bits [9:5] with capdac value
  //Serial.println(capdac);
  //capdac = capdac * 3.125; //in pF
  //Serial.println(capdac);
  //convert from 2's complement to binary
  if((rawdata & 0x800000) > 0)
  {
      data = -1*((~rawdata)+1);   
  }
  else
  {
      data = rawdata;
  }
  
  data = ( data / 524288.0); //pow(2,19) equation from page 16 of datasheet
  // Serial.println(data);
  // Serial.println(capdac);
  return data+(capdac*3.125);
}

uint8_t FDC1004::startRepeatMeasurement(uint8_t measurements, uint8_t rate)
{
    //set which measurements should be repeatedly triggered at given rate
    uint16_t trigger_data;
    trigger_data = (1<<8);        //repeat ENABLED
    trigger_data |= (rate) << 10; // sample rate
    trigger_data |= (measurements << 4);
    //Serial.println(trigger_data, BIN);
    writeRegister16(FDC1004_CONFIG, trigger_data);

    trigger_data = readRegister16(FDC1004_CONFIG);
    //Serial.println(trigger_data, BIN);
    // writeRegister16(FDC1004_CONFIG, trigger_data);
  
    delay(40);
    trigger_data = readRegister16(FDC1004_CONFIG);
    //Serial.println(trigger_data, BIN);
    return 0;
}

uint8_t FDC1004::getRepeatRawData(uint8_t measurements, uint32_t * value)
{
  uint32_t data;
  uint16_t msb;
  uint16_t lsb;
  int idx = 0;

  // uint16_t trigger_data;
  // trigger_data = readRegister16(FDC1004_CONFIG);
  // Serial.print(trigger_data, BIN);
  // writeRegister16(FDC1004_CONFIG, trigger_data);
  
  //always return array[4] and only fill request measurements?
  //or return array with measurement size request, in ascending order?

  if(((measurements & 8)>0))
  {
    msb = readRegister16(FDC1004_MEAS1_MSB);
    lsb = readRegister16(FDC1004_MEAS1_LSB);
    data = ((msb << 16) | lsb) >> 8;
    value[idx] = data;
    idx += 1;
  }
  if((measurements & 4)>0)
  {
    msb = readRegister16(FDC1004_MEAS2_MSB);
    lsb = readRegister16(FDC1004_MEAS2_LSB);
    data = ((msb << 16) | lsb) >> 8;
    value[idx] = data;
    idx += 1;
  }
  if((measurements & 2)>0)
  {
    msb = readRegister16(FDC1004_MEAS3_MSB);
    lsb = readRegister16(FDC1004_MEAS3_LSB);
    data = ((msb << 16) | lsb) >> 8;
    value[idx] = data;
    idx += 1;
  }
  if((measurements & 1)>0)
  {
    msb = readRegister16(FDC1004_MEAS4_MSB);
    lsb = readRegister16(FDC1004_MEAS4_LSB);
    data = ((msb << 16) | lsb) >> 8;
    value[idx] = data;
    idx += 1;
  }
  return 0;

}


uint8_t FDC1004::getRepeatCapacitance(uint8_t measurements, float * capacitance)
{
  uint32_t rawdata;
  float data; 
  uint16_t capdac;
  uint32_t value[4];
  getRepeatRawData(measurements, value);
  int idx = 0;
  if((measurements & 8)>0)
  {
      rawdata = (value[idx] & 0xFFFFFF); //make sure it is only 24 bit data
      data = convert2Decimal24(rawdata);

      capdac = readRegister16(FDC1004_CONF_MEAS1);
      capdac = (capdac >> 5) & 0x1F; //isolate 5 bits [9:5] with capdac value
      //capdac = capdac * 3.125; //in pF
      
      capacitance[idx] = (data/524288.0) + (capdac*3.125);
      idx+=1;//pow(2,19) equation from page 16 of datasheet
  }
  if((measurements & 4)>0)
  {
      rawdata = (value[idx] & 0xFFFFFF); //make sure it is only 24 bit data
      data = convert2Decimal24(rawdata);

      capdac = readRegister16(FDC1004_CONF_MEAS2);
      capdac = (capdac >> 5) & 0x1F; //isolate 5 bits [9:5] with capdac value
      //capdac = capdac * 3.125; //in pF
      
      capacitance[idx] = (data/524288.0) + (capdac*3.125);
      idx+=1; //pow(2,19) equation from page 16 of datasheet
  }
  if((measurements & 2)>0)
  {
      rawdata = (value[idx] & 0xFFFFFF); //make sure it is only 24 bit data
      data = convert2Decimal24(rawdata);

      capdac = readRegister16(FDC1004_CONF_MEAS3);
      capdac = (capdac >> 5) & 0x1F; //isolate 5 bits [9:5] with capdac value
      //capdac = capdac * 3.125; //in pF
      
      capacitance[idx] = (data/524288.0) + (capdac*3.125); //pow(2,19) equation from page 16 of datasheet
      idx+=1;
  }
  if((measurements & 1)>0)
  {
      rawdata = (value[idx] & 0xFFFFFF); //make sure it is only 24 bit data
      data = convert2Decimal24(rawdata);

      capdac = readRegister16(FDC1004_CONF_MEAS4);
      capdac = (capdac >> 5) & 0x1F; //isolate 5 bits [9:5] with capdac value
      //capdac = capdac * 3.125; //in pF
      
      capacitance[idx] = (data/524288.0) + (capdac*3.125); //pow(2,19) equation from page 16 of datasheet
      idx+=1;
  }

  return 0;
}

//convert 24-bit number in 2's complement to decimal value 
float FDC1004::convert2Decimal24(uint32_t rawdata)
{
  float data;
  if((rawdata & 0x800000) > 0)
  {
      data = -1*((~rawdata)+1);   
  }
  else
  {
      data = rawdata;
  }
  return data;
}


float FDC1004::getOffsetCalibration(uint8_t sensor)
{
  //5 bits [15:11] are integer in 2's complement
  //11 bits [10:0] are fractional part (in 2's complement????)
  //-16 to 16 pF
  uint16_t rawoffset = readRegister16(FDC1004_OFFSET_CAL_CIN1+(sensor-1));
  int integer_part = (rawoffset >> 10) << 19;               //make it 24 bit
  integer_part = (convert2Decimal24(integer_part) / pow(2,19));   //make it 5 bit
  float offset = integer_part + ((rawoffset & 0x0788) / pow(2,11));
  return offset;
}

float FDC1004::getGainCalibration(uint8_t sensor)
{
  //2 bits [15:14] are integer portion (in 2's complement????)
  //14 bits [13:0] are fractional portion (in 2's complement????)
  //0 to 4
  uint16_t rawgain = readRegister16(FDC1004_GAIN_CAL_CIN1+(sensor-1));

  //convert from fixed point to double
  float gain = (float) rawgain / pow(2,14);
  //float gain = (rawgain >> 14);
  return gain;
}