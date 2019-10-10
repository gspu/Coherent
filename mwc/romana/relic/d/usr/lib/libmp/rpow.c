#include "mprec.h"


/*
 *	Rpow sets the mint pointed to by "c" to the mint pointed to by "a"
 *	raised to the mint pointed to by "b" power.  If "b" is negative then
 *	mperr is called with the appropriate error message.
 *	Note that no assumption is made as to the distinctness of "a", "b" and
 *	"c".
 */

void
rpow(a, b, c)
register mint *a, *b, *c;
{
	mint	al, bl;
	int rem;

	if (!ispos(b))
		mperr("negative power");

	/* make local copies of a and b */
	minit(&al);
	mcopy(a, &al);
	minit(&bl);
	mcopy(b, &bl);

	/* form actual power */
	sdiv(&bl, 2, &bl, &rem);
	if (rem != 0)
		mcopy(&al, c);
	else
		mcopy(mone, c);
	while (!zerop(&bl)) {
		mult(&al, &al, &al);
		sdiv(&bl, 2, &bl, &rem);
		if (rem != 0)
			mult(c, &al, c);
	}

	/* clean up garbage */
	mpfree(al.val);
	mpfree(bl.val);
}
