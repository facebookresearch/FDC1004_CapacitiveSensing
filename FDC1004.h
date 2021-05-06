/*!
 *  @file FDC1004.h
 *  This is a library for the FDC1004 4-Channel Capacitive Sensor
 *
 *  This library requires I2C connection to communicate
 *
 *  Copyright (c) Facebook, Inc. and its affiliates.
 *  This source code is licensed under the MIT license found in the
 *  LICENSE file in the root directory of this source tree.
 */

#ifndef _FDC1004
#define _FDC1004

#include "Arduino.h"
#include "Wire.h"

// The default I2C address
#define FDC1004_I2CADDR_DEFAULT 0x50        ///< default I2C address
#define MPR121_TOUCH_THRESHOLD_DEFAULT 12  ///< default touch threshold value
#define MPR121_RELEASE_THRESHOLD_DEFAULT 6 ///< default relese threshold value

/*!
 *  Device register map
 */
enum {
  FDC1004_MEAS1_MSB = 0x00,
  FDC1004_MEAS1_LSB = 0x01,
  FDC1004_MEAS2_MSB = 0x02,
  FDC1004_MEAS2_LSB = 0x03,
  FDC1004_MEAS3_MSB = 0x04,
  FDC1004_MEAS3_LSB = 0x05,
  FDC1004_MEAS4_MSB = 0x06,
  FDC1004_MEAS4_LSB = 0x07,

  FDC1004_CONF_MEAS1 = 0x08,
  FDC1004_CONF_MEAS2 = 0x09,
  FDC1004_CONF_MEAS3 = 0x0A,
  FDC1004_CONF_MEAS4 = 0x0B,

  FDC1004_CONFIG = 0x0C,

  FDC1004_OFFSET_CAL_CIN1 = 0x0D,
  FDC1004_OFFSET_CAL_CIN2 = 0x0E,
  FDC1004_OFFSET_CAL_CIN3 = 0x0F,
  FDC1004_OFFSET_CAL_CIN4 = 0x10,

  FDC1004_GAIN_CAL_CIN1 = 0x11,
  FDC1004_GAIN_CAL_CIN2 = 0x12,
  FDC1004_GAIN_CAL_CIN3 = 0x13,
  FDC1004_GAIN_CAL_CIN4 = 0x14,

  FDC1004_ID = 0xFE,		//ID of manufacturer, Texas Instruments
  FDC1004_DEVICE_ID = 0xFF, // ID of FDC1004 device

};

/*!
 *  @brief  Class that stores state and functions for interacting with MPR121
 *  proximity capacitive touch sensor controller.
 */
class FDC1004 {

  public:
    // Hardware I2C
    FDC1004(uint16_t rate = 0x01);

    //bool begin(uint8_t i2caddr = FDC1004_I2CADDR_DEFAULT);
    
    uint8_t setupSingleMeasurement(uint8_t measurement, uint8_t sensor, uint8_t capdac);
    uint8_t setupDifferentialMeasurement(uint8_t measurement, uint8_t sensorA, uint8_t sensorB);

    // trigger and read one measurement, turns off repeat measurements
    uint32_t getRawData(uint8_t measurement, uint8_t rate);
    float getCapacitance(uint8_t measurement, uint8_t rate);

    // continuously read measurements
    uint8_t startRepeatMeasurement(uint8_t measurements, uint8_t rate);
    uint8_t getRepeatRawData(uint8_t measurements, uint32_t * value);
    uint8_t getRepeatCapacitance(uint8_t measurements, float * capacitance);

    uint8_t setOffsetCalibration(uint8_t sensor);
    float getOffsetCalibration(uint8_t sensor);

    uint8_t setGainCalibration(uint8_t sensor);
    float getGainCalibration(uint8_t sensor);

    uint8_t readRegister8(uint8_t reg);
    uint16_t readRegister16(uint8_t reg);
    void writeRegister16(uint8_t reg, uint16_t value);

    //offset calibration
    //gain calibration 
  private:
    uint8_t _addr;
    uint8_t _rate;
    //struct for each measurement channel with capdac and rate and last value
    float convert2Decimal24(uint32_t rawdata);
};

#endif
