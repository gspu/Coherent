/*
 * libc/stdio/exit.c
 * C general utilities library.
 * exit()
 * ANSI 4.10.4.3.
 * Normal termination.
 * Implementation defined behavior:
 *	successful termination (status==0 or status==EXIT_SUCCESS) returns 0
 *	unsuccessful termination (status==EXIT_FAILURE) returns 1
 *	otherwise (status!=EXIT_SUCCESS && status!=EXIT_FAILURE) returns status
 * This just passes the given status to the system,
 * which works since EXIT_SUCCESS is 0 and EXIT_FAILURE is 1.
 * Should be in libc/stdlib/exit.c but is in libc/stdio/exit.c instead
 * because the order of objects in the library is significant,
 * cf. libc/stdio/Makefile.
 */

#include <stdio.h>
#include <stdlib.h>

void	(**_atexitfp)();	/* pointer to last atexit-registered function */
int	_atexitn;		/* count of atexit-registered functions */

void
exit(status)
{
	while (_atexitn-- > 0)
		(**--_atexitfp)();	/* Execute registered functions	*/
	_finish();			/* Close open streams		*/
	_exit(status);			/* Return status		*/
}

/* end of libc/stdio/exit.c */
