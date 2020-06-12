/* ============================================

===============================================
*/

#include "myMPU9250.h"

/** Default constructor, uses default I2C address.
 * @see MPU9250_DEFAULT_ADDRESS
 */
myMPU9250::myMPU9250() {
    devAddr = MPU9250_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see MPU9250_DEFAULT_ADDRESS
 * @see MPU9250_ADDRESS_AD0_LOW
 * @see MPU9250_ADDRESS_AD0_HIGH
 */
myMPU9250::myMPU9250(uint8_t address) {
    devAddr = address;
}

/** Trigger a full device reset.
 * A small delay of ~50ms may be desirable after triggering a reset.
 * @see MPU9250_RA_PWR_MGMT_1
 * @see MPU9250_PWR1_DEVICE_RESET_BIT
 */
void myMPU9250::reset() {
    I2Cdev::writeBit(devAddr, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_DEVICE_RESET_BIT, true);
    delay(50);
}

/** Power on and prepare for general usage.
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void myMPU9250::initialize() {
    __setClockSource(MPU9250_CLOCK_PLL_XGYRO);
    setFullScaleGyroRange(MPU9250_GYRO_FS_250);
    setFullScaleAccelRange(MPU9250_ACCEL_FS_2);
    setSleepEnabled(false); // thanks to Jack Elston for pointing this one out!
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool myMPU9250::testConnection() {
    return getDeviceID() == 0x71;
}

/** Set clock source setting.
 * An internal 8MHz oscillator, gyroscope based clock, or external sources can
 * be selected as the MPU-60X0 clock source. When the internal 8 MHz oscillator
 * or an external source is chosen as the clock source, the MPU-60X0 can operate
 * in low power modes with the gyroscopes disabled.
 *
 * Upon power up, the MPU-60X0 clock source defaults to the internal oscillator.
 * However, it is highly recommended that the device be configured to use one of
 * the gyroscopes (or an external clock source) as the clock reference for
 * improved stability. The clock source can be selected according to the following table:
 *
 * <pre>
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
 * </pre>
 *
 * @param source New clock source setting
 * @see getClockSource()
 * @see MPU9250_RA_PWR_MGMT_1
 * @see MPU9250_PWR1_CLKSEL_BIT
 * @see MPU9250_PWR1_CLKSEL_LENGTH
 */
void myMPU9250::__setClockSource(uint8_t source) {
    I2Cdev::writeBits(devAddr, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_CLKSEL_BIT, MPU9250_PWR1_CLKSEL_LENGTH, source);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU9250_GYRO_FS_250
 * @see MPU9250_RA_GYRO_CONFIG
 * @see MPU9250_GCONFIG_FS_SEL_BIT
 * @see MPU9250_GCONFIG_FS_SEL_LENGTH
 */
void myMPU9250::setFullScaleGyroRange(uint8_t range) {
    I2Cdev::writeBits(devAddr, MPU9250_RA_GYRO_CONFIG, MPU9250_GCONFIG_FS_SEL_BIT, MPU9250_GCONFIG_FS_SEL_LENGTH, range);
}

/** Set full-scale accelerometer range.
 * @param range New full-scale accelerometer range setting
 * @see getFullScaleAccelRange()
 */
void myMPU9250::setFullScaleAccelRange(uint8_t range) {
    I2Cdev::writeBits(devAddr, MPU9250_RA_ACCEL_CONFIG, MPU9250_ACONFIG_AFS_SEL_BIT, MPU9250_ACONFIG_AFS_SEL_LENGTH, range);
}

/** Set sleep mode status.
 * @param enabled New sleep mode enabled status
 * @see getSleepEnabled()
 * @see MPU9250_RA_PWR_MGMT_1
 * @see MPU9250_PWR1_SLEEP_BIT
 */
void myMPU9250::setSleepEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_SLEEP_BIT, enabled);
}

/** Get Device ID.
 * This register is used to verify the identity of the device (0b110100, 0x34).
 * @return Device ID (6 bits only! should be 0x34)
 * @see MPU9250_RA_WHO_AM_I
 * @see MPU9250_WHO_AM_I_BIT
 * @see MPU9250_WHO_AM_I_LENGTH
 */
uint8_t myMPU9250::getDeviceID() {
    I2Cdev::readBits(devAddr, MPU9250_RA_WHO_AM_I, MPU9250_WHO_AM_I_BIT, MPU9250_WHO_AM_I_LENGTH, buffer);
    return buffer[0];
}

void myMPU9250::setXGyroOffset() {
    int16_t offset;
    uint8_t length = 20;
    uint8_t gx[2] = {0,0};
    int32_t gyro_sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        int16_t gyro_temp = 0;
        I2Cdev::readBytes(devAddr, MPU9250_RA_GYRO_XOUT_H, 2, gx); // read data for averaging
        gyro_temp  = (int16_t) (((int16_t)gx[0] << 8) | gx[1]  ) ;
        gyro_sum  += (int32_t) gyro_temp;
    }
    offset = (int16_t)((gyro_sum / 4) / length);
    I2Cdev::writeWord(devAddr, MPU9250_RA_XG_OFFS_USRH, -offset);
}
void myMPU9250::setXGyroOffset(int16_t offset) {
    I2Cdev::writeWord(devAddr, MPU9250_RA_XG_OFFS_USRH, offset);
}
void myMPU9250::setYGyroOffset() {
    int16_t offset;
    uint8_t length = 20;
    uint8_t gy[2] = {0,0};
    int32_t gyro_sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        int16_t gyro_temp = 0;
        I2Cdev::readBytes(devAddr, MPU9250_RA_GYRO_YOUT_H, 2, gy); // read data for averaging
        gyro_temp  = (int16_t) (((int16_t)gy[0] << 8) | gy[1]  ) ;
        gyro_sum  += (int32_t) gyro_temp;
    }
    offset = (int16_t)((gyro_sum / 4) / length);
    I2Cdev::writeWord(devAddr, MPU9250_RA_YG_OFFS_USRH, -offset);
}
void myMPU9250::setYGyroOffset(int16_t offset) {
    I2Cdev::writeWord(devAddr, MPU9250_RA_YG_OFFS_USRH, offset);
}
void myMPU9250::setZGyroOffset() {
    int16_t offset;
    uint8_t length = 20;
    uint8_t gz[2] = {0,0};
    int32_t gyro_sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        int16_t gyro_temp = 0;
        I2Cdev::readBytes(devAddr, MPU9250_RA_GYRO_ZOUT_H, 2, gz); // read data for averaging
        gyro_temp  = (int16_t) (((int16_t)gz[0] << 8) | gz[1]  ) ;
        gyro_sum  += (int32_t) gyro_temp;
    }
    offset = (int16_t)((gyro_sum / 4) / length);
    I2Cdev::writeWord(devAddr, MPU9250_RA_ZG_OFFS_USRH, -offset);
}
void myMPU9250::setZGyroOffset(int16_t offset) {
    I2Cdev::writeWord(devAddr, MPU9250_RA_ZG_OFFS_USRH, offset);
}

int16_t myMPU9250::getXGyroOffset() {
    I2Cdev::readWord(devAddr, MPU9250_RA_XG_OFFS_USRH, word_buffer);
    return word_buffer[0];
}

void myMPU9250::dump_reg() {
  // MPU9250_RA_XG_OFFS_USRH = 0x13 - Gyros Offset XH, XL, YH, YL, ZH, ZL
  I2Cdev::readBytes(devAddr, MPU9250_RA_XG_OFFS_USRH, 6, buffer);
  for (int i=0; i<6; i++) { 
    Serial.print(MPU9250_RA_XG_OFFS_USRH + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_RA_SMPLRT_DIV = 0x19
  I2Cdev::readBytes(devAddr, MPU9250_RA_SMPLRT_DIV, 7, buffer);
  for (int i=0; i<7; i++) { 
    Serial.print(MPU9250_RA_SMPLRT_DIV + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_RA_FIFO_EN = 0x23
  I2Cdev::readBytes(devAddr, MPU9250_RA_FIFO_EN, 1, buffer);
  for (int i=0; i<1; i++) { 
    Serial.print(MPU9250_RA_FIFO_EN + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_RA_INT_PIN_CFG = 0x37 - MPU9250_RA_INT_ENABLE = 0x38
  I2Cdev::readBytes(devAddr, MPU9250_RA_INT_PIN_CFG, 2, buffer);
  for (int i=0; i<2; i++) { 
    Serial.print(MPU9250_RA_INT_PIN_CFG + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_RA_INT_STATUS = 0x3A - MPU9250_RA_ACCEL_XOUT_H = 0x3B, Acell/Temp/Gyro XH, XL, YH, YL, ZH, ZL
  I2Cdev::readBytes(devAddr, MPU9250_RA_INT_STATUS, 7, buffer);
  for (int i=0; i<7; i++) { 
    Serial.print(MPU9250_RA_INT_STATUS + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_RA_TEMP_OUT_H = 0x42 - Temp/Gyro XH, XL, YH, YL, ZH, ZL
  I2Cdev::readBytes(devAddr, MPU9250_RA_TEMP_OUT_H, 8, buffer);
  for (int i=0; i<8; i++) { 
    Serial.print(MPU9250_RA_TEMP_OUT_H + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_RA_USER_CTRL = 0x6A - MPU9250_RA_PWR_MGMT_1 = 0x6B - MPU9250_RA_PWR_MGMT_2 = 0x6C
  I2Cdev::readBytes(devAddr, MPU9250_RA_USER_CTRL, 3, buffer);
  for (int i=0; i<3; i++) { 
    Serial.print(MPU9250_RA_USER_CTRL + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_RA_FIFO_COUNTH = 0x72 - MPU9250_RA_FIFO_COUNTL - MPU9250_RA_FIFO_R_W - MPU9250_RA_WHO_AM_I
  I2Cdev::readBytes(devAddr, MPU9250_RA_FIFO_COUNTH, 4, buffer);
  for (int i=0; i<4; i++) { 
    Serial.print(MPU9250_RA_FIFO_COUNTH + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_XA_OFFSET_H = 0x77 - MPU9250_XA_OFFSET_L = 0x78
  I2Cdev::readBytes(devAddr, MPU9250_XA_OFFSET_H, 2, buffer);
  for (int i=0; i<2; i++) { 
    Serial.print(MPU9250_XA_OFFSET_H + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_YA_OFFSET_H = 0x7A - MPU9250_YA_OFFSET_L = 0x7B
  I2Cdev::readBytes(devAddr, MPU9250_YA_OFFSET_H, 2, buffer);
  for (int i=0; i<2; i++) { 
    Serial.print(MPU9250_YA_OFFSET_H + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
  // MPU9250_ZA_OFFSET_H = 0x7D - MPU9250_ZA_OFFSET_L = 0x7E
  I2Cdev::readBytes(devAddr, MPU9250_ZA_OFFSET_H, 2, buffer);
  for (int i=0; i<2; i++) { 
    Serial.print(MPU9250_ZA_OFFSET_H + i, HEX);
    Serial.print(": ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
}

void myMPU9250::setXAccelOffset() { 
  int16_t offset;
  uint8_t length = 20;
  uint8_t ax[2] = {0,0};
  uint8_t rax[2] = {0,0};
  int32_t accel_sum = 0;
  for (uint8_t i = 0; i < length; i++) {
    int16_t accel_temp = 0;
    I2Cdev::readBytes(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_ACCEL_XOUT_H, 2, ax); // read data for averaging
    accel_temp  = (int16_t) (((int16_t)ax[0] << 8) | ax[1] );
    //Serial.print(i);
    //Serial.print(" -> ");
    //Serial.println(accel_temp);
    accel_sum  += accel_temp;
  }
  int16_t accel_bias_reg = 0; // A place to hold the factory accelerometer trim biases
  I2Cdev::readBytes(MPU9250_DEFAULT_ADDRESS, MPU9250_XA_OFFSET_H, 2, rax); // Read factory accelerometer trim values
  accel_bias_reg = (((int16_t)rax[0] << 8) | rax[1]);
  offset = (int16_t) accel_bias_reg - (int16_t) (accel_sum / length / 8);
  I2Cdev::writeWord(MPU9250_DEFAULT_ADDRESS, MPU9250_XA_OFFSET_H, offset);
/*   Serial.println((int16_t)accel_sum/length);
  Serial.print(accel_bias_reg);
  Serial.print(" - ");
  Serial.print(accel_sum / length / 8);
  Serial.print(" = ");
  Serial.println(offset);
  Serial.println();
 */
}

void myMPU9250::setYAccelOffset() {
  int16_t offset;
  uint8_t length = 20;
  uint8_t ay[2] = {0,0};
  uint8_t ray[2] = {0,0};
  int32_t accel_sum = 0;
  for (uint8_t i = 0; i < length; i++) {
    int16_t accel_temp = 0;
    I2Cdev::readBytes(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_ACCEL_YOUT_H, 2, ay); // read data for averaging
    accel_temp  = (int16_t) (((int16_t)ay[0] << 8) | ay[1] );
    accel_sum  += (int32_t) accel_temp;
  }
  int16_t accel_bias_reg = 0; // A place to hold the factory accelerometer trim biases
  I2Cdev::readBytes(MPU9250_DEFAULT_ADDRESS, MPU9250_YA_OFFSET_H, 2, ray); // Read factory accelerometer trim values
  accel_bias_reg = (((int16_t)ray[0] << 8) | ray[1]);

  offset = (int16_t)accel_bias_reg - (int16_t) (accel_sum / length / 8);
  I2Cdev::writeWord(MPU9250_DEFAULT_ADDRESS, MPU9250_YA_OFFSET_H, offset);
}

void myMPU9250::setZAccelOffset() { 
  int16_t offset;
  uint8_t length = 20;
  uint8_t az[2] = {0,0};
  uint8_t raz[2] = {0,0};
  int32_t accel_sum = 0;
  for (uint8_t i = 0; i < length; i++) {
    int16_t accel_temp = 0;
    I2Cdev::readBytes(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_ACCEL_ZOUT_H, 2, az); // read data for averaging
    accel_temp  = (int16_t) (((int16_t)az[0] << 8) | az[1] );
    accel_sum  += (int32_t) accel_temp;
  }
  int16_t accel_bias_reg = 0; // A place to hold the factory accelerometer trim biases
  I2Cdev::readBytes(MPU9250_DEFAULT_ADDRESS, MPU9250_ZA_OFFSET_H, 2, raz); // Read factory accelerometer trim values
  accel_bias_reg = (int32_t) (((int16_t)raz[0] << 8) | raz[1]);

  offset = (int16_t)accel_bias_reg - (((int16_t)accel_sum / length) / 8);
  I2Cdev::writeWord(MPU9250_DEFAULT_ADDRESS, MPU9250_ZA_OFFSET_H, offset);
}

void myMPU9250::getMotion6() {
    int16_t ax, ay, az, gx, gy, gz;
    I2Cdev::readBytes(devAddr, MPU9250_RA_ACCEL_XOUT_H, 14, buffer);
    ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    az = (((int16_t)buffer[4]) << 8) | buffer[5];
    gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    gz = (((int16_t)buffer[12]) << 8) | buffer[13];

    Axyz[0] = (float)ax*(2.0/32768.0); // g
    Axyz[1] = (float)ay*(2.0/32768.0);
    Axyz[2] = (float)az*(2.0/32768.0);
    Gxyz[0] = (float)gx*(250.0/32768.0); // degree per sec  - DPS
    Gxyz[1] = (float)gy*(250.0/32768.0);  
    Gxyz[2] = (float)gz*(250.0/32768.0);   
}