/*
 * Expansion - quoting, separation, substitution, globbing
 */

static char *RCSid = "$Header: /newbits/usr/bin/korn/RCS/eval.c,v 1.2 91/08/01 12:39:55 bin Exp Locker: bin $";

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include "sh.h"
#include "lex.h"
#include "tree.h"
#include "table.h"
#include "expand.h"

#if COHERENT
struct	dirent	*readdir();		/* missing from headers */
DIR	*opendir();
#endif

/*
 * string expansion
 *
 * first pass: quoting, IFS separation, ${} and $() substitution.
 * second pass: filename expansion (*?[]~).
 */

/* expansion generator state */
typedef struct Expand {
	/* int  type; */	/* see expand() */
	char   *str;		/* string */
	union {
		char  **strv;	/* string[] */
		FILE   *file;	/* file */
	} u;			/* source */
	short	split;		/* split "$@"*/
} Expand;

#define	XBASE	0		/* scanning original */
#define	XSUB	1		/* expanding ${} string */
#define	XARGSEP	2		/* ifs0 between "$@" */
#define	XARG	3		/* expanding $*, $@ */
#define	XCOM	4		/* expanding $() */

static	void	expand ARGS((char *, XPtrV *, int));
static	int	comsub ARGS((Expand *, char *comm));
static	int	varsub ARGS((Expand *, char *name, int stype));
static	void	glob ARGS((char *cp, XPtrV *wp));
static	void	globit ARGS((char *ds, char *dp, char *sp, XPtrV *wp, int check));
static	char   *tilde ARGS((char *wp));
static	char   *trimsub ARGS((char *str, char *pat, int how));

int	ifs0 = ' ';		/* todo: first char of $IFS */

/* compile and expand word */
char *
substitute(cp, f)
	char Const *cp;
	int f;
{
	struct source *s, *sold;

	sold = source;
	s = pushs(SWSTR);
	s->str = (char *) cp;
	source = s;
	if (yylex(ONEWORD) != LWORD)
		errorf("eval:substitute error\n");
	source = sold;
	return evalstr(yylval.cp, f);
}

/*
 * expand arg-list
 */
char **
eval(ap, f)
	register char **ap;
{
	XPtrV w;

	if (*ap == NULL)
		return ap;
	XPinit(w, 32);
	XPput(w, NULL);		/* space for shell name */
	while (*ap != NULL)
		expand(*ap++, &w, f);
	XPput(w, NULL);
	return (char **) XPclose(w) + 1;
}

/*
 * expand string
 */
char *
evalstr(cp, f)
	register char *cp;
	int f;
{
	XPtrV w;

	XPinit(w, 1);
	expand(cp, &w, f);
	cp = (XPsize(w) == 0) ? "" : (char*) *XPptrv(w);
	XPfree(w);
	return cp;
}

/* for nested substitution: ${var:=$var2} */
typedef struct SubType {
	short	type;		/* [=+-?%#] action after expanded word */
	short	base;		/* begin position of expanded word */
	char   *name;		/* name for ${var=word} */
} SubType;

#if 1
mypr(cp)
char	*cp;
{
	fprintf(stderr, "%r", &cp);
}
#endif

static void
expand(cp, wp, f)
	char *cp;		/* input word */
	register XPtrV *wp;	/* output words */
	int f;			/* DO* flags */
{
	register int c;
	register int type = XBASE; /* expansion type */
	register int quote = 0;	/* quoted */
	XString ds;		/* destination string */
	register char *dp, *sp;	/* dest., source */
	int fdo, word, combase;	/* second pass flags; have word */
	Expand x;		/* expansion variables */
	SubType subtype [10];	/* substitution type stack */
	register SubType *st = subtype + 10;

	if (cp == NULL)
		errorf("eval:expand(NULL)\n");
	if (flag[FNOGLOB]) {
		f &= ~ DOGLOB;
	}
	Xinit(ds, dp, 128);	/* init dest. string */
	type = XBASE;
	sp = cp;
	fdo = 0;
	word = !(f&DOBLANK);

	while (1) {
		Xcheck(ds, dp);

		switch (type) {
		  case XBASE:	/* original prefixed string */
			c = *sp++;
			switch (c) {
			  case EOS:
				c = 0;
				break;
			  case CHAR:
				c = *sp++;
				break;
			  case QCHAR:
				quote |= 2; /* temporary quote */
				c = *sp++;
				break;
			  case OQUOTE:
				word = quote = 1;
				continue;
			  case CQUOTE:
				quote = 0;
				continue;
			  case COMSUB:
				type = comsub(&x, sp);
				sp = strchr(sp, 0) + 1;
				combase = Xsavepos(ds, dp);
				continue;
			  case OSUBST: /* ${var{:}[=+-?]word} */
				cp = sp; 		/* variable */
				sp = strchr(sp, 0) + 1;	/* skip variable */
				c = (*sp == CSUBST) ? 0 : *sp++;
				if ((c&0x7F) == '#' || (c&0x7F) == '%')
					f |= DOPAT;
				type = varsub(&x, cp, c);
				if (type == XBASE) {	/* expand? */
					if (st == subtype)
						errorf("ridiculous ${} nesting\n");
					--st;
					st->type = c;
					st->base = Xsavepos(ds, dp);
					st->name = cp;
				} else
					/* todo: nested OSUBST/CSUBST */
					sp = wdscan(sp, CSUBST); /* skip word */
				continue;
			  case CSUBST: /* only get here if expanding word */
				*dp = 0;
				if (f&DOGLOB)
					f &= ~DOPAT;
				switch (st->type&0x7F) {
				  case '#':
				  case '%':
					*dp = 0;
					dp = Xrestpos(ds, dp, st->base);
					x.str = trimsub(x.str, dp, st->type);
					type = XSUB;
					continue;
				  case '=':
#if 0
					if ((x.u.vp->flag&RDONLY))
						errorf("cannot set readonly %s\n", cp);
#endif
					setstr(global(st->name), Xrestpos(ds, dp, st->base));
					break;
				  case '?':
					if (dp == Xrestpos(ds, dp, st->base))
						errorf("missing value for %s\n", cp);
					else
						errorf("%s\n", Xrestpos(ds, dp, st->base));
				}
				st++;
				type = XBASE;
				continue;
			}
			break;

		  case XSUB:
			if ((c = *x.str++) == 0) {
				type = XBASE;
				continue;
			}
			break;

		  case XARGSEP:
			type = XARG;
			quote = 1;
		  case XARG:
			if ((c = *x.str++) == 0) {
				if ((x.str = *x.u.strv++) == NULL) {
					type = XBASE;
					continue;
				} else if (quote && x.split) {
					/* terminate word for "$@" */
					type = XARGSEP;
					quote = 0;
				}
				c = ifs0;
			}
			break;

		  case XCOM:
			c = getc(x.u.file);
			if (quote) {
				if (c == EOF) {
					cp = Xrestpos(ds, sp, combase);
					for (dp--; dp >= cp && *dp == '\n'; dp--)
						;
					dp++;
					fclose(x.u.file);
					if (x.split)
						waitlast();
					type = XBASE;
					continue;
				}
			} else {	/* this part is probably redundant */
				if (c == EOF || c == '\n') {
					while ((c = getc(x.u.file)) == '\n')
						;
					if (c == EOF) {
						fclose(x.u.file);
						if (x.split)
							waitlast();
						type = XBASE;
						continue;
					}
					ungetc(c, x.u.file);
					c = ifs0;
				}
			}
			break;
		}

		/* check for end of word or IFS separation */
		if (c == 0 || !quote && (f&DOBLANK) && ctype(c, C_IFS)) {
			if (word) {
				*dp++ = 0;
				cp = Xclose(ds, dp);
				if (fdo&DOTILDE)
					cp = tilde(cp);
				if (fdo&DOGLOB)
					glob(cp, wp);
				else
					{XPput(*wp, cp);}
				fdo = word = 0;
				if (c != 0)
					Xinit(ds, dp, 128);
			} else
				; /* ignore IFS */
			if (c == 0)
				return;
		} else {
			/* mark any special second pass chars */
			if (!quote)
				switch (c) {
				  case '*':
				  case '?':
				  case '[':
					if (f&(DOPAT|DOGLOB)) {
						fdo |= (f&DOGLOB);
						*dp++ = MAGIC;
					}
					break;
				  case '~':
					if ((f&DOTILDE) && dp == Xstring(ds, dp) ||
					    !(f&DOBLANK) && 
					    (dp[-1] == '=' || dp[-1] == ':')) {
						fdo |= DOTILDE;
						*dp++ = MAGIC;
					}
					break;
				}
			else
				quote &= ~2; /* undo temporary */

			word = 1;
			*dp++ = c; /* save output char */
		}
	}
}

/*
 * Prepare to generate the string returned by ${} substitution.
 */
static int
varsub(xp, sp, stype)
	register Expand *xp;
	register char *sp;
	int stype;
{
	register int c;
	int type;

	/* ${#var}, string length or argc */
	if (sp[0] == '#' && (c = sp[1]) != 0) {
		c = (c == '*' || c == '@') ? e.loc->argc :
			strlen(strval(global(sp+1)));
		xp->str = strsave(ulton((unsigned long)c, 10), ATEMP);
		return XSUB;
	}

	c = sp[0];
	if (c == '*' || c == '@') {
		if (e.loc->argc == 0) {
			xp->str = null;
			type = XSUB;
		} else {
			xp->u.strv = e.loc->argv + 1;
			xp->str = *xp->u.strv++;
			xp->split = c == '@'; /* $@ */
			type = XARG;
		}
	} else {
		xp->str = strval(global(sp));
		type = XSUB;
	}

	c = stype&0x7F;
	/* test the compiler's code generator */
	if (c == '%' || c == '#' ||
	    (((stype&0x80) ? *xp->str=='\0' : xp->str==null) ? /* undef? */
	     c == '=' || c == '-' || c == '?' : c == '+'))
		type = XBASE;	/* expand word instead of variable value */
	if (type != XBASE && flag[FNOUNSET] && xp->str == null)
		errorf("%s: unset variable\n", sp);
	return type;
}

/*
 * Run the command in $(...) and read its output.
 */
static int
comsub(xp, cp)
	register Expand *xp;
	char *cp;
{
	Source *s;
	register struct op *t;
	FILE *fi;

	s = pushs(SSTRING);
	s->str = cp;
	t = compile(s);

	if (t != NULL && t->type == TCOM && /* $(<file) */
	    *t->args == NULL && *t->vars == NULL && t->ioact != NULL) {
		register struct ioword *io = *t->ioact;

		if (io->flag != IOREAD)
			errorf("funny $() command\n");
		fi = fopen(evalstr(io->name, DOTILDE), "r");
		if (fi != NULL)
			fileno(fi) = savefd(fileno(fi));
		xp->split = 0;	/* no waitlast() */
	} else {
		int ofd1, pv[2];

		openpipe(pv);
		fi = fdopen(pv[0], "r");
		ofd1 = savefd(1);
		dup2(pv[1], 1);
		close(pv[1]);
#if 0
		exchild(t, XXCOM|XPIPEO);
#else
		execute(t, XFORK|XXCOM|XPIPEO);
#endif
		dup2(ofd1, 1);
#if COHERENT
		close(ofd1);
#endif
		xp->split = 1;	/* waitlast() */
	}	

	if (fi == NULL)
		errorf("cannot open $() input\n");
	setvbuf(fi, (char *)NULL, _IOFBF, BUFSIZ);
	xp->u.file = fi;
	return XCOM;
}

/*
 * perform #pattern and %pattern substitution in ${}
 */

static char *
trimsub(str, pat, how)
	register char *str;
	char *pat;
	int how;
{
	register char *end = strchr(str, 0);
	register char *p, c;

	switch (how) {
	  case '#':		/* shortest at begin */
		for (p = str; p <= end; p++) {
			c = *p; *p = '\0';
			if (gmatch(str, pat)) {
				*p = c;
				return p;
			}
			*p = c;
		}
		break;
	case '#'|0x80:		/* longest match at begin */
		for (p = end; p >= str; p--) {
			c = *p; *p = '\0';
			if (gmatch(str, pat)) {
				*p = c;
				return p;
			}
			*p = c;
		}
		break;
	  case '%':		/* shortest match at end */
		for (p = end; p >= str; p--) {
			if (gmatch(p, pat)) {
				*p = '\0';
				return str;
			}
		}
		break;
	  case '%'|0x80:	/* longest match at end */
		for (p = str; p <= end; p++) {
			if (gmatch(p, pat)) {
				*p = '\0';
				return str;
			}
		}
		break;
	}

	return str;		/* no match, return string */
}

/*
 * glob
 * Name derived from V6's /etc/glob, the program that expanded filenames.
 */

static	char   *debunk();

static void 
glob(cp, wp)
	char *cp;
	register XPtrV *wp;
{
	char path [PATH];
	register char *sp = cp;
	int oldsize;

	oldsize = XPsize(*wp);
	globit(path, path, sp, wp, 0);

	if (XPsize(*wp) == oldsize)
		{XPput(*wp, debunk(cp));}
	else
		qsortp(XPptrv(*wp) + oldsize, (size_t)(XPsize(*wp) - oldsize), xstrcmp);
}

static void
globit(ds, dp, sp, wp, check)
	char *ds;		/* dest path */
	char *dp;		/* dest end */
	char *sp;		/* source path */
	register XPtrV *wp;	/* output list */
	int check;		/* check dest existence */
{
	register char *np;	/* next source component */
	register char *tsp, *tdp;

	if (sp == NULL) {	/* end of source path */
		if (check && access(ds, 0) < 0)
			return;
		XPput(*wp, strsave(ds, ATEMP));
		return;
	}

	if (dp > ds)
		*dp++ = '/';
	while (*sp == '/')
		*dp++ = *sp++;
	np = strchr(sp, '/');
	if (np != NULL)
		*np++ = 0;

	*dp = 0;
	if (strchr(sp, MAGIC) == NULL) { /* contains no pattern? */
		tdp = dp; tsp = sp;
		while ((*tdp++ = *tsp++) != 0)
			;
		--tdp;
		globit(ds, tdp, np, wp, check);
	} else {
		DIR *dirp;
		struct dirent *d;

		dirp = opendir(ds);
		if (dirp == NULL)
			goto Nodir;
		while ((d = readdir(dirp)) != NULL) {
			tsp = d->d_name;
			if (tsp[0] == '.' &&
			    (tsp[1] == 0 || tsp[1] == '.' && tsp[2] == 0))
				continue; /* always ignore . and .. */
			if (*tsp == '.' && *sp != '.' || !gmatch(tsp, sp))
				continue;

			tdp = dp;
			while ((*tdp++ = *tsp++) != 0)
				;
			--tdp;
			globit(ds, tdp, np, wp, np != NULL);
		}
		closedir(dirp);
	  Nodir:;
	}

	if (np != NULL)
		*--np = '/';
}

/* remove MAGIC from string */
static char *
debunk(cp)
	char *cp;
{
	register unsigned char *dp, *sp;

	for (dp = sp = cp; *sp != 0; sp++)
		if (*sp != MAGIC)
			*dp++ = *sp;
	*dp = 0;
	return cp;
}

/*
 * tilde expansion
 *
 * based on a version by Arnold Robbins
 */

static char *homedir();

static char *
tilde(acp)
	char *acp;
{
	register unsigned int c;
	char path[PATH+1];
	register unsigned char *cp = acp, *wp = path, *dp;
	char userid[16+1];

  Again:
	while (1) {
		if ((c = *cp++) == 0) {
			*wp = 0;
			afree((Void*)acp, ATEMP);
			return strsave(path, ATEMP);
		} else if (c == MAGIC && *cp == '~')
			break;
		else
			*wp++ = c;
	}

	dp = NULL;	/* no output substitution */
	if (cp[1] == 0 || cp[1] == '/' || cp[1] == ':') /* ~ or ~/ */
		dp = strval(global("HOME")), cp += 1;
	else if (cp[1] == '+' && (cp[2] == '/' || cp[2] == ':' || cp[2] == 0))
		dp = strval(global("PWD")), cp += 2;
	else if (cp[1] == '-' && (cp[2] == '/' || cp[2] == ':' || cp[2] == 0))
		dp = strval(global("OLDPWD")), cp += 2;
	else if (letter(cp[1])) {
		char *save = cp;
		for (dp = userid, cp++; letnum(*cp) && dp < userid+16; )
			*dp++ = *cp++;
		*dp = 0;
		dp = homedir(userid);
		if (dp == NULL)
			cp = save;
	}
	/* substitute */
	if (dp != NULL)
		while (*dp != 0)
			*wp++ = *dp++;
	goto Again;
}

/*
 * map userid to user's home directory.
 * todo: implement a cache with the "homedirs" table.
 * note that 4.3's getpw adds more than 6K to the shell,
 * and the YP version probably adds much more.
 * we might consider our own version of getpwnam() to keep the size down.
 */

static char *
homedir(name)
	char *name;
{
	register struct tbl *ap;
	register struct passwd *pw;

	ap = tsearch(&homedirs, name, hash(name));
	if ((ap != NULL && (ap->flag&ISSET)))
		return ap->val.s;
	pw = getpwnam(name);
	if (pw == NULL)
		return NULL;
	return pw->pw_dir;
}

