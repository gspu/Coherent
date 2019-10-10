#include "sh.h"
#include <signal.h>

/*
 * Interrupt flags.
 */
int	intflag = 0;
int	realint = 0;
int	norecur = 0;

/*
 * Trap storage area.
 */
typedef struct {
	int	t_cnt;		/* Signal received */
	void	(*t_set)();	/* Current action */
	void	(*t_def)();	/* Default action */
	char	*t_act;		/* Action to take */
} TRAP;

TRAP trap[NSIG+1];

/*
 * Set default traps for various contexts.
 */
dflttrp(context)
register int context;
{
	register int sig;

	switch (context) {
	case IRDY:	/* Trap initialisation */
		for (sig=0; sig<=NSIG; sig+=1)
			deftrp(sig, signal(sig, SIG_IGN));
		if (iflag) {
			signal(SIGINT, sigintr);
			signal(SIGTERM, SIG_IGN);
		}
#ifndef DEBUG
		signal(SIGQUIT, SIG_IGN);
#endif
		break;
	case IBACK:	/* Background initialisation */
		deftrp(SIGINT, SIG_IGN);
		deftrp(SIGQUIT, SIG_IGN);
		break;
	case IFORK:	/* Sub-shell initialization */
		signal(SIGINT, trap[SIGINT].t_set);
		signal(SIGTERM, trap[SIGTERM].t_set);
		break;
	case ICMD:	/* Command execution initialisation */
		signal(SIGQUIT, trap[SIGQUIT].t_set);
		break;
	}
}

deftrp(sig, def)
register int sig;
register int (*def)();
{
	signal(sig, def);
	trap[sig].t_cnt = 0;
	trap[sig].t_set = def;
	trap[sig].t_def = def;
	sfree(trap[sig].t_act);
	trap[sig].t_act = NULL;
}


/*
 * Show the contents of traps.
 */
telltrp()
{
	register int sig;
	register void (*act)();
	register char *what;

	if ((what=trap[0].t_act) != NULL)
		prints("0=exit: %s\n", what);
	for (sig=1; sig<=NSIG; sig+=1) {
		act = trap[sig].t_set;
		if (act==SIG_IGN)
			what = "ignored";
		else if (act==sigintr)
			what = trap[sig].t_act;
		else
			what = NULL;
		if (what)
			prints("%d=%s: %s\n", sig, signame[sig], what);
	}
	return 0;
}

/*
 * Set a trap.
 */
setstrp(sig, actp)
register int sig;
register char *actp;
{
	int spc;

	if (sig < 0 || sig > NSIG) {
		printe("Bad trap: %d", sig);
		return 1;
	}
	spc = (iflag && (sig==SIGINT||sig==SIGTERM)) || sig==SIGQUIT;
	sfree(trap[sig].t_act);
	trap[sig].t_act = NULL;
	if (actp==NULL) {
		if (! spc)
			signal(sig, trap[sig].t_def);
		trap[sig].t_set = trap[sig].t_def;
	} else if (*actp=='\0') {
		if (! spc)
			signal(sig, SIG_IGN);
		trap[sig].t_set = SIG_IGN;
	} else if (trap[sig].t_def == SIG_IGN) {
		;
	} else {
		trap[sig].t_act = duplstr(actp, 1);
		if (! spc)
			signal(sig, sigintr);
		trap[sig].t_set = sigintr;
	}
	return 0;
}

/*
 * Execute a trap action.
 */
/*
 * Interrupt routine.
 */
void sigintr(sig)
register int sig;
{
	signal(sig, sigintr);
	intflag += 1;
	trap[sig].t_cnt += 1;
	if (sig==SIGINT && iflag) {
		noeflag += 1;
		realint += 1;
	}
}

/*
 * Recover from an interrupt.
 */
recover(context)
{
	register int sig;
	register char *actp;

	if (! intflag || norecur)
		return 1;
	norecur++;
	realint = realint && iflag;	/* iflag may have been cleared */
	switch (context) {
	case IPROF:
		exit(0377);
	case IRDY:
	case ICMD:
		break;
	case ILEX:
	case IEVAL:
		norecur = 0;
		return ! realint;
	}
	for (sig = 0; sig <= NSIG; sig += 1) {
		if (trap[sig].t_cnt) {
			trap[sig].t_cnt = 0;
			if (actp = trap[sig].t_act) {
#if	0
				trap[sig].t_act = NULL;
				session(SARGS, actp);
				trap[sig].t_act = actp;
				setstrp(sig, NULL);
#else
				session(SARGS, actp);
#endif
			} else if (sig==SIGINT) {
				if (! realint)
					kill(getpid(), SIGINT);
			}
		}
	}
	intflag = 0;
	norecur = 0;
	if (realint) {
		noeflag = 0;
		realint = 0;
		reset(RINT);
		NOTREACHED;
	}
	return 1;
}
