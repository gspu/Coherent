#include "mprec.h"


/*
 *	Mzero and mone are pointers to mints with values zero and one
 *	respectively.  They are used as convenient constants in various
 *	routines.  Mminint and Mmaxint are pointers to mints with value
 *	the minimum and maximum integer which will fit into an int.
 *	They are used to check for overflow on conversion.  Care should
 *	be taken that none of these mints is ever changed.
 */
static char	__mzero = 0;
static mint	_mzero = {1, &__mzero};
mint	*mzero = &_mzero;

static char	__mone = 1;
static mint	_mone = {1, &__mone};
mint	*mone = &_mone;

#ifdef _I386
static char	__mminint[] = {   0,	0,    0,    0, 0170, 0177};
static char	__mmaxint[] = {0177, 0177, 0177, 0177, 7};
static mint	_mminint = {6, __mminint};
static mint	_mmaxint = {5, __mmaxint};
#else
static char	__mminint[] = {0, 0, 0176, 0177};	/* min int (-32768) */
static char	__mmaxint[] = {0177, 0177, 1};	/* max int (32767) */
static mint	_mminint = {4, __mminint};
static mint	_mmaxint = {3, __mmaxint};
#endif
mint	*mminint = &_mminint;

mint	*mmaxint = &_mmaxint;
