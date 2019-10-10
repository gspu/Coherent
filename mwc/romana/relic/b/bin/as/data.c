/*
 * 80386 assembler common data
 */
#include <asm.h>

#ifdef GEM
long _stksize = 4096;
#endif
short errCt;		/* count of errors */
short kind;		/* the op->kind of current opcode */
short lastToken;	/* last token returned to yacc */
char gswitch;		/* make symbols of type S_NEW global */

char lswitch;		/* print a listing */
char pswitch;		/* print headers */
char mlistsw = 1;	/* print macro expansions */
char rswitch;		/* don't use % on register names */
char fswitch;		/* reverse order of operands */
char bswitch;		/* reverse bracket sense */
char wswitch;		/* shut off as warnings */
char nswitch;		/* place nop after E9 type jump */
char Qswitch;		/* no messages */
char xswitch;		/* don't put local symbols in object file */
char Xswitch;		/* don't put .L symbols in object file */
char *title;		/* title header */
char *dTime;		/* time of compile */
char *lastL;		/* last line read for listing */
char alignon;		/* automatically align data objects */
char alignonX;		/* alignon from switches */
short lineSize = 79;	/* listing line length */
short nlpp = 56;	/* default lines per page */
short linect = 56;	/* current line number */
short pass = 0;		/* current pass number */
int statement;		/* statment number */
inpctl *inpc;		/* file stack */
macro *inMacDef;	/* in macro definition */
macro *macFound;	/* the macro found by lex.c */
macctl *macExp;		/* in macro expansion */
macctl *trueMac;	/* get the real macro being expanded */
macctl *logic;		/* logical level of control */
macline *lastDef;	/* last macro line defined */
short macNo;		/* macro expansion number */
short defCt;		/* number of defines */
short longMode = 1;	/* 1 = 80386 mode, 0 = 80286 mode */
unsigned lflags;	/* is it long mode or not */
char *outName;		/* name of output file */
char lswitchX;		/* storage for lswitch from -l */
char fswitchX;		/* storage for fswitch from -f */
char bswitchX;		/* storage for bswitch from -b */
char wswitchX;		/* storage for wswitch from -w */
char nswitchX;		/* storage for nswitch from -n */
short pcnt, bcnt;	/* count op parens and brackets + left - right */
short choices;		/* Number of ways to do this opcode */
char xpass;		/* set if a branch changes size */

long showSym;		/* location of clobbered symbol */
FILE *errdev;		/* Where to put out errors */
