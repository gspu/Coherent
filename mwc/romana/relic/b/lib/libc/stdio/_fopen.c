/*
 * libc/stdio/_fopen.c
 * ANSI-compliant C standard i/o library internals.
 * _fopen()
 * Open a file.
 */

/*
 * The recognized mode strings are as follows:
 *	mode a.k.a.  notfound	found	mode   truncate	append
 *	r		error	read	RMODE	0	0
 *	w		create	trunc.	WMODE	1	0
 *	a		create	append	WMODE	0	1
 *	r+   rw r+w	error	rd.&wr.	RWMODE	0	0
 *	w+   wr w+r	create	trunc.	RWMODE	1	0
 *	a+   ar a+r	create	app.&rd	RWMODE	0	1
 * In all cases, a trailing 'b' means ascii==1, otherwise ascii==0.
 * ANSI 4.9.5.3 requires only the first mode forms above (e.g. "r+")
 * but tolerates the alternate forms (e.g. "r+w" or "rw",
 * see code involving local 'cn' below).
 * Use the given FILE if fp is not NULL, otherwise allocate a new FILE.
 * Use the given fd if nonnegative.
 * The get and put function pointers in the FILE are initialized
 * later, possibly after a setvbuf() call specifying desired buffering.
 */

#include <stdio.h>
#include <stdlib.h>
#include "stdio.int.h"

#define	RMODE	0
#define	WMODE	1
#define	RWMODE	2
#define	CRMODE	0666	/* default access permissions on create */

FILE *
_fopen(name, type, fp, fd) char *name, *type; register FILE *fp; register int fd;
{
	register _FILE2	*f2p;
	register int	mode;
	int		truncate, append, ascii;
	char		c, cn;

	/* Parse the type string and set flags accordingly. */
	mode = WMODE;
	ascii = 1;
	truncate = append = 0;
	cn = 'r';
	if ((c = *type++) == 'r') {
		mode = RMODE;
		cn = 'w';
	} else if (c=='w')
		truncate++;
	else if (c=='a')
		append++;
	else
		return NULL;
	if ((c = *type) != '\0') {
		if (c == 'b') {
			ascii = 0;
			c = *++type;
		}
		if (c == '+') {
			mode = RWMODE;
			c = *++type;
		}
		if (c == cn) {
			mode = RWMODE;
			c = *++type;
		}
		if (c == 'b') {
			ascii = 0;
			c = *++type;
		}
		if (c != '\0')
			return NULL;
	}

#if	_ASCII
	/*
	 * Beware of ASCII files ending with ASCII EOF character in append mode.
	 * Open for r+w, to allow checking last character.
	 */
	if (append
	 && ascii
	 && (fd >= 0 || (fd = open(name, RWMODE)) >=0)) {
		if (lseek(fd, -1L, SEEK_END) != -1L) {	/* seek to end-1 */
			if (read(fd, &c, 1) == 1	/* read last char */
			 && c == _EOFCHAR)		/* last char is EOF */
				lseek(fd, -1L, SEEK_CUR); /* so unread it */
		} else					/* might be empty */
			lseek(fd, 0L, SEEK_END);
	}
#endif

	/* Try opening existing file, unless truncation is specified. */
	if (fd < 0 && !truncate)
		fd = open(name, mode);

	/*
	 * If "r" or "r+" and the open above failed, return an error.
	 * If "w" or "w+", create the file.
	 * If "a" or "a+" and the open above, create the file.
	 * If "w+" or "a+", close it and reopen with the desired mode.
	 */
	if (fd < 0
	 && (truncate || append)
	 && (fd = creat(name, CRMODE)) >= 0
	 && mode != WMODE) {
		close(fd);
		fd = open(name, mode);
	}
	if (fd < 0)
		return NULL;			/* Open failed */

#if	_ASCII
	if (append && !ascii)			/* ASCII append seek above */
#else
	if (append)
#endif
		lseek(fd, 0L, SEEK_END);	/* Seek to end */

	/* Allocate a FILE if necessary, die on failure. */
	if (fp == NULL) {
		if ((fp = (FILE *)malloc(sizeof(FILE)+sizeof(_FILE2))) == NULL) {
			close(fd);
			return NULL;
		}
		fp->_f2p = (char *)fp + sizeof(FILE);
	}

	/* Initialize the FILE. */
	fp->_cc = 0;
	fp->_cp = NULL;
	fp->_ff1 = 0;
	fp->_fd = fd;
	fp->_ff2 = _FINUSE;
	fp->_mode = _MODE_UNINIT;
	f2p = fp->_f2p;
	f2p->_gt = &_fginit;
	f2p->_pt = &_fpinit;
	f2p->_bp = f2p->_dp = f2p->_ep = f2p->_nm = NULL;
	f2p->_uc = 0;
	if (append)
		fp->_ff2 |= _FAPPND;
#if	_ASCII
	if (ascii)
		fp->_ff2 |= _FASCII;
#if	MSDOS
	else
		_setraw(fd);
#endif
#endif
	if (mode == RMODE) {
		fp->_ff1 |= _FRONLY;
		f2p->_pt = &_fpute;
	} else if (mode == WMODE) {
		fp->_ff1 |= _FWONLY;
		f2p->_gt = &_fgete;
	} else if (mode == RWMODE)
		fp->_ff1 |= _FRW;
	return fp;
}

/* end of libc/stdio/_fopen.c */
