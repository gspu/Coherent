/*
 * n0/cc0key.c
 * C compiler.
 * Symbol table initialization.
 */

#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Initialized tokens.
 * Each of these gets linked in place into the hash table.
 * This is a truly sleazy hack; each cxfoo string ends up at the right
 * place to be the t_id member of the TOK.
 */
static TOK txfile=	{NULL,NULL};	static char cxfile[]=	"__FILE__";
static TOK txuline=	{NULL,NULL};	static char cxuline[]=	"__LINE__";
static TOK txdate=	{NULL,NULL};	static char cxdate[]=	"__DATE__";
static TOK txtime=	{NULL,NULL};	static char cxtime[]=	"__TIME__";
#if	0
/*
 * ANSI C says __STDC__ should be defined as 0 for a non-ANSI compiler,
 * but too many sources use #ifdef __STDC__ when they should use #if __STDC__.
 * This is therefore conditionalized out here and in n0/expand.c.
 */
static TOK txstdc=	{NULL,NULL};	static char cxstdc[]=	"__STDC__";
#endif
static TOK txbasefile=	{NULL,NULL};	static char cxbasefile[]="__BASE_FILE__";
static TOK txudefined=	{NULL,NULL};	static char cxudefined[]="defined";
static TOK txdefine=	{NULL,NULL};	static char cxdefine[]=	"#define";
static TOK txinclude=	{NULL,NULL};	static char cxinclude[]="#include";
static TOK txundef=	{NULL,NULL};	static char cxundef[]=	"#undef";
static TOK txline=	{NULL,NULL};	static char cxline[]=	"#line";
static TOK txassert=	{NULL,NULL};	static char cxassert[]=	"#assert";
static TOK txerror=	{NULL,NULL};	static char cxerror[]=	"#error";
static TOK txpragma=	{NULL,NULL};	static char cxpragma[]=	"#pragma";
static TOK txif=	{NULL,NULL};	static char cxif[]=	"#if";
static TOK txifdef=	{NULL,NULL};	static char cxifdef[]=	"#ifdef";
static TOK txifndef=	{NULL,NULL};	static char cxifndef[]=	"#ifndef";
static TOK txelse=	{NULL,NULL};	static char cxelse[]=	"#else";
static TOK txelif=	{NULL,NULL};	static char cxelif[]=	"#elif";
static TOK txendif=	{NULL,NULL};	static char cxendif[]=	"#endif";
static TOK txident=	{NULL,NULL};	static char cxident[]=	"#ident";
static TOK tint=	{NULL,NULL};	static char cint[]=	"int";
static TOK tchar=	{NULL,NULL};	static char cchar[]=	"char";
static TOK tfloat=	{NULL,NULL};	static char cfloat[]=	"float";
static TOK tdouble=	{NULL,NULL};	static char cdouble[]=	"double";
static TOK tunsigned=	{NULL,NULL};	static char cunsigned[]="unsigned";
static TOK tsigned=	{NULL,NULL};	static char csigned[]=	"signed";
static TOK tregister=	{NULL,NULL};	static char cregister[]="register";
static TOK tlong=	{NULL,NULL};	static char clong[]=	"long";
static TOK tstruct=	{NULL,NULL};	static char cstruct[]=	"struct";
static TOK tunion=	{NULL,NULL};	static char cunion[]=	"union";
static TOK tenum=	{NULL,NULL};	static char cenum[]=	"enum";
static TOK tauto=	{NULL,NULL};	static char cauto[]=	"auto";
static TOK tvoid=	{NULL,NULL};	static char cvoid[]=	"void";
static TOK tstatic=	{NULL,NULL};	static char cstatic[]=	"static";
static TOK textern=	{NULL,NULL};	static char cextern[]=	"extern";
static TOK tgoto=	{NULL,NULL};	static char cgoto[]=	"goto";
static TOK treturn=	{NULL,NULL};	static char creturn[]=	"return";
static TOK tif=		{NULL,NULL};	static char cif[]=	"if";
static TOK twhile=	{NULL,NULL};	static char cwhile[]=	"while";
static TOK tfor=	{NULL,NULL};	static char cfor[]=	"for";
static TOK tdo=		{NULL,NULL};	static char cdo[]=	"do";
static TOK telse=	{NULL,NULL};	static char celse[]=	"else";
static TOK tswitch=	{NULL,NULL};	static char cswitch[]=	"switch";
static TOK tcase=	{NULL,NULL};	static char ccase[]=	"case";
static TOK tdefault=	{NULL,NULL};	static char cdefault[]=	"default";
static TOK tbreak=	{NULL,NULL};	static char cbreak[]=	"break";
static TOK tcontinue=	{NULL,NULL};	static char ccontinue[]="continue";
static TOK ttypedef=	{NULL,NULL};	static char ctypedef[]=	"typedef";
static TOK tsizeof=	{NULL,NULL};	static char csizeof[]=	"sizeof";
static TOK tshort=	{NULL,NULL};	static char cshort[]=	"short";
static TOK tconst=	{NULL,NULL};	static char cconst[]=	"const";
static TOK tvolatile=	{NULL,NULL};	static char cvolatile[]="volatile";
#ifdef READONLY
static TOK treadonly=	{NULL,NULL};	static char creadonly[]="readonly";
#endif
#ifdef ALIEN
static TOK talien=	{NULL,NULL};	static char calien[]=	"alien";
#endif

/*
 * Initialized symbols.
 * Each of these gets linked in place into the symbol table,
 * and the token each symbol specifies is entered into the token hash.
 * This must be correspond to the symbol order in ktok[] below.
 */
static KEYSYM ktab[] = {
	NULL,	SL_CPP,		XUFILE,
	NULL,	SL_CPP,		XULINE,
	NULL,	SL_CPP,		XUDATE,
	NULL,	SL_CPP,		XUTIME,
#if	0
	NULL,	SL_CPP,		XUSTDC,
#endif
	NULL,	SL_CPP,		XUBASE,
	NULL,	SL_CPP,		XDEFINED,
	NULL,	SL_CPP,		XDEFINE,
	NULL,	SL_CPP,		XINCLUDE,
	NULL,	SL_CPP,		XUNDEF,
	NULL,	SL_CPP,		XLINE,
	NULL,	SL_CPP,		XASSERT,
	NULL,	SL_CPP,		XERROR,
	NULL,	SL_CPP,		XPRAGMA,
	NULL,	SL_CPP,		XIF,
	NULL,	SL_CPP,		XIFDEF,
	NULL,	SL_CPP,		XIFNDEF,
	NULL,	SL_CPP,		XELSE,
	NULL,	SL_CPP,		XELIF,
	NULL,	SL_CPP,		XENDIF,
	NULL,	SL_CPP,		XIDENT,
	NULL,	SL_KEY,		INT,
	NULL,	SL_KEY,		CHAR,
	NULL,	SL_KEY,		FLOAT,
	NULL,	SL_KEY,		DOUBLE,
	NULL,	SL_KEY,		UNSIGNED,
	NULL,	SL_KEY,		SIGNED,
	NULL,	SL_KEY,		REGISTER,
	NULL,	SL_KEY,		LONG,
	NULL,	SL_KEY,		STRUCT,
	NULL,	SL_KEY,		UNION,
	NULL,	SL_KEY,		ENUM,
	NULL,	SL_KEY,		AUTO,
	NULL,	SL_KEY,		VOID,
	NULL,	SL_KEY,		STATIC,
	NULL,	SL_KEY,		EXTERN,
	NULL,	SL_KEY,		GOTO,
	NULL,	SL_KEY,		RETURN,
	NULL,	SL_KEY,		IF,
	NULL,	SL_KEY,		WHILE,
	NULL,	SL_KEY,		FOR,
	NULL,	SL_KEY,		DO,
	NULL,	SL_KEY,		ELSE,
	NULL,	SL_KEY,		SWITCH,
	NULL,	SL_KEY,		CASE,
	NULL,	SL_KEY,		DEFAULT,
	NULL,	SL_KEY,		BREAK,
	NULL,	SL_KEY,		CONTINUE,
	NULL,	SL_KEY,		TYPEDEF,
	NULL,	SL_KEY,		SIZEOF,
	NULL,	SL_KEY,		SHORT,
	NULL,	SL_KEY,		CONST,
	NULL,	SL_KEY,		VOLATILE,
#ifdef READONLY
	NULL,	SL_KEY,		READONLY,
#endif
#ifdef ALIEN
	NULL,	SL_KEY,		ALIEN
#endif
};
#define	NKEYS	sizeof(ktab)/sizeof(KEYSYM)

/*
 * Keyword token pointers, used to initialize symbol table.
 * This must be correspond to the symbol order in ktab[] above.
 */
static TOK *ktok[] = {
	&txfile,
	&txuline,
	&txdate,
	&txtime,
#if	0
	&txstdc,
#endif
	&txbasefile,
	&txudefined,
	&txdefine,
	&txinclude,
	&txundef,
	&txline,
	&txassert,
	&txerror,
	&txpragma,
	&txif,
	&txifdef,
	&txifndef,
	&txelse,
	&txelif,
	&txendif,
	&txident,
	&tint,
	&tchar,
	&tfloat,
	&tdouble,
	&tunsigned,
	&tsigned,
	&tregister,
	&tlong,
	&tstruct,
	&tunion,
	&tenum,
	&tauto,
	&tvoid,
	&tstatic,
	&textern,
	&tgoto,
	&treturn,
	&tif,
	&twhile,
	&tfor,
	&tdo,
	&telse,
	&tswitch,
	&tcase,
	&tdefault,
	&tbreak,
	&tcontinue,
	&ttypedef,
	&tsizeof,
	&tshort,
	&tconst,
	&tvolatile,
#ifdef READONLY
	&treadonly,
#endif
#ifdef ALIEN
	&talien
#endif
};

/*
 * Enter reserved words into the hash table.
 */
kinit()
{
	register char	*p;
	register int	c;
	register TOK	**tpp, *tp;
	register KEYSYM *kp;

	for (kp = ktab, tpp = ktok; kp < &ktab[NKEYS]; ++kp) {
		tp = *tpp++;
#ifdef VALIEN
		if (kp->s_value==ALIEN && notvariant(VALIEN))
			continue;
#endif
#ifdef VREADONLY
		if (kp->s_value==READONLY && notvariant(VREADONLY))
			continue;
#endif
		p = tp->t_id;
		idhash = 0;
		while (c = *p++)
			idhash += c;
		idhash %= NHASH;
		tp->t_tp = hash0[idhash];
		hash0[idhash] = tp;
		tp->t_sym = kp;
	}
}

/* end of n0/cc0key.c */
