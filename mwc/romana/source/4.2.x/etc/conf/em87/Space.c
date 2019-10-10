/* Generated from Space.spc on Thu Feb 17 16:09:11 1994 CST */
/*
 * Configurable information for "em87" floating point emulator.
 */

#define __KERNEL__	 1

#include <signal.h>
#include <sys/ndp.h>
#include "conf.h"

/*
 * Emulator configuration.
 * Default values shown.
 *
 * ndpCW = 0x0272;	NDP Control Word at start of each NDP process.
 * ndpDump = 0;		Change to 1 for NDP register dump on FP exceptions.
 * ndpType = NDP_TYPE_AUTO;	Let CPU do NDP type sensing.
 * ndpEmSig = SIGFPE;	Signal sent on receiving emulator traps.
 */

short	ndpCW = NDP_CW;
short	ndpDump = NDP_DUMP;
short	ndpType = NDP_TYPE;

int	ndpEmSig = SIGFPE;

/* Emulator-related function pointers. */

int	math_emulate();
int	kfsave();
int	kfrstor();

int	(*ndpEmFn)() = math_emulate;
int	(*ndpKfsave)() = kfsave;
int	(*ndpKfrstor)() = kfrstor;
