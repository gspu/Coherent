/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_TRACE_H__
#define	__KERNEL_TRACE_H__

#include <kernel/proc_lib.h>
#include <sys/cmn_err.h>
#include <common/ccompat.h>

/*
 * This header contains the MWC tracing macros.
 */

#define CHIRP(ch)		chirp(ch)
#define _CHIRP(ch, locn)	_chirp(ch, locn)

/*
 * Control flags for tracing features.
 *
 * t_piggy bits:
 *	0x0001	pexit()
 *	0x0002	286 system calls
 *	0x0004	386 system calls
 *	0x0008	uioctl()
 *	0x0020	ssread()/sswrite()	in io.386/ss.c
 *	0x0200	uread()			in coh.386/sys3.c
 *	0x0400	ssblock()		in io.386/ss.c
 *	0x1000	XP_DATA_IN/OUT		in state machine in io.386/ss.c
 *	0x10000	open() bad exit values	in coh.386/sys3.c
 *
 * t_hal bits:
 *	0x0001	TCSETA/TCSETAF/TCSETAW ioctl's
 *	0x0002	TIOCSETP/TIOCSETN ioctl's
 *	0x0004	al[01] verbose mode (see alx.c); asy devmsgs
 *	0x0008	obrk()
 *	0x0010	pipe open/read/write
 *	0x0020	1= NO c_grow() calls;  alx overruns
 *	0x0040	monitor number of free clists
 *	0x0080	'+' when asleep waiting for free clist
 *	0x0100	trace c_grow() calls
 *	0x0200	run check_slot() on alloc() and ubrk()
 *	0x0400	async opens, closes, speed changes
 *	0x0800	asy, all interrupts
 *	0x1000	explanation of page faults
 *	0x2000
 *	0x4000	all traps
 *	0x8000	ubrk()
 *
 * t_vlad bits:
 *	0x0001
 *	0x0002	fcntl
 *
 * t_errno bits:
 *	0x0001	trace u.u_error settings
 *	0x0002  trace 386 system calls
 * t_con - trace console events
 *
 * t_msgq bits:
 *	0x0001	memory alloc
 *	0x0002	permissions
 */

#define	TRACE_FILESYS	128
#define	TRACE_INODE	64
#define	TRACE_PIGGY	32
#define	TRACE_CON	16
#define	TRACE_MSGQ	8
#define	TRACE_VLAD	4
#define	TRACE_HAL	2
#define	TRACE_ERRNO	1

#if	TRACER & TRACE_PIGGY

/*
 * T_PIGGY() is for piggy-controlled tracing.
 */

extern unsigned t_piggy;
#define T_PIGGY(flag, expr)	((t_piggy & (flag)) != 0 ? (void) (expr) : \
							   (void) 0)
#else
#define T_PIGGY(f,c)			((void) 0)
#endif


#if	TRACER & TRACE_HAL

/*
 * T_HAL() is for hal-controlled tracing.
 */

extern unsigned t_hal;
#define T_HAL(flag, expr)	((t_hal & (flag)) != 0 ? (void) (expr) : \
							 (void) 0)
#else
#define T_HAL(f,c)			((void) 0)
#endif


#if	TRACER & TRACE_VLAD
/*
 * T_VLAD() is for vlad-controlled tracing.
 */
extern unsigned t_vlad;
#define T_VLAD(flag, expr)	((t_vlad & (flag)) != 0 ? (void) (expr) : \
							  (void) 0)
#else
#define T_VLAD(f,c)			((void) 0)
#endif


#if	TRACER & TRACE_MSGQ
/*
 * T_MSGQ() is for message queue-controlled tracing.
 */
extern unsigned t_msgq;
#define T_MSGQ(flag, expr)	((t_msgq & (flag)) != 0 ? (void) (expr) : \
							  (void) 0)
#else
#define T_MSGQ(f,c)			((void) 0)
#endif


#if	TRACER & TRACE_ERRNO
/*
 * SET_U_ERROR() is for error/system-call tracing.
 */

extern unsigned t_errno;

#define	T_ERRNO(mask, cmd) \
		((t_errno & mask) != 0 ? (void) (cmd) : (void) 0)
#else
#define T_ERRNO(mask, msg)	((void) 0)
#endif

#define SET_U_ERROR(errno, msg)	\
	(set_user_error (errno), \
	 T_ERRNO (1, cmn_err (CE_NOTE, "u_error: %d: %s:\n", (errno), (msg))))

#if	TRACER & TRACE_CON

/*
 * T_CON() is for video & keyboard tracing.
 */
extern unsigned t_con;
#define T_CON(flag, expr)	((t_con & (flag)) != 0 ? (void) (expr) : \
							 (void) 0)

#else
#define T_CON(f,c)			((void) 0)
#endif

#if	TRACER & TRACE_INODE

extern	unsigned short	t_inumber;

#define	T_INODE(ip, cmd) \
		(((ip)->i_flag & IFTRACE) != 0 ? (void) (cmd) : (void) 0)
#else
#define	T_INODE(ip, cmd)	((void) 0)
#endif

#if	TRACER & TRACE_FILESYS

extern	unsigned short	t_filesys;
extern	unsigned short	t_filedev;

#define	T_FILESYS(dev, mask, cmd) \
		((t_filesys & (mask)) != 0 && \
		 (dev == t_filedev || t_filedev == 0) ? \
		 (void) (cmd) : (void) 0)
#else
#define	T_FILESYS(dev, mask, cmd)	((void) 0)
#endif

void		_chirp		__PROTO ((char c, int off));
void		chirp		__PROTO ((char c));
void		mchirp		__PROTO ((char c));
void		strchirp	__PROTO ((char * str));

#endif	/* ! defined (__KERNEL_TRACE_H__) */
