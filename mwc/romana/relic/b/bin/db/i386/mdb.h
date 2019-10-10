/*
 * db/i386/mdb.h
 * A debugger.
 * i386-specific header.
 * i386/mdb.h contains definitions used by machine-independent code.
 * i386/mdbdefs.h defines externals referenced by machine-independent code.
 * i386/i386db.h contains definitions used only by machine-dependent code.
 */

#define	MCHNAME	"i386"			/* machine name			*/

/* Conditional compilation switches. */
#define	NOCANON	1			/* no canonization code		*/
/* #define NOFP	1			   if no FP desired		*/

/* Types. */
typedef	unsigned long	ADDR_T;		/* Target machine address	*/
typedef	unsigned long	WORD_T;		/* Target word 			*/
typedef char		BIN[1];		/* Breakpoint instruction size	*/

/*
 * Constants.
 */
#define MAX_ADDR ((ADDR_T)0xFFFFFFFFL)	/* Maximum machine address	*/
#define MIN_ADDR ((ADDR_T)0)		/* Minimum machine address	*/

/*
 * Formats.
 */
#define	INLEN		1		/* Size of smallest instruction	*/
#define ADDR_FMT	"%08lX"		/* Display address format	*/
#define ADDR16_FMT	"%04lX"		/* Display 16-bit address fmt	*/
#define	DDCHR		'x'		/* Default debugger format	*/

/*
 * Host sizeof(int), the byte count for ptrace() read/write calls.
 */
#define	PTSIZE	4

/* end of db/i386/mdb.h */
