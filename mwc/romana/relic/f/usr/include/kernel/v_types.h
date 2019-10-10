#ifndef	__KERNEL_V_TYPES_H__
#define	__KERNEL_V_TYPES_H__

/*
 * Type-name definitions for compatibility with the System V manuals.
 *
 * This file contains type-names that are referenced by STREAMS structures
 * and which we should at least try and build equivalents to.
 *
 * The exact contents of these structure and type definitions have been taken
 * from the System V Release 4 STREAMS Programmer's Guide, and the System V
 * Release 4 Multi-Processor DDI/DKI Reference Manual.
 *
 * The System V definition have been used specifically and only to aid in the
 * support of binary compatibility for STREAMS drivers. This may be an
 * unattainable goal, since the values of certain constant definitions have
 * not been published. However, we include all the published data in the
 * hope of reducing the number of gratuitous differences.
 */
/*
 * The types defined in this file are available from <sys/types.h> under the
 * DDI/DKI, but that is a little problematical, in that this file has to live
 * between two worlds. We require that any file compiled under the DDI/DKI
 * environment be compiled such that <sys/types.h> knows to include these
 * definitions rather than any old definitions. A few files need to work with
 * both the new and old definitions simultaneously; these files should define
 * _TRANSITION to prefix symbols that are sources of conflict with the prefix
 * "n_" (by symmetry with the use of "o_" for old types).
 */

#include <common/ccompat.h>
#include <common/__clock.h>		/* for drv_ prototypes */
#include <common/_cred.h>
#include <kernel/__pl.h>
#include <kernel/pri.h>
#include <kernel/x86io.h>
#include <kernel/_lock.h>
#include <kernel/_toid.h>
#include <sys/types.h>


/*
 * Types used in the System V DDI/DKI
 */

typedef unsigned short	bool_t;		/* boolean variable type */
typedef	__pl_t		pl_t;


/*
 * The major/minor device number concepts have been altered a little under
 * System V release 4 to encompass the idea of internal and external minor
 * device numbers.
 *
 * Under this concept the "bdevsw" and "cdevsw" can map down from 32-bit
 * device numbers to some smaller internal major/minor space.
 */

typedef	unsigned short	minor_t;	/* external minor device number */
typedef	unsigned short	major_t;	/* external major device number */

#define	NODEV		((major_t) -1)


/*
 * The following constants are used to request values from the drv_getparm ()
 * function and to set kernel variables via the drv_setparm () function.
 */

enum {
	LBOLT,			/* Clock ticks since last kernel reboot */
	UPROCP,			/* "proc_t *" for use with vtop () */
	UCRED,			/* "cred_t *" for use with drv_priv () */
	TIME,			/* POSIX-format time, seconds since 1970 */

	SYSCANC,		/* count of cooked terminal characters */
	SYSMINT,		/* count of modem interrupts */
	SYSOUTC,		/* count of characters output to terminal */
	SYSRAWC,		/* count of raw terminal characters */
	SYSRINT,		/* count of terminal receive interrupts */
	SYSXINT			/* count of terminal transmit interrupts */
};


/*
 * We need a TRUE and FALSE along the lines of the old-style <bool.h>
 */

enum { FALSE = 0, TRUE = 1 };


/*
 * Type of function argument to itimeout ().
 */

typedef	void	     (* __tfuncp_t)	__PROTO ((_VOID * _arg));


/*
 * Note that ltimeout () below is a local experimental extension to the
 * DDI/DKI, and is the reason for <kernel/_lock.h>.
 */

__EXTERN_C_BEGIN__

int		drv_getparm	__PROTO ((ulong_t _parm, ulong_t * _value_p));
__clock_t	drv_hztousec	__PROTO ((__clock_t _ticks));
int		drv_priv	__PROTO ((cred_t * _credp));
int		drv_setparm	__PROTO ((ulong_t _parm, ulong_t _value));
__clock_t	drv_usectohz	__PROTO ((__clock_t _microsecs));

toid_t		itimeout	__PROTO ((__tfuncp_t _fn, _VOID * _arg,
					  __clock_t _ticks, pl_t _pl));
toid_t		ltimeout	__PROTO ((__tfuncp_t _fn, _VOID * _arg,
					  __clock_t _ticks, __lock_t * lockp,
					  pl_t _pl));
void		untimeout	__PROTO ((toid_t));

minor_t		etoimajor	__PROTO ((major_t _emaj));
major_t		getemajor	__PROTO ((n_dev_t _dev));
minor_t		geteminor	__PROTO ((n_dev_t _dev));
major_t		getmajor	__PROTO ((n_dev_t _dev));
minor_t		getminor	__PROTO ((n_dev_t _dev));
major_t		itoemajor	__PROTO ((major_t _imaj, major_t _prevemaj));
n_dev_t		makedevice	__PROTO ((major_t _majnum, minor_t _minnum));

#if	_SYSV3

unsigned	major		__PROTO ((o_dev_t _dev));
unsigned	minor		__PROTO ((o_dev_t _dev));
o_dev_t		makedev		__PROTO ((unsigned _maj, unsigned _min));

#endif

int		copyin		__PROTO ((_VOID * _userbuf,
					  _VOID * _driverbuf, size_t _count));
int		copyout		__PROTO ((_VOID * _driverbuf,
					  _VOID * _userbuf, size_t _count));

void		bcopy		__PROTO ((__CONST__ _VOID * _from,
					  _VOID * _to, size_t _bcount));
void		bzero		__PROTO ((_VOID * _addr, size_t _bytes));

ulong_t		btop		__PROTO ((ulong_t _numbytes));
ulong_t		btopr		__PROTO ((ulong_t _numbytes));
ulong_t		ptob		__PROTO ((ulong_t _numpages));

__VOID__      *	proc_ref	__PROTO ((void));
int		proc_signal	__PROTO ((_VOID * _pref, int _sig));
void		proc_unref	__PROTO ((_VOID * _pref));

#ifdef	_DDI_DKI_IMPL
	/*
	 * Functions for the implementation! use at your own risk.
	 */
void		proc_kill_group	__PROTO ((pid_t _group, int _sig));

#endif

__EXTERN_C_END__


/*
 * 4Kbyte pages for the 386 mean a shift count of 12.
 */

#if	defined (GNUDOS) || __COHERENT__ || __BORLANDC__
# define	__PAGE_SHIFT	12
#endif

#define	__PAGE_MASK		((1UL << __PAGE_SHIFT) - 1)

#define	btop(n)			((ulong_t) (n) >> __PAGE_SHIFT)
#define	btopr(n)		(((ulong_t) (n) + __PAGE_MASK) >> __PAGE_SHIFT)
#define	ptob(n)			((ulong_t) (n) << __PAGE_SHIFT)


/*
 * Old-style Coherent functions, like modern-day getmajor (), but for the old
 * SVR3-style types.
 */

#if	_SYSV3

#define	major(dev)		(((dev) >> 8) & 0xFF)
#define	minor(dev)		((dev) & 0xFF)
#define	makedev(maj,min)	(((maj) << 8) | (min))

#endif

#endif	/* ! defined (__KERNEL_V_TYPES_H__) */
