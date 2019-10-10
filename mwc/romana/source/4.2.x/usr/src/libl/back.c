/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#include "lexlib.h"

yyback (c)
{
	if (qct (ll_tq) == QSIZE)
		error (inpovf);
	qug (ll_tq, c);
	return c;
}
