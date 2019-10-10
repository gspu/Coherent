/*
 * File:	defer.c
 *
 * Purpose:	Handle deferring of functions and subsequent execution.
 *
 * $Log:	defer.c,v $
 * Revision 1.9  93/06/14  13:42:38  bin
 * Hal: kernel 78 update
 * 
 * Revision 1.1  92/11/09  17:08:40  root
 * Just before adding vio segs.
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define	DEFLIM	128	/* maximum number of deferred functions */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void defer();
void defend();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static void (*defunc[DEFLIM])();
static int defarg[DEFLIM];
static int defqix, defqox, defcnt;

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * defer()
 *
 *	Defer a function [usually from interrupt level].
 *	The deferred function takes a single int-sized arg.
 */
#define DEFDBG 1
#if DEFDBG
static int deftag[DEFLIM];

void
defer0(f,a,t)
void (*f)();
int a;
int t;
{
	int s=sphi();

	if (defcnt < DEFLIM) {		/* is there room in defer queue? */
		defunc[defqix] = f;
		defarg[defqix] = a;
		deftag[defqix] = t;
		if (++defqix >= DEFLIM)
			defqix = 0;
		defcnt++;
	} else {
		static foo;
		int *r=(int *)(&f);
		int tmpqox = defqox+DEFLIM-16;

		if (foo++ == 0) {
			printf("\nDefer overflow r=%x t =%x f=%x a=%x ",
			  *(r-1), t, f, a);
			do {
				tmpqox = (tmpqox+1) % DEFLIM;
				printf("\nt=%x f=%x a=%x ox=%d ",
				  deftag[tmpqox], defunc[tmpqox],
				  defarg[tmpqox], tmpqox);
			} while (tmpqox != defqox);
		}
	}
	spl(s);
}

void
defer(f,a)
void (*f)();
int a;
{
	defer0(f, a, 0);
}
#else
void
defer(f,a)
void (*f)();
int a;
{
	int s=sphi();

	if (defcnt < DEFLIM) {		/* is there room in defer queue? */
		defunc[defqix] = f;
		defarg[defqix] = a;
		if (++defqix >= DEFLIM)
			defqix = 0;
		defcnt++;
	} else {
		static foo;
		int *r=(int *)(&f);
		if (foo++ < 4)
			printf("\nDefer overflow r=%x f=%x a=%x ", *(r-1), f, a);
	}
	spl(s);
}
#endif

/*
 * defend()
 *
 *	Evaluate all deferred functions.
 *	Should be called periodically by busy-wait device drivers.
 */
void
defend()
{
	while (defcnt) {
		(*defunc[defqox])(defarg[defqox]); 
		if (++defqox >= DEFLIM)
			defqox = 0;
		defcnt--;
	}
}
