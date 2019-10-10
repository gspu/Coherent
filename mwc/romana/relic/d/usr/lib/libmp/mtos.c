#include "mprec.h"
#include <assert.h>


/*
 *	Sizfac is an array whoose obase - 2 component contains the
 *	maximum number of characters needed per byte of mint when
 *	converted to a string in base obase.  This simply means that 
 *	sizfac[obase - 2] contains the log (base obase) of BASE, rounded
 *	up.
 */

static int	sizfac[] = {
	7,	/* 2^7 = 128 */
	5,	/* 3^5 = 243 */
	4,	/* 4^4 = 256 */
	3,	/* 5^3 = 125 */
	3,	/* 6^3 = 216 */
	3,	/* 7^3 = 343 */
	3,	/* 8^3 = 512 */
	3,	/* 9^3 = 729 */
	3,	/* 10^3 = 1000 */
	3,	/* 11^3 = 1331 */
	2,	/* 12^2 = 144 */
	2,	/* 13^2 = 169 */
	2,	/* 14^2 = 196 */
	2,	/* 15^2 = 225 */
	2	/* 16^2 = 256 */
};


/*
 *	Dtc is an array of characters indexed by the allowable
 *	digist (0 to 15).  Dtc[i] is the character to represent
 *	the digit i.
 */

static char	dtc[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};


/*
 *	Obase is a int which is the output base used by mtos.  The value
 *	of obase shoud be between 2 and 16.  For bases greater than 10
 *	the characters A to F are used as digits.
 */

int	obase = 10;


/*
 *	Mtos converts the mint pointed to by "a" into a string of characters
 *	in base obase.  It returns a pointer to that string.  Note that
 *	obase is assumed to satisfy 2 <= obase <= 16.
 *	Also note that space for the string is allocated useing mpalc so
 *	that to release the space, the caller should use mpfree.
 */

char *
mtos(a)
mint *a;
{
	register char *rp, *to;
	mint	ac;
	int mifl;
	int dig;
	unsigned size;
	char *res;

	assert(2 <= obase <= 16);

	/* set ac = abs(a), copying if necessary */
	minit(&ac);
	if (mifl = ispos(a))
		mcopy(a, &ac);
	else
		mneg(a, &ac);

	/* calculate maximum size of result and allocate space */
	size = 1 + ac.len * sizfac[obase - 2];
	if (!mifl)
		++size;
	res = (char *)mpalc(size);
	if (res == NULL)
		mperr("No space for string.");

	/*	create string - from back to front */
	rp = res + size;
	*--rp = '\0';
	do {
		sdiv(&ac, obase, &ac, &dig);
		*--rp = dtc[dig];
	} while (!zerop(&ac));
	mpfree(ac.val);
	if (!mifl)
		*--rp = '-';
	assert(rp >= res);

	/* left justify string if necessary */
	if (rp != res) {
		to = res;
		while ((*to++ = *rp++) != '\0')
			;
	}

	return(res);
}
