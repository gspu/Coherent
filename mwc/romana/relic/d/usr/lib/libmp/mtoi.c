/*
 *	Mtoi returns the integer equivalent of the mint pointed to by "a".
 *	If the value is too large for an int, it calls mperr with an
 *	appropriate error message.
 */
#include "mprec.h"

mtoi(a)
mint *a;
{
	register char *ap;
	register int mifl, res;

 	if (mcmp(mminint, a) > 0 || mcmp(a, mmaxint) >0)
		mperr("mtoi argument too big");
	ap = & a->val[a->len - 1];
	mifl = ispos(a);
	res = (mifl ? *ap : 0);
	while (--ap >= a->val)
		res = res * BASE + (mifl  ? *ap : NEFL - *ap);
	return (mifl ? res : - (res + 1));
}
