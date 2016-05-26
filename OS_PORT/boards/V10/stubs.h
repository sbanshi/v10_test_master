/*
 * stubs.h
 *
 *  Created on: 28-Aug-2014
 *      Author: nikhil
 */

#ifndef STUBS_H_
#define STUBS_H_

#ifndef NULL
#define NULL ((void *)0)
#endif

///* The ABI requires a 64-bit type.  */
__extension__ typedef int __guard __attribute__((mode (__DI__)));

int __cxa_guard_acquire(__guard *);
void __cxa_guard_release (__guard *);
void __cxa_guard_abort (__guard *);

void *__dso_handle = NULL;


#endif /* STUBS_H_ */

