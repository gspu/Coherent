/*
 * libc/stdio/finit.c
 * Standard i/o library internals.
 * Initialize file for i/o:
 *	decide if buffered
 *	allocate buffer if required
 *	stuff appropriate get/put functions
 *	reset char count
 */

#include <stdio.h>
#include <errno.h>

extern	long	lseek();

extern	int	_fgetc();
extern	int	_fputc();
extern	int	_fgetb();
extern	int	_fputb();
extern	int	_fputt();

void
finit(fp) register FILE *fp;
{
	register int tty, sav;

	sav = errno;			/* because isatty() can set errno */
	if (fp->_bp == NULL
	 && (fp->_ff&_FSTBUF || (tty=isatty(fileno(fp))) && fp!=stdout
	    || (fp->_bp = malloc(BUFSIZ)) == NULL)) {
		fp->_gt = &_fgetc;
		fp->_pt = &_fputc;
	} else if ((fp->_ff&_FSTBUF)==0 && tty && fp==stdout) {
		fp->_gt = &_fgetb;
		fp->_pt = &_fputt;
		fp->_dp = fp->_cp = fp->_bp;
		fp->_cc = 0;
	} else {
		fp->_gt = &_fgetb;
		fp->_pt = &_fputb;
		fp->_dp = fp->_cp = fp->_bp + boffset(fileno(fp));
		fp->_cc = 0;
	}
	errno = sav;
}

static
int
boffset(fd) int fd;
{
	register long off;

	if ((off=lseek(fd, 0L, SEEK_CUR))==-1L)
		return 0;
	else
		return (unsigned)off%BUFSIZ;
}

int
_fginit(fp) register FILE *fp;
{
	finit(fp);
	return (*fp->_gt)(fp);
}

int
_fpinit(c, fp) register char c; register FILE *fp;
{
	finit(fp);
	return (*fp->_pt)(c, fp);
}


/*
 * Close all files.
 * Called from exit().
 */
void
_finish()
{
	register FILE **fpp;

	for (fpp = &_fp[_NFILE-1]; fpp>=&_fp[0]; --fpp)
		if (*fpp != NULL)
			fclose(*fpp);
}

/* end of libc/stdio/finit.c */
