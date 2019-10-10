/*
 * Standard I/O Library.
 * Rewind file.
 * ANSI 4.9.9.5: void rewind(FILE *stream)
 * For historical reasons,
 * this defines rewind() returning int rather than void.
 */

#include <stdio.h>

int
rewind(fp)
register FILE	*fp;
{
	register int status;

	if ((status = fseek(fp, 0L, SEEK_SET)) == 0)
		fp->_ff &= ~_FERR;		/* ANSI 4.9.9.5 */
	return (status);
}
