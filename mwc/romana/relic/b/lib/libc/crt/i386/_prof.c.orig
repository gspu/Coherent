/*
 * Called from the run-time start-off to enable profiling and to
 * write out the final profiling information.
 */
#include <stdio.h>
#include <sys/types.h>


#define	STDERR	2


_profon()
{
#ifndef	Z8001
	register unsigned	bufl;
	register short		*buf;
	extern			__end_text();
	static char		emsg[]	= "No room for profil buffer\n";

	bufl = (unsigned)__end_text / 2;
	buf = (short *)sbrk(bufl * sizeof (short));
	if (buf == NULL) {
		write(STDERR, emsg, sizeof emsg);
		abort();
	}
	monitor((vaddr_t)1, (vaddr_t)__end_text, buf, bufl);
#else
	monitor((vaddr_t)1);		/* dummy call to start things off */
#endif
}


_profoff()
{
	monitor(NULL);
}

