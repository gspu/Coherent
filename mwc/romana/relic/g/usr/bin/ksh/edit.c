/*
 *  EDIT.C -- Emacs-like command line editing and history
 *
 *  created by Ron Natalie at BRL
 *  modified by Doug Kingston, Doug Gwyn, and Lou Salkind
 *  adapted to PD ksh by Eric Gisin
 */

#if EDIT

static char *RCSid = "$Header: edit.c,v 3.1 88/11/03 09:14:51 egisin Exp $";

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#if !COHERENT
#include <sys/types.h>
#endif
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#include "sh.h"
#include "lex.h"
#include "tree.h"		/* DOTILDE */
#include "tty.h"
#include "table.h"
#include "expand.h"

#if COHERENT
struct	dirent	*readdir();	/* missing from headers */
DIR	*opendir();
#endif

Area	aedit;
#define	AEDIT	&aedit		/* area for kill ring and macro defns */

#undef CTRL			/* _BSD brain damage */
#define	CTRL(x)		((x) == '?' ? 0x7F : (x) & 0x1F)	/* ASCII */
#define	UNCTRL(x)	((x) == 0x7F ? '?' : (x) | 0x40)	/* ASCII */

#if ! defined S_ISDIR
#define S_ISDIR(mode)	(((mode) & S_IFMT) == S_IFDIR)
#endif

#if ! defined S_ISREG
#define S_ISREG(mode)	(((mode) & S_IFMT) == S_IFREG)
#endif

#if defined _CRAY2
extern unsigned	sleep();
#endif

/* values returned by keyboard functions */
#define	KSTD	0
#define	KPREF	1		/* ^[, ^X */
#define	KEOL	2		/* ^M, ^J */
#define	KINTR	3		/* ^G, ^C */
	
struct	x_ftab  {
	int	(*xf_func)();
	char	*xf_name;
	char	xf_db_tab;
	char	xf_db_char;
	short	xf_flags;
};

#define	XF_NINPUT	1
#define	XF_ALLOC	2
#define	XF_NOBIND	4

#define	isfs(c)		(c == ' ' || c == '\t')
#define	BEL		0x07
#define	CMASK		0x7F	/* 7-bit ASCII character mask */

static bool_t	x_mode = FALSE;
static int	x_prefix1 = CTRL('['), x_prefix2 = CTRL('X');
static char   **x_histp;	/* history position */
static	char   *xbuf;		/* beg input buffer */
static	char   *xend;		/* end input buffer */
static char    *xcp;
static char    *xep;
static int    (*x_last_command)();
/*static struct	x_ftab *x_tab[3][128];*/
static struct	x_ftab Const *(*x_tab)[128] = NULL; /* key definition */
static char    *(*x_atab)[128] = NULL; /* macro definitions */
#define	KILLSIZE	20
static char    *killstack[KILLSIZE];
static int	killsp, killtp;
static int	x_curprefix;
static char    *macroptr;
static int	first_time = 1;
static int	x_maxlen;	/* to determine column width */
static int	x_cols = 80;	/* todo: $COLUMNS */

static void	x_flush(), x_putc(), x_puts();
static void	x_goto(), x_bs(), x_delete(), x_ins(), x_mapin();
static int	x_fword(), x_bword(), x_size(), x_size_str();
static void	x_zotc(), x_zots(), x_push(), x_redraw(), x_load_hist();
static void	compl_command(), compl_dec(), compl_file();
static int	x_insert(), x_ins_string(), x_del_back();
static int	x_del_char(), x_del_bword(), x_mv_bword(), x_mv_fword();
static int	x_del_fword(), x_mv_back(), x_mv_forw(), x_search_char();
static int	x_newline(), x_end_of_text(), x_abort(), x_error();
static int	x_beg_hist(), x_end_hist(), x_prev_com(), x_next_com();
static int	x_search_hist(), x_del_line(), x_mv_end(), x_mv_begin();
static int	x_draw_line(), x_transpose(), x_meta1(), x_meta2();
static int	x_kill(), x_yank(), x_meta_yank(), x_literal();
static int	x_stuffreset(), x_stuff(), x_complete(), x_enumerate();
#if SILLY
static int	x_game_of_life();
#endif
static int	x_comp_file(), x_comp_comm();
static int	x_list_file(), x_list_comm();
static int	strmatch();

static struct x_ftab Const x_ftab[] = {
 	{x_insert,	"auto-insert",		0,	 0,	0 },
	{x_error,	"error",		0,	 0,	0 },
 	{x_ins_string,	"macro-string",		0,	 0,	XF_NOBIND|XF_ALLOC},
/* Do not move the above! */
	{x_del_back,	"delete-char-backward",	0, CTRL('H'),	0 },
	{x_del_char,	"delete-char-forward",	0, CTRL('D'),	0 },
	{x_del_bword,	"delete-word-backward",	0, CTRL('W'),	0 },
	{x_mv_bword,	"backward-word", 	1,	'b',	0 },
	{x_mv_fword,	"forward-word",		1,	'f',	0 },
	{x_del_fword,	"delete-word-forward", 	1,	'd',	0 },
	{x_mv_back,	"backward-char",	0, CTRL('B'),	0 },
	{x_mv_forw,	"forward-char",		0, CTRL('F'),	0 },
	{x_search_char,	"search-character",	0, CTRL(']'),	0 },
	{x_newline,	"newline",		0, CTRL('M'),	0 },
	{x_newline,	"newline",		0, CTRL('J'),	0 },
	{x_end_of_text,	"eot",			0, CTRL('_'),	0 },
	{x_abort,	"abort",		0, CTRL('G'),	0 },
	{x_prev_com,	"up-history",		0, CTRL('P'),	XF_NINPUT},
	{x_next_com,	"down-history",		0, CTRL('N'),	XF_NINPUT},
	{x_search_hist,	"search-history",	0, CTRL('R'),	XF_NINPUT},
	{x_beg_hist,	"beginning-of-history",	1,	'<',	XF_NINPUT},
	{x_end_hist,	"end-of-history",	1,	'>',	XF_NINPUT},
	{x_del_line,	"kill-line",		0, CTRL('U'),	0 },
	{x_mv_end,	"end-of-line",		0, CTRL('E'),	0 },
	{x_mv_begin,	"beginning-of-line",	0, CTRL('A'),	0 },
	{x_draw_line,	"redraw",		0, CTRL('L'),	0 },
	{x_meta1,	"prefix-1",		0, CTRL('['),	0 },
	{x_meta2,	"prefix-2",		0, CTRL('X'),	0 },
	{x_kill,	"kill-to-eol",		0, CTRL('K'),	0 },
	{x_yank,	"yank",			0, CTRL('Y'),	0 },
	{x_meta_yank,	"yank-pop", 		1,	'y',	0 },
	{x_literal,	"quote",		0, CTRL('^'),	0 },
	{x_stuffreset, 	"stuff-reset",		0,	 0,	0 },
#if BRL && defined(TIOCSTI)
	{x_stuff, 	"stuff",		0, CTRL('T'),	0 },
	{x_transpose,	"transpose-chars",	0,	 0,	0 },
#else
	{x_stuff, 	"stuff",		0,	 0,	0 },
	{x_transpose,	"transpose-chars",	0, CTRL('T'),	0 },
#endif
	{x_complete,	"complete",		1, CTRL('['),	0 },
	{x_enumerate,	"list",			1,	'?',	0 },
	{x_comp_file,	"complete-file",	2, CTRL('X'),	0 },
	{x_comp_comm,	"complete-command",	2, CTRL('['),	0 },
	{x_list_file,	"list-file",		0,	 0,	0 },
	{x_list_comm,	"list-command",		2,	'?',	0 },
#if SILLY
	{x_game_of_life, "play-game-of-life",	0,	0,	0 },
#endif 
	{ 0 }
};

#define	xft_insert &x_ftab[0]
#define	xft_error &x_ftab[1]
#define	xft_ins_string &x_ftab[2]

int
x_read(fd, buf, len)
	int fd;			/* not used */
	char *buf;
	size_t len;
{
	char	c;
	int	i;
	int   (*func)();
	extern	x_insert();

	(void)set_xmode(TRUE);
	xbuf = buf; xend = buf + len;
	xcp = xep = buf;
	*xcp = 0;
	x_curprefix = 0;
	macroptr = null;
	x_histp = histptr + 1;

	while (1)  {
		x_flush();
		if (*macroptr)  {
			c = *macroptr++;
			if (*macroptr == 0)
				macroptr = null;
		} else {
			i = read(ttyfd, &c, 1);
			if (i != 1)
				goto Exit;
		}

		if (x_curprefix == -1)
			func = x_insert;
		else
			func = x_tab[x_curprefix][c&CMASK]->xf_func;
		if (func == NULL)
			func = x_error;
		i = c | (x_curprefix << 8);
		x_curprefix = 0;
		switch (i = (*func)(i))  {
		  case KSTD:
			x_last_command = func;
		  case KPREF:
			break;
		  case KEOL:
			i = xep - xbuf;
			x_last_command = 0;
			/* XXX -- doesn't get them all */
			if (strncmp(xbuf, "stty", 4) == 0)
				first_time = 1;
			goto Exit;
		  case KINTR:	/* special case for interrupt */
			i = -1;
			errno = EINTR;
			*xbuf = '\0';
			goto Exit;
		}
	}
Exit:
	(void)set_xmode(FALSE);
	if (i < 0 && errno == EINTR)
		trapsig(SIGINT);
	return i;
}

static int
x_insert(c)  {
	char	str[2];

	/*
	 *  Should allow tab and control chars.
	 */
	if (c == 0)  {
		x_putc(BEL);
		return KSTD;
	}
	str[0] = c;
	str[1] = 0;
	x_ins(str);
	return KSTD;
}

static int
x_ins_string(c)
{
	if (*macroptr)   {
		x_putc(BEL);
		return KSTD;
	}
	macroptr = x_atab[c>>8][c & CMASK];
	return KSTD;
}

static void
x_ins(cp)
	char	*cp;
{
	int	count, i;

	count = strlen(cp);
	if (xep+count >= xend) {
		x_putc(BEL);
		return;
	}

	if (xcp != xep)
		memmove(xcp+count, xcp, xep - xcp + 1);
	else
		xcp[count] = 0;
	memmove(xcp, cp, count);
	x_zots(xcp);
	xcp += count;
	xep += count;
	i = xep - xcp;
	cp = xep;
	while (i--)
		x_bs(*--cp);
	return;
}

static int
x_del_back(c)  {
	if (xcp == xbuf)  {
		x_putc(BEL);
		return KSTD;
	}
	x_goto(xcp - 1);
	x_delete(1);
	return KSTD;
}

static int
x_del_char(c)  {
	if (xcp == xep)  {
		x_putc(BEL);
		return KSTD;
	}
	x_delete(1);
	return KSTD;
}

static void
x_delete(nc)  {
	int	i,j;
	char	*cp;

	if (nc == 0)
		return;
	xep -= nc;
	cp = xcp;
	j = 0;
	i = nc;
	while (i--)  {
		j += x_size(*cp++);
	}
	memmove(xcp, xcp+nc, xep - xcp + 1);	/* Copies the null */
	x_zots(xcp);
	i = j;
	while (i--)
		x_putc(' ');
	i = j;
	while (i--)
		x_putc('\b');
	/*x_goto(xcp);*/
	i = xep - xcp;
	cp = xep;
	while (i--)
		x_bs(*--cp);
	return;	
}

static int
x_del_bword(c)  {
	x_delete(x_bword());
	return KSTD;
}

static int
x_mv_bword(c)  {
	(void)x_bword();
	return KSTD;
}

static int
x_mv_fword(c)  {
	x_goto(xcp + x_fword());
	return KSTD;
}

static int
x_del_fword(c)  {
	x_delete(x_fword());
	return KSTD;
}

static int
x_bword()  {
	int	nc = 0;
	register char *cp = xcp;

	if (cp == xbuf)  {
		x_putc(BEL);
		return 0;
	}
	while (cp != xbuf && isfs(cp[-1]))  {
		cp--;
		nc++;
	}
	while (cp != xbuf && !isfs(cp[-1]))  {
		cp--;
		nc++;
	}
	x_goto(cp);
	return nc;
}

static int
x_fword()  {
	int	nc = 0;
	char	*cp = xcp;

	if (cp == xep)  {
		x_putc(BEL);
		return 0;
	}
	while (cp != xep && !isfs(*cp))  {
		cp++;
		nc++;
	}
	while (cp != xep && isfs(*cp))  {
		cp++;
		nc++;
	}
	return nc;
}

static void
x_goto(cp)
	register char *cp;
{
	if (cp < xcp) {		/* move back */
		while (cp < xcp)
			x_bs(*--xcp);
	} else
	if (cp > xcp) {		/* move forward */
		while (cp > xcp)
			x_zotc(*xcp++);
	}
}

static void
x_bs(c)  {
	register i;
	i = x_size(c);
	while (i--)
		x_putc('\b');
}

static int
x_size_str(cp)
	register char *cp;
{
	register size = 0;
	while (*cp)
		size += x_size(*cp++);
	return size;
}

static int
x_size(c)  {
	if (c=='\t')
		return 4;	/* Kludge, tabs are always four spaces. */
	if (c < ' ' || c == 0x7F) /* ASCII control char */
		return 2;
	return 1;
}

static void
x_zots(str)
	register char *str;
{
	while (*str)
		x_zotc(*str++);
}

static void
x_zotc(c)
	int c;
{
	if (c == '\t')  {
		/*  Kludge, tabs are always four spaces.  */
		x_puts("    ");
	} else if (c < ' ' || c == 0x7F)  { /* ASCII */
		x_putc('^');
		x_putc(UNCTRL(c));
	} else
		x_putc(c);
}

/* tty output */

void
x_flush()
{
	fflush(stdout);
}

void
x_putc(c)
	int c;
{
	putc(c, stdout);
}

void
x_puts(s)
	register char *s;
{
	while (*s != 0)
		putc(*s++, stdout);
}

static int
x_mv_back(c)  {
	if (xcp == xbuf)  {
		x_putc(BEL);
		return KSTD;
	}
	x_goto(xcp-1);
	return KSTD;
}

static int
x_mv_forw(c)  {
	if (xcp == xep)  {
		x_putc(BEL);
		return KSTD;
	}
	x_goto(xcp+1);
	return KSTD;
}

static int
x_search_char(c) {
	char ci, *cp;

	*xep = '\0';
	if (read(ttyfd, &ci, 1) != 1 ||
	    /* we search forward, I don't know what Korn does */
	    (cp = (xcp == xep) ? NULL : strchr(xcp+1, ci)) == NULL &&
	    (cp = strchr(xbuf, ci)) == NULL) {
		x_putc(BEL);
		return KSTD;
	}
	x_goto(cp);
	return KSTD;
}

static int
x_newline(c)  {
	x_putc('\n');
	x_flush();
	*xep++ = '\n';
	return KEOL;
}

static int
x_end_of_text(c)  {
#if 0
	x_store_hist();
#endif
	return KEOL;
}

static int x_beg_hist(c) {x_load_hist(history); return KSTD;}

static int x_end_hist(c) {x_load_hist(histptr); return KSTD;}

static int x_prev_com(c) {x_load_hist(x_histp-1); return KSTD;}

static int x_next_com(c) {x_load_hist(x_histp+1); return KSTD;}

static void
x_load_hist(hp)
	register char **hp;
{
	int	oldsize;

	if (hp < history || hp > histptr) {
		x_putc(BEL);
		return;
	}
	x_histp = hp;
	oldsize = x_size_str(xbuf);
	(void)strcpy(xbuf, *hp);
	xep = xcp = xbuf + strlen(*hp);
	x_redraw(oldsize);
}

static int x_search(), x_match();

/* reverse incremental history search */
static int
x_search_hist(ci)
{
	int offset = -1;	/* offset of match in xbuf, else -1 */
	static char c[2];	/* input buffer */
	char pat [256+1];	/* pattern buffer */
	register char *p = pat;
	int (*func)();

	*p = 0;
	while (1) {
		if (offset < 0) {
			x_puts("\nI-search: ");
			x_zots(pat);
		}
		x_flush();
		if (read(ttyfd, c, 1) < 1)
			return KSTD;
		func = x_tab[0][*c&CMASK]->xf_func;
		if (*c == CTRL('['))
			break;
		else if (func == x_search_hist)
			offset = x_search(pat, offset);
		else if (func == x_del_back)
			continue;	/* todo */
		else if (func == x_insert) {
			/* add char to pattern */
			*p++ = *c, *p = 0;
			if (offset >= 0) {
				/* already have partial match */
				offset = x_match(xbuf, pat);
				if (offset >= 0) {
					x_goto(xbuf + offset + (p - pat) - (*pat == '^'));
					continue;
				}
			}
			offset = x_search(pat, offset);
		} else { /* other command */
			macroptr = c; /* push command */
			break;
		}
	}
	if (offset < 0)
		x_redraw(-1);
	return KSTD;
}

/* search backward from current line */
static int
x_search(pat, offset)
	char *pat;
	int offset;
{
	register char **hp;
	int i;

	for (hp = x_histp; --hp >= history; ) {
		i = x_match(*hp, pat);
		if (i >= 0) {
			if (offset < 0)
				x_putc('\n');
			x_load_hist(hp);
			x_goto(xbuf + i + strlen(pat) - (*pat == '^'));
			return i;
		}
	}
	x_putc(BEL);
	x_histp = histptr;
	return -1;
}

/* return position of first match of pattern in string, else -1 */
static int
x_match(str, pat)
	char *str, *pat;
{
	if (*pat == '^') {
		return (strncmp(str, pat+1, strlen(pat+1)) == 0) ? 0 : -1;
	} else {
		char *q = strstr(str, pat);
		return (q == NULL) ? -1 : q - str;
	}
}

static int
x_del_line(c)  {
	int	i, j;

	*xep = 0;
	i = xep- xbuf;
	j = x_size_str(xbuf);
	xcp = xbuf;
	x_push(i);
	xep = xbuf;
	*xcp = 0;
	x_redraw(j);
	return KSTD;
}

static int
x_mv_end(c)  {
	x_goto(xep);
	return KSTD;
}

static int
x_mv_begin(c)  {
	x_goto(xbuf);
	return KSTD;
}

static int
x_draw_line(c)
{
	x_redraw(-1);
	return KSTD;

}

static void
x_redraw(limit)  {
	int	i, j;
	char	*cp;

	if (limit == -1)
		x_putc('\n');
	else 
		x_putc('\r');
	x_flush();
	pprompt(prompt);
	x_zots(xbuf);
	if (limit != -1)  {
		i = limit - x_size_str(xbuf);
		j = 0;
		while (j < i)  {
			x_putc(' ');
			j++;
		}
		while (j--)
			x_putc('\b');
	}
	i = xep - xcp;
	cp = xep;
	while (i--)
		x_bs(*--cp);
	return;
}

static int
x_transpose(c)  {
	char	tmp;
	if (xcp == xbuf || xcp == xep)  {
		x_putc(BEL);
		return KSTD;
	}
	x_bs(xcp[-1]);
	x_zotc(xcp[0]);
	x_zotc(xcp[-1]);
	tmp = xcp[-1];
	xcp[-1] = xcp[0];
	xcp[0] = tmp;
	x_bs(xcp[0]);
	return KSTD;
}

static int
x_literal(c)  {
	x_curprefix = -1;
	return KSTD;
}

static int
x_meta1(c)  {
	x_curprefix = 1;
	return KPREF;
}

static int
x_meta2(c)  {
	x_curprefix = 2;
	return KPREF;
}

static int
x_kill(c)  {
	int	i;

	i = xep - xcp;
	x_push(i);
	x_delete(i);
	return KSTD;
}

static void
x_push(nchars)  {
	char	*cp;
	cp = alloc((size_t)(nchars+1), AEDIT);
	memmove(cp, xcp, nchars);
	cp[nchars] = 0;
	if (killstack[killsp])
		afree((Void *)killstack[killsp], AEDIT);
	killstack[killsp] = cp;
	killsp = (killsp + 1) % KILLSIZE;
}

static int
x_yank(c)  {
	if (killsp == 0)
		killtp = KILLSIZE;
	else
		killtp = killsp;
	killtp --;
	if (killstack[killtp] == 0)  {
		x_puts("\nnothing to yank");
		x_redraw(-1);
		return KSTD;
	}
	x_ins(killstack[killtp]);
	return KSTD;
}

static int
x_meta_yank(c)  {
	int	len;
	if (x_last_command != x_yank && x_last_command != x_meta_yank)  {
		x_puts("\nyank something first");
		x_redraw(-1);
		return KSTD;
	}
	len = strlen(killstack[killtp]);
	x_goto(xcp - len);
	x_delete(len);
	do  {
		if (killtp == 0)
			killtp = KILLSIZE - 1;
		else
			killtp--;
	}  while (killstack[killtp] == 0);
	x_ins(killstack[killtp]);
	return KSTD;
}

static int
x_abort(c) {
	/* x_zotc(c); */
	return KINTR;
}

static int
x_error(c) {
	x_putc(BEL);
	return KSTD;
}

#if _BSD
#if defined TIOCGATC
static struct ttychars lchars, lcharsorig;
#else
static struct tchars tchars, tcharsorig;
static struct ltchars ltchars, ltcharsorig;
#endif
#endif

#if _BSD || (COHERENT && !MWC_HP)
bool_t
set_xmode(onoff)
bool_t	onoff;
{
	bool_t	prev;
typedef	struct sgttyb	Sgttyb;
	static 	Sgttyb cb;
	static  Sgttyb orig;

	if (x_mode == onoff) return x_mode;
	prev = x_mode;
	x_mode = onoff;
	if (onoff)  {
		if (first_time)  {
			(void)ioctl(0, TIOCGETP, &cb);
			orig = cb;
			cb.sg_flags &= ~ECHO;
			cb.sg_flags |= CBREAK;
#if defined TIOCGATC
			(void)ioctl(0, TIOCGATC, &lcharsorig);
			lchars = lcharsorig;
			lchars.tc_suspc = -1;
			lchars.tc_dsuspc = -1;
			lchars.tc_lnextc = -1;
			lchars.tc_statc = -1;
			lchars.tc_intrc = -1;
			lchars.tc_quitc = -1;
			lchars.tc_rprntc = -1;
#else
#if !COHERENT
			(void)ioctl(0, TIOCGETC, &tcharsorig);
			(void)ioctl(0, TIOCGLTC, &ltcharsorig);
			tchars = tcharsorig;
			ltchars = ltcharsorig;
			ltchars.t_suspc = -1;
			ltchars.t_dsuspc = -1;
			ltchars.t_lnextc = -1;
			tchars.t_intrc = -1;
			tchars.t_quitc = -1;
			ltchars.t_rprntc = -1;
#endif
#endif
			first_time = 0;
		}
		(void)ioctl(0, TIOCSETN, &cb);
#if defined TIOCGATC
		(void)ioctl(0, TIOCSATC, &lchars);
#else
#if !COHERENT
		(void)ioctl(0, TIOCSETC, &tchars);
		(void)ioctl(0, TIOCSLTC, &ltchars);
#endif
#endif
	}
	else {
		(void)ioctl(0, TIOCSETN, &orig);
#if defined TIOCGATC
		(void)ioctl(0, TIOCSATC, &lcharsorig);
#else
#if !COHERENT
		(void)ioctl(0, TIOCSETC, &tcharsorig);
		(void)ioctl(0, TIOCSLTC, &ltcharsorig);
#endif
#endif
	}
	return prev;
}

#else	/* !_BSD */

bool_t
set_xmode(onoff)
bool_t	onoff;
{
	bool_t	prev;
	static	struct termio cb, orig;

	if (x_mode == onoff) return x_mode;
	prev = x_mode;
	x_mode = onoff;

	if (onoff)  {
		if (first_time)  {
			(void)ioctl(0, TCGETA, &cb);
			orig = cb;
#if defined _CRAY2				/* brain-damaged terminal handler */
			cb.c_lflag &= ~(ICANON|ECHO);
			/* rely on print routine to map '\n' to CR,LF */
#else
			cb.c_iflag &= ~(INLCR|ICRNL);
#if _BSD_SYSV	/* need to force CBREAK instead of RAW (need CRMOD on output) */
			cb.c_lflag &= ~(ICANON|ECHO);
#else
			cb.c_lflag &= ~(ISIG|ICANON|ECHO);
#endif
			cb.c_cc[VTIME] = 0;
			cb.c_cc[VMIN] = 1;
#endif	/* _CRAY2 */
			first_time = 0;
		}
#if ! defined TCSETAW				/* e.g. Cray-2 */
		/* first wait for output to drain */
#if defined TCSBRK
		(void)ioctl(0, TCSBRK, 1);
#else	/* the following kludge is minimally intrusive, but sometimes fails */
		(void)sleep((unsigned)1);	/* fake it */
#endif
#endif
#if _BSD_SYSV || !defined(TCSETAW) || COHERENT
/* _BSD_SYSV needs to force TIOCSETN instead of TIOCSETP (preserve type-ahead) */
		(void)ioctl(0, TCSETA, &cb);
#else
		(void)ioctl(0, TCSETAW, &cb);
#endif
	}
	else {
#if ! defined TCSETAW				/* e.g. Cray-2 */
		/* first wait for output to drain */
#if defined TCSBRK
		(void)ioctl(0, TCSBRK, 1);
#else
/* doesn't seem to be necessary when leaving xmode */
/*		(void)sleep((unsigned)1);	*/
#endif
#endif
#if _BSD_SYSV || !defined(TCSETAW) || COHERENT
/* _BSD_SYSV needs to force TIOCSETN instead of TIOCSETP (preserve type-ahead) */
		(void)ioctl(0, TCSETA, &orig);
#else
		(void)ioctl(0, TCSETAW, &orig);
#endif
	}
	return prev;
}
#endif	/* _BSD */

static int
x_stuffreset(c)
{
#if defined TIOCSTI
	(void)x_stuff(c);
	return KINTR;
#else
	x_zotc(c);
	xcp = xep = xbuf;
	*xcp = 0;
	x_redraw(-1);
	return KSTD;
#endif
}

static int
x_stuff(c)
{
#if defined TIOCSTI
	char	ch = c;
	bool_t	savmode = set_xmode(FALSE);

	(void)ioctl(0, TIOCSTI, &ch);
	(void)set_xmode(savmode);
	x_redraw(-1);
#endif
	return KSTD;
}

static void
x_mapin(cp)
	char	*cp;
{
	char	*op;

	op = cp;
	while (*cp)  {
		/* XXX -- should handle \^ escape? */
		if (*cp == '^')  {
			cp++;
			if (*cp >= '?')	/* includes '?'; ASCII */
				*op++ = CTRL(*cp);
			else  {
				*op++ = '^';
				cp--;
			}
		} else
			*op++ = *cp;
		cp++;
	}
	*op = 0;
}

static char *
x_mapout(c)
	int c;
{
	static char buf[8];
	register char *p = buf;

	if (c < ' ' || c == 0x7F)  { /* ASCII */
		*p++ = '^';
		*p++ = (c == 0x7F) ? '?' : (c | 0x40);
	} else
		*p++ = c;
	*p = 0;
	return buf;
}

static void
x_print(prefix, key)
	int prefix, key;
{
	if (prefix == 1)
		shellf("%s", x_mapout(x_prefix1));
	if (prefix == 2)
		shellf("%s", x_mapout(x_prefix2));
	shellf("%s = ", x_mapout(key));
	if (x_tab[prefix][key]->xf_func != x_ins_string)
		shellf("%s\n", x_tab[prefix][key]->xf_name);
	else
		shellf("'%s'\n", x_atab[prefix][key]);
}

void
x_bind(a1, a2, macro)
	char *a1, *a2;
	int macro;		/* bind -m */
{
	struct x_ftab Const *fp;
	int prefix, key;
	char *sp = NULL;

	if (x_tab == NULL)
		errorf("cannot bind, not a tty\n");

	if (a1 == NULL) {
		for (prefix = 0; prefix < 3; prefix++)
		    for (key = 0; key < 0x80; key++) {
			fp = x_tab[prefix][key];
			if (fp == NULL ||
			    fp->xf_func == x_insert || fp->xf_func == x_error)
				continue;
			x_print(prefix, key);
		    }
		return;
	}

	x_mapin(a1);
	prefix = key = 0;
	for (;; a1++) {
		key = *a1;
		if (x_tab[prefix][key]->xf_func == x_meta1)
			prefix = 1;
		else
		if (x_tab[prefix][key]->xf_func == x_meta2)
			prefix = 2;
		else
			break;
	}

	if (a2 == NULL) {
		x_print(prefix, key);
		return;
	}

	if (*a2 == 0)
		fp = xft_insert;
	else if (!macro) {
		for (fp = x_ftab; fp->xf_func; fp++)
			if (strcmp(fp->xf_name, a2) == 0)
				break;
		if (fp->xf_func == NULL || (fp->xf_flags & XF_NOBIND))
			errorf("%s: no such function\n", a2);
		if (fp->xf_func == x_meta1)
			x_prefix1 = key;
		if (fp->xf_func == x_meta2)
			x_prefix2 = key;
	} else {
		fp = xft_ins_string;
		x_mapin(a2);
		sp = strsave(a2, AEDIT);
	}

	if ((x_tab[prefix][key]->xf_flags & XF_ALLOC) && x_atab[prefix][key])
		afree((Void *)x_atab[prefix][key], AEDIT);
	x_tab[prefix][key] = fp;
	x_atab[prefix][key] = sp;
}

void
x_init()
{
	register int i, j;
	struct x_ftab Const *fp;

	ainit(AEDIT);

	x_tab = alloc(sizeofN(*x_tab, 3), AEDIT);
	for (j = 0; j < 128; j++)
		x_tab[0][j] = xft_insert;
	for (i = 1; i < 3; i++)
		for (j = 0; j < 128; j++)
			x_tab[i][j] = xft_error;
	for (fp = x_ftab; fp->xf_func; fp++)
		if (fp->xf_db_char || fp->xf_db_tab)
			x_tab[fp->xf_db_tab][fp->xf_db_char] = fp;

	x_atab = alloc(sizeofN(*x_atab, 3), AEDIT);
	for (i = 1; i < 3; i++)
		for (j = 0; j < 128; j++)
			x_atab[i][j] = NULL;
}

#if SILLY
static int
x_game_of_life(c)  {
	char	newbuf [256+1];
	register char *ip, *op;
	int	i, len;

	i = xep - xbuf;
	*xep = 0;
	len = x_size_str(xbuf);
	xcp = xbuf;
	memmove(newbuf+1, xbuf, i);
	newbuf[0] = 'A';
	newbuf[i] = 'A';
	for (ip = newbuf+1, op = xbuf; --i >= 0; ip++, op++)  {
		/*  Empty space  */
		if (*ip < '@' || *ip == '_' || *ip == 0x7F)  {
			/*  Two adults, make whoopee */
			if (ip[-1] < '_' && ip[1] < '_')  {
				/*  Make kid look like parents.  */
				*op = '`' + ((ip[-1] + ip[1])/2)%32;
				if (*op == 0x7F) /* Birth defect */
					*op = '`';
			}
			else
				*op = ' ';	/* nothing happens */
			continue;
		}
		/*  Child */
		if (*ip > '`')  {
			/*  All alone, dies  */
			if (ip[-1] == ' ' && ip[1] == ' ')
				*op = ' ';
			else	/*  Gets older */
				*op = *ip-'`'+'@';
			continue;
		}
		/*  Adult  */
		/*  Overcrowded, dies */
		if (ip[-1] >= '@' && ip[1] >= '@')  {
			*op = ' ';
			continue;
		}
		*op = *ip;
	}
	*op = 0;
	x_redraw(len);
	return KSTD;
}
#endif

/*
 *	File/command name completion routines
 */

/* type: 0 for list, 1 for completion */

static	XPtrV words;

static void
add_stash(dirnam, name)
	char *dirnam;	/* directory name, if file */
	char *name;
{
	char *cp;
	register int type = 0;	/* '*' if executable, '/' if directory, else 0 */
	register int len = strlen(name);

	/* determine file type */
	if (dirnam)  {
		struct stat statb;
		char *buf = alloc((size_t)(strlen(dirnam)+len+2), ATEMP);

		if (strcmp(dirnam, ".") == 0)
			*buf = '\0';
		else if (strcmp(dirnam, "/") == 0)
			(void)strcpy(buf, "/");
		else
			(void)strcat(strcpy(buf, dirnam), "/");
		(void)strcat(buf, name);
		if (stat(buf, &statb)==0)
			if (S_ISDIR(statb.st_mode))
				type = '/';
			else if (S_ISREG(statb.st_mode) && eaccess(buf, 01)==0)
				type = '*';
		if (type)
			++len;
		afree((Void *)buf, ATEMP);
	}

	if (len > x_maxlen)
		x_maxlen = len;

	/* stash name for later sorting */
	cp = alloc((size_t)(len+1), ATEMP);
	(void)strcpy(cp = alloc((size_t)(len+1), ATEMP), name);
	if (dirnam && type)  {	/* append file type indicator */
		cp[len-1] = type;
		cp[len] = '\0';
	}
	XPput(words, cp);
}

static void
list_stash()
{
	register char **array, **record;
	int items = 0, tabstop, loc, nrows, jump, offset;

	items = XPsize(words);
	array = (char**) XPptrv(words);
	if (items == 0)
		return;
	qsortp(XPptrv(words), (size_t)XPsize(words), xstrcmp);

	/* print names */
	x_maxlen = (x_maxlen/8 + 1) * 8;	/* column width */
	nrows = (items-1) / (x_cols/x_maxlen) + 1;
	for (offset = 0; offset < nrows; ++offset)  {
		tabstop = loc = 0;
		x_putc('\n');
		for (jump = 0; offset+jump < items; jump += nrows)  {
			if (jump)
				while (loc < tabstop)  {
					x_putc('\t');
					loc = (loc/8 + 1) * 8;
				}
			record = array + (offset + jump);
			x_puts(*record);
			afree((Void *)*record, ATEMP);
			loc += strlen(*record);
			tabstop += x_maxlen;	/* next tab stop */
		}
	}

	afree((Void*)array, ATEMP);
	x_redraw(-1);
}

static int
x_comp_comm(c)  {
	compl_command(1);
	return KSTD;
}
static int
x_list_comm(c)  {
	compl_command(0);
	return KSTD;
}
static int
x_complete(c)  {
	compl_dec(1);
	return KSTD;
}
static int
x_enumerate(c)  {
	compl_dec(0);
	return KSTD;
}
static int
x_comp_file(c)   {
	compl_file(1);
	return KSTD;
}
static int
x_list_file(c)  {
	compl_file(0);
	return KSTD;
}

static void
compl_dec(type)
{
	char	*cp;
	cp = xcp;

	while (cp != xbuf && !isfs(*cp))
		cp--;
	if (cp == xbuf && strchr(cp, '/') == NULL)
		compl_command(type);
	else
		compl_file(type);
}

static void
compl_file(type)
{
	char	*str;
	register char *cp, *xp;
	char	*lastp;
	char	*dirnam;
	char	buf [256+1];
	char	bug [256+1];
	DIR    *dirp;
	struct dirent *dp;
	long	loc = -1;
	int	len;
	int	multi = 0;

	str = xcp;
	cp = buf;
	xp = str;
	while (xp != xbuf)  {
		--xp;
		if (isfs(*xp))  {
			xp++;
			break;
		}
	}
	if (digit(*xp) && (xp[1] == '<' || xp[1] == '>'))
		xp++;
	while (*xp == '<' || *xp == '>')
		xp++;
	if (type)
		while (*xcp && !isfs(*xcp))
			x_zotc(*xcp++);
	else {
		x_maxlen = 0;
		XPinit(words, 16);
	}
	while (*xp && !isfs(*xp))
		*cp++ = *xp++;

	*cp = 0;
	strcpy(buf, cp = substitute(buf, DOTILDE));
	afree((Void*)cp, ATEMP);
	lastp = strrchr(buf, '/');
	if (lastp)
		*lastp = 0;

	dirnam = (lastp == NULL) ? "." : (lastp == buf) ? "/" : buf;
	dirp = opendir(dirnam);
	if (dirp == NULL) {
		x_putc(BEL);
		return;
	}

	if (lastp == NULL)
		lastp = buf;
	else
		lastp++;
	len = strlen(lastp);

	while ((dp = readdir(dirp)) != NULL)  {
		cp = dp->d_name;
		if (cp[0] == '.' &&
		    (cp[1] == '\0' || cp[1] == '.' && cp[2] == '\0'))
			continue; /* always ignore . and .. */
		if (strncmp(lastp, cp, len) == 0)
			if (type)  {
				if (loc == -1)  {
					(void)strcpy(bug, cp);
					loc = strlen(cp);
				} else {
					multi = 1;
					loc = strmatch(bug, cp);
					bug[loc] = 0;
				}
			} else
				add_stash(dirnam, cp);
	}
	(void)closedir(dirp);

	if (type) {
		if (loc <= 0)  {
			x_putc(BEL);
			return;
		}
		cp = bug + len;
		x_ins(cp);
		if (!multi)  {
			struct stat statb;
			if (lastp == buf)
				buf[0] = 0;
			else if (lastp == buf + 1)  {
				buf[1] = 0;
				buf[0] = '/';
			}  else
				(void)strcat(buf, "/");
			(void)strcat(buf, bug);
			if (stat(buf, &statb) == 0 && S_ISDIR(statb.st_mode))
				x_ins("/");
			else
				x_ins(" ");
		}
	} else
		list_stash();
}

static void
compl_command(type)
{
	register struct tbl *tp;
	char	*str;
	char	buf [256+1];
	char	bug [256+1];
	char	*xp;
	char	*cp;
	int  len;
	int  multi;
	int  loc;

	str = xcp;
	cp = buf;
	xp = str;
	while (xp != xbuf)  {
		--xp;
		if (isfs(*xp))  {
			xp++;
			break;
		}
	}
	if (type)
		while (*xcp && !isfs(*xcp))
			x_zotc(*xcp++);
	else {
		x_maxlen = 0;
		XPinit(words, 16);
	}
	while (*xp && !isfs(*xp))
		*cp++ = *xp++;
	*cp = 0;

	len = strlen(buf);
	loc = -1;
	multi = 0;

	for (twalk(&commands); (tp = tnext()) != NULL; ) {
		int	klen;

		if (!(tp->flag&ISSET))
			continue;
		klen = strlen(tp->name);
		if (klen < len)
			return;
		if (strncmp(buf, tp->name, len) ==0)
			if (type)  {
				if (loc == -1)  {
					(void)strcpy(bug, tp->name);
					loc = klen;
				} else {
					multi = 1;
					loc = strmatch(bug, tp->name);
					bug[loc] = 0;
				}
			} else
				add_stash((char *)0, tp->name);
	}

	if (type)  {
		if (loc <= 0)  {
			x_putc(BEL);
			return;
		}
		cp = bug + len;
		x_ins(cp);
		if (!multi)
			x_ins(" ");
	} else
		list_stash();
}

static int
strmatch(s1, s2)
	register char *s1, *s2;
{
	register char *p;

	for (p = s1; *p == *s2++ && *p != 0; p++)
		;
	return p - s1;
}

#endif /* EDIT */

