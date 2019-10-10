static char *RCSid = "$Header: var.c,v 3.1 88/11/03 09:18:22 egisin Exp $";

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <time.h>
#include <sys/types.h>
#include "sh.h"
#include "table.h"
#include "expand.h"

/*
 * Variables
 *
 * WARNING: unreadable code, needs a rewrite
 *
 * if (flag&INTEGER), val.i contains integer value, and type contains base.
 * otherwise, (val.s + type) contains string value.
 * if (flag&EXPORT), val.s contains "name=value" for E-Z exporting.
 */
char	null []	= "";
static	struct tbl vtemp;
static	void getspec(), setspec();
static	void export ARGS((struct tbl *, char *val));
static	int special ARGS ((char *name));

/*
 * create a new block for function calls and simple commands
 * assume caller has allocated and set up e.loc
 */
void
newblock()
{
	register struct block *l = e.loc;
	static char *empty[] = {""};

	ainit(&l->area);
	l->argc = 0;
	l->argv = empty;
	l->exit = l->error = NULL;
	tinit(&l->vars, &l->area);
	tinit(&l->funs, &l->area);
}

/*
 * pop a block handling special variables
 */
void
popblock()
{
	register struct block *l = e.loc;
	register struct tbl *vp, **vpp = l->vars.tbls;
	register int i;

	e.loc = l->next;	/* pop block */
	for (i = l->vars.size; --i >= 0; )
		if ((vp = *vpp++) != NULL && (vp->flag&SPECIAL))
			setspec(global(vp->name));
	afreeall(&l->area);
}

/*
 * Search for variable, if not found create globally.
 */
struct tbl *
global(n)
	register char *n;
{
	register struct block *l = e.loc;
	register struct tbl *vp;
	register int c;
	unsigned h = hash(n);

	c = n[0];
	if (digit(c)) {
		vp = &vtemp;
		lastarea = ATEMP;
		vp->flag = (DEFINED|RDONLY);
		vp->type = 0;
		*vp->name = c;	/* should strncpy */
		for (c = 0; digit(*n) && c < 1000; n++)
			c = c*10 + *n-'0';
		if (c <= l->argc)
			setstr(vp, l->argv[c]);
		return vp;
	} else
	if (!letter(c)) {
		vp = &vtemp;
		lastarea = ATEMP;
		vp->flag = (DEFINED|RDONLY);
		vp->type = 0;
		*vp->name = c;
		if (n[1] != '\0')
			return vp;
		vp->flag |= ISSET|INTEGER;
		switch (c) {
		  case '$':
			vp->val.i = getpid();
			break;
		  case '!':
			vp->val.i = async;
			break;
		  case '?':
			vp->val.i = exstat;
			break;
		  case '#':
			vp->val.i = l->argc;
			break;
		  case '-':
			vp->flag &= ~ INTEGER;
			vp->val.s = getoptions();
			break;
		  default:
			vp->flag &= ~(ISSET|INTEGER);
		}
		return vp;
	}
	for (l = e.loc; l != NULL; l = l->next) {
		vp = tsearch(&l->vars, n, h);
		lastarea = &l->area;
		if (vp != NULL)
			return vp;
		if (l->next == NULL)
			break;
	}
	vp = tenter(&l->vars, n, h);
	vp->flag |= DEFINED;
	if (special(n))
		vp->flag |= SPECIAL;
	return vp;
}

/*
 * Search for local variable, if not found create locally.
 */
struct tbl *
local(n)
	register char *n;
{
	register struct block *l = e.loc;
	register struct tbl *vp;
	unsigned h = hash(n);

	if (!letter(*n)) {
		vp = &vtemp;
		lastarea = ATEMP;
		vp->flag = (DEFINED|RDONLY);
		vp->type = 0;
		return vp;
	}
	vp = tenter(&l->vars, n, h);
	lastarea = &l->area;
	vp->flag |= DEFINED;
	if (special(n))
		vp->flag |= SPECIAL;
	return vp;
}

/* get variable string value */
char *
strval(vp)
	register struct tbl *vp;
{
	register char *s;
	static char strbuf[40];

	if ((vp->flag&SPECIAL))
		getspec(vp);
	if (!(vp->flag&ISSET))
		return null;	/* special to dollar() */
	if (!(vp->flag&INTEGER))	/* string source */
		s = vp->val.s + vp->type;
	else {				/* integer source */
		register unsigned long n;
		register int base;

		s = strbuf + sizeof(strbuf);
		n = (vp->val.i < 0) ? -vp->val.i : vp->val.i;
		base = (vp->type == 0) ? 10 : vp->type;

		*--s = '\0';
		do {
			*--s = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[n%base];
			n /= base;
		} while (n != 0);
		/* todo: should we output base# ? */
		if (vp->val.i < 0)
			*--s = '-';
	}
	return s;
}

/* get variable integer value */
long
intval(vp)
	register struct tbl *vp;
{
	register struct tbl *vq;

	if ((vp->flag&SPECIAL))
		getspec(vp);
	if ((vp->flag&INTEGER))
		return vp->val.i;
	vq = &vtemp;
	vq->flag = (INTEGER);
	vq->type = 0;
	strint(vq, vp);
	return vq->val.i;
}

/* set variable to string value */
void
setstr(vq, s)
	register struct tbl *vq;
	char *s;
{
	if (!(vq->flag&INTEGER)) { /* string dest */
		if ((vq->flag&ALLOC))
			afree((Void*)vq->val.s, lastarea);
		vq->flag &= ~ (ISSET|ALLOC);
		vq->type = 0;
		if ((vq->flag&EXPORT))
			export(vq, s);
		else
			vq->val.s = strsave(s, lastarea);
		vq->flag |= ALLOC;
	} else {		/* integer dest */
		register struct tbl *vp = &vtemp;	
		vp->flag = (DEFINED|ISSET);
		vp->type = 0;
		vp->val.s = s;
		strint(vq, vp);
	}
	vq->flag |= ISSET;
	if ((vq->flag&SPECIAL))
		setspec(vq);
}
	
/* convert variable to integer variable */
struct tbl *
strint(vq, vp)
	register struct tbl *vq, *vp;
{
	register char *s = vp->val.s + vp->type;
	register int c;
	int base, neg = 0;
	
	vq->flag |= INTEGER;
	if ((vp->flag&INTEGER)) {
		vq->val.i = vp->val.i;
		return vq;
	}
	vq->val.i = 0;
	base = 10;
	for (c = *s++; c ; c = *s++)
		if (c == '-') {
			neg++;
		} else if (c == '#') {
			base = vq->type = vq->val.i;
			vq->val.i = 0;
		} else if (letnum(c)) {
			if ('0' <= c && c <= '9')
				c -= '0';
			else if ('a' <= c && c <= 'z') /* fuck EBCDIC */
				c -= 'a'-10;
			else if ('A' <= c && c <= 'Z')
				c -= 'A'-10;
			vq->val.i = (vq->val.i*base) + c;
		} else
			break;
	if (neg)
		vq->val.i = -vq->val.i;
	if (vq->type < 2 || vq->type > 36)
		vq->type = 0;	/* default base (10) */
	return vq;
}

/* set variable to integer */
void
setint(vq, n)
	register struct tbl *vq;
	long n;
{
	if (!(vq->flag&INTEGER)) {
		register struct tbl *vp = &vtemp;
		vp->flag = (ISSET|INTEGER);
		vp->type = 0;
		vp->val.i = n;
		setstr(vq, strval(vp));	/* ? */
	} else
		vq->val.i = n;
	vq->flag |= ISSET;
	if ((vq->flag&SPECIAL))
		setspec(vq);
}

/* set variable from enviroment */
import(thing)
	char *thing;
{
	register struct tbl *vp;
	register char *val;

	val = strchr(thing, '=');
	if (val == NULL)
		return 0;
	*val = '\0';
	vp = local(thing);
	*val++ = '=';
	vp->flag |= DEFINED|ISSET|EXPORT;
	vp->val.s = thing;
	vp->type = val - thing;
	if ((vp->flag&SPECIAL))
		setspec(vp);
	return 1;
}

/*
 * make vp->val.s be "name=value" for quick exporting.
 */
static void
export(vp, val)
	register struct tbl *vp;
	char *val;
{
	register char *cp, *xp;
	char *op = (vp->flag&ALLOC) ? vp->val.s : NULL;

	xp = (char*)alloc(strlen(vp->name) + strlen(val) + 2, lastarea);
	vp->flag |= ALLOC;
	vp->val.s = xp;
	for (cp = vp->name; (*xp = *cp++) != '\0'; xp++)
		;
	*xp++ = '=';
	vp->type = xp - vp->val.s; /* offset to value */
	for (cp = val; (*xp++ = *cp++) != '\0'; )
		;
	if (op != NULL)
		afree((Void*)op, lastarea);
}

/*
 * lookup variable (according to (set&LOCAL)),
 * set its attributes (INTEGER, RDONLY, EXPORT, TRACE),
 * and optionally set its value if an assignment.
 */
struct tbl *
typeset(var, set, clr)
	register char *var;
	int clr, set;
{
	register struct tbl *vp;
	register char *val;

	/* check for valid variable name, search for value */
	val = var;
	if (!letter(*val))
		return NULL;
	for (val++; *val != '\0'; val++)
		if (*val == '=')
			break;
		else if (letnum(*val))
			;
		else
			return NULL;
	if (*val == '=')
		*val = '\0';
	else
		val = NULL;
	vp = (set&LOCAL) ? local(var) : global(var);
	set &= ~ LOCAL;
	if (val != NULL)
		*val++ = '=';

	if (!(vp->flag&ISSET))
		vp->flag = vp->flag & ~clr | set;
	else
	    if (!(vp->flag&INTEGER) && (set&INTEGER)) {
		/* string to integer */
		vtemp.flag = (ISSET);
		vtemp.type = 0;
		vtemp.val.s = vp->val.s + vp->type;
		if ((vp->flag&ALLOC))
			afree((Void*)vp->val.s, lastarea); /* dangerous, used later */
		vp->flag &= ~ ALLOC;
		vp->flag |= INTEGER;
		vp->type = 0;
		strint(vp, &vtemp);
	    } else
	    if ((clr&INTEGER) && (vp->flag&INTEGER)) {
		/* integer to string */
		vtemp.val.s = strval(vp);
		vp->flag &= ~ INTEGER;
		setstr(vp, vtemp.val.s);
	    }

	vp->flag = vp->flag & ~clr | set;

	if (val != NULL) {
		if ((vp->flag&RDONLY))
			errorf("cannot set readonly %s\n", var);
		if ((vp->flag&INTEGER))
			/* setstr should be able to handle this */
			(void)evaluate(var);
		else
			setstr(vp, val);
	}

	if ((vp->flag&EXPORT) && !(vp->flag&INTEGER) && vp->type == 0)
		export(vp, (vp->flag&ISSET) ? vp->val.s : null);

	return vp;
}

void
unset(vp)
	register struct tbl *vp;
{
	if ((vp->flag&ALLOC))
		afree((Void*)vp->val.s, lastarea);
	vp->flag &= ~ (ALLOC|ISSET);
}

int
isassign(s)
	register char *s;
{
	if (!letter(*s))
		return (0);
	for (s++; *s != '='; s++)
		if (*s == 0 || !letnum(*s))
			return (0);
	return (1);
}

/*
 * Make the exported environment from the exported names in the dictionary.
 */
char **
makenv()
{
	struct block *l = e.loc;
	XPtrV env;
	register struct tbl *vp, **vpp;
	register int i;

	XPinit(env, 64);
	for (l = e.loc; l != NULL; l = l->next)
		for (vpp = l->vars.tbls, i = l->vars.size; --i >= 0; )
			if ((vp = *vpp++) != NULL
			    && (vp->flag&(ISSET|EXPORT)) == (ISSET|EXPORT)) {
				register struct block *l2;
				register struct tbl *vp2;
				unsigned h = hash(vp->name);

				lastarea = &l->area;

				/* unexport any redefined instances */
				for (l2 = l->next; l2 != NULL; l2 = l2->next) {
					vp2 = tsearch(&l2->vars, vp->name, h);
					if (vp2 != NULL)
						vp2->flag &= ~ EXPORT;
				}
				if ((vp->flag&INTEGER)) {
					/* integer to string */
					char *val;
					val = strval(vp);
					vp->flag &= ~ INTEGER;
					setstr(vp, val);
				}
				XPput(env, vp->val.s);
			}
	XPput(env, NULL);
	return (char **) XPclose(env);
}

/*
 * handle special variables with side effects - PATH, SECONDS.
 */

static int
special(name)
	register char * name;
{
	if (strcmp("PATH", name) == 0)
		return V_PATH;
	if (strcmp("IFS", name) == 0)
		return V_IFS;
	if (strcmp("SECONDS", name) == 0)
		return V_SECONDS;
	if (strcmp("OPTIND", name) == 0)
		return V_OPTIND;
	return V_NONE;
}

extern	time_t time();
static	time_t	seconds;		/* time SECONDS last set */

static void
getspec(vp)
	register struct tbl *vp;
{
	switch (special(vp->name)) {
	case V_SECONDS:
		vp->flag &= ~ SPECIAL;
		setint(vp, time((time_t *)0) - seconds);
		vp->flag |= SPECIAL;
		break;
	}
}

static void
setspec(vp)
	register struct tbl *vp;
{
	switch (special(vp->name)) {
	  case V_PATH:
		path = strval(vp);
		flushcom(1);	/* clear tracked aliases */
		break;
	  case V_IFS:
		setctypes(strval(vp), C_IFS);
		break;
	  case V_SECONDS:
		seconds = time((time_t *)0);
		break;
	  case V_OPTIND:
		if (intval(vp) == 1)
			resetopts();
		break;
	}
}

