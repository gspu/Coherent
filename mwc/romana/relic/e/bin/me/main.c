/*
 * This program is in public domain; written by Dave G. Conroy.
 * This file contains the main driving routine, and some keyboard processing
 * code, for the MicroEMACS screen editor.
 * Modified by W. L. Sheldon for use with the COHERENT Operating System -
 * VT100 terminal key bindings.
 */
 
#include	<stdio.h>
#include	"ed.h"

#if	VMS
#include	<ssdef.h>
#define	GOOD	(SS$_NORMAL)
#define	BAD	(SS$_ABORT)
#endif

#ifndef	GOOD
#define	GOOD	0
#define	BAD	1
#endif

int	currow;				/* Working cursor row		*/
int	curcol;				/* Working cursor column	*/
int	curgoal;			/* Goal column			*/
BUFFER	*curbp;				/* Current buffer		*/
WINDOW	*curwp;				/* Current window		*/
BUFFER	*bheadp;			/* BUFFER listhead		*/
WINDOW	*wheadp;			/* WINDOW listhead		*/
BUFFER	*blistp;			/* Buffer list BUFFER		*/
#if	LIBHELP
BUFFER	*helpbp;			/* Help buffer			*/
#endif
BUFFER	*errbp;				/* Error file BUFFER		*/
uchar	pat[NPAT];			/* Pattern			*/

uchar	errfile[NFILEN];	/* Error file name		*/

#if	LIBHELP
uchar	*helpfile=0;			/* Help file name ptr.		*/
uchar	*helpindex=0;			/* Help index file name ptr.	*/
uchar	hfname[NFILEN];			/* Help file name place		*/
uchar	hiname[NFILEN];			/* Help index file name place	*/
#endif
extern  char	*strcpy();		/* copy string */
extern  char	*getenv();

/*
 * File-name list for command line...
 */
uchar	*cfiles[NCFILES];		/* Command line specified files	*/
int	cfilecnt;			/* File count...		*/

/*
 * Command line switch flags...
 */
unsigned int	runswitch;

#if	LK201
/*
 * Mapping table for all of the funny keys with the numeric parameters
 * on the LK201.
 * Indexed by the code,	which is between 0 (unused) and 34 (F20).
 * An entry of 0 means no mapping.  The map goes to command keys.
 * If I had a "special" bit, I could use the code in the escape sequence
 * as a key code, and return (for example) "do" as SPECIAL + 29.
 * Then the dispatch would be done by the default keymap. This is probably a
 * better way to go.
 */
short	lkmap[]	= {
#if	EXKEYS
	0,
	FN18,				/* 1	Find			*/
	FN19,				/* 2	Insert here		*/
	FN1A,				/* 3	Remove			*/
	FN1B,				/* 4	Select			*/
	FN1C,				/* 5	Previous screen		*/
	FN1D,				/* 6	Next screen		*/
	0,
	0,
	0,
	FN17,				/* 10	Compose			*/
	0,
	FN3,				/* 12	Print screen		*/
	0,
	FN4,				/* 14	F4			*/
	0,
	0,
	FN6,				/* 17	F6			*/
	FN7,				/* 18	F7			*/
	FN8,				/* 19	F8			*/
	FN9,				/* 20	F9			*/
	FNA,				/* 21	F10			*/
	0,
	0,
	0,
	0,
	FNE,				/* 26	F14			*/
	0,
	FN15,				/* 28	Help			*/
	FN16,				/* 29	Do	C-X E		*/
	0,
	FN11,				/* 31	F17	C-X P		*/
	FN12,				/* 32	F18	C-X N		*/
	FN13,				/* 33	F19	C-X Z		*/
	FN14				/* 34	F20	C-X C-Z		*/
#else
	0,
	OBND|CTRL|'S',			/* 1	Find			*/
	OBND|CTRL|'Y',			/* 2	Insert here		*/
	OBND|CTRL|'W',			/* 3	Remove			*/
	OBND|CTRL|'@',			/* 4	Select			*/
	OBND|META|'V',			/* 5	Previous screen		*/
	OBND|CTRL|'V',			/* 6	Next screen		*/
	0,
	0,
	0,
	0,				/* 10	Compose			*/
	0,
	0,				/* 12	Print screen		*/
	0,
	0,				/* 14	F4			*/
	0,
	0,
	0,				/* 17	F6			*/
	0,				/* 18	F7			*/
	0,				/* 19	F8			*/
	0,				/* 20	F9			*/
	0,				/* 21	F10			*/
	0,
	0,
	0,
	0,
	0,				/* 26	F14			*/
	0,
	0,				/* 28	Help			*/
	OBND|PFX1|'E',			/* 29	Do	C-X E		*/
	0,
	OBND|PFX1|'P',			/* 31	F17	C-X P		*/
	OBND|PFX1|'N',			/* 32	F18	C-X N		*/
	OBND|PFX1|'Z',			/* 33	F19	C-X Z		*/
	OBND|PFX1|CTRL|'Z'		/* 34	F20	C-X C-Z		*/
#endif
};
#endif

main(argc, argv)
uchar	*argv[];
{
	register int	c;
	register int	f;
	register int	n;
	uchar		bname[NBUFN];

#if	MSDOS
	setkeys();
#endif
#if	IBM
	vidnit();
#endif
	for (c = 0; c < MAXREB; c++)	/* nothing in the new table */
		bind.table[c].k_synonym = bind.table[c].k_code = -1;
	runswitch = 0;				/* Initialize the switches */
	bind.ffold = FALSE;			/* initialize the fold flg */
	bind.bracket = 1;			/* initialize bracket mode */
	bind.pfx1 = CTRL|'X';			/* initialize prefix keys */
	bind.pfx2 = bind.pfx3 = -1;
	bind.repeat = CTRL|'U';
	argproc(argc, argv);			/* Parse the arg list	*/
#if	GEM
	if (runswitch & CF_GRABMEM)		/* Get largest chunk of	*/
		grabmem(0, 0);			/* memory (ST only)	*/
#endif
#if	_I386
	{
		/*
		 * Speed up processing on 80386 for small files.
		 */
		char *junk;

		if (NULL != (junk = malloc(128L * 1024L)))
			free(junk);
	}
#endif
	topen();				/* Force the length setup */
	strcpy(bname, "main");			/* Work out the name of	*/
	if (cfilecnt > 0)			/* the default buffer.	*/
		makename(bname, cfiles[0]);	/* Make a buffer name	*/
	edinit(bname);				/* Buffers, windows.	*/
	vtinit();				/* Displays.		*/
	if (cfilecnt > 0) {			/* If there are files	*/
		update();			/* You have to update	*/
		readin(cfiles[0]);		/* in case "[New file]"	*/
	}
	if (cfilecnt > 1) {			/* If more than one	*/
		n = (term.t_nrow - cfilecnt - 1) / cfilecnt;
		for (c = 1; c < cfilecnt ; c++) { /* For all other files... */
			splitwind(0,0);		/* Split this window...	*/
			if ((f=curwp->w_ntrows-n) != 0)
				shrinkwind(0,f); /* Even out the windows */
			nextwind(0,0);		/* Go on to the next one */
			visitfile(cfiles[c]);	/* Read in that file	*/
		}
	}
	if ((runswitch & CF_ERROR) != 0) {
		splitwind(0,0);			/* Split this window	*/
		f = curwp->w_ntrows - ERRLINES;	/* Make error window small */
		if (f > 0)
			shrinkwind(0,f);
		readerr();
		nextwind();
		mlerase();
		update();
		nexterr(0,1);
		update();
	}
	lastflag = 0;							/* Fake last flags.	*/
	if (NULL != bind.macs[MAXMAC])		/* initialization macro */
		doMac(bind.macs + MAXMAC, FALSE, 1);

	for (;;) {
		update();				/* Fix up the screen	*/
		c = getbind(0);				/* Get a key		*/
		if (mpresf != FALSE) {			/* If a message there	*/
			mlerase();			/* get rid of it...	*/
			update();			/* Fix screen		*/
			if (c == ' ')			/* ITS EMACS does this	*/
				continue;		/*  (eat a space)	*/
		}
		f = FALSE;
		n = 1;
		if (c == bind.repeat) {			/* ^U, start argument	*/
			int	ctmp;

			f = TRUE;			/* We have a count	*/
			n = getnum("Arg", 4, &ctmp);	/* get the count	*/
			c = ctmp;			/* And get the last chr	*/
		}
		if (kbdmip != NULL) {			/* Save macro strokes.	*/
			if (kbdmip > (kbdm + ((NKBDM - 3)/2))) {
				ctrlg(FALSE, 0);
				continue;
			}
			if (f != FALSE) {
				*kbdmip++ = bind.repeat;
				*kbdmip++ = n;
			}
			*kbdmip++ = c;
		}
		bracketoff();
		execute(c, f, n);			/* Do it.		*/
	}
}

/*
 * Initialize all of the buffers and windows.
 * The buffer name is passed down as an argument, because the main routine may
 * have been told to read in a file by default, and we want the buffer name to
 * be right.
 */
edinit(bname)
uchar	bname[];
{
	register BUFFER	*bp;
	register WINDOW	*wp;

	bp = bfind(bname, TRUE, 0);		/* First buffer		*/
	blistp = bfind("[List]", TRUE, BFTEMP);	/* Buffer list buffer	*/
#if	LIBHELP
	helpbp = bfind("[Help]", TRUE, BFTEMP|BFHELP);	/* Help buffer	*/
#endif
	wp = (WINDOW *) malloc(sizeof(WINDOW));	/* First window		*/
	if (bp==NULL || wp==NULL || blistp==NULL)
		abort();
	curbp  = bp;				/* Make this current	*/
	wheadp = wp;
	curwp  = wp;
	wp->w_wndp  = NULL;			/* Initialize window	*/
	wp->w_bufp  = bp;
	bp->b_nwnd  = 1;			/* Displayed.		*/
	wp->w_linep = bp->b_linep;
	wp->w_dotp  = bp->b_linep;
	wp->w_doto  = 0;
	wp->w_markp = NULL;
	wp->w_marko = 0;
	wp->w_toprow = 0;
	wp->w_ntrows = term.t_nrow-1;		/* "-1" for mode line.	*/
	wp->w_force = 0;
	wp->w_flag  = WFMODE|WFHARD;		/* Full.		*/
}


/*
 * Read in a key.  Do the standard keyboard preprocessing.
 * Convert the keys to the internal character set.  On the LK201, which lacks
 * a reasonable ESC key, make the grave accent a meta key too; this is a fairly
 * common customization around Digital.  Also read and decode the arrow keys,
 * and other special keys. This is done in Rainbow mode; does this work on all
 * the terminals with LK201 keyboards?
 */
getkey()
{
	register int	c;
#if	LK201
	register int	n;
	for (;;) {
		if ((c = tgetc()) == AGRAVE)		/* Alternate M- prefix.	*/
			return (META | getCtl());
		if (c == METACH) {			/* M-, or special key.	*/
			if ((c = tgetc()) == '[') {	/* Arrows and extras.	*/
				switch (c = tgetc()) {
				case 'A':
					return (OBND | CTRL | 'P');
				case 'B':
					return (OBND | CTRL | 'N');
				case 'C':
					return (OBND | CTRL | 'F');
				case 'D':
					return (OBND | CTRL | 'B');
				}
				if (c >= '0' && c <= '9') {
					n = 0;
					do {
						n = 10*n + c - '0';
					} 
					while ((c = tgetc()) >= '0' && c <= '9');
					if (c == '~' && n <= 34 && (c = lkmap[n]))
						return (c);
				}
				continue;
			}
			if (c == 'O') {
				switch (tgetc()) {
				case 'P':		/* PF1 => M-X (Future)	*/
					return (OBND | META | 'X');
				case 'Q':		/* PF2 => C-Q		*/
					return (OBND | CTRL | 'Q');
				case 'R':		/* PF3 => C-S		*/
					return (OBND | CTRL | 'S');
				case 'S':		/* PF4 => C-R		*/
					return (OBND | CTRL | 'R');
				}
				continue;
			}
			return (META | toCtl(c));
		}
		break;
	}
#else
#if	VT100
	for (;;) {
		if ((c = tgetc()) == METACH) {	/* Apply M- prefix	*/
			if ((c = tgetc()) == '[') { /* Arrow keys.	*/
				switch (tgetc()) {
				case 'A':
					return (OBND | CTRL | 'P');
				case 'B':
					return (OBND | CTRL | 'N');
				case 'C':
					return (OBND | CTRL | 'F');
				case 'D':
					return (OBND | CTRL | 'B');
				case 'P':
					return (OBND | CTRL | 'D');  /* DEL key */
				case 'H':
					return (OBND | CTRL | 'A');  /* HOME key */
				case '1':
					switch (tgetc()) {
					case 'x':
						return (OBND | META | '?'); /* F1 help C/ASM word */
					case 'y':
						return (OBND | PFX1 | '2'); /* Alt-F1 open 2nd window */
					}
				case '2':
					switch (tgetc()) {
					case '4': 
						if ((tgetc()) == 'H')
							return (OBND | CTRL | 'E');  /* END key */
					case 'x': 						
							return (OBND | PFX1 | CTRL | 'V'); /*  new file F2 */
					}
				case 'V':
					return (OBND | META | 'V');     /* PGUP key */
				case 'U':
					return (OBND | CTRL | 'V');     /* PGDN key */
				case '3':
					switch (tgetc()) {
					case 'x':
						return (OBND | META | 'S');  /* F3 search forward */
					case 'y':
						return (OBND | META | '/');  /* ALT-F3 cont search */
					}
				case '4':
					tgetc();
					return (OBND | META | 'R');  /* F4 search backward */
				case '5':
					tgetc();
					return (OBND | META | '%');  /* F5 search & replace */
				case '6':				
					tgetc();
					return (OBND | PFX1 | 'N');  /* F6 next window */
				case '8':
					tgetc();
					return (OBND | CTRL | 'Z'); /* F8 save/exit */
				case '0':
					switch (tgetc()) {
					case 'x':
						return (OBND | PFX1 | '1');  /* F10 close other wndws */
					case 'y':
						return (OBND | META | 'J');
					}
				}
				continue;
			}
			if (c == 'O') { 	/* function keys */
				switch (tgetc()) {
				case 'P':		/* PF1 => M-X (Future)	*/
					return (OBND | META | 'X');
				case 'Q':		/* PF2 => C-Q		*/
					return (OBND | CTRL | 'Q');
				case 'R':		/* PF3 => C-S		*/
					return (OBND | CTRL | 'S');
				case 'S':		/* PF4 => C-R		*/
					return (OBND | CTRL | 'R');
				}
				continue;
			}
			return (META | toCtl(c));
		}
		break;
	}
#else
	if ((c = tgetc()) == METACH)		/* Apply M- prefix	*/
		return (META | getCtl());
#endif
#endif
	if (c == 0x0D && bind.autoindent)
		return (OBND | CTRL | 'J');
	if (c >= 0x00 && c <= 0x1F)			/* C0 control -> C-	*/
		return (CTRL | '@' | c);
	return (c);
}

/*
 * Apply control modifications to a key
 */
toCtl(c)
register int	c;
{
	if (c>='a' && c<='z')			/* Force to upper	*/
		return (c - 0x20);
	if (c>=0x00 && c<=0x1F)			/* C0 control -> C-	*/
		return (c | CTRL | '@');
	return (c);
}

getCtl()
{
	return (toCtl(tgetc()));
}

/*
 * Fancy quit command, as implemented by Norm.
 * If the current buffer has changed do a write current buffer and exit emacs,
 * otherwise simply exit.
 */
quickexit(f, n)
{
	if ((curbp->b_flag&BFCHG) != 0		/* Changed.		*/
	&& (curbp->b_flag&(BFTEMP|BFERROR|BFNOWRT)) == 0)
		/* Real and not R/O...	*/
		filesave(f, n);
	quit(f, n);				/* conditionally quit	*/
}

/*
 * Quit command.  If an argument, always quit.  Otherwise confirm
 * if a buffer has been changed and not written out.
 * Normally bound to "C-X C-C".
 */
quit(f, n)
{
	register int	s = FALSE;

	if (f != FALSE				/* Argument forces it.	*/
	|| anycb() == FALSE			/* All buffers clean.	*/
	|| (s=mlyesno("Quit")) == TRUE) {	/* User says it's OK.	*/
		vttidy();
#if	MSDOS
		resetkeys();
#endif
		if (f != FALSE || s != FALSE)
			exit(BAD);
		else
			exit(GOOD);
	}
#if	MSDOS
	setkeys();
#endif
	return (s);
}

/*
 * Get binding char. Turn prefix chars into or'ed bits.
 */
getbind(bound)
register int bound;
{
	register int c;

	if (((c = getkey()) == bind.pfx1) && !(bound & PFX1))
		return (PFX1 | toCtl(getbind(bound | PFX1)));
	if ((c == bind.pfx2) && !(bound & PFX2))
		return (PFX2 | toCtl(getbind(bound | PFX2)));
	if ((c == bind.pfx3) && !(bound & PFX3))
		return (PFX3 | toCtl(getbind(bound | PFX3)));
	return (c);
}

/*
 * Abort.
 * Beep the beeper.
 * Kill off any keyboard macro,
 * etc., that is in progress.
 * Sometimes called as a routine,
 * to do general aborting of
 * stuff.
 */
ctrlg(f, n)
{
	tbeep();
	if (kbdmip != NULL) {
		if (NULL != kbdm) {
			free(kbdm);
			kbdm = NULL;
		}
		kbdmip  = NULL;
	}
	return (ABORT);
}

#if	MSDOS
setkeys()					/* redefine cursor keys */
/* so that they make    */
/* sense to microEMACS  */
/* as described in IBM  */
/* DOS tech. reference  */
/* manual		*/
{
#if	!IBM
	static uchar *ctlseq[] = {
		"\033[0;72;16p",	/* up = <ctrl-p>        */
		"\033[0;77;6p",		/* right = <ctrl-f>     */
		"\033[0;75;2p",		/* left = <ctrl-b>      */
		"\033[0;80;14p",	/* down = <ctrl-n>      */
		"\033[0;81;22p",	/* pg dn = <ctrl-v>     */
		"\033[0;73;27;86p",	/* pg up = <esc>V       */
		"\033[0;71;27;60p",	/* home = <esc><        */
		"\033[0;79;27;62p",	/* end = <esc>>         */
		"\033[0;83;127p",	/* del = del            */
		"\033[0;3;27;46p",	/* <ctrl-@> = <exc>.    */
		NULL
	};
	register uchar **ctlp;

	for (ctlp = ctlseq; NULL != *ctlp; ctlp++)
		mlwrite(*ctlp);
#endif
}



resetkeys()					/* redefine cursor keys */
/* to default values    */
{
#if  !IBM
	static uchar *ctlseq[] = {
		"\033[0;72;0;72p",
		"\033[0;77;0;77p",
		"\033[0;75;0;75p",
		"\033[0;80;0;80p",
		"\033[0;81;0;81p",
		"\033[0;73;0;73p",
		"\033[0;71;0;71p",
		"\033[0;79;0;79p",
		"\033[0;83;0;83p",
		"\033[0;3;0;3p",
		NULL
	};
	register uchar **ctlp;

	for (ctlp = ctlseq; NULL != *ctlp; ctlp++)
		mlwrite(*ctlp);
#endif
}
#endif

argproc(argc, argv)
int argc;
uchar **argv;
{
	int i;
	uchar *flexn;
	extern uchar *flexName();
	uchar *ptr;

	if (!bind.tabsiz && 
	     (NULL == (ptr = getenv("TABSIZ")) ||
	      !(bind.tabsiz = atoi(ptr))))
		bind.tabsiz = 8;
	cfilecnt = 0;
	flexn = NULL;
	for (i = 1; i < argc; i++) {
		ptr = argv[i];		/* Get this argument...		*/
		if (*ptr == '-') {	/* Is this a switch?		*/
			switch (ptr[1]) {
			case 'd':
				runswitch |= CF_DEBUG;
				break;
			case 'e':
				runswitch |= CF_ERROR;
				if (ptr[2] == 0) {
					if (++i == argc) {
						runswitch &= ~CF_ERROR;
						continue;
					}
					strcpy(errfile, argv[i]);
				} 
				else {
					strcpy(errfile, &ptr[2]);
				}
				break;
			case 'f':
				if (ptr[2] == 0) {
					if (++i == argc)
						continue;
					flexn = argv[i];	
				} 
				else
				    flexn = ptr + 1;
				break;
#if	LIBHELP
			case 'h':		/* Alternate help */
				if (ptr[2] == 0) {
					if (++i == argc) {
						continue;
					}
					strcpy(hfname, argv[i]);
				} 
				else {
					strcpy(hfname, &ptr[2]);
				}
				strcpy(hiname, hfname);
#if	GEM || MSDOS
				strcat(hfname, ".hlp");
#endif
				strcat(hiname, ".idx");
				helpfile = hfname;
				helpindex = hiname;
				break;
#endif
			case 'w':
				runswitch |= CF_WARN;
				break;
#if	NATIVE && GEM
			case 'l':	/* long screen	*/
				runswitch |= CF_LONGSCR;
				break;
			case 't':	/* very long screen */
				runswitch |= CF_VLONG;
				break;
#endif
#if	GEM
			case 'x':	/* grab all memory */
				runswitch |= CF_GRABMEM;
				break;
#endif
			default:
				break;
			}
			/* Process this switch		*/
		} 
		else {		/* Otherwise...			*/
			if (cfilecnt >= NCFILES)
				continue;
			cfiles[cfilecnt++] = ptr;	/* This is a file. */
#if	GEM
			fixname(cfiles[cfilecnt-1]);
#endif
		}
	}
	if (NULL == flexn)
		loadBup(flexName(), TRUE);
	else
		loadBup(flexn, ABORT);
}

#if	EXKEYS
/*
 * Do nothing.  ("Dead")
 */
ignore()
{
	return TRUE;
}
#endif

/*
 * Get a numeric argument...
 */
getnum(prompt, n, lastc)
register uchar *prompt;
register int n;
register int *lastc;
{
	register int mflag = 0;
	register int c;

	mflag = 0;			/* that can be discarded. */
	mlwrite("%s: %d", prompt, n);
	while ((c = getbind(0)) >='0' && c<='9' 
	    || c==bind.repeat || c=='-'){
		if (c == bind.repeat)
			n = n*4;
		/*
				 * If dash, and start of argument string, set arg.
				 * to -1.  Otherwise, insert it.
				 */
		else if (c == '-') {
			if (mflag)
				break;
			n = 0;
			mflag = -1;
		}
		/*
				 * If first digit entered, replace previous argument
				 * with digit and set sign.  Otherwise, append to arg.
				 */
		else {
			if (!mflag) {
				n = 0;
				mflag = 1;
			}
			n = 10*n + c - '0';
		}
		mlwrite("%s: %d", prompt, (mflag >=0) ? n : (n ? -n : -1));
	}
	*lastc = c;		/* Return the terminal char.	*/
	/*
		 * Make arguments preceded by a minus sign negative and change
		 * the special argument "^U -" to an effective "^U -1".
		 */
	if (mflag == -1) {
		if (n == 0)
			n++;
		n = -n;
	}
	return n;
}

#if	GEM
/*
 * The following routine gets around a problem with GEMDOS Malloc(),
 * it forces a single, very large Malloc() so very large files can
 * be read.
 * It is available only on the Atari ST, and is bound to M-+
 * it can also be specified using the -x switch
 */
#include <osbind.h>

grabmem(f, n)
int f, n;
{
	extern char *lmalloc();
	register char *p = NULL;
	register long t;

	t = Malloc(-1L);			/* How big is free memory? */
	while (p == NULL) {			/* Until we have a block */
		t -= 4096L;			/* shrink the block */
		if (t < 2048) {			/* if too small, tell user */
			mlwrite( "[Cannot allocate memory]" );
			return 1;		/* and fail	*/
		}
		p = lmalloc(t);			/* Try to get this chunk */
	}					/* loop until success or fail */
	free(p);				/* return chunk to arena */
	mlwrite( "[Allocated %ld bytes]", t );	/* tell user how much */
	return 0;				/* and return success */
}
#endif
