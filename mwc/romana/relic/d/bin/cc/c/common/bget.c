/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Input a byte.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#include "INC$LIB:stream.h"
#else
#include "mch.h"
#include "stream.h"
#endif

extern	FILE	*ifp;

bget()
{
#if	TEMPBUF
	if (ifp == NULL) {
		if (inbufp == inbufmax)
			return(EOF);
		return(*inbufp++);
	}
#endif
#if	MSDOS
	return (_bingetc(ifp));
#else
	return (getc(ifp));
#endif
}
