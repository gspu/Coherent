/*
 * /usr/include/ieeefp.h
 *
 * Describes the stack frame presented to a user signal handler,
 * including (optional) floating point context.
 *
 * Revised Mon Mar 22 13:11:43 1993 CST
 */

#ifndef __IEEEFP_H__
#define __IEEEFP_H__

#include <sys/reg.h>
/*
 * "fpem" objects are for floating point emulation.
 *
 * The stackframe passed to a signal handler includes a pointer "fpsp".
 * This pointer is a (struct _fpstate *) when a true coprocessor is in use.
 * It is actually a (struct _fpemstate *), and must be cast thus, when
 * the emulator is used.
 * The declaration of "fpsp" is kept as (struct _fpstate *) for BCS
 * compatibility.
 */

struct _fpem_reg {
	char sign;
	char tag;
	long exp;
	unsigned sigl;
	unsigned sigh;
};

struct _fpstackframe {
	long	signo;
	long	regs[SS+1];
	struct	_fpstate *fpsp;
	char	*wsp;
};

struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpstate {
	unsigned long	cw,
			sw,
			tag,
			ipoff,
			cssel,
			dataoff,
			datasel;
	struct _fpreg	_st[8];
	unsigned long	status;
};

struct _fpemstate {
	unsigned long	cw,
			sw,
			tag,
			ipoff,
			cssel,
			dataoff,
			datasel,
			top;
	struct  _fpem_reg regs[8];	/* pseudo regs */
	unsigned char lookahead;
	unsigned long	status,
			entry_eip;
};

/* For kernel context in u area, which keeps either emulator or ndp status. */
union _fpcontext {
	struct _fpstate		fpstate;
	struct _fpemstate	fpemstate;
};
#endif
