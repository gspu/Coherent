/*
 * Standard I/O Library Internals
 * open string for getc, putc
 */

#include <stdio.h>
#include <sys/mdata.h>

FILE *
_stropen(string, length, fp)
char	*string;
int	length;
register FILE	*fp;
{
#if !RSX
	int	_fgeteof(),
		_fputeos(),
		_fputexs();

	fp->_bp = fp->_cp = string;
	fp->_gt = &_fgeteof;
	fp->_cc = -length;
	if (fp->_cc < -MAXINT) {	/* most negative int means unlimited */
		fp->_cc = MAXINT;
		fp->_pt = &_fputexs;
	} else
		fp->_pt = &_fputeos;
#else
	fp->v_flag = VF_FIL;
	fp->v_r0 = -length;
	fp->v_r1 = string;
	fp->v_rblk = 1L;	/* seek block > eof block */
	fp->v_efbk = 0L;	/* => eof at end of string */
	fp->v_uget = -1;
#endif
	return (fp);
}

#if !RSX
static
int
_fputeos(c, fp)
unsigned char	c;
register FILE	*fp;
{
	fp->_cc = 0;
	return (EOF);
}

static
int
_fputexs(c, fp)
unsigned char	c;
register FILE	*fp;
{
	fp->_cc = MAXINT;
	return (*fp->_cp++ = c);
}
#endif
