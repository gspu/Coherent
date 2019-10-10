#include <stdio.h>

char	*__modemcap;
char	*MODEMCAP = "/etc/modemcap";

static	isent (ent, name)
char	*ent;
char	*name;
{
	char	buf[16];
	register int	i;

	while (*ent != ':' && *ent != 0) {
		for (i = 0;*ent != ':' && *ent != '|' && *ent != 0 && i < 15;i++)
			buf[i] = *ent++;

		if (*ent == '|')
			ent++;

		buf[i] = 0;
		if (strcmp (buf, name) == 0)
			return (1);
	}
	return (0);
}

mgetent (bp, name)
char	*bp;
char	*name;
{
	static	char	buf[1024];
	register char	*cp;
	register FILE	*modemcap;
	register int	i;
	char	*getenv ();

	if ((cp = getenv ("MODEMCAP")) != NULL) {
		if (*cp != '/') {
			if (isent (cp, name)) {
				strcpy (buf, cp);
				return (1);
			}
		}
		MODEMCAP = cp;
	}
	if ((modemcap = fopen (MODEMCAP, "r")) == NULL)
		return (-1);

	while (fgets (buf, 512, modemcap) != NULL) {
		if (buf[0] == '#')		/* skip all comment lines*/
			continue;

		i = strlen (buf) - 1;		/* find last character in line*/
		buf[i] = 0;			/* remove trailing newline*/
		if (i == 0)			/* ignore blank lines	*/
			continue;

		while (buf[(i = strlen (buf) - 1)] == '\\') {	/* is last character a \\, still more*/
			cp = &buf[i];		/* find last character	*/
			cp[0] = 0;		/* nullify, end of this part*/
			if (fgets (cp, 512, modemcap) == NULL)	/* end of file?	...	*/
				break;		/* ... end of entry	*/

			cp[strlen (cp) - 1] = 0;/* remove trailing newline*/
			if (cp[0] == '#') {	/* comment line? ...	*/
				cp[0] = 0;	/* remove that line	*/
				continue;	/* go get another line	*/
			}
		}
		if (isent (buf, name)) {
			__modemcap = bp;
			strcpy (bp, buf);
			fclose (modemcap);
			return (1);
		}
	}
	fclose (modemcap);
	return (0);
}
