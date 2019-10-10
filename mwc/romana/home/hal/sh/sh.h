/*
 * Bourne shell.
 */

#include <common/_tricks.h>
#include <sys/compat.h>
#include <sys/types.h>
#include <setjmp.h>
#include <string.h>

#define STRSIZE	1000			/* Maximum length of single token */
#define IALSIZE	8			/* Initial argument list size */


/*
 * Masks for looking up lextab.
 */

#define MNQUO	0x01		/* Special in argument */
#define MDQUO	0x02		/* Special in double quotes */
#define MHERE	0x04		/* Special in here document */
#define MRVAR	(MBVAR | MDIGI)	/* Valid in regular variable name */
#define	MBQUO	0x08		/* Special in graves */
#define MDIGI	0x10		/* Is an ascii digit */
#define MBVAR	0x20		/* Valid 1st char in regular var name */
#define MNAME	0x40		/* Does not start a name */
#define MGLOB	0x80		/* Must call glob */

#define	SPECIAL_VAR_CHARS	"!#$*-0123456789?@"


/*
 * Determine if a character is a member of the given class. A value of -1
 * is interpreted specially.
 */

#define class(c, m)	((unsigned) (c + 1) <= (unsigned) (0x7F + 1) && \
			 (lextab [c + 1] & m) != 0)

/*
 * An improved system for lexical analysis and buffer management requires a
 * way of embedding extra structure into the lexical analyser output. While
 * coding this information printably is feasible, there are also virtues in
 * using simple counted buffers to allow embedded NUL characters. Here we
 * define a structure for representing counted strings in memory. Note that
 * most counted strings are still NUL-terminated, but that some kinds of
 * embedded structures (such as might result from $()) may recursively embed
 * other buffers containing their own terminators.
 *
 * I reject the use of flex-arrays or structures that do not permit simple
 * embedding, and in this case the solution of an extra pointer is not
 * appropriate. Therefore, here I declare an incomplete structure and some
 * accessors. For the uninitiated, this prevents inappropriate usage at the
 * cost of making the accessors less type-safe using unadorned C.
 *
 * Note that the stored length does not include the terminating NUL, while the
 * size does!
 */

typedef	struct counted_string	CSTRING;

#define	CSTRING_LENGTH(csp)	(* (size_t *) (csp))
#define	CSTRING_SIZE(csp)	(CSTRING_LENGTH (csp) + 1 + sizeof (size_t))
#define	CSTRING_STRING(csp)	((unsigned char *) ((size_t *) (csp) + 1))
#define	CSTRING_NEW(space)	((* (size_t *) space = 0), \
				 (CSTRING *) (space))
#define	CSTRING_ADD(csp,ch)	(void) (CSTRING_STRING (csp) \
					  [CSTRING_LENGTH (csp)] = (ch), \
					CSTRING_LENGTH (csp) ++)
#define	CSTRING_END(csp,cp)	(void) (* (size_t *) (csp) = \
						(unsigned char *) (cp) - \
						CSTRING_STRING (csp))

#if	USE_INLINE
LOCAL INLINE size_t (CSTRING_LENGTH) (CSTRING * _csp) {
	return CSTRING_LENGTH (_csp);
}
LOCAL INLINE size_t (CSTRING_SIZE) (CSTRING * _csp) {
	return CSTRING_SIZE (_csp);
}
LOCAL INLINE unsigned char * (CSTRING_STRING) (CSTRING * _csp) {
	return CSTRING_STRING (_csp);
}
#undef	CSTRING_LENGTH
#undef	CSTRING_SIZE
#undef	CSTRING_STRING
#endif


/*
 * Types of parse nodes.
 */

enum node_tag {

#define	PARSE_NODE(name,desc,tag,recur)	__CONCAT (N, name),
#include "node.tab"
#undef	PARSE_NODE

	NODE_MAX
};


/*
 * The following table is used to described the node content.
 */

enum {
	NULLP,				/* no union content */
	NODEP,				/* n_auxp */
	STRP,				/* n_strp */
	CSTRP,				/* n_cstrp */
	FILEP				/* n_file */
};
enum {
	NORM,				/* normal node linkage */
	LOOPS				/* looping node linkage */
};

extern struct nodeinfo {
	CONST char    *	ni_name;	/* node description */
	unsigned	ni_union : 4;	/* which union member */
	unsigned	ni_loops : 1;	/* special node linkage for loops */
} nodeinfo [];


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
	int		tf_refs;
};


/*
 * Node.
 */

typedef struct node	NODE;

struct node {
	unsigned short	n_type;		/* Type of node */
	NODE	      * n_next;		/* Pointer to next */
	union {
		NODE	      *	nu_auxp;	/* Auxiliary pointer */
		char	      *	nu_strp;	/* Pointer to string */
		CSTRING	      *	nu_cstrp;	/* Counted string */
		TEMP_FILE     *	nu_filep;	/* temporary file */
	} n_u;
};

#define	n_auxp	n_u.nu_auxp
#define	n_strp	n_u.nu_strp
#define	n_cstrp	n_u.nu_cstrp
#define	n_filep	n_u.nu_filep

#define	NODE_UNION_TAG(nodep)	(nodeinfo [(nodep)->n_type].ni_union)
#define	NODE_LOOPS(nodep)	(nodeinfo [(nodep)->n_type].ni_loops)

/*
 * Flags in variables.
 */

enum {
	VAR_EXPORT	= 1,		/* Variable is exported */
	VAR_READONLY	= 2,		/* Read only */
	VAR_SETTER	= 4		/* Variable is used by shell */
};


/*
 * Enviroment list and variable structure.
 */

typedef struct var	VAR;

struct var {
	int		v_flag;		/* Flags */
	VAR	      *	v_next;		/* Pointer to next entry */
	char	      *	v_strp;		/* Pointer to string */
};


/*
 * Buffer structure.
 */

typedef struct buf	BUF;

struct buf {
	BUF	      *	b_next;		/* Pointer to next */
	int		b_size;		/* Size of buffer */
};


/*
 * Per executing command data structure.
 * Used for break/continue.
 */

typedef struct con	CON;

struct con {
	CON	      *	c_next;		/* Pointer to next */
	NODE	      *	c_node;		/* Node which created this control */
	BUF	     **	c_bpp;		/* Current free buffer */
	jmp_buf		c_envl;		/* Enviroment list */
};

#define	DESTROY_CON(cp)		(freebuf ((cp)->c_bpp))

/*
 * Shell functions.
 */

typedef struct shfunc	SHFUNC;

struct shfunc {
	SHFUNC	      *	fn_link;	/* Next in list of all functions */
	CSTRING	      *	fn_name;
	NODE	      *	fn_body;	/* Parsed function statements */
};


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

typedef	struct ses	SES;

struct ses {
	SES	      *	s_next;
	int		s_type;
	int		s_flag;
	NODE	      *	s_node;
	CON	      *	s_con;
	BUF	     **	s_bpp;
	jmp_buf		s_envl;
	unsigned char *	s_strp;
	unsigned char**	s_argv;
	int		s_ifd;
	off_t		s_count;	/* characters in buffer */
	int		s_line;		/* line number */
};


/*
 * For now, we fix the size of a session buffer for SFILE and SSTR sessions.
 */

#define	SESBUFSIZ	512


/*
 * Session flags
 */

#define	INTERACTIVE	1
#define	SESSION_EOF	2
#define	SESSION_EOL	4


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
#define	RINTDIE	11	/* Fatal interrupt signal */


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
#define	IPARENT	7	/* Parent before fork */
#define	IWAIT	8	/* Parent before wait () */
#define	IWAITINT 9	/* From user "wait" */
#define	IDONE	10	/* Done waiting for children */
#define	IPIPE	11	/* In LHS of pipe sequence */
#define	IBUILTIN 12	/* in built-in command execution */


/*
 * Evaluation contexts.
 */

#define EWORD	0	/* Evaluate $N, `cmd`, strip quotes */
#define EHERE	1	/* Evaluate $N, `cmd`, ignore quotes */
#define EARGS	2	/* Evaluate $N, `cmd`, strip quotes, *?[, blanks */
#define EPATT	3	/* Evaluate for pattern match */


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
 * The waitc () function uses the flags below to control whether the sleep
 * for the child process is interruptible or not.
 */

enum {
	WAIT_NORMAL,
	WAIT_INTERRUPTIBLE
};


/*
 * Codes for controlling the behaviour of free_node ().
 */

enum {
	CLEAN,			/* temp files remove */
	DEALLOCATE,		/* remove temp and free node */
	DETACH			/* detach temp files only */
};


/*
 * Codes for controlling do_lex ()
 */

enum {
	LEX_NORMAL,
	LEX_REDIRS = 1,
	LEX_HERE = 2,
	LEX_QUOTED = 4,
	MAX_LEXFLAG
};

	
/*
 * Global variables.
 *	Run time and set time flags.
 */

extern struct flag {
	unsigned	f_char : 8;	/* flag name */
	unsigned	f_value : 1;	/* flag value */
	unsigned	f_startup : 1;	/* for startup only? */
	unsigned	f_obsolete : 1;	/* non-POSIX */
 } shflags [17];

#define eflag	shflags [0].f_value	/* Exit on non-zero status */
#define kflag	shflags [1].f_value	/* All assignments into environ */
#define nflag	shflags [2].f_value	/* Do not execute commands */
#define pflag	shflags [3].f_value	/* Unknown */
#define tflag	shflags [4].f_value	/* Exit after read/exec one command */
#define uflag	shflags [5].f_value	/* Unset variables are errors */
#define vflag	shflags [6].f_value	/* Print input as read */
#define xflag	shflags [7].f_value	/* Print commands as executed */

#define cflag	shflags [8].f_value	/* Execute argument as command */
#define iflag	shflags [9].f_value	/* Be interactive shell */
#define rflag	shflags [10].f_value	/* Restricted shell */
#define sflag	shflags [11].f_value	/* input<&0, output>&2 */

#define	noclobber_flag	shflags [12].f_value /* Don't overwrite files with > */
#define	expassign_flag	shflags [13].f_value /* auto-export assignments */
#define	nopathexp_flag	shflags [14].f_value /* disable pathname expansion */

#define	debug_flag	shflags [15].f_value /* shell debugging */
#define	udo_dammit	shflags [16].f_value /* turn off $(), $(()) */

extern	int	lgnflag;		/* Login shell */
extern	int	no1flag;		/* Invoked shell */
extern	int	cmdflag;		/* Command file processor */

/* Shell status housekeeping */

extern	jmp_buf	restart;		/* Restart execution context */
extern	SHFUNC	*sh_fnp;		/* User-defined shell function list */
extern	int	in_sh_fn;		/* Executing shell function */
extern	int	ret_done;		/* "return" executed */
extern	int	sargc;			/* Argument count to shell */
extern	char	*sarg0;			/* Name shell called with */
extern	char	**sargp;		/* Current argument to shell */
extern	char	*scmdp;			/* Path/filename of command file */
extern	int	nargc;			/* New argument count, for command */
extern	char	**nargv;		/* New argument vector, for command */
extern	char	**nenvp;		/* New environment, for command */
extern	char	**niovp;		/* Io redirection, for command */
extern	NODE	 *nctlp;		/* Control node, for command */
extern	SES	*sesp;			/* Current session structure */
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

/* Parameters used by shell. */

extern	CONST char    *	vhome;		/* Home directory */
extern	CONST char    *	vpath;		/* Search path */
extern	CONST char    *	vmail;		/* Mail file */
extern	CONST char    *	vps1;		/* Prompt string 1 */
extern	CONST char    *	vps2;		/* Prompt string 2 */
extern	CONST char    *	vifs;		/* Internal field seperators */


/* Shell data */

extern	VAR	*varp;			/* Pointer to shell variables */

#if	DIRECTORY_STACK

#define	DSTACKN	30			/* Directory stack depth */
extern	char	*dstack[DSTACKN];	/* Directory stack */
extern	int	dstkp;			/* Directory stack pointer */

#endif


/* Tables */

extern	unsigned char	lextab[];		/* Character type table */

/* Data external to shell */

extern	int	errno;			/* System call error number */


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
extern	void	sigintr();		/* in trap.c */
extern	VAR	*setsvar();		/* in var.c */
extern	VAR	*flagvar();		/* in var.c */
extern	char	**envlvar();		/* in var.c */
extern	char	*signame();		/* in trap.c */

/* NB: new stuff for shell functions */

extern	TEMP_FILE     *	capture_temp ();	/* main.c */
extern	void		unlink_temp ();		/* main.c */
extern	TEMP_FILE     *	dup_temp_files ();	/* main.c */
extern	void		cleanup_shell_fns ();	/* exec3.c */


/*
 * New-style lexer global.
 */

CSTRING	      *	sh_yytext;


/*
 * NIGEL: Some real prototypes, at last.
 */

EXTERN_C_BEGIN

char	      *	eval_buf	PROTO ((char * _inbuf, size_t _inbufsize,
					int _flag));
char	      *	eval_node	PROTO ((NODE * _nodep, int _flag));
CSTRING	      *	dupcstr		PROTO ((CSTRING * _cstrp, int _f));
void		free_node	PROTO ((NODE * np, int mode));
char	     **	addargl		PROTO ((char ** _app, VOID * _sp, int _cstr));
void		syntax		PROTO ((CONST char * _desc));
int		getn		PROTO ((void));
void		dump_cstring	PROTO ((CSTRING * _cstrp));
int		do_lex		PROTO ((int _c, int _flag));

void		lex_init	PROTO ((void));
char	      *	lex_done	PROTO ((void));
void		lex_add		PROTO ((unsigned char _c));
void		lex_add_string	PROTO ((CONST char * _str));
CONST char    *	ffind		PROTO ((CONST char * _paths,
					CONST char * _file, int _mode));
void		sfree		PROTO ((VOID * _cp));
void		vfree		PROTO ((char * CONST * _vecp, int _extra));
char	     **	vdupl		PROTO ((char * CONST * _vecp, int _extra));

NO_RETURN	reset		PROTO ((int _code));

REDIR_UNDO    *	remember_undo	PROTO ((REDIR_UNDO * _next, int save));
void		redirundo	PROTO ((REDIR_UNDO * _undo));
void		write_parse	PROTO ((NODE * _node));

int		namevar		PROTO ((CONST char * _name));
VAR	      *	findvar		PROTO ((CONST char * _name));
int		unsetvar	PROTO ((CONST char * _name));
CONST char    *	convvar		PROTO ((CONST VAR * _vp));
VAR	      *	assnvar		PROTO ((size_t _len, CONST char * _name,
					CONST char * _value));

int		ihash		PROTO ((CONST char * _str));

int		waitc		PROTO ((int _pid, int _intflag));
int		clone		PROTO ((void));
int		flexec		PROTO ((void));

#define	COMMAND_NORMAL		0
#define	COMMAND_NO_FORK		1
#define	COMMAND_TRUTH_CONTEXT	2

int		command		PROTO ((NODE * _np, int _flags));
int		shell_function	PROTO ((int _flags));

void		lex_push	PROTO ((void));
void		lex_pop		PROTO ((void));

int		push_session	PROTO ((int _type, VOID * _info,
					SES * _session));

int		special_builtin PROTO ((void));
int		shell_builtin	PROTO ((void));

void		ecantopen	PROTO ((CONST char * _file));
void		ecantfind	PROTO ((CONST char * _file));
void		ecantmake	PROTO ((CONST char * _file));
void		eillval		PROTO ((CONST char * _var));
void		eredir		PROTO ((CONST char * _desc));

EXTERN_C_END

#define	sfree(ptr)		free (ptr)

