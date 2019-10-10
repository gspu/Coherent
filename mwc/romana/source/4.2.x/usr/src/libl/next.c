/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#include "lexlib.h"

int
yynext ()
{
	return qct (ll_tq) ? qgt (ll_tq) : _llic ();
}
