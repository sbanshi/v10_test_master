/*
 * spi_v8.h
 *  
 *      Author: nikhil
 */

#ifndef DRV_SPI_V8_H_
#define DRV_SPI_V8_H_

#include "ch.h"
#include "hal.h"
#include "Setup.h"

#define MPU_WHO_AM_I		0x75
#define MPU_PWR_MGMT_1		0x6B
#define MPU_GYRO_CONFIG		0x1B
#define MPU_ACCEL_CONFIG	0x1C
#define MPU_ACCEL_XOUT_H	0x3B


#define MS_RESET			0x1E
#define MS_D1_256			0x40
#define MS_D1_512			0x42
#define MS_D1_1024			0x44
#define MS_D1_2048			0x46
#define MS_D1_4096			0x48
#define MS_D2_256			0x50
#define MS_D2_512			0x52
#define MS_D2_1024			0x54
#define MS_D2_2048			0x56
#define MS_D2_4096			0x58
#define MS_ADC				0x00
#define MS_PROM				0xA0

extern float ax, ay, az, gx, gy, gz;

void start_spi2(void);

void get_spi2(void);

void start_MPU(void);

uint8_t spi_read_byte(uint8_t addr);

void spi_write_byte(uint8_t addr, uint8_t value);

void get_mpu_data(void);

void start_ms_spi(void);

void get_ms_data(void);

#endif /* DRV_SPI_V8_H_ */
