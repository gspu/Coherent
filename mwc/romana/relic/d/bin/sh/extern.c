/*
 * sh/extern.c
 * Bourne shell.
 * External variables.
 */

#include "sh.h"

/* Set time, run time, and internal flags */
/* See exec3.c/set before you move these around */
char shflags [] = {
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0
};
char shfnams [] = {
	'e','k','n','p','t','u','v','x','c','i','r','s','-', 0 , 0 , 0 , 0 , 0
};
/* Shell status housekeeping */
jmp_buf	restart;
int	sargc = 0;
char	*sarg0 = NULL;
char	**sargv = NULL;
char	**senvp = NULL;
char	**sargp = NULL;
char	*scmdp = NULL;
int	nargc = 0;
char	**nargv = NULL;
char	**nenvp = NULL;
char	**niovp = NULL;
NODE	 *nctlp = NULL;
SES	*sesp = NULL;
int	yyline = 1;
int	shpid = 0;
int	slret = 0;
int	sback = 0;
int	spipe = 0;
int	ufmask = 0777;
/* Other internal flags. */
int	comflag = 1;
int	errflag = 0;
int	keyflag = 1;
int	noeflag = 0;
int	prpflag = 1;
int	readflag = 0;
/* Parameters used by shell, names and values */
char	*vhome = NULL;
char	*vpath = NULL;
char	*vmail = NULL;
char	*vps1 = NULL;
char	*vps2 = NULL;
char	*vifs = NULL;
/* Shell data */
char	strt[STRSIZE];
char	*strp;
VAR	*varp = NULL;

/* Directory stack. */
char	*dstack[DSTACKN];		/* Directory stack */
int	dstkp;				/* Directory stack pointer */

/* end of sh/extern.c */
