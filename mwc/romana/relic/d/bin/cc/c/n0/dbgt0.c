/*
 * cc0 - debug table and symbol table type information output.
 * all intermediate file items are of format:
 * DLABEL, class, lineno, name, optional value, type.
 */
#ifndef __LINE__
#define __LINE__ 0
#endif
#ifdef vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Called from cc0.c/main() for the original source file name.
 *
 * The compiler passes source filename information to the output
 * writer as DC_FILE debug records.  The output writer will
 * nest file name records, but csd is not prepared to deal with
 * nested source files so we mark all included source as part
 * of the line which included it.
 */
dbfname(f)
char *f;
{
	if (notvariant(VDEBUG) && notvariant(VLINES))
		return;
	bput(DLABEL);
	bput(DC_FILE);
	iput((ival_t) line);
	sput(f);
	bput(DT_NONE);
	zput((sizeof_t) 0);
}

/*
 * Called from stat.c/statement() at entry, LBRACE, and RBRACE.
 * Also in cc0.c/xfunc() for declaration brackets.
 */
dbstat(s, line)
int s;
int line;
{
	register int mys;

	if (line == 0 || (notvariant(VDEBUG) && notvariant(VLINES))
	 || s==CASE || s==DEFAULT || (s==ID && spnextis(':'))) {
		return;
	} else if (s==LBRACE) {
		mys = '{';
	} else if (s==RBRACE) {
		mys = '}';
	} else {
		mys = ';';
		if (notvariant(VLINES))
			return;
	}
	bput(DLABEL);
	bput(DC_LINE);
	iput((ival_t) (istackp<istack+NLEV ? istack[NLEV-1].i_line : line));
	bput(mys);
	bput(0);
	bput(DT_NONE);
	zput((sizeof_t) 0);
}

/*
 * Output a label identifier.
 * These work like line number records in cc2
 * so that the exact label number assigned is
 * irrelevant.
 */
dblabel(sp) SYM *sp;
{
	if (notvariant(VDEBUG))
		return;
	dbdecl(sp);
}

/*
 * Called directly for external and static declaration.
 * Arg lists are driven by dbargs(),
 * locals are filtered by dblocal(),
 * extrefs, typedefs, structs, unions and enums are driven by dblevel().
 */
dbdecl(sp) SYM *sp;
{
	if (notvariant(VDEBUG) && (notvariant(VTYPES) || sp->s_class==C_SEX))
		return;
	if (sp->s_seg==SALIEN)
		return;
	bput(DLABEL);
	dbpr(sp);
}

/*
 * Put out the parameters to a function.
 */
dbargs()
{
	register int i;
	register SYM *sp;

	if (notvariant(VDEBUG))
		return;
	for (i = 0; i < nargs; i += 1) {
		sp = args[i];
		if (sp->s_class == C_REG) {
			sp->s_class = C_PREG;
			dbdecl(sp);
			sp->s_class = C_REG;
		} else
			dbdecl(sp);
	}
}

/*
 * Output a local symbol declaration.
 * Forget about references.
 */
dblocal(sp) SYM *sp;
{
	register int class;

	if (notvariant(VDEBUG))
		return;
	class = sp->s_class;
	if (class!=C_GREF
	 && class!=C_GDEF
	 && class!=C_SEX
	 && class!=C_CXT
	 && class!=C_TYPE)
		dbdecl(sp);
}

/*
 * Output any debug information at the current lexical
 * level -- this will generally be typedef/struct/union/enum
 * definitions, and when llex is < LL_EXT the external
 * references which have been used.
 */
dbdown()
{
	register SYM *tsp;
	register int c;
	register TOK *tp;
	register int i;
	int changes;

	if (notvariant(VDEBUG) && notvariant(VTYPES))
		return;
	/* External references */
	if (llex < LL_EXT) {
		for (i=0; i < NHASH; i+=1)
		for (tp = hash0[i]; tp != NULL; tp = tp->t_tp) {
			for (tsp = tp->t_sym; tsp != NULL; tsp = tsp->s_sp) {
				if (tsp->s_slevel < SL_VAR)
					continue;
				c = tsp->s_class;
				if ((tsp->s_flag&S_USED) != 0
				 && (c==C_GREF || c==C_CXT))
					dbdecl(tsp);
			}
		}
	}
	/* Type definitions */
	do {
		changes = 0;
		for (i=0; i < NHASH; i+=1)
		for (tp = hash0[i]; tp != NULL; tp = tp->t_tp) {
			for (tsp = tp->t_sym; tsp != NULL; tsp = tsp->s_sp) {
				if (tsp->s_slevel < SL_VAR)
					continue;
				if (tsp->s_level <= llex)
					continue;
				c = tsp->s_class;
				if (istype(c)
				 && (tsp->s_flag&S_USED) != 0
				 && (tsp->s_flag&S_SYMB) == 0) {
					tsp->s_flag |= S_SYMB;
					changes += 1;
					dbdecl(tsp);
				}
			}
		}
	} while (changes);
}

/*
 * Given a symbol, see if typedef or tagged type is defined for it.
 */
static
SYM *
findtype(sp)
register SYM *sp;
{
	register int i;

	i = sp->s_type;
	if (i==T_FSTRUCT || i==T_FUNION || i==T_FENUM)
		return (sp->s_ip);
	if (i!=T_STRUCT && i!=T_UNION && i!=T_ENUM)
		return (NULL);
	return (taglookup(sp->s_ip));
}

/*
 * Write a symbol entry in intermediate format.
 */

static char tmap[] = {
	DT_NONE,
	DT_CHAR,
	DT_UCHAR,
	DT_SHORT,
	DT_USHORT,
	DT_INT,
	DT_UINT,
	-1,		/* T_PTR */
	DT_LONG,
	DT_ULONG,
	DT_FLOAT,
	DT_DOUBLE,
	DT_VOID,
	DT_STRUCT,
	DT_STRUCT,	/* T_FSTRUCT */
	DT_UNION,
	DT_UNION,	/* T_FUNION */
	DT_ENUM,
	DT_ENUM		/* T_FENUM */
};

static char dmap[] = {
	DD_PTR,
	DD_FUNC,
	DD_ARRAY,
	-1		/* D_MOSAR */
};

static char cmap[] = {
	-1,		/* C_NONE */
	DC_AUTO,
	DC_SIN,
	DC_SEX,
	DC_GDEF,
	DC_GREF,
	-1,		/* C_ARG */
	DC_GREF,	/* C_CXT */
	DC_LAB,
	-1,		/* C_FREF */
	-1,		/* C_KEY */
	DC_TYPE,
	DC_REG,
	DC_STAG,
	DC_UTAG,
	DC_ETAG,
	DC_MOS,
	DC_MOU,
	DC_MOE,
	DC_PAUTO,
	DC_PREG
};

dbpr(sp) register SYM *sp;
{
	register int n;
	int t;

	/* Identify class-segment */
	if ((n = cmap[sp->s_class]) < 0)
		dfatal(__LINE__);
	bput(n);

	/* Put line number */
	if (istackp < istack+NLEV)
		iput((ival_t) istack[NLEV-1].i_line);
	else
		iput((ival_t) sp->s_dline);

	/* Identify value */
	if (n < DC_AUTO) {
		;	/* No value */
	} else if (n < DC_MOS) {
		iput((ival_t) sp->s_value);
	} else {
		bput(sp->s_width);
		bput(sp->s_offset);
		iput((ival_t) sp->s_value);
	}

	/* Identify symbol */
	if (n < DC_TYPE)	/* Global */
		nput(sp->s_id);
	else if (n < DC_FILE)	/* Type name */
		sput(sp->s_id);
	else			/* Local */
		sput(sp->s_id);

	/* Get a real type name */
	if ((t = sp->s_type) == T_NONE) {
		switch (sp->s_class) {
		case C_STAG: t = T_STRUCT; break;
		case C_UTAG: t = T_UNION; break;
		case C_ETAG: t = T_ENUM; break;
		case C_MOE:
		case C_LAB: t = T_NONE; break;
		default: dfatal(__LINE__); break;
		}
	}

	/* Generate dimension string */
	if (notvariant(VTYPES)) {
		bput(DT_NONE);
		zput((sizeof_t)0);
	} else {
		register DIM *dp;
	
		for (dp = sp->s_dp; dp != NULL; dp = dp->d_dp) {
			if ((n = dmap[dp->d_type]) < 0)
				dfatal(__LINE__);
			bput(n);
			zput(tidsize(t, sp->s_ip, dp, 0));
		}

		/* Generate type string */
		if ((n = tmap[t]) < 0)
			dfatal(__LINE__);
		bput(n);
		zput(tidsize(t, sp->s_ip, dp, 0));

		/* Generate tag string */
		if (n==DT_STRUCT || n==DT_UNION || n==DT_ENUM)
			dbinfo(sp);
	}
}

static dbinfo(sp) SYM *sp;
{
	register INFO *ip;
	register SYM **ssp;
	register int i;

	if ((ip = sp->s_ip) == NULL)
		dfatal(__LINE__);
	if ( ! istag(sp->s_class) && (sp = findtype(sp)) != NULL) {
		sp->s_flag |= S_USED;
		bput(DX_NAME);
		zput((sizeof_t)0);
		sput(sp->s_id);
		return;
	}

	i = ip->i_nsp;
	ssp = ip->i_sp;
	bput(DX_MEMBS);
	zput((sizeof_t)i);
	while (--i >= 0) dbpr(*ssp++);
	return;
}

dfatal(line)
{
	cbotch("bad value in debug on line %d", line);
}
