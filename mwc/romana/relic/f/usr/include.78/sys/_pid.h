#ifndef	__SYS__PID_H__
#define	__SYS__PID_H__

/*
 * This internal header is intended as the sole point of definition for the
 * POSIX.1 data type "pid_t". We simply rely on another internal definition
 * of a private name (which exists so that headers can declare a "pid_t" datum
 * without exporting the "pid_t" type into the user's namespace).
 */

#include <sys/__pid.h>

typedef	__pid_t		pid_t;

#endif	/* ! defined (__SYS__PID_H__) */
