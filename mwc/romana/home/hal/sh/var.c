/*
 * Bourne shell.
 * Variables.
 */

#include <common/tricks.h>
#include <limits.h>

#include "shellio.h"
#include "sh.h"

/*
 * Construct a variable.
 */

#if	USE_PROTO
VAR * vnode (int f, char * s, VAR * n)
#else
VAR *
vnode (f, s, n)
int		f;
char	      *	s;
VAR	      *	n;
#endif
{
	VAR	      *	vp;

	vp = (VAR *) salloc (sizeof * vp);
	vp->v_flag = f;
	vp->v_strp = s;
	vp->v_next = n;
	return vp;
}


/*
 * See if two variable names are equal.
 */

#if	USE_PROTO
LOCAL int vareq (CONST char * cp1, CONST char * cp2)
#else
LOCAL int
vareq (cp1, cp2)
CONST char    *	cp1;
CONST char    *	cp2;
#endif
{
	int c2;
	int c1;

	for (;;) {
		if ((c1 = * cp1 ++) == 0)
			c1 = '=';
		if ((c2 = * cp2 ++) == 0)
			c2 = '=';
		if (c1 != c2)
			return 0;
		if (c1 == '=')
			return 1;
	}
}


/*
 * Return a pointer to the given variable.
 */

#if	USE_PROTO
VAR * findvar (CONST char * cp)
#else
VAR *
findvar (cp)
CONST char    *	cp;
#endif
{
	VAR * vp;

	for (vp = varp ; vp ; vp = vp->v_next)
		if (vareq (cp, vp->v_strp))
			break;
	return vp;
}


/*
 * Set the internal variable to the shell variable.
 */

#if	USE_PROTO
LOCAL void set_internal (CONST char * np)
#else
LOCAL void
set_internal (np)
CONST char    *	np;
#endif
{
	CONST char    *	cp;
static	struct {
		char	      *	i_name;
		CONST char   **	i_save;
	} intsvals [] = {
		"HOME",		& vhome,
		"IFS",		& vifs,
		"MAIL",		& vmail,
		"PATH",		& vpath,
		"PS1",		& vps1,
		"PS2",		& vps2
	};
#if	0
static	struct {
		char	      *	i_name;
		int	      *	i_save;
	} intivals [] = {
		"OPTIND", 	& optind
	};
#endif
	int		i;

	cp = np;
	while (* cp != 0)
		if (* cp ++ == '=')
			break;

	for (i = 0 ; i < ARRAY_LENGTH (intsvals) ; i ++)
		if (vareq (intsvals [i].i_name, np)) {
			* intsvals [i].i_save = cp;
			return;
		}
#if	0
	for (i = 0 ; i < ARRAY_LENGTH (intivals) ; i ++)
		if (vareq (intivals [i].i_name, np)) {
			intivals [i].i_save = strtol (cp, NULL, 0);
			return;
		}
#endif
}


/*
 * Set the given shell variable.
 */

#if	USE_PROTO
VAR * setsvar (CONST char * cp)
#else
VAR *
setsvar (cp)
CONST char    *	cp;
#endif
{
	VAR * vp;

	if ((vp = findvar (cp)) == NULL)
		vp = varp = vnode (0, NULL, varp);

	if (vp->v_flag & VAR_READONLY) {
		shellerr_simple ("variable is readonly", cp);
		return NULL;
	}

	if (vp->v_strp != NULL)
		sfree (vp->v_strp);
	vp->v_strp = duplstr (cp, 1);
	if (vp->v_flag & VAR_SETTER)
		set_internal (vp->v_strp);
	return vp;
}


/*
 * Set flags in the shell variable.
 */

#if	USE_PROTO
VAR * flagvar (CONST char * cp, int f)
#else
VAR *
flagvar (cp, f)
CONST char    *	cp;
int		f;
#endif
{
	VAR * vp;

	if ((vp = findvar (cp)) == NULL)
		vp = varp = vnode (f, duplstr (cp, 1), varp);
	else if (strchr (cp, '=') != NULL && (vp = setsvar (cp)) == NULL)
		return NULL;
	vp->v_flag |= f;
	if (f & VAR_SETTER)
		set_internal (vp->v_strp);
	return vp;
}


/*
 * Initialize variables.
 *	Clear the list, if any; reset initial values, and initialize
 *	the environment.
 */

void
initvar (envp)
CONST char   **	envp;
{
	static struct initvals {
		int 		i_flag;
		char          *	i_name;
	} initvals [] = {
		VAR_SETTER,	"IFS= \t\n",
		VAR_SETTER,	"PS2=> ",
		VAR_SETTER,	"PS1=$ ",
		VAR_SETTER,	"MAIL=",
		VAR_SETTER,	"PATH=:/bin:/usr/bin",
		VAR_SETTER,	"HOME=",
#if	MAINTAIN_CWD
		VAR_SETTER | VAR_EXPORT,	"CWD=",
#endif
		VAR_SETTER,	"SH_VERSION=" VERSION
#if	0
		VAR_SETTER,	"OPTIND=1"
#endif
	};

	while (varp != NULL) {
		VAR	      *	temp = varp;

		varp = temp->v_next;
		sfree (temp->v_strp);
		sfree (temp);
	}

	{
		struct initvals * ivp;

		for (ivp = initvals;
		     ivp != initvals + ARRAY_LENGTH (initvals) ; ivp ++)
			flagvar (ivp->i_name, ivp->i_flag);
	}

	while (envp && * envp != NULL)
		flagvar (* envp ++, VAR_EXPORT);

#if	MAINTAIN_CWD || DIRECTORY_STACK
	{
		char		temp [PATH_MAX];
		char	      *	wd;

		if (getcwd (temp, sizeof (temp)) == NULL)
			strcpy (temp, ".");
#if	DIRECTORY_STACK
		{
			int		i;
			for (i = 0 ; i <= dstkp ; i ++)
				if (dstack [i] != NULL)
					sfree (dstack [i]);
			dstkp = 0;
		}
		dstack [dstkp] = wd = duplstr (temp, 1);/* to dstack */
#endif
		assnvar (sizeof ("CWD") - 1, "CWD", wd);/* and to $CWD */
	}
#endif
}


/*
 * Destroy a given variable. Returns -1 if the variable could not be found.
 */

#if	USE_PROTO
int unsetvar (CONST char * cp)
#else
int
unsetvar (cp)
CONST char    *	cp;
#endif
{
	VAR * vp, * prev;

	for (vp = varp, prev = NULL ; vp != NULL ; vp = (prev = vp)->v_next)
		if (vareq (cp, vp->v_strp)) {
			if (prev == NULL)
				varp = vp->v_next;
			else
				prev->v_next = vp->v_next;
			sfree (vp->v_strp);
			sfree (vp);
			return 0;
		}
	return -1;
}


/*
 * Given a variable pointer, return a pointer to a string
 * containing the value.
 */

#if	USE_PROTO
CONST char * convvar (CONST VAR * vp)
#else
CONST char *
convvar (vp)
CONST VAR     *	vp;
#endif
{
	char * cp;
	char c;

	cp = vp->v_strp;
	while ((c = * cp ++) != 0)
		if (c == '=')
			break;
	return cp;
}


/*
 * Assign a value to the given shell variable.
 */

#if	USE_PROTO
VAR * assnvar (size_t len, CONST char * name, CONST char * val)
#else
VAR *
assnvar (len, name, val)
size_t		len;
CONST char    *	name;
CONST char    *	val;
#endif
{
	lex_init ();
	while (len --)
		lex_add (* name ++);
	lex_add ('=');
	lex_add_string (val);
	lex_add (0);
	return setsvar (lex_done ());
}


/*
 * For tellvar (), print a variable and value such that it can be read back
 * by the shell. We print the value single-quoted because values often
 * contain spaces and this makes the result easier for humans to read.
 */

#if	USE_PROTO
LOCAL void print_quoted (CONST char * str)
#else
LOCAL void
print_quoted (str)
CONST char    *	str;
#endif
{
	char		ch;
	CONST char    *	tmp = str;

	/*
	 * If there are no characters needing quoting, don't quote!
	 */

	do {
		if ((ch = * tmp ++) == 0) {
			cmd_outstr (str);
			return;
		}
	} while (class (ch, MNQUO) == 0);

	while ((ch = * str ++) != '=') {
		if (class (ch, MNQUO))
			cmd_outchar ('\\');
		cmd_outchar (ch);
	}

	cmd_outstr ("='");

	while ((ch = * str ++) != 0) {
		if (ch == '\'' || ch == '\\')
			cmd_outchar ('\\');
		cmd_outchar (ch);
	}

	cmd_outchar ('\'');
}


/*
 * Report function for variables.
 */

void
tellvar (f, prompt)
int		f;
int		prompt;
{
	VAR * vp;
	char ** evp;

	for (vp = varp ; vp ; vp = vp->v_next) {
		if (f != 0 && (vp->v_flag & f) == 0)
			continue;

		if (! prompt) {
			cmd_outstr (vp->v_strp);
			cmd_endl ();

			continue;
		}

		if (f != 0)
			cmd_outstr (f == VAR_EXPORT ? "export " : "readonly ");
		print_quoted (vp->v_strp);
		cmd_endl ();
	}
}


/*
 * Scan the varlist for exportables.
 *  If an exportable matches a variable already in envp, forget it.
 */

char **
envlvar (envp)
char ** envp;
{
	char ** enpp;
	char ** oenvp;
	VAR * vp;

	for (vp = varp ; vp ; vp = vp->v_next) {
		if ((vp->v_flag & VAR_EXPORT) != 0) {
			enpp = envp;
			while (* enpp != NULL && ! vareq (* enpp, vp->v_strp))
				enpp ++;
			if (* enpp == NULL)
				envp = addargl (envp, vp->v_strp, 0);
		}
	}
	return envp;
}

