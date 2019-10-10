/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#include "lexlib.h"

void
yyrjct ()
{
	ll_tf = ll_tokn;
	ll_lf = ll_tlen;
	yyless (yyleng - ll_tlen);
}
