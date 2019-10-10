/*
 * libc/gen/i386/signame.c
 * Status from wait (low byte).
 * i386.
 */

#include <signal.h>

char *signame[NSIG] ={
	"",
	"hangup",
	"interrupt",
	"quit",
	"illegal instruction",
	"trace/breakpoint trap",
	"abort",
	"emulator trap",
	"floating point exception",
	"killed",
	"bus error",
	"segmentation violation",
	"bad system call",
	"pipe broken",
	"alarm clock",
	"terminated",
	"signal 16",
	"signal 17",
	"child died",
	"restart",
	"window changed",
	"signal 21",
	"stream event polled"
};

/* libc/gen/i386/signame.c */
