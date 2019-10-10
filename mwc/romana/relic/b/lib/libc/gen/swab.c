/*
 * Swap odd and even bytes of `f' into `t'
 * for `n' bytes.  If `n' is odd, the last byte
 * is not touched.
 */

swab(f, t, n)
register char *f, *t;
register unsigned n;
{
	register int tmp;

	while (n >= 2) {
		tmp = *f++;
		*t++ = *f++;
		*t++ = tmp;
		n -= 2;
	}
}
