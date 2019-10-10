#include "lexlib.h"
yyback(c)
{
	if (qct(ll_tq) == QSIZE)
		error(inpovf);
	qug(ll_tq, c);
	return (c);
}
