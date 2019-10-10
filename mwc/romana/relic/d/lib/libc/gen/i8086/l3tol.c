/*
 * Convert an array of filesystem 3 byte
 * numbers to longs. This routine, unlike the old one,
 * is independent of the order of bytes in a long.
 * Bytes have 8 bits, though.
 */
l3tol(lp, cp, nl)
register long *lp;
register unsigned char *cp;
register unsigned nl;
{
	register long l;

	if (nl != 0) {
		do {
			l  = (long)cp[0] << 16;
			l |= (long)cp[1];
			l |= (long)cp[2] << 8;
			cp += 3;
			*lp++ = l;
		} while (--nl);
	}
}
