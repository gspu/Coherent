/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
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
#if !OMF286
	bput('_');
#endif
	bput(0);
}
