
#line 259 "asm.y"
typedef union {
	long	val;	/* numeric value */
	double  dbl;
	sym	*s;	/* name size loc bitd bitl flag */
	opc	*o;	/* opcode kind */
	expr	*e;	/* mode loc size exp bitd bitl chain next */
	char	*t;	/* token */
	parm	*p;	/* parm  */
	data	*d;	/* data item */
	} YYSTYPE;
#define PC 256
#define NUMBER 257
#define FNUM 258
#define COMPARISON 312
#define OP 260
#define DATA 261
#define CMD 262
#define DCMD 263
#define ICMD 264
#define NCMD 265
#define ECMD 266
#define ECMDX 267
#define IDENTIFIER 268
#define REG 269
#define DEFINED 320
#define SIZEOF 321
#define SEGMENT 322
#define LOCATION 323
#define PLUS 315
#define MINUS 316
#define TIMES 317
#define DIVIDE 318
#define REM 319
#define LSHIFT 313
#define RSHIFT 314
#define AND 311
#define OR 309
#define XOR 310
#define COMMA 305
#define LPAREN 328
#define RPAREN 286
#define LBRACK 329
#define RBRACK 288
#define AT 307
#define D_SIGN 290
#define NOT 324
#define BANG 326
#define COLON 306
#define PARMCT 294
#define TOSTRING 330
#define TONUMBER 331
#define TOFLOAT 332
#define FSTACK 298
#define NL 333
#define TOKEN 327
#define P_SIGN 308
#define LEN 325
#ifdef YYTNAMES
extern struct yytname
{
	char	*tn_name;
	int	tn_val;
} yytnames[];
#endif
extern	YYSTYPE	yylval;
