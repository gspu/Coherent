#ifndef	__SYS_KDEBUG_H__
#define	__SYS_KDEBUG_H__

/*
 * This header defines debugging macros suitable for use in an operating
 * system kernel. There can be a variety of handling disciplines in place
 * for each different class of internal problem, depending on whether or
 * not this is a production or debugging system (and on the presence of some
 * kind of debugger which we might like to call upon).
 *
 * One aim is that the code which is being developed should be able to pass
 * the strictest of "lint"-style checks. In the past it has often been the
 * case that developers have not taken certain classes of diagnostics
 * seriously, especially where the return value of a function is not used.
 *
 * In an operating system kernel, it is *never* safe to assume that any
 * function that returns a value can have that value ignored. Even if there
 * is no sane corrective action that can take place, developers should be
 * able to expect assert ()-style checks will catch such "impossible"
 * conditions.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__STRING ()
 *		__PROTO ()
 *	<common/xdebug.h>
 *		__FILE_INFO__
 */

#include <common/xdebug.h>


__EXTERN_C_BEGIN__

__NO_RETURN__ void
		__assert_fail2	__PROTO ((__CONST__ char * _exp,
					  __CONST__ char * _info,
					  int _lineno));
__NO_RETURN__ void
		__assert_fail	__PROTO ((__CONST__ char * _exp));

__EXTERN_C_END__


/*
 * The ASSERT () macro evaluates it's argument and checks for a non-zero
 * (boolean TRUE, in the C language) result. If any other value is returned,
 * an assert ()-style diagnostic is issued to the system console via
 * cmn_err () with a severity of CE_WARN.
 *
 * The name ASSERT () indicates that the value of the test expression is just
 * that, a test, and may not be evaluated if the system is compiled with
 * debugging facilities suppressed.
 */

#ifndef	NDEBUG
# if	__NO_FILE_INFO__
#  define ASSERT(exp)		((void) ((exp) ? (void) 0 : \
				 __assert_fail (__STRING (exp), NULL, 0)))
# else
#  define ASSERT(exp)		((void) ((exp) ? (void) 0 : \
				 __assert_fail2 (__STRING (exp), __FILE_INFO__)))
# endif
#else
# define ASSERT(exp)		((void) 0)
#endif


#endif	/* ! defined (__SYS_KDEBUG_H__) */
