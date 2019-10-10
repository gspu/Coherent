/*
 * This file contains all of the lexical analysis routines
 * for the portable C compiler.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Character classes.
 * Indexed by ASCII character code.
 */
unsigned char ct[] = {
	JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,
	JUNK,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   JUNK,   JUNK,
	JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,
	JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,   JUNK,
	SKIP,   NOT,    STRING, SHARP,  JUNK,   REM,    AND,    QUOTE,
	LPAREN, RPAREN, MUL,    ADD,    COMMA,  SUB,    DOT,    DIV,
	CON,    CON,    CON,    CON,    CON,    CON,    CON,    CON,
	CON,    CON,    COLON,  SEMI,   LT,     ASSIGN, GT,     QUEST,
	JUNK,   ID,     ID,     ID,     ID,     ID,     ID,     ID,
	ID,     ID,     ID,     ID,     ID,     ID,     ID,     ID,
	ID,     ID,     ID,     ID,     ID,     ID,     ID,     ID,
	ID,     ID,     ID,     LBRACK, BACKDIV,RBRACK, XOR,    ID,
	JUNK,   ID,     ID,     ID,     ID,     ID,     ID,     ID,
	ID,     ID,     ID,     ID,     ID,     ID,     ID,     ID,
	ID,     ID,     ID,     ID,     ID,     ID,     ID,     ID,
	ID,     ID,     ID,     LBRACE, OR,     RBRACE, COM,    JUNK,
	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,
	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,
	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,
	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,	HIGH0,
	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,
	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,
	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,
	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,	HIGH1,
	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,
	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,
	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,
	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,	HIGH2,
	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,
	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,
	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,
	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3,	HIGH3
};

/*
 * Read and decode the next token of the input.
 * The token is returned and stored in 's'.
 * Identifiers are packed into 'id'
 * and their hash table slot is stashed in 'idhash'.
 * Integers are returned in 'ival',
 * long integers in 'lval' and doubles in 'dval'.
 */
lex()
{
	register SYM	*sp;
	register int	c;
	register int	n;

	for (;;) {
		if ((c=get()) < 0)
			return (s = EOF);
		switch (s = ct[c]) {

		case SKIP:
			continue;

		case QUOTE:
			n = 0;
			lval = 0;
			instring = c;		/* to allow high-bit chars */
			while ((c = getmap('\'')) >= 0) {
				lval <<= 8;
				lval |= c;
				++n;
			}
			instring = 0;
			ival = lval;
			tval = T_INT;
			s = ICON;
#if 0
			switch (n) {
			case 0:
				cwarn("empty character constant");
				break;
			case 2:
				cwarn("multiple character constant");
				tval = T_INT;
				break;
			case 1:
				break;
			case 3:
			case 4:
				cwarn("character constant promoted to long");
				tval = T_LONG;
				s = LCON;
				break;
			default:
				cerror("character constant overflows long");
				tval = T_LONG;
				s = LCON;
				break;
			}
#else
			if (n != 1)
				cerror("multiple character constant");
#endif
			break;

		case ADD:
			if (nextis('+'))
				s = INCBEF;
			break;

		case SUB:
			if (nextis('-'))
				s = DECBEF;
			else if (nextis('>'))
				s = ARROW;
			break;

		case LT:
			if (nextis('='))
				s = LE;
			else if (nextis('<'))
				s = SHL;
			break;

		case GT:
			if (nextis('='))
				s = GE;
			else if (nextis('>'))
				s = SHR;
			break;

		case ASSIGN:
			if (nextis('='))
				s = EQ;
			break;

		case NOT:
			if (nextis('='))
				s = NE;
			break;

		case AND:
			if (nextis('&'))
				s = ANDAND;
			break;

		case OR:
			if (nextis('|'))
				s = OROR;
			break;

		case DOT:
			c = get();
			if (c<0 || ct[c]!=CON) {
				unget(c);
				break;
			}
			getnum(c, 1);
			break;

		case CON:
			getnum(c, 0);
			break;

		case ID:
			if (expand(c))
				continue;
			for (sp = idp->t_sym; sp != NULL; sp = sp->s_sp)
				if (sp->s_slevel == SL_KEY) {
					s = sp->s_value;
					break;
				}
			break;

		case JUNK:
		case HIGH0:
		case HIGH1:
		case HIGH2:
		case HIGH3:
			cerror("illegal character (%d decimal)", c);
			continue;

		}
		if ((s>=ADD && s<=SHR) && nextis('='))
			s += AADD-ADD;
		return (s);
	}
}

/*
 * Test if the next character in the input is 'n'.
 * If it is, eat it up and return true;
 * otherwise put it back and return false.
 */
nextis(n) int n;
{
	register int	c;

	if ((c = get()) == n)
		return (1);
	unget(c);
	return (0);
}

/*
 * Get the next non blank character from the input.
 * This is for cpp, so we do not pass newline.
 */
getnb()
{
	register int c;

	while ((c = get())==' ' || c=='\t')
		;
	return (c);
}

/*
 * Get the next non-skip character.
 * Different from getnb() which chokes
 * on newline.
 */
getskip()
{
	register int c;

	while ((c = get()) > 0 && ct[c] == SKIP)
		;
	return c;
}

/*
 * Peek ahead in the input (ignoring whitespace)
 * to see if the next character is 'n'.
 * True return it it is.
 * The character is left in the input stream.
 * This routine is used to look for the ':' after labels
 * and the '(' after undefined names in expressions.
 */
spnextis(n) int n;
{
	register int	c;

	while ((c=get()) >= 0) {
		if (ct[c] != SKIP) {
			unget(c);
			if (c == n)
				return (1);
			return (0);
		}
	}
	return (0);
}

/*
 * This routine is like lex(), except that it correctly skips
 * over any strings in the input.
 * Used during error recovery.
 */
skip()
{
	if (s == STRING)
		while (getmap('\"') >= 0)
			;
	lex();
}

/*
 * Read the next character from a string or a character constant.
 * Return -1 when either end of file or the delimiter 'd' is encountered.
 */
getmap(d) int d;
{
	register int	c, n, v;

	if ((c=get()) == d)
		return (-1);
	if (c<0 || c=='\n') {
		unget(c);
		cerror("nonterminated string or character constant");
		return (-1);
	}
	if (c == '\\') {
		c = get();
		switch (c) {

		case 'a':
			c = '\07';
			break;

		case 'b':
			c = '\b';
			break;

		case 'f':
			c = '\f';
			break;

		case 'n':
			c = '\n';
			break;

		case 'r':
			c = '\r';
			break;

		case 't':
			c = '\t';
			break;

		case 'v':
			c = '\013';
			break;

		case 'x':
			notbook();
			c = get();
			n = 0;
			v = 0;
			while (++n<=3
				&& ( (c>='0' && c<='9')
				  || (c>='A' && c<='F')
				  || (c>='a' && c<='f') )) {
				v <<= 4;
				if (c>='0' && c<='9')
					v += c - '0';
				else if (c>='A' && c<='F')
					v += c - 'A' + 10;
				else
					v += c - 'a' + 10;
				c = get();
			}
			unget(c);
			c = v;
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			n = 0;
			v = 0;
			while (++n<=3 && c>='0' && c<='7') {
				v = (v<<3) + c - '0';
				c = get();
			}
			unget(c);
			c = v;
			break;
		}
	}
	return (c);
}

/*
 * Read an identifier into id[]
 * and set idhash.
 */
getid(ch) int ch;
{
	{
		register char *p;
		register int c;
		c = ch;
		p = id;
		idhash = 0;
		idsize = 0;
		idhide = -1;	/* Universal */
		do {
			if (p < &id[NCSYMB-1]) {
				*p++ = c;
				idhash += c;
				idsize += 1;
			}
			if ((c = get()) >= SET0) {
				idhide = hideint(c, idhide);
				c = get();
			}
		} while (c>=0 && (ct[c]==ID || ct[c]==CON));
		unget(c);
		if (idhide == -1)
			idhide = SET0;	/* Empty */
		*p++ = 0;
		*p++ = 0;
		*p++ = 0;
	}
	{
		register TOK *tp, **tpp;

		idhash %= NHASH;
		for (tpp = hash0+idhash; (tp = *tpp) != NULL; tpp = &tp->t_tp)
			if (ideq(tp))
				break;
		if (tp == NULL)
			*tpp = tp = newtoken();
		idp = tp;
	}
}

setid(cp) char *cp;
{
	{
		register char *p1, *p2;
		register int c;

		p1 = cp;
		p2 = id;
		idhash = 0;
		idsize = 0;
		while (c = *p1++) {
			if (p2 < &id[NCSYMB-1]) {
				*p2++ = c;
				idhash += c;
				idsize += 1;
			}
		}
		*p2 = 0;
	}
	{
		register TOK *tp, **tpp;

		idhash %= NHASH;
		for (tpp = hash0+idhash; (tp = *tpp) != NULL; tpp = &tp->t_tp)
			if (ideq(tp))
				break;
		if (tp == NULL)
			*tpp = tp = newtoken();
		idp = tp;
	}
}

/*
 * Read number into id[].
 * Set 's' appropriately.
 */
getnum(c, dotf) register int c; int dotf;
{
	register char	*np;
	int		expf, sgnf, ntlf, base;
	int		islong, isunsigned, isfloat;
	long		n;
	static char ulong[] = "unsigned long";
#	define slong	(ulong+9)
#if	NATIVEFP
	double		d;
	char		*cp, *dp;
	extern double	atof();
#endif

	expf = 0;
	ntlf = 0;
	sgnf = 1;
	base = 10;
	islong = 0;
	isunsigned = 0;
	isfloat = 0;
	np = id;
	n = 0;
	if (dotf)
		*np++ = '.';
	if (dotf==0 && c=='0') {
		base = 8;
		if ((c = get()) == 'x' || c == 'X') {
			*np++ = '0';
			*np++ = c;
			base = 16;
			c = get();
		} else {
			unget(c);
			c = '0';
		}
	}
	do {
		if (base!=16 && c=='.') {
			if (dotf++)
				break;
		} else if (c=='+' || c=='-') {
			if (sgnf++)
				break;
		} else if (base!=16 && (c=='e' || c=='E')) {
			if (expf++)
				break;
			sgnf = 0;
		} else if (c<'0' || c>'9') {
			if (base != 16)
				break;
			if (!((c>='A' && c<='F') || (c>='a' && c<='f')))
				break;
		} else
			++sgnf;
		if (np < &id[NCSYMB-1])
			*np++ = c;
		else
			ntlf = 1;
		if (base == 8)
			n <<= 3;
		else if (base == 10)
			n = (n<<3) + (n<<1);
		else
			n <<= 4;
		if (c>='A' && c<='F')
			c -= 'A'-10;
		else if (c>='a' && c<='f')
			c -= 'a'-10;
		else
			c -= '0';
		n += c;
	} while ((c = get()) >= 0);
	for (;;) {
		if (c=='l' || c=='L') {
			++islong;
			*np++ = c;
			c = get();
		} else if (c == 'u' || c == 'U') {
			++isunsigned;
			*np++ = c;
			c = get();
		} else if (c == 'f' || c == 'F') {
			++isfloat;
			*np++ = c;
			c = get();
		} else {
			unget(c);
			*np = 0;
			break;
		}
	}
	s = ICON;
	tval = T_INT;
	ival = lval = n;
	if (ntlf) {
		cerror("number has too many digits");
		return;
	}
	if (dotf || expf || isfloat) {
		if (isunsigned || (isfloat+islong > 1)) {
			cerror("illegal floating constant suffix");
			return;
		}
		tval = isfloat ? T_FLOAT : islong ? T_LDOUBLE : T_DOUBLE;
#if	NATIVEFP
		d = atof(id);
		for (dp = (char *)dval, cp = ((char *)&d)+sizeof(double)-1; cp >= &d; )
			*dp++ = *cp--;
#else
		dvalread((char *)dval, id);
#endif
		s = DCON;
		return;
	}
	if (isunsigned > 1 || islong > 1) {
		cerror("illegal integer constant suffix");
		return;
	}
	np = NULL;
	if (islong) {
		s = LCON;
		tval = T_LONG;
		if (isunsigned || (n&SLMASK) != 0)
			tval = T_ULONG;
	} else if (isunsigned) {
		tval = T_UINT;
		if ((n&UIMASK) != 0) {
			s = LCON;
			tval = T_ULONG;
			np = ulong;
		}
	} else if (base == 10 && (n&SIMASK) != 0) {
		s = LCON;
		tval = T_LONG;
		np = slong;
		if ((n&SLMASK) != 0) {
			tval = T_ULONG;
			np = ulong;
		}
	} else if ((n&SIMASK) != 0) {
		tval = T_UINT;
		if ((n&UIMASK) != 0) {
			s = LCON;
			tval = T_LONG;
			np = slong;
			if ((n&SLMASK) != 0) {
				tval = T_ULONG;
				np = ulong;
			}
		}
	}
	if (tval == T_ULONG) {
		/* Test for unsigned long overflow */
		/* and set np = NULL if a diagnostic is given */
		;
	}
	if (incpp == 0 && np != NULL && isvariant(VSLCON))
		cwarn("\"%s\" promoted to %s", id, np);
}
