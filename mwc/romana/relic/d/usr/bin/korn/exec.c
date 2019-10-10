/*
 * execute command tree
 */

static char *RCSid = "$Header: /newbits/usr/bin/korn/RCS/exec.c,v 1.2 91/08/01 12:40:11 bin Exp Locker: bin $";

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include "sh.h"
#include "lex.h"
#include "tree.h"
#include "table.h"

static	int	comexec ARGS((struct op *t, char **vp, char **ap, int flags));
static	void	iosetup ARGS((struct ioword *iop));
static	void	echo ARGS((char **, char **));
static	int	herein ARGS((char *name, int sub));

/*
 * execute command tree
 */
int
execute(t, flags)
	register struct op *t;
	Volatile int flags;	/* if XEXEC don't fork */
{
	int i;
	int Volatile rv = 0;
	int pv[2];
	register char **ap;
	char *s, *cp;
	struct ioword **iowp;

	if (t == NULL)
		return 0;

	if ((flags&XFORK) && !(flags&XEXEC) && t->type != TPIPE)
		return exchild(t, flags); /* run in sub-process */

	newenv(E_EXEC);
	if (trap)
		runtraps();
 
	if (t->ioact != NULL || t->type == TPIPE) {
		e.savefd = alloc(sizeofN(short, NUFILE), ATEMP);
		for (i = 0; i < NUFILE; i++)
			e.savefd[i] = 0; /* not redirected */
	}

	/* do redirection, to be restored in quitenv() */
	if (t->ioact != NULL)
		for (iowp = t->ioact; *iowp != NULL; iowp++) {
			if ((flags&XPIPEI) && (*iowp)->unit == 0 ||
			    (flags&XPIPEO) && (*iowp)->unit == 1)
				errorf("attempt to redirect fd 0/1 in pipe\n");
			iosetup(*iowp);
		}

	switch(t->type) {
	  case TCOM:
		e.type = E_TCOM;
		rv = comexec(t, eval(t->vars, DOTILDE),
			     eval(t->args, DOBLANK|DOGLOB|DOTILDE), flags);
		break;

	  case TPAREN:
		rv = execute(t->left, flags|XFORK);
		break;

	  case TPIPE:
		flags |= XFORK;
		e.savefd[0] = savefd(0);
		e.savefd[1] = savefd(1);
		flags |= XPIPEO;
		(void) dup2(e.savefd[0], 0); /* stdin of first */
		while (t->type == TPIPE) {
			openpipe(pv);
			(void) dup2(pv[1], 1);	/* stdout of curr */
			exchild(t->left, flags);
			(void) dup2(pv[0], 0);	/* stdin of next */
			closepipe(pv);
			flags |= XPIPEI;
			t = t->right;
		}
		flags &= ~ XPIPEO;
		(void) dup2(e.savefd[1], 1); /* stdout of last */
		exchild(t, flags);
		(void) dup2(e.savefd[0], 0); /* close pipe in */
		/*
		 * added background check to avoid waiting on unwanted pipelines
		 */
		if (!(flags & XBGND))
			rv = waitlast();
		break;

	  case TLIST:
		while (t->type == TLIST) {
			execute(t->left, 0);
			t = t->right;
		}
		rv = execute(t, 0);
		break;

	  case TASYNC:
		rv = execute(t->left, flags|XBGND|XFORK);
		break;

	  case TOR:
	  case TAND:
		rv = execute(t->left, 0);
		if (t->right != NULL && (rv == 0) == (t->type == TAND))
			rv = execute(t->right, 0);
		break;

	  case TFOR:
		e.type = E_LOOP;
		ap = (t->vars != NULL) ?
			eval(t->vars, DOBLANK|DOGLOB|DOTILDE) : e.loc->argv + 1;
		while ((i = setjmp(e.jbuf)))
			if (i == LBREAK)
				goto Break1;
		while (*ap != NULL) {
			setstr(global(t->str), *ap++);
			rv = execute(t->left, 0);
		}
	  Break1:
		break;

	  case TWHILE:
	  case TUNTIL:
		e.type = E_LOOP;
		while ((i = setjmp(e.jbuf)))
			if (i == LBREAK)
				goto Break2;
		while ((execute(t->left, 0) == 0) == (t->type == TWHILE))
			rv = execute(t->right, 0);
	  Break2:
		break;

	  case TIF:
	  case TELIF:
		if (t->right == NULL)
			break;	/* should be error */
		rv = execute(t->left, 0) == 0 ?
			execute(t->right->left, 0) :
			execute(t->right->right, 0);
		break;

	  case TCASE:
		cp = evalstr(t->str, 0);
		for (t = t->left; t != NULL && t->type == TPAT; t = t->right)
		    for (ap = t->vars; *ap; ap++)
			if ((s = evalstr(*ap, DOPAT)) && gmatch(cp, s))
				goto Found;
		break;
	  Found:
		rv = execute(t->left, 0);
		break;

	  case TBRACE:
		rv = execute(t->left, 0);
		break;

	  case TFUNCT:
		rv = define(t->str, t->left);
		break;

	  case TTIME:
		rv = timex(t, flags);
		break;

	  case TEXEC:		/* an eval'd TCOM */
		s = t->args[0];
		ap = makenv();
#if _MINIX || COHERENT			/* no F_SETFD close-on-exec */
		for (i = 10; i < 20; i++)
			close(i);
#endif
		execve(t->str, t->args, ap);
		if (errno == ENOEXEC) {
			*t->args-- = t->str;
			*t->args = s;
			execve(SHELL, t->args, ap);
			errorf("No shell\n");
		}
		errorf("%s: %s\n", s, strerror(errno));
	}

	quitenv();		/* restores IO */
	if (e.interactive) {	/* flush stdout, shlout */
		fflush(shf[1]);
		fflush(shf[2]);
	}
	if ((flags&XEXEC))
		exit(rv);	/* exit child */
	return rv;
}

/*
 * execute simple command
 */

static int
comexec(t, vp, ap, flags)
	struct op *t;
	register char **ap, **vp;
	int flags;
{
	int i;
	int rv = 0;
	register char *cp;
	register struct tbl *tp = NULL;
	register struct block *l;
	static struct op texec = {TEXEC};
	extern int c_exec(), c_builtin();

	if (flag[FXTRACE])
		echo(vp, ap);

	/* create new variable/function block */
	l = alloc(sizeof(struct block), ATEMP);
	l->next = e.loc; e.loc = l;
	newblock();

 Doexec:
	if ((cp = *ap) == NULL)
		cp = ":";
	tp = findcom(cp, 1);

	switch (tp->type) {
	  case CSHELL:			/* shell built-in */
		while (tp->val.f == c_builtin) {
			if ((cp = *++ap) == NULL)
				break;
			tp = tsearch(&builtins, cp, hash(cp));
			if (tp == NULL)
				errorf("%s: not builtin\n", cp);
		}
		if (tp->val.f == c_exec) {
			if (*++ap == NULL) {
				e.savefd = NULL; /* don't restore redirection */
				break;
			}
			flags |= XEXEC;
			goto Doexec;
		}
		if ((tp->flag&TRACE))
			e.loc = l->next; /* no local block */
		i = (tp->flag&TRACE) ? 0 : LOCAL;
		while (*vp != NULL)
			(void) typeset(*vp++, i, 0);
		rv = (*tp->val.f)(ap);
		break;

	case CFUNC:			/* function call */
		if (!(tp->flag&ISSET))
			errorf("%s: undefined function", cp);
		l->argv = ap;
		for (i = 0; *ap++ != NULL; i++)
			;
		l->argc = i - 1;
		resetopts();
		while (*vp != NULL)
			(void) typeset(*vp++, LOCAL, 0);
		e.type = E_FUNC;
		if (setjmp(e.jbuf))
			rv = exstat; /* return # */
		else
			rv = execute(tp->val.t, 0);
		break;

	case CEXEC:		/* executable command */
		if (!(tp->flag&ISSET)) {
			shellf("%s: not found\n", cp);
			rv = 1;
			break;
		}

		/* set $_ to program's full path */
		setstr(typeset("_", LOCAL|EXPORT, 0), tp->val.s);
		while (*vp != NULL)
			(void) typeset(*vp++, LOCAL|EXPORT, 0);

		if ((flags&XEXEC)) {
			j_exit();
#if !COHERENT
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
#endif
		}

		/* to fork we set up a TEXEC node and call execute */
		texec.left = t;	/* for tprint */
		texec.str = tp->val.s;
		texec.args = ap;
		rv = exchild(&texec, flags);
		break;
	}
	if (rv != 0 && flag[FERREXIT])
		leave(rv);
	return (exstat = rv);
}

int
shcomexec(wp)
	register char **wp;
{
	register struct tbl *tp;

	tp = tsearch(&builtins, *wp, hash(*wp));
	if (tp == NULL)
		errorf("%s: shcomexec botch\n", *wp);
	return (*tp->val.f)(wp);
}

/*
 * define function
 */
int
define(name, t)
	char	*name;
	struct op *t;
{
	register struct block *l;
	register struct tbl *tp;

	for (l = e.loc; l != NULL; l = l->next) {
		lastarea = &l->area;
		tp = tsearch(&l->funs, name, hash(name));
		if (tp != NULL && (tp->flag&DEFINED))
			break;
		if (l->next == NULL) {
			tp = tenter(&l->funs, name, hash(name));
			tp->flag = DEFINED|FUNCT;
			tp->type = CFUNC;
		}
	}

	if ((tp->flag&ALLOC))
		tfree(tp->val.t, lastarea);
	tp->flag &= ~(ISSET|ALLOC);

	if (t == NULL)		/* undefine */
		return 0;

	tp->val.t = tcopy(t, lastarea);
	tp->flag |= (ISSET|ALLOC);

	return 0;
}

/*
 * add builtin
 */
builtin(name, func)
	char *name;
	int (*func)();
{
	register struct tbl *tp;
	int flag = DEFINED;

	if (*name == '=') {		/* sets keyword variables */
		name++;
		flag |= TRACE;	/* command does variable assignment */
	}

	tp = tenter(&builtins, name, hash(name));
	tp->flag |= flag;
	tp->type = CSHELL;
	tp->val.f = func;
}

/*
 * find command
 * either function, hashed command, or built-in (in that order)
 */
struct tbl *
findcom(name, insert)
	char	*name;
	int	insert;			/* insert if not found */
{
	register struct block *l = e.loc;
	unsigned int h = hash(name);
	register struct	tbl *tp = NULL;
	static struct tbl temp;

	if (strchr(name, '/') != NULL) {
		tp = &temp;
		tp->type = CEXEC;
		tp->flag = 0;	/* make ~ISSET */
		goto Search;
	}
	for (l = e.loc; l != NULL; l = l->next) {
		tp = tsearch(&l->funs, name, h);
		if (tp != NULL && (tp->flag&DEFINED))
			break;
	}
	if (tp == NULL)
		tp = tsearch(&commands, name, h);
	if (tp == NULL)
		tp = tsearch(&builtins, name, h);
	if (tp == NULL && insert) {
		tp = tenter(&commands, name, h);
		tp->type = CEXEC;
		tp->flag = DEFINED;
	}
  Search:
	if (tp->type == CEXEC && !(tp->flag&ISSET)) {
		if (!flag[FHASHALL]) {
			tp = &temp;
			tp->type = CEXEC;
			tp->flag = 0;	/* make ~ISSET */
		}
		name = search(name, path, 1);
		if (name != NULL) {
			tp->val.s = strsave(name,
					    (tp == &temp) ? ATEMP : APERM);
			tp->flag |= ISSET|ALLOC;
		}
	}
	return tp;
}

/*
 * flush executable commands with relative paths
 */
flushcom(all)
	int all;		/* just relative or all */
{
	register struct tbl *tp;

	for (twalk(&commands); (tp = tnext()) != NULL; )
		if ((tp->flag&ISSET) && (all || tp->val.s[0] != '/')) {
			if ((tp->flag&ALLOC))
				afree(tp->val.s, commands.areap);
			tp->flag = DEFINED; /* make ~ISSET */
		}
}

/*
 * search for command with PATH
 */
char *
search(name, path, mode)
	char *name, *path;
	int mode;		/* 0: readable; 1: executable */
{
	register int i;
	register char *sp, *tp;
	int	colon = FALSE;

	if (strchr(name, '/'))
		return (eaccess(name, mode) == 0) ? name : NULL;

	sp = path;
	do {
		tp = line;
		colon = FALSE;
		for (; *sp != '\0'; tp++) {
			if ((*tp = *sp++) == ':') {
				colon = TRUE;
				break;
			}
		}
		if (tp != line)
			*tp++ = '/';
		for (i = 0; (*tp++ = name[i++]) != '\0';)
			;
		i = eaccess(line, mode);
		if (i == 0)
			return line;
		/* what should we do about EACCES? */
	} while (*sp != '\0' || colon);
	return NULL;
}

/*
 * set up redirection, saving old fd's in e.savefd
 */
static void
iosetup(iop)
	register struct ioword *iop;
{
	register int u = -1;
	char *cp, *msg;
	extern long lseek();

	if (iop->unit == 0 || iop->unit == 1 || iop->unit == 2)
		e.interactive = 0;
	e.savefd[iop->unit] = savefd(iop->unit);

	msg = iop->flag&(IOREAD|IOHERE)? "open": "create";
	cp = iop->name;
	if (!(iop->flag & IOHERE))
		cp = evalstr(cp, DOTILDE);
	if (iop->flag&IODUP)
		iop->flag &= ~(IOREAD|IOWRITE);	/* todo: lex.c */

	switch (iop->flag) {
	  case IOREAD:
		u = open(cp, 0);
		break;

	  case IOHERE:
	  case IOHERE|IOXHERE:
		u = herein(cp, iop->flag&IOXHERE);
		/* cp may have wrong name */
		break;

	  case IOWRITE|IOCAT:
		if ((u = open(cp, 1)) >= 0) {
			(void) lseek(u, (long)0, 2);
			break;
		}
		/* FALLTHROUGH */
	  case IOWRITE:
		u = creat(cp, 0666);
		break;

	  case IODUP:
		if (*cp == '-')
			close(iop->unit);
		else
		if (digit(*cp))
			u = *cp - '0';
		else
			errorf("%s: illegal >& argument\n", cp);
		break;
	}
	if (u < 0)
		errorf("%s: cannot %s\n", cp, msg);
	if (u != iop->unit) {
		(void) dup2(u, iop->unit);
		if (iop->flag != IODUP)
			close(u);
	}

	fopenshf(iop->unit);
}

/*
 * open here document temp file.
 * if unquoted here, expand here temp file into second temp file.
 */
static int
herein(hname, sub)
	char *hname;
	int sub;
{
	int fd;
	FILE * Volatile f = NULL;

	f = fopen(hname, "r");
	if (f == NULL)
		return -1;
	setvbuf(f, (char *)NULL, _IOFBF, BUFSIZ);

	if (sub) {
		char *cp;
		struct source *s;
		struct temp *h;

		newenv(E_ERRH);
		if (setjmp(e.jbuf)) {
			if (f != NULL)
				fclose(f);
			quitenv();
			return -1; /* todo: error()? */
		}

		/* set up yylex input from here file */
		s = pushs(SFILE);
		s->u.file = f;
		source = s;
		if (yylex(ONEWORD) != LWORD)
			errorf("exec:herein error\n");
		cp = evalstr(yylval.cp, 0);

		/* write expanded input to another temp file */
		h = maketemp(ATEMP);
		h->next = e.temps; e.temps = h;
		if (h == NULL)
			error();
		f = fopen(h->name, "w+");
		if (f == NULL)
			error();
		setvbuf(f, (char *)NULL, _IOFBF, BUFSIZ);
		fputs(cp, f);
		rewind(f);

		quitenv();
	}
	fd = dup(fileno(f));
	fclose(f);
	return fd;
}

static void
echo(vp, ap)
	register char **vp, **ap;
{
	shellf("+");
	while (*vp != NULL)
		shellf(" %s", *vp++);
	while (*ap != NULL)
		shellf(" %s", *ap++);
	shellf("\n");
}

