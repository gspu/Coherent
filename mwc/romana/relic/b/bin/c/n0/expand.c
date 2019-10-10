/*
 * n0/expand.c
 * C preprocessor.
 * Macro expansion.
 */

#include <time.h>
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Expand the string as an actual parameter.
 * This happens after the list of strings
 * in the substituted macro has been pushed
 * onto dstack; expand the string as if it
 * formed the rest of the source file and
 * substitute the expanded string back into
 * the source stream, with the hidden sets of
 * identifiers augmented by the macro name we
 * are substituting.
 */
char *argexpand(hide, p) int hide; char *p;
{
	register char *ap, *spshp;
	register int c;

	/* Push the parameter as input */
	dspush(DS_IEOF, NULL);
	dspush(DS_STRNG, p);
	/* Save the push buffer */
	spshp = dpshp;
	/* Scan the parameter as input */
	while ((c = get()) >= 0) {
		switch (ct[c]) {
		case QUOTE:
		case STRING:
			dpshstr(c);
			continue;
		case DOT:
		case CON:
			dpshnum(c);
			continue;
		case ID:
			if (expand(c))
				continue;
			dpshs(id);
			/* Augment hidden set */
			dpshc(hideaug(hide, idhide));
			continue;
		default:
			dpshc(c);
			continue;
		}
	}
	++dstackp;			/* pop IEOF */
	/* Now pop the expanded string off the push buffer */
	/* and onto the put buffer */
	p = dputp;
	for (ap = spshp; --ap >= dpshp; )
		dputc(*ap);
	dputc(0);
	/* Restore push buffer pointer */
	dpshp = spshp;
	/* Return pointer to expanded argument */
	return p;
}

/*
 * Scan and augment the hidden sets in a string.
 * An in place copy in essence.
 */
char *argpaste(hide, p) int hide; char *p;
{
	register int c;
	char *sputp;
	sputp = dputp;
	dputp = p;
	dspush(DS_IEOF, NULL);
	dspush(DS_STRNG, p);
	while ((c = get()) >= 0) {
		switch (ct[c]) {
		case QUOTE:
		case STRING:
			dputstr(c);
			continue;
		case DOT:
		case CON:
			dputnum(c);
			continue;
		case ID:
			getid(c);
			dputs(id);
			/* Augment hidden set */
			dputc(hideaug(hide, idhide));
			continue;
		default:
			dputc(c);
			continue;
		}
	}
	++dstackp;
	dputp = sputp;
	return p;
}


/*
 * Substitute for the macro specified.
 * Collect the actual parameters into the push buffer,
 * then reverse them onto the put buffer.
 * (Collecting the parameters may pop the dstackp.)
 * Then parse the macro body into the put buffer,
 * forming a list of literal strings
 * (with their hidden sets augmented),
 * # parameters, ## parameters, and
 * vanilla parameters.
 * Then scan the list pushed and
 * recursively expand the vanilla
 * parameters with argexpand(),
 * expanding each parameter only once
 * no matter how many times it appears
 * in the substituted body.
 * Then rescan the list pushed again
 * to augment the hidden sets of ## params.
 */
substitute(tp, sp) TOK *tp; CPPSYM *sp;
{
	register int c, d;
	register int narg, plev;
	register char *p, *pp;
	register DSTACK *dsp;
	DSTACK *dparamp, *dbodyp, *dbasep;
	char *cparamp;
	int tpidhide;

	/* Install hide set index */
	tpidhide = hideset(tp);

	/* Collect parameters */
	dparamp = dlistp;
	cparamp = dpshp;
	if (sp->s_value == XUSERA) {
		c = getskip();
		for (narg = 0; c >= 0 && c != ')'; narg += 1) {
			plev = 0; p = dpshp;
			while (c >= 0) {
				switch (ct[c]) {
				case QUOTE:
				case STRING:
					dpshstr(c); c = get(); continue;
				case LPAREN:
					plev++; break;
				case COMMA:
					if (!plev) goto next; break;
				case RPAREN:
					if (!plev) goto next; plev--; break;
				case SKIP:
					unget(getskip()); c = ' '; break;
				case ID:
					getid(c); dpshs(id); dpshc(idhide);
					c = get(); continue;
				case DOT:
				case CON:
					dpshnum(c); c = get(); continue;
				}
				dpshc(c); c = get();
			}
		next:	if (narg < sp->s_narg) {
				if (dpshp < p && dpshp[0] == ' ')
					++dpshp;
				dpshc(0);
				dslist(DS_PARAM, p);
			}
			if (c == ',')
				c = getskip();
		}
		if (c < 0) {
			cerror("EOF in macro \"%s\" invocation", tp->t_id);
			tp = NULL;
		}
		if (sp->s_narg != narg) {
			cerror("\"%s\" argument mismatch", tp->t_id);
			tp = NULL;
		}
		if (tp == NULL) {
			dlistp = dparamp;
			dpshp = cparamp;
			return 1;
		}
	}
	/* Copy the parameter strings onto the top of dbuf */
	dpshp = cparamp;
	cparamp = dputp;
	for (dsp = dparamp+sp->s_narg; --dsp >= dparamp; ) {
		p = dsp->ds_ptr;
		dsp->ds_ptr = dputp;
		while (d = *--p) dputc(d);
		dputc(d);
	}
	/* Copy the macro body into dbuf */
	dbodyp = dlistp;
	dspush(DS_IEOF, 0);
	dspush(DS_STRNG, sp->s_body);
	p = dputp;
	dsp = dparamp;
	while ((c = get()) >= 0) {
		switch (ct[c]) {
		case QUOTE:
		case STRING:
			dputstr(c); continue;
		case ID:
			getid(c);
			dputs(id);
			dputc(hideaug(tpidhide, idhide));
			continue;
		case CON:
		case DOT:
			dputnum(c); continue;
		default:
			dputc(c); continue;
		case HIGH0:		/* param */
			c -= ARG0;
			pp = dsp[c].ds_ptr;
			if (p != dputp) {
				dputc(0);
				dslist(DS_STRNG, p);
				p = dputp;
			}
			c = get();
			d = get();
			unget(d);
			unget(c);
			if (c == '#' && d == '#')
				dslist(DS_SHARP2, pp);
			else
				dslist(DS_PARAM, pp);
			continue;
		case SHARP:		/* # or ## */
			c = get();
			if (ct[c] == HIGH0) {
				c -= ARG0;
				pp = dsp[c].ds_ptr;
				if (p != dputp) {
					dputc(0);
					dslist(DS_STRNG, p);
					p = dputp;
				}
				dslistc(DS_UNGET, '"');
				dslist(DS_SHARP, pp);
				dslistc(DS_UNGET, '"');
				continue;
			}
			c = get();
			if (ct[c] == HIGH0) {
				c -= ARG0;
				pp = dsp[c].ds_ptr;
				if (p != dputp) {
					dputc(0);
					dslist(DS_STRNG, p);
					p = dputp;
				}
				dslist(DS_SHARP2, pp);
				continue;
			}
			unget(c);
			continue;
		}
	}
	if (p != dputp) {
		dputc(0);
		dslist(DS_STRNG, p);
		p = dputp;
	}
	++dstackp;	/* Pop IEOF */
	dbasep = dstackp;
	dspush(DS_NAME, tp);
	dspush(DS_DPUTP, cparamp);
	while (--dlistp >= dbodyp)
		*--dstackp = *dlistp;
	dlistp = dparamp;
	/* Expand parameters and augment their hidden sets */
	for (dsp = dstackp; dsp < dbasep; dsp += 1) {
		if (dsp->ds_type == DS_PARAM) {
			p = argexpand(tpidhide, pp = dsp->ds_ptr);
			/* Now make all references to pp */
			/* refer to the expanded version p */
			for (dparamp = dsp; dsp < dbasep; dsp += 1)
				if (dsp->ds_type == DS_PARAM
				 && dsp->ds_ptr == pp) {
					dsp->ds_type = DS_STRNG;
					dsp->ds_ptr = p;
				}
			dsp = dparamp;
		}
	}
	/* Now scan for glued parameters and augment their hidden sets */
	/* Done in a separate loop since ##param might appear before */
	/* param itself in the substitution list, and they both refer */
	/* to the same copy of the parameter in dbuf before argexpand */
	for (dsp = dstackp; dsp < dbasep; dsp += 1) {
		if (dsp->ds_type == DS_SHARP2) {
			p = argpaste(tpidhide, pp = dsp->ds_ptr);
			for (dparamp = dsp; dsp < dbasep; dsp += 1)
				if (dsp->ds_type == DS_SHARP2
				 && dsp->ds_ptr == pp) {
					dsp->ds_type = DS_STRNG;
					dsp->ds_ptr = p;
				}
			dsp = dparamp;
		}
	}
	return 1;
}

/*
 * Try macro expansion,
 * return true if it happened.
 * Read id[] starting with 'c'
 * and see if it is defined.
 * Since this is called for each
 * identifier read, it should use
 * minimal registers and defer most
 * processing to subfunctions.
 */
expand(c) register int c;
{
	char *p;
	CPPSYM *sp;
	TOK *tp;
	extern char *ctime();
	static char lineno[16];

	getid(c);
	tp = idp;
	if ((sp=idp->t_sym) == NULL
	 || sp->s_slevel != SL_CPP
	 || hidden()) {
 none_such:	if (incpp > 1) {	/* Parsing a #if expression */
			dspush(DS_STRNG, deffalse);
			return 1;
		}
		return 0;
	}

	switch ((int)sp->s_value) {
	case XUSER:
		return substitute(tp, sp);
	case XUFILE:
		unget('"');
		dspush(DS_UFILE, file);
		unget('"');
		return 1;
	case XUBASE:
		unget('"');
		dspush(DS_UFILE, basefile);
		unget('"');
		return 1;
	case XULINE:
		dspush(DS_STRNG, lineno);
		sprintf(lineno, "%d", line);
		return 1;
	case XUDATE:
	case XUTIME:
		p = ctime(&curtime);
		if (sp->s_value == XUDATE) {
			p[24] = 0;
			strcpy(p+11, p+20);
			p += 4;
		} else {
			p[19] = 0;
			p += 11;
		}
		unget('"');
		dspush(DS_STRNG, p);
		unget('"');
		return 1;
#if	0
/*
 * ANSI C says __STDC__ should be defined as 0 for a non-ANSI compiler,
 * but too many sources use #ifdef __STDC__ when they should use #if __STDC__.
 * This is therefore conditionalized out here and in n0/cc0key.c.
 */
	case XUSTDC:
		dspush(DS_STRNG, deffalse);
		return 1;
#endif
	case XDEFINED:
		if (incpp < 2)
			return 0;
		if ((c = getnb()) == '(') {
			if ((c = getnb()) < 0 || ct[c] != ID)
				goto bad;
			getid(c);
			if ((c = getnb()) != ')')
				goto bad;
		} else if (ct[c] == ID)
			getid(c);
		else {
	bad:		cerror("illegal use of defined");
			dspush(DS_STRNG, deffalse);
			return 1;
		}
		tp = idp;
		if ((sp = tp->t_sym) != NULL
		 && sp->s_slevel == SL_CPP
		 && ! hidden())
			dspush(DS_STRNG, deftrue);
		else
			dspush(DS_STRNG, deffalse);
		return 1;
	case XUSERA:
		if (skipto('('))
			return substitute(tp, sp);
		goto none_such;
	default:
		cbotch("value %d in expand", sp->s_value);
	}
	return 0;
}

/* end of n0/expand.c */
