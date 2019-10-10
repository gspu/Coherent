#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * This file implements many of the drv_... () functions defined in the System
 * V DDI/DKI.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<common/__clock.h>
 *		__clock_t
 *	<sys/debug.h>
 *		ASSERT ()
 *	<sys/types.h>
 *		cred_t
 *		ulong_t
 *	<stddef.h>
 *		NULL
 *	<limits.h>
 *		CHAR_BIT
 */

#include <common/ccompat.h>
#include <sys/debug.h>
#include <sys/types.h>
#include <stddef.h>
#include <limits.h>

#include <sys/types.h>


#if	__COHERENT__

#include <sys/coherent.h>
#include <sys/param.h>

__LOCAL__ cred_t	_cred = {
	1, 0, 0, 0, 0, 0, NULL
};

#define	PRIVELEGED(credp)	(super ())
#define	SYSTEM_LBOLT()		(lbolt)
#define	SYSTEM_UPROCP()		(cprocp)
#define	SYSTEM_UCRED()		(& _cred)
#define	SYSTEM_TIME()		(timer.t_time)


/*
 * Converting between ticks and microseconds accurately is a nightmare in the
 * general case. I've make the routine that does this system-specific so that
 * it's easy to do; for Coherent, there are 1000 microseconds per tick (in
 * theory), so we can just multiply and/or divide.
 */

#if	(1000000L % HZ) != 0
#error	These routines tuned for an even number of microseconds per tick
#endif

#define	U_PER_TICK	(1000000L / HZ)

#elif	__BORLANDC__ || defined (GNUDOS)

#include <time.h>

__LOCAL__ __clock_t	_lbolt;
__LOCAL__ cred_t	_cred = {
	1, 0, 0, 0, 0, 0, NULL
};

#define	PRIVELEGED(credp)	0
#define	SYSTEM_LBOLT()		(_lbolt ++)
#define	SYSTEM_UPROCP()		(1)
#define	SYSTEM_UCRED()		(& _cred)
#define	SYSTEM_TIME()		(time (NULL))

/*
 * Under MSDOS, just use the 18.2Hz minimum.
 */

#define	U_PER_TICK		54945

#endif

#if	U_PER_TICK >= 1000

/*
 * Routines for time processing basic on microseconds per tick... these won't
 * work when the tick and microseconds times are too close or ticks are in
 * fact smaller than microseconds.
 */


#if	__USE_PROTO__
__clock_t (USEC_TO_HZ) (__clock_t usec)
#else
__clock_t
USEC_TO_HZ __ARGS ((usec))
__clock_t	usec;
#endif
{
	ulong_t		ticks = usec / U_PER_TICK;

	if ((usec % U_PER_TICK) != 0)
		ticks ++;

	return ticks;
}

#if	__USE_PROTO__
__clock_t (HZ_TO_USEC) (__clock_t ticks)
#else
__clock_t
HZ_TO_USEC __ARGS ((ticks))
__clock_t	ticks;
#endif
{
	ASSERT ((__clock_t) -1 > 0);

	if (ticks >= (__clock_t) -1 / U_PER_TICK)
		return (__clock_t) -1;

	return ticks * U_PER_TICK;
}

#endif


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	drv_getparm	Retrieve kernel state information.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/ddi.h>
 *
 *	int drv_getparm (ulong_t parm, ulong_t * value_p);
 *
 *-ARGUMENTS:
 *	parm		The kernel parameter to be obtained. Possible values
 *			are:
 *			LBOLT	Read the number of clock ticks since the last
 *				kernel reboot. The difference between the
 *				values returned from successive calls to
 *				retrieve this parameter provides an indication
 *				of the elapsed time between the calls in units
 *				of clock ticks. The length of a clock tick can
 *				vary across different implementations, and
 *				therefore drivers should not include any hard-
 *				coded assumptions about the length of a tick.
 *				The drv_hztousec () and drv_usectohz ()
 *				functions can be used to convert between clock
 *				ticks and microseconds.
 *
 *			UPROCP	Retrieve a pointer to the process structure
 *				for the current process. The value returned in
 *				"* value_p" is of type "(proc_t *)" and the
 *				only valid use of the value is as an argument
 *				to vtop (). Since this value is associated
 *				with the current process, the caller must have
 *				process context (that is, must be at base
 *				level) when attempting to retrieve this value.
 *				Also, this value should only be used in the
 *				context of the process in which it was
 *				retrieved.
 *
 *			UCRED	Retrieve a pointer to the credential structure
 *				describing the current user credentials for
 *				the current process. The value returned in
 *				"* value_p" is of type "(cred_t *)" and the
 *				only valid use of the value is an an argument
 *				to drv_priv (). Since this value is associated
 *				with the current process, the caller must have
 *				process context (ie, must be at base level)
 *				when attempting to retrieve this value. Also,
 *				this value should only be used in the context
 *				of the process in which it was retrieved.
 *
 *			TIME	Read the time in seconds. This is the same
 *				time value that is returned by the time ()
 *				system call. The value is defined as the time
 *				in seconds since "00:00:00 GMT, January 1,
 *				1970". This definition presupposes that the
 *				administrator has set the correct system time
 *				and date.
 *
 *	value_p		A pointer to the data space into which the value of
 *			the parameter is to be copied.
 *
 *-DESCRIPTION:
 *	drv_getparm () returns the value of the parameter specified by "parm"
 *	in the location pointed to by "value_p".
 *
 *	drv_getparm () does not explicitly check to see whether the driver has
 *	the appropriate context when the function is called. It is the
 *	responsibility of the driver to use this function only when it is
 *	appropriate to do so and to correctly declare the data space needed.
 *
 *-RETURN VALUE:
 *	If the function is successful, 0 is returned. Otherwise, -1 is
 *	returned to indicate that "parm" specified an invalid parameter.
 *
 *-LEVEL:
 *	Base only when using the UPROCP or UCRED argument values.
 *
 *	Base or interrupt when using the LBOLT or TIME argument values.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	drv_hztousec (), drv_priv (), drv_usectohz (), vtop ()
 */

#if	__USE_PROTO__
int (drv_getparm) (ulong_t parm, ulong_t * value_p)
#else
int
drv_getparm __ARGS ((parm, value_p))
ulong_t		parm;
ulong_t	      *	value_p;
#endif
{
	ASSERT (value_p != NULL);

	switch (parm) {

	case LBOLT:
		* value_p = SYSTEM_LBOLT ();
		break;

	case UPROCP:
		* value_p = (ulong_t) SYSTEM_UPROCP ();
		break;

	case UCRED:
		* value_p = (ulong_t) SYSTEM_UCRED ();
		break;

	case TIME:
		* value_p = SYSTEM_TIME ();
		break;

	default:
		return -1;
	}

	return 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	drv_hztousec	Convert clock ticks to microseconds.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/ddi.h>
 *
 *	clock_t	drv_hztousec (clock_t ticks);
 *
 *-ARGUMENTS:
 *	ticks		The number of clock ticks to convert to equivalent
 *			microseconds.
 *
 *-DESCRIPTION:
 *	drv_hztousec () converts the length of time expressed by "ticks",
 *	which is in units of clock ticks, into units of microseconds.
 *
 *	Several functions either take time values expressed in clock ticks as
 *	arguments [itimeout (), delay ()] or return time values expressed in
 *	clock ticks [drv_getparm ()]. The length of a clock tick can vary
 *	across different implementations and therefore drivers should not
 *	include any hard-coded assumptions about the length of a tick.
 *	drv_hztousec () and the complementary function drv_usectohz () can be
 *	used as necessary to convert between clock ticks and microseconds.
 *
 *-RETURN VALUE:
 *	The number of microseconds equivalent to the "ticks" argument. No
 *	error value is returned. If the microsecond equivalent to "ticks" is
 *	too large to be represented as a "clock_t", then the maximum "clock_t"
 *	value will be returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The time value returned by drv_getparm () with an "LBOLT" argument
 *	will frequently be too large to represent in microseconds as a
 *	"clock_t". When using drv_getparm () together with drv_hztousec () to
 *	time operations, drivers can help avoid overflow by converting the
 *	difference between return values from successive calls to
 *	drv_getparm () instead of trying to convert the return values
 *	themselves.
 *
 *-SEE ALSO:
 *	delay (), drv_getparm (), drv_usectohz (), dtimeout (), itimeout ()
 */

#if	__USE_PROTO__
__clock_t (drv_hztousec) (__clock_t ticks)
#else
__clock_t
drv_hztousec __ARGS ((ticks))
__clock_t	ticks;
#endif
{
	return HZ_TO_USEC (ticks);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	drv_priv	Determine whether credentials are priveleged.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/ddi.h>
 *
 *	int drv_priv (cred_t * crp);
 *
 *-ARGUMENTS:
 *	crp		Pointer to the user credential structure.
 *
 *-DESCRIPTION:
 *	The drv_priv () function determines whether the credentials specified
 *	by the credential structure pointer to by "crp" identify a priveleged
 *	process. This function should only be used when file access modes and
 *	special minor device numbers are insufficient to provide the necessary
 *	protection for the driver operation being performed. Calls to
 *	drv_priv () should replace all calls to suser () and any explicit
 *	checks for effective user ID equal to zero in driver code.
 *
 *	A credential structure pointer is passed into various driver entry
 *	point functions [open (), close (), read () and ioctl ()] and can also
 *	be obtained by calling drv_getparm () from base level driver code.
 *
 *-RETURN VALUE:
 *	This routine returns 0 is the specified credentials identify a
 *	priveleged process and EPERM otherwise.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The only valid use for a credential structure pointer is an an
 *	argument to drv_priv (). The contents of a credential structure are
 *	not defined by the DDI/DKI and a driver may not examine the contents
 *	of the structure directly.
 *
 *-SEE ALSO:
 *	drv_getparm ()
 */

#if	__USE_PROTO__
int (drv_priv) (cred_t * crp)
#else
int
drv_priv __ARGS ((crp))
cred_t	      *	crp;
#endif
{
	ASSERT (crp != NULL);

	return PRIVELEGED (crp);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	drv_setparm	Set kernel state information.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/ddi.h>
 *
 *	int drv_setparm (ulong_t parm, ulong_t value);
 *
 *-ARGUMENTS:
 *	parm		The kernel parameter to be updated. Possible values
 *			are:
 *			SYSCANC	Add "value" to the count of the number of
 *				characters received from a terminal device
 *				after the characters have been processed to
 *				remove special characters such as "break" or
 *				"backspace".
 *
 *			SYSMINT	Add "value" to the count of the number of
 *				modem interrupts received.
 *
 *			SYSOUTC	Add "value" to the count of the number of
 *				characters output to a terminal device.
 *
 *			SYSRAWC	Add "value" to the count of the number of
 *				characters received from a terminal device,
 *				before canonical processing has occurred.
 *
 *			SYSRINT	Add "value" to the count of the number of
 *				interrupts generated by data ready to be
 *				received from a terminal device.
 *
 *			SYSXINT	Add "value" to the count of the number of
 *				interrupts generated by data ready to be
 *				transmitted to a terminal device.
 *
 *	value		The value to be added to the parameter.
 *
 *-DESCRIPTION:
 *	drv_setparm () verifies that "parm" corresponds to a kernel parameter
 *	that may be modified. If the value of "parm" correspoonds to a
 *	parameter that may not be modified, -1 is returned. Otherwise, the
 *	parameter is incremented by "value".
 *
 *	No checking is performed to determine the validity of "value". It is
 *	the driver's reponsibility to guarantee the correctness of "value".
 *
 *-RETURN VALUE:
 *	If the function is successful, 0 is returned. Otherwise, -1 is
 *	returned to indicate that "parm" specified an invalid parameter.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	drv_getparm ()
 */

#if	__USE_PROTO__
int (drv_setparm) (ulong_t parm, ulong_t value)
#else
int
drv_setparm __ARGS ((parm, value))
ulong_t		parm;
ulong_t		value;
#endif
{
	switch (parm) {

	case SYSCANC:
	case SYSMINT:
	case SYSOUTC:
	case SYSRAWC:
	case SYSRINT:
	case SYSXINT:
		break;

	default:
		return -1;
	}

	return 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	drv_usectohz	Convert microseconds to clock ticks.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/ddi.h>
 *
 *	clock_t drv_usectohz (clock_t microsecs);
 *
 *-ARGUMENTS:
 *	microsecs	The number of microseconds to convert to equivalent
 *			clock ticks.
 *
 *-DESCRIPTION:
 *	drv_usectohz () converts the length of time expressed by "microsecs",
 *	which is in units of microseconds, into units of clock ticks.
 *
 *	Several functions either take time values expressed in clock ticks as
 *	arguments [itimeout (), delay ()] or return time values expressed in
 *	clock ticks [drv_getparm ()]. The length of a clock tick can vary
 *	across different implementations, and therefore drivers should not
 *	include any hard-coded assumptions about the length of a tick.
 *	drv_usectohz () and the complementary function drv_hztousec () can be
 *	used as necessary to convert between microseconds and clock ticks.
 *
 *-RETURN VALUE:
 *	The value returned is the smallest number of clock ticks that
 *	represent a time interval equal to or greater than the "microsecs"
 *	argument. No error value is returned. If the number of ticks
 *	equivalent to the "microsecs" argument is too large to be represented
 *	as a "clock_t", then the maximum "clock_t" value will be returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	delay (), drv_getparm (), drv_hztousec (), dtimeout (), itimeout ()
 */

#if	__USE_PROTO__
__clock_t (drv_usectohz) (__clock_t microsecs)
#else
__clock_t
drv_usectohz __ARGS ((microsecs))
__clock_t	microsecs;
#endif
{
	return USEC_TO_HZ (microsecs);
}
