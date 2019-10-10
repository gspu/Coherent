/*
 * libc/stdio/fclose.c
 * ANSI-compliant C standard i/o library.
 * fclose()
 * ANSI 4.9.5.1.
 * Close a stream.
 */

#include <stdio.h>

int
fclose(stream) register FILE *stream;
{
	register int flags, status;
	register _FILE2 *f2p;

	f2p = stream->_f2p;
	flags = stream->_ff2;
	if ((flags & _FINUSE) == 0)
		return EOF;			/* Not in use */
	status = fflush(stream);		/* Flush unwritten data */
#if	_DONTC
	if (flags & _FDONTC)
		return status;			/* Don't close */
#endif
	close(fileno(stream));			/* Call sys close */
	if (flags & _FFREEB)
		free(f2p->_bp);			/* Release allocated buffer */
	if (f2p->_nm != NULL) {
		remove(f2p->_nm);		/* Delete temporary file */
		free(f2p->_nm);			/* and release allocated name */
	}
	stream->_ff1 = stream->_ff2 = 0;	/* Reset all flags */
	return status;
}

/* end of libc/stdio/fclose.c */
