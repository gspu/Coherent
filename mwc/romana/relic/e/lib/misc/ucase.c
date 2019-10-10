/*
 * Convert a string to upper case.
 */
char *
ucase(s)
char *s;
{
	register char *p, c;

	for (p = s; c = *p; p++)
		if ((c >= 'a') && (c <= 'z'))
			*p = c + 'A' - 'a';
	return (s);
}
