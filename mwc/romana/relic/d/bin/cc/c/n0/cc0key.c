/*
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
 * Each of these gets linked in place
 * into the hash table.
 */
static TOK txfile={0,0}; static char cxfile[]="__FILE__";
static TOK txuline={0,0}; static char cxuline[]="__LINE__";
static TOK txdate={0,0}; static char cxdate[]="__DATE__";
static TOK txtime={0,0}; static char cxtime[]="__TIME__";
static TOK txstdc={0,0}; static char cxstdc[]="__STDC__";
static TOK txudefined={0,0}; static char cxudefined[]="defined";
static TOK txdefine={0,0}; static char cxdefine[]="#define";
static TOK txinclude={0,0}; static char cxinclude[]="#include";
static TOK txundef={0,0}; static char cxundef[]="#undef";
static TOK txline={0,0}; static char cxline[]="#line";
static TOK txassert={0,0}; static char cxassert[]="#assert";
static TOK txerror={0,0}; static char cxerror[]="#error";
static TOK txif={0,0}; static char cxif[]="#if";
static TOK txifdef={0,0}; static char cxifdef[]="#ifdef";
static TOK txifndef={0,0}; static char cxifndef[]="#ifndef";
static TOK txelse={0,0}; static char cxelse[]="#else";
static TOK txelif={0,0}; static char cxelif[]="#elif";
static TOK txendif={0,0}; static char cxendif[]="#endif";
static TOK tint={0,0}; static char cint[]="int";
static TOK tchar={0,0}; static char cchar[]="char";
static TOK tfloat={0,0}; static char cfloat[]="float";
static TOK tdouble={0,0}; static char cdouble[]="double";
static TOK tunsigned={0,0}; static char cunsigned[]="unsigned";
static TOK tsigned={0,0}; static char csigned[]="signed";
static TOK tregister={0,0}; static char cregister[]="register";
static TOK tlong={0,0}; static char clong[]="long";
static TOK tstruct={0,0}; static char cstruct[]="struct";
static TOK tunion={0,0}; static char cunion[]="union";
static TOK tenum={0,0}; static char cenum[]="enum";
static TOK tauto={0,0}; static char cauto[]="auto";
static TOK tvoid={0,0}; static char cvoid[]="void";
static TOK tstatic={0,0}; static char cstatic[]="static";
static TOK textern={0,0}; static char cextern[]="extern";
static TOK tgoto={0,0}; static char cgoto[]="goto";
static TOK treturn={0,0}; static char creturn[]="return";
static TOK tif={0,0}; static char cif[]="if";
static TOK twhile={0,0}; static char cwhile[]="while";
static TOK tfor={0,0}; static char cfor[]="for";
static TOK tdo={0,0}; static char cdo[]="do";
static TOK telse={0,0}; static char celse[]="else";
static TOK tswitch={0,0}; static char cswitch[]="switch";
static TOK tcase={0,0}; static char ccase[]="case";
static TOK tdefault={0,0}; static char cdefault[]="default";
static TOK tbreak={0,0}; static char cbreak[]="break";
static TOK tcontinue={0,0}; static char ccontinue[]="continue";
static TOK ttypedef={0,0}; static char ctypedef[]="typedef";
static TOK tsizeof={0,0}; static char csizeof[]="sizeof";
static TOK tshort={0,0}; static char cshort[]="short";
static TOK tconst={0,0}; static char cconst[]="const";
static TOK tvolatile={0,0}; static char cvolatile[]="volatile";
#ifdef READONLY
static TOK treadonly={0,0}; static char creadonly[]="readonly";
#endif
#ifdef ALIEN
static TOK talien={0,0}; static char calien[]="alien";
#endif

/*
 * Initialized symbols.
 * Each of these gets linked in place
 * into the symbol table,
 * and the token each symbol specifies
 * is entered into the token hash.
 */
static KEYSYM ktab[] = {
	&txfile,	SL_CPP,		XUFILE,
	&txuline,	SL_CPP,		XULINE,
	&txdate,	SL_CPP,		XUDATE,
	&txtime,	SL_CPP,		XUTIME,
	&txstdc,	SL_CPP,		XUSTDC,
	&txudefined,	SL_CPP,		XDEFINED,
	&txdefine,	SL_CPP,		XDEFINE,
	&txinclude,	SL_CPP,		XINCLUDE,
	&txundef,	SL_CPP,		XUNDEF,
	&txline,	SL_CPP,		XLINE,
	&txassert,	SL_CPP,		XASSERT,
	&txerror,	SL_CPP,		XERROR,
	&txif,		SL_CPP,		XIF,
	&txifdef,	SL_CPP,		XIFDEF,
	&txifndef,	SL_CPP,		XIFNDEF,
	&txelse,	SL_CPP,		XELSE,
	&txelif,	SL_CPP,		XELIF,
	&txendif,	SL_CPP,		XENDIF,
	&tint,		SL_KEY,		INT,
	&tchar,		SL_KEY,		CHAR,
	&tfloat,	SL_KEY,		FLOAT,
	&tdouble,	SL_KEY,		DOUBLE,
	&tunsigned,	SL_KEY,		UNSIGNED,
	&tsigned,	SL_KEY,		SIGNED,
	&tregister,	SL_KEY,		REGISTER,
	&tlong,		SL_KEY,		LONG,
	&tstruct,	SL_KEY,		STRUCT,
	&tunion,	SL_KEY,		UNION,
	&tenum,		SL_KEY,		ENUM,
	&tauto,		SL_KEY,		AUTO,
	&tvoid,		SL_KEY,		VOID,
	&tstatic,	SL_KEY,		STATIC,
	&textern,	SL_KEY,		EXTERN,
	&tgoto,		SL_KEY,		GOTO,
	&treturn,	SL_KEY,		RETURN,
	&tif,		SL_KEY,		IF,
	&twhile,	SL_KEY,		WHILE,
	&tfor,		SL_KEY,		FOR,
	&tdo,		SL_KEY,		DO,
	&telse,		SL_KEY,		ELSE,
	&tswitch,	SL_KEY,		SWITCH,
	&tcase,		SL_KEY,		CASE,
	&tdefault,	SL_KEY,		DEFAULT,
	&tbreak,	SL_KEY,		BREAK,
	&tcontinue,	SL_KEY,		CONTINUE,
	&ttypedef,	SL_KEY,		TYPEDEF,
	&tsizeof,	SL_KEY,		SIZEOF,
	&tshort,	SL_KEY,		SHORT,
	&tconst,	SL_KEY,		CONST,
	&tvolatile,	SL_KEY,		VOLATILE,
#ifdef READONLY
	&treadonly,	SL_KEY,		READONLY,
#endif
#ifdef ALIEN
	&talien,	SL_KEY,		ALIEN,
#endif
	NULL
};

/*
 * Enter reserved words into the hash table.
 */
kinit()
{
	register char	*p;
	register int	c;
	register TOK	*tp;
	register KEYSYM *sp;

	sp = ktab;
	while ((tp = sp->s_sp) != NULL) {
#ifdef VALIEN
		if (sp->s_value==ALIEN && notvariant(VALIEN)) {
			++sp;
			continue;
		}
#endif
#ifdef VREADONLY
		if (sp->s_value==READONLY && notvariant(VREADONLY)) {
			++sp;
			continue;
		}
#endif
		p = tp->t_id;
		idhash = 0;
		while (c = *p++)
			idhash += c;
		idhash %= NHASH;
		tp->t_tp = hash0[idhash];
		hash0[idhash] = tp;
		tp->t_sym = sp;
		sp->s_sp = NULL;
		++sp;
	}
}

