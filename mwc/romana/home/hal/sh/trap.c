#include <signal.h>

#include "shellio.h"
#include "sh.h"

/*
 * Interrupt flags.
 */

static	int	intflag = 0;
static	int	realint = 0;
static	int	norecur = 0;


/*
 * Trap storage area.
 */

typedef struct {
	int	t_cnt;		/* Signal received */
	void	(*t_set)();	/* Current action */
	void	(*t_def)();	/* Default action */
	char	*t_act;		/* Action to take */
} TRAP;

TRAP trap [NSIG + 1];

/*
 * Return a character-string name for a signal.
 */

char *
signame (signo)
int		signo;
{
	switch (signo) {
	/*
	 * Construct the case: labels from a general system.
	 */

#define	SIGNAL(name,short,desc)	case name: return desc;
#include "signal.tab"
#undef	SIGNAL

	default:
		return NULL;
	}
}


void
deftrp (sig, def)
int sig;
int (*def)();
{
	sigset (sig, def);
	trap [sig].t_cnt = 0;
	trap [sig].t_set = def;
	trap [sig].t_def = def;
	if (trap [sig].t_act != NULL) {
		sfree (trap [sig].t_act);
		trap [sig].t_act = NULL;
	}
}


/*
 * Set default traps for various contexts.
 */

void
dflttrp (context)
int		context;
{
	int		sig;

	switch (context) {
	case IRDY:	/* Trap initialisation */
		for (sig = 0; sig <= NSIG; sig += 1)
			deftrp (sig, sigset (sig, SIG_IGN));

		if (iflag) {
			sigset (SIGINT, sigintr);
			sigset (SIGTERM, SIG_IGN);
		}
		sigset (SIGQUIT, SIG_IGN);
		break;
		
	case IBACK:	/* Background initialisation */
		deftrp (SIGINT, SIG_IGN);
		deftrp (SIGQUIT, SIG_IGN);
		break;

	case IDONE:	/* Done wait () */
		sigset (SIGINT, iflag ? sigintr : trap [SIGINT].t_set);
		break;

	case IBUILTIN:	/* Builtin command initialisation */
	case IWAITINT:	/* Parent, waiting interactively */
		if (iflag)
			sigset (SIGINT, sigintr);
		sigset (SIGQUIT, SIG_IGN);
		break;

	case IPARENT:	/* parent, before fork () */
	case IWAIT:	/* Parent, waiting for child */
		sigset (SIGINT, SIG_HOLD);
		sigset (SIGQUIT, SIG_IGN);
		break;

	case IFORK:	/* Sub-shell initialization */
		sigset (SIGINT, trap [SIGINT].t_set);
		sigset (SIGTERM, trap [SIGTERM].t_set);
		break;

	case ICMD:	/* Command execution initialisation */
		sigset (SIGQUIT, trap [SIGQUIT].t_set);
		break;

	case IPIPE:	/* In child about to take LHS of | command */
		break;
	}
}


/*
 * Show the contents of traps.
 */

telltrp ()
{
	int sig;
	void (*act)();
	char *what;

	if ((what = trap [0].t_act) != NULL) {
		cmd_outstr ("0 = exit : ");
		cmd_outstr (what);
		cmd_endl ();
	}

	for (sig = 1; sig <= NSIG ; sig += 1) {
		act = trap [sig].t_set;

		if (act == SIG_IGN)
			what = "ignored";
		else if (act == sigintr)
			what = trap [sig].t_act;
		else
			what = NULL;

		if (what) {
			cmd_outint (sig);
			cmd_outstr (" = ");

			if (signame (sig) != NULL) {
				cmd_outstr (signame (sig));
				cmd_outstr (" : ");
			}

			cmd_outstr (what);
			cmd_endl ();
		}
	}
	return 0;
}


/*
 * Set a trap.
 */

int
setstrp (sig, actp)
int		sig;
char	      *	actp;
{
	int spc;

	if (sig < 0 || sig > NSIG) {
		if (cmderr_begin ("trap")) {
			cmderr_outstr ("trap number out of range: ");
			cmderr_outint (sig);
			cmderr_endl ();
		}
		return 1;
	}

	spc = (iflag && (sig == SIGINT || sig == SIGTERM)) || sig == SIGQUIT;
	if (trap [sig].t_act != NULL)
		sfree (trap [sig].t_act);
	trap [sig].t_act = NULL;

	if (actp == NULL) {
		if (! spc)
			sigset (sig, trap [sig].t_def);
		trap [sig].t_set = trap [sig].t_def;
	} else if (* actp == 0) {
		if (! spc)
			sigset (sig, SIG_IGN);
		trap [sig].t_set = SIG_IGN;
	} else if (trap [sig].t_def == SIG_IGN) {
		;
	} else {
		trap [sig].t_act = duplstr (actp, 1);
		if (! spc)
			sigset (sig, sigintr);
		trap [sig].t_set = sigintr;
	}
	return 0;
}


/*
 * Execute a trap action.
 */
/*
 * Interrupt routine.
 */

void
sigintr (sig)
unsigned int	sig;
{
	intflag += 1;

	if (sig <= NSIG)
		trap [sig].t_cnt += 1;

	if (sig == SIGINT) {
		noeflag += 1;
		realint += 1;
	}
}


/*
 * Recover from an interrupt.
 */

int
recover (context)
{
	int		sig;
	char	      *	actp;

	if (! intflag || norecur)
		return 1;

	norecur ++;

	switch (context) {
	case IPROF:
		exit (-1);

	case IRDY:
	case ICMD:
		break;

	case ILEX:
	case IEVAL:
		norecur = 0;
		return ! (realint && iflag);
	}

	for (sig = 0; sig <= NSIG; sig += 1) {
		if (trap [sig].t_cnt) {
			trap [sig].t_cnt = 0;
			if ((actp = trap [sig].t_act) != NULL) {
				/*
				 * NIGEL: Take steps to preserve the exit code.
				 */

				int oslret = slret;

#if	0
				trap [sig].t_act = NULL;
				session (SARGS, actp);
				trap [sig].t_act = actp;
				setstrp (sig, NULL);
#else
				session (SARGS, actp);
				slret = oslret;
#endif
			}
		}
	}
	intflag = 0;
	norecur = 0;
	if (realint) {
		noeflag = 0;
		realint = 0;
		reset (iflag ? RINT : RINTDIE);
	}

	return 1;
}
