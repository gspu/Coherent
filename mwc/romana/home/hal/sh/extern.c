/*
 * Bourne shell.
 * External variables.
 */

#include "sh.h"

/* Set time, run time, and internal flags */
/* See POSIX.2 3.14.11 */

struct flag shflags [] = {
	{ 'e', 0, 0, 0 },	/* exit on error */
	{ 'k', 0, 0, 1 },	/* all assignments into environment */
	{ 'n', 0, 0, 0 },	/* read but do not execute */
	{ 'p', 0, 0, 1 },	/* ?? */
	{ 't', 0, 0, 1 },	/* read one command and exit */
	{ 'u', 0, 0, 0 },	/* message to stderr on expanding unset var */
	{ 'v', 0, 0, 0 },	/* write input to stderr as read */
	{ 'x', 0, 0, 0 },	/* write cmd to stderr before executing */
	{ 'c', 0, 1, 0 },	/* execute argument as command */
	{ 'i', 0, 1, 0 },	/* shell is interactive */
	{ 'r', 0, 1, 0 },	/* restricted shell */
	{ 's', 0, 1, 0 },	/* write output to stderr */
	{ 'C', 0, 0, 0 },	/* don't clobber files with > */
	{ 'a', 0, 0, 0 },	/* make assignments exported by default */
	{ 'f', 0, 0, 0 },	/* disable pathname expansion */
	{ 'd', 0, 0, 0 },	/* debugging stuff */
	{ 'U', 0, 0, 0 }	/* for Udo, no $() or $(()) */
};

int		lgnflag;		/* Login shell */
int		no1flag = 1;		/* Invoked shell */


/*
 * Parse node information.
 */

struct nodeinfo nodeinfo [] = {

#define	PARSE_NODE(node,desc,union,loop) { STRING (desc), union, loop },
#include "node.tab"
#undef	PARSE_NODE

	{ NULL, 0, 0 }
};


/* Shell status housekeeping */
jmp_buf	restart;
SHFUNC	*sh_fnp = NULL;
int	in_sh_fn = 0;
int	ret_done = 0;
int	sargc = 0;
char	*sarg0 = NULL;
char	**sargp = NULL;
char	*scmdp = NULL;
int	nargc = 0;
char	**nargv = NULL;
char	**nenvp = NULL;
char	**niovp = NULL;
NODE	 *nctlp = NULL;
SES	*sesp = NULL;
int	shpid;
int	slret;
int	sback;
int	spipe;
int	ufmask = 0777;
/* Other internal flags. */
int	comflag = 1;
int	errflag;
int	keyflag;
int	noeflag;
int	prpflag = 1;


/* Parameters used by shell, names and values */
CONST char    *	vhome;
CONST char    *	vpath;
CONST char    *	vmail;
CONST char    *	vps1;
CONST char    *	vps2;
CONST char    *	vifs;

/* Shell data */
VAR	*varp = NULL;
