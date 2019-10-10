/*
 * Standard I/O library
 */

#ifndef	STDIO_H
#define	STDIO_H	STDIO_H

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
#endif

#ifndef NULL
#define	NULL	((char *)0)
#endif
#define	EOF	(-1)
#define	BUFSIZ	(1<<9)
#define	_NFILE	20

extern	FILE	_stdin, _stdout, _stderr, *_fp[_NFILE];

/* Flags in _ff */
#define	_FINUSE	01
#define	_FSTBUF	02		/* setbuf was called */
#define	_FUNGOT	04		/* ungotten char present */
#define	_FEOF	0100
#define	_FERR	0200

#define	_ep(fp)		((fp)->_bp+BUFSIZ)

char	*gets();
char	*fgets();
FILE	*fopen();
FILE	*freopen();
FILE	*fdopen();
FILE	*popen();
FILE	*_stropen();
long	ftell();
void	puts();
void	fputs();
void	setbuf();
char	*malloc();
char	*sbrk();

#define	getchar()	getc(stdin)
#define getc(f)		fgetc(f)
#define	putchar(c)	putc(c, stdout)
#define putc(c, fp)	fputc(c, fp)
#define	feof(fp)	((fp)->_ff&(_FEOF|_FERR))
#define	ferror(fp)	((fp)->_ff&_FERR)
#define	clearerr(fp)	((fp)->_ff &= ~(_FERR|_FEOF))
#define	fileno(fp)	((fp)->_fd)
#define	wdleng()	(16)

#define	stdin	(&_stdin)
#define	stdout	(&_stdout)
#define	stderr	(&_stderr)

/*
 * Temporary file directory manifests for System V compatibility
 */
#define P_tmpdir	"/tmp"		/* Default temporary directory */
#define L_tmpnam	64		/* Maximum length of temp file name */

/* end of stdio.h */
