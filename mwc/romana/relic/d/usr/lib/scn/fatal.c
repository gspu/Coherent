/*
 * Fatal error with message.
 */
#include <scn.h>

void fatal(s)
char *s;
{
	closeUp();
	fprintf(stderr, "\nFatal: %r\n", &s);
	exit(1);
}
