/*
 *	Mitom sets the mint pointed to by "mp" to have a value equal
 *	to the int "n".
 */
#include "mprec.h"
void
mitom(n, mp)
int	n;
mint	*mp;
{
	register char *rp, *limit, *value;
	unsigned an;
	char mifl;	/* minus flag */
	char tev[NORSIZ];	/* temporary for converted value */

	mpfree(mp->val);
	an = ((mifl = n<0) ? -n : n);
	rp = tev;
	while (an != 0) {
		*rp++ = an % BASE;
		an >>= L2BASE;
	}
	*rp++ = 0;
	limit = rp;
	if (mifl) {
		rp = tev - 1;
		while (++rp < limit)
			*rp = NEFL - *rp;
		++*tev;
	}
	value = (char *)mpalc(limit - tev);
	mp->val = value;
	mp->len = limit - tev;
	rp = tev;
	while (rp < limit)
		*value++ = *rp++;
	norm(mp);
}
