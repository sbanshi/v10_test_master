/*
 * pwm_v8.cpp
 *  
 *      Author: nikhil
 */

#include "pwm_v8.h"

extern float rcinput[8];

#define RC_IN_RANGE(x) ((x >= 0.9) && (x <= 2.3))

uint32_t start[7] = {0, 0, 0, 0, 0, 0, 0} ;

inline float convertCounterToMilliseconds(uint32_t start, uint32_t end) {
  return (((float)(end - start) / (float)STM32_SYSCLK) * 1000.0f) ;
}

static void extcb1(EXTDriver *extp, expchannel_t channel) {
  (void)extp ;
  (void)channel ;

  chSysLockFromIsr() ;
  if(palReadPad(RC1_PORT, RC1_PIN) == PAL_HIGH) {
    start[0] = halGetCounterValue() ;
  }
  else if((start[0] != 0) && (palReadPad(RC1_PORT, RC1_PIN) == PAL_LOW)) {
    float tmp = convertCounterToMilliseconds(start[0], halGetCounterValue()) ;
    if(RC_IN_RANGE(tmp))
      rcinput[0] = tmp*1000;
    start[0] = 0 ;
  }
  chSysUnlockFromIsr() ;
}

static void extcb2(EXTDriver *extp, expchannel_t channel) {
  (void)extp ;
  (void)channel ;

  chSysLockFromIsr() ;
  if(palReadPad(RC2_PORT, RC2_PIN) == PAL_HIGH) {
    start[1] = halGetCounterValue() ;
  }
  else if((start[1] != 0) && (palReadPad(RC2_PORT, RC2_PIN) == PAL_LOW)) {
    float tmp = convertCounterToMilliseconds(start[1], halGetCounterValue()) ;
    if(RC_IN_RANGE(tmp))
      rcinput[1] = tmp*1000;
    start[1] = 0 ;
  }
  chSysUnlockFromIsr() ;
}

static void extcb3(EXTDriver *extp, expchannel_t channel) {
  (void)extp ;
  (void)channel ;

  chSysLockFromIsr() ;
  if(palReadPad(RC3_PORT, RC3_PIN) == PAL_HIGH) {
    start[2] = halGetCounterValue() ;
  }
  else if((start[2] != 0) && (palReadPad(RC3_PORT, RC3_PIN) == PAL_LOW)) {
    float tmp = convertCounterToMilliseconds(start[2], halGetCounterValue()) ;
    if(RC_IN_RANGE(tmp))
      rcinput[2] = tmp*1000;
    start[2] = 0 ;
  }
  chSysUnlockFromIsr() ;
}

static void extcb4(EXTDriver *extp, expchannel_t channel) {
  (void)extp ;
  (void)channel ;

  chSysLockFromIsr() ;
  if(palReadPad(RC4_PORT, RC4_PIN) == PAL_HIGH) {
    start[3] = halGetCounterValue() ;
  }
  else if((start[3] != 0) && (palReadPad(RC4_PORT, RC4_PIN) == PAL_LOW)) {
    float tmp = convertCounterToMilliseconds(start[3], halGetCounterValue()) ;
    if(RC_IN_RANGE(tmp))
      rcinput[3] = tmp*1000;
    start[3] = 0 ;
  }
  chSysUnlockFromIsr() ;
}

static void extcb5(EXTDriver *extp, expchannel_t channel) {
  (void)extp ;
  (void)channel ;

  chSysLockFromIsr() ;
  if(palReadPad(RC5_PORT, RC5_PIN) == PAL_HIGH) {
    start[4] = halGetCounterValue() ;
  }
  else if((start[4] != 0) && (palReadPad(RC5_PORT, RC5_PIN) == PAL_LOW)) {
    float tmp = convertCounterToMilliseconds(start[4], halGetCounterValue()) ;
    if(RC_IN_RANGE(tmp))
      rcinput[4] = tmp*1000;
    start[4] = 0 ;
  }
  chSysUnlockFromIsr() ;
}

static void extcb6(EXTDriver *extp, expchannel_t channel) {
  (void)extp ;
  (void)channel ;

  chSysLockFromIsr() ;
  if(palReadPad(RC6_PORT, RC6_PIN) == PAL_HIGH) {
    start[5] = halGetCounterValue() ;
  }
  else if((start[5] != 0) && (palReadPad(RC6_PORT, RC6_PIN) == PAL_LOW)) {
    float tmp = convertCounterToMilliseconds(start[5], halGetCounterValue()) ;
    if(RC_IN_RANGE(tmp))
      rcinput[5] = tmp*1000;
    start[5] = 0 ;
  }
  chSysUnlockFromIsr() ;
}


static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, extcb2},
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, extcb1},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb6},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb5},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb4},
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb3},
    {EXT_CH_MODE_DISABLED, NULL}
  },

};

void start_reciever(void) {

	extStart(&EXTD1, &extcfg);
	debug("Receiver Started!");

}

static PWMConfig pwmcfg3 = {
  PWM_CLK_FREQUENCY,
  PWM_PERIOD,
  NULL,
  {
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0
};

static PWMConfig pwmcfg4 = {
  PWM_CLK_FREQUENCY,
  PWM_PERIOD,
  NULL,
  {
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0,
};

void start_servo(void){

	pwmStart(&PWMD3, &pwmcfg3);
	delay(10);

	pwmStart(&PWMD4, &pwmcfg4);
	delay(10);

}

void set_servo(uint16_t servo, float wid){
	switch(servo) {
	case 0:
		pwmEnableChannelI(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, PWM_PERIOD, wid));
		break;
	case 1:
		pwmEnableChannelI(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, PWM_PERIOD, wid));
		break;
	case 2:
		pwmEnableChannelI(&PWMD4, 2, PWM_FRACTION_TO_WIDTH(&PWMD4, PWM_PERIOD, wid));
		break;
	case 3:
		pwmEnableChannelI(&PWMD4, 3, PWM_FRACTION_TO_WIDTH(&PWMD4, PWM_PERIOD, wid));
		break;

	case 4:
		pwmEnableChannelI(&PWMD3, 0, PWM_FRACTION_TO_WIDTH(&PWMD3, PWM_PERIOD, wid));
		break;
	case 5:
		pwmEnableChannelI(&PWMD3, 1, PWM_FRACTION_TO_WIDTH(&PWMD3, PWM_PERIOD, wid));
		break;
	case 6:
		pwmEnableChannelI(&PWMD3, 2, PWM_FRACTION_TO_WIDTH(&PWMD3, PWM_PERIOD, wid));
		break;
	case 7:
		pwmEnableChannelI(&PWMD3, 3, PWM_FRACTION_TO_WIDTH(&PWMD3, PWM_PERIOD, wid));
		break;

	default:
		break;

	}
}
