/*
 * libc/stdio/ungetc.c
 * ANSI-compliant C standard i/o library.
 * ungetc()
 * ANSI 4.9.7.11.
 * Unget a character.
 */

#include <stdio.h>
#include <string.h>

extern	int	_fungotc();
extern	int	_funungetc();

int
ungetc(c, stream) int c; register FILE *stream;
{
	register _FILE2 *f2p;

	f2p = stream->_f2p;
	if (c == EOF || (stream->_ff1 & _FWONLY))
		return EOF;		/* Leave input stream unchanged */
	f2p->_gt = &_fungotc;		/* Replace get function */
	if (!(stream->_ff1 & _FRONLY))
		f2p->_pt = &_funungetc;	/* Replace put function */
	stream->_cc = 0;
	stream->_ff2 |= _FUNGOT;	/* Set ungot flag */
	stream->_ff1 &= ~_FEOF;		/* ANSI 4.9.7.11 (24) */
	f2p->_uc = c;			/* Store ungot character */
	return c;
}

/*
 * Get ungetc character.
 * Restore the appropriate get and put functions.
 */
static
int
_fungotc(fp) register FILE *fp;
{
	register _FILE2 *f2p;

	f2p = fp->_f2p;

#if	_ASCII
	register int isascii;

	isascii = fp->_ff2 & _FASCII;
#endif

	switch (fp->_mode) {
	case _MODE_UNINIT:		/* Uninitialized */
		f2p->_gt = &_fginit;
		f2p->_pt = &_fpinit;
		break;

	case _MODE_FBUF:
	case _MODE_LBUF:
		/* Fully buffered or line buffered */
		if (fp->_mode == _MODE_FBUF) {
			/* Fully buffered output */
#if	_ASCII
			f2p->_pt = isascii ? &_fputba : &_fputb;
#else
			f2p->_pt = &_fputb;
#endif
		} else {
			/* Line buffered output */
#if	_ASCII
			f2p->_pt = isascii ? &_fputta : &_fputt;
#else
			f2p->_pt = &_fputt;
#endif
		}
#if	_ASCII
		f2p->_gt = isascii ? &_fgetba : &_fgetb;
#else
		f2p->_gt = &_fgetb;
#endif
		if ((fp->_cc = fp->_cp - f2p->_dp) > 0)
			fp->_cc = f2p->_ep - fp->_cp;
		break;

	case _MODE_NBUF:
		/* Unbuffered */
#if	_ASCII
		f2p->_gt = isascii ? &_fgetca : &_fgetc;
		f2p->_pt = isascii ? &_fputca : &_fputc;
#else
		f2p->_gt = &_fgetc;
		f2p->_pt = &_fputc;
#endif
		break;

	case _MODE_STR:
		/* String */
		f2p->_gt = &_fgetstr;
		fp->_cc = strlen(fp->_cp);
		break;
	}

	if (fp->_ff1 & _FRONLY)
		f2p->_pt = &_fpute;
	fp->_ff2 &= ~_FUNGOT;		/* Clear the ungot flag */
	return f2p->_uc;		/* Return the ungot character */
}

/*
 * Undo unget.
 * This occurs after an ungetc()
 * when put function is done before get function.
 */
static
int
_funungetc(c, fp) register int c; register FILE *fp;
{
	register _FILE2 *f2p;

	f2p = fp->_f2p;
	(*f2p->_gt)(fp);		/* Undo the unget */
	return putc(c, fp);		/* and do the put */
}

/* end of libc/stdio/ungetc.c */
