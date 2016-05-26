/*
 * pwm_v8.h
 *  
 *      Author: nikhil
 */

#ifndef DRV_PWM_V8_H_
#define DRV_PWM_V8_H_

#include "Setup.h"

void start_reciever(void);

#define PAL_LOW 	0
#define PAL_HIGH 	1

#define RC1_PORT    GPIOA
#define RC1_PIN      3

#define RC2_PORT    GPIOA
#define RC2_PIN      2

#define RC3_PORT    GPIOE
#define RC3_PIN      14

#define RC4_PORT    GPIOE
#define RC4_PIN      13

#define RC5_PORT    GPIOE
#define RC5_PIN      11

#define RC6_PORT    GPIOE
#define RC6_PIN      9

void start_reciever();

#define PWM_CLK_FREQUENCY	1000000
#define PWM_PERIOD			20000

void start_servo(void);

void set_servo(uint16_t servo, float pwm);

#endif /* DRV_PWM_V8_H_ */
