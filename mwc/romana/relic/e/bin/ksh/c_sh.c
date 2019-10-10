/*
 * built-in Bourne commands
 */

static char *RCSid = "$Header: c_sh.c,v 3.1 88/11/03 09:14:31 egisin Exp $";

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/times.h>
#include <unistd.h>		/* getcwd */
#include "sh.h"
#include "lex.h"
#include "tree.h"
#include "table.h"

static	void putvlist();
static	char *clocktos();

int
c_label(wp)
	char **wp;
{
	return 0;
}

/* todo: add symlink hacks */
int
c_cd(wp)
	register char **wp;
{
	static char path[PATH];
	register char *cp;
	register struct tbl *vp;

	if ((cp = wp[1]) == NULL && (cp = strval(global("HOME"))) == NULL)
		errorf("no home directory");
	if (strcmp(cp, "-") == 0) {
		cp = strval(global("OLDPWD"));
		shellf("%s\n", cp);
	}
	if (chdir(cp) < 0)
		errorf("%s: bad directory\n", cp);
	flushcom(0);

	/* maintain $PWD and $OLDPWD */
	vp = global("PWD");
	cp = strval(vp);
	if (cp != null)
		setstr(global("OLDPWD"), cp);
	cp = getcwd(path, (size_t)PATH);
	if (cp == NULL)
		unset(vp);
	else
		setstr(vp, cp);

	return 0;
}

int
c_shift(wp)
	register char **wp;
{
	register struct block *l = e.loc;
	register int n;

	n = wp[1] ? evaluate(wp[1]) : 1;
	if (l->argc < n) {
		errorf("nothing to shift\n");
		return (1);
	}
	l->argv[n] = l->argv[0];
	l->argv += n;
	l->argc -= n;
	return 0;
}

int
c_umask(wp)
	register char **wp;
{
	register int i;
	register char *cp;

	if ((cp = wp[1]) == NULL) {
		i = umask(0);
		umask(i);
		printf("%03o\n", i);	/* should this be shell output? */
	} else {
		for (i = 0; *cp>='0' && *cp<='7'; cp++)
			i = i*8 + (*cp-'0');
		umask(i);
	}
	return 0;
}

int
c_dot(wp)
	char **wp;
{
	char *file, *cp;

	if ((cp = wp[1]) == NULL)
		return 0;
	file = search(cp, path, 0);
	if (file == NULL)
		errorf("%s: not found\n", cp);
	if (include(file))
		return exstat;
	return -1;
}

int
c_wait(wp)
	char **wp;
{
	register char *cp;

	wp++;
	cp = *wp;
	if (cp == NULL)
		cp = "%";
	/* todo: print status ? */
	return waitfor(j_lookup(cp));
}

int
c_read(wp)
	register char **wp;
{
	register int c = 0;
	FILE *f = stdin;
	int expand = 1;
	register char *cp;

	for (wp++; (cp = *wp) != NULL && *cp++ == '-'; wp++) {
		while (*cp) switch (*cp++) {
		  case 'e':
			expand = 1;
			break;
		  case 'r':
			expand = 0;
			break;
		  case 'u':
			if (!digit(*cp) || (f = shf[*cp++-'0']) == NULL)
				errorf("bad -u argument\n");
			break;
		}
	}

	if (*wp == NULL)
		errorf("missing name\n");
	if ((cp = strchr(*wp, '?')) != NULL) {
		*cp = 0;
		if (flag[FTALKING]) {
			shellf("%s ", cp+1);
			fflush(shlout);
		}
	}

	for (; *wp != NULL; wp++) {
		for (cp = line; cp <= line+LINE; ) {
			if (c == '\n')
				break;
			c = getc(f);
			if (c == EOF)
				return 1;
			if (expand && c == '\\') {
				c = getc(f);
				if (c == '\n')
					c = 0;
				else
					*cp++ = c;
				continue;
			}
			if (c == '\n' || wp[1] && ctype(c, C_IFS))
				break;
			*cp++ = c;
		}
		*cp = 0;
		setstr(global(*wp), line);
	}
	return 0;
}

int
c_eval(wp)
	register char **wp;
{
	register struct source *s;

	s = pushs(SWORDS);
	s->u.strv = wp+1;
	return shell(s);
}

void setsig ARGS((struct trap *p, handler_t f));

int
c_trap(wp)
	register char **wp;
{
	int i;
	char *s;
	register struct trap *p;

	wp++;
	if (*wp == NULL) {
		for (p = sigtraps, i = SIGNALS; --i >= 0; p++) {
			if (p->trap != NULL)
				shellf("%s: %s\n", p->name, p->trap);
		}
		return 0;
	}

	s = (gettrap(*wp) == NULL) ? *wp++ : NULL; /* get command */
	if (s != NULL && s[0] == '-' && s[1] == '\0')
		s = NULL;

	/* set/clear traps */
	while (*wp != NULL) {
		p = gettrap(*wp++);
		if (p == NULL)
			errorf("trap: bad signal %s\n", wp[-1]);
		if (p->trap != NULL)
			afree((Void*)p->trap, APERM);
		p->trap = NULL;
		if (s != NULL) {
			if (strlen(s) != 0) {
				p->trap = strsave(s, APERM);
				setsig(p, trapsig);
			} else
				setsig(p, (handler_t)SIG_IGN);
		} else
			/* todo: restore to orginal value */
		    setsig(p,
		       (p->signal==SIGINT || p->signal==SIGQUIT) && flag[FTALKING]
			   ? (handler_t)SIG_IGN : (handler_t)SIG_DFL);
	}
	return 0;
}

void
setsig(p, f)
	register struct trap *p;
	void (*f)();
{
	if (p->signal == 0)
		return;
	if (signal(p->signal, SIG_IGN) != SIG_IGN || p->ourtrap) {
		p->ourtrap = 1;
		signal(p->signal, f);
	}
}

int
c_return(wp)
	char **wp;
{
	wp++;
	if (*wp != NULL)
		exstat = getn(*wp);
	quitenv();		/* pop E_TCOM */
	while (e.type == E_LOOP || e.type == E_EXEC)
		quitenv();
	if (e.type == E_FUNC)
		longjmp(e.jbuf, 1);
	leave(exstat);
}

int
c_brkcont(wp)
	register char **wp;
{
	int quit;

	quit = wp[1] == NULL ? 1 : getn(wp[1]);
	quitenv();		/* pop E_TCOM */
	while (e.type == E_LOOP || e.type == E_EXEC) {
		if (e.type == E_LOOP && --quit <= 0)
			longjmp(e.jbuf, (*wp[0] == 'b') ? LBREAK : LCONTIN);
		quitenv();
	}
	errorf("cannot %s\n", wp[0]);
}

int
c_exit(wp)
	char **wp;
{
	register char *cp;

	e.oenv = NULL;
	if ((cp = wp[1]) != NULL)
		exstat = getn(cp);
#if JOBS
	if (flag[FMONITOR] && j_stopped()) /* todo: only once */
		errorf("There are stopped jobs\n");
#endif
	leave(exstat);
}

int
c_exro(wp)
	register char **wp;
{
	int flag = (**wp == 'e') ? EXPORT : RDONLY;

	if (*++wp != NULL) {
		for (; *wp != NULL; wp++)
			if (typeset(*wp, flag, 0) == NULL)
				errorf("%s: bad identifier\n", *wp);
	} else
		putvlist(flag);
	return 0;
}

static void
putvlist(flag)
	register int flag;
{
	struct block *l = e.loc;
	register struct tbl *vp, **p;

	flag |= ISSET;
	for (l = e.loc; l != NULL; l = l->next)
		for (p = tsort(&l->vars); (vp = *p++) != NULL; )
			if ((vp->flag&flag) == flag) {
				if (vp->flag & EXPORT)
					printf("export ");
				if (vp->flag & RDONLY)
					printf("readonly ");
				printf("%s\n", vp->name);
			}
}

int
c_set(wp)
	register char **wp;
{
	struct block *l = e.loc;
	register struct tbl *vp, **p;
	register char **owp = wp;
	register char *cp;
	int old_fmonitor = flag[FMONITOR];

	if ((cp = *++wp) == NULL) {
		static char * Const args [] = {"set", "-", NULL};
		extern int c_typeset ARGS((char **args));
		return c_typeset(args);
	}
	
	for (; (cp = *wp) != NULL && (*cp == '-' || *cp == '+');) {
		int i, n = *cp++ == '-'; /* set or clear flag */
		wp++;
		if (*cp == '\0') {
			if (n)
				flag[FXTRACE] = flag[FVERBOSE] = 0;
			break;
		}
		if (*cp == '-')
			goto setargs;
		for (; *cp != '\0'; cp++)
			if (*cp == 'o') {
				if (*wp == NULL) {
					printoptions();
					return 0;
				}
				i = option(*wp++);
				if (i == 0)
					shellf("%s: unknown option\n", *--wp);
				flag[i] = n;
			} else if (*cp>='a' && *cp<='z')
				flag[FLAG(*cp)] = n;
			else
				errorf("%c: bad flag\n", *cp);
		if (flag[FTALKING])
			flag[FERREXIT] = 0;
	}

#if JOBS
	if (old_fmonitor != flag[FMONITOR])
		j_change();
#endif

	/* set $# and $* */
	if (*wp != NULL) {
	  setargs:
		owp = --wp;
		wp[0] = l->argv[0]; /* save $0 */
		while (*++wp != NULL)
			*wp = strsave(*wp, &l->area);
		l->argc = wp - owp - 1;
		l->argv = (char **) alloc(sizeofN(char *, l->argc+2), &l->area);
		for (wp = l->argv; (*wp++ = *owp++) != NULL; )
			;
		resetopts();
	}
	return 0;
}

int
c_unset(wp)
	register char **wp;
{
	register char *id;
	int flagf = 0;

	for (wp++; (id = *wp) != NULL && *id == '-'; wp++)
		if (*++id == 'f')
			flagf++;
	for (; (id = *wp) != NULL; wp++)
		if (!flagf) {	/* unset variable */
			unset(local(id));
		} else {	/* unset function */
			register struct tbl *tp;
			tp = tsearch(&e.loc->funs, id, hash(id));
			if (tp != NULL)
				define(tp, (struct op *)NULL);
		}
	return 0;
}

int
c_ulimit(wp)
	register char **wp;
{
	extern int do_ulimit();

	return do_ulimit(wp[1], wp[2]);
}

int
c_times(wp)
	char **wp;
{
	struct tms all;

	(void) times(&all);
	printf("Shell: ");
	printf("%8s user ", clocktos(all.tms_utime));
	printf("%8s system\n", clocktos(all.tms_stime));
	printf("Kids:  ");
	printf("%8s user ", clocktos(all.tms_cutime));
	printf("%8s system\n", clocktos(all.tms_cstime));

	return 0;
}

/*
 * time pipeline (really a statement, not a built-in comman)
 */
int
timex(t, f)
	struct op *t;
	int f;
{
	int rv;
	struct tms t0, t1;
	clock_t t0t, t1t, time();
	extern clock_t j_utime, j_stime; /* computed by j_wait */

	j_utime = j_stime = 0;
#if COHERENT
	t0t = time((clock_t)NULL);
	(void)times(&t0);
#else
	t0t = times(&t0);
#endif
	rv = execute(t->left, f);
#if COHERENT
	t1t = time((clock_t)NULL);
	(void)times(&t1);
#else
	t1t = times(&t1);
#endif
	shellf("%8s real ", clocktos((t1t - t0t) * 100));
	shellf("%8s user ",
	       clocktos(t1.tms_utime - t0.tms_utime + j_utime));
	shellf("%8s system ",
	       clocktos(t1.tms_stime - t0.tms_stime + j_stime));
	shellf("\n");

	return rv;
}

static char *
clocktos(t)
	clock_t t;
{
	static char temp[20];
	register int i;
	register char *cp = temp + sizeof(temp);

#if !COHERENT
#if CLK_TCK != 100		/* convert to 1/100'ths */
	t = (t < 1000000000/CLK_TCK) ?
		(t * 100) / CLK_TCK : (t / CLK_TCK) * 100;
#endif
#endif
	*--cp = '\0';
	*--cp = 's';
	for (i = -2; i <= 0 || t > 0; i++) {
		if (i == 0)
			*--cp = '.';
		*--cp = '0' + (char)(t%10);
		t /= 10;
	}
	return cp;
}

/* dummy function, special case in comexec() */
int
c_exec(wp)
	char ** wp;
{
	return 0;
}

/* dummy function, special case in comexec() */
int
c_builtin(wp)
	char ** wp;
{
	return 0;
}

extern	int c_test();		/* in test.c */

Const struct builtin shbuiltins [] = {
	{"=:", c_label},
	{"=.", c_dot},
	{"[", c_test},
	{"=cd", c_cd},
	{"builtin", c_builtin},
	{"=exec", c_exec},
	{"=shift", c_shift},
	{"wait", c_wait},
	{"read", c_read},
	{"=eval", c_eval},
	{"trap", c_trap},
	{"break", c_brkcont},
	{"continue", c_brkcont},
	{"exit", c_exit},
	{"=export", c_exro},
	{"=readonly", c_exro},
	{"=return", c_return},
	{"=set", c_set},
	{"=unset", c_unset},
	{"umask", c_umask},
	{"test", c_test},
	{"times", c_times},
	{"ulimit", c_ulimit},
	{NULL, NULL}
};

