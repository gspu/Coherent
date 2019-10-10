/*
 * /usr/include/sys/coherent.h
 *
 * Coherent-specific definitions, for linking kernels and related bits.
 *
 * Revised: Thu May 27 08:07:17 1993 CDT
 */
#ifndef	__SYS_COHERENT_H__
#define	__SYS_COHERENT_H__

#define	 KERNEL
#define	__KERNEL__
#include <sys/__paddr.h>
#include <sys/types.h>
#include <sys/_time.h>
#include <sys/param.h>
#include <sys/fun.h>
#include <sys/mmu.h>
#include <sys/uproc.h>
#ifdef _I386
#include <sys/reg.h>
#define v_sleep(a1,a2,a3,a4,a5)		w_sleep(a1,a2,a5)
#else
#include <sys/machine.h>
#define v_sleep(a1,a2,a3,a4,a5)		sleep(a1,a2,a3,a4)
#endif

#ifdef TRACER
#include <sys/mwc_coherent.h>
#else
#define SET_U_ERROR(errno, msg)	{ u.u_error = errno; }
#define T_HAL(f,c)
#define T_PIGGY(f,c)
#define T_VLAD(f,c)
#define T_CON(f,c)
#define T_MSGQ(f,c)
#define DV(v)
#endif /* TRACER */

#ifdef _I386
#define CHIRP(ch)		chirp(ch)
#define _CHIRP(ch, locn)	_chirp(ch, locn)
#if 1
#define kclear(buf, nbytes)		memset(buf, 0, nbytes)
#define kkcopy(src, dest, nbytes)	(memcpy(dest, src, nbytes),nbytes)
#endif
#else
#define CHIRP(ch)
#define _CHIRP(ch, locn)
#endif

/*
 * Null
 */
#ifndef	NULL		/* machine.h doesn't have any ideas */
#define NULL	0
#endif

/*
 * Storage management functions.
 */
extern	char		*alloc();
#define	kalloc(n)	alloc(allkp, n)
#define kfree(p)	free(p)

/*
 * Functions for copying between kernel and segments.
 */
#define kscopy(k, s, o, n)	kpcopy(k, s->s_paddr+o, n)
#define skcopy(s, o, k, n)	pkcopy(s->s_paddr+o, k, n)

/*
 * Time of day structure.
 */

extern struct _TIME_OF_DAY {
	time_t	t_time;			/* Time and date */
	int	t_tick;			/* Clock ticks into this second */
	int	t_zone;			/* Time zone */
	int	t_dstf;			/* Daylight saving time used */
} timer;

/*
 * General global variables.
 */
extern	int	 debflag;		/* General debug flag */
extern	int	 batflag;		/* Turn on clock flag */
extern	int	 outflag;		/* Device timeouts */
extern	int	 ttyflag;		/* Console is present */
extern	int	 mactype;		/* Machine type */
extern	unsigned utimer;		/* Unsigned timer */
extern	long	 lbolt;			/* Clock ticks since system startup */
extern	TIM	stimer;			/* Swap timer */
extern	unsigned msize;			/* Memory size in K */
extern	unsigned asize;			/* Alloc size in bytes */
extern	char	 *icodep;		/* Init code start */
extern	int	 icodes;		/* Init code size */
extern	dev_t	 rootdev;		/* Root device */
extern	dev_t	 swapdev;		/* Swap device */
extern	dev_t	 pipedev;		/* Pipe device */
extern	__paddr_t	 corebot;	/* Bottom of core */
extern	__paddr_t	 coretop;	 /* Top of core */
extern	__paddr_t	 holebot;	/* Bottom of I/O memory */
extern	__paddr_t	 holetop;	/* Top of I/O memory */
extern	daddr_t	 swapbot;		/* Bottom of swap */
extern	daddr_t	 swaptop;		/* Top of swap */
extern	__paddr_t	 clistp;	/* Base of clists */
extern	struct	 all *allkp;		/* Alloc space */
extern	int	 NSLOT;			/* Num of loadable driver data slots */
extern	int	 slotsz;		/* Size of loadable driver data slot */
extern	int *	 slotp;			/* Loadable driver pids/data space */
extern	int	 (*altclk)();		/* hook for polled devices */
extern	UPROC	 u;			/* Current user area. */

#ifdef _I386

extern	unsigned total_mem;		/* Total physical memory in bytes.  */
extern	SR	blockp;			/* Base of buffers */
extern	SR	allocp;
extern	int	dev_loaded;
extern	int	DUMP_LIM;

#else

extern	char	 *idatap;		/* Init data start */
extern	int	 idatas;		/* Init data size */
extern	__paddr_t	 blockp;	/* Base of buffers */
extern	int	 altsel;		/* for far polling, this is selector */
					/* ... for altclk; for near polling, */
					/* ... this is zero */
extern	int 	is_ps;			/* 1 if running on a PS/2            */	

#endif
#endif /* COHERENT_H */
