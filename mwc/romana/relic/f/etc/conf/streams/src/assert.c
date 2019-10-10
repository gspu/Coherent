#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * This file contains the definition for the kernel assertion helper function
 * __assert_fail (), and a manual page for ASSERT ().
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<sys/cmn_err.h>
 *		CE_PANIC
 *		cmn_err ()
 */

#include <common/ccompat.h>
#include <sys/cmn_err.h>
#include <stddef.h>

#include <sys/debug.h>


/*
 *-STATUS:
 *	DDI/DKI.
 *
 *-NAME:
 *	ASSERT ()		Verify assertion.
 *
 *-SYNOPSIS:
 *	#include <sys/debug.h>
 *
 *	void ASSERT (int expression);
 *
 *-ARGUMENTS:
 *	expression		Expression to be evaluated.
 *
 *-DESCRIPTION:
 *	ASSERT () is a debugging interface for verifying program invariants
 *	within code that is compiled with the DEBUG compilation option
 *	defined. "expression" is a boolean expression that the caller expects
 *	to evaluate to non-zero (that is, the caller is asserting that the
 *	expression has a non-zero value). If "expression" evaluates to non-
 *	zero, the ASSERT () call has no effect. If "expression" evaluates to
 *	zero, ASSERT () causes the system to panic with a message identifying
 *	the expression, the name of the source file containing the assertion,
 *	and the line number within the source file where the assertion is
 *	made.
 *
 *	When the DEBUG compilation option is not defined, ASSERT () calls are
 *	not compiled into the code, and therefore have no effect.
 *
 *-RETURN VALUE:
 *	If "expression" evaluates to non-zero, ASSERT () returns no value. If
 *	"expression" evaluates to zero, ASSERT () panics the system.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	cmn_err ()
 */

/*
 *-STATUS:
 *	Internal helper for ASSERT ().
 *
 *-NAME:
 *	__assert_fail ()	Report an assertion failure.
 *
 *-ARGUMENTS:
 *	exp	The text of the source code for the expression that ASSERT ()
 *		evaluated and found to be false.
 *
 *-DESCRIPTION:
 *	Like assert () in C programs, the ASSERT () macro within the kernel
 *	provides a simple debugging mechanism. It is good software engineering
 *	practice to check even those situations that "cannot happen", at least
 *	during the development phase(s) of a section of code. The assertion
 *	mechanisms provide a simple and inexpensive way of performing those
 *	checks that if they fail indicate some condition that indicates a
 *	bug in some code. Normally, the debugging statements are compiled to
 *	check the assertions, but at the developer's option all calls to the
 *	ASSERT () macro will instead expand to no code.
 *
 *	When the ASSERT () macro detects that an assertion condition did not
 *	evaluate to a logically true value (which in C is anything non-zero)
 *	then this helper function is called to halt the kernel with an
 *	appropriate error message.
 *
 *-RETURNS:
 *	Does not return.
 *
 *-LEVEL:
 *	Base or Interrupt.
 *
 *-NOTES:
 *	This function calls cmn_err () to panic the kernel on the CPU where
 *	the fault was detected (and to print the assertion failure message).
 */

#if	__USE_PROTO__
void (__assert_fail) (__CONST__ char * exp)
#else
void
__assert_fail __ARGS ((exp))
__CONST__ char * exp;
#endif
{
	cmn_err (CE_PANIC, "Assertion \"%s\" failed", exp);
}


/*
 *-STATUS:
 *	Internal helper for ASSERT ().
 *
 *-NAME:
 *	__assert_fail2 ()	Verbose form of __assert_fail ()
 *
 *-ARGUMENTS:
 *	exp	The text of the source code for the expression that ASSERT ()
 *		evaluated and found to be false.
 *
 *	info	Location information that aids in locating the source code
 *		which failed the assertion. There are various levels of
 *		detail that may be desired; see <common/xdebug.h> for more on
 *		selecting the contents of the __FILE_INFO__ macro.
 *
 *	lineno	Optional line number in the source file.
 *
 *-DESCRIPTION:
 *	Like assert () in C programs, the ASSERT () macro within the kernel
 *	provides a simple debugging mechanism. It is good software engineering
 *	practice to check even those situations that "cannot happen", at least
 *	during the development phase(s) of a section of code. The assertion
 *	mechanisms provide a simple and inexpensive way of performing those
 *	checks that if they fail indicate some condition that indicates a
 *	bug in some code. Normally, the debugging statements are compiled to
 *	check the assertions, but at the developer's option all calls to the
 *	ASSERT () macro will instead expand to no code.
 *
 *	When the ASSERT () macro detects that an assertion condition did not
 *	evaluate to a logically true value (which in C is anything non-zero)
 *	then this helper function is called to halt the kernel with an
 *	appropriate error message.
 *
 *-RETURNS:
 *	Does not return.
 *
 *-LEVEL:
 *	Base or Interrupt.
 *
 *-NOTES:
 *	This function calls cmn_err () to panic the kernel on the CPU where
 *	the fault was detected (and to print the assertion failure message).
 */

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
	cmn_err (CE_PANIC,
		 lineno == 0 ? "Assertion \"%s\" failed in %s" :
			       "Assertion \"%s\" failed in %s at line %d",
		 exp, info, lineno);
}
