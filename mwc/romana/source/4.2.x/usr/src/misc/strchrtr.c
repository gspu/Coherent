/*
 * Lookup a char on a string, return the corresponding
 * char on a second string or a default.
 */
strchrtr(from, to, c, def)
char *from, *to;
register int c;
int def;
{
	register int i, fc;

	for (i = 0; fc = from[i]; i++)
		if (fc == c)
			return (to[i]);
	return (def);
}
