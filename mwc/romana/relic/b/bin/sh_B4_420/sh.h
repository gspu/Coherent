/*
 * sh/sh.h
 * Bourne shell.
 * Header file.
 */

#include <stdio.h>
#include <setjmp.h>
#include <string.h>

#define STRSIZE	2000			/* Maximum length of single token */
#define IALSIZE	8			/* Initial argument list size */
#define	DSTACKN	30			/* Directory stack depth */
#define NOTREACHED	return

/*
 * Masks for looking up lextab.
 */
#define MNQUO	0001		/* Special in argument */
#define MDQUO	0002		/* Special in double quotes */
#define MHERE	0004		/* Special in here document */
#define MRVAR	(MBVAR|MDIGI)		/* Valid in regular variable name */
#define MSVAR	0010		/* Valid in special variable name */
#define MDIGI	0020		/* Is an ascii digit */
#define MBVAR	0040		/* Valid 1st char in regular var name */
#define MNAME	0100		/* Does not start a name */
#define MGLOB	0200		/* Must call glob */

/*
 * Determine if a character is a member of the given class.
 * (This depends of the fact that EOF == -1).
 */
#define class(c, m)	(c>=EOF && c<=0177 && (lextab[1+c]&m))

/*
 * Types of nodes.
 */
#define NNULL	0			/* Null */
#define NFOR	1			/* For */
#define NCASE	2			/* Case */
#define NWHILE	3			/* While */
#define NUNTIL	4			/* Until */
#define NIF	5			/* If */
#define NELSE	6			/* Else */
#define NCOMS	7			/* Command sequence */
#define NLIST	8			/* Command list */
#define NBACK	9			/* Background process */
#define NPIPE	10			/* Pipe */
#define NPARN	11			/* New shell */
#define NBRAC	12			/* Command group */
#define NANDF	13			/* Logical and operator */
#define NORF	14			/* Logical or operator */
#define NARGS	15			/* Argument string */
#define NIORS	16			/* Redirection of io */
#define NASSG	17			/* Assignment keyword */
#define NFOR2	18			/* Second part for, in list scan */
#define NCASE2	19			/* Second part case, command */
#define NCASE3	20			/* Third part case, pattern */
#define NCTRL	21			/* Control statement in list */
#define NRPIPE	22			/* Named pipe for read */
#define NWPIPE	23			/* Named pipe for write */
#define	NFUNC	24			/* Shell function */
#define	NRET	25			/* Return */

/*
 * Node.
 */
typedef	struct	node {
	int	n_type;			/* Type of node */
	struct	node *n_next;		/* Pointer to next */
	union {
		struct	node *nu_auxp;	/* Auxiliary pointer */
		char	*nu_strp;	/* Pointer to string */
	} n_u;
} NODE;
#define	n_auxp	n_u.nu_auxp
#define	n_strp	n_u.nu_strp

/*
 * Flags in variables.
 */
#define VEXP	001			/* Variable is exported */
#define VRDO	002			/* Read only */
#define VSET	004			/* Variable is used by shell */

/*
 * Enviroment list and variable structure.
 */
typedef	struct	var {
	int	v_flag;			/* Flags */
	struct	var *v_next;		/* Pointer to next entry */
	char	*v_strp;		/* Pointer to string */
} VAR;

/*
 * Buffer structure.
 */
typedef	struct	buf {
	struct	buf *b_next;		/* Pointer to next */
	int	b_size;			/* Size of buffer */
} BUF;

/*
 * Per executing command data structure.
 * Used for break/continue.
 */
typedef	struct	con {
	struct	con *c_next;		/* Pointer to next */
	NODE	*c_node;		/* Node which created this control */
	BUF	**c_bpp;		/* Current free buffer */
	jmp_buf	c_envl;			/* Enviroment list */
} CON;

/*
 * Session types.
 */
#define SINIT	0	/* Initial session */
#define SARGS	1	/* String */
#define SARGV	2	/* Vector of strings */
#define SFILE	3	/* File name */
#define SSTR	4	/* Open stream */

/*
 * Per session information structure.
 */
typedef struct ses {
	struct	ses	*s_next;
	int		s_type;
	int		s_flag;
	NODE		*s_node;
	CON		*s_con;
	BUF		**s_bpp;
	jmp_buf		s_envl;
	char		*s_strp;
	char		**s_argv;
	FILE		*s_ifp;
} SES;

/*
 * Session long jump codes
 */
#define RSET	0	/* Initial setjmp call */
#define REOF	1	/* parse matched start */
#define RCMD	2	/* parse matched command line */
#define RERR	3	/* parse matched error */
#define RINT	4	/* Interrupt recovery */
#define RNOSBRK	5	/* no more memory from alloc */
#define RSYSER	6	/* system returned -1 */
#define RUABORT	7	/* user requested error abort */
#define RUEXITS	8	/* user requested exit command */
#define RBRKCON	9	/* break or continue beyond program */
#define RNOWAY	10	/* Shell assertion failed */

/*
 * Interrupt recovery and set contexts.
 * Used to determine recovery action and default signals.
 */
#define IRDY	0	/* In session */
#define	ILEX	1	/* Parse in progress */
#define ICMD	2	/* Prior to next command execution */
#define IEVAL	3	/* Command evaluation in progress */
#define IBACK	4	/* Background process */
#define IFORK	5	/* Child after fork */
#define IPROF	6	/* Reading /etc/profile for login */

/*
 * Evaluation contexts.
 */
#define EWORD	0	/* Evaluate $N, `cmd`, strip quotes */
#define EHERE	1	/* Evaluate $N, `cmd`, ignore quotes */
#define EARGS	2	/* Evaluete $N, `cmd`, strip quotes, *?[, blanks */
#define EPATT	3	/* Evaluate for pattern match */


/*
 * NB: Here-document processing in shell functions places extra demands on
 * the management of temporary files. When a shell function is interned, any
 * here documents created within the function are turned into temporary
 * files which must persist for the life of the function (ie, until the
 * shell exits or the function is removed via "unset -f").
 *
 * The following data structure is intended to allow the temporary-file
 * context to be managed in the manner required. A pointer to a list of these
 * structures is stored in the function definition, taken from the global
 * list of temporary files when a function is defined.
 *
 * Related functions:
 *	def_sh_fn (nodep)
 *		Captures the current temporary-file context and attaches it
 *		to the shell function being defined.
 *	remember_temp (filename)
 *		Adds a filename to the current global list of temporary
 *		files (copies filename to private buffer).
 *	capture_temp ()
 *		Detaches the global list of temporary files and returns
 *		the list to the caller.
 *	unlink_temp (templist)
 *		Walks over the list of temporary files, unlinking the files
 *		and deallocating the list nodes.
 *	forget_temp (templist)
 *		Deallocates the memory for the list of temporary files.
 */

typedef struct temp_file TEMP_FILE;

struct temp_file {
	TEMP_FILE     *	tf_next;
	char	      *	tf_name;
};


/*
 * Shell functions.
 */
typedef struct shfunc {
	struct	shfunc	*fn_link;
	int		fn_hash;
	char		*fn_name;
	NODE		*fn_body;
	TEMP_FILE     *	fn_temp;
} SHFUNC;


/*
 * NB : The shell is required to permit redirections of built-ins and special
 * built-in operations in such a way that they are not run in subshells (since
 * they have to affect the top-level environment). To permit this, the
 * redirection code makes duplicates of file descriptors it overwrites and
 * records all the information necessary to reverse the effect of the
 * redirections on the global environment. This structure holds the "undo"
 * information. Note that "ru_newfd" may be set to -1 if the fd that was
 * redirected did not originally refer to an open file.
 *
 * Related functions:
 *	redirect (iovp, undo)
 *		The "undo" argument is an optional pointer to the head of a
 *		list of REDIR_UNDO entries.
 *	redirundo (undo)
 *		Processes a list of REDIR_UNDO entries.
 */

typedef struct redir_undo REDIR_UNDO;

struct redir_undo {
	REDIR_UNDO    *	ru_next;	/* link to next undo item */
	int		ru_oldfd;	/* fd changed by redirection */
	int		ru_newfd;	/* duplicate of original fd */
};


/*
 * Flags for controlling collect () in lex.c
 */

enum {
	CONSUME_BACKSLASH,	/*
				 * Allow back-slash quoting of anything and
				 * consume the backslashes.
				 */
	BACKSLASH_END,		/*
				 * Recognise backslash-quoting of the end
				 * character, but retain the backslash.
				 */
	NO_BACKSLASH,		/* backslashes are ordinary */
	NO_ERRORS		/* backslashes are ordinary, silent errors */
};
				

/*
 * Global variables.
 *	Run time and set time flags.
 */
extern 	char	shflags[];		/* Flag array */
extern	char	shfnams[];		/* Flag name array */

#define eflag	shflags[0]		/* Exit on non-zero status */
#define kflag	shflags[1]		/* All assignments into environ */
#define nflag	shflags[2]		/* Do not execute commands */
#define pflag	shflags[3]		/* Unknown */
#define tflag	shflags[4]		/* Exit after read/exec one command */
#define uflag	shflags[5]		/* Unset variables are errors */
#define vflag	shflags[6]		/* Print input as read */
#define xflag	shflags[7]		/* Print commands as executed */

#define cflag	shflags[8]		/* Execute argument as command */
#define iflag	shflags[9]		/* Be interactive shell */
#define rflag	shflags[10]		/* Restricted shell */
#define sflag	shflags[11]		/* input<&0, output>&2 */

#define lgnflag	shflags[12]		/* Login shell */
#define no1flag	shflags[13]		/* Invoked shell */
#define bckflag	shflags[14]		/* Background process */
#define nllflag shflags[15]		/* Forked shell */
#define cmdflag	shflags[16]		/* Command file processor */

/* Shell status housekeeping */
extern	jmp_buf	restart;		/* Restart execution context */
extern	SHFUNC	*sh_fnp;		/* User-defined shell function list */
extern	int	in_sh_fn;		/* Executing shell function */
extern	int	ret_done;		/* "return" executed */
extern	int	sargc;			/* Argument count to shell */
extern	char	*sarg0;			/* Name shell called with */
extern	char	**sargv;		/* Argument vector to shell */
extern	char	**senvp;		/* Environment vector to shell */
extern	char	**sargp;		/* Current argument to shell */
extern	char	*scmdp;			/* Path/filename of command file */
extern	int	nargc;			/* New argument count, for command */
extern	char	**nargv;		/* New argument vector, for command */
extern	char	**nenvp;		/* New environment, for command */
extern	char	**niovp;		/* Io redirection, for command */
extern	NODE	 *nctlp;		/* Control node, for command */
extern	SES	*sesp;			/* Current session structure */
extern	int	yyline;			/* Yacc command line counter */
extern	int	shpid;			/* Shell's process id */
extern	int	slret;			/* Exit status of last command */
extern	int	sback;			/* Pid of last background command */
extern	int	spipe;			/* Pid of last pipe process */
extern	int	ufmask;			/* File creation mask */
/* Other internal flags. */
extern	int	comflag;		/* Command line is complete?? */
extern	int	errflag;		/* Set by call to printe */
extern	int	keyflag;		/* Look for keyword in next word */
extern	int	noeflag;		/* Turn off errors */
extern	int	prpflag;		/* Do prompt before next read */
extern	int	readflag;		/* Doing read command? */
/* Parameters used by shell. */
extern	char	*vhome;			/* Home directory */
extern	char	*vpath;			/* Search pathname */
extern	char	*vmail;			/* Mail file */
extern	char	*vps1;			/* Prompt string 1 */
extern	char	*vps2;			/* Prompt string 2 */
extern	char	*vifs;			/* Internal field seperators */
extern	char	*vshell;		/* Shell to use */
/* Shell data */
extern	char	strt[STRSIZE];		/* String buffer */
extern	char	*strp;			/* Pointer in string buffer */
extern	VAR	*varp;			/* Pointer to shell variables */
extern	char	*dstack[DSTACKN];	/* Directory stack */
extern	int	dstkp;			/* Directory stack pointer */

/* Tables */
extern	char	lextab[];		/* Character type table */
/* Data external to shell */
extern	int	errno;			/* System call error number */
extern	char	*signame[];		/* Wait status message table */

/*
 * Functions.
 */
extern	char	*shtmp();		/* in main.c */
extern	char	*salloc();		/* in alloc.c */
extern	char	*balloc();		/* in alloc.c */
extern	char	*duplstr();		/* in alloc.c */
extern	char	**makargl();		/* in alloc.c */
extern	char	**addargl();		/* in alloc.c */
extern	BUF	**savebuf();		/* in alloc.c */
extern	char	**vdupl();		/* in alloc.c */
extern	char	*gany();		/* in glob.c */
extern	int	sigintr();		/* in trap.c */
extern	VAR	*findvar();		/* in var.c */
extern	VAR	*setsvar();		/* in var.c */
extern	VAR	*flagvar();		/* in var.c */
extern	VAR	*assnvar();		/* in var.c */
extern	char	*convvar();		/* in var.c */
extern	char	**envlvar();		/* in var.c */
extern	char	*_getwd();		/* in /lib/libc.a */
#define	index(cp, c)	strchr((cp), (c))
#define	rindex(cp, c)	strrchr((cp), (c))

/* NB: new stuff for shell functions */
extern	TEMP_FILE     *	capture_temp ();	/* main.c */
extern	void		unlink_temp ();		/* main.c */
extern	void		cleanup_shell_fns ();	/* exec3.c */

#ifdef	ALLOC_DEBUG
struct alloc_debug {
	char	      *	name;
	int		count;
	int		maxcount;
};
# define	ALLOC_COUNT(x)		struct alloc_debug x##debug = { #x };
# define	ALLOC_ALLOC(x)		alloc_alloc (& x##debug);
# define	ALLOC_FREE(x)		x##debug.count --;
#
#else
# define	ALLOC_COUNT(x)
# define	ALLOC_ALLOC(x)
# define	ALLOC_FREE(x)
#endif

/* end of sh/sh.h */
