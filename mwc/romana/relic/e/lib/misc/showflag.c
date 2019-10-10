/*
 * Turn flags into Display chars or -
 */
void
showflag(data, flags, output)
long data;
register char *flags;	/* low order first */
register char *output;	/* a data area as big as flags */
{
	register int c;

	for (;c = *flags++; data >>= 1)
		*output++ = (data & 1) ? c : '-';
	output = '\0';
}
