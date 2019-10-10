/*
 * Check for Yes No
 */
yesNo(s)
register char *s;
{
	switch (*s) {
	case 'Y':
		*s = 'y';
		return (1);
	case 'N':
		*s = 'n';
	case 'y':
	case 'n':
		return (1);
	}
	showError("Must be y or n");
	return (0);
}
