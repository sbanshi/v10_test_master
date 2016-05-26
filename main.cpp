#include <stdlib.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "ff.h"
#include "chprintf.h"
#include "Setup.h"
#include "ff.h"
#include "spi_v8.h"
#include "sdc_v8.h"
#include "pwm_v8.h"
#include "sbus.h"

static WORKING_AREA(blinkerThread, 128);
static msg_t blinker(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");

  while (TRUE) {
	  palTogglePad(GPIOD, 0);
	  delay(fs_ready ? 500 : 125);
  }
  return 0;
}

void led_start_pattern(void){
	palSetPad(GPIOD, 0);
	palSetPad(GPIOD, 1);

	delay(500);

	palClearPad(GPIOD, 0);
	palClearPad(GPIOD, 1);

	delay(500);

	palSetPad(GPIOD, 0);
	palSetPad(GPIOD, 1);

	delay(500);

	palClearPad(GPIOD, 0);
	palClearPad(GPIOD, 1);
}


int main(void){

	halInit();
	chSysInit();

	led_start_pattern();

	StartTelemetry();

	chThdCreateStatic(blinkerThread, sizeof(blinkerThread), NORMALPRIO-2, blinker, NULL);
	delay(500);

//	start_MPU();delay(500);

	start_ms_spi();delay(500);


	while(TRUE){

//		get_mpu_data();

		get_ms_data();

		delay(100);
	}

	return 0;
}
