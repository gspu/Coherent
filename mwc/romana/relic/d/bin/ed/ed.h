/*
 * An editor.
 * Header file.
 */

#define ULARGE	65535L			/* Largest unsigned value */
#define LNSIZE	2000			/* Initial size of line number table */
#define LBSIZE	512			/* Size of line buffer */
#define CBSIZE	512			/* Size of code buffer */
#define TBSIZE	CBSIZE			/* Size of temporary buffer */
#define SBSIZE	512			/* Replace substitute buffer size */
#define GBSIZE	512			/* Global command buffer size */
#define DBSIZE	512			/* Size of disk buffer cache */
#define BRSIZE	9			/* Number of \( ... \) allowed */
#define FNSIZE	64			/* File name size */
#define	CKSIZE	64			/* Length of crypt key */
#define MKSIZE	26			/* Size of mark line table */
#define PGSIZE	22			/* Page size */

/*
 * Functions for finding out whether a character is a letter and
 * to switch case from lower to upper and vice versa.
 * Knows about ctype.h internals.
 */
#define isallet(c)	(_ctype[(c)+1]&(_L|_N))
#define toother(c)	((c)^' ')

/*
 * Click size definitions.
 * These functions are used to get the block number and offset into
 * the block given a seek pointer gotten from the line number table.
 * It takes into account the size of the disk buffer (DBSIZE) and
 * the fact that the seek pointer may be off by one (caused by the
 * global command marking lines.
 */
#define CLSIZE		16
#define blockn(a)	((a)>>6)
#define offset(a)	(((a)&076) << 3)
#define linead()	(tmpseek >> 3)

/*
 * Definition for stream directives in regular expressions.
 * The order of these must not be changed.  In particular,
 * the `stream of' directives must immediately follow their
 * single counterparts.
 */
#define CSNUL	000			/* End of expression */
#define CSSOL	001			/* Match start of line */
#define CSEOL	002			/* End of line */
#define CSOPR	003			/* \( */
#define CSCPR	004			/* \) */
#define CSBRN	005			/* Match nth \( ... \) */
#define CSDOT	006			/* Any character */
#define CMDOT	007			/* Stream of any characters */
#define CSCHR	010			/* Match given character */
#define CMCHR	011			/* Match stream of given characters */
#define CSSCC	012			/* Char in single case */
#define CMSCC	013			/* Stream of chars in single case */
#define CSCCL	014			/* Character class */
#define CMCCL	015			/* Stream of character class */
#define CSNCL	016			/* Not character class */
#define CMNCL	017			/* Stream of not char class */

/*
 * Typedefs.
 */
typedef	unsigned LINE;			/* Typedef for line table */

/*
 * Structure for remembering \( ... \).
 */
typedef	struct {
	char	*b_bp;			/* Ptr to start of string matched */
	char	*b_ep;			/* Ptr to end of string matched */
} BRACE;

/*
 * External function declarations.
 */
extern	char	*calloc();
extern	FILE	*copen();
extern	char	*getdisk();
extern	char	*getenv();
extern	char	*getpass();
extern	char	*malloc();
extern	char	*match();
extern	int	readfil();
extern	int	readtty();
extern	char	*realloc();
extern	char	*strcpy();
extern	FILE	*xfopen();

/*
 * Global variables.
 */
extern	int	cflag;			/* Print character counts */
extern	int	mflag;			/* Allow multiple commands per line */
extern	int	pflag;			/* Editor prompts */
extern	int	oflag;			/* Behaves like the old editor */
extern	int	sflag;			/* Match patterns in single case */
extern	int	tflag;			/* Set up for screen editor */
extern	int	vflag;			/* Verbose error messages */
extern	int	intflag;		/* Interrupt has been hit */
extern	char	*tfn;			/* Temp file name */
extern	FILE	*tmp;			/* Temp file pointer */
extern	long	tmpseek;		/* Free space seek ptr in tmp file */
extern	int	rcurbno;		/* Current read block number */
extern	int	wcurbno;		/* Current write block number */
extern	LINE	*line;			/* Pointer to line table */
extern	int	lnsize;			/* Current size of line table */
extern	int	savechr;		/* Character that was ungetx'ed */
extern	int	lastchr;		/* Last character we read */
extern	char	*gcp;			/* Global input pointer */
extern	char	linebuf[LBSIZE];	/* Line buffer */
extern	char	codebuf[CBSIZE];	/* Code buffer */
extern	char	tempbuf[TBSIZE];	/* Temporary buffer */
extern	char	subsbuf[SBSIZE];	/* Substitute buffer */
extern	char	globbuf[GBSIZE];	/* Buffer for global command */
extern	char	rdbcbuf[DBSIZE];	/* Disk buffer cache */
extern	char	wdbcbuf[DBSIZE];	/* Write buffer cache */
extern	BRACE	brace[1+BRSIZE];	/* For remembering \( \) */
extern	char	file[FNSIZE+1];		/* Filename */
extern	LINE	marklin[MKSIZE];	/* Mark line table */
extern	int	dotadd;			/* Address of the current line */
extern	int	doladd;			/* Address of last line */
extern	char	vcom;			/* Verify command */
extern	int	saved;			/* File saved since last written */
extern	FILE	*fp;			/* File pointer for readfil */
extern	long	cct;			/* Number of chars read in append */
extern	long	lct;			/* Number of lines read in append */
extern	int	appflag;		/* In append mode */
extern	int	addspec;		/* Number of addresses specified */
extern	int	adderrr;		/* Error in computing address */
extern	int	addpage;		/* An ampersand was found */
extern	int	addques;		/* A question mark was found */
extern	int	subnths;		/* Which substitute wanted */
extern	int	subnewl;		/* A newline is being replaced */
extern	int	subseek;		/* Seek position of new line */
extern	int	suborig;		/* Seek position of old line */
extern	char	*errstr;		/* Pointer to last error message */
extern	char	*keyp;			/* Pointer to crypt key */
