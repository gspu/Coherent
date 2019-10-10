/*
 * libc/stdio/ftell.c
 * ANSI-compliant C standard i/o library.
 * ftell()
 * ANSI 4.9.9.4.
 * Find logical file position on stream.
 */

#include <stdio.h>
#include <errno.h>

#define	EFGETPOS	EINVAL

long
ftell(stream) register FILE *stream;
{
	register long offset;
	register _FILE2 *f2p;

	f2p = stream->_f2p;

	if ((offset=lseek(fileno(stream), 0L, SEEK_CUR)) == -1L) {
		errno = EFGETPOS;
		return offset;
	}
	if (f2p->_bp!=NULL)
		offset += stream->_cp - f2p->_dp;
	if (stream->_ff2&_FUNGOT)
		--offset;
	return offset;
}

/* end of libc/stdio/ftell.c */
