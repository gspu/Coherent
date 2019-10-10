
#line 77 "expr.y"
typedef union {
	char *str;
} YYSTYPE;
#define STR 256
#define LE 258
#define GE 259
#define EQ 260
#define NEQ 261
#define UMINUS 262
#define LEN 263
#ifdef YYTNAMES
extern struct yytname
{
	char	*tn_name;
	int	tn_val;
} yytnames[];
#endif
extern	YYSTYPE	yylval;
