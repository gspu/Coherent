/*
 * Regular expressions.
 * Contains the definition of type `RE' plus
 * the interpreter ops, etc.
 */

/* Operators (order is important) */
#define	REEND	0		/* End of RE */
#define	STEND	1		/* Stack end */
#define	OR	2		/* Union of RE's */
#define	LPAR	3		/* Left parenthesis */
#define	RPAR	4		/* Right Parenthesis */
#define	CLOS	5		/* Closure */
#define	NECLOS	6		/* Non-empty closure */
#define	ZORO	7		/* Zero or one occurrences */
#define	CONC	8		/* any character - also pseudo op */
#define	DCONC	9		/* Any character - dual case compare */
#define	CCLASS	10		/* RE character class */
#define	DCCLASS	11		/* RE character class - dual case */
#define	BOL	12		/* Beginning of line */
#define	EOL	13		/* End of line */
#define	ANY	14		/* Any character */
#define	termop(x)	((x)>=CONC)
#define	postop(x)	((x)>=CLOS&&(x)<=ZORO)

/* Miscellany */
#define	NCCHAR	256		/* Number of characters in a character class */
#define	NBPC	8		/* Bits per character */
#define	NCLASS	((NCCHAR+NBPC-1)/NBPC)	/* Size of class */
#define	NRE	100		/* Max. number of elements on RE stack */

typedef struct RE {
	struct RE *r_next;
	int	r_op;
	union rebit {
		int	u_ival;
		char	*u_cptr;
		struct RE *u_re;
	}	r_left, r_right;
}	RE;

extern	char *reerror;
extern	redual;			/* Dual case mode */
extern	int	refull;		/* Full expression */
RE *reparse();
int	reget();
int	reunget();
