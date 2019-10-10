/*
 * Check for numeric field.
 */
numeric(s)
register char *s;
{
	register c;

	while (c = *s++)
		if ((c < '0') || (c > '9')) {
			showError("Field must be numeric");
			return (0);
		}
	return (1);
}
