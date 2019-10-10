#include <stdio.h>
#ifdef	vax
#include "INC$LIB:mch.h"
#include "INC$LIB:stream.h"
#include "INC$LIB:ops.h"
#else
#include "mch.h"
#include "stream.h"
#include "ops.h"
#endif

/*
 * Copy debug label information.
 */

ival_t icpy();

tcpy(class)
{
	register int n;
	register int nline;
	register int c;
	register sizeof_t z;
	extern int line;

	/* Write class */
	bput(class);

	/* Read line number */
	nline = icpy();

	/* Get value */
	if (class < DC_LINE)
		;
	else if (class < DC_AUTO)
		line = nline;
	else if (class < DC_MOS)
		icpy();
	else {
		bcpy();
		bcpy();
		icpy();
	}

	/* Get name */
	while (bcpy() != 0);

	/* Get type */
	for (;;) {
		c = bcpy();
		z = zget();
		zput(z);
		switch (c) {
		case DT_NONE:
		case DT_CHAR:
		case DT_UCHAR:
		case DT_SHORT:
		case DT_USHORT:
		case DT_INT:
		case DT_UINT:
		case DT_LONG:
		case DT_ULONG:
		case DT_FLOAT:
		case DT_DOUBLE:
		case DT_VOID:
			break;
		case DT_STRUCT:
		case DT_UNION:
		case DT_ENUM:
		case DD_PTR:
		case DD_FUNC:
		case DD_ARRAY:
			continue;
		case DX_MEMBS:
			n = z;
			while (--n >= 0) tcpy(bget());
			break;
		case DX_NAME:
			while (bcpy() != 0);
			break;
		default:
			cbotch("bad type in tcpy: %d", n);
		}
		break;
	}
}

static
bcpy()
{
	register int n;

	n = bget();
	bput(n);
	return (n);
}

static ival_t
icpy()
{
	register ival_t i;

	i = iget();
	iput(i);
	return (i);
}
