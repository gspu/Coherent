/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#include "lexlib.h"
yyless(n)
{
	if (n < 0)
		return;
	if (yyleng-n+qct(ll_tq) > QSIZE)
		error(inpovf);
	while (yyleng > n) {
		qug(ll_tq, yytext[--yyleng]);
		if (ll_tlen)
			--ll_tlen;
	}
	if (yyleng)
		ll_lc = yytext[yyleng-1];
	yytext[yyleng] = '\0';
}
