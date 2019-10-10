/*
 * Standard I/O Library
 * Put word (int) to file
 * Routine instead of macro for the cheap and lazy
 */

#include <stdio.h>
#undef	putw

int
putw(w, fp)
int	w;
register FILE	*fp;
{
	putc(w, fp);
	putc(w>>8, fp);
	return (w);
}
