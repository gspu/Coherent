

/*
 * search environment for name
 * An environmental parameter is a string of the form "name=value".
 * A pointer to the value is returned if the supplied argument matches the
 * name, otherwise 0.  The value is not a copy, so alterations will be
 * reflected in the environment.  Bogus parameters are ignored.
 */
char	*
getenv( name)
char	*name;
{
	register char	*p,
			*q,
			**ep;
	extern char	**environ;

	ep = environ;
	if (ep == ((char **)0))
		return (((char *)0));

	while (p = *ep++) {
		for (q=name; *q==*p++; ++q)
			if (*q == '\0')
				break;
		if (*q=='\0' && p[-1]=='=')
			return (p);
	}

	return (((char *)0));
}
