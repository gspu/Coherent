#include "modemcap.h"

static	char	f_names[] = "asditthc";
static	char	*f_caps[] = {
	&AS, &DI, &TT, &HC
};

static	char	i_names[] = "bdblidad";
static	int	*i_caps[] = {
	&BD, &BL, &ID, &AD
};

static	char	c_names[] = "cscedsdeiscoclathu";
static	char	**c_caps[] = {
	&CS, &CE, &DS, &DE, &IS, &CO, &CL, &AT, &HU
};

initmodem (modem, fd)
char	*modem;					/* name of modem	*/
int	fd;					/* channel to modem	*/
{
	static	char	mcapbuf[1024];
	static	char	area[1024];
	char	*ap = area;
	register char	*cp;
	register int	i, j;
	char	*mgetstr ();

	if (mgetent (mcapbuf, modem) != 1)
		return (0);

	for (i = 0, cp = f_names;*cp;i++, cp += 2)
		*(f_caps[i]) = mgetflag (cp);

	for (i = 0, cp = i_names;*cp;i++, cp += 2)
		*(i_caps[i]) = ((j=mgetnum(cp)) >= 0) ? j: 0;

	for (i = 0, cp = c_names;*cp;i++, cp += 2)
		*(c_caps[i]) = mgetstr (cp, &ap);

	if (IS != (char *) 0) {
		write (fd, IS, strlen (IS));
		if (ID)
			sleep (ID);
	}
	return (1);
}
