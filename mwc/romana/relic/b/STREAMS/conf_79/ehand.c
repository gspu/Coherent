/*
 * Error-handling for an application.
 */

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		USE_PROTO
 *		ARGS ()
 *	<setjmp.h>
 *		longjmp ()
 *	<stdarg.h>
 *		va_list
 *		va_start ()
 *		va_end ()
 *	<stdio.h>
 *		stderr
 *		fprintf ()
 *		putc ()
 *		vfprintf ()
 *	<stdlib.h>
 *		EXIT_FAILURE
 *		exit ()
 */

#include <sys/compat.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ehand.h"


/*
 * Some ancient systems do not define EXIT_SUCCESS or EXIT_FAILURE.
 */

#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE	1
#endif


ehand_t	      *	estack_base;


/*
 * Throw an error - write out an error report and throw to the last installed
 * handler (or call exit () if there is no installed handler).
 */


#ifdef	USE_PROTO
void (throw_error) (CONST char * fmt, ...)
#else
void
throw_error ARGS ((fmt))
CONST char    *	fmt;
#endif
{
#if	0
	/*
	 * There is no vfprintf () under any version of Coherent up to 4.0.1,
	 * so we must simulate the effect using the obsolete extension that
	 * Coherent *does* support, namely the %r (recursive) format.
	 */
	(void) fprintf (stderr, "ERROR : %r", & fmt);
#else
	va_list		args;

	va_start (args, fmt);
	(void) fprintf (stderr, "ERROR : ");
	(void) vfprintf (stderr, fmt, args);
	va_end (args);
#endif

	putc ('\n', stderr);

	if (estack_base == NULL)
		exit (EXIT_FAILURE);

	longjmp (estack_base->eh_buf, -1);
}


/*
 * This function can be called when the code to pop the current exception
 * handler detects that the stack base is not where it should be (implying
 * that an inner function did not pop its handler).
 */

#ifdef	USE_PROTO
void (error_error) (CONST char * file, int line)
#else
void
error_error ARGS ((file, line))
CONST char    *	file;
int		line;
#endif
{
	fprintf (stderr, "FATAL ERROR : Exception handler nesting error ");
	fprintf (stderr, "detected in file\n  '%s' at line %d\n", file, line);

	exit (EXIT_FAILURE);
}


/*
 * Chain to the next error-handler in the stack of error handlers after the
 * handler passed to us.
 */

#ifdef	USE_PROTO
void (chain_error) (ehand_t * err)
#else
void
chain_error ARGS ((err))
ehand_t	      *	err;
#endif
{
	if (err != estack_base)
		error_error ("chain_error, " __FILE__, __LINE__);

	if ((estack_base = err->eh_next) == NULL)
		exit (EXIT_FAILURE);

	longjmp (estack_base->eh_buf, -1);
}

