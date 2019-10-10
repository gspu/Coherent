/*
 * Print error messages based on value in `errno'.
 */

#include <stdio.h>

extern	int	errno;
extern	char	*sys_errlist[];
extern	int	sys_nerr;

perror(s)
register char *s;
{
	register char *es;

	if (errno < sys_nerr)
		es = sys_errlist[errno]; else
		es = "Bad error number";
	if (s != NULL) {
		fputs(s, stderr);
		fputs(": ", stderr);
	}
	fputs(es, stderr);
	fputs("\n", stderr);
}
