/**
*
* @file	bmp280.c
* @date	2020-01-10
* @version	v3.3.4
*
*/
#include "bmp280.h"
#include <Arduino.h>
#include <Wire.h>

/********************** function implementations ************************/

/*!
 *  @brief Function that creates a mandatory delay required in some of the APIs such as "bmg250_soft_reset",
 *      "bmg250_set_foc", "bmg250_perform_self_test"  and so on.
 *
 *  @param[in] period_ms  : the required wait time in milliseconds.
 *  @return void.
 *
 */
void delay_ms(uint32_t period_ms)
{
    /* Implement the delay routine according to the target machine */
    delay(period_ms);
}

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[in] reg_data : Pointer to the data buffer whose value is to be written.
 *  @param[in] length   : No of bytes to write.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

  /* Implement the I2C write routine according to the target machine. */
  Wire.beginTransmission(i2c_addr);
  for (uint16_t i = 0; i<length;i++) {
    Wire.write(reg_addr);
    Wire.write(*reg_data);
    reg_data++;
    reg_addr++;
  }
  Wire.endTransmission();    // stop transmitting
  //return -1;
  return 0;
}

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : Sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

  /* Implement the I2C read routine according to the target machine. */
  Wire.beginTransmission(i2c_addr);
  Wire.write(reg_addr);
  Wire.endTransmission();
  Wire.requestFrom(i2c_addr, length);
  while (Wire.available()) {
    *reg_data = Wire.read();
    reg_data++;
  }
  //return -1;
  return 0;
}

/*!
 *  @brief Function for writing the sensor's registers through SPI bus.
 *
 *  @param[in] cs           : Chip select to enable the sensor.
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data : Pointer to the data buffer whose data has to be written.
 *  @param[in] length       : No of bytes to write.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Implement the SPI write routine according to the target machine. */
    
    return -1;
}

/*!
 *  @brief Function for reading the sensor's registers through SPI bus.
 *
 *  @param[in] cs       : Chip select to enable the sensor.
 *  @param[in] reg_addr : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Implement the SPI read routine according to the target machine. */
    return -1;
}

/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : name of the API whose execution status has to be printed.
 *  @param[in] rslt     : error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void print_rslt(const char api_name[], int8_t rslt)
{
    if (rslt != BMP280_OK)
    {
        Serial.println(api_name);
        if (rslt == BMP280_E_NULL_PTR)
        {
            Serial.print("Null pointer error");
            Serial.println(rslt);
        }
        else if (rslt == BMP280_E_COMM_FAIL)
        {
            Serial.print("Bus communication failed");
            Serial.println(rslt);
        }
        else if (rslt == BMP280_E_IMPLAUS_TEMP)
        {
            Serial.print("Invalid Temperature");
            Serial.println(rslt);
        }
        else if (rslt == BMP280_E_DEV_NOT_FOUND)
        {
            Serial.print("Device not found");
            Serial.println(rslt);
        }
        else
        {
            /* For more error codes refer "*_defs.h" */
            Serial.print("Unknown error code");
            Serial.println(rslt);
        }
    }
}