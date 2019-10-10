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
void (__assert_fail2) (__CONST__ char * exp, __CONST__ char * info,
		       int lineno)	
#else
void
__assert_fail2 __ARGS ((exp, info, lineno))
__CONST__ char * exp;
__CONST__ char * info;
int		lineno;
#endif
{
}

#if	__COHERENT__
#include <sys/cmn_err.h>

#if	__USE_PROTO__
void (cmn_err) (int level, char * format, ...)
#else
void
cmn_err __ARGS ((level, format))
int		level;
char	      *	format;
#endif
{
	switch(level) {
	case CE_CONT:
		printf("%r", & format);
		break;
	case CE_NOTE:
		printf("NOTICE: %r\n", & format);
		break;
	case CE_WARN:
		printf("WARNING: %r\n", & format);
		break;
	case CE_PANIC:
		panic("%r", & format);
		break;
	}
}
#endif

/* copyin() and copyout() */

#define	TOUSER(usr,ker,n)	kucopy ((caddr_t) ker, (caddr_t) usr, n)
#define	FROMUSER(ker,usr,n)	ukcopy ((caddr_t) usr, (caddr_t) ker, n)

#if	__USE_PROTO__
int (copyin) (_VOID * userbuf, _VOID * driverbuf, size_t count)
#else
int
copyin __ARGS ((userbuf, driverbuf, count))
_VOID	      *	userbuf;
_VOID	      *	driverbuf;
size_t		count;
#endif
{
	ASSERT (driverbuf != 0);

	return FROMUSER (driverbuf, userbuf, count) != count ? -1 : 0;
}

#if	__USE_PROTO__
int (copyout) (_VOID * driverbuf, _VOID * userbuf, size_t count)
#else
int
copyout __ARGS ((driverbuf, userbuf, count))
_VOID	      *	driverbuf;
_VOID	      *	userbuf;
size_t		count;
#endif
{
	ASSERT (driverbuf != NULL);

	return TOUSER (userbuf, driverbuf, count) != count ? -1 : 0;
}

