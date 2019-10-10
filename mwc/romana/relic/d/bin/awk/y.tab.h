
#line 24 "awk.y"
typedef union {
	int	u_char;
	CHAR	*u_charp;
	NODE	*u_node;
	int	(*u_func)();
} YYSTYPE;
#define IF_ 256
#define WHILE_ 257
#define FOR_ 258
#define ELSE_ 259
#define BREAK_ 260
#define CONTINUE_ 261
#define NEXT_ 262
#define EXIT_ 263
#define IN_ 264
#define PRINT_ 265
#define PRINTF_ 266
#define BEGIN_ 267
#define END_ 268
#define FAPPEND_ 269
#define FOUT_ 270
#define ID_ 271
#define STRING_ 272
#define NUMBER_ 273
#define FUNCTION_ 274
#define SCON_ 295
#define ASADD_ 296
#define ASSUB_ 297
#define ASMUL_ 298
#define ASDIV_ 299
#define ASMOD_ 300
#define OROR_ 301
#define ANDAND_ 302
#define NMATCH_ 303
#define EQ_ 304
#define NE_ 305
#define GE_ 306
#define LE_ 307
#define INC_ 308
#define DEC_ 309
#define REEOL_ 310
#define REBOL_ 311
#define REANY_ 312
#define RECLASS_ 313
#define RECHAR_ 314
#define REOR_ 315
#define RECON_ 316
#define RECLOS_ 317
#define RENECL_ 318
#define REZOCL_ 319
#ifdef YYTNAMES
extern struct yytname
{
	char	*tn_name;
	int	tn_val;
} yytnames[];
#endif
extern	YYSTYPE	yylval;
