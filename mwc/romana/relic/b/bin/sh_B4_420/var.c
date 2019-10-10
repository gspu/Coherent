/*
 * sh/var.c
 * Bourne shell.
 * Variables.
 */

#include "sh.h"

VAR *vnode();

/*
 * Initial variable list.
 */

/*
 * Initialize variables.
 *	Clear the list, if any; reset initial values, and initialize
 *	the environment.
 */
initvar(envp)
char **envp;
{
	register char **nvp;
	char *wd;
	static struct initvals {
		int i_flag;
		char *i_name;
	} initvals[] = {
		VSET,	"IFS= \t\n",
		VSET,	"PS2=> ",
		VSET,	"PS1=$ ",
		VSET,	"MAIL=",
		VSET,	"PATH=:/bin:/usr/bin",
		VSET,	"HOME=",
		VSET,	"CWD=",
		VSET,	"VERSION=" VERSION,
		0,	NULL
	};
	static char lasterror[] = "LASTERROR";

	if (varp) {
		register VAR *vp, *tvp;

		for (vp = varp; tvp = vp; ) {
			vp = vp->v_next;
			sfree(tvp->v_strp);
			sfree(tvp);
		}
		varp = NULL;
	}

	{
		register struct initvals *ivp;

		for (ivp=initvals; ivp->i_name != NULL; ivp+=1)
			flagvar(ivp->i_name, ivp->i_flag);
	}
	if (senvp != NULL) {
		vfree(senvp);
	}
	senvp = envp;
	for (nvp = senvp; nvp && *nvp != NULL; )
		setsvar(*nvp++);
	if (findvar(lasterror) == NULL)
		flagvar(lasterror, VEXP);
	if (findvar("CWD") == NULL)
		flagvar("CWD", VEXP);
	if ((wd = _getwd()) == NULL)
		wd = ".";			/* _getwd() failed */
	dstack[dstkp] = wd = duplstr(wd, 1);	/* to dstack */
	assnvar("CWD", wd);			/* and to $CWD */
}

/*
 * Make sure the given variable name is valid.
 * Only user settable names are accepted.
 */
namevar(np)
char *np;
{
	register int c;
	register char *cp;

	cp = np;
	if (class(*cp, MBVAR)) {
		do c = *cp++;
		while (class(c, MRVAR));
		if (c=='\0' || c=='=')
			return (1);
	}
	return (0);
}

/*
 * Return a pointer to the given variable.
 */
VAR *
findvar(cp)
register char *cp;
{
	register VAR *vp;

	for (vp=varp; vp; vp=vp->v_next)
		if (vareq(cp, vp->v_strp))
			break;
	return (vp);
}

/*
 * Given a variable pointer, return a pointer to a string
 * containing the value.
 */
char *
convvar(vp)
register VAR *vp;
{
	register char *cp;

	cp = vp->v_strp;
	for (;;) {
		if (*cp == '\0')
			return (NULL);
		if (*cp++ == '=')
			return (cp);
	}
}

/*
 * Assign a value to the given shell variable.
 */
VAR *
assnvar(np, cp)
char *np;
char *cp;
{
	strt[0] = '\0';
	strcat(strt, np);
	strcat(strt, "=");
	strcat(strt, cp);
	return (setsvar(strt));
}

/*
 * Set the given shell variable.
 */
VAR *
setsvar(cp)
register char *cp;
{
	register VAR *vp;

#ifdef VERBOSE
	if (xflag)
		prints("<%d> setsvar(%s)\n", getpid(), cp);
#endif
	if ((vp=findvar(cp)) == NULL)
		vp = varp = vnode(0, NULL, varp);
	if (vp->v_flag & VRDO) {
		printe("Cannot set %s", cp);
		return (NULL);
	}
	sfree(vp->v_strp);
	vp->v_strp = duplstr(cp, 1);
	if (vp->v_flag & VSET)
		setsint(vp->v_strp);
	return (vp);
}

/*
 * Set the internal variable to the shell variable.
 */
setsint(np)
register char *np;
{
	register char *cp;
	static struct intsvals {
		char *j_name;
		char **j_save;
	} intsvals[] = {
		"HOME",	&vhome,
		"IFS",	&vifs,
		"MAIL",	&vmail,
		"PATH",	&vpath,
		"PS1",	&vps1,
		"PS2",	&vps2,
		NULL,	NULL
	};
	register struct intsvals *ivp;

	cp = np;
	while (*cp != '\0')
		if (*cp++ == '=')
			break;
	for (ivp=intsvals; ivp->j_name != NULL; ivp+=1)
		if (vareq(ivp->j_name, np)) {
			*ivp->j_save = cp;
			break;
		}
}

/*
 * Set flags in the shell variable.
 */
VAR *
flagvar(cp, f)
register char *cp;
{
	register VAR *vp;

	if ((vp=findvar(cp)) == NULL)
		vp = varp = vnode(f, duplstr(cp, 1), varp);
	else if (index(cp, '=') != NULL
	 && (vp = setsvar(cp)) == NULL)
		return (NULL);
	vp->v_flag |= f;
	if (f & VEXP)
		setsexp(vp->v_strp);
	if (f & VSET)
		setsint(vp->v_strp);
	return (vp);
}

setsexp(np)
register char *np;
{
	register char **envp;

	/* New export values supersede inherited ones */
	for (envp=senvp; envp && *envp; envp+=1)
		if (vareq(np, *envp)) {
			do envp[0]=envp[1];
			while (*envp++ != NULL);
			break;
		}
}

/*
 * See if two variable names are equal.
 */
vareq(cp1, cp2)
register char *cp1;
register char *cp2;
{
	int c2;
	register int c1;

	for (;;) {
		if ((c1=*cp1++) == '\0')
			c1 = '=';
		if ((c2=*cp2++) == '\0')
			c2 = '=';
		if (c1 != c2)
			return (0);
		if (c1 == '=')
			return (1);
	}
}

/*
 * Report function for variables.
 */
tellvar(f)
{
	register VAR *vp;
	register char **evp;

	if (f==VEXP)
		for (evp=senvp; evp && *evp; )
			prints("%s\n", *evp++);
	for (vp=varp; vp; vp=vp->v_next)
		if (f==0 || (vp->v_flag&f))
			prints("%s\n", vp->v_strp);
}

/*
 * Scan the varlist for exportables.
 *  If an exportable matches a variable already in envp, forget it.
 */
char **
envlvar(envp)
char **envp;
{
	register char **enpp;
	register char **oenvp;
	register VAR *vp;

	for (oenvp=senvp; oenvp && *oenvp; oenvp+=1) {
		for (enpp=envp; ; enpp+=1)
			if (*enpp==NULL || vareq(*oenvp, *enpp))
				break;
		if (*enpp==NULL)
			envp = addargl(envp, duplstr(*oenvp, 0));
	}
	for (vp=varp; vp; vp=vp->v_next) {
		if (vp->v_flag&VEXP) {
			for (enpp=envp; ; enpp+=1)
				if (*enpp==NULL || vareq(*enpp, vp->v_strp))
					break;
			if (*enpp==NULL)
				envp = addargl(envp, duplstr(vp->v_strp, 0));
		}
	}
	return (envp);
}

VAR *
vnode(f, s, n)
char *s;
VAR *n;
{
	register VAR *vp;

	vp = (VAR *)salloc(sizeof *vp);
	vp->v_flag = f;
	vp->v_strp = s;
	vp->v_next = n;
	return (vp);
}

/* end of sh/var.c */
