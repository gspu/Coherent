/*
 * libc/stdio/_fp.c
 * ANSI-compliant C standard i/o library internals.
 * _fp[]
 * File structures.
 */

#include <stdio.h>
extern	_FILE2	_iob2[_NSTDFILE];

/*
 * Standard FILEs.
 * Rather than statically allocating _NFILE FILE structures,
 * this allocates only _NSTDFILE structures.
 * Others are malloc()'ed as needed by _fopen().
 */

FILE	_iob[_NSTDFILE]	= {

	{
		/* Standard input. */
		0,					/* _cc		*/
		NULL,					/* _cp		*/
		&_iob2[0],				/* _f2p		*/
		_FRONLY,				/* _ff1		*/
		0,					/* _fd		*/
		_FINUSE|_FASCII|_FDONTC,		/* _ff2		*/
		_MODE_UNINIT				/* _mode	*/
	},

	{
		/* Standard output. */
		0,					/* _cc		*/
		NULL,					/* _cp		*/
		&_iob2[1],				/* _f2p		*/
		_FWONLY,				/* _ff1		*/
		1,					/* _fd		*/
		_FINUSE|_FASCII|_FDONTC,		/* _ff2		*/
		_MODE_UNINIT				/* _mode	*/
	},

	{
		/* Standard error, unbuffered. */
		0,					/* _cc		*/
		NULL,					/* _cp		*/
		&_iob2[2],				/* _f2p		*/
		_IONBF|_FWONLY,				/* _ff1		*/
		2,					/* _fd		*/
		_FINUSE|_FASCII|_FDONTC,		/* _ff2		*/
		_MODE_NBUF				/* _mode	*/
	}

#if	MSDOS
	 ,
	{
		/* AUX: */
		0,					/* _cc		*/
		NULL,					/* _cp		*/
		&_iob2[3],				/* _f2p		*/
		_FRW,					/* _ff1		*/
		3,					/* _fd		*/
		_FINUSE|_FDONTC,			/* _ff2		*/
		_MODE_UNINIT				/* _mode	*/
	   },

	{
		/* PRN: */
		0,					/* _cc		*/
		NULL,					/* _cp		*/
		&_iob2[4],				/* _f2p		*/
		_FWONLY,				/* _ff1		*/
		4,					/* _fd		*/
		_FINUSE|_FASCII|_FDONTC,		/* _ff2		*/
		_MODE_UNINIT				/* _mode	*/
	}
#endif
};

_FILE2	_iob2[_NSTDFILE] = {

	{
		/* Standard input. */
		&_fginit,				/* _gt		*/
		&_fpute,				/* _pt		*/
		NULL,					/* _bp		*/
		NULL,					/* _dp		*/
		NULL,					/* _ep		*/
		NULL,					/* _nm		*/
		0					/* _uc		*/

	},

	{
		/* Standard output. */
		&_fgete,				/* _gt		*/
		&_fpinit,				/* _pt		*/
		NULL,					/* _bp		*/
		NULL,					/* _dp		*/
		NULL,					/* _ep		*/
		NULL,					/* _nm		*/
		0					/* _uc		*/
	},

	{
		/* Standard error, unbuffered. */
#if	_ASCII
		&_fgete,				/* _gt		*/
		&_fputca,				/* _pt		*/
#else
		&_fgete,				/* _gt		*/
		&_fputc,				/* _pt		*/
#endif
		NULL,					/* _bp		*/
		NULL,					/* _dp		*/
		NULL,					/* _ep		*/
		NULL,					/* _nm		*/
		0					/* _uc		*/
	}

#if	MSDOS
	 ,
	{
		/* AUX: */
		&_fginit,				/* _gt		*/
		&_fpinit,				/* _pt		*/
		NULL,					/* _bp		*/
		NULL,					/* _dp		*/
		NULL,					/* _ep		*/
		NULL,					/* _nm		*/
		0					/* _uc		*/
	   },

	{
		/* PRN: */
		&_fgete,				/* _gt		*/
		&_fpinit,				/* _pt		*/
		NULL,					/* _bp		*/
		NULL,					/* _dp		*/
		NULL,					/* _ep		*/
		NULL,					/* _nm		*/
		0					/* _uc		*/
	}
#endif
};

/* FILE pointers. */
FILE	*_fp[_NFILE] = {
	stdin,
	stdout,
	stderr
#if	MSDOS
	      ,
	stdaux,
	stdprn
#endif
};

/* end of libc/stdio/_fp.c */
