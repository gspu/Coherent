/*
 * libc/gen/decvax_f.c
 */

/*
 * decvax_f(float *dfp, float *ifp)
 * Convert float from IEEE format to DECVAX format.
 * ifp points to an IEEE format float to convert.
 * dfp points to a destination for the converted DECVAX value;
 * dfp may be identical to ifp for in-place conversion.
 * The significand remains unchanged.
 * Returns 0 if successful, 1 on overflow.
 */

int
decvax_f(dfp, ifp) float *dfp, *ifp;
{
	register unsigned char *cp;
	register int exp, a012;

	/* Copy to destination. */
	cp = (char *)dfp;
	if (dfp != ifp)
		memcpy(cp, (char *)ifp, sizeof(float));

	/* Extract biased exponent. */
	exp = ((cp[3] & 0x7F) << 1) | ((cp[2] & 0x80) >> 7);

	/* Adjust exponent bias from 127 to 129. */
	if (exp == 0) {
		a012 = (cp[2] & 0x0F) | cp[1] | cp[0];
		memset((char *)dfp, 0, 4);	/* set to 0.0 */
		return (a012 == 0) ? 0 : -1;	/* return -1 for denormals */
	} else if (exp < 256) {
		/* Add 2 by incrementing the low bit of the hi byte. */
		++cp[3];
		return 0;
	}

	/* Exponent overflow, also returned for NaNs. */
	cp[3] |= 0x7F;
	cp[2] = cp[1] = cp[0] = 0xFF;
	return 1;
}

/* end of libc/gen/decvax_f.c */
