/*
 * signal handling
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include "sh.h"
#include "lex.h"

Trap sigtraps [SIGNALS] = {
	{0,	"EXIT", "Signal 0"}, /* todo: belongs in e.loc->exit */
	{SIGHUP, "HUP", "Hangup"},
	{SIGINT, "INT", "Interrupt"},
	{SIGQUIT, "QUIT", "Quit"},
#if !COHERENT
	{SIGILL, "ILL", "Illegal instruction"},
#endif
	{SIGTRAP, "TRAP", "Trace trap"},
#if !COHERENT
	{SIGIOT, "IOT", "Abort"},
	{SIGEMT, "EMT", "EMT trap"},
	{SIGFPE, "FPE", "Floating exception"},
#endif
	{SIGKILL, "KILL", "Killed"},
#if !COHERENT
	{SIGBUS, "BUS", "Bus error"},
#endif
	{SIGSEGV, "SEGV", "Memory fault"},
	{SIGSYS, "SYS", "Bad system call"},
	{SIGPIPE, "PIPE", "Broken pipe"},
	{SIGALRM, "ALRM", "Alarm clock"},
	{SIGTERM, "TERM", "Terminated"},
#if JOBS			/* todo: need to be more portable */
	{SIGURG, "URG", "Urgent condition"}, /* BSDism */
	{SIGSTOP, "STOP", "Stop (signal)"},
	{SIGTSTP, "TSTP", "Stop"},
	{SIGCONT, "CONT", "Continue"},
	{SIGCHLD, "CHLD", "Waiting children"},
	{SIGTTIN, "TTIN", "Stop (tty input)"},
	{SIGTTOU, "TTOU", "Stop (tty output)"},
#endif
};

/*
 * compare two strings, mapping the case of the first string in
 * case the second string was lower case.
 */
int
namecmp(s1, s2)
char	*s1, *s2;
{
	while (*s1) {
		if (*s1 != *s2 && (!isalpha(*s1) || tolower(*s1) != *s2))
			break;
		++s1; ++s2;
	}
	return (*s1 - *s2);
}

Trap *
gettrap(name)
	char *name;
{
	int i;
	register Trap *p;

	if (digit(*name)) {
		i = getn(name);
		return (0 <= i && i < SIGNALS) ? &sigtraps[i] : NULL;
	}
#if 0
	if (strcmp("ERR", name) == 0)
		return &e.loc->err;
	if (strcmp("EXIT", name) == 0)
		return &e.loc->exit;
#endif
	for (p = sigtraps, i = SIGNALS; --i >= 0; p++)
		if (namecmp(p->name, name) == 0)
			return p;
	return NULL;
}

/*
 * trap signal handler
 */
void
trapsig(i)
	int i;
{
	trap = sigtraps[i].set = 1;

	if (i == SIGINT && e.type == E_PARSE) {
		if (source && source->type == STTY)
			source->str = null;
		/* dangerous but necessary to deal with BSD silly signals */
		longjmp(e.jbuf, 1);
	}
	(void) signal(i, trapsig); /* todo: use sigact */
}

/*
 * run any pending traps
 */
runtraps()
{
	int i;
	register Trap *p;

	for (p = sigtraps, i = SIGNALS; --i >= 0; p++)
		if (p->set)
			runtrap(p);
	trap = 0;
}

runtrap(p)
	Trap *p;
{
	char *trapstr;

	p->set = 0;
	if ((trapstr = p->trap) == NULL)
		if (p->signal == SIGINT)
			unwind();	/* return to shell() */
		else
			return;
	if (p->signal == 0)	/* ??? */
		p->trap = 0;
	command(trapstr);
}
 
/* restore signals for children */
cleartraps()
{
	int i;
	register Trap *p;

	for (i = SIGNALS, p = sigtraps; --i >= 0; p++) {
		p->set = 0;
		if (p->ourtrap && signal(p->signal, SIG_IGN) != SIG_IGN)
			(void) signal(p->signal, SIG_DFL);
	}
}

ignoresig(i)
	int i;
{
	if (signal(i, SIG_IGN) != SIG_IGN)
		sigtraps[i].sig_dfl = 1;
}

restoresigs()
{
	int i;
	register Trap *p;

	for (p = sigtraps, i = SIGNALS; --i >= 0; p++)
		if (p->sig_dfl) {
			p->sig_dfl = 0;
			(void) signal(p->signal, SIG_DFL);
		}
}

