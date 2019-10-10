/*
 * libc/gen/ieee_f.c
 */

/*
 * ieee_f(float *ifp, float *dfp)
 * Convert float from DECVAX format to IEEE format.
 * dfp points to a DECVAX format float to convert.
 * ifp points to a destination for the converted IEEE value;
 * ifp may be identical to dfp for in-place conversion.
 * The significand remains unchanged.
 * Returns 0 if successful, -1 on underflow.
 */

int
ieee_f(ifp, dfp) float *ifp, *dfp;
{
	register int exp, s;
	register unsigned char *cp;

	/* Copy to destination. */
	cp = (char *)ifp;
	if (ifp != dfp)
		memcpy(cp, (char *)dfp, sizeof(float));

	/* Adjust exponent bias from 129 to 127. */
	exp = ((cp[3] & 0x7F) << 1) | ((cp[2] & 0x80) >> 7);
	if (exp == 0)
		return 0;
	else if (exp > 2) {
		/* Subtract 2 by decrementing the low bit of the hi byte. */
		--cp[3];
		return 0;
	}

	/* Exponent underflow. */
	cp[3] = cp[2] = cp[1] = cp[0] = 0;
	return -1;
}

/* end of libc/gen/ieee_f.c */
