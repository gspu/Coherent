/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Unget a byte.
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

unbget(b)
{
#if	TEMPBUF
	if (ifp == NULL) {
		if (inbuf == inbufp)
			return(EOF);
		*--inbufp = b;
		return(b);
	}
#endif
	ungetc(b, ifp);
}
