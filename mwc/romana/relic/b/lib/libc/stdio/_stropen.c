/*
 * libc/stdio/_stropen.c
 * ANSI-compliant C standard i/o library internals.
 * _stropen()
 * Open string stream.
 * Called by sprintf(), sscanf(), vsprintf().
 */

#include <stdio.h>
#include <limits.h>
#include "stdio.int.h"

extern	int	_fputstr();

/* length is -1 for output to string, strlen(string) for input from string. */
FILE *
_stropen(string, length, fp) char *string; int length; register FILE *fp;
{
	register _FILE2 *f2p;

	f2p = fp->_f2p;
	f2p->_bp = fp->_cp = string;
	f2p->_dp = f2p->_ep = f2p->_nm = NULL;
	fp->_mode = _MODE_STR;
	fp->_ff1 = fp->_ff2 = fp->_fd = f2p->_uc = 0;
	if (length < 0) {
		fp->_ff1 |= _FWONLY;
		fp->_cc = INT_MAX;
		f2p->_pt = &_fputstr;
		f2p->_gt = &_fgete;
	} else {
		fp->_ff1 |= _FRONLY;
		fp->_cc = length;
		f2p->_pt = &_fpute;
		f2p->_gt = &_fgetstr;
	}
	return fp;
}

/*
 * _fputstr() is static;
 * _fgetstr() (in _fgetstr.c) is not, because it must be visible to ungetc().
 */
static
int
_fputstr(c, fp) int c; register FILE *fp;
{
	return (*fp->_cp++ = (unsigned char)c);
}

/* end of libc/stdio/_stropen.c */
