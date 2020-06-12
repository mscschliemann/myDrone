#include "AK8963.h"

/** Default constructor, uses default I2C address.
 * @see MPU9250_DEFAULT_ADDRESS
 */
AK8963::AK8963() {
    devAddr = AK8963_ADDRESS;
    Mmode = 0x02;  
}

/** Specific address constructor.
 * @param address I2C address
 * @see MPU9250_DEFAULT_ADDRESS
 * @see MPU9250_ADDRESS_AD0_LOW
 * @see MPU9250_ADDRESS_AD0_HIGH
 */
AK8963::AK8963(uint8_t address) {
    devAddr = address;
    Mmode = 0x02;  // 2 for 8 Hz, 6 for 100 Hz continuous magnetometer data read
}

void AK8963::reset() {

}


void AK8963::initialize() {
  // First extract the factory calibration for each magnetometer axis
  uint8_t rawData[3];  // x/y/z gyro calibration data stored here
  I2Cdev::writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
  delay(10);
  I2Cdev::writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
  delay(10);
  I2Cdev::readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
  magCalibration[0] =  (float)(rawData[0] - 128)/256. + 1.;   // Return x-axis sensitivity adjustment values, etc.
  magCalibration[1] =  (float)(rawData[1] - 128)/256. + 1.;  
  magCalibration[2] =  (float)(rawData[2] - 128)/256. + 1.; 
  I2Cdev::writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
  delay(10);
  // Configure the magnetometer for continuous read and highest resolution
  // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
  // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
  I2Cdev::writeByte(AK8963_ADDRESS, AK8963_CNTL, Mscale << 4 | Mmode); // Set magnetometer data resolution and sample ODR
  delay(10);
}    

float* AK8963::getMagCalibration() {
  return magCalibration;
}

void AK8963::getMotion3() {
  uint8_t rawData[7];  // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
  int16_t mx, my, mz;
  uint8_t data[1];
  I2Cdev::readByte(AK8963_ADDRESS, AK8963_ST1, data);
  if(data[0] & 0x01) { // wait for magnetometer data ready bit to be set
    I2Cdev::readBytes(AK8963_ADDRESS, AK8963_XOUT_L, 7, &rawData[0]);  // Read the six raw data and ST2 registers sequentially into data array
    uint8_t c = rawData[6]; // End data read by reading ST2 register
    if(!(c & 0x08)) { // Check if magnetic sensor overflow set, if not then report data
      mx = ((int16_t)rawData[1] << 8) | rawData[0] ;  // Turn the MSB and LSB into a signed 16-bit value
      my = ((int16_t)rawData[3] << 8) | rawData[2] ;  // Data stored as little Endian
      mz = ((int16_t)rawData[5] << 8) | rawData[4] ; 
    }
    Mxyz[0] = (float)mx*(10.*4912./32760.0)*magCalibration[0] - magbias[0]; // mGauss
    Mxyz[1] = (float)my*(10.*4912./32760.0)*magCalibration[1] - magbias[1];
    Mxyz[2] = (float)mz*(10.*4912./32760.0)*magCalibration[2] - magbias[2];
  }
}
