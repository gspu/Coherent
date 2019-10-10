/* Generated from Space.spc on Wed Aug 10 15:42:43 1994 CDT */
/*
 * Configurable information for "lp" parallel port output device driver.
 */

#define __KERNEL__	 1

#include "conf.h"

/*
 * Defaults in ()'s:
 *
 * LP0_OK (0) specifies whether LP0 is always THERE.
 * Note: since some PC clones lp ports can't be read,
 * their lpflag field has to be patched to 'LPTHERE'.
 * 1's place in LP0_OK is for lpt1, 2's for lpt2, 4's for lpt3.
 *
 * LPTIME (4) specifies number of ticks between polls.
 * LPWAIT (400) specifies loop counter to wait in poll.
 * LPTEST (1) specifies whether or not to test for on-line conditition.
 */
int	LP0_OK = (LPT1_LP0_BUG | (LPT2_LP0_BUG << 1) | (LPT3_LP0_BUG << 2));
int	LPTIME = LPTIME_SPEC;
int	LPWAIT = LPWAIT_SPEC;
int	LPTEST = LPTEST_SPEC;

/*
 * Port addresses.
 */
int	LPT1_BASE = LPT1_BASE_SPEC;
int	LPT2_BASE = LPT2_BASE_SPEC;
int	LPT3_BASE = LPT3_BASE_SPEC;
