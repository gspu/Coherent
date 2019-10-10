/*
 * Quicker sort algorithm (from C. A. R. Hoare)
 * This uses linear insertion sort for number of elements
 * smaller than M in some partitioning.
 */

char	*qmedian();

#define	M	10

qsort(base, nel, width, compar)
register char *base;
unsigned nel, width;
int (*compar)();
{
	register char *bot, *top;

	bot = base;
	top = base + nel*width;
	if (nel < M) {
		if (nel > 1)
			qlinsert(bot, nel, width, compar);
		return;
	}
	qexchange(bot, qmedian(compar, bot, bot+(nel/2)*width, top-width), width);
	for (;;) {
		while ((*compar)(base, bot+=width)>=0 && bot<top)
			;
		while ((*compar)(top-=width, base)>=0 && top>base)
			;
		if (bot < top)
			qexchange(bot, top, width);
		else
			break;
	}
	qexchange(top, base, width);
	qsort(base, (top-base)/width, width, compar);
	qsort(bot, nel - (bot-base)/width, width, compar);
}

/*
 * Exchange two records pointed to by
 * `p1' and `p2'.  Each record is of `width'
 * bytes.
 */
static
qexchange(p1, p2, width)
register char *p1, *p2;
register unsigned width;
{
	int save;

	if (width)
		do {
			save = *p1;
			*p1++ = *p2;
			*p2++ = save;
		} while (--width);
}

/*
 * Produce the median of the first, middle,
 * and last elements of a file by calling
 * the compare routine.
 */
static char *
qmedian(comp, a, b, c)
int (*comp)();
char *a, *b, *c;
{
	register char *bmin, *bmax;

	if ((*comp)(a, b) < 0) {
		bmin = a;
		bmax = b;
	} else {
		bmin = b;
		bmax = a;
	}
	if ((*comp)(bmax, c) < 0)
		return (bmax);
	return ((*comp)(bmin, c) < 0 ? c : bmin);
}

/*
 * Linear insertion sort used to speed up
 * final sorts when parititions get relatively small.
 */
static
qlinsert(base, nel, width, compar)
char *base;
register unsigned nel;
unsigned width;
int (*compar)();
{
	register char *min, *mbase;
	int n;

	--nel;
	do {
		n = nel;
		min = base;
		mbase = min+width;
		do {
			if ((*compar)(mbase, min) < 0)
				min = mbase;
			mbase += width;
		} while (--n);
		qexchange(min, base, width);
		base += width;
	} while (--nel);
}
