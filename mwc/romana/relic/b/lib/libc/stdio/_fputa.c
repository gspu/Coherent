/*
 * libc/stdio/_fputa.c
 * ANSI-compliant C standard i/o library internals.
 * _fputa()
 * Append character to end of file.
 * Executed on first write after seek on append mode stream.
 */

#include <stdio.h>
#include "stdio.int.h"

int
_fputa(c, fp) int c; register FILE *fp;
{
	register _FILE2 *f2p;

#if	_ASCII
	register int isascii;

	isascii = fp->_ff2 & _FASCII;
#endif
	/* Seek to end. */
	if (lseek(fileno(fp), 0L, SEEK_END) == -1L)
		return EOF;

	/* Restore appropriate put function pointer. */
	f2p = fp->_f2p;
	switch (fp->_mode) {

	case _MODE_UNINIT:
		f2p->_pt = &_fpinit;
		break;

	case _MODE_FBUF:
#if	_ASCII
		f2p->_pt = isascii ? &_fputba : &_fputb;
#else
		f2p->_pt = &_fputb;
#endif
		break;

	case _MODE_LBUF:
#if	_ASCII
		f2p->_pt = isascii ? &_fputta : &_fputt;
#else
		f2p->_pt = &_fputt;
#endif
		break;

	case _MODE_NBUF:
#if	_ASCII
		f2p->_pt = isascii ? &_fputca : &_fputc;
#else
		f2p->_pt = &_fputc;
#endif
		break;

	default:
		return EOF;				/* should not happen */
		break;
	}

	/* Put the character. */
	return (*f2p->_pt)(c, fp);
}

/* end of libc/stdio/_fputa.c */
