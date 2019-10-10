/*
 * startup, main loop, enviroments and error handling
 */

static char *RCSid = "$Header: main.c,v 3.1 88/11/03 09:17:04 egisin Exp $";

#define Extern

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include "sh.h"
#include "lex.h"
#include "tree.h"
#include "table.h"

/*
 * global data
 */

Area	aperm;

static	void	reclaim ARGS((void));

/*
 * shell initialization
 */

static	char	initifs [] = "IFS= \t\n"; /* must be R/W */

static	Const	char   initsubs [] = 
  "${SHELL:=/bin/sh} ${PATH:=/bin:/usr/bin:.} ${HOME:=/} ${PS1:=$ } ${PS2:=> }";

static	Const	char *initcoms [] = {
	"cd", ".", NULL,		/* set up $PWD */
	"typeset", "-x", "SHELL", "PATH", "HOME", NULL,
	"typeset", "-r", "PWD", "OLDPWD", NULL,
	"typeset", "-i", "SECONDS=0", "OPTIND", NULL,
	"alias",
	  "integer=typeset -i", "pwd=print -r \"$PWD\"",
	  "history=fc -l", "r=fc -s",
#if !COHERENT
	  "nohup=nohup ",
#endif
	  "login=exec login", "newgrp=exec newgrp",
	  "type=whence -v", "functions=typeset -f",
	  "echo=print", "true=:", "false=let", "[=\\[", NULL,
	NULL
};

main(argc, argv, envp)
	int argc;
	register char **argv;
	char **envp;
{
	register int i;
	register char *arg;
	int cflag = 0, qflag = 0;
	char *name;
	register Source *s;
	register struct block *l = &globals;
	register char **wp0, **wp;
	extern char ksh_version [];

	ainit(&aperm);		/* initialize permanent Area */

	/* set up base enviroment */
	e.type = E_NONE;
	ainit(&e.area);
	e.loc = l;
	e.savefd = NULL;
	e.oenv = NULL;

	initctypes();

	/* open file streams for fd's 0,1,2 */
	fopenshf(0);	fopenshf(1);	fopenshf(2);

	/* set up variable and command dictionaries */
	newblock();		/* set up global l->vars and l->funs */
	tinit(&commands, APERM);
	tinit(&builtins, APERM);
	tinit(&lexicals, APERM);
	tinit(&homedirs, APERM);

	/* import enviroment */
	if (envp != NULL)
		for (wp = envp; *wp != NULL; wp++)
			import(*wp);

	typeset(initifs, 0, 0);	/* for security */
	typeset(ksh_version, 0, 0); /* RDONLY */

	/* define shell keywords */
	keywords();

	/* define built-in commands */
	for (i = 0; shbuiltins[i].name != NULL; i++)
		builtin(shbuiltins[i].name, shbuiltins[i].func);
	for (i = 0; kshbuiltins[i].name != NULL; i++)
		builtin(kshbuiltins[i].name, kshbuiltins[i].func);

	/* assign default shell variable values */
	substitute(initsubs, 0);
	/* execute initialization statements */
	for (wp0 = (char**) initcoms; *wp0 != NULL; wp0 = wp+1) {
		/* copy because the alias initializers are readonly */
		for (wp = wp0; *wp != NULL; wp++)
			*wp = strsave(*wp, ATEMP);
		shcomexec(wp0);
	}
	afreeall(ATEMP);

	if (geteuid() == 0)
		setstr(global("PS1"), "# ");

	s = pushs(SFILE);
	s->u.file = stdin;
	cflag = 0;
	name = *argv++;

	/* what a bloody mess */
	if (--argc >= 1) {
		if (argv[0][0] == '-' && argv[0][1] != '\0') {
			for (arg = argv[0]+1; *arg; arg++)
				switch (*arg) {
				  case 'c':
					cflag = 1;
					if (--argc > 0) {
						s->type = SSTRING;
						s->str = *++argv;
					}
					break;
	
				  case 'q':
					qflag = 1;
					break;

				  default:
					if (*arg>='a' && *arg<='z')
						flag[FLAG(*arg)]++;
				}
		} else {
			argv--;
			argc++;
		}
		if (s->type == SFILE && --argc > 0) {
			if ((s->u.file = fopen(*++argv, "r")) == NULL)
				errorf("%s: cannot open\n", *argv);
			s->file = *argv;
			fileno(s->u.file) = savefd(fileno(s->u.file));
			setvbuf(s->u.file, (char *)NULL, _IOFBF, BUFSIZ);
		}
	}

	if (s->type == SFILE) {
		if (fileno(s->u.file) == 0)
			flag[FSTDIN] = 1;
		if (isatty(0) && isatty(1) && !cflag)
			flag[FTALKING] = 1;
		if (flag[FTALKING] && flag[FSTDIN])
			s->type = STTY;
	}
	if (s->type == STTY) {
		ttyfd = fcntl(0, F_DUPFD, FDBASE);
#if !COHERENT
		(void) fcntl(ttyfd, F_SETFD, FD_CLEXEC);
#endif
#if EDIT
		x_init();
#endif
	}

	/* initialize job control */
	j_init();

	if (!qflag)
		ignoresig(SIGQUIT);

	if (name[0] == '-') {
		flag[FTALKING] = 1;
#if !COHERENT
		(void) include("/etc/profile");
#endif
		(void) include(".profile");
	}

	/* include $ENV */
	arg = substitute(strval(global("ENV")), DOTILDE);
	if (*arg != '\0')
		(void) include(arg);

	if (flag[FTALKING]) {
		signal(SIGTERM, trapsig);
		ignoresig(SIGINT);
	} else
		flag[FHASHALL] = 1;

#if JOBS			/* todo: could go before includes? */
	if (s->type == STTY) {
		flag[FMONITOR] = 1;
		j_change();
	}
#endif

	l->argv = argv;
	l->argc = argc;
	l->argv[0] = name;
	resetopts();

	argc = shell(s);
	leave(argc);
}

int
include(name)
	register char *name;
{
	register FILE *f;
	register Source *s;

	if (strcmp(name, "-") != 0) {
		f = fopen(name, "r");
		if (f == NULL)
			return 0;
		/* todo: the savefd doesn't get popped */
		fileno(f) = savefd(fileno(f)); /* questionable */
		setvbuf(f, (char *)NULL, _IOFBF, BUFSIZ);
	} else
		f = stdin;
	s = pushs(SFILE);
	s->u.file = f;
	s->file = name;
	/*return*/ shell(s);
	if (f != stdin)
		fclose(f);
	return 1;
}

int
command(comm)
	register char *comm;
{
	register Source *s;

	s = pushs(SSTRING);
	s->str = comm;
	return shell(s);
}

/*
 * run the commands from the input source, returning status.
 */
int
shell(s)
	Source *s;		/* input source */
{
	struct op *t;
	Volatile int attempts = 13;

	newenv(E_PARSE);
	e.interactive = 1;
	exstat = 0;
	if (setjmp(e.jbuf)) {
		/*shellf("<unwind>");*/
		if (trap)	/* pending SIGINT */
			shellf("\n");
		sigtraps[SIGINT].set = 0;
	}

	while (1) {
		if (trap)
			runtraps();
		if (flag[FTALKING])
			signal(SIGINT, trapsig);

		if (s->next == NULL)
			s->echo = flag[FVERBOSE];

		j_notify();

		if (s->type == STTY)
			prompt = substitute(strval(global("PS1")), 0);

		t = compile(s);
		if (t != NULL && t->type == TEOF)
			if (s->type == SEOF && flag[FIGNEOF] && --attempts > 0) {
				s->type = STTY;
				shellf("Use `exit'\n");
			} else
				break;
		flushshf(2);	/* flush -v output */

		if (!flag[FNOEXEC] || s->type == STTY)
			execute(t, 0);

		reclaim();
	}
  Error:
	quitenv();
	return exstat;
}

void
leave(rv)
	int rv;
{
	if (e.type == E_TCOM && e.oenv != NULL)	/* exec'd command */
		unwind();
	runtrap(&sigtraps[0]);
	j_exit();
	exit(rv);
	/* NOTREACHED */
}

error()
{
	if (flag[FERREXIT] || !flag[FTALKING])
		leave(1);
	unwind();
}

/* return to closest error handler or shell(), exit if none found */
unwind()
{
	while (1)
		switch (e.type) {
		  case E_NONE:
			leave(1);
			/* NOTREACHED */
		  case E_PARSE:
			longjmp(e.jbuf, 1);
			/* NOTREACHED */
		  case E_ERRH:
			longjmp(e.jbuf, 1);
			/* NOTREACHED */
		  default:
			quitenv();
			break;
		}
}

newenv(type)
{
	register struct env *ep;

	ep = (struct env *) alloc(sizeof(*ep), ATEMP);
	*ep = e;
	ainit(&e.area);
	e.type = type;
	e.oenv = ep;
	e.savefd = NULL;
	e.temps = NULL;
}

quitenv()
{
	register struct env *ep;
	register int fd;

	if ((ep = e.oenv) == NULL)
		exit(exstat);	/* exit child */
	if (e.loc != ep->loc)
		popblock();
	if (e.savefd != NULL)
		for (fd = 0; fd < NUFILE; fd++)
			restfd(fd, e.savefd[fd]);
	reclaim();
	e = *ep;
}

/* remove temp files and free ATEMP Area */
static void
reclaim()
{
	register struct temp *tp;

	for (tp = e.temps; tp != NULL; tp = tp->next)
		remove(tp->name);
	e.temps = NULL;
	afreeall(&e.area);
}

void
aerror(ap, msg)
	Area *ap;
	Const char *msg;
{
	errorf("alloc internal error: %s\n", msg);
}

