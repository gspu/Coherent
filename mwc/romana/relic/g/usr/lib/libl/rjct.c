#include "lexlib.h"
yyrjct()
{
	ll_tf = ll_tokn;
	ll_lf = ll_tlen;
	yyless(yyleng-ll_tlen);
}
