/*
 * sdc_v8.h
 *  
 *      Author: nikhil
 */

#ifndef DRV_SDC_V8_H_
#define DRV_SDC_V8_H_

#include "hal.h"
#include "ch.h"
#include "Setup.h"

extern bool fs_ready;

void start_sdc(void);

void log_start(void);

void log_update(void);

#endif /* DRV_SDC_V8_H_ */
