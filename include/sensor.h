/**
*
* @file	bmp280.h
* @date	2020-01-10
* @version	v3.3.4
*
*/
#ifndef __BMP280_H__
#define __BMP280_H__
#endif

/******************************  BMP280 *****************************/
/**
 * @brief This section describes the function prototypes 
 * end declares variables for sensor.cpp
 * 
 */
void delay_ms(uint32_t period_ms);
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
void print_rslt(const char api_name[], int8_t rslt);
int8_t rslt;
struct bmp280_dev bmp;
struct bmp280_config conf;
struct bmp280_uncomp_data ucomp_data;
int32_t temp32;
uint32_t pres32;
double temp;
double pres;

/******************************  IR Sensor *****************************/
/**
 * @brief This section defines the transmitter button values
 * 
 */
// iDual Fernbedienung (NEC)
const long TRIM_VOR = 0x407f708f;
const long TRIM_RECHTS = 0x407fe01f;
const long TRIM_RUECK = 0x407f10ef;
const long TRIM_LINKS = 0x407fa857;
const long SPEED_UP = 0x407FF00F;
const long SPEED_DOWN = 0x407f58a7;
const long HALT = 0x407F40BF;
const long MAX = 0x407F807F;

// NAD Fernbedienung (NEC)
const long NAD_ON = 0xE13E0DF2; // repeatable
const long NAD_OFF = 0xE13E8D72; // repeatable
const long NAD_MUTE = 0xE13E05FA; // repeatable
const long NAD_SOURCE_UP = 0xE13E5DA2; // repeatable
const long NAD_SOURCE_DOWN = 0xE13EDD22; // repeatable
const long NAD_VOL_UP = 0xE13E44BB; // repeatable
const long NAD_VOL_DOWN = 0xE13EC43B; // repeatable
const long NAD_PRESET_RIGHT = 0xE13EC53A; // repeatable
const long NAD_PRESET_LEFT = 0xE13E25DA; // repeatable
const long NAD_REW = 0xE13EA05F; // repeatable
const long NAD_PLAY = 0xE13E807F; // repeatable
const long NAD_FOR = 0xE13E609F; // repeatable


#include "sensor.cpp"