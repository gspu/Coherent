/*
 * A stream editor.
 * Header file.
 */
#define HUGE	((unsigned)65535L)	/* A large integer */
#define	NBRC	9			/* Number of \( ... \) allowed */
#define LHPSIZE	512			/* Size of line, hold and pat buffer */
#define LNMSIZE	16			/* Size of label */
#define FNMSIZE	16			/* Size of file name */

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
 * A function for finding whether a character is a letter and a
 * function to switch case (lower to upper and vice versa).
 */
#define isallet(c)	(_ctype[(c)+1]&(_L|_N))
#define toother(c)	((c)^040)

/*
 * Address.
 */
typedef	struct	add {
	unsigned a_lno;			/* Line number */
	char	*a_pat;			/* Pattern */
} ADD;

/*
 * Structure for remembering \( ... \).
 */
typedef	struct {
	char	*b_bp;			/* Ptr to start of string matched */
	char	*b_ep;			/* Ptr to end of string matched */
} BRC;

/*
 * Commands.
 */
typedef struct com {
	struct	com *c_next;		/* Pointer to next */
	char	c_name;			/* Command name */
	char	c_nadd;			/* Number of addresses */
	char	c_notf;			/* Not flag */
	char	c_iran;			/* In range */
	ADD	c_a[2];			/* Line addesses */
	union {
		struct	com *p_com;	/* Command */
		struct	lab *p_lab;	/* Label */
		struct	sub *p_sub;	/* Substitute */
		struct	fil *p_fil;	/* File */
		char	*p_buf;		/* Buffer */
	}	c_p;			/* Parameters */
} COM;

/*
 * Argument list.
 */
typedef struct ecl {
	struct	ecl *e_next;		/* Pointer to next */
	char	*e_argp;		/* Argument pointer */
} ECL;

/*
 * Files.
 */
typedef	struct	fil {
	struct	fil *f_next;		/* Pointer to next */
	char	f_name[FNMSIZE];	/* Pointer to file name */
	FILE	*f_filp;		/* File pointer */
} FIL;

/*
 * Labels.
 */
typedef	struct	lab {
	struct	lab *l_next;		/* Pointer to next */
	char	l_name[LNMSIZE];	/* Name */
	struct	com *l_comp;		/* Address */
	int	l_refc;			/* Reference count */
} LAB;

/*
 * Substitute.
 */
typedef struct sub {
	char	*s_pat;			/* Pattern */
	char	*s_rep;			/* Replacement */
	int	s_nth;			/* N'th */
	int	s_cop;			/* Copy flag */
	struct	fil *s_fil;		/* File to write to */
} SUB;

/*
 * Queue.
 */
typedef struct qcl {
	struct	que *q_next;		/* Pointer to next */
	struct	com *q_comp;		/* Pointer to command */
} QCL;

/*
 * Global variables.
 */
extern	FILE	*ifp;			/* Input file pointer */
extern	BRC	brcl[1+NBRC];		/* For remembering \( \) */
extern	COM	*comp;			/* Pointer to command list */
extern	COM	**compp;		/* Ptr to add next element */
extern	ECL	*eclp;			/* Argument list pointer */
extern	FIL	*filp;			/* Pointer to file list */
extern	LAB	*labp;			/* Label list */
extern	QCL	*qcbp;			/* For stacking braces */
extern	int	pattlen;		/* Length of pattern */
extern	int	holdlen;		/* Length of hold buffer */
extern	int	dolflag;		/* Last line in file */
extern	int	addnone;		/* Number was not found */
extern	int	nflag;			/* Don't copy to output */
extern	int	sflag;			/* Case insensitive */
extern	int	nerr;			/* Number of errors */
extern	int	lno;			/* Current line number */
extern	char	linebuf[LHPSIZE];	/* Line buffer */
extern	char	holdbuf[LHPSIZE];	/* Hold buffer */
extern	char	pattbuf[LHPSIZE];	/* Pattern buffer */
extern	char	*ncp;			/* Pointer in input */

extern	char	*salloc();		/* Memory allocation function */
