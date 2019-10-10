/*
 * This header file contains the definitions used by
 * all parts of the C compiler parser (cc0)
 * and the C compiler preprocessor (cpp).
 * It includes the definitions of the reserved words,
 * the built-in types, and so forth.
 */
#define	CC0_H	CC0_H

#include <stdio.h>
#include <setjmp.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#include "INC$LIB:host.h"
#include "INC$LIB:ops.h"
#include "INC$LIB:var.h"
#include "INC$LIB:varmch.h"
#include "INC$LIB:cc0mch.h"
#include "INC$LIB:stream.h"
#include "INC$LIB:cppmch.h"
#else
#include "mch.h"
#include "host.h"
#include "ops.h"
#include "var.h"
#include "varmch.h"
#include "cc0mch.h"
#include "stream.h"
#include "cppmch.h"
#endif

/*
 * These defines adjust the sizes of the tables in the parser.
 * They can be changed to just about anything, 
 */
#define NHASH   67		/* Hash buckets */
#define	NDIRS	32		/* Number of include directories */
#define	NLEV	32		/* Nesting level of cpp constructs */
#define NDLEV	128		/* DSTACK size */
#define	NDBUF	1024		/* Size of the define buffer */
#define ARG0	0200		/* Base of formals in define bodies */
#define SET0	(ARG0+NARGS)	/* Base of hide set indices in text */
/* High parity characters are accepted in strings, but otherwise forbidden */
/* because they conflict with this scheme for packing macro formals and hide */
/* sets into the least space and overhead I can manage */

/*
 * cpp keywords and macro classes.
 * these hide at a distinct level
 * in the symbol table.
 */
#define	XUSER	0	/* User macro */
#define	XUSERA	1	/* User macro with arguments */
#define	XUFILE	2	/* Macro __FILE__ */
#define	XULINE	3	/* Macro __LINE__ */
#define XUDATE	4	/* Macro __DATE__ */
#define XUTIME	5	/* Macro __TIME__ */
#define XUSTDC	6	/* Macro __STDC__ */
#define XDEFINED 8	/* User macro defined() */
#define	XDEFINE	9	/* #define */
#define	XINCLUDE 10	/* #include */
#define	XUNDEF	11	/* #undef */
#define	XLINE	12	/* #line */
#define	XASSERT	13	/* #assert */
#define XERROR	14	/* #error */
#define XPRAGMA	15	/* #pragma */
#define	XIF	20	/* #if is the first conditional */
#define	XIFDEF	21	/* #ifdef */
#define	XIFNDEF	22	/* #ifndef */
#define	XELSE	23	/* #else */
#define XELIF	24	/* #elif */
#define	XENDIF	25	/* #endif */

/*
 * Tokens.
 * Character classes.
 * Other things that are C compiler parser specific.
 * By convention, these begin at 100 decimal.
 */
#define DOT	100		/* . */
#define ARROW	101		/* -> */
#define SEMI	102		/* ; */
#define SHARP	103		/* # */
#define JUNK	104		/* Junk character */
#define SKIP	105		/* Whitespace */
#define LBRACE	106		/* { */
#define RBRACE	107		/* } */
#define LBRACK	108		/* [ */
#define RBRACK	109		/* ] */
#define LPAREN	110		/* ( */
#define RPAREN	111		/* ) */
#define ID	112		/* All identifiers */
#define CON	113		/* Constants */
#define QUOTE	114		/* ' */
#define STRING	115		/* " */
#define HIGH0	116		/* High parity junk, 0200..0237 */
#define HIGH1	117		/* High parity junk, 0240..0277 */
#define HIGH2	118		/* High parity junk, 0300..0337 */
#define HIGH3	119		/* High parity junk, 0340..0377 */
#define BACKDIV	120		/* Back slash */

/*
 * Types.
 * Classes.
 * Do not change these unless you update the tables in "gcandt.c".
 */
#define INT	150		/* int */
#define CHAR	151		/* char */
#define FLOAT	152		/* float */
#define DOUBLE	153		/* double */
#define	VOID	154		/* void */
#define STRUCT	155		/* struct */
#define UNION	156		/* union */
#define ENUM	157		/* enum */
#define LONG	158		/* long */
#define SHORT	159		/* short */
#define UNSIGNED 160		/* unsigned */
#define EXTERN	161		/* extern */
#define STATIC	162		/* static */
#define REGISTER 163		/* register */
#define TYPEDEF	164		/* typedef */
#define AUTO	165		/* auto */
#define CONST	166		/* const */
#define VOLATILE 167		/* volatile */
#define SIGNED	168		/* signed */

/*
 * Other C reserved words.
 * These are in no particular order at all.
 */
#define DEFAULT	180		/* default */
#define DO	181		/* do */
#define ELSE	182		/* else */
#define GOTO	183		/* goto */
#define IF	184		/* if */
#define RETURN	185		/* return */
#define SWITCH	186		/* switch */
#define CONTINUE 187		/* continue */
#define FOR	188		/* for */
#define WHILE	189		/* while */
#define BREAK	190		/* break */
#define	CASE	191		/* case */

/* MWC proprietary keywords */
#define	READONLY 200		/* readonly */
#define ALIEN	 201		/* Other language interface */

/*
 * Types.
 */
#define T_NONE  0		/* No type (yet) */
#define T_CHAR  1		/* Char */
#define T_UCHAR 2		/* Unsigned char */
#define T_SHORT 3		/* Short */
#define T_USHORT 4		/* Unsigned short */
#define T_INT   5		/* Int */
#define T_UINT  6		/* Unsigned int */
#define T_PTR   7		/* Pointer */
#define T_LONG  8		/* Long */
#define T_ULONG 9		/* Unsigned long */
#define T_FLOAT 10		/* Float */
#define T_DOUBLE 11		/* Double */
#define	T_VOID	12		/* Void */
#define T_STRUCT 13		/* Struct */
#define T_FSTRUCT 14		/* Forward struct reference */
#define T_UNION 15		/* Union */
#define T_FUNION 16		/* Forward union reference */
#define T_ENUM  17		/* Enumeration */
#define T_FENUM 18		/* Forward enumeration reference */
#define T_LDOUBLE 19		/* Long double, should follow double NB */

/*
 * Storage classes.
 */
#define C_NONE  0		/* No class */
#define C_AUTO  1		/* Auto */
#define C_SIN   2		/* Static internal */
#define C_SEX   3		/* Static external */
#define C_GDEF  4		/* Global def. */
#define C_GREF  5		/* Global ref. */
#define C_ARG   6		/* Argument */
#define C_CXT   7		/* Contextual */
#define C_LAB   8		/* Label */
#define C_FREF  9		/* Forward ref. label */
#define C_KEY   10		/* Keyword */
#define C_TYPE  11		/* Typedef name */
#define C_REG   12		/* Register */
#define C_STAG  13		/* Structure tag */
#define C_UTAG  14		/* Union tag */
#define C_ETAG  15		/* Enumeration tag */
#define C_MOS   16		/* Member of structure */
#define C_MOU   17		/* Member of union */
#define C_MOE   18		/* Member of enumeration */
#define C_PAUTO 19		/* Parametric auto */
#define C_PREG  20		/* Parametric register */

/*
 * Dimensions.
 * A list of these begins at the "s_dp" of a symbol
 * and at the "e_dp" of an expression tree node.
 */
typedef	struct	dim {
	struct  dim *d_dp;		/* Link */
	short	d_type;			/* Type */
	sizeof_t d_bound;		/* Array bound */
}	DIM;

#define D_PTR   0			/* Pointer */
#define D_FUNC  1			/* Function returning */
#define D_ARRAY 2			/* Array */
#define D_MOSAR 3			/* MOS array */
#define D_CONST	4			/* const modifier */
#define D_VOLATILE 5			/* volatile modifier */

/*
 * An INFO structure keeps track of structure elements,
 * union elements and enumeration constants.
 * It is pointed to by the INFO links of symbols and expressions.
 * The reference count only considers symbol usages.
 */
typedef	struct	{
	short	i_refc;			/* Reference count */
	short	i_nsp;			/* Number of symbols */
	union {
		sizeof_t i_data0;
		int	 i_data1;
	} i_data;
	struct	sym *i_sp[];		/* The symbols */
}	INFO;

#define	i_size	i_data.i_data0		/* Structure or union size */
#define	i_type	i_data.i_data1		/* Base type of an enumeration */

/*
 * Symbol.
 * Symbols are kept in a hash table.
 * Each bucket has the keywords on the front.
 */

typedef struct symbol SYM;
typedef struct token TOK;
struct symbol {
	SYM	*s_sp;			/* Link */
	int	s_slevel;		/* Symbol table level */
	int     s_value;	        /* Value */
	int	s_dline;		/* Decl. line number */
	char    s_level;	        /* Block level */
	char    s_flag;			/* Flag */
	char    s_width;	        /* Field width */
	char    s_offset;		/* Bit offset */
	char	s_seg;			/* Segment */
	char    s_class;	        /* Class */
	char    s_type;			/* Type */
	DIM	*s_dp;			/* Dimensions */
	INFO	*s_ip;			/* Info */
	char	*s_id;			/* Name string */
};

/* Keyword symbols occupy less space */
typedef struct {
	SYM	*s_sp;
	int	s_slevel;
	int	s_value;
} KEYSYM;

/* Preprocessor symbols take a little more space */
typedef struct {
	SYM	*s_sp;
	int	s_slevel;
	int	s_value;
	int	s_narg;
#if ANSI_STUPID
	TOK	*s_targs;
#endif
	char	s_body[];
} CPPSYM;

/* C symbol s_flag values */
#define	S_RONLY	01			/* Readonly */
#define S_ALIEN	02			/* Other language */
#define S_USED  04			/* Used */
#define	S_TAG	010			/* Tag present */
#define	S_INIT	020			/* Initialized */
#define S_SYMB	040			/* Written to debug table */

/* Disjunct symbol table levels */
#define SL_CPP	-2			/* Cpp tokens */
#define SL_KEY	-1			/* C keywords and ops */
#define SL_VAR	0			/* Normal symbols */
#define SL_LAB	1			/* Labels */
#define SL_TAG	2			/* Tags */
#define SL_MOS	3			/* Members of structures, and up */

/* Lexical scope levels */
#define LL_EXT	0			/* Externals */
#define LL_ARG	1			/* Parameters */
#define LL_AUTO	2			/* Automatics, and up */

/*
 * All tokens are hashed.
 * Each unique token is the head
 * of a list of symbol buckets.
 */
struct token {
	TOK	*t_tp;
	SYM	*t_sym;
	char	t_id[];
};

/*
 * Macro expansion stack.
 */
typedef	struct {
	char	ds_type;		/* Type of stack item */
	char	ds_char;		/* Ungotten char or temp */
	char	*ds_ptr;		/* Pointer to string */
} DSTACK;

#define DS_FILE		0	/* Dstack is at source file level */
#define DS_UNGET	1	/* Dstack is ungotten character */
#define DS_FUNGET	2	/* Dstack is ungotten file character */
#define DS_STRNG	3	/* Dstack is normal string to read */
#define DS_PARAM	4	/* Dstack is parameter to be expanded */
#define DS_SHARP	5	/* Dstack is parameter to stringize */
#define DS_SHARP2	6	/* Dstack is parameter to glue */
#define DS_DPUTP	7	/* Dstack is pushed dputp pointer */
#define DS_NAME		8	/* Dstack is pushed name of macro */
#define DS_IEOF		9	/* Dstack is internal end of file */
#define DS_UFILE	10	/* Dstack is __FILE__ to expand */

/*
 * Macro hide set structure.
 */
typedef struct hideset HIDESET;
typedef struct hidemem HIDEMEM;
struct hideset {
	HIDESET	*h_next_set;
	HIDEMEM *h_this_set;
};
struct hidemem {
	HIDEMEM *h_next_mem;
	TOK	*h_this_mem;
};

/*
 * Conditional stack.
 * One of these is allocated when a #if* line
 * is encountered to store the state of the
 * conditional block until the matching #endif
 * is found.
 */
typedef struct {
	int		c_op;		/* Last operator seen in block */
	int		c_state;	/* Has any block been true */
} CSTACK;

/*
 * Include pushdown stack entry.
 * One of these is allocated when a #include line
 * is encountered to hold the file name, the line number
 * and the FILE pointer of the old source file.
 */
typedef	struct {
	FILE		*i_fp;		/* Saved FILE pointer */
	int		i_line;		/* Saved line */
	TOK		*i_file;	/* Saved file name */
	CSTACK		*i_cstackp;	/* Saved conditional stack state */
} ISTACK;

/*
 * These structures keep track of the labels and constants for switches.
 * It is used by code in the statement reader "stat.c".
 */
typedef struct {
	int	sc_lab;			/* Label number */
	ival_t	sc_val;			/* Value */
} SCASE;

typedef struct {
	int	sb_ncase;		/* # of cases */
	int	sb_dlab;		/* Default label */
	SCASE	sb_case[];		/* Adjustable array */
}	SBLOCK;

/*
 * Tree nodes.
 * All tree nodes are the same size.
 * A node of any type may be changed into a node of any other type.
 * The madness with the defines and the dots is just niceness
 * in the wake of wishing to compile under both Bell and UCB structure rules.
 */
typedef	struct tree TREE;
struct tree
{
	short   t_op;			/* Opcode */
	short   t_type;			/* Internal type (T_) */
	DIM	*t_dp;			/* DIM list */
	INFO	*t_ip;			/* INFO if aggregate */
	union	{
		struct	{
			TREE	*t_xlp;
			union	{
				TREE	*t_xrp;
				struct	{
					char	t_xfw;
					char	t_xfb;
				}	t_1;
			}	t_2;
		}	t_3;
		ival_t	t_xival;
		lval_t	t_xlval;
		dval_t	t_xdval;
		sizeof_t t_xzval;
		struct	{
			sizeof_t t_xoffs;
			int	t_xseg;
			union	{
				int	t_xlab;
				SYM	*t_xsp;
			}	t_4;
		}	t_5;
		short	t_xreg;
	}	t_6;
};

#define	t_lp	t_6.t_3.t_xlp		/* Left link */
#define	t_rp	t_6.t_3.t_2.t_xrp	/* Right link */
#define	t_width	t_6.t_3.t_2.t_1.t_xfw	/* Field width, bits */
#define	t_base	t_6.t_3.t_2.t_1.t_xfb	/* Field base bit */
#define	t_ival	t_6.t_xival		/* ICON value */
#define	t_lval	t_6.t_xlval		/* LCON value */
#define	t_dval	t_6.t_xdval		/* DCON value */
#define t_zval	t_6.t_xzval		/* ZCON value */
#define	t_reg	t_6.t_xreg		/* Register code */
#define t_offs	t_6.t_5.t_xoffs		/* Label offset */
#define	t_seg	t_6.t_5.t_xseg		/* Segment */
#define	t_label	t_6.t_5.t_4.t_xlab	/* Label # */
#define	t_sp	t_6.t_5.t_4.t_xsp	/* Global symbol link */

/*
 * Opdope flag bits.
 */
#define PRIO    037			/* Priority */
#define NSTL	040			/* No structures on left */
#define RAS     0100			/* Right associative */
#define RLVL    0200			/* Require lvalue on left */
#define NFLT    0400			/* No floats */
#define NPTR    01000			/* No pointers on right */
#define NPTL    02000			/* No pointers on left */
#define RTOR    04000			/* Require truth value on right */
#define RTOL    010000			/* Require truth value on left */
#define ASGN    020000			/* Assignment op */
#define NSTR    040000			/* No structures on right */

/*
 * Conversion dope bits.
 */
#define GOAL	077			/* Goal type */
#define CVRA	0100			/* Convert right on assignment */
#define CVR	0200			/* Convert right */
#define CVL	0400			/* Convert left */
#define GTL     01000			/* Goal type from left */
#define GTR     02000			/* Goal type from right */
#define UREL    04000			/* Unsigned relation check needed */
#define CARA	010000			/* Cast right on assignment */

/*
 * cpp variables.
 */
extern char	dbuf[NDBUF], *dputp, *dpshp;
extern DSTACK	dstack[], *dstackp, *dlistp;
extern ISTACK	istack[], *istackp;
extern CSTACK	cstack[], *cstackp;
extern HIDESET	*hidefree, *hidesets;
extern int	cstate;
extern int	lastchar;
extern int	instring;
extern int	notskip;
extern int	ndirs;
extern char	*incdirs[];
extern char	deftrue[];
extern char	deffalse[];
extern long	curtime;

/*
 * Functions and variables.
 */
extern  int     mysizes[];
extern  SYM	*cfsym;
extern  int     cflab;
extern  char    id[];
extern  unsigned     idhash;
extern	unsigned     idhide;
extern	int	idsize;
extern	TOK	*idp;
extern  TOK	*hash0[];
extern  int     llex;
extern	int	lsym;
extern  int     s;
extern  ival_t  ival;
extern  lval_t  lval;
extern	dval_t	dval;
extern	short	tval;
extern  int     line;
extern  char    file[];
extern	TOK	*tfile;
extern unsigned char ct[];
extern	TOK	*newtoken();
extern  SYM	*newsym();
extern	CPPSYM	*newcpp();
extern  DIM	*dupldim();
extern  DIM	*tackdim();
extern  INFO    *newinfo();
extern  char    *new();
extern  int     nerr;
extern	char	*passname;
extern  int     nargs;
extern  SYM	*args[];
extern	TREE	*expr();
extern	int	incpp;
extern  int     ininit;
extern	int	incase;
extern  char    *talloc();
extern	TREE	*transform();
extern  short   opdope[];
extern  short   cvdope[];
extern	TREE	*pexpr();
extern  DIM	*tdalloc();
extern	TREE	*build();
extern	TREE	*charray();
extern	TREE	*chfun();
extern	TREE	*bicon();
extern	TREE	*bzcon();
extern	TREE	*bid();
extern	TREE	*bcvt();
extern	TREE	*bconvert();
extern	TREE	*term();
extern	TREE	*tree();
extern	TREE	*bstring();
extern  char    mytypes[];
extern	TREE	*fold0();
extern	SYM	*deflookup();
extern	SYM	*reflookup();
extern	SYM	*moslookup();
extern	SYM	*fakedef();
extern	SYM	*taglookup();
extern	SYM	*fixlevel();
extern	SYM	*declare();
extern	long	fieldalign();
extern	sizeof_t ssize();
extern	sizeof_t tsize();
extern	sizeof_t sdsize();
extern	sizeof_t tidsize();
extern	sizeof_t psize();
extern	sizeof_t getbound();
extern	sizeof_t szcheck();	/* mch - object size check */
extern	TREE	*cast();	/* cast declarator reader */
extern	int	oldseg;
extern	int	mnamef;
extern	int	labgen;
extern	int	cblab;
extern	int	cclab;
extern	SBLOCK	*sbp;
extern	FILE	*ifp;
extern	FILE	*ofp;		/* Output file */

#if OVERLAID
extern	jmp_buf	death;		/* Fatal errors */
#endif
