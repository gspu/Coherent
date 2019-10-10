/*
 * libc/crt/_prof.c
 * Runtime profiling.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#if	_I386
extern	_start();
extern	__end_text();
#define	BTEXT	_start
#define	ETEXT	__end_text
#else
extern	etext();
#define	BTEXT	(caddr_t)1
#define	ETEXT	etext
#endif

/*
 * The profiling runtime start-off mcrts0.s
 * enables profiling by calling _profon() before calling main().
 */
_profon()
{
#ifndef	Z8001
	register unsigned	bufl;
	register short		*buf;
	static char		emsg[]	= "No room for profil buffer\n";

	bufl = ((unsigned)ETEXT - (unsigned)BTEXT) / 2;
	buf = (short *)sbrk(bufl * sizeof (short));
	if (buf == NULL) {
		write(STDERR_FILENO, emsg, sizeof emsg);
		abort();
	}
	monitor((caddr_t)BTEXT, (caddr_t)ETEXT, buf, bufl);
#else
	monitor((caddr_t)1);		/* dummy call to start things off */
#endif
}

/*
 * The profiling runtime start-off mcrts0.s
 * disables profiling by calling _profoff() at _exit.
 * It turns off profiling and writes out the final profiling information.
 */
_profoff()
{
	monitor(NULL);
}

/* end of libc/crt/_prof.c */
