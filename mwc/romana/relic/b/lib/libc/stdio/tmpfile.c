/*
 * libc/stdio/tmpfile.c
 * ANSI-compliant C standard i/o library.
 * tmpfile()
 * ANSI 4.9.4.3.
 * Create a temporary stream.
 */

#include <stdio.h>
#include <stdlib.h>

FILE *
tmpfile()
{
	register char *name;
	register FILE *fp;

	/* Allocate file name buffer. */
	if ((name = malloc((size_t)L_tmpnam)) == NULL)
		return NULL;

	/* Generate a temporary file name and open the temp file. */
	if ((fp = fopen(tmpnam(name), "w+b")) == NULL) {
		free(name);
		return NULL;
	}

	/*
	 * Save the name.
	 * The temp file will be removed and the name buffer freed when closed.
	 */
	fp->_f2p->_nm = name;
	return fp;
}

/* end of libc/stdio/tmpfile.c */
