/*
 * Shell sort with the calling sequence of
 * qsort.
 */

#define A(v,i)  ((v)+((i)*size))

shellsort(v, n, size, compar)
register char *v;
int (*compar)();
{
	int gap, i;
	register j;

	for (gap = n/2; gap > 0; gap /= 2)
		for (i = gap; i < n; i++)
			for (j = i-gap; j>=0; j -= gap) {
				if ((*compar)(A(v,j), A(v,j+gap)) <= 0)
					break;
				qexch(A(v,j), A(v,j+gap), size);
			}
}

static
qexch(p1, p2, n)
register char *p1, *p2, n;
{
	int t;

	if (n)
		do {
			t = *p1;
			*p1++ = *p2;
			*p2++ = t;
		} while (--n);
}
