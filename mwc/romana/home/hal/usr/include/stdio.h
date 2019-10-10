/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * COHERENT Standard Input/Output library header.
 * ANSI C Standard, Section 4.9.
 */

#ifndef	__STDIO_H__
#define	__STDIO_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/__off.h>
#include <common/__valist.h>
#include <common/_size.h>
#include <common/_null.h>
#include <common/whence.h>

/* Macros. */
#define	BUFSIZ		512		/* default buffer size	*/
#define	EOF		(-1)		/* end of file		*/
#define	_EOFCHAR	26		/* ASCII EOF character	*/
#define	FILENAME_MAX	64		/* max filename length	*/
#define	FOPEN_MAX	_NFILE		/* max # of open files	*/
#define	_NFILE		60		/* number of FILEs	*/
#define	_NSTDFILE	3		/* number of predefined FILEs	*/
#define	L_ctermid	9		/* ctermid () length */
#define	L_tmpnam	64		/* tmpnam length	*/
#define	P_tmpdir	"/tmp"		/* default temporary directory */
#define	TMP_MAX		91		/* number of tmpnams	*/

/* Types. */
typedef long		fpos_t;		/* file position type	*/

/*
 * The order the first five FILE members corresponds to the order in iBCS2,
 * to allow a degree of binary compatibility.
 */
typedef struct	FILE {
	int		_cc;		/* character count	*/
	unsigned char	*_cp;		/* current character	*/
	struct _FILE2	*_f2p;		/* more info		*/
	char		_ff1;		/* flags		*/
	char		_fd;		/* file descriptor	*/
	char		_ff2;		/* more flags; see below */
	char		_mode;		/* mode			*/
}	FILE;
/* These additional members are not in struct FILE for compatibility reasons. */
typedef	struct	_FILE2 {
	int		(*_gt)();	/* getc function	*/
	int		(*_pt)();	/* putc function	*/
	unsigned char	*_bp;		/* start of buffer	*/
	unsigned char	*_dp;		/* start of data	*/
	unsigned char	*_ep;		/* end of buffer	*/
	char		*_nm;		/* temp file name	*/
	int		_uc;		/* ungot char		*/
}	_FILE2;

/* iBCS2 compatibility. */
#define	_cnt	_cc
#define	_ptr	_cp
#define	_base	_f2p
#define	_flag	_ff1
#define	_file	_fd

/*
 * Standard FILEs. Technically, these definitions violate the user name-space,
 * but these names are required for iBCS2.
 */
extern	FILE	_iob[_NSTDFILE];
extern	FILE	*_fp[_NFILE];
#define	stdin	(&_iob[0])
#define	stdout	(&_iob[1])
#define	stderr	(&_iob[2])

/* _IO[FLN]BF are used for setvbuf() type args. */
#define	_IOFBF		0x00		/* fully buffered	*/
#define	_IONBF		0x04		/* unbuffered		*/
#define	_IOLBF		0x40		/* line buffered	*/

/* Flags in _ff1, cf. iBCS2. */
#define	_FEOF		0x10		/* end of file		*/
#define	_FERR		0x20		/* error		*/

/* Non-iBCS2 flags in _ff1. */
#define	_FRONLY		0x01		/* read only		*/
#define	_FWONLY		0x02		/* write only		*/
#define	_FRW		0x80		/* read and write	*/

/* Flags in _ff2, not in iBCS2. */
#define	_FINUSE		0x01		/* in use		*/
#define	_FAPPND		0x02		/* append		*/
#define _FASCII		0x04		/* ASCII		*/
#define	_FDONTC		0x08		/* do not close		*/
#define	_FFREEB		0x10		/* free buffer when closed	*/
#define	_FUNGOT		0x80		/* ungotten char present	*/

/* Mode field values. */
#define	_MODE_FBUF	0x01		/* fully buffered	*/
#define	_MODE_LBUF	0x02		/* line buffered	*/
#define	_MODE_NBUF	0x04		/* unbuffered		*/
#define	_MODE_STR	0x08		/* string		*/
#define	_MODE_UNINIT	0x10		/* uninitialized	*/


/* Standard functions. */

__EXTERN_C_BEGIN__

int		remove		__PROTO ((__CONST__ char * _filename));
int		rename		__PROTO ((__CONST__ char * _old,
					  __CONST__ char * _new));
FILE	      *	tmpfile		__PROTO ((void));
char	      *	tmpnam		__PROTO ((char * _s));
int		fclose		__PROTO ((FILE * _stream));
int		fflush		__PROTO ((FILE * _stream));
FILE	      *	fopen		__PROTO ((__CONST__ char * _filename,
					  __CONST__ char * _mode));
FILE	      *	freopen		__PROTO ((__CONST__ char * _filename,
					  __CONST__ char * _mode,
					  FILE * _stream));
void		setbuf		__PROTO ((FILE * _stream, char * _buf));
int		setvbuf		__PROTO ((FILE * _stream, char * _buf,
					  int _mode, size_t _size));
int		fprintf		__PROTO ((FILE * _stream,
					  __CONST__ char * _format, ...))
				__PRINTF_LIKE (2, 3);
int		fscanf		__PROTO ((FILE * _stream,
					  __CONST__ char * _format, ...))
				__SCANF_LIKE (2, 3);
int		printf		__PROTO ((__CONST__ char * _format, ...))
				__PRINTF_LIKE (1, 2);
int		scanf		__PROTO ((__CONST__ char * _format, ...))
				__SCANF_LIKE (1, 2);
int		sprintf		__PROTO ((char * _s,
					  __CONST__ char * _format, ...))
				__PRINTF_LIKE (2, 3);
int		sscanf		__PROTO ((__CONST__ char * _s,
					  __CONST__ char * _format, ...))
				__SCANF_LIKE (2, 3);
int		vfprintf	__PROTO ((FILE * _stream,
					  __CONST__ char * _format,
					  __va_list arg));
int		vprintf		__PROTO ((__CONST__ char * _format,
					  __va_list arg));
int		vsprintf	__PROTO ((char * _s,
					  __CONST__ char * _format,
					  __va_list arg));
int		fgetc		__PROTO ((FILE * _stream));
char	      *	fgets		__PROTO ((char * _s, int _n, FILE * _stream));
int		fputc		__PROTO ((int _c, FILE * _stream));
int		fputs		__PROTO ((__CONST__ char * _s,
					  FILE * _stream));
int		getc		__PROTO ((FILE * stream));
int		getchar		__PROTO ((void));
char	      *	gets		__PROTO ((char * _s));
int		putc		__PROTO ((int _c, FILE * _stream));
int		putchar		__PROTO ((int _c));
int		puts		__PROTO ((__CONST__ char * _s));
int		ungetc		__PROTO ((int _c, FILE * _stream));
size_t		fread		__PROTO ((__VOID__ * _ptr, size_t _size,
					  size_t _nmemb, FILE * _stream));
size_t		fwrite		__PROTO ((__CONST__ __VOID__ * _ptr,
					  size_t _size, size_t _nmemb,
					  FILE * _stream));
int		fgetpos		__PROTO ((FILE * _stream, fpos_t * _pos));
int		fseek		__PROTO ((FILE * _stream, __off_t _offset,
					  int whence));
int		fsetpos		__PROTO ((FILE * _stream,
					  __CONST__ fpos_t * pos));
__off_t		ftell		__PROTO ((FILE * _stream));
void		rewind		__PROTO ((FILE * _stream));
void		clearerr	__PROTO ((void));
int		feof		__PROTO ((FILE * _stream));
int		ferror		__PROTO ((FILE * _stream));
void		perror		__PROTO ((__CONST__ char * _s));

char	      *	ctermid		__PROTO ((char * _s));

#if	! _STDC_SOURCE

FILE	      *	fdopen		__PROTO ((int _fildes,
					  __CONST__ char * _type));
int		fileno		__PROTO ((FILE * _stream));

#if	_POSIX_C_SOURCE < 2

FILE	      *	popen		__PROTO ((__CONST__ char * _command,
					  __CONST__ char * _mode));
int		pclose		__PROTO ((FILE * _stream));

#endif	/* _POSIX_C_SOURCE < 2 */
#endif	/* ! _STDC_SOURCE */

__EXTERN_C_END__

/* Macros covering standard functions. */
#define	clearerr(fp)	((fp)->_ff1 &= ~(_FERR|_FEOF))
#define	feof(fp)	((fp)->_ff1 & _FEOF)
#define	ferror(fp)	((fp)->_ff1 & _FERR)
#define	getc(fp)	(--(fp)->_cc<0 ? ++(fp)->_cc,(*(fp)->_f2p->_gt)((fp)) \
				       : *(fp)->_cp++)
#define	getchar()	getc(stdin)
#define	putc(c,fp)	(--(fp)->_cc<0 ? ++(fp)->_cc,(*(fp)->_f2p->_pt)((c),(fp)) \
				       : (*(fp)->_cp++=(c)))
#define	putchar(c)	putc((c), stdout)

/* Other macros, non-ANSI. */
#define	fileno(fp)	((fp)->_fd)

#endif	/* ! defined (__STDIO_H__) */
