#define FOREACH 256
#define PATHNAME 257
#define STRING 258
#define INDENT 259
#define UNINDENT 260
#define NL 261
#define ERROR_TOKEN 262
#define WARNING_TOKEN 263
#define EXIST 264
#define OWNER 265
#define GROUP 266
#define CHMOD_PERMISSION 267
#define NUMBER 268
#define FILE_TOKEN 269
#define DIRECTORY 270
#define PIPE 271
#define CHARACTER_SPECIAL 272
#define BLOCK_SPECIAL 273
typedef	int	YYSTYPE;
#ifdef YYTNAMES
extern struct yytname
{
	char	*tn_name;
	int	tn_val;
} yytnames[];
#endif
extern	YYSTYPE	yylval;
