# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 6 (of 6)."
# Contents:  regexp.c Makefile
# Wrapped by mark@jhereg on Tue Dec 27 19:38:04 1988
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f regexp.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"regexp.c\"
else
echo shar: Extracting \"regexp.c\" \(30611 characters\)
sed "s/^X//" >regexp.c <<'END_OF_regexp.c'
X/* $Source: /u/mark/src/pax/RCS/regexp.c,v $
X *
X * $Revision: 1.1 $
X *
X * regexp.c - regular expression matching
X *
X * DESCRIPTION
X *
X *	Underneath the reformatting and comment blocks which were added to 
X *	make it consistent with the rest of the code, you will find a
X *	modified version of Henry Specer's regular expression library.
X *	Henry's functions were modified to provide the minimal regular
X *	expression matching, as required by P1003.  Henry's code was
X *	copyrighted, and copy of the copyright message and restrictions
X *	are provided, verbatim, below:
X *
X *	Copyright (c) 1986 by University of Toronto.
X *	Written by Henry Spencer.  Not derived from licensed software.
X *
X *	Permission is granted to anyone to use this software for any
X *	purpose on any computer system, and to redistribute it freely,
X *	subject to the following restrictions:
X *
X *	1. The author is not responsible for the consequences of use of
X *         this software, no matter how awful, even if they arise
X *	   from defects in it.
X *
X *	2. The origin of this software must not be misrepresented, either
X *	   by explicit claim or by omission.
X *
X *	3. Altered versions must be plainly marked as such, and must not
X *	   be misrepresented as being the original software.
X *
X * 	Beware that some of this code is subtly aware of the way operator
X * 	precedence is structured in regular expressions.  Serious changes in
X * 	regular-expression syntax might require a total rethink.
X *
X * AUTHORS
X *
X *     Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
X *     Henry Spencer, University of Torronto (henry@utzoo.edu)
X *
X * Sponsored by The USENIX Association for public distribution. 
X *
X * $Log:	regexp.c,v $
X * Revision 1.1  88/12/23  18:02:32  mark
X * Initial revision
X * 
X */
X
X/* Headers */
X
X#include "pax.h"
X
X#ifndef lint
Xstatic char    *Ident = "$Id: regexp.c,v 1.1 88/12/23 18:02:32 mark Rel $";
X#endif
X
X
X/*
X * The "internal use only" fields in regexp.h are present to pass info from
X * compile to execute that permits the execute phase to run lots faster on
X * simple cases.  They are:
X *
X * regstart	char that must begin a match; '\0' if none obvious
X * reganch	is the match anchored (at beginning-of-line only)?
X * regmust	string (pointer into program) that match must include, or NULL
X * regmlen	length of regmust string
X *
X * Regstart and reganch permit very fast decisions on suitable starting points
X * for a match, cutting down the work a lot.  Regmust permits fast rejection
X * of lines that cannot possibly match.  The regmust tests are costly enough
X * that regcomp() supplies a regmust only if the r.e. contains something
X * potentially expensive (at present, the only such thing detected is * or +
X * at the start of the r.e., which can involve a lot of backup).  Regmlen is
X * supplied because the test in regexec() needs it and regcomp() is computing
X * it anyway.
X */
X
X/*
X * Structure for regexp "program".  This is essentially a linear encoding
X * of a nondeterministic finite-state machine (aka syntax charts or
X * "railroad normal form" in parsing technology).  Each node is an opcode
X * plus a "nxt" pointer, possibly plus an operand.  "Nxt" pointers of
X * all nodes except BRANCH implement concatenation; a "nxt" pointer with
X * a BRANCH on both ends of it is connecting two alternatives.  (Here we
X * have one of the subtle syntax dependencies:  an individual BRANCH (as
X * opposed to a collection of them) is never concatenated with anything
X * because of operator precedence.)  The operand of some types of node is
X * a literal string; for others, it is a node leading into a sub-FSM.  In
X * particular, the operand of a BRANCH node is the first node of the branch.
X * (NB this is *not* a tree structure:  the tail of the branch connects
X * to the thing following the set of BRANCHes.)  The opcodes are:
X */
X
X/* definition	number	opnd?	meaning */
X#define	END	0		/* no	End of program. */
X#define	BOL	1		/* no	Match "" at beginning of line. */
X#define	EOL	2		/* no	Match "" at end of line. */
X#define	ANY	3		/* no	Match any one character. */
X#define	ANYOF	4		/* str	Match any character in this string. */
X#define	ANYBUT	5		/* str	Match any character not in this
X				 * string. */
X#define	BRANCH	6		/* node	Match this alternative, or the
X				 * nxt... */
X#define	BACK	7		/* no	Match "", "nxt" ptr points backward. */
X#define	EXACTLY	8		/* str	Match this string. */
X#define	NOTHING	9		/* no	Match empty string. */
X#define	STAR	10		/* node	Match this (simple) thing 0 or more
X				 * times. */
X#define	OPEN	20		/* no	Mark this point in input as start of
X				 * #n. */
X /* OPEN+1 is number 1, etc. */
X#define	CLOSE	30		/* no	Analogous to OPEN. */
X
X/*
X * Opcode notes:
X *
X * BRANCH	The set of branches constituting a single choice are hooked
X *		together with their "nxt" pointers, since precedence prevents
X *		anything being concatenated to any individual branch.  The
X *		"nxt" pointer of the last BRANCH in a choice points to the
X *		thing following the whole choice.  This is also where the
X *		final "nxt" pointer of each individual branch points; each
X *		branch starts with the operand node of a BRANCH node.
X *
X * BACK		Normal "nxt" pointers all implicitly point forward; BACK
X *		exists to make loop structures possible.
X *
X * STAR		complex '*', are implemented as circular BRANCH structures 
X *		using BACK.  Simple cases (one character per match) are 
X *		implemented with STAR for speed and to minimize recursive 
X *		plunges.
X *
X * OPEN,CLOSE	...are numbered at compile time.
X */
X
X/*
X * A node is one char of opcode followed by two chars of "nxt" pointer.
X * "Nxt" pointers are stored as two 8-bit pieces, high order first.  The
X * value is a positive offset from the opcode of the node containing it.
X * An operand, if any, simply follows the node.  (Note that much of the
X * code generation knows about this implicit relationship.)
X *
X * Using two bytes for the "nxt" pointer is vast overkill for most things,
X * but allows patterns to get big without disasters.
X */
X#define	OP(p)	(*(p))
X#define	NEXT(p)	(((*((p)+1)&0377) + (*((p)+2)&0377))<<8)
X#define	OPERAND(p)	((p) + 3)
X
X/*
X * Utility definitions.
X */
X
X#define	FAIL(m)	{ regerror(m); return(NULL); }
X#define	ISMULT(c)	((c) == '*')
X#define	META	"^$.[()|*\\"
X#ifndef CHARBITS
X#define	UCHARAT(p)	((int)*(unsigned char *)(p))
X#else
X#define	UCHARAT(p)	((int)*(p)&CHARBITS)
X#endif
X
X/*
X * Flags to be passed up and down.
X */
X#define	HASWIDTH	01	/* Known never to match null string. */
X#define	SIMPLE		02	/* Simple enough to be STAR operand. */
X#define	SPSTART		04	/* Starts with * */
X#define	WORST		0	/* Worst case. */
X
X/*
X * Global work variables for regcomp().
X */
Xstatic char    *regparse;	/* Input-scan pointer. */
Xstatic int      regnpar;	/* () count. */
Xstatic char     regdummy;
Xstatic char    *regcode;	/* Code-emit pointer; &regdummy = don't. */
Xstatic long     regsize;	/* Code size. */
X
X/*
X * Forward declarations for regcomp()'s friends.
X */
X#ifndef STATIC
X#define	STATIC	static
X#endif
XSTATIC char    *reg();
XSTATIC char    *regbranch();
XSTATIC char    *regpiece();
XSTATIC char    *regatom();
XSTATIC char    *regnode();
XSTATIC char    *regnext();
XSTATIC void     regc();
XSTATIC void     reginsert();
XSTATIC void     regtail();
XSTATIC void     regoptail();
X#ifdef STRCSPN
XSTATIC int      strcspn();
X#endif
X
X/*
X - regcomp - compile a regular expression into internal code
X *
X * We can't allocate space until we know how big the compiled form will be,
X * but we can't compile it (and thus know how big it is) until we've got a
X * place to put the code.  So we cheat:  we compile it twice, once with code
X * generation turned off and size counting turned on, and once "for real".
X * This also means that we don't allocate space until we are sure that the
X * thing really will compile successfully, and we never have to move the
X * code and thus invalidate pointers into it.  (Note that it has to be in
X * one piece because free() must be able to free it all.)
X *
X * Beware that the optimization-preparation code in here knows about some
X * of the structure of the compiled regexp.
X */
Xregexp *regcomp(exp)
Xchar           *exp;
X{
X    register regexp *r;
X    register char  *scan;
X    register char  *longest;
X    register int    len;
X    int             flags;
X    extern char    *malloc();
X
X    if (exp == NULL)
X	FAIL("NULL argument");
X
X    /* First pass: determine size, legality. */
X    regparse = exp;
X    regnpar = 1;
X    regsize = 0L;
X    regcode = &regdummy;
X    regc(MAGIC);
X    if (reg(0, &flags) == NULL)
X	return (NULL);
X
X    /* Small enough for pointer-storage convention? */
X    if (regsize >= 32767L)	/* Probably could be 65535L. */
X	FAIL("regexp too big");
X
X    /* Allocate space. */
X    r = (regexp *) malloc(sizeof(regexp) + (unsigned) regsize);
X    if (r == NULL)
X	FAIL("out of space");
X
X    /* Second pass: emit code. */
X    regparse = exp;
X    regnpar = 1;
X    regcode = r->program;
X    regc(MAGIC);
X    if (reg(0, &flags) == NULL)
X	return (NULL);
X
X    /* Dig out information for optimizations. */
X    r->regstart = '\0';		/* Worst-case defaults. */
X    r->reganch = 0;
X    r->regmust = NULL;
X    r->regmlen = 0;
X    scan = r->program + 1;	/* First BRANCH. */
X    if (OP(regnext(scan)) == END) {	/* Only one top-level choice. */
X	scan = OPERAND(scan);
X
X	/* Starting-point info. */
X	if (OP(scan) == EXACTLY)
X	    r->regstart = *OPERAND(scan);
X	else if (OP(scan) == BOL)
X	    r->reganch++;
X
X	/*
X	 * If there's something expensive in the r.e., find the longest
X	 * literal string that must appear and make it the regmust.  Resolve
X	 * ties in favor of later strings, since the regstart check works
X	 * with the beginning of the r.e. and avoiding duplication
X	 * strengthens checking.  Not a strong reason, but sufficient in the
X	 * absence of others. 
X	 */
X	if (flags & SPSTART) {
X	    longest = NULL;
X	    len = 0;
X	    for (; scan != NULL; scan = regnext(scan))
X		if (OP(scan) == EXACTLY && strlen(OPERAND(scan)) >= len) {
X		    longest = OPERAND(scan);
X		    len = strlen(OPERAND(scan));
X		}
X	    r->regmust = longest;
X	    r->regmlen = len;
X	}
X    }
X    return (r);
X}
X
X/*
X - reg - regular expression, i.e. main body or parenthesized thing
X *
X * Caller must absorb opening parenthesis.
X *
X * Combining parenthesis handling with the base level of regular expression
X * is a trifle forced, but the need to tie the tails of the branches to what
X * follows makes it hard to avoid.
X */
Xstatic char *reg(paren, flagp)
Xint             paren;		/* Parenthesized? */
Xint            *flagp;
X{
X    register char  *ret;
X    register char  *br;
X    register char  *ender;
X    register int    parno;
X    int             flags;
X
X    *flagp = HASWIDTH;		/* Tentatively. */
X
X    /* Make an OPEN node, if parenthesized. */
X    if (paren) {
X	if (regnpar >= NSUBEXP)
X	    FAIL("too many ()");
X	parno = regnpar;
X	regnpar++;
X	ret = regnode(OPEN + parno);
X    } else
X	ret = NULL;
X
X    /* Pick up the branches, linking them together. */
X    br = regbranch(&flags);
X    if (br == NULL)
X	return (NULL);
X    if (ret != NULL)
X	regtail(ret, br);	/* OPEN -> first. */
X    else
X	ret = br;
X    if (!(flags & HASWIDTH))
X	*flagp &= ~HASWIDTH;
X    *flagp |= flags & SPSTART;
X    while (*regparse == '|') {
X	regparse++;
X	br = regbranch(&flags);
X	if (br == NULL)
X	    return (NULL);
X	regtail(ret, br);	/* BRANCH -> BRANCH. */
X	if (!(flags & HASWIDTH))
X	    *flagp &= ~HASWIDTH;
X	*flagp |= flags & SPSTART;
X    }
X
X    /* Make a closing node, and hook it on the end. */
X    ender = regnode((paren) ? CLOSE + parno : END);
X    regtail(ret, ender);
X
X    /* Hook the tails of the branches to the closing node. */
X    for (br = ret; br != NULL; br = regnext(br))
X	regoptail(br, ender);
X
X    /* Check for proper termination. */
X    if (paren && *regparse++ != ')') {
X	FAIL("unmatched ()");
X    } else if (!paren && *regparse != '\0') {
X	if (*regparse == ')') {
X	    FAIL("unmatched ()");
X	} else
X	    FAIL("junk on end");/* "Can't happen". */
X	/* NOTREACHED */
X    }
X    return (ret);
X}
X
X/*
X - regbranch - one alternative of an | operator
X *
X * Implements the concatenation operator.
X */
Xstatic char  *regbranch(flagp)
Xint            *flagp;
X{
X    register char  *ret;
X    register char  *chain;
X    register char  *latest;
X    int             flags;
X
X    *flagp = WORST;		/* Tentatively. */
X
X    ret = regnode(BRANCH);
X    chain = NULL;
X    while (*regparse != '\0' && *regparse != '|' && *regparse != ')') {
X	latest = regpiece(&flags);
X	if (latest == NULL)
X	    return (NULL);
X	*flagp |= flags & HASWIDTH;
X	if (chain == NULL)	/* First piece. */
X	    *flagp |= flags & SPSTART;
X	else
X	    regtail(chain, latest);
X	chain = latest;
X    }
X    if (chain == NULL)		/* Loop ran zero times. */
X	regnode(NOTHING);
X
X    return (ret);
X}
X
X/*
X - regpiece - something followed by possible [*]
X *
X * Note that the branching code sequence used for * is somewhat optimized:  
X * they use the same NOTHING node as both the endmarker for their branch 
X * list and the body of the last branch.  It might seem that this node could 
X * be dispensed with entirely, but the endmarker role is not redundant.
X */
Xstatic char *regpiece(flagp)
Xint            *flagp;
X{
X    register char  *ret;
X    register char   op;
X    register char  *nxt;
X    int             flags;
X
X    ret = regatom(&flags);
X    if (ret == NULL)
X	return (NULL);
X
X    op = *regparse;
X    if (!ISMULT(op)) {
X	*flagp = flags;
X	return (ret);
X    }
X    if (!(flags & HASWIDTH))
X	FAIL("* operand could be empty");
X    *flagp = (WORST | SPSTART);
X
X    if (op == '*' && (flags & SIMPLE))
X	reginsert(STAR, ret);
X    else if (op == '*') {
X	/* Emit x* as (x&|), where & means "self". */
X	reginsert(BRANCH, ret);	/* Either x */
X	regoptail(ret, regnode(BACK));	/* and loop */
X	regoptail(ret, ret);	/* back */
X	regtail(ret, regnode(BRANCH));	/* or */
X	regtail(ret, regnode(NOTHING));	/* null. */
X    } 
X    regparse++;
X    if (ISMULT(*regparse))
X	FAIL("nested *");
X
X    return (ret);
X}
X
X/*
X - regatom - the lowest level
X *
X * Optimization:  gobbles an entire sequence of ordinary characters so that
X * it can turn them into a single node, which is smaller to store and
X * faster to run.  Backslashed characters are exceptions, each becoming a
X * separate node; the code is simpler that way and it's not worth fixing.
X */
Xstatic char *regatom(flagp)
Xint            *flagp;
X{
X    register char  *ret;
X    int             flags;
X
X    *flagp = WORST;		/* Tentatively. */
X
X    switch (*regparse++) {
X    case '^':
X	ret = regnode(BOL);
X	break;
X    case '$':
X	ret = regnode(EOL);
X	break;
X    case '.':
X	ret = regnode(ANY);
X	*flagp |= HASWIDTH | SIMPLE;
X	break;
X    case '[':{
X	    register int    class;
X	    register int    classend;
X
X	    if (*regparse == '^') {	/* Complement of range. */
X		ret = regnode(ANYBUT);
X		regparse++;
X	    } else
X		ret = regnode(ANYOF);
X	    if (*regparse == ']' || *regparse == '-')
X		regc(*regparse++);
X	    while (*regparse != '\0' && *regparse != ']') {
X		if (*regparse == '-') {
X		    regparse++;
X		    if (*regparse == ']' || *regparse == '\0')
X			regc('-');
X		    else {
X			class = UCHARAT(regparse - 2) + 1;
X			classend = UCHARAT(regparse);
X			if (class > classend + 1)
X			    FAIL("invalid [] range");
X			for (; class <= classend; class++)
X			    regc(class);
X			regparse++;
X		    }
X		} else
X		    regc(*regparse++);
X	    }
X	    regc('\0');
X	    if (*regparse != ']')
X		FAIL("unmatched []");
X	    regparse++;
X	    *flagp |= HASWIDTH | SIMPLE;
X	}
X	break;
X    case '(':
X	ret = reg(1, &flags);
X	if (ret == NULL)
X	    return (NULL);
X	*flagp |= flags & (HASWIDTH | SPSTART);
X	break;
X    case '\0':
X    case '|':
X    case ')':
X	FAIL("internal urp");	/* Supposed to be caught earlier. */
X	break;
X    case '*':
X	FAIL("* follows nothing");
X	break;
X    case '\\':
X	if (*regparse == '\0')
X	    FAIL("trailing \\");
X	ret = regnode(EXACTLY);
X	regc(*regparse++);
X	regc('\0');
X	*flagp |= HASWIDTH | SIMPLE;
X	break;
X    default:{
X	    register int    len;
X	    register char   ender;
X
X	    regparse--;
X	    len = strcspn(regparse, META);
X	    if (len <= 0)
X		FAIL("internal disaster");
X	    ender = *(regparse + len);
X	    if (len > 1 && ISMULT(ender))
X		len--;		/* Back off clear of * operand. */
X	    *flagp |= HASWIDTH;
X	    if (len == 1)
X		*flagp |= SIMPLE;
X	    ret = regnode(EXACTLY);
X	    while (len > 0) {
X		regc(*regparse++);
X		len--;
X	    }
X	    regc('\0');
X	}
X	break;
X    }
X
X    return (ret);
X}
X
X/*
X - regnode - emit a node
X */
Xstatic char *regnode(op)
Xchar            op;
X{
X    register char  *ret;
X    register char  *ptr;
X
X    ret = regcode;
X    if (ret == &regdummy) {
X	regsize += 3;
X	return (ret);
X    }
X    ptr = ret;
X    *ptr++ = op;
X    *ptr++ = '\0';		/* Null "nxt" pointer. */
X    *ptr++ = '\0';
X    regcode = ptr;
X
X    return (ret);
X}
X
X/*
X - regc - emit (if appropriate) a byte of code
X */
Xstatic void regc(b)
Xchar            b;
X{
X    if (regcode != &regdummy)
X	*regcode++ = b;
X    else
X	regsize++;
X}
X
X/*
X - reginsert - insert an operator in front of already-emitted operand
X *
X * Means relocating the operand.
X */
Xstatic void reginsert(op, opnd)
Xchar            op;
Xchar           *opnd;
X{
X    register char  *src;
X    register char  *dst;
X    register char  *place;
X
X    if (regcode == &regdummy) {
X	regsize += 3;
X	return;
X    }
X    src = regcode;
X    regcode += 3;
X    dst = regcode;
X    while (src > opnd)
X	*--dst = *--src;
X
X    place = opnd;		/* Op node, where operand used to be. */
X    *place++ = op;
X    *place++ = '\0';
X    *place++ = '\0';
X}
X
X/*
X - regtail - set the next-pointer at the end of a node chain
X */
Xstatic void regtail(p, val)
Xchar           *p;
Xchar           *val;
X{
X    register char  *scan;
X    register char  *temp;
X    register int    offset;
X
X    if (p == &regdummy)
X	return;
X
X    /* Find last node. */
X    scan = p;
X    for (;;) {
X	temp = regnext(scan);
X	if (temp == NULL)
X	    break;
X	scan = temp;
X    }
X
X    if (OP(scan) == BACK)
X	offset = scan - val;
X    else
X	offset = val - scan;
X    *(scan + 1) = (offset >> 8) & 0377;
X    *(scan + 2) = offset & 0377;
X}
X
X/*
X - regoptail - regtail on operand of first argument; nop if operandless
X */
Xstatic void regoptail(p, val)
Xchar           *p;
Xchar           *val;
X{
X    /* "Operandless" and "op != BRANCH" are synonymous in practice. */
X    if (p == NULL || p == &regdummy || OP(p) != BRANCH)
X	return;
X    regtail(OPERAND(p), val);
X}
X
X/*
X * regexec and friends
X */
X
X/*
X * Global work variables for regexec().
X */
Xstatic char    *reginput;	/* String-input pointer. */
Xstatic char    *regbol;		/* Beginning of input, for ^ check. */
Xstatic char   **regstartp;	/* Pointer to startp array. */
Xstatic char   **regendp;	/* Ditto for endp. */
X
X/*
X * Forwards.
X */
XSTATIC int      regtry();
XSTATIC int      regmatch();
XSTATIC int      regrepeat();
X
X#ifdef DEBUG
Xint             regnarrate = 0;
Xvoid            regdump();
XSTATIC char    *regprop();
X#endif
X
X/*
X - regexec - match a regexp against a string
X */
Xint regexec(prog, string)
Xregister regexp *prog;
Xregister char  *string;
X{
X    register char  *s;
X
X    /* Be paranoid... */
X    if (prog == NULL || string == NULL) {
X	regerror("NULL parameter");
X	return (0);
X    }
X    /* Check validity of program. */
X    if (UCHARAT(prog->program) != MAGIC) {
X	regerror("corrupted program");
X	return (0);
X    }
X    /* If there is a "must appear" string, look for it. */
X    if (prog->regmust != NULL) {
X	s = string;
X	while ((s = strchr(s, prog->regmust[0])) != NULL) {
X	    if (strncmp(s, prog->regmust, prog->regmlen) == 0)
X		break;		/* Found it. */
X	    s++;
X	}
X	if (s == NULL)		/* Not present. */
X	    return (0);
X    }
X    /* Mark beginning of line for ^ . */
X    regbol = string;
X
X    /* Simplest case:  anchored match need be tried only once. */
X    if (prog->reganch)
X	return (regtry(prog, string));
X
X    /* Messy cases:  unanchored match. */
X    s = string;
X    if (prog->regstart != '\0')
X	/* We know what char it must start with. */
X	while ((s = strchr(s, prog->regstart)) != NULL) {
X	    if (regtry(prog, s))
X		return (1);
X	    s++;
X	}
X    else
X	/* We don't -- general case. */
X	do {
X	    if (regtry(prog, s))
X		return (1);
X	} while (*s++ != '\0');
X
X    /* Failure. */
X    return (0);
X}
X
X/*
X - regtry - try match at specific point
X */
X#ifdef __STDC__
X
Xstatic int regtry(regexp *prog, char *string)
X
X#else
X
Xstatic int regtry(prog, string)
Xregexp         *prog;
Xchar           *string;
X
X#endif
X{
X    register int    i;
X    register char **sp;
X    register char **ep;
X
X    reginput = string;
X    regstartp = prog->startp;
X    regendp = prog->endp;
X
X    sp = prog->startp;
X    ep = prog->endp;
X    for (i = NSUBEXP; i > 0; i--) {
X	*sp++ = NULL;
X	*ep++ = NULL;
X    }
X    if (regmatch(prog->program + 1)) {
X	prog->startp[0] = string;
X	prog->endp[0] = reginput;
X	return (1);
X    } else
X	return (0);
X}
X
X/*
X - regmatch - main matching routine
X *
X * Conceptually the strategy is simple:  check to see whether the current
X * node matches, call self recursively to see whether the rest matches,
X * and then act accordingly.  In practice we make some effort to avoid
X * recursion, in particular by going through "ordinary" nodes (that don't
X * need to know whether the rest of the match failed) by a loop instead of
X * by recursion.
X */
X#ifdef __STDC__
X
Xstatic int regmatch(char *prog)
X
X#else
X
Xstatic int regmatch(prog)
Xchar           *prog;
X
X#endif
X{
X    register char  *scan;	/* Current node. */
X    char           *nxt;	/* nxt node. */
X
X    scan = prog;
X#ifdef DEBUG
X    if (scan != NULL && regnarrate)
X	fprintf(stderr, "%s(\n", regprop(scan));
X#endif
X    while (scan != NULL) {
X#ifdef DEBUG
X	if (regnarrate)
X	    fprintf(stderr, "%s...\n", regprop(scan));
X#endif
X	nxt = regnext(scan);
X
X	switch (OP(scan)) {
X	case BOL:
X	    if (reginput != regbol)
X		return (0);
X	    break;
X	case EOL:
X	    if (*reginput != '\0')
X		return (0);
X	    break;
X	case ANY:
X	    if (*reginput == '\0')
X		return (0);
X	    reginput++;
X	    break;
X	case EXACTLY:{
X		register int    len;
X		register char  *opnd;
X
X		opnd = OPERAND(scan);
X		/* Inline the first character, for speed. */
X		if (*opnd != *reginput)
X		    return (0);
X		len = strlen(opnd);
X		if (len > 1 && strncmp(opnd, reginput, len) != 0)
X		    return (0);
X		reginput += len;
X	    }
X	    break;
X	case ANYOF:
X	    if (*reginput == '\0' || strchr(OPERAND(scan), *reginput) == NULL)
X		return (0);
X	    reginput++;
X	    break;
X	case ANYBUT:
X	    if (*reginput == '\0' || strchr(OPERAND(scan), *reginput) != NULL)
X		return (0);
X	    reginput++;
X	    break;
X	case NOTHING:
X	    break;
X	case BACK:
X	    break;
X	case OPEN + 1:
X	case OPEN + 2:
X	case OPEN + 3:
X	case OPEN + 4:
X	case OPEN + 5:
X	case OPEN + 6:
X	case OPEN + 7:
X	case OPEN + 8:
X	case OPEN + 9:{
X		register int    no;
X		register char  *save;
X
X		no = OP(scan) - OPEN;
X		save = reginput;
X
X		if (regmatch(nxt)) {
X		    /*
X		     * Don't set startp if some later invocation of the same
X		     * parentheses already has. 
X		     */
X		    if (regstartp[no] == NULL)
X			regstartp[no] = save;
X		    return (1);
X		} else
X		    return (0);
X	    }
X	    break;
X	case CLOSE + 1:
X	case CLOSE + 2:
X	case CLOSE + 3:
X	case CLOSE + 4:
X	case CLOSE + 5:
X	case CLOSE + 6:
X	case CLOSE + 7:
X	case CLOSE + 8:
X	case CLOSE + 9:{
X		register int    no;
X		register char  *save;
X
X		no = OP(scan) - CLOSE;
X		save = reginput;
X
X		if (regmatch(nxt)) {
X		    /*
X		     * Don't set endp if some later invocation of the same
X		     * parentheses already has. 
X		     */
X		    if (regendp[no] == NULL)
X			regendp[no] = save;
X		    return (1);
X		} else
X		    return (0);
X	    }
X	    break;
X	case BRANCH:{
X		register char  *save;
X
X		if (OP(nxt) != BRANCH)	/* No choice. */
X		    nxt = OPERAND(scan);	/* Avoid recursion. */
X		else {
X		    do {
X			save = reginput;
X			if (regmatch(OPERAND(scan)))
X			    return (1);
X			reginput = save;
X			scan = regnext(scan);
X		    } while (scan != NULL && OP(scan) == BRANCH);
X		    return (0);
X		    /* NOTREACHED */
X		}
X	    }
X	    break;
X	case STAR:{
X		register char   nextch;
X		register int    no;
X		register char  *save;
X		register int    min;
X
X		/*
X		 * Lookahead to avoid useless match attempts when we know
X		 * what character comes next. 
X		 */
X		nextch = '\0';
X		if (OP(nxt) == EXACTLY)
X		    nextch = *OPERAND(nxt);
X		min = (OP(scan) == STAR) ? 0 : 1;
X		save = reginput;
X		no = regrepeat(OPERAND(scan));
X		while (no >= min) {
X		    /* If it could work, try it. */
X		    if (nextch == '\0' || *reginput == nextch)
X			if (regmatch(nxt))
X			    return (1);
X		    /* Couldn't or didn't -- back up. */
X		    no--;
X		    reginput = save + no;
X		}
X		return (0);
X	    }
X	    break;
X	case END:
X	    return (1);		/* Success! */
X	    break;
X	default:
X	    regerror("memory corruption");
X	    return (0);
X	    break;
X	}
X
X	scan = nxt;
X    }
X
X    /*
X     * We get here only if there's trouble -- normally "case END" is the
X     * terminating point. 
X     */
X    regerror("corrupted pointers");
X    return (0);
X}
X
X/*
X - regrepeat - repeatedly match something simple, report how many
X */
X#ifdef __STDC__
X
Xstatic int regrepeat(char *p)
X
X#else
X
Xstatic int regrepeat(p)
Xchar           *p;
X
X#endif
X{
X    register int    count = 0;
X    register char  *scan;
X    register char  *opnd;
X
X    scan = reginput;
X    opnd = OPERAND(p);
X    switch (OP(p)) {
X    case ANY:
X	count = strlen(scan);
X	scan += count;
X	break;
X    case EXACTLY:
X	while (*opnd == *scan) {
X	    count++;
X	    scan++;
X	}
X	break;
X    case ANYOF:
X	while (*scan != '\0' && strchr(opnd, *scan) != NULL) {
X	    count++;
X	    scan++;
X	}
X	break;
X    case ANYBUT:
X	while (*scan != '\0' && strchr(opnd, *scan) == NULL) {
X	    count++;
X	    scan++;
X	}
X	break;
X    default:			/* Oh dear.  Called inappropriately. */
X	regerror("internal foulup");
X	count = 0;		/* Best compromise. */
X	break;
X    }
X    reginput = scan;
X
X    return (count);
X}
X
X
X/*
X - regnext - dig the "nxt" pointer out of a node
X */
X#ifdef __STDC__
X
Xstatic char *regnext(register char *p)
X
X#else
X
Xstatic char *regnext(p)
Xregister char  *p;
X
X#endif
X{
X    register int    offset;
X
X    if (p == &regdummy)
X	return (NULL);
X
X    offset = NEXT(p);
X    if (offset == 0)
X	return (NULL);
X
X    if (OP(p) == BACK)
X	return (p - offset);
X    else
X	return (p + offset);
X}
X
X#ifdef DEBUG
X
XSTATIC char    *regprop();
X
X/*
X - regdump - dump a regexp onto stdout in vaguely comprehensible form
X */
X#ifdef __STDC__
X
Xvoid regdump(regexp *r)
X
X#else
X
Xvoid regdump(r)
Xregexp         *r;
X
X#endif
X{
X    register char  *s;
X    register char   op = EXACTLY;	/* Arbitrary non-END op. */
X    register char  *nxt;
X    extern char    *strchr();
X
X
X    s = r->program + 1;
X    while (op != END) {		/* While that wasn't END last time... */
X	op = OP(s);
X	printf("%2d%s", s - r->program, regprop(s));	/* Where, what. */
X	nxt = regnext(s);
X	if (nxt == NULL)	/* nxt ptr. */
X	    printf("(0)");
X	else
X	    printf("(%d)", (s - r->program) + (nxt - s));
X	s += 3;
X	if (op == ANYOF || op == ANYBUT || op == EXACTLY) {
X	    /* Literal string, where present. */
X	    while (*s != '\0') {
X		putchar(*s);
X		s++;
X	    }
X	    s++;
X	}
X	putchar('\n');
X    }
X
X    /* Header fields of interest. */
X    if (r->regstart != '\0')
X	printf("start `%c' ", r->regstart);
X    if (r->reganch)
X	printf("anchored ");
X    if (r->regmust != NULL)
X	printf("must have \"%s\"", r->regmust);
X    printf("\n");
X}
X
X/*
X - regprop - printable representation of opcode
X */
X#ifdef __STDC__
X
Xstatic char *regprop(char *op)
X
X#else
X
Xstatic char *regprop(op)
Xchar           *op;
X
X#endif
X{
X    register char  *p;
X    static char     buf[50];
X
X    strcpy(buf, ":");
X
X    switch (OP(op)) {
X    case BOL:
X	p = "BOL";
X	break;
X    case EOL:
X	p = "EOL";
X	break;
X    case ANY:
X	p = "ANY";
X	break;
X    case ANYOF:
X	p = "ANYOF";
X	break;
X    case ANYBUT:
X	p = "ANYBUT";
X	break;
X    case BRANCH:
X	p = "BRANCH";
X	break;
X    case EXACTLY:
X	p = "EXACTLY";
X	break;
X    case NOTHING:
X	p = "NOTHING";
X	break;
X    case BACK:
X	p = "BACK";
X	break;
X    case END:
X	p = "END";
X	break;
X    case OPEN + 1:
X    case OPEN + 2:
X    case OPEN + 3:
X    case OPEN + 4:
X    case OPEN + 5:
X    case OPEN + 6:
X    case OPEN + 7:
X    case OPEN + 8:
X    case OPEN + 9:
X	sprintf(buf + strlen(buf), "OPEN%d", OP(op) - OPEN);
X	p = NULL;
X	break;
X    case CLOSE + 1:
X    case CLOSE + 2:
X    case CLOSE + 3:
X    case CLOSE + 4:
X    case CLOSE + 5:
X    case CLOSE + 6:
X    case CLOSE + 7:
X    case CLOSE + 8:
X    case CLOSE + 9:
X	sprintf(buf + strlen(buf), "CLOSE%d", OP(op) - CLOSE);
X	p = NULL;
X	break;
X    case STAR:
X	p = "STAR";
X	break;
X    default:
X	regerror("corrupted opcode");
X	break;
X    }
X    if (p != NULL)
X	strcat(buf, p);
X    return (buf);
X}
X#endif
X
X/*
X * The following is provided for those people who do not have strcspn() in
X * their C libraries.  They should get off their butts and do something
X * about it; at least one public-domain implementation of those (highly
X * useful) string routines has been published on Usenet.
X */
X#ifdef STRCSPN
X/*
X * strcspn - find length of initial segment of s1 consisting entirely
X * of characters not from s2
X */
X
X#ifdef __STDC__
X
Xstatic int strcspn(char *s1, char *s2)
X
X#else
X
Xstatic int strcspn(s1, s2)
Xchar           *s1;
Xchar           *s2;
X
X#endif
X{
X    register char  *scan1;
X    register char  *scan2;
X    register int    count;
X
X    count = 0;
X    for (scan1 = s1; *scan1 != '\0'; scan1++) {
X	for (scan2 = s2; *scan2 != '\0';)	/* ++ moved down. */
X	    if (*scan1 == *scan2++)
X		return (count);
X	count++;
X    }
X    return (count);
X}
X#endif
X
X
X/*
X - regsub - perform substitutions after a regexp match
X */
X#ifdef __STDC__
X
Xvoid regsub(regexp *prog, char *source, char *dest)
X
X#else
X
Xvoid regsub(prog, source, dest)
Xregexp         *prog;
Xchar           *source;
Xchar           *dest;
X
X#endif
X{
X    register char  *src;
X    register char  *dst;
X    register char   c;
X    register int    no;
X    register int    len;
X    extern char    *strncpy();
X
X    if (prog == NULL || source == NULL || dest == NULL) {
X	regerror("NULL parm to regsub");
X	return;
X    }
X    if (UCHARAT(prog->program) != MAGIC) {
X	regerror("damaged regexp fed to regsub");
X	return;
X    }
X    src = source;
X    dst = dest;
X    while ((c = *src++) != '\0') {
X	if (c == '&')
X	    no = 0;
X	else if (c == '\\' && '0' <= *src && *src <= '9')
X	    no = *src++ - '0';
X	else
X	    no = -1;
X
X	if (no < 0) {		/* Ordinary character. */
X	    if (c == '\\' && (*src == '\\' || *src == '&'))
X		c = *src++;
X	    *dst++ = c;
X	} else if (prog->startp[no] != NULL && prog->endp[no] != NULL) {
X	    len = prog->endp[no] - prog->startp[no];
X	    strncpy(dst, prog->startp[no], len);
X	    dst += len;
X	    if (len != 0 && *(dst - 1) == '\0') {	/* strncpy hit NUL. */
X		regerror("damaged match string");
X		return;
X	    }
X	}
X    }
X    *dst++ = '\0';
X}
X
X
X#ifdef __STDC__
X
Xvoid regerror(char *s)
X
X#else
X
Xvoid regerror(s)
Xchar           *s;
X
X#endif
X{
X    fprintf(stderr, "regexp(3): %s", s);
X    exit(1);
X}
END_OF_regexp.c
if test 30611 -ne `wc -c <regexp.c`; then
    echo shar: \"regexp.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f 'Makefile' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'Makefile'\"
else
echo shar: Extracting \"'Makefile'\" \(3296 characters\)
sed "s/^X//" >'Makefile' <<'END_OF_FILE'
X#
X# PAX - read and write POSIX conformant tar and cpio archives 
X#
X# Written by Mark H. Colburn (mark@jhereg.mn.org)
X#
X# $Id: Makefile,v 1.1 88/12/23 18:02:42 mark Rel $
X#
X
X#
X# CONFIGURATION SECTION
X#
X# The following defines may need to be changed for each system which PAX
X# is installed on.  Please review these settings before installing on your
X# system.
X
X#
X# You should define _POSIX_SOURCE if you are running on a POSIX system.  This
X# include has to be in the command line because it has to appear before any
X# include file is included in the source.  For most systems in use today,
X# it should be left blank.
X# 
X# POSIX= -D_POSIX_SOURCE
XPOSIX= 
X
X#
X# Set CFLAGS to whatever makes your C compiler happy.  Be sure to include 
X# the definition of $(POSIX) in the flag.
X#
XCFLAGS = -O $(POSIX)
XCC = cc
X
X#
X# Set LFLAGS to whatever makes your linker happy
X#
X#LDFLAGS = -s
XLDFLAGS = 
X
X#
X# Set LINTFLAGS to whatever makes your implementation of lint happy.  If
X# you don't undef __STDC__ and you have an ANSI C compiler, lint will choke 
X# on the function prototypes present in func.h
X#
XLINTFLAGS = -U__STDC__ $(POSIX)
X
X#
X# BINDIR - points to the directory in which you want the final pax, tar and
X# cpio binaries installed in.
X#
XBINDIR = /usr/local/bin
X
X#
X# MANDIR - specify the directory in which the man pages will be installed
X#
XMAN5 = /usr/man/man5
XMAN1 = /usr/man/man1
X
X#
X# There are three different ways to get POSIX or BSD conformant directory 
X# access routines: 1) they are installed in your system library, 2) you 
X# are using Doug Gwyn's dirent library (/usr/lib/libdirent.a), or 3) you 
X# need the source for the dirent package.  Based on that, pick one of the 
X# following three options:
X#
X# 1. Pick the first dirent line and make sure that config.h is defined
X#    correctly for your version of directory access routines.  THIS IS
X#    THE LINE WHICH SHOULD BE USED FOR BSD SYSTEMS.
X# 2. Chose the second dirent line which  used a library at link time.  You
X#    may need to change the name of the library to match your system.
X# 3. If you need #3, then you must copy everything in the subdirectory dirent 
X#    to this directory and choose the DIROBJ lines.  Please note that this 
X#    version of dirent has been modified to work as a stand-alone. 
X#
XDIRENT=
X#DIRENT= -ldirent
X#DIROBJ= paxdir.o
X
X#
X# END CONFIGURATION SECTION 
X#
X# Nothing beyond this point should need to be changed.
X#
X
XMISC   = Makefile pax.1 tar.5 cpio.5 README PATCHLEVEL
XHEADERS= config.h func.h limits.h port.h pax.h 
XSOURCE= pax.c append.c buffer.c cpio.c create.c extract.c fileio.c\
X	link.c list.c mem.c namelist.c names.c pass.c pathname.c\
X	port.c regexp.c replace.c tar.c ttyio.c warn.c wildmat.c
XOBJECT= pax.o append.o buffer.o cpio.o create.o extract.o fileio.o\
X	link.o list.o mem.o namelist.o names.o pass.o pathname.o\
X	port.o regexp.o replace.o tar.o ttyio.o warn.o wildmat.o $(DIROBJ)
XPROGS = pax tar cpio
XPMAN1 = pax.1 tar.1
XPMAN5 = pax.5 tar.5
X
Xall: $(PROGS)
X
Xinstall: $(PROGS)
X	mv $(PROGS) $(BINDIR)
X	cp $(PMAN1) $(MAN1)
X#	cp $(PMAN5) $(MAN5)
X
Xclean:
X	rm -f $(OBJECT)
X	rm -f $(PROGS) a.out *.BAK *.bak 
X
Xlint:
X	lint $(LINTFLAGS) $(SOURCE)
X
Xpax : $(OBJECT)
X	$(CC) $(CFLAGS) -o pax $(OBJECT) $(DIRENT) 
X
Xtar: pax
X	rm -f tar
X	ln pax tar
X
Xcpio: pax
X	rm -f cpio
X	ln pax cpio
X
X$(OBJECT): $(HEADERS)
END_OF_FILE
if test 3296 -ne `wc -c <'Makefile'`; then
    echo shar: \"'Makefile'\" unpacked with wrong size!
fi
# end of 'Makefile'
fi
echo shar: End of archive 6 \(of 6\).
cp /dev/null ark6isdone
MISSING=""
for I in 1 2 3 4 5 6 ; do
    if test ! -f ark${I}isdone ; then
	MISSING="${MISSING} ${I}"
    fi
done
if test "${MISSING}" = "" ; then
    echo You have unpacked all 6 archives.
    rm -f ark[1-9]isdone
else
    echo You still need to unpack the following archives:
    echo "        " ${MISSING}
fi
##  End of shell archive.
exit 0

