#include "asm.h"

int	line;
int	inbss;
jmp_buf env;
int	page;
int	lop;
int	pass;
int	lflag;
int	xflag;
int	gflag;
int	eflag;
address	laddr;
address	fuzz;
int	lmode;
char	*ep;
char	eb[NERR];
char	*ip;
char	ib[NINPUT];
char	*cp;
char	cb[NCODE];
char	tb[NTIT];

struct	tsymp tsymp[10];
struct	sym *symhash[NHASH];
int	nloc;
int	nerr;
struct  loc  *loc[NLSEG];
struct  loc  *defloc;

FILE	*ofp;
FILE	*efp;
#if SEG
#if INTEL
char	*ofn	= "sl.hex";
#else
char	*ofn 	= "sl.out";
#endif
#else
#if INTEL
char	*ofn	= "l.hex";
#else
char	*ofn	= "l.out";
#endif
#endif
char	*ifn	= NULL;
FILE	*sfp	= NULL;

char	ctype[128] = {
	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,
	ILL,	SPACE,	ILL,	ILL,	SPACE,	ILL,	ILL,	ILL,
	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,
	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,	ILL,
	SPACE,	ETC,	ETC,	ETC,	ETC,	ETC,	ETC,	ETC,
	ETC,	ETC,	BINOP,	BINOP,	ETC,	BINOP,	LETTER,	ETC,
	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,
	DIGIT,	DIGIT,	ETC,	ETC,	ETC,	ETC,	ETC,	ETC,
	ETC,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	ETC,	ETC,	ETC,	BINOP,	LETTER,
	ETC,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	ETC,	BINOP,	ETC,	ETC,	ETC
};
