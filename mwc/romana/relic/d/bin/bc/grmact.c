#include <stdio.h>
#include <assert.h>
#include "bc.h"


/*
 *	Gerror is used by the various grammar actions when a
 *	semantic error is discoverd.  It simply prints out a
 *	message and sets allok to FALSE.
 *	It is also called by yyerror to report syntax errors.
 *	It is also called by lexerror to report lexical errors.
 *	Allok, errors, and any partially defined function or statement
 *	will be cleared at the end of the line, except for the
 *	too much code error which clears at the end of the function
 *	or statement which has grown too large.
 */

gerror(str)
char	*str;
{
	if (allok == TRUE) {
		if (infnam != NULL)
			fprintf(stderr, "%s: ", infnam);
		if (inline != 0)
			fprintf(stderr, "%d: ", inline);
		fprintf(stderr, "%r\n", &str);
		allok = FALSE;
	}
}

/*
 * Chktype verifies that the type of `var' will work in sload, aeload,
 * or arload.  Otherwise it emits an error and returns zero so the
 * grammar action can abort the parse.
 */
chktype(var, type)
register dicent *var;
register int type;
{
	if (var->localt == type)
		return 1;
	else if (var->localt == UNDEFINED)
		if (var->globalt == type || var->globalt == UNDEFINED)
			return 1;
		else
			gerror("'%s' is wrong global type", var->word);
	else
		gerror("'%s' is wrong local type", var->word);
	return 0;
}

/*
 *	Sload emits the code to load the scalar with dictionary entry
 *	var.
 */

sload(var)
register dicent	*var;
{
	switch (var->localt) {
	case SCALAR:
		emitop(PLOSC);
		emitcnt(var->localv);
		break;
	case UNDEFINED:
		switch (var->globalt) {
		case UNDEFINED:
			var->globalt = SCALAR;
			newscalar(&var->globalv.rvalue);
			/* fall thru */
		case SCALAR:
			emitop(PGLSC);
			emitnum(&var->globalv.rvalue);
			break;
		}
	}
}


/*
 *	Aeload emits the code to load the array element with dictionary
 *	entry var.
 */

aeload(var)
register dicent	*var;
{
	switch (var->localt) {
	case ARRAY:
		emitop(PLOAE);
		emitcnt(var->localv);
		break;
	case UNDEFINED:
		switch (var->globalt) {
		case UNDEFINED:
			var->globalt = ARRAY;
			newarray(&var->globalv.arvalue);
			/* fall thru */
		case ARRAY:
			emitop(PGLAE);
			emitarry(&var->globalv.arvalue);
			break;
		}
	}
}


/*
 *	Arload emits the code to load the whole array with dictionary
 *	entry var.
 */

arload(var)
register dicent	*var;
{
	switch (var->localt) {
	case ARRAY:
		emitop(PLOAR);
		emitcnt(var->localv);
		break;
	case UNDEFINED:
		switch (var->globalt) {
		case UNDEFINED:
			var->globalt = ARRAY;
			newarray(&var->globalv.arvalue);
			/* fall thru */
		case ARRAY:
			emitop(PGLAR);
			emitarry(&var->globalv.arvalue);
			break;
		}
	}
}


/*
 *	If there is room for another code item, incloc advances loc
 *	and returns the old loc.  If not then incloc calls gerror.
 *	Note that both the value returned by incloc and loc itself
 *	always point to a location in cstream.
 */

code	*
incloc()
{
	register code	*res;

	res = loc++;
	if (res >= &cstream[MAXCODE - 1]) {
		loc = res;
		gerror("Too much code");
	}
	return (res);
}


/*
 *	Negate returns the opcode which branches on the opposite
 *	condition of when `op' branches.
 */

opcode
negate(op)
opcode op;
{
	switch (op) {
	case BRALW:
		return (BRNEV);
	case BRNEV:
		return (BRALW);
	case BRLT:
		return (BRGE);
	case BRLE:
		return (BRGT);
	case BREQ:
		return (BRNE);
	case BRGE:
		return (BRLT);
	case BRGT:
		return (BRLE);
	case BRNE:
		return (BREQ);
	}
	assert(FALSE);
}


/*
 *	Locaddr sets the local addresses (ie frame pointer offsets) for
 *	all local variables and parameters.  `vec' is an array of `len'
 *	pointers to dictionary entries and `base' is the frame offset
 *	of the first entry.
 *	Note that locaddr assumes that it is never called with len zero.
 */

locaddr(vec, len, base)
register dicent	*vec[];
register int	len,
		base;
{
	do {
		(*vec++)->localv = base++;
	} while (--len > 0);
}


/*
 *	Chkfunc checks to make sure that the identifier with dictionary
 *	entry pointed to by `dicp' can be used as a function.  If it
 *	is not already a function, then the body and types fields of the
 *	func value are set to NULL.
 */

chkfunc(dicp)
register dicent	*dicp;
{
	if (dicp->globalt == FUNCTION)
		return 1;
	if (dicp->globalt == UNDEFINED) {
		dicp->globalt = FUNCTION;
		dicp->globalv.fvalue.fcsize = 0;
		dicp->globalv.fvalue.body = dicp->globalv.fvalue.types = NULL;
		return 1;
	}
	gerror("'%s' is not a function", dicp->word);
	return 0;
}

/*
 *	Install installs the definition of the function `fnc'.
 *	`pvec' and `lpvec' (`avec' and `lavec') are the vector
 *	of formal parameters (respectively automatic variables)
 *	and its length.
 */

install(fnc, pvec, lpvec, avec, lavec)
register func	*fnc;
dicent	**pvec,
	**avec;
int	lpvec,
	lavec;
{
	int	csize;

	litfree(fnc->body, fnc->body+fnc->fcsize);
	mpfree(fnc->body);
	mpfree(fnc->types);
	fnc->nparams = lpvec;
	fnc->nautos = lavec;
	if (lpvec + lavec == 0)
		fnc->types = NULL;
	else {
		fnc->types = (type *)mpalc((lpvec + lavec) * sizeof (type));
		copylty(pvec, fnc->types, lpvec);
		copylty(avec, &fnc->types[lpvec], lavec);
	}
	fnc->fcsize = loc - cstream;
	csize = fnc->fcsize * sizeof (code);
	fnc->body = (code *)mpalc(csize);
	copy((char *)cstream, (char *)fnc->body, csize);
	remloc(pvec, lpvec);
	remloc(avec, lavec);
}

/*
 *	Copylty copyies the local types from the dictionary vector
 *	`dvec' to the type vector `tvec'.  `len' is the number of
 *	entries to copy.
 */

copylty(dvec, tvec, len)
register dicent	*dvec[];
register type	tvec[];
register int	len;
{
	while (--len >= 0)
		*tvec++ = (*dvec++)->localt;
}


/*
 *	Copy copyies the block of bytes at `from' to that at `to'.
 *	`len' is the number of bytes to copy.  Note that these
 *	blocks should not overlap.
 */

copy(from, to, len)
register char	*from,
		*to;
register int	len;
{
	while (--len >= 0)
		*to++ = *from++;
}

/*
 *	Remloc is used to remove the local types from the dictionary
 *	vector `dvec'.  `len' is the length of the vector.
 */

remloc(dvec, len)
register dicent	**dvec;
register int	len;
{
	while (--len >= 0)
		(*dvec++)->localt = UNDEFINED;
}

/*
 * Litfree recovers literal and string storage from bp to ep.
 * Called after interp() on cstream to loc,
 * after function definition abort on cstream to loc,
 * during function redefinition on the old body.
 */
litfree(pc, end) register code *pc, *end;
{
	register int op;
	while (pc < end) {
		op = pc++->opcode;
		if (pc >= end)
			break;
		switch (op) {
		case BRNEV:	/* Why by two? */
			printf("BRNEV in litfree\n");
		case CALL:
			++pc;
		case PGLSC:
		case PLOSC:
		case PGLAE:
		case PLOAE:
		case PGLAR:
		case PLOAR:
		case RETURN:
		case BRALW:
		case BRLT:
		case BRLE:
		case BREQ:
		case BRGE:
		case BRGT:
		case BRNE:
			++pc;
		case LOAD:
		case LIBASE:
		case LOBASE:
		case LSCALE:
		case STORE:
		case SIBASE:
		case SOBASE:
		case SSCALE:
		case POP:
		case PRVAL:
		case STOP:
		case INC:
		case DEC:
		case PRNUM:
		case PRNL:
		case LENGTH:
		case SCALE:
		case SQRT:
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case REM:
		case EXP:
		case NEG:
		case EXIT:
			break;
		case PLISC:
			mvfree(&pc->lvalue->mantissa);
			mpfree(pc++->lvalue);
			break;
		case PRSTR:
			mpfree(pc++->svalue);
			break;
		default:
			fprintf(stderr, "Mixup in litfree\n");
			exit(1);
		}
	}
}
