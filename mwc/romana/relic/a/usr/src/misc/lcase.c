/*
 * Convert a string to lower case.
 */
char *
lcase(s)
char *s;
{
	register char *p, c;

	for(p = s; c = *p; p++)
		if((c >= 'A') && (c <= 'Z'))
			*p = c + 'a' - 'A';
	return(s);
}
