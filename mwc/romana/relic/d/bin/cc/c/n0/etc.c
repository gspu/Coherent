/*
 * C compiler.
 * Small things that don't seem
 * to go anywhere else.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Allocate some space.
 * If no space, abort with a message.
 */
char    *
new(n)
{
	register char *p;

	if ((p=malloc(n)) == NULL)
		cnomem("new");
	return (p);
}

/*
 * Check if the symbol in the window 's' is token 't'.
 * If it is, skip over it; if it is not, print an error.
 */
mustbe(t)
{
	register char *p;

	if (s == t) {
		lex();
		return;
	}
	if (s == RPAREN) {
		cerror("missing '('");
		return;
	}
	switch (t) {

	case RBRACK:
		p = "]";
		break;

	case WHILE:
		p = "while";
		break;

	case LPAREN:
		p = "(";
		break;

	case RPAREN:
		p = ")";
		break;

	case SEMI:
		p = ";";
		break;

	case RBRACE:
		p = "}";
		break;

	case LBRACE:
		p = "{";
		break;

	case COLON:
		p = ":";
		break;

	case COMMA:
		p = ",";
		break;

	default:
		p = "{Something}";
		break;
	}
	cerror((p[1]=='\0') ? "missing '%s'" : "missing \"%s\"", p);
}

/*
 * Put out a local label.
 */
label(n)
{
	bput(LLABEL);
	iput((ival_t) n);
}

/*
 * Put out a local jump.
 */
jump(n)
{
	bput(JUMP);
	iput((ival_t) n);
}

/*
 * Put a label at the current
 * location.
 * Return the label number.
 */
here()
{
	register int	n;

	n = newlab();
	label(n);
	return (n);
}

/*
 * Complain about things not in the
 * bible (K and R).
 */
notbook()
{
	if (isvariant(VSBOOK))
		cstrict("construction not in Kernighan and Ritchie");
}
