/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)curses.h	5.4 (Berkeley) 6/30/88
 */

# ifndef WINDOW

# include	<stdio.h>
# include	<sgtty.h>
# include	<unctrl.h>

# define	bool	char
# define	reg	register

# define	TRUE	(1)
# define	FALSE	(0)
# define	ERR	(0)
# define	OK	(1)

# define	_ENDLINE	001
# define	_FULLWIN	002
# define	_SCROLLWIN	004
# define	_FLUSH		010
# define	_FULLLINE	020
# define	_IDLINE		040
# define	_STANDOUT	0200
# define	_NOCHANGE	-1

/*
 * The COHERENT Tputs(s,n,f) macro attempts to ensure string output will be
 * atomic by flushing the stdio output buffer in _need(s) if required.  This
 * should prevent the splitting of escape sequences by other terminal output,
 * provided that the terminal drive guarantees atomicity of writes.
 */
#ifdef	COHERENT
# define	Tputs(s,n,f)	(_need(s), tputs(s, n, f))
#else
# define	Tputs(s,n,f)	tputs(s, n, f)
#endif

# define	_puts(s)	Tputs(s, 0, _putchar)

typedef	struct sgttyb	SGTTY;
typedef short		chtype;	/* Character + Attributes */

/*
 * Capabilities from termcap.
 */
#define	CS	change_scroll_region
#define	VE	cursor_normal
#define	VS	cursor_visible
#define SO	enter_standout_mode
#define US	enter_underline_mode
#define	VB	flash_screen
#define	K0	key_f0
#define	K1	key_f1
#define	K2	key_f2
#define	K3	key_f3
#define	K4	key_f4
#define	K5	key_f5
#define	K6	key_f6
#define	K7	key_f7
#define	K8	key_f8
#define	K9	key_f9
#define	KD	key_down
#define	KH	key_home
#define	KL	key_left
#define	KR	key_right
#define	KU	key_up

extern bool     AM, BS, CA, DA, DB, EO, HC, HZ, IN, MI, MS, NC, NS, OS, UL,
		XB, XN, XT, XS, XX;

extern char	*AL,
		*BC, *BT,
		*CD, *CE, *CL, *CM, *CR, *CS,
		*DC, *DL, *DM, *DO,
		*ED, *EI,
		*HO,
		*IC, *IM, *IP,
		*KE, *KS,
		*LL,
		*MA,
		*ND, *NL,
		*RC,
		*SC, *SE, *SF, *SR,
		*TA, *TE, *TI,
		*UC, *UE, *UP,
		*AL_PARM, *DL_PARM, *UP_PARM, *DOWN_PARM,
		*LEFT_PARM, *RIGHT_PARM;
extern char	PC;

/*
 * System V Capabilities.
 */
extern bool	prtr_silent, xon_xoff;

extern char	*bell,
		*cursor_invisible, *cursor_normal, *cursor_visible,
		*enter_am_mode, *enter_blink_mode, *enter_bold_mode,
		*enter_delete_mode, *enter_dim_mode, *enter_protected_mode,
		*enter_reverse_mode, *enter_secure_mode,
		*enter_standout_mode, *enter_underline_mode,
		*exit_am_mode, *exit_attribute_mode,
		*flash_screen,
		*init_1string, *init_2string, *init_3string,
		*key_a1, *key_a3,
		*key_b2, *key_backspace, *key_beg, *key_btab,
		*key_c1, *key_c3, *key_cancel, *key_catab, *key_clear,
		*key_close, *key_command, *key_copy, *key_create, *key_ctab,
		*key_dc, *key_dl, *key_down,
		*key_eic, *key_end, *key_enter, *key_eol, *key_eos, *key_exit,
		*key_f0,  *key_f1,  *key_f2,  *key_f3,  *key_f4,
		*key_f5,  *key_f6,  *key_f7,  *key_f8,  *key_f9,
		*key_f10, *key_f11, *key_f12, *key_f13, *key_f14,
		*key_f15, *key_f16, *key_f17, *key_f18, *key_f19,
		*key_f20, *key_f21, *key_f22, *key_f23, *key_f24,
		*key_f25, *key_f26, *key_f27, *key_f28, *key_f29,
		*key_f30, *key_f31, *key_f32, *key_f33, *key_f34,
		*key_f35, *key_f36, *key_f37, *key_f38, *key_f39,
		*key_f40, *key_f41, *key_f42, *key_f43, *key_f44,
		*key_f45, *key_f46, *key_f47, *key_f48, *key_f49,
		*key_f50, *key_f51, *key_f52, *key_f53, *key_f54,
		*key_f55, *key_f56, *key_f57, *key_f58, *key_f59,
		*key_f60, *key_f61, *key_f62, *key_f63,
		*key_find,
		*key_help, *key_home,
		*key_ic, *key_il,
		*key_left, *key_ll,
		*key_mark, *key_message, *key_move,
		*key_next, *key_npage,
		*key_open, *key_options,
		*key_ppage, *key_previous, *key_print,
		*key_redo, *key_reference, *key_refresh, *key_replace,
		*key_restart, *key_resume, *key_right,
		*key_save, *key_sbeg, *key_scancel, *key_scommand, *key_scopy,
		*key_screate, *key_sdc, *key_sdl, *key_select, *key_send,
		*key_seol, *key_sexit, *key_sf, *key_sfind, *key_shelp,
		*key_shome, *key_sic, *key_sleft, *key_smessage, *key_smove,
		*key_snext, *key_soptions, *key_sprevious, *key_sprint,
		*key_sr, *key_sredo, *key_sreplace, *key_sright, *key_srsume,
		*key_ssave, *key_ssuspend, *key_stab, *key_sundo, *key_suspend,
		*key_undo, *key_up,
		*prtr_off, *prtr_on,
		*reset_1string, *reset_2string, *reset_3string,
		*xoff_character, *xon_character;

/*
 * From the tty modes...
 */
extern bool	GT, NONL, UPPERCASE, normtty, _pfast;

struct _win_st {
	short		_cury, _curx;
	short		_maxy, _maxx;
	short		_begy, _begx;
	short		_flags;
	short		_attrs;
	short		_ch_off;
	bool		_clear;
	bool		_leave;
	bool		_scroll;
	bool		_use_keypad;	/* 0=no, 1=yes, 2=yes/timeout */
	bool		_nodelay;	/* T=don't wait for tty input */
	char		**_y;
	short		*_firstch;
	short		*_lastch;
	struct _win_st	*_nextp, *_orig;
};

# define	WINDOW	struct _win_st

/*
 * Translation table entry: Termcap field name into address of a boolean var.
 */
typedef struct tflgent_s {
	char id[2];	/* termcap code */
	bool *bp;	/* pointer to boolean var */
} tflgent_t;

/*
 * Translation table entry: keyboard token into address of a string variable.
 */
typedef struct tkeyent_s {
	short	id;	/* key code */
	char	**cpp;	/* pointer to var which in turn points to string */
} tkeyent_t;

/*
 * Translation table entry: Termcap field name into address of a string var.
 */
typedef struct tstrent_s {
	char id[2];	/* termcap code */
	char **cpp;	/* pointer to var which in turn points to string */
} tstrent_t;

/*
 * Types for System V compatibility.
 */
#define	SCREEN	char

/*
 * Default Translation Tables.
 * End of table is signified by an id byte of 0.
 */
extern tflgent_t tflgtab[];	/* Default boolean  translation table */
extern tkeyent_t tkeytab[];	/* Default keyboard translation table */
extern tstrent_t tstrtab[];	/* Default string   translation table */
extern tflgent_t * tflgmap;	/* Current boolean  translation table */
extern tkeyent_t * tkeymap;	/* Current keyboard translation table */
extern tstrent_t * tstrmap;	/* Current string   translation table */

extern bool	My_term, _echoit, _rawmode, _endwin;

extern char	*Def_term, ttytype[];

extern int	LINES, COLS, _tty_ch, _res_flg;
extern int	magic_cookie_glitch;

extern SGTTY	_tty;

extern WINDOW	*stdscr, *curscr;

/*
 *	Define VOID to stop lint from generating "null effect"
 * comments.
 */
# ifdef lint
int	__void__;
# define	VOID(x)	(__void__ = (int) (x))
# else
# define	VOID(x)	(x)
# endif

/*
 * psuedo functions for standard screen
 */
# define	addch(ch)	VOID(waddch(stdscr, ch))
# define	getch()		VOID(wgetch(stdscr))
# define	addbytes(da,co)	VOID(waddbytes(stdscr, da,co))
# define	addstr(str)	VOID(waddbytes(stdscr, str, strlen(str)))
# define	getstr(str)	VOID(wgetstr(stdscr, str))
# define	move(y, x)	VOID(wmove(stdscr, y, x))
# define	clear()		VOID(wclear(stdscr))
# define	erase()		VOID(werase(stdscr))
# define	clrtobot()	VOID(wclrtobot(stdscr))
# define	clrtoeol()	VOID(wclrtoeol(stdscr))
# define	insertln()	VOID(winsertln(stdscr))
# define	deleteln()	VOID(wdeleteln(stdscr))
# define	refresh()	VOID(wrefresh(stdscr))
# define	inch()		VOID(winch(stdscr))
# define	insch(c)	VOID(winsch(stdscr,c))
# define	delch()		VOID(wdelch(stdscr))
# define	standout()	VOID(wstandout(stdscr))
# define	standend()	VOID(wstandend(stdscr))
# define	attron(at)	wattron(stdscr,at)
# define	attroff(at)	wattroff(stdscr,at)
# define	attrset(at)	wattrset(stdscr,at)

/*
 * mv functions
 */
#define	mvwaddch(win,y,x,ch)	VOID(wmove(win,y,x)==ERR?ERR:waddch(win,ch))
#define	mvwgetch(win,y,x)	VOID(wmove(win,y,x)==ERR?ERR:wgetch(win))
#define	mvwaddbytes(win,y,x,da,co) \
		VOID(wmove(win,y,x)==ERR?ERR:waddbytes(win,da,co))
#define	mvwaddstr(win,y,x,str) \
		VOID(wmove(win,y,x)==ERR?ERR:waddbytes(win,str,strlen(str)))
#define mvwgetstr(win,y,x,str)  VOID(wmove(win,y,x)==ERR?ERR:wgetstr(win,str))
#define	mvwinch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : winch(win))
#define	mvwdelch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : wdelch(win))
#define	mvwinsch(win,y,x,c)	VOID(wmove(win,y,x) == ERR ? ERR:winsch(win,c))
#define	mvaddch(y,x,ch)		mvwaddch(stdscr,y,x,ch)
#define	mvgetch(y,x)		mvwgetch(stdscr,y,x)
#define	mvaddbytes(y,x,da,co)	mvwaddbytes(stdscr,y,x,da,co)
#define	mvaddstr(y,x,str)	mvwaddstr(stdscr,y,x,str)
#define mvgetstr(y,x,str)       mvwgetstr(stdscr,y,x,str)
#define	mvinch(y,x)		mvwinch(stdscr,y,x)
#define	mvdelch(y,x)		mvwdelch(stdscr,y,x)
#define	mvinsch(y,x,c)		mvwinsch(stdscr,y,x,c)

/*
 * pseudo functions
 */
#define	clearok(win,bf)	 (win->_clear = bf)
#define	keypad(win,bf)	 (win->_use_keypad = bf)
#define	leaveok(win,bf)	 (win->_leave = bf)
#define	nodelay(win,bf)	 (win->_nodelay = bf)
#define	scrollok(win,bf) (win->_scroll = bf)
#define flushok(win,bf)	 (bf ? (win->_flags |= _FLUSH):(win->_flags &= ~_FLUSH))
#define	getyx(win,y,x)	 y = win->_cury, x = win->_curx
#define	winch(win)	 (win->_y[win->_cury][win->_curx] & 0177)

#define raw()	 (_tty.sg_flags|=RAW, _pfast=_rawmode=TRUE, stty(_tty_ch,&_tty))
#define noraw()	 (_tty.sg_flags&=~RAW,_rawmode=FALSE,_pfast=!(_tty.sg_flags&CRMOD),stty(_tty_ch,&_tty))
#define cbreak() (_tty.sg_flags |= CBREAK, _rawmode = TRUE, stty(_tty_ch,&_tty))
#define nocbreak() (_tty.sg_flags &= ~CBREAK,_rawmode=FALSE,stty(_tty_ch,&_tty))
#define crmode() cbreak()	/* backwards compatability */
#define nocrmode() nocbreak()	/* backwards compatability */
#define echo()	 (_tty.sg_flags |= ECHO, _echoit = TRUE, stty(_tty_ch, &_tty))
#define noecho() (_tty.sg_flags &= ~ECHO, _echoit = FALSE, stty(_tty_ch, &_tty))
#define nl()	 (_tty.sg_flags |= CRMOD,_pfast = _rawmode,stty(_tty_ch, &_tty))
#define nonl()	 (_tty.sg_flags &= ~CRMOD, _pfast = TRUE, stty(_tty_ch, &_tty))
#define	savetty() ((void) gtty(_tty_ch, &_tty), _res_flg = _tty.sg_flags)
#define	resetty() (_tty.sg_flags = _res_flg, (void) stty(_tty_ch, &_tty))

#define	erasechar()	(_tty.sg_erase)
#define	killchar()	(_tty.sg_kill)
#define baudrate()	(_tty.sg_ospeed)

WINDOW	*initscr(), *newwin(), *subwin();
char	*longname(), *getcap();

/*
 * Various video attributes - A_STANDOUT is only sure bet.
 */
#define A_STANDOUT	0000200
#define A_UNDERLINE	0000400
#define A_REVERSE	0001000
#define A_BLINK		0002000
#define A_DIM		0004000
#define A_BOLD		0010000

#define A_NORMAL	0000000
#define A_ATTRIBUTES	0177600		/* Mask to extract video attributes */
#define A_CHARTEXT	0000177		/* Mask to extract character value  */

/*
 * Funny "characters" enabled for various special function keys for input
 */
#define KEY_BREAK	0401		/* break key (unreliable) */
#define KEY_DOWN	0402		/* The four arrow keys ... */
#define KEY_UP		0403
#define KEY_LEFT	0404
#define KEY_RIGHT	0405		/* ... */
#define KEY_HOME	0406		/* Home key (upward+left arrow) */
#define KEY_BACKSPACE	0407		/* backspace (unreliable) */
#define KEY_F0		0410		/* Function keys.  Space for 64 */
#define KEY_F(n)	(KEY_F0+(n))	/* keys is reserved. */
#define KEY_DL		0510		/* Delete line */
#define KEY_IL		0511		/* Insert line */
#define KEY_DC		0512		/* Delete character */
#define KEY_IC		0513		/* Insert char or enter insert mode */
#define KEY_EIC		0514		/* Exit insert char mode */
#define KEY_CLEAR	0515		/* Clear screen */
#define KEY_EOS		0516		/* Clear to end of screen */
#define KEY_EOL		0517		/* Clear to end of line */
#define KEY_SF		0520		/* Scroll 1 line forward */
#define KEY_SR		0521		/* Scroll 1 line backwards (reverse) */
#define KEY_NPAGE	0522		/* Next page */
#define KEY_PPAGE	0523		/* Previous page */
#define KEY_STAB	0524		/* Set tab */
#define KEY_CTAB	0525		/* Clear tab */
#define KEY_CATAB	0526		/* Clear all tabs */
#define KEY_ENTER	0527		/* Enter or send (unreliable) */
#define KEY_SRESET	0530		/* soft (partial) reset (unreliable) */
#define KEY_RESET	0531		/* reset or hard reset (unreliable) */
#define KEY_PRINT	0532		/* print or copy */
#define KEY_LL		0533		/* home down or bottom (lower left) */
					/* The keypad is arranged like this: */
					/*    a1    up    a3   */
					/*   left   b2  right  */
					/*    c1   down   c3   */
#define KEY_A1		0534		/* upper left of keypad */
#define KEY_A3		0535		/* upper right of keypad */
#define KEY_B2		0536		/* center of keypad */
#define KEY_C1		0537		/* lower left of keypad */
#define KEY_C3		0540		/* lower right of keypad */
#define	KEY_BTAB	0541		/* back tab key */
#define	KEY_BEG		0542		/* beg(inning) key */
#define	KEY_CANCEL	0543		/* cancel key */
#define	KEY_CLOSE	0544		/* close key */
#define	KEY_COMMAND	0545		/* cmd (command) key */
#define	KEY_COPY	0546		/* copy key */
#define	KEY_CREATE	0547		/* create key */
#define	KEY_END		0550		/* end key */
#define	KEY_EXIT	0551		/* exit key */
#define	KEY_FIND	0552		/* find key */
#define	KEY_HELP	0553		/* help key */
#define	KEY_MARK	0554		/* mark key */
#define	KEY_MESSAGE	0555		/* message key */
#define	KEY_MOVE	0556		/* move key */
#define	KEY_NEXT	0557		/* next object key */
#define	KEY_OPEN	0560		/* open key */
#define	KEY_OPTIONS	0561		/* options key */
#define	KEY_PREVIOUS	0562		/* previous object key */
#define	KEY_REDO	0563		/* redo key */
#define	KEY_REFERENCE	0564		/* ref(erence) key */
#define	KEY_REFRESH	0565		/* refresh key */
#define	KEY_REPLACE	0566		/* replace key */
#define	KEY_RESTART	0567		/* restart key */
#define	KEY_RESUME	0570		/* resume key */
#define	KEY_SAVE	0571		/* save key */
#define	KEY_SBEG	0572		/* shifted beginning key */
#define	KEY_SCANCEL	0573		/* shifted cancel key */
#define	KEY_SCOMMAND	0574		/* shifted command key */
#define	KEY_SCOPY	0575		/* shifted copy key */
#define	KEY_SCREATE	0576		/* shifted create key */
#define	KEY_SDC		0577		/* shifted delete char key */
#define	KEY_SDL		0600		/* shifted delete line key */
#define	KEY_SELECT	0601		/* select key */
#define	KEY_SEND	0602		/* shifted end key */
#define	KEY_SEOL	0603		/* shifted clear line key */
#define	KEY_SEXIT	0604		/* shifted exit key */
#define	KEY_SFIND	0605		/* shifted find key */
#define	KEY_SHELP	0606		/* shifted help key */
#define	KEY_SHOME	0607		/* shifted home key */
#define	KEY_SIC		0610		/* shifted input key */
#define	KEY_SLEFT	0611		/* shifted left arrow key */
#define	KEY_SMESSAGE	0612		/* shifted message key */
#define	KEY_SMOVE	0613		/* shifted move key */
#define	KEY_SNEXT	0614		/* shifted next key */
#define	KEY_SOPTIONS	0615		/* shifted options key */
#define	KEY_SPREVIOUS	0616		/* shifted prev key */
#define	KEY_SPRINT	0617		/* shifted print key */
#define	KEY_SREDO	0620		/* shifted redo key */
#define	KEY_SREPLACE	0621		/* shifted replace key */
#define	KEY_SRIGHT	0622		/* shifted right-arrow key */
#define	KEY_SRSUME	0623		/* shifted resume key */
#define	KEY_SSAVE	0624		/* shifted save key */
#define	KEY_SSUSPEND	0625		/* shifted suspend key */
#define	KEY_SUNDO	0626		/* shifted undo key */
#define	KEY_SUSPEND	0627		/* suspend key */
#define	KEY_UNDO	0630		/* undo key */

# endif
