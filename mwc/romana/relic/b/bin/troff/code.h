/*
 * code.h
 * Nroff/Troff.
 * Data structure for storing character codes and stream directives.
 */

/*
 * Stream directives.
 */
#define DNULL	0			/* Null				*/
#define DHMOV	(-1)			/* Move horizontally		*/
#define DVMOV	(-2)			/* Move vertically		*/
#define DFONT	(-3)			/* Change font			*/
#define DPSZE	(-4)			/* Change pointsize		*/
#define DSPAR	(-5)			/* Space down and return	*/
#define DPADC	(-6)			/* Paddable character		*/
#define DHYPH	(-7)			/* Place to hyphenate		*/
#define	DHYPC	(-8)			/* Hyphen character		*/
#define	DTRAB	(-9)			/* Transparent buffer		*/
#define	DFPOS	(-10)			/* Fix position			*/
#define	DHLIN	(-11)			/* Horizontal line		*/
#define	DVLIN	(-12)			/* Vertical line		*/

/*
 * All characters and commands are stored in this structure.
 * To identify whether an element is a character or a command,
 * call the function ifcchar(element) which returns 1 if the
 * element is a character code and 0 if the element is a command.
 */
typedef	union	{
	struct c_arg {			/* Structure containing character */
		int	c_code;		/* Character code		*/
		unsigned c_move;	/* Distance to move after char	*/
	} c_arg;
	struct l_arg {			/* Command with one int argument */
		int	c_code;		/* Type of command		*/
		int	c_iarg;		/* Int parameter		*/
	} l_arg;
	struct b_arg {			/* Command with buffer ptr. (dag)*/
		int	c_code;		/* Type of command		*/
		char	*c_bufp;	/* Pointer to buffer		*/
	} b_arg;
} CODE;

/*
 * Macros testing whether an element is a character code
 * or a stream directive.
 */
#define is_char(c)	((c) > 0)
#define is_dir(c)	((c) < 0)

/*
 * Global variables.
 */
extern	CODE	codeval;		/* Diversion struct returned by getl */

/* end of code.h */
