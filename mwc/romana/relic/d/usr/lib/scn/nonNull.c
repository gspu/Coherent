/*
 * Check for some data in field.
 */
nonNull(s)
register char *s;
{
	if (*s)
		return (1);

	showError("Field must contain data");
	return (0);
}
