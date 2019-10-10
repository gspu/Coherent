/*
 * Standard I/O Library Internals
 * File structures
 * (note stderr default unbuffered)
 */

#include <stdio.h>

extern	int	_fginit(),
		_fpinit();
FILE	*_fp[_NFILE] = { &_stdin, &_stdout, &_stderr };
FILE	_stdin	= { NULL, NULL, NULL, 0, &_fginit, &_fpinit, _FINUSE, 0, 0 };
FILE	_stdout	= { NULL, NULL, NULL, 0, &_fginit, &_fpinit, _FINUSE, 1, 0 };
FILE	_stderr	= { NULL, NULL, NULL, 0, &_fginit, &_fpinit, _FINUSE|_FSTBUF, 2, 0 };
