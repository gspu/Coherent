
#line 22 "../scn.y"
typedef union {
	char *string;
	int  val;
} YYSTYPE;
#define NAME 256
#define STRING 257
#define NUMBER 258
#define EXT 259
#define RONLY 260
#define LONG 261
#define DEFAULT 262
#define SKIP 263
#define GROUP 264
#define HELP 265
#define VERIFY 266
#define NL 267
#define DO 268
#define DONE 269
#define DESIG 270
#ifdef YYTNAMES
extern readonly struct yytname
{
	char	*tn_name;
	int	tn_val;
} yytnames[];
#endif
extern	YYSTYPE	yylval;
