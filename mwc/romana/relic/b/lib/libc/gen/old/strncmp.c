/*
 * Coherent Library
 * Compare up to n chars of strings
 * Does UNSIGNED compare (which is both reasonable and portable)
 */

int
strncmp(p, q, n)
register unsigned char	*p, *q;
register int	n;
{
	while (--n >= 0) {
		if (*p != *q)
			return (*p - *q);
		else if (*p == '\0')
			break;
		p++;
		q++;
	}
	return (0);
}
