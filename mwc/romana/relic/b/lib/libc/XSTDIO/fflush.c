/*
 * libc/stdio/fflush.c
 * ANSI-compliant C standard i/o library.
 * fflush()
 * ANSI 4.9.5.2.
 * Write out unwritten data in buffer.
 */

#include <stdio.h>
#if	COHERENT || GEMDOS
#include <errno.h>
#endif

int
fflush(stream) register FILE *stream;
{
	register int n;
	register FILE **fpp;
	register _FILE2 *f2p;

	if (stream == NULL) {
		/* fflush all streams. */
		n = 0;
		for (fpp = _fp+_NFILE; --fpp >= _fp; )
			if ((stream = *fpp) != NULL
			 && (stream->_ff2 & _FINUSE)
			 && fflush(stream) == EOF)
				n = EOF;
		return n;
	}
	stream->_cc = 0;
	if ((stream->_ff1 & _FERR) != 0)
		return EOF;
	f2p = stream->_f2p;
	if ((n = stream->_cp - f2p->_dp) <= 0
	  || write(fileno(stream), f2p->_dp, n) == n ) {
		if (n < 0)
			;
		else if (stream->_cp == f2p->_ep)
			f2p->_dp = stream->_cp = f2p->_bp;
		else
			f2p->_dp = stream->_cp;
		return 0;
	} else {
#if	COHERENT || GEMDOS
		if (errno == EINTR)
			errno = 0;
		else
#endif
		stream->_ff1 |= _FERR;
		return EOF;
	}
}

/* end of libc/stdio/fflush.c */
