#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * Code to manage the processor interrupt-priority level.
 *
 * This module exists to define external-linkage versions of the macros given
 * in <sys/inline.h>, and to give a manual-page entry.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 */

#include <common/ccompat.h>
#include <sys/inline.h>


#if	__COHERENT__
#error	This file is not for the Coherent version of STREAMS
#endif

/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	spl... ()	Block/allow interrupts on a processor.
 *
 *-SYNOPSIS:
 *	#include <sys/inline.h>
 *
 *	pl_t splbase ();
 *	pl_t spltimeout ();
 *	pl_t spldisk ();
 *	pl_t splstr ();
 *	pl_t splhi ();
 *    	pl_t splx (pl_t oldlevel);
 *
 *-ARGUMENTS:
 *	oldlevel	Last set priority value (only splx () has an input
 *			argument)
 *
 *-DESCRIPTION:
 *	The spl... () functions block or allow servicing of interrupts on the
 *	processor on which the function is called. Hardware devices are
 *	assigned to interrupt priority levels depending on the type of device.
 *	Each spl... () function which blocks interrupts is associated with
 *	with some machine-dependent interrupt priority level and will prevent
 *	interrupts occurring at or below this priority level from being
 *	serviced on the processor on which the spl... () function is called.
 *
 *	On a multiprocessor system, interrupts may be serviced by more than
 *	one processor, and therefore use of an spl... () function alone is not
 *	sufficient to prevent interrupt code from executing and manipulating
 *	driver data structures during a critical section. Drivers that must
 *	prevent execution of interrupt-level code in order to protect the
 *	integrity of their should use basic locks or read/write locks for
 *	this purpose [see LOCK_ALLOC () or RW_ALLOC ()].
 *
 *	Calling a given spl... () function will block interrupts specified for
 *	that function as well as all interrupts at equal an lower levels. The
 *	notion of low vs. high levels assumes a defined order of priority
 *	level. The following partial order is defined:
 *		splbase <= spltimeout <= spldisk, splstr <= splhi
 *
 *	The ordering of spldisk () and splstr () relative to each other is not
 *	defined.
 *
 *-RETURN VALUE:
 *	All spl... () functions return the previous priority level.
 *
 *-LEVEL:
 *	Base or Interrupt.
 *
 *-NOTES:
 *	All spl... () functions do not sleep.
 *
 *	Driver-defined basic locks and read/write locks may be held across
 *	calls to these functions, but the spl... () call must not cause the
 *	priority level to be lowered below the level associated with the
 *	lock.
 *
 *	Driver-defined sleep locks may be held across calls to these
 *	functions.
 *
 *	When setting a given priority level, the previous level returned
 *	should be saved and splx (), UNLOCK () or RW_UNLOCK () should be
 *	used as appropriate to restore this level.
 *
 *	Interrupt-level code must never lower the interrupt priority level
 *	below the level at which the interrupt handler was entered. For
 *	example, is an interrupt handler is entered at the priority level
 *	associated with spldisk (), the handler must not call spltimeout ().
 */

#if	__USE_PROTO__
pl_t (splbase) (void)
#else
pl_t
splbase __ARGS (())
#endif
{
#ifdef	splbase
	return splbase ();
#else
	return splx (plbase);
#endif
}


#if	__USE_PROTO__
pl_t (spltimeout) (void)
#else
pl_t
spltimeout __ARGS (())
#endif
{
#ifdef	spltimeout
	return spltimeout ();
#else
	return splx (pltimeout);
#endif
}


#if	__USE_PROTO__
pl_t (spldisk) (void)
#else
pl_t
spldisk __ARGS (())
#endif
{
#ifdef	spldisk
	return spldisk ();
#else
	return splx (pldisk);
#endif
}


#if	__USE_PROTO__
pl_t (splstr) (void)
#else
pl_t
splstr __ARGS (())
#endif
{
#ifdef	splstr
	return splstr ();
#else
	return splx (plstr);
#endif
}


#if	__USE_PROTO__
pl_t (splhi) (void)
#else
pl_t
splhi __ARGS (())
#endif
{
#ifdef	splhi
	return splhi ();
#else
	return splx (plhi);
#endif
}

#if	__USE_PROTO__
pl_t (splx) (pl_t newpl)
#else
pl_t
splx __ARGS ((newpl))
pl_t		newpl;
#endif
{
#ifdef	splx
	return splx (newpl);
#else
	pl_t		prev_pl;
	intmask_t	mask = __GET_BASE_MASK () | _masktab [newpl];

	__OUT_IPL_MASK (mask);

	prev_pl = (pl_t) ddi_cpu_data ()->dc_ipl;
	ddi_cpu_data ()->dc_ipl = newpl;

	return prev_pl;
#endif
}


/*
 *-STATUS:
 *	Private internal (used by various assertions)
 *
 *-NAME:
 *	splcmp		Compare processor priority levels
 *
 *-ARGUMENTS:
 *	l		Processor priority level considered to be on the LHS
 *			of the l < r relational comparison.
 *	r		Processor priority that "l" is to be compared with
 *
 *-DESCRIPTION:
 *	This functions performs a relational comparison of processor priority
 *	levels which can be used to test assertions about the relative order
 *	in which priority levels are set. It is guaranteed that this function
 *	will return values consistent with the partial order defined by the
 *	DDI/DKI.
 *
 *	Note that attempting to compare the values "plstr" and "pldisk" is not
 *	valid. In a system where processor priorities are implemented using
 *	masks, "pldisk" and "plstr" may be genuinely unordered in that the
 *	resulting masks may be disjoint. An implementation is free to diagnose
 *	such a comparison by causing a kernel panic.
 *
 *	This function is intended for use in ASSERT () statements only. If it
 *	is desired to set the processor priority level to the maximum of two
 *	values, use splraise () instead so that in an implementation which
 *	uses masks the logical union or intersection of the masks can be
 *	taken.
 *
 *-RETURNS:
 *	-1	if the level "l" compares less than "r"
 *	0	if "l" and "r" specify the same actual priority level
 *	1	if the level "l" compares greater that "r"
 *
 *-LEVEL:
 *	Base or Interrupt.
 *
 *-NOTES:
 *	This function does not sleep.
 *
 *	Driver-defined basic locks, read/write locks and sleep locks may be
 *	held across calls to this function.
 */

#if	__USE_PROTO__
int (splcmp) (pl_t l, pl_t r)
#else
int
splcmp __ARGS ((l, r))
pl_t		l;
pl_t		r;
#endif
{
	return (l < r) ? -1 : l != r;
}


/*
 *-STATUS:
 *	Private internal (used by freezestr ())
 *
 *-NAME:
 *	splraise	Raise processor priority level.
 *
 *-ARGUMENTS:
 *	pl		Processor interrupt priority level to raise to.
 *
 *-DESCRIPTION:
 *	splraise () changes the processor interrupt priority level to the
 *	maximum value defined by the previous level and the current level.
 *
 *	If the previous and requested priority levels are "pldisk" and "plstr"
 *	(in either order) this function does the appropriate action given that
 *	the "maximum" level in a mask-based system is in the fact the union or
 *	intersection of the two masks.
 *
 *-RETURNS:
 *	The previous processor interrupt priority level.
 *
 *-LEVEL:
 *	Base or Interrupt.
 *
 *-NOTES:
 *	This function does not sleep.
 *
 *	Driver-defined basic locks, read/write locks and sleep locks may be
 *	held across calls to this function.
 */

#if	__USE_PROTO__
pl_t (splraise) (pl_t newpl)
#else
pl_t
splraise __ARGS ((newpl))
pl_t		newpl;
#endif
{
#ifdef	splraise
	return splraise (newpl);
#else
	pl_t		prev_pl;

	if (newpl > (prev_pl = (pl_t) ddi_cpu_data ()->dc_ipl)) {
		intmask_t	mask = __GET_BASE_MASK () | _masktab [newpl];

		__OUT_IPL_MASK (mask);
		ddi_cpu_data ()->dc_ipl = newpl;
	}

	return prev_pl;
#endif
}
