#include <stdlib.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "ff.h"
#include "chprintf.h"
#include "drv/pwm_v10.h"
#include "drv/sdc_v10.h"
#include "drv/spi_v10.h"
#include "Setup.h"
#include "ff.h"
#include "sbus.h"

extern SerialUSBDriver SDU1;

static WORKING_AREA(blinkerThread, 128);
static msg_t blinker(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");

  while (TRUE) {
//	  palTogglePad(GPIOD, 0);
	  palTogglePad(GPIOC, 5);
	  delay(fs_ready ? 500 : 125);
  }
  return 0;
}


void led_start_pattern(void){
/*	palSetPad(GPIOD, 0);
	palSetPad(GPIOD, 1);

	delay(500);

	palClearPad(GPIOD, 0);
	palClearPad(GPIOD, 1);

	delay(500);

	palSetPad(GPIOD, 0);
	palSetPad(GPIOD, 1);

	delay(500);

	palClearPad(GPIOD, 0);
	palClearPad(GPIOD, 1);*/

	palSetPad(GPIOC, 5);
	palSetPad(GPIOE, 10);

	delay(500);

	palClearPad(GPIOC, 5);
	palClearPad(GPIOE, 10);

	delay(500);

	palSetPad(GPIOC, 5);
	palSetPad(GPIOE, 10);

	delay(500);

	palClearPad(GPIOC, 5);
	palClearPad(GPIOE, 10);

}


int main(void){

	halInit();
	chSysInit();

	led_start_pattern();

	StartTelemetry();

	chThdCreateStatic(blinkerThread, sizeof(blinkerThread), NORMALPRIO-2, blinker, NULL);
	delay(500);

	palSetPad(GPIOB, 12);
	palSetPad(GPIOE, 12);
	palSetPad(GPIOD, 11);
//	palSetPad(GPIOE, 15);

	start_ms_spi();delay(500);

//	start_MPU();delay(500);

//	start_fram();
//
//	start_reciever();delay(500);
//
//	start_sdc();delay(500);
//
//	if(fs_ready)log_start();


	while(TRUE){

/*		palSetPad(GPIOE, 15);
		delay(5000);
		palClearPad(GPIOE, 15);
		delay(5000);*/

//		get_mpu_data();

		get_ms_data();

//		print_receiver();

//		start_ms_spi();
//		start_fram();
//
//		if(fs_ready)log_update();

//		delay(100);


		delay(1);

	}

	return 0;
}
