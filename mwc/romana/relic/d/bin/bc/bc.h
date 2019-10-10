/*
 *	Secondary inclusions.
 */

#include <mprec.h>	/* multi-precision integer declarations */
#include "bcmch.h"	/* bc-machine definitions */
#include "bcsymtbl.h"	/* symbol table definitions */


/*
 *	Manifest constants.
 */

#define	TRUE		(0 == 0)
#define	FALSE		(0 != 0)
#define	MAXWORD		80		/* maximum token length */
#define	MAXSTRING	250		/* maximum quoted-string length */
#define	MAXSTACK	200		/* max pseudo-machine run-time stack */
#define	MAXCODE		1000		/* maximum function code size */
#define	ABUMP		8		/* array growth quantum */
#define	LINELEN		72		/* maximum output line length */
#define NOSHELL		127		/* system return if no shell */

/*
 *	The nel macro returns the number of elements in the array
 *	"array".  It will be a compile time constant.
 */

#define nel(array)	((sizeof (array)) / (sizeof *(array)))

/*
 *	The following macros are used to place items in the code
 *	stream of the bc-machine.
 */

#define	emitop(op)		(incloc()->opcode = (op))
#define	emitzap			(incloc()->address = -1, loc - 1)
#define	emitaddr(addr)		(loc->address = (addr) - loc, incloc())
#define	emitnum(num)		(incloc()->lvalue = (num))
#define emitarry(arry)		(incloc()->alvalue = (arry))
#define emitstr(str)		(incloc()->svalue = (str))
#define emitcnt(cnt)		(incloc()->ivalue = (cnt))
#define emitid(ident)		(incloc()->dvalue = (ident))
#define patch(target, addr)	((target)->address = (addr) - (target))

/*
 *	Global functions that return non-ints.
 */

rvalue		*getnum();		/* read number from stdin */
rvalue		*select();		/* select an array element */
code		*incloc();		/* return next slot and advance loc */
mint		*pow10();		/* return 10 to a power */
opcode		negate();		/* negate condition of branch */
stkent		*pauto();		/* initialize autos used in a bc fnc.*/
stkent		*newframe();		/* frame value after a function call */
char		*realloc();		/* call in dc.c and bcmutil.c */

/*
 *	Global variables.
 */

extern int	allok;			/* True iff no syntax errors */
extern int	scale;			/* scale register */
extern int	ibase;			/* input base */
extern dicent	*dictionary;		/* root of string table */
extern code	cstream[];		/* code stream */
extern code	*loc;			/* where next code item goes */
extern mint	ten;			/* constant ten */
extern mint	maxsobase;		/* max small output base (16) */
extern mint	outbase;		/* output base */
extern rvalue	dot;			/* last number printed */
extern rvalue	zero;			/* constant zero */
extern FILE	*infile;		/* current input file */
extern int	inline;			/* current input line */
extern char	*infnam;		/* current file name */
