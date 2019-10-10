/* Generated from Space.spc on Wed Aug 10 15:43:38 1994 CDT */
/*
 * Configurable information for COHERENT semaphores.
 */

#define __KERNEL__	 1

#include "conf.h"

unsigned SEMMNI = SEMMNI_SPEC;	/* max # of the semaphore sets, systemwide */
unsigned SEMMNS = SEMMNS_SPEC;	/* max # of semaphores, systemwide */
unsigned SEMMSL = SEMMSL_SPEC;	/* max # of semaphores per set */
unsigned SEMVMX = SEMVMX_SPEC;	/* max value of any semaphore */
