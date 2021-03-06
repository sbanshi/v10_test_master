/*
 * spi_v8.cpp
 *  
 *      Author: nikhil
 */

#include "spi_v10.h"
#include <math.h>

#define SPI_F2		0
#define SPI_F4		(SPI_CR1_BR_0)
#define SPI_F8		(SPI_CR1_BR_1)
#define SPI_F16		(SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define SPI_F32		(SPI_CR1_BR_2)
#define SPI_F64		(SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define SPI_F128	(SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define SPI_F265	(SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)

#define MS_CFG	spi2cfg
#define MS_SPI	SPID2
#define MS_PORT	GPIOE
#define MS_PIN	15

static const SPIConfig spi1cfg = {
  NULL,
  GPIOA,
  4,
  SPI_F265
};

static const SPIConfig spi2cfg = {
  NULL,
  GPIOE,
  15,
  SPI_F265
};

static const SPIConfig spi4cfg = {
  NULL,
  GPIOE,
  3,
  SPI_F265
};

static const SPIConfig spi4cfg_fram = {
  NULL,
  GPIOE,
  4,
  SPI_F265
};

void start_spi2(void){
	spiStart(&SPID2, &spi2cfg);
	palSetPad(GPIOB, 12);
	delay(20);
}

void get_spi2(void){
	spiStart(&SPID2, &spi2cfg);
	uint8_t txbuf[3], rxbuf[3];
	txbuf[0] = 0x12;
	txbuf[1] = 0x70;
	txbuf[2] = 0x3C;
	spiExchange(&SPID2, 6, txbuf, rxbuf);

	debug("SPI2 %x, %x, %x", rxbuf[0], rxbuf[1], rxbuf[2]);
}



void start_MPU(void){
	spiStart(&SPID1, &spi1cfg);
	palSetPad(GPIOA, 4);

	delay(20);


	uint8_t out = spi_read_byte(MPU_WHO_AM_I);

	debug("MPU_WHO_AM_I %x", out);

	//Set Power management bit
	uint8_t txbuf[3], rxbuf[3];
	txbuf[0] = MPU_PWR_MGMT_1;
	txbuf[1] = 0;

	spiSelect(&SPID1);
	spiSend(&SPID1, 2, txbuf);
	spiUnselect(&SPID1);

	txbuf[0] = MPU_PWR_MGMT_1| 0x80;
	txbuf[1] = 0;
	txbuf[2] = 0;

	spiSelect(&SPID1);
	spiExchange(&SPID1, 2, txbuf, rxbuf);
	spiUnselect(&SPID1);

	debug("MPU_PWR_MGMT_1 %x %x %x", rxbuf[0], rxbuf[1], rxbuf[2]);

	txbuf[0] = MPU_GYRO_CONFIG;
	txbuf[1] = 3 << 3;

	spiSelect(&SPID1);
	spiSend(&SPID1, 2, txbuf);
	spiUnselect(&SPID1);

	txbuf[0] = MPU_GYRO_CONFIG| 0x80;
	txbuf[1] = 0;
	txbuf[2] = 0;

	spiSelect(&SPID1);
	spiExchange(&SPID1, 3, txbuf, rxbuf);
	spiUnselect(&SPID1);


	debug("MPU_GYRO_CONFIG %x %x %x", rxbuf[0], rxbuf[1], rxbuf[2]);

	txbuf[0] = MPU_ACCEL_CONFIG;
	txbuf[1] = 2 << 3;

	spiSelect(&SPID1);
	spiSend(&SPID1, 2, txbuf);
	spiUnselect(&SPID1);

	txbuf[0] = MPU_ACCEL_CONFIG| 0x80;
	txbuf[1] = 0;
	txbuf[2] = 0;

	spiSelect(&SPID1);
	spiExchange(&SPID1, 2, txbuf, rxbuf);
	spiUnselect(&SPID1);

	debug("MPU_ACCEL_CONFIG %x %x %x", rxbuf[0], rxbuf[1], rxbuf[2]);

}

float ax, ay, az, gx, gy, gz;

void get_mpu_data(void){

	uint8_t txbuf[3], rxbuf[15];
	int16_t accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z;



	txbuf[0] = MPU_ACCEL_XOUT_H | 0x80;
	txbuf[1] = 0;
	txbuf[2] = 0;

	spiSelect(&SPID1);
	spiExchange(&SPID1, 15, txbuf, rxbuf);
	spiUnselect(&SPID1);

	accel_x = (rxbuf[1] << 8 ) + rxbuf[2];
	accel_y = (rxbuf[3] << 8 ) + rxbuf[4];
	accel_z = ((rxbuf[5] << 8 ) + rxbuf[6]);
	gyro_x  = (rxbuf[9] << 8 ) + rxbuf[10];
	gyro_y  = (rxbuf[11] << 8) + rxbuf[12];
	gyro_z  = ((rxbuf[13] << 8) + rxbuf[14]);

	ax = accel_x * (9.80665f / 4096.0f);
	ay = accel_y * (9.80665f / 4096.0f);
	az = accel_z * (9.80665f / 4096.0f);

	gx = gyro_x*(0.0174532f / 16.4f);
	gy = gyro_y*(0.0174532f / 16.4f);
	gz = gyro_z*(0.0174532f / 16.4f);


//	debug("MPU %d %d %d %d %d %d", accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z);
	debug("MPU %f %f %f %f %f %f", ax, ay, az, gx, gy, gz);

}

uint8_t spi_read_byte(uint8_t addr){

	uint8_t txbuf[3], rxbuf[3];
	txbuf[0] = addr | 0x80;
	txbuf[1] = 0;
	txbuf[2] = 0;

	spiSelect(&SPID1);
	spiExchange(&SPID1, 3, txbuf, rxbuf);
	spiUnselect(&SPID1);

	debug("%x, %x, %x", rxbuf[0], rxbuf[1], rxbuf[2]);

	return rxbuf[1];

}

void spi_write_byte(uint8_t addr, uint8_t value){

	uint8_t txbuf[2];

	txbuf[0] = addr;
	txbuf[1] = value;

    spiSelect(&SPID1);
    spiSend(&SPID1, 2, txbuf);
    spiUnselect(&SPID1);


}

uint16_t ms_prom[8];

void start_ms_spi(void){
//	palClearPad(GPIOE, 4);
	spiStart(&MS_SPI, &MS_CFG);
//	palSetPad(GPIOE, 3);

	delay(200);

	uint8_t txbuf[3], rxbuf[4];
	txbuf[0] = MS_RESET;


	spiSelect(&MS_SPI);
	spiSend(&MS_SPI, 2, txbuf);
	spiUnselect(&MS_SPI);

	delay(200);

	for(int i = 0; i < 8; i++){

		txbuf[0] = MS_PROM + (i << 1);

		spiSelect(&MS_SPI);
		spiExchange(&MS_SPI, 4, txbuf, rxbuf);
		spiUnselect(&MS_SPI);

		ms_prom[i] = (uint16_t)((rxbuf[1] << 8) | (rxbuf[2]));

		debug("MS_PROM %u, %u", i, ms_prom[i]);

		delay(20);
	}

//	spiStop(&MS_SPI);

}

float ground_press = 1;
float ground_temp = 25;
uint16_t press_cnt = 0 ;
void get_ms_data(void){

	uint8_t txbuf[3], rxbuf[18];
	uint32_t D1, D2;

	txbuf[0] = MS_D1_1024;

	spiSelect(&MS_SPI);
	spiSend(&MS_SPI, 2, txbuf);
	spiUnselect(&MS_SPI);

	delay(20);

	txbuf[0] = MS_ADC;

	spiSelect(&MS_SPI);
	spiExchange(&MS_SPI, 6, txbuf, rxbuf);
	spiUnselect(&MS_SPI);

	delay(20);

	D1 = ((uint32_t)rxbuf[1] << 16) | ((int32_t)rxbuf[2] << 8) | rxbuf[3];

//	debug("D1 %u", D1);



	txbuf[0] = MS_D2_1024;

	spiSelect(&MS_SPI);
	spiSend(&MS_SPI, 2, txbuf);
	spiUnselect(&MS_SPI);

	delay(20);

	txbuf[0] = MS_ADC;

	spiSelect(&MS_SPI);
	spiExchange(&MS_SPI, 6, txbuf, rxbuf);
	spiUnselect(&MS_SPI);

	delay(20);

	D2 = ((uint32_t)rxbuf[1] << 16) | ((int32_t)rxbuf[2] << 8) | rxbuf[3];

//	debug("Ms D1 %u, D2 %u", D1, D2);

	float dT;
	float TEMP;
	int64_t OFF;
	int64_t SENS;
	float P;

	// Formulas from manufacturer datasheet
	// sub -20c temperature compensation is not included

	// we do the calculations using floating point
	// as this is much faster on an AVR2560, and also allows
	// us to take advantage of the averaging of D1 and D1 over
	// multiple samples, giving us more precision
	dT = (float)D2-(((float)ms_prom[4])*256.0f);
	TEMP = (float)(((int64_t)dT * (int64_t)ms_prom[5]) >> 23);
	OFF = (int64_t)((uint32_t)ms_prom[1] << 16) + (int64_t)(dT / 128.0f * (float)ms_prom[3]);
	SENS = (int64_t)((uint32_t)ms_prom[0] << 15) + (int64_t)(dT / 256.0f * (float)ms_prom[2]);

	//debug("dT=%f, TEMP=%f, OFF=%d, SENS=%d", dT, TEMP, OFF, SENS);

	if (TEMP < 0) {
		// second order temperature compensation when under 20 degrees C
		//2^31 = 2147483648 and 2^31/2 = 1073741824.
		float T2 = (dT)/1073741824.0f;
		T2 = T2*(dT)/2.0f;
		float Aux = TEMP*TEMP;
		int64_t OFF2 = (int64_t)(2.5f*Aux);
		int64_t SENS2 = (int64_t)(1.25f*Aux);
		TEMP = TEMP - T2;
		OFF = OFF - OFF2;
		SENS = SENS - SENS2;
	}

	P = ((float)D1*((float)SENS/2097152.0f) - (float)OFF)/32768.0f;
	TEMP = (TEMP + 2000.0f) * 0.01f;

	press_cnt++;

	if(press_cnt == 20 && ground_press == 1){
		ground_press = P;
		ground_temp = TEMP;
	}
	if(press_cnt > 2000)press_cnt = 25;

    float scaling = P/ground_press ;
    float temp    = ground_temp + 273.15f;

    float ret = 153.8462f * temp * (1.0f - expf(0.190259f * logf(scaling)));

    debug("baro_alt %f", ret);

}

#define FRAM_WREN	0b00000110
#define FRAM_WRDI	0b00000100
#define FRAM_RDSR	0b00000101
#define FRAM_WRSR	0b00000001
#define FRAM_READ	0b00000011
#define FRAM_FSTRD	0b00001011
#define FRAM_WRITE	0b00000010
#define FRAM_SLEEP	0b10111001
#define FRAM_RDID	0b10011111

void start_fram(void){
//	palClearPad(GPIOE, 3);palSetPad(GPIOE, 4);
	spiStart(&SPID4, &spi4cfg_fram);



	delay(20);

	uint8_t txbuf[3], rxbuf[10];
	txbuf[0] = FRAM_RDID | 0x80;


	spiSelect(&SPID4);
	spiExchange(&SPID4, 10, txbuf, rxbuf);

	spiUnselect(&SPID4);
//	spiStop(&SPID4);

	for(int i = 0; i < 10; i++){
		debug("%x", rxbuf[i]);
	}




}
