
#line 20 "parse.y"
typedef union {
	NODE	      *	yu_node;
	char	      *	yu_strp;
	CSTRING	      *	yu_cstrp;
	int		yu_nval;
} YYSTYPE;
#define	YYSTYPE	YYSTYPE
#define _ANDF 256
#define _ASGN 257
#define _CASE 258
#define _CBRAC 259
#define _DO 260
#define _DONE 261
#define _DSEMI 262
#define _ELIF 263
#define _ELSE 264
#define _ESAC 265
#define _FI 266
#define _FOR 267
#define _IF 268
#define _IN 269
#define _IORS 270
#define _NAME 271
#define _NULL 272
#define _OBRAC 273
#define _ORF 274
#define _PARENS 275
#define _RET 276
#define _THEN 277
#define _UNTIL 278
#define _WHILE 279
#define _NOT 280
#ifdef YYTNAMES
extern struct yytname
{
	char	 * tn_name;
	int	tn_val;
} yytnames [];
#endif
extern	YYSTYPE	yylval;
