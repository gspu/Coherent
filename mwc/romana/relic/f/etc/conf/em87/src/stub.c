/*
 * stub.c file for FP emulation.
 */
#define __KERNEL__ 1

#include <signal.h>
#include <sys/ndp.h>

int	(*ndpEmFn)() = 0;
int	(*ndpKfsave)() = 0;
int	(*ndpKfrstor)() = 0;

short	ndpCW = 0x0272;
short	ndpDump = 0;
short	ndpType = NDP_TYPE_AUTO;
int	ndpEmSig = SIGFPE;
