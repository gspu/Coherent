/*
 * Screen control routines for termcap.
 * A good example of how to use termcap.
 * For more information get
 *	Termcap & Terminfo
 *	O'Reilly & Associates, Inc.
 *	ISBN 0-93717522-6
 */
#include <stdio.h>
#include <sgtty.h>
#include "ed.h"

#if TERMCAP

#define NROW	24	/* default rows on the screen */
#define NCOL	80	/* default columns on the screen */
#define BEL	0x07
#define ESC	0x1B

#define TERMBUF 1024	/* the largest buffer termcap can use */

extern int	ttopen();
extern int	ttgetc();
extern int	ttputc();
extern int	ttflush();
extern int 	ttclose();
extern int 	tcapmove();
extern int 	tcapeeol();
extern int	tcapeeop();
extern int	tcapbeep();
extern int	tcapopen();
extern int	tcapstand();
extern int	tput();
extern char	*tgoto();
extern char	*tgetstr();
extern char	*realloc();

/* pointers to various tcap strings */
static char	*CM, *CE, *CL, *SO, *SE;
static uchar	*ptr;	/* pointer to first free spot in termcap buffer */


TERM term = {
	NROW-1,
	NCOL,
	tcapopen,
	ttclose,
	ttgetc,
	ttputc,
	ttflush,
	tcapmove,
	tcapeeol,
	tcapeeop,
	tcapbeep,
	tcapstand
};

/*
 * Get a required termcap string or exit with a message.
 */
static uchar *
qgetstr(ref)
register uchar *ref;
{
	register uchar *tmp;

	if (NULL == (tmp = tgetstr(ref, &ptr))) {
		printf("/etc/termcap must have a '%s' entry\n", ref);
		exit(1);
	}
	return (tmp);
}

/*
 * Get termcap information for this terminal type
 */
tcapopen()
{
	 /* buffer for the strings we need to keep */
	static uchar 	*tcapbuf = NULL;
	uchar tcbuf[TERMBUF];	/* address saved by termcap for tgetstr */

	if (NULL != tcapbuf) {	/* we've been here before */
		ttopen();
		return;
	}

	/*
	 * Set up termcap type.
	 */
	{
		extern char *getenv();
		uchar *tv_stype;

		if ((tv_stype = getenv("TERM")) == NULL) {
			puts("Environment variable TERM not defined!");
			exit(1);
		}

		if (tgetent(tcbuf, tv_stype) != 1) {
			printf("Unknown terminal type %s!\n", tv_stype);
			exit(1);
		}
	}

	/*
	 * Get termcap entries for later use.
	 */
	{
		extern short ospeed;	/* termcap's device speed */
		extern char PC;		/* termcap's pad character */
		char *p;
		struct sgttyb tty;

		ospeed = 0;	/* set terminal output speed */
		if (isatty(fileno(stdout))) {
			gtty(fileno(stdout), &tty);
			ospeed = tty.sg_ospeed;
		}

		/* get far too much space and shrink later */
		if (NULL == (ptr = tcapbuf = malloc(TERMBUF)))
			abort();

		/*
		 * Get required entries. There must be cd= clear after cursor
		 * or cl= clear screen
		 */
		if (NULL == (CL = tgetstr("cl", &ptr)))
			CL = qgetstr("cd");
	
		CM = qgetstr("cm");	/* move cursor to row, col */
		CE = qgetstr("ce");	/* clear to end of line */

		/* Get optional entries. */
		SO = tgetstr("so", &ptr); /* begin standout mode */
		SE = tgetstr("se", &ptr); /* end standout mode */

		/* set termcap's pad char */
		PC = ((NULL == (p = tgetstr("pc", &ptr))) ? 0 : *p);

		/*
		 * check that realloc truncates buffer in place.
		 */
		if (tcapbuf != realloc(tcapbuf, (unsigned)(ptr - tcapbuf)))
		{
			puts("Buffer not shrunk in place!\n");
			exit(1);
		}
	}

	/*
	 * Get the number of lines and collumns for the terminal.
	 * Leave NCOL and NROW if data is not there.
	 */
	{
		int i;
		extern int tgetnum();

		if (0 < (i = tgetnum("co")))
			term.t_ncol = i;
		if (0 < (i = tgetnum("li")))
			term.t_nrow = i - 1;
	}

	ttopen();
}

/*
 * Move to row and collum.
 */
tcapmove(row, col)
register int row, col;
{
	putpad(tgoto(CM, col, row));
}

/*
 * Clear to end of line.
 */
tcapeeol()
{
	putpad(CE);
}

/*
 * Clear screen. The CL command may contain either clear screen cl=
 * or clear from cursor to end of screen cd=.
 */
tcapeeop()
{
	tcapmove(0, 0);
	putpad(CL);
}

/*
 * Say beep.
 */
tcapbeep()
{
	ttputc(BEL);
}

/*
 * output string, set padding to one line affected.
 */
putpad(str)
uchar	*str;
{
	tputs(str, 1, ttputc);
}

/*
 * if (f)
 *	Put terminal in standout, if possible.
 * else
 *	Take terminal out of standout, if possible.
 *
 * Used for status line standout mode.
 */
tcapstand(f)
{
	register char *msg;

	if (NULL != (msg = (f ? SO : SE)))
		putpad(msg);
}
#endif TERMCAP
