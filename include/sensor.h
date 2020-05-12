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

//#include "bmp280.h"

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

#include "sensor.cpp"