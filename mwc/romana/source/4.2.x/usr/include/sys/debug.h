/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_DEBUG_H__
#define	__SYS_DEBUG_H__

/*
 * This header defines debugging macros suitable for use in an operating
 * system kernel. There can be a variety of handling disciplines in place
 * for each different class of internal problem, depending on whether or
 * not this is a production or debugging system (and on the presence of some
 * kind of debugger which we might like to call upon).
 */

#include <common/xdebug.h>


__EXTERN_C_BEGIN__

__NO_RETURN__	__assert_fail2	__PROTO ((__CONST__ char * _exp,
					  __CONST__ char * _info,
					  int _lineno));
__NO_RETURN__	__assert_fail	__PROTO ((__CONST__ char * _exp));

__EXTERN_C_END__


/*
 * The ASSERT () macro evaluates its argument and checks for a non-zero
 * (boolean TRUE, in the C language) result. If any other value is returned,
 * an assert ()-style diagnostic is issued to the system console via
 * cmn_err () with a severity of CE_WARN.
 *
 * The name ASSERT () indicates that the value of the test expression is just
 * that, a test, and may not be evaluated if the system is compiled with
 * debugging facilities suppressed.
 *
 * Note that we do not use the __STRING () macro, because that would expand
 * our argument.
 */

#if	defined (DEBUG) || _DEBUG
# if	__NO_FILE_INFO__
#  define __FAIL(string)	__assert_fail (string, NULL, 0)
# else
#  define __FAIL(string)	__assert_fail2 (string, __FILE_INFO__)
# endif
# if	_HAVE_STRINGIZE
#  define ASSERT(exp)		((exp) ? (void) 0 : __FAIL (# exp))
# else
#  define ASSERT(exp)		((exp) ? (void) 0 : __FAIL ("exp"))
# endif
#else
# define ASSERT(exp)		((void) 0)
#endif


#endif	/* ! defined (__SYS_DEBUG_H__) */
