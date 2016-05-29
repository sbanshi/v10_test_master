/*
 * sdc_v8.cpp
 *  
 *      Author: nikhil
 */

#include "sdc_v10.h"

#include "ff.h"

#include<stdio.h>

#include "spi_v10.h"
/*===========================================================================*/
/* Card insertion monitor.                                                   */
/*===========================================================================*/

#define POLLING_INTERVAL                10
#define POLLING_DELAY                   10

/**
 * @brief   Card monitor timer.
 */
static VirtualTimer tmr;

/**
 * @brief   Debounce counter.
 */
static unsigned cnt;

/**
 * @brief   Card event sources.
 */
static EventSource inserted_event, removed_event;

/**
 * @brief   Insertion monitor timer callback function.
 *
 * @param[in] p         pointer to the @p BaseBlockDevice object
 *
 * @notapi
 */
static void tmrfunc(void *p) {
  BaseBlockDevice *bbdp = (BaseBlockDevice *)p;

  chSysLockFromIsr();
  if (cnt > 0) {
    if (blkIsInserted(bbdp)) {
      if (--cnt == 0) {
        chEvtBroadcastI(&inserted_event);
      }
    }
    else
      cnt = POLLING_INTERVAL;
  }
  else {
    if (!blkIsInserted(bbdp)) {
      cnt = POLLING_INTERVAL;
      chEvtBroadcastI(&removed_event);
    }
  }
  chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, bbdp);
  chSysUnlockFromIsr();
}

/**
 * @brief   Polling monitor start.
 *
 * @param[in] p         pointer to an object implementing @p BaseBlockDevice
 *
 * @notapi
 */
static void tmr_init(void *p) {

  chEvtInit(&inserted_event);
  chEvtInit(&removed_event);
  chSysLock();
  cnt = POLLING_INTERVAL;
  chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, p);
  chSysUnlock();
}

/*===========================================================================*/
/* FatFs related.                                                            */
/*===========================================================================*/

/**
 * @brief FS object.
 */
static FATFS SDC_FS;

/* FS mounted and ready.*/
bool fs_ready = false;

/* Generic large buffer.*/
static uint8_t fbuff[1024];

/*
 * Card insertion event.
 */
static void InsertHandler(eventid_t id) {
  FRESULT err;

  (void)id;
  /*
   * On insertion SDC initialization and FS mount.
   */
  if (sdcConnect(&SDCD1))
    return;

  err = f_mount(0, &SDC_FS);
  if (err != FR_OK) {
    sdcDisconnect(&SDCD1);
    return;
  }
  fs_ready = TRUE;
}

/*
 * Card removal event.
 */
static void RemoveHandler(eventid_t id) {

  (void)id;
  sdcDisconnect(&SDCD1);
  fs_ready = FALSE;
}


static WORKING_AREA(SDC_Thread, 4096);
static msg_t sdc_thd(void *arg) {

  (void)arg;
  chRegSetThreadName("sdc_thd");

  static const evhandler_t evhndl[] = {
    InsertHandler,
    RemoveHandler
  };
  struct EventListener el0, el1;

  sdcStart(&SDCD1, NULL);

  tmr_init(&SDCD1);

  chEvtRegister(&inserted_event, &el0, 0);
  chEvtRegister(&removed_event, &el1, 1);

  while (TRUE) {

	  chEvtDispatch(evhndl, chEvtWaitOneTimeout(ALL_EVENTS, MS2ST(500)));
	  delay(200);
  }
  return 0;
}

void start_sdc(void){
	chThdCreateStatic(SDC_Thread, sizeof(SDC_Thread), NORMALPRIO, sdc_thd, NULL);
}

FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}

int  endLine(FIL* _fil){
	int err;
	err = f_printf(_fil, "\n");
	return err;
}

int writeFloat(FIL* _fil, float f){
	int er;
	char buf_f[8];
	snprintf(buf_f, sizeof(buf_f), "%f", f);
	er = f_printf(_fil, "%s,", buf_f);
	return er;

}

char ahrs_fileName[32] = "AHRS.txt";

FIL ahrs_fil;

FRESULT fr;

void log_start(void){



	fr = open_append(&ahrs_fil, ahrs_fileName);
	if(fr == FR_OK){
		f_printf(&ahrs_fil, "SysTime,");
		f_printf(&ahrs_fil, "LoopTime,");
		f_printf(&ahrs_fil, " Roll,");
		f_printf(&ahrs_fil, " pitch,");
		f_printf(&ahrs_fil, " yaw,");
		f_printf(&ahrs_fil, " GyroX,");
		f_printf(&ahrs_fil, " GyroY,");
		f_printf(&ahrs_fil, " GyroZ,");
		f_printf(&ahrs_fil, " AccX,");
		f_printf(&ahrs_fil, " AccY,");
		f_printf(&ahrs_fil, " AccZ,");
	}
	else{
		debug("Prob writing SDC");
	}
	endLine(&ahrs_fil);
	fr = f_close(&ahrs_fil);
}



void log_update(void){
	fr = open_append(&ahrs_fil, ahrs_fileName);
	if(fr == FR_OK){
		writeFloat(&ahrs_fil, ax);
		writeFloat(&ahrs_fil, ay);
		writeFloat(&ahrs_fil, az);
		writeFloat(&ahrs_fil, gx);
		writeFloat(&ahrs_fil, gy);
		writeFloat(&ahrs_fil, gz);endLine(&ahrs_fil);

	fr = f_close(&ahrs_fil);
	}
	else{
		debug("failed to write log");
	}
}
