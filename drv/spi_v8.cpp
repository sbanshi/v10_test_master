/*
 * spi_v8.cpp
 *  
 *      Author: nikhil
 */

#include "spi_v8.h"

#define SPI_F2		0
#define SPI_F4		(SPI_CR1_BR_0)
#define SPI_F8		(SPI_CR1_BR_1)
#define SPI_F16		(SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define SPI_F32		(SPI_CR1_BR_2)
#define SPI_F64		(SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define SPI_F128	(SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define SPI_F265	(SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)

static const SPIConfig spi1cfg = {
  NULL,
  GPIOA,
  4,
  SPI_F2
};

static const SPIConfig spi2cfg = {
  NULL,
  GPIOB,
  12,
  SPI_F2
};

static const SPIConfig spi4cfg = {
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

	spiStart(&SPID4, &spi4cfg);
	palSetPad(GPIOE, 4);

	delay(20);

	uint8_t txbuf[3], rxbuf[4];
	txbuf[0] = MS_RESET;


	spiSelect(&SPID4);
	spiSend(&SPID4, 2, txbuf);
	spiUnselect(&SPID4);

	delay(20);

	for(int i = 0; i < 8; i++){

		txbuf[0] = MS_PROM + (i << 1);

		spiSelect(&SPID4);
		spiExchange(&SPID4, 4, txbuf, rxbuf);
		spiUnselect(&SPID4);

		ms_prom[i] = (uint16_t)((rxbuf[1] << 8) | (rxbuf[2]));

		debug("MS_PROM %u, %u", i, ms_prom[i]);
	}

}

void get_ms_data(void){

	uint8_t txbuf[3], rxbuf[18];
	uint32_t D1, D2;

	txbuf[0] = MS_D1_1024;

	spiSelect(&SPID4);
	spiSend(&SPID4, 2, txbuf);
	spiUnselect(&SPID4);

	delay(10);

	txbuf[0] = MS_ADC;

	spiSelect(&SPID4);
	spiExchange(&SPID4, 6, txbuf, rxbuf);
	spiUnselect(&SPID4);

	D1 = ((uint32_t)rxbuf[1] << 16) | ((int32_t)rxbuf[2] << 8) | rxbuf[3];

//	debug("D1 %u", D1);



	txbuf[0] = MS_D2_1024;

	spiSelect(&SPID4);
	spiSend(&SPID4, 2, txbuf);
	spiUnselect(&SPID4);

	delay(10);

	txbuf[0] = MS_ADC;

	spiSelect(&SPID4);
	spiExchange(&SPID4, 6, txbuf, rxbuf);
	spiUnselect(&SPID4);

	D2 = ((uint32_t)rxbuf[1] << 16) | ((int32_t)rxbuf[2] << 8) | rxbuf[3];

	debug("Ms D1 %u, D2 %u", D1, D2);





}
