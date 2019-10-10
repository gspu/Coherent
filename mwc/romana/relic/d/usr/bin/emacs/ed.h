/*
 * This file is the general header file for all parts of the MicroEMACS
 * display editor.  It contains definitions used by everyone, and it
 * contains the stuff you have to edit to create a version of the editor for
 * a specific operating system and terminal.
 */
#ifdef	COHERENT
#define	V7	1			/* V7 UN*X or Coherent		*/
#define	VMS	0			/* VAX/VMS			*/
#define	CPM	0			/* CP/M-86			*/
#define	MSDOS	0			/* MS-DOS			*/
#define	GEM	0			/* GEMDOS			*/
#define	PROMPT	" Coherent MicroEMACS -- "
#define ANSI	0
#define VT52	0			/* VT52 terminal (Zenith).	*/
#define	VT100	1			/* Handle VT100 style keypad.	*/
#define	LK201	0			/* Handle LK201 style keypad.	*/
#define	RAINBOW	0			/* Use Rainbow fast video.	*/
#define TERMCAP 1			/* Use TERMCAP			*/
#define	NATIVE	0			/* Native ATARI ST screen	*/
#define	EXKEYS	0			/* Extended keys		*/
#endif

#ifdef	GEMDOS
#define	V7	0			/* V7 UN*X or Coherent		*/
#define	VMS	0			/* VAX/VMS			*/
#define	CPM	0			/* CP/M-86			*/
#define	GEM	1			/* GEMDOS			*/
#define	MSDOS	0			/* MS-DOS			*/
#define	UPPERNM	0			/* if 0 names in all lower case	*/
#define	PROMPT	" ST MicroEMACS 1.8 -- "
#define ANSI	0
#define VT52	0			/* VT52 terminal (Zenith).	*/
#define	VT100	0			/* Handle VT100 style keypad.	*/
#define	LK201	0			/* Handle LK201 style keypad.	*/
#define	RAINBOW	0			/* Use Rainbow fast video.	*/
#define TERMCAP 0			/* Use TERMCAP			*/
#define	NATIVE	1			/* Native ATARI ST screen	*/
#define	EXKEYS	1			/* Extended keys		*/
#endif

#if	MSDOS
#define	V7	0			/* V7 UN*X or Coherent		*/
#define	VMS	0			/* VAX/VMS			*/
#define	CPM	0			/* CP/M-86			*/
#define	GEM	0			/* GEMDOS			*/
#define	PROMPT	" DOS MicroEMACS V4.0.16 -- "
#ifndef	IBM
#define	ANSI	1			/* Use ANSI.SYS			*/
#else
#define ANSI	0			/* don't use ANSI.SYS		*/
#endif
#define VT52	0			/* VT52 terminal (Zenith).	*/
#define	VT100	0			/* Handle VT100 style keypad.	*/
#define	LK201	0			/* Handle LK201 style keypad.	*/
#define	RAINBOW	0			/* Use Rainbow fast video.	*/
#define TERMCAP 0			/* Use TERMCAP			*/
#define	NATIVE	0			/* Native ATARI ST screen	*/
#define	EXKEYS	1			/* Extended keys		*/
#endif

#ifndef	PROMPT
#define	PROMPT	" MicroEMACS -- "
#endif

/* 
 * NORMAL_CHARS will be 1 when the ctype.h problems are solved
 * for internationalization or when international characters are
 * not required.
 */
#define NORMAL_CHARS 0

#define	CVMVAS	1			/* C-V, M-V arg. in screens.	*/
#define	LIBHELP	1			/* Use help stuff		*/

#define  MAXMAC 10			/* Max # of macros bound to keybindings */
#define  MAXREB 50			/* Size of rebind table */
#define  HASHP  41			/* Prime for hashing main table */
#define	 BINDID ((MAXREB << 8) ^ (HASHP << 4) ^ MAXMAC)
#define	NCFILES	5			/* Max # of files on command line */
#define	NFILEN	80			/* # of bytes, file name	*/
#define	NBUFN	16			/* # of bytes, buffer name	*/
#define	NLINE	256			/* # of bytes, line		*/
#define	NKBDM	256			/* # of strokes, keyboard macro	*/
#define	NPAT	80			/* # of bytes, pattern		*/
#define	HUGE	1000			/* Huge number			*/
#define	NSRCH	128			/* undoable search command len	*/
#define	ERRLINES 3			/* error window lines displayed	*/

#define	AGRAVE	0x60			/* M- prefix,   Grave (LK201)	*/
#define	METACH	0x1B			/* M- prefix,   Control-[, ESC	*/
#define	CTMECH	0x1C			/* C-M- prefix, Control-\	*/
#define	EXITCH	0x1D			/* Exit level,  Control-]	*/
#define	CTRLCH	0x1E			/* C- prefix,	Control-^	*/
#define	HELPCH	0x1F			/* Help key,    Control-_	*/

#define	FALSE	0			/* False, no, bad, etc.		*/
#define	TRUE	1			/* True, yes, good, etc.	*/
#define	ABORT	2			/* Death, ^G, abort, etc.	*/

#define	FIOSUC	0			/* File I/O, success.		*/
#define	FIOFNF	1			/* File I/O, file not found.	*/
#define	FIOEOF	2			/* File I/O, end of file.	*/
#define	FIOERR	3			/* File I/O, error.		*/

#define	CFCPCN	0x0001			/* Last command was C-P, C-N	*/
#define	CFKILL	0x0002			/* Last command was a kill	*/


#if	EXKEYS
/*
 * The following codes should be bound to the function keys in the system
 * specific keyboard handling (mostly in termio.c).
 *
 * When used in the key table in main.c, the same functions should be
 * available through other key sequences.
 */
#define	FN0	0x80			/* Function key 0	*/
#define	FN1	0x81			/* Function key 1	*/
#define	FN2	0x82			/* Function key 2	*/
#define	FN3	0x83			/* Function key 3	*/
#define	FN4	0x84			/* Function key 4	*/
#define	FN5	0x85			/* Function key 5	*/
#define	FN6	0x86			/* Function key 6	*/
#define	FN7	0x87			/* Function key 7	*/
#define	FN8	0x88			/* Function key 8	*/
#define	FN9	0x89			/* Function key 9	*/
#define	FNA	0x8A			/* Function key 10	*/
#define	FNB	0x8B			/* Function key 11	*/
#define	FNC	0x8C			/* Function key 12	*/
#define	FND	0x8D			/* Function key 13	*/
#define	FNE	0x8E			/* Function key 14	*/
#define	FNF	0x8F			/* Function key 15	*/
#define	FN10	0x90			/* Function key 16	*/
#define	FN11	0x91			/* Function key 17	*/
#define	FN12	0x92			/* Function key 18	*/
#define	FN13	0x93			/* Function key 19	*/
#define	FN14	0x94			/* Function key 20	*/
#define	FN15	0x95			/* Function key 21	*/
#define	FN16	0x96			/* Function key 22	*/
#define	FN17	0x97			/* Function key 23	*/
#define	FN18	0x98			/* Function key 24	*/
#define	FN19	0x99			/* Function key 25	*/
#define	FN1A	0x9A			/* Function key 26	*/
#define	FN1B	0x9B			/* Function key 27	*/
#define	FN1C	0x9C			/* Function key 28	*/
#define	FN1D	0x9D			/* Function key 29	*/
#define	FN1E	0x9E			/* Function key 30	*/
#define	FN1F	0x9F			/* Function key 31	*/
#endif

#define	CTRL	0x0100			/* Control flag, or'ed in	*/
#define	META	0x0200			/* Meta flag, or'ed in		*/
#define	PFX1	0x0400			/* prefix 1 flag, or'ed in	*/
#define	PFX2	0x0800			/* prefix 2 flag, or'ed in	*/
#define	PFX3	0x1000			/* prefix 3 flag, or'ed in	*/
#define OBND	0x2000			/* original binding only	*/

typedef unsigned char uchar;

/*
 * Key bindings point into a function table which
 * contains function names.
 */
typedef struct KEYTAB KEYTAB;
struct	KEYTAB {
	short	k_code;			/* Key code			*/
	char	k_fun;		/* Function number */
	char 	k_synonym;	/* Next at this hash value */
};

typedef struct FUNTAB FUNTAB;
struct	FUNTAB {
	int	(*f_fp)();		/* Routine to handle it	*/
};

/*
 * flexable bindings data.
 */
typedef struct BIND {
	KEYTAB	table[MAXREB];		/* overlay keybind table	*/
	uchar	*macs[MAXMAC + 2];	/* list of kbd macros		*/
	short	maclen[MAXMAC + 2];	/* lengths of kbd macros	*/
	short	pfx1;			/* prefix 1 char		*/
	short	pfx2;			/* prefix 2 char		*/
	short	pfx3;			/* prefix 3 char		*/
	short	repeat;			/* repeat argument char		*/
	int	ffold;			/* Fold Flag			*/
	int	fillcol;		/* Current fill column		*/
	int	bracket;		/* Bracket Mode switch		*/
	int	tabsiz;			/* tabsize 			*/
	int	tabsize;		/* Tab size (0: use real tabs)	*/
	int	autoindent;		/* Autoindent			*/
	int	dispmode;		/* display char < ' ' directly  */
} BIND;
extern BIND bind;

/*
 * There is a window structure allocated for every active display window.
 * The windows are kept in a big list, in top to bottom screen order, with
 * the listhead at "wheadp".  Each window contains its own values of dot and
 * mark.  The flag field contains some bits that are set by commands to guide
 * redisplay; although this is a bit of a compromise in terms of decoupling,
 * the full blown redisplay is just too expensive to run for every input
 * character. 
 */
typedef	struct	WINDOW {
	struct	WINDOW *w_wndp;		/* Next window			*/
	struct	BUFFER *w_bufp;		/* Buffer displayed in window	*/
	struct	LINE *w_linep;		/* Top line in the window	*/
	struct	LINE *w_dotp;		/* Line containing "."		*/
	short	w_doto;			/* Byte offset for "."		*/
	struct	LINE *w_markp;		/* Line containing "mark"	*/
	short	w_marko;		/* Byte offset for "mark"	*/
	char	w_toprow;		/* Origin 0 top row of window	*/
	char	w_ntrows;		/* # of rows of text in window	*/
	char	w_force;		/* If NZ, forcing row.		*/
	char	w_flag;			/* Flags.			*/
}	WINDOW;

#define	WFFORCE	0x01			/* Window needs forced reframe	*/
#define	WFMOVE	0x02			/* Movement from line to line	*/
#define	WFEDIT	0x04			/* Editing within a line	*/
#define	WFHARD	0x08			/* Better to a full display	*/
#define	WFMODE	0x10			/* Update mode line.		*/

/*
 * Text is kept in buffers. A buffer header, described
 * below, exists for every buffer in the system. The buffers are
 * kept in a big list, so that commands that search for a buffer by
 * name can find the buffer header. There is a safe store for the
 * dot and mark in the header, but this is only valid if the buffer
 * is not being displayed (that is, if "b_nwnd" is 0). The text for
 * the buffer is kept in a circularly linked list of lines, with
 * a pointer to the header line in "b_linep".
 */
typedef	struct	BUFFER {
	struct	BUFFER *b_bufp;		/* Link to next BUFFER		*/
	struct	LINE *b_dotp;		/* Link to "." LINE structure	*/
	short	b_doto;			/* Offset of "." in above LINE	*/
	struct	LINE *b_markp;		/* The same as the above two,	*/
	short	b_marko;		/* but for the "mark"		*/
	struct	LINE *b_linep;		/* Link to the header LINE	*/
	char	b_nwnd;			/* Count of windows on buffer	*/
	char	b_flag;			/* Flags			*/
	uchar	b_fname[NFILEN];	/* File name			*/
	uchar	b_bname[NBUFN];		/* Buffer name			*/
}	BUFFER;

#define	BFTEMP	0x01			/* Internal temporary buffer	*/
#define	BFCHG	0x02			/* Changed since last write	*/
#define	BFERROR	0x04			/* Error file buffer		*/
#define	BFNOWRT	0x08			/* Don't write this buffer	*/
#define	BFBACKP	0x10			/* Rename file before write	*/
#define	BFWRITE	0x20			/* Written since read		*/
#define	BFTRUNC	0x40			/* File truncated		*/
#define	BFHELP	0x80			/* Buffer is a help buffer	*/

/*
 * The starting position of a
 * region, and the size of the region in
 * characters, is kept in a region structure.
 * Used by the region commands.
 */
typedef	struct	{
	struct	LINE *r_linep;		/* Origin LINE address.		*/
	short	r_offset;		/* Origin LINE offset.		*/
	short	r_size;			/* Length in characters.	*/
}	REGION;

/*
 * All text is kept in circularly linked
 * lists of "LINE" structures. These begin at the
 * header line (which is the blank line beyond the
 * end of the buffer). This line is pointed to by
 * the "BUFFER". Each line contains a the number of
 * bytes in the line (the "used" size), the size
 * of the text array, and the text. The end of line
 * is not stored as a byte; its implied. Future
 * additions will include update hints, and a
 * list of marks into the line.
 */
typedef	struct	LINE {
	struct	LINE *l_fp;		/* Link to the next line	*/
	struct	LINE *l_bp;		/* Link to the previous line	*/
	short	l_size;			/* Allocated size		*/
	short	l_used;			/* Used size			*/
	long	l_lnumber;		/* Line number in original file	*/
	uchar	l_text[];		/* A bunch of characters.	*/
}	LINE;

#define	lforw(lp)	((lp)->l_fp)
#define	lback(lp)	((lp)->l_bp)
#define	lgetc(lp, n)	((lp)->l_text[(n)]&0xFF)
#define	lputc(lp, n, c)	((lp)->l_text[(n)]=(c))
#define	llength(lp)	((lp)->l_used)
#define	l_number(lp)	((lp)->l_lnumber)
/*
 * The editor communicates with the display
 * using a high level interface. A "TERM" structure
 * holds useful variables, and indirect pointers to
 * routines that do useful operations. The low level get
 * and put routines are here too. This lets a terminal,
 * in addition to having non standard commands, have
 * funny get and put character code too. The calls
 * might get changed to "termp->t_field" style in
 * the future, to make it possible to run more than
 * one terminal type.
 */  
typedef	struct	{
	short	t_nrow;			/* Number of rows.		*/
	short	t_ncol;			/* Number of columns.		*/
	int	(*t_open)();		/* Open terminal at the start.	*/
	int	(*t_close)();		/* Close terminal at end.	*/
	int	(*t_getchar)();		/* Get character from keyboard.	*/
	int	(*t_putchar)();		/* Put character to display.	*/
	int	(*t_flush)();		/* Flush output buffers.	*/
	int	(*t_move)();		/* Move the cursor, origin 0.	*/
	int	(*t_eeol)();		/* Erase to end of line.	*/
	int	(*t_eeop)();		/* Clear screen			*/
	int	(*t_beep)();		/* Beep.			*/
	int	(*t_stand)();		/* Standout mode.		*/
}	TERM;

/* Shorthand for terminal routines...	*/
#define	tputc(X)	(*term.t_putchar)(X)	/* Put a character	*/
#define	tgetc		(*term.t_getchar)	/* Get a character	*/
#define	tbeep		(*term.t_beep)		/* Beep the bell	*/
#define	tstand(X)	(*term.t_stand)(X)	/* Standout mode	*/
#define	topen		(*term.t_open)		/* Open the terminal	*/
#define	tclose		(*term.t_close)		/* Close the terminal	*/
#define	tmove(X,Y)	(*term.t_move)((X),(Y))	/* Move cursor		*/
#define	teeol		(*term.t_eeol)		/* Erase to end of line	*/
#define	teeop		(*term.t_eeop)		/* Clear screen		*/
#define	tflush		(*term.t_flush)		/* Flush output buff	*/

/* Command line switch flags -- set in runswitch	*/
#define	CF_ERROR	(0x0001)	/* Error edit switch specified	*/
#define	CF_WARN		(0x0004)	/* Warn user before overwrite	*/
#define CF_GRABMEM	(0x0008)	/* GEM grab memory		*/
#define	CF_VLONG	(0x2000)	/* Very long screen flag	*/
#define	CF_LONGSCR	(0x4000)	/* Long screen flag		*/
#define	CF_DEBUG	(0x8000)	/* Debugging flag		*/

#define taber(c) ((c) += ((bind.tabsiz - (c % bind.tabsiz)) - 1))

extern	unsigned int runswitch;		/* Switch flags			*/

extern	int	currow;			/* Cursor row			*/
extern	int	curcol;			/* Cursor column		*/
extern	int	thisflag;		/* Flags, this command		*/
extern	int	lastflag;		/* Flags, last command		*/
extern	int	curgoal;		/* Goal for C-P, C-N		*/
extern	int	mpresf;			/* Stuff in message line	*/
extern	int	sgarbf;			/* State of screen unknown	*/
extern	WINDOW	*curwp;			/* Current window		*/
extern	BUFFER	*curbp;			/* Current buffer		*/
extern	WINDOW	*wheadp;		/* Head of list of windows	*/
extern	BUFFER	*bheadp;		/* Head of list of buffers	*/
extern	BUFFER	*blistp;		/* Buffer for C-X C-B		*/
#if	LIBHELP
extern	BUFFER	*helpbp;		/* Buffer for help		*/
#endif
extern	BUFFER	*errbp;			/* Error file buffer		*/
extern	short	*kbdm;			/* Holds kayboard macro data	*/
extern	short	*kbdmip;		/* Input pointer for above	*/
extern	short	*kbdmop;		/* Output pointer for above	*/
extern	uchar	pat[];			/* Search pattern		*/
extern	TERM	term;			/* Terminal information.	*/
extern	uchar	*ufiles[];		/* command-line specified files	*/
extern	uchar	errfile[];		/* error file name		*/
#if	LIBHELP
extern	uchar	hfname[];		/* Help file name buffer	*/
extern	uchar	hiname[];		/* Help index name buffer	*/
extern	uchar	*helpfile;		/* Help file name		*/
extern	uchar	*helpindex;		/* Help index file name		*/
#endif
extern	BUFFER	*bfind();		/* Lookup a buffer by name	*/
extern	WINDOW	*wpopup();		/* Pop up window creation	*/
extern	LINE	*lalloc();		/* Allocate a line		*/

extern KEYTAB keytab[];
extern FUNTAB funtab[];

/* A major optimization for native GEMDOS situations...	*/
#if	GEM && NATIVE
#undef	tputc
extern	long	bios();
#define	tputc(X)	bios(3, 2, (X))	/* Faster output		*/
#endif
