/*
 * libc/gen/getpwent.c
 * Coherent I/O Library.
 * Get password file entry.
 * Searches by next entry, name or numerical id.
 */

#include <stdio.h>
#include <pwd.h>

#define field(x)	{ x=cp; while (*cp++); }
#define	NPWLINE	120
#define	PWFILE	"/etc/passwd"

static	char	pwline[NPWLINE];
static	struct	passwd pw;
static	FILE	*pwfile	= { NULL };

struct passwd *
getpwnam(name)
char *name;
{
	register struct passwd *pwp;

	setpwent();
	while ((pwp = getpwent()) != NULL)
		if (streq(name, pwp->pw_name))
			return pwp;
	return NULL;
}

struct	passwd *
getpwuid(uid)
{
	register struct passwd *pwp;

	setpwent();
	while ((pwp = getpwent()) != NULL)
		if (uid == pwp->pw_uid)
			return pwp;
	return NULL;
}

struct passwd *
getpwent()
{
	register char *cp, *xp;
	register int c, nseps;

	if (pwfile == NULL)
		if ((pwfile = fopen(PWFILE, "r")) == NULL)
			return NULL;
again:
	cp = pwline;
	for (nseps = 0; (c = getc(pwfile))!=EOF && c!='\n'; ) {
		if (c == ':') {
			c = '\0';
			++nseps;
		}
		if (cp < &pwline[NPWLINE-1])
			*cp++ = c;
	}
	if (c == EOF)
		return NULL;
	if (nseps != 6)
		goto again;		/* accept only lines with six ':'s */
	*cp = '\0';
	cp = pwline;
	field(pw.pw_name);
	field(pw.pw_passwd);
	field(xp);
	pw.pw_uid = atoi(xp);
	field(xp);
	pw.pw_gid = atoi(xp);
	field(pw.pw_comment);
	pw.pw_gecos = pw.pw_comment;
	field(pw.pw_dir);
	field(pw.pw_shell);
	return &pw;
}

setpwent()
{
	if (pwfile != NULL)
		rewind(pwfile);
}

endpwent()
{
	if (pwfile != NULL) {
		fclose(pwfile);
		pwfile = NULL;
	}
}

/* end of getpwent.c */
