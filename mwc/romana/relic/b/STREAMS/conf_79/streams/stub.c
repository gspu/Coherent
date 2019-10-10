/*
 * This file contains the entry points into the STREAMS system called by the
 * Coherent kernel. In this file, they are all stubs, for use when other parts
 * of the kernel have had STREAMS support enabled but you really don't want
 * STREAMS.
 */

#define	_DDI_DKI	1
#define	_SYSV3		1

#include <common/ccompat.h>
#include <sys/inline.h>
#include <stddef.h>

#define	__KERNEL__	2

#if	__MSDOS__
#include <sys/_con.h>
#else
#include <sys/con.h>
#endif

#undef	__KERNEL__


#if	__USE_PROTO__
void (STREAMS_SCHEDULER) (void)
#else
void
STREAMS_SCHEDULER __ARGS (())
#endif
{
}


/*
 * Shut things down, in the right order.
 */

#if	__USE_PROTO__
void (STREAMS_EXIT) (void)
#else
void
STREAMS_EXIT __ARGS (())
#endif
{
}


/*
 * Get an old Coherent "CON" entry.
 */

#if	__USE_PROTO__
CON * (STREAMS_GETCON) (o_dev_t dev)
#else
CON *
STREAMS_GETCON __ARGS ((dev))
o_dev_t		dev;
#endif
{
	return NULL;
}


/*
 * Start things up, in the right order. If we can't proceed, panic.
 */

#if	__USE_PROTO__
void (STREAMS_INIT) (void)
#else
void
STREAMS_INIT __ARGS (())
#endif
{
}


/*
 * Check STREAMS timeouts.
 */

#if	__USE_PROTO__
void (STREAMS_TIMEOUT) (void)
#else
void
STREAMS_TIMEOUT __ARGS (())
#endif
{
}


/*
 * Set an interrupt mask.
 */

#if	__USE_PROTO__
void (DDI_BASE_MASTER_MASK) (uchar_t mask)
#else
void
DDI_BASE_MASTER_MASK __ARGS ((mask))
uchar_t		mask;
#endif
{
	outb (__PICM__, mask);
}

#if	__USE_PROTO__
void (DDI_BASE_SLAVE_MASK) (uchar_t mask)
#else
void
DDI_BASE_SLAVE_MASK __ARGS ((mask))
uchar_t		mask;
#endif
{
	outb (__SPICM__, mask);
}

#if	__USE_PROTO__
int (__assert_fail2) (__CONST__ char * exp, __CONST__ char * info,
		      int lineno)	
#else
int
__assert_fail2 __ARGS ((exp, info, lineno))
__CONST__ char * exp;
__CONST__ char * info;
int		lineno;
#endif
{
	return 0;
}

