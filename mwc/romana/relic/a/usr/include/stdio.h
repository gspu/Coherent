/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * stdio.h
 * COHERENT Standard I/O library.
 */

#ifndef	STDIO_H
#define	STDIO_H

/* The FILE structure. */
typedef struct	FILE {
	unsigned char	*_cp,		/* current character ptr */
			*_dp,		/* ptr to start of data in buffer */
			*_bp;		/* buffer pointer */
	int	_cc;			/* character count */
	int	(*_gt)(),		/* getc function */
		(*_pt)();		/* putc function */
	char	_ff;			/* flags; see below */
	char	_fd;			/* file descriptor (reqd by reopen) */
	int	_uc;			/* ungot char */
}	FILE;

/* Manifest constants. */
#define	_NFILE	20
#define	BUFSIZ	(1<<9)
#define	EOF	(-1)
#ifndef	NULL
#define	NULL	((char *)0)
#endif

/* Definitions for System V-style tmpnam() and tempnam(). */
#define	L_tmpnam	64		/* Maximum length of temp file name */
#define	P_tmpdir	"/tmp"		/* Default temporary directory */

/* Origin arguments for fseek() and lseek(). */
#define	SEEK_SET	0		/* from beginning */
#define	SEEK_CUR	1		/* from current position */
#define	SEEK_END	2		/* from end */

/* Standard FILEs. */
extern	FILE	_stdin, _stdout, _stderr, *_fp[_NFILE];
#define	stdin	(&_stdin)
#define	stdout	(&_stdout)
#define	stderr	(&_stderr)

/* Flags in _ff. */
#define	_FINUSE	0x01
#define	_FSTBUF	0x02		/* setbuf was called */
#define	_FUNGOT	0x04		/* ungotten char present */
#define	_FEOF	0x40
#define	_FERR	0x80

/* Functions. */
FILE	*_stropen();
FILE	*fdopen();
char	*fgets();
FILE	*fopen();
FILE	*freopen();
long	ftell();
char	*gets();
char	*malloc();
FILE	*popen();
char	*sbrk();
void	setbuf();

/* Macros. */
#define	_ep(fp)		((fp)->_bp+BUFSIZ)
#define	clearerr(fp)	((fp)->_ff &= ~(_FERR|_FEOF))
#define	feof(fp)	((fp)->_ff&_FEOF)
#define	ferror(fp)	((fp)->_ff&_FERR)
#define	fileno(fp)	((fp)->_fd)
#define	getc(fp)	(++(fp)->_cc>0 ? --(fp)->_cc,(*(fp)->_gt)((fp)) \
				       : *(fp)->_cp++)
#define	getchar()	getc(stdin)
#define	putc(c,fp)	(--(fp)->_cc<0 ? ++(fp)->_cc,(*(fp)->_pt)((c),(fp)) \
				       : (*(fp)->_cp++=(c)))
#define	putchar(c)	putc(c, stdout)
#define	wdleng()	(16)

#endif

/* end of stdio.h */
