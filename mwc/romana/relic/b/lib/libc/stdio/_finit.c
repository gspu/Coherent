/*
 * libc/stdio/_finit.c
 * ANSI-compliant C standard i/o library internals.
 * _fginit(), _fpinit()
 * Initialize file for i/o.
 */

#include <stdio.h>
#include <errno.h>

/*
 * The get and put function pointers in a FILE
 * initially contain &_fginit and &_fpinit.
 * The first get/put on the stream calls _f[gp]init()
 * which calls finit().
 * finit() calls setvbuf() to initialize stream buffering
 * and to reset the get and put functions appropriately.
 */
static
void
finit(stream) register FILE *stream;
{
	register int mode, sav;

	if (stream->_mode != _MODE_UNINIT)
		return;				/* setvbuf already called */
	sav = errno;			/* because isatty() can set errno */
	mode = (!isatty(fileno(stream))) ? _IOFBF
		    : (stream == stdout) ? _IOLBF : _IONBF;
	if (setvbuf(stream, NULL, mode, (size_t)BUFSIZ) == 0)
		return;				/* buffered or line buffered */
	setvbuf(stream, NULL, _IONBF, (size_t)0);	/* unbuffered */
	errno = sav;
}

/*
 * Initialize and get.
 */
int
_fginit(stream) register FILE *stream;
{
	finit(stream);
	return (*stream->_f2p->_gt)(stream);
}

/*
 * Initialize and put.
 */
int
_fpinit(c, stream) register char c; register FILE *stream;
{
	finit(stream);
	return (*stream->_f2p->_pt)(c, stream);
}

/* end of libc/stdio/_finit.c */
