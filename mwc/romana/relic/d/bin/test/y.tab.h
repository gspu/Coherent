
#line 19 "test.y"
typedef union {
	NODE *nodeptr;
	char *fname;
	} YYSTYPE;
#define OR 256
#define AND 257
#define _R 258
#define _W 259
#define _F 260
#define _D 261
#define _S 262
#define _T 263
#define _Z 264
#define _N 265
#define SEQ 266
#define SNEQ 267
#define _EQ 268
#define _NE 269
#define _GT 270
#define _GE 271
#define _LT 272
#define _LE 273
#define STR 275
#ifdef YYTNAMES
extern struct yytname
{
	char	*tn_name;
	int	tn_val;
} yytnames[];
#endif
extern	YYSTYPE	yylval;
