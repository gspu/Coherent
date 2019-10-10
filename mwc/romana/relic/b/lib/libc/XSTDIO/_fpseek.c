/*
 * libc/stdio/_fpseek.c
 * ANSI-compliant C standard i/o library internals.
 * _fpseek()
 * Align file seek address with current pointer in buffer.
 */

#include <stdio.h>

int
_fpseek(fp) register FILE *fp;
{
	register _FILE2 *f2p;

	f2p = fp->_f2p;
	if (fp->_ff2 & _FUNGOT)
		(*f2p->_gt)(fp);	/* Discard ungotten character */
	if (f2p->_dp <= fp->_cp)
		return fflush(fp);	/* Flush buffered output */
	if (lseek(fileno(fp), (long)(fp->_cp-f2p->_dp), SEEK_CUR) == -1L)
		return EOF;		/* Seek back over unread input */
	f2p->_dp = fp->_cp;
	fp->_cc = 0;
	return 0;
}

/* end of libc/stdio/_fpseek.c */
