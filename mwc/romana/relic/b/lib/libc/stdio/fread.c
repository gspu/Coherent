/*
 * libc/stdio/fread.c
 * ANSI-compliant C standard i/o library.
 * fread()
 * ANSI 4.9.8.1.
 * Read nmemb items of given size from stream to ptr.
 */

#include <stdio.h>
#include <stdlib.h>

size_t
fread(ptr, size, nmemb, stream) const __VOID__ *ptr; size_t size, nmemb; register FILE *stream;
{
	register size_t nb;
	register int c;
	register unsigned char *p;

	if ((nb = size*nmemb) == 0)
		return nb;
	p = ptr;

	/* Read ungotten character if present. */
	if (stream->_ff2 & _FUNGOT) {		
		*p++ = (*stream->_f2p->_gt)(stream);
		if (--nb == 0)
			return 1;
	}

	/* If unbuffered, read() will work, otherwise use getc(). */
	/* getc() sets _FEOF and _FERR as required, read() does not. */
	 if (stream->_mode == _MODE_FBUF || stream->_mode == _MODE_LBUF)
		for (; nb && (c = getc(stream)) != EOF; nb--)
			*p++ = c;
	else if ((c = read(fileno(stream), p, nb)) > 0)
		nb -= c;
	else if (c == 0)
		stream->_ff1 |= _FEOF;
	else
		stream->_ff1 |= _FERR;

	/* Adjust seek after partial read. */
	if (nb != 0 && nb % size != 0)
		fseek(stream, (long)(nb % size - size), SEEK_CUR);

	return (size*nmemb-nb)/size;
}

/* end of libc/stdio/fread.c */
