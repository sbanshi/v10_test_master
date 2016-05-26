/*
 * sbus.cpp
 *  
 *      Author: nikhil
 */


#include "sbus.h"

static uint16_t step = 0;
static uint16_t sbus_cnt = 1;
static uint8_t buffer[25];

float rcinput[8];

static uint16_t _channels[16];

float scale_to_pwm(uint16_t in){
	float t = 1000.0;
	if(t > 352)
	t = 1000.0 + (((float)(in-352)) * 0.74404);
	return t;

}

void parse_sbus(void){

	_channels[0]  = ((buffer[1]    |buffer[2]<<8)                 & 0x07FF);
	_channels[1]  = ((buffer[2]>>3 |buffer[3]<<5)                 & 0x07FF);
	_channels[2]  = ((buffer[3]>>6 |buffer[4]<<2 |buffer[5]<<10)  & 0x07FF);
	_channels[3]  = ((buffer[5]>>1 |buffer[6]<<7)                 & 0x07FF);
	_channels[4]  = ((buffer[6]>>4 |buffer[7]<<4)                 & 0x07FF);
	_channels[5]  = ((buffer[7]>>7 |buffer[8]<<1 |buffer[9]<<9)   & 0x07FF);
	_channels[6]  = ((buffer[9]>>2 |buffer[10]<<6)                & 0x07FF);
	_channels[7]  = ((buffer[10]>>5|buffer[11]<<3)                & 0x07FF);
	_channels[8]  = ((buffer[12]   |buffer[13]<<8)                & 0x07FF);
	_channels[9]  = ((buffer[13]>>3|buffer[14]<<5)                & 0x07FF);
	_channels[10] = ((buffer[14]>>6|buffer[15]<<2|buffer[16]<<10) & 0x07FF);
	_channels[11] = ((buffer[16]>>1|buffer[17]<<7)                & 0x07FF);
	_channels[12] = ((buffer[17]>>4|buffer[18]<<4)                & 0x07FF);
	_channels[13] = ((buffer[18]>>7|buffer[19]<<1|buffer[20]<<9)  & 0x07FF);
	_channels[14] = ((buffer[20]>>2|buffer[21]<<6)                & 0x07FF);
	_channels[15] = ((buffer[21]>>5|buffer[22]<<3)                & 0x07FF);

//	debug("%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u",
//			_channels[0], _channels[1], _channels[2], _channels[3],
//			_channels[4], _channels[5], _channels[6], _channels[7],
//			_channels[8], _channels[9], _channels[10], _channels[11],
//			_channels[12], _channels[13], _channels[14], _channels[15]);

	rcinput[0] = scale_to_pwm(_channels[0]);
	rcinput[1] = scale_to_pwm(_channels[1]);
	rcinput[2] = scale_to_pwm(_channels[2]);
	rcinput[3] = scale_to_pwm(_channels[3]);
	rcinput[4] = scale_to_pwm(_channels[4]);
	rcinput[5] = scale_to_pwm(_channels[5]);
	rcinput[6] = scale_to_pwm(_channels[5]);
	rcinput[7] = scale_to_pwm(_channels[7]);
}

void update_sbus(uint8_t data){

	switch(step){
	case 0:
		if(data == 0xF){
			buffer[0] = 0xF;
			step++;
		}
		break;
	case 1:
		buffer[sbus_cnt] = data;
		sbus_cnt ++;
		if(sbus_cnt > 24){
			parse_sbus();
			step = 0;
			sbus_cnt = 1;
		}
		break;
	default:
		step = 0;
		break;
	}
}

static WORKING_AREA(sbusThread, 2048);
static msg_t sbus(void *arg) {

	(void)arg;
	chRegSetThreadName("sbus");

	uint16_t bytes;
	uint8_t data;

  while (TRUE) {

		bytes = chQSpaceI(&(&SBUS_PORT)->iqueue);
//		debug("%u", bytes);
		for (uint16_t i=0; i < bytes; i++){
			data = sdGetTimeout(&SBUS_PORT, 2);
			update_sbus(data);
		}
		delay(10);
  }
  return 0;
}


void start_sbus(void){
	palSetPad(GPIOE, 7);
	chThdCreateStatic(sbusThread, sizeof(sbusThread), NORMALPRIO, sbus, NULL);
	delay(20);
}

void print_receiver(void) {
	debug("%f, %f, %f, %f, %f, %f, %f, %f", rcinput[0],
			rcinput[1], rcinput[2], rcinput[3], rcinput[4], rcinput[5], rcinput[6], rcinput[7]) ;
}

