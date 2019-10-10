/*
 * common/nput.c
 * Standard stream library for the C compiler
 * and other compilers that use C compiler facilities.
 * Output a name.
 */

#include <stdio.h>
#ifdef   vax
#include "INC$LIB:ops.h"
#include "INC$LIB:mch.h"
#else
#include "ops.h"
#include "mch.h"
#endif

nput(id)
register char *id;
{
	register c;

	while ((c = *id++) != 0)
		bput(c);
#if	APPENDBAR
	bput('_');
#endif
	bput(0);
}

/* end of common/nput.c */
