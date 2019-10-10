/*
 * libc/stdio/fwrite.c
 * ANSI-compliant C standard i/o library.
 * fwrite()
 * ANSI 4.9.8.2.
 * Write nmemb items of given size from ptr to stream.
 */

#include <stdio.h>
#include <stdlib.h>

size_t
fwrite(ptr, size, nmemb, stream) const __VOID__ *ptr; size_t size, nmemb; register FILE *stream;
{
	register size_t nb, n;
	register unsigned char *p;

	n = nb = size * nmemb;

	/* Throw away ungotten character if present. */
	if (stream->_ff2 & _FUNGOT)
		(*stream->_f2p->_gt)(stream);

	/* If unbuffered, write() will work, otherwise use putc(). */
	/* putc() sets _FERR as required, write() does not. */
	if (stream->_mode == _MODE_FBUF || stream->_mode == _MODE_LBUF) {
		for (p = ptr; nb && putc(*p++, stream) != EOF; nb--)
			;
		if (nb)
			return (n - nb) / size;
	} else if ((nb = write(fileno(stream), ptr, nb)) != n) {
		stream->_ff1 |= _FERR;
		return nb / size;
	}
	return nmemb;
}

/* end of libc/stdio/fwrite.c */
