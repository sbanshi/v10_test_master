/*
 * Setup.h
 *
 *  Created on: 28-Aug-2014
 *      Author: nikhil
 */

#ifndef SETUP_H_
#define SETUP_H_

#include <string.h>
#include "ch.h"
#include "hal.h"

void StartI2C(void);

void StartTelemetry(void);

void debug(const char *fmt, ...);

uint32_t millis(void);

#endif /* SETUP_H_ */
