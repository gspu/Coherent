/*
 * Default regerror routine from regexp package.
 * \n added at end of messages to fit MWC convensions.
 */
#include <stdio.h>

void
regerror(s)
char *s;
{
#ifdef ERRAVAIL
	error("regexp: %s\n", s);
#else
	fprintf(stderr, "regexp(3): %s\n", s);
	exit(1);
#endif
	/* NOTREACHED */
}
