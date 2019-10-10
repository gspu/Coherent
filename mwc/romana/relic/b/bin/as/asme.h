/*
 * 80386 assembler externs.
 */

/* main */
void dodefs();		/* process -D and -E options at each pass */

/* getargs */
extern int optix;	/* reset to 1 to reporcess */
extern char *optarg;	/* Global argument pointer */
extern int getargs();	/* fancy get arguments */

/* hash */
unsigned short hash();

/* symbol handlers */
void symInit();		/* init symbol tables */
unsigned short symGlob(); /* if -g mark new symbols global */
void symDump();		/* dump string table if coff output */
sym *symLookUp();	/* look up a symbol and maybe create one */
short  opLookUp();	/* look up an opcode on the symbol table */
void  opDelete();	/* remove an entry form the opcode table */
macro *macLookUp();	/* look up a macro */
void defMac();		/* create a new macro name */

/* ind.c */
void indBra();		/* indef branch */
int  indPass();		/* Do we need another pass ? */

/* c_out.c */
void segment();
void section();

/* common subs */
FILE *xopen();			/* open a file or die in the attempt */
char *scpy();			/* make a new copy of a string */
char *gcpy();			/* copy into tmp space */
void yyerror();			/* display error msg */
void fatal();			/* put a msg and die */
char *trim();			/* trim trailing spaces and tabs */
short  countList();		/* count things with next or prev ptrs */
char *memset();
void freeLevel();		/* pop a logic level */
void newLevel();		/* push a logic level */
char *parmFind();		/* infd a parm by number */
void doShift();			/* shift parms by 1 */
char *lookList();		/* get a parm by name */
void fileOpen();		/* open a file & creat ctl block */
void buildlab();		/* build a label */
void labelIgnored();		/* if label given print error message */

/* space.c functions */
void freeList();	/* free list connected by next ptrs */
char *alloc();		/* get space or die */
void initStor();	/* init sotorage functions */
char *galloc();		/* alloc and remember it */
void umark();		/* forget a galloc() */
void freel();		/* free all galloc()ed areas not umarked */
void umList();		/* umark a list */
expr *xalloc();		/* get an expr block */
sym *copySym();		/* make a copy of a sym block */
data *gitem();		/* get misc data item space */

/* common data */
extern char bswitch;	/* reverse bracket sense */
extern char fswitch;	/* reverse order of operands */
extern char gswitch;	/* make symbols of type S_UNDEF global */
extern char lswitch;	/* print a listing */
extern char mlistsw;	/* print macro expansion */
extern char pswitch;	/* print headers */
extern char rswitch;	/* don't use % on register names */
extern char Qswitch;	/* shut down all messages */
extern char wswitch;	/* shut down as warnings */
extern char xswitch;	/* don't output local symbols in object */
extern char nswitch;	/* no fixes for chip errata */
extern char Xswitch;	/* don't output local .L symbols in object */
extern char alignon;	/* automatically align data objects */
extern char alignonX;

extern short errCt;	/* count of errors */
extern short kind;	/* the kind of the current opcode */
extern short lastToken;	/* last token returned to yacc */
extern short longMode;	/* Which Mode current 386 or 286 */
extern unsigned lflags; /* Addressing mode flags */
extern char *title;	/* title header */
extern char *dTime;	/* time of compile */
extern char *lastL;	/* last line read for printing */
extern short lineSize;	/* listing line length */
extern short linect;	/* line counter */
extern short nlpp;	/* lines per page */
extern short pass;	/* current pass number */
extern int statement;	/* statement number */
extern inpctl *inpc;	/* file stack */
extern macro *inMacDef;	/* in macro definition */
extern macro *macFound;	/* the macro found by lex.c */
extern macctl *macExp;	/* in macro expansion */
extern macctl *logic;	/* logical level of control */
extern macctl *trueMac;	/* get the real macro being expanded */
extern macline *lastDef;/* last macro line defined */
extern short macNo;	/* macro expansion number */
extern short defCt;	/* number of defines */
extern char *outName;	/* name of output file */
extern char lswitchX;	/* storage for lswitch from -l */
extern char fswitchX;	/* storage for fswitch from -f */
extern char bswitchX;	/* storage for bswitch from -b */
extern char wswitchX;	/* storage for wswitch from -w */
extern char nswitchX;	/* storage for nswitch from -n */
extern short pcnt, bcnt; /* count op parens and brackets + left - right */
extern short choices;	 /* number of ways to do this op */
extern char xpass;	/* set if a branch changes size */
extern long showSym;
extern FILE *errdev;	/* Where to put out errors */

/* from libs */
char *strcpy();
char *strstr();
char *strchr();
char *strrchr();
char *memcpy();
char *ctime();
char *realloc();
long atol();
long time();
double strtod();
void free();
void exit();

/* tables from symtab.c */
extern symt typTab[];
extern char charLump[];
extern opc prefTab[];
extern nhash hashCodes[];
extern psym symtab[];

