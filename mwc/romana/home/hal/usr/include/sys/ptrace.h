/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __SYS_PTRACE_H__
#define	 __SYS_PTRACE_H__

#include <common/feature.h>
#include <sys/ksynch.h>

/*
 * Commands for ptrace() system call.  First argument is one of these.
 */

#define PTRACE_SETUP	0	/* init called by child */
#define PTRACE_RD_TXT	1	/* parent reads child text */
#define PTRACE_RD_DAT	2	/* parent reads child data */
#define PTRACE_RD_USR	3	/* parent reads child u area */
#define PTRACE_WR_TXT	4	/* parent writes child text */
#define PTRACE_WR_DAT	5	/* parent writes child data */
#define PTRACE_WR_USR	6	/* parent writes child u area */
#define PTRACE_RESUME	7	/* resume child, maybe fake signal to child */
#define PTRACE_TERM	8	/* terminate child */
#define PTRACE_SSTEP	9	/* single-step child, maybe fake sig to child */


/*
 * Pseudo offsets of registers in USR segment.
 *
 * When ptracing a process, the parent can access the register set
 * of the child using PTRACE_RD_USR and PTRACE_WR_USR commands and
 * an addr (third argument to the system call) from the following list.
 *
 * There are two ESP values available:
 *	PTRACE_UESP selects the user stack pointer - value from the child
 *	  process saved on entry to, e.g., the debug trap handler.
 *	  PTRACE_SS gives access to the corresponding stack selector.
 *	PTRACE_ESP is the stack pointer used by the trap handler itself.
 *
 * PTRACE_ERR will have the fault or trap number in case of exceptions.
 *
 * PTRACE_SIG is the current signal number.
 *
 * PTRACE_UEND is the size of accessible u area plus one - addresses
 *   greater than or equal to this value are not valid in ptrace
 *   read/write of USR segment.
 */

/* CPU trap frame - see sys/reg.h and ieeefp.h (struct _fpstackframe) */

#define	PTRACE_GS	0
#define	PTRACE_FS	4
#define	PTRACE_ES	8
#define	PTRACE_DS	12
#define	PTRACE_EDI	16
#define	PTRACE_ESI	20
#define	PTRACE_EBP	24
#define	PTRACE_ESP	28
#define	PTRACE_EBX	32
#define	PTRACE_EDX	36
#define	PTRACE_ECX	40
#define	PTRACE_EAX	44
#define PTRACE_TRAPNO	48
#define	PTRACE_ERR	52
#define	PTRACE_EIP	56
#define	PTRACE_CS	60
#define	PTRACE_EFL	64
#define	PTRACE_UESP	68
#define	PTRACE_SS	72
#define	PTRACE_SIG	76


/* NDP context - see ieeefp.h (struct _fpstate) */

#define PTRACE_FP_CW	80
#define	PTRACE_FP_SW	84
#define PTRACE_FP_TAG	88
#define	PTRACE_FP_EIP	92
#define PTRACE_FP_CS	96
#define	PTRACE_FP_FOO	100
#define PTRACE_FP_FS	104
#define	PTRACE_FP_ST0	108
#define PTRACE_FP_ST1	118
#define	PTRACE_FP_ST2	128
#define PTRACE_FP_ST3	138
#define	PTRACE_FP_ST4	148
#define PTRACE_FP_ST5	158
#define	PTRACE_FP_ST6	168
#define PTRACE_FP_ST7	178
#define	PTRACE_FP_STAT	188


/* Debug context */

#define	PTRACE_DR0	192
#define	PTRACE_DR1	196
#define	PTRACE_DR2	200
#define	PTRACE_DR3	204
#define	PTRACE_DR4	208
#define	PTRACE_DR5	212
#define	PTRACE_DR6	216
#define	PTRACE_DR7	220

#define	PTRACE_UEND	224	/* Offsets >= UEND are not valid */

#endif	/* ! defined (__SYS_PTRACE_H__) */
