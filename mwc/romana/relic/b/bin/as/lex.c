/*
 * This is the controlling loop in the assembler.
 *
 */
#include <utype.h>
#include <asm.h>
#include <asflags.h>
#include <y_tab.h>
#include <symtab.h>

/* states */
#define INITIAL	0
#define INSYM   1
#define INCHAR	2	/* in char constant */
#define LTS	3
#define GTS	4
#define BSLP	5	/* back slash while reading parms */
#define WHITE	6	/* in white space */
#define ENDLINE 7
#define CSTATE	8	/* Pick up parms in macro expansion */
#define INTOKEN 9

#define OPONLY1	10	/* search for operator only */
#define OPONLY2 11
#define OPONLY3	12
#define OPONLY4	13

#define INSTR	14	/* string detected */
#define BSL	15	/* back slash */
#define BSLDIG  16	/* digit after back slash */

#define CMSTATE 17	/* pick up names in macro definition */
#define INNAME	18

#define LZERO	19	/* lead zero */
#define XNUM	20	/* hex number */
#define DNUM	21	/* decimal number */
#define ONUM	22	/* octal number */
#define BNUM	23	/* binary number */

#define BANGS	 24	/* ! seen */
#define EQUS	 25	/* = seen */

#define DSTATE	 26	/* eat whole line for S_EQUS */

/* positions */
#define LABEL	0
#define OPCODE	1
#define OPERAND	2
#define COMMENT 3

/* comparisons */
#define LT 1
#define GT 2
#define EQ 4

#define RET(toYacc) return(lastToken = (toYacc))
#define RSTATE(toYacc, newstate) { state = newstate; RET(toYacc); }
#define NEWS(newstate) { state = newstate; continue; }

static unsigned short state = INITIAL;
static unsigned short pstate;	/* previous state when \ detected */
static unsigned short pos = LABEL;

static unsigned short ctype;	/* current logic type */

static char buf[1024], *bp = buf;
static char *backOff;	/* start of decimal number */
static char fromMac = ' ';		/* ' ' or '+' */
static short opIx;	/* index of current opcode */
static psw = 0;
static long number = 0;
static char token[1024], *tp;

static int lastChar;	/* last Character in */
static short vctr, scnt, dcnt;
static symt *stype;
static short ytype;

/*
 * Invalid character message.
 */
static void
invChar(c, s)
register short c;
char *s;
{
	unsigned short p;

	p = bp - lastL;
	if (isprint(c))
		yyerror("Invalid character '%c' %s at position %d", c, s, p);
		/**/
	else
		yyerror("Invalid character 0x%02x %s at position %d",
			c & 255, s, p); /**/
}

/*
 * Expand parms in macro line.
 * # as first char of line foils replacment.
 */
static char *
expand(s, recurs)
register char *s;
{
	register char c;
	char *tp, *rep, *newl, *start;
	macro *mac;
	short sav, pb;
	unsigned short point;

	start = s;
	pb = 0;
	for(state = WHITE; ; ) {
		switch(state) {
		case WHITE:
			if (isalpha(c = *s++)) {
				state = INSYM;
				tp = token;
				*tp++ = c;
				break;
			}
			pb = 0;
			break;
		case INSYM:
			if (isalnum(c = *s++)) {
				*tp++ = c;
				break;
			}
			mac = NULL;
			*tp = '\0';
			if ((!recurs && (NULL != trueMac) &&
			    (NULL != (rep = lookList(token)))) ||
			   (NULL != (mac = macLookUp(token, MACSTR)))) {
				tp = --s - (strlen(token) + pb);
				sav = *tp;	/* stomp parm name */
				*tp = '\0';
				if (NULL == mac)	/* parm match */
					rep = expand(rep, 1);
				else {	/* equs match */
					rep = mac->names->str;
					mac->type = MACSCAN;
					rep = expand(rep, 1);
					mac->type = MACSTR;
				}
				point = strlen(start) + strlen(rep);
				newl = galloc(point + strlen(s) + 1);
				sprintf(newl, "%s%s%s", start, rep, s);
				*tp = sav;	/* Un stomp parm name */
				start = newl;
				s = start + point; /* don't rescan */
			}
			pb = (c == '\\');
			state = WHITE;
		}
		if (!c)
			break;
	}
	return(start);
}

/*
 * Get next line from macro or while.
 */
static char *
nextMac()
{
	macctl *tmp;
	macline *l;
	extern char *strcpy();

	for(;;) {
		if (NULL == macExp) {	/* not in a macro */
			fromMac = ' ';
			return(NULL);
		}
		if (NULL != (l = macExp->curr)) {
			fromMac = '+';
			macExp->curr =  l->next;
			return(strcpy(buf, l->line));
		}

		if ((tmp = macExp) == trueMac) { /* popping macro */
			freeList(macExp->parms);

			while(logic->expno == trueMac->expno) {
				if ((logic->type == INWDEF) ||
				   (logic->type == INPWDEF))
					yyerror("End of macro building .while");
				/* A \fB.macro\fR ended while reading in a
				 * \fB.while\fR loop. */
				freeLevel();
			}

			do {
				trueMac = trueMac->prev;
			} while((NULL != trueMac) &&
			        (trueMac->type != MACTYPE));
		}

		macExp = macExp->prev;		/* pop a level */
		free((char *)tmp);
	}
}

/*
 * Save a line in a macro.
 */
saveLine()
{
	macline *newl;

	newl = (macline *)alloc((unsigned)(sizeof(*newl) + strlen(lastL)));
	strcpy(newl->line, lastL);
	if (NULL == inMacDef->first)
		lastDef = inMacDef->first = newl;
	else {
		lastDef->next = newl;
		lastDef = newl;
	}
}

/*
 * Get a line off a file.
 */
static char *
getLine()
{
	register char *line;

	++statement;		/* increment statement number */
	if (NULL != (line = nextMac()))
		return(line);

	while(NULL == (fgets(line = buf, sizeof(buf), inpc->fp))) {
		if (NULL != inpc->prev) {
			inpctl *tmp;

			fclose(inpc->fp);
			inpc = (tmp = inpc)->prev;
			free((char *)tmp);
		}
		else {	/* end of pass */
			for(;NORMAL != logic->type; freeLevel()) {
				switch(logic->type) {
				case INMACDEF:
				case PMACDEF:
					yyerror("Missing .endm");
				 /* Input ended leaving \fB.macro\fR open. */
					break;
				case INIF0:
				case INIF1:
				case INIFX:
					yyerror("Missing .endi");
				 /* Input ended leaving \fB.if\fR open. */
					break;
				case INWDEF:
				case INPWDEF:
					yyerror("Missing .endw");
				 /* Input ended leaving \fB.while\fR open. */
				}
			}
			if (2 == pass) {
				cleanUp();

				switch (errCt) {
				case 0:
					exit(0);
				case 1:
					fprintf(errdev, "1 Error\n");
					break;
				default:
					fprintf(errdev, "%d Errors\n", errCt);
				}
				unlink(outName);
				exit(1);
			}
			else {	/* avoid close & open */
				rewind(inpc->fp);
				inpc->lineNo = 0;
				statement = 1;

				newPass(outName);

				/* reset compiler switches to initial state */
				alignon = alignonX;
				lswitch = lswitchX;
				nswitch = nswitchX;
				wswitch = wswitchX;
				fswitch = fswitchX;
				bswitch = bswitchX;

				dodefs();
				outLine((char *)NULL, 0);
				continue;
			}
		}
	}
	inpc->lineNo++;
	return(line);
}

/*
 * In OPONLY3 we found an op process it.
 * separated to keep from crashing Coherent compiler in cc2.
 */
static unsigned short
procOp()
{
	register opc *op;

	*tp = '\0';
	bp--;
	if (-1 == (opIx = opLookUp(token)))
		return(OPONLY4);

	if (INIF1 <= ctype)
		fatal("Logic error in macro def '%s' %d", token, ctype); /* TECH */

	op = prefTab + opIx;
	switch(op->kind) {	/* switch on function type */	
	case S_MACRO:
		if (ctype < INIF0)
			newLevel(PMACDEF);
		break;
	case S_ENDM:
		switch(ctype) {
		case INMACDEF:
			inMacDef = (macro *)NULL;
			lastDef = (macline *)NULL;
			freeLevel();
			return(INITIAL);
		case PMACDEF:
			freeLevel();
		}
		break;
	case S_WHILE:
		switch(ctype) {
		case INWDEF:
		case INPWDEF:
			newLevel(INPWDEF);
		}
		break;
	case S_ENDW:
		switch(ctype) {
		case INWDEF: {
			macctl *tmp;
				saveLine();
			logic = (tmp = logic)->prev;
			tmp->type  = WHILETYPE;
			tmp->curr = tmp->first = inMacDef->first;
			tmp->parms = tmp->names = NULL;
			tmp->prev = macExp;
			macExp = tmp;
			free((char *)inMacDef);
			lastDef  = (macline *)NULL;
			inMacDef = (macro *)NULL;
			return(INITIAL);
		}
		case INPWDEF:
			freeLevel();
			break;
		}
		break;
	case S_IF:
		switch(ctype) {
		case INIF0:
		case INIFX:
			newLevel(INIFX);
		}
		break;
	case S_ELSE:
		switch(ctype) {
		case INIF0:
			logic->type = INIF1;
			return(INITIAL);
		}
		break;
	case S_ENDI:
		switch(ctype) {
		case INIF0:
		case INIFX:
			freeLevel();
		}
		break;
	}
	return(OPONLY4);
}

/*
 * Read line and start processing.
 * separated from yylex() to keep from crashing compiler in cc2.
 */
static unsigned short
startLine()
{
	register short c;

	do {	/* GNU outputs # line numbers */
		if (';' != lastChar) {
			outLine(lastL, fromMac);
			bp = lastL = getLine();
		}
		if (COMMA != lastToken)
			freel();	/* trash free space from last line */
	} while ('#' == *bp);

	ctype = logic->type;

	if (('#' != *bp) &&
	   (';' != lastChar) &&
	   (defCt || (NULL != trueMac)) &&
	   ((ctype > INPWDEF) || (ctype < INWDEF))) {
		bp = lastL = expand(bp, 0);

		if ((lastL != buf) && (fromMac != '+')) {
			outLine(buf, fromMac);
			fromMac = '@';
		}
	}

	if ('#' == *bp)	/* don't lex noexpand */
		bp++;

	if (COMMA == lastToken)	/* continuation line ? */
		return(psw);

	lastChar = c = *bp;	/* get a char */
	if (ctype < INIF1) {
		if ((' ' == c) || ('\t' == c))
			return(OPONLY1);
		else if (isalpha(c))
			return(OPONLY2);
		else
			return(OPONLY4);
	}
	if ((' ' == c) || ('\t' == c)) {
		pos = OPCODE;
		return(WHITE);
	}
	if (isalpha(c)) {
		pos = LABEL;
		tp = token;
		*tp++ = c;
		return(INSYM);
	}
	switch(c) {
	case '/': case '\n': case 0: case ';':
		break;
	default:
		invChar(c, "");
	}
	return(INITIAL);
}

/*
 * Got an opcode, set it up.
 */
static
doOp(opIx)
{
	register opc *op;

	lflags = 0;
	stype = typTab + (kind = (op = prefTab + opIx)->kind);
	yylval.o = op;
	pos = OPERAND;
	/* op code determines operand syntax */
	switch(ytype = stype->type) {
	case NCMD:
		RSTATE(ytype, ENDLINE)
	case OP:
		if (stype->bldr & REP_INSTR)
			pos = OPCODE;
	case ICMD:
	case DATA:
		RSTATE(ytype, WHITE)
	case ECMD:
	case ECMDX:
	case CMD:
		if (S_EQUS == kind) {
			tp = token;
			scnt = dcnt = 0;
			RSTATE(ytype, DSTATE)
		}
		RSTATE(ytype, CSTATE)
	case DCMD:
		RSTATE(ytype, CMSTATE)
	default:
		fatal("Optype %d in lex", ytype); /* TECH */
	}
}

/*
 * get tokens
 */
yylex()
{
    register short c;

    for(;;) {
    	/*
	 * Allow Japaneese comments and strings, catch other non ascii
	 * and abort the line.
	 */
 	if ((state != INITIAL) &&
	   !isascii(lastChar = c = *bp++) &&
	   (state != INSTR) &&
	   (state != INCHAR)) {
		invChar(c, "");
		RSTATE(NL, INITIAL)
	}

	switch(state) {
	case INITIAL:		/* start of line */
		bcnt = pcnt = 0;
		psw = startLine();
		lastChar = c = *bp++;
		if (!isascii(c)) {
			invChar(c, "");
			RSTATE(NL, INITIAL)
		}
		NEWS(psw);
		
	case OPONLY1:	/* find start of opcode */
		switch(c) {
		case ' ':
		case '\t':
			continue;
		default:
			if (!islower(c))
				NEWS(OPONLY4)
			tp = token;
			*tp++ = c;
			NEWS(OPONLY3)
		}

	case OPONLY2:			/* bypass label */
		if (isalnum(c))
			continue;
		--bp;
		if ((' ' == c) || ('\t' == c))
			NEWS(OPONLY1)
		NEWS(OPONLY4)
		
	case OPONLY3:
		if (isalnum(c)) {	/* accumulate opcode */
			*tp++ = c;
			continue;
		}
		NEWS(procOp());

	case OPONLY4:	/* default processing */
		if (logic->type < INIF0)
			saveLine();
		NEWS(INITIAL)

	case INSYM:
		if (isalnum(c)) {
			*tp++ = c;
			continue;
		}
		*tp = '\0';
		--bp;
		if (pos == OPERAND) {
			sym *sp;

			sp = symLookUp(token, S_UNDEF, 0L, 0);
			switch(sp->type) {
			case IDENTIFIER:
			case REG:
			case LEN:
				yylval.s = sp;
				break;
			case NUMBER:
				yylval.val = sp->loc;
			}
			RSTATE(sp->type, WHITE)
		}

		/* pos != OPERAND may be opcode or label */
		if (':' != c) {
			if (-1 != (opIx = opLookUp(token)))
				return(doOp(opIx));

			if (NULL != (macFound = macLookUp(token, MACTYPE))) {
				static opc smac = { /* fake opcode */
					0, S_MAC
				};

				yylval.o = &smac; /* macro expansion */
				RSTATE(CMD, CSTATE)
			}

			if (!strcmp(token, ".")) {
				yylval.t = token;
				pos = OPCODE;
				RSTATE(TOKEN, WHITE)
			}
		}

		if ((pos == LABEL) || (':' == c)) {
			yylval.t = token;
			pos = OPCODE;
			RSTATE(TOKEN, WHITE)
		}

		yyerror("Invalid opcode: '%s'", token);
		/* The string in the opcode position is
		 * not one of our opcodes or one of
		 * your macros. */
		RSTATE(NL, INITIAL)

	case DSTATE:	/* eat whole line for define */
		switch(c) {
		case '"':
			if (dcnt)
				dcnt = 0;
			else if (!scnt)
				dcnt = 1;
			break;
		case '\'':
			if (scnt)
				scnt = 0;
			else if (!dcnt)
				scnt = 1;
			break;
		case '\\':
			*tp++ = c;
			psw = state;
			NEWS(BSLP)
		case '/':
			if (scnt | dcnt)
				break;
		case 0:		/* let # define take semicolon */
		case '\n':
			*tp = '\0';
			yylval.t = trim(token);
			RSTATE(TOKEN, ENDLINE)
		}
		*tp++ = c;
		continue;

	case CSTATE: /* pick up parms on macro expansion */
		state = psw = INTOKEN;
		*(tp = token) = '\0';
		scnt = dcnt = pcnt = bcnt = 0;
		vctr = 1;
		switch(c) {
		case 0:
		case ';':
		case '\n':
		case '/':
			if (COMMA == lastToken)
				NEWS(INITIAL)
			RSTATE(NL, INITIAL)
		case ',':
			switch(lastToken) {
			case CMD:
			case COMMA:
				bp--;
				yylval.t = token;
				RSTATE(TOKEN, CSTATE)
			}
			RSTATE(COMMA, CSTATE)
		}

	case INTOKEN:	/* pick up parms on macro expansion */
		switch(c) {
		case '(':
			pcnt += vctr; break;
		case ')':
			if (pcnt)
				pcnt -= vctr;
			break;
		case '[':
			bcnt += vctr; break;
		case ']':
			if (bcnt)
				bcnt -= vctr;
			break;
		case '"':
			if (dcnt)
				dcnt = 0;
			else if (!scnt)
				dcnt = 1;
			vctr = (dcnt | scnt) ^ 1;
			break;
		case '\'':
			if (scnt)
				scnt = 0;
			else if (!dcnt)
				scnt = 1;
			vctr = (dcnt | scnt) ^ 1;
			break;
		case '\\':
			*tp++ = c;
			psw = state;
			NEWS(BSLP)

		case ';':
		case '/':
			if (!vctr)
				break;
		case 0:
		case '\n':
			if (pcnt | bcnt | scnt | dcnt) {
				yyerror("Unmatched bracket in parmeter");
		/* Line ended leaving an open bracket or parenthesis. */
				RSTATE(NL, INITIAL)
			}
			--bp;
			*tp = '\0';
			if (*(yylval.t = trim(token)))
				RSTATE(TOKEN, CSTATE)
			NEWS(CSTATE)
		case ',':
			if (pcnt | bcnt | scnt | dcnt)
				break;
			--bp;
			*tp = '\0';
			yylval.t = trim(token);
			switch(ytype) {
			case ECMD:
			case ECMDX:
				RSTATE(TOKEN, WHITE)
			}
			RSTATE(TOKEN, CSTATE)
		}
		*tp++ = c;
		continue;

	case BSLP:	/* back slash while reading parm */
		switch(c) {
		case 0:
		case '\n':
			--bp;
			yyerror("End of line after backslash reading parm");
		/* Macro parmeters may not be broken up with backslash. */
			break;
		}
		*tp++ = c;
		NEWS(psw)

	case CMSTATE: /* pick up names on macro definition */
		psw = state = INNAME;
		tp = token;
		switch(c) {
		case ',':
			RET(COMMA);
		case 0:
		case ';':
		case '\n':
		case '/':
			if (COMMA == lastToken)
				NEWS(INITIAL)
			RSTATE(NL, INITIAL)
		}
	case INNAME:	/* pick up parms on macro definition */
		switch(c) {
		case 0:
		case ';':
		case '\n':
		case '/':
		case ',':
			--bp;
			*tp = '\0';
			if (*(yylval.t = trim(token)))
				RSTATE(TOKEN, CMSTATE)
			NEWS(CMSTATE)
		default:
			if (ispunct(c)) {
				invChar(c, "in macro name");
				RSTATE(NL, INITIAL)
			}
		}
		*tp++ = c;
		continue;


	case WHITE:
		/*
		 * This piece of code is heavily trafficked.
		 * Force the fastest form of switch statement
		 * by covering many cases.
		 */
		switch(c) {
		case '1':	case '2':	case '3':	case '4':
		case '5':	case '6':	case '7':	case '8':
		case '9':	case '0':
			backOff = bp - 1;
			if (number = c - '0')
				NEWS(DNUM)
			NEWS(LZERO);

		/* % allowed as first char only */
		case 'a':	case 'b':	case 'c':	case 'd':
		case 'e':	case 'f':	case 'g':	case 'h':
		case 'i':	case 'j':	case 'k':	case 'l':
		case 'm':	case 'n':	case 'o':	case 'p':
		case 'q':	case 'r':	case 's':	case 't':
		case 'u':	case 'v':	case 'w':	case 'x':
		case 'y':	case 'z':
		case 'A':	case 'B':	case 'C':	case 'D':
		case 'E':	case 'F':	case 'G':	case 'H':
		case 'I':	case 'J':	case 'K':	case 'L':
		case 'M':	case 'N':	case 'O':	case 'P':
		case 'Q':	case 'R':	case 'S':	case 'T':
		case 'U':	case 'V':	case 'W':	case 'X':
		case 'Y':	case 'Z':	case '?':	case '_':
		case '%':	case '.':
			tp = token;
			*tp++ = c;
			NEWS(INSYM)

		case ' ':
		case '\t':
			continue;
		case 0:
		case ';':
		case '\n':
		case '/':
			if (COMMA == lastToken)
				NEWS(INITIAL)
			RSTATE(NL, INITIAL)
		case '<':
			NEWS(LTS)
		case '>':
			NEWS(GTS)
		case '!':
			NEWS(BANGS)
		case '=':
			NEWS(EQUS)
		case '+':
			RET(PLUS);
		case '-':
			RET(MINUS);
		case '*':
			RET(TIMES);
		case ':':
			if (OPERAND == pos)
				RET(COLON);
			pos = OPCODE;
			RSTATE(NL, WHITE)
	
		case '&':
			RET(AND);
		case '|':
			RET(OR);
		case '^':
			RET(XOR);
		case '#':
			RET(P_SIGN);
		case '$':
			RET(D_SIGN);
		case '~':
			RET(NOT);
		case ',':
			psw = WHITE;
			RET(COMMA);
		/*
		 * This assembler may reverse the normal usage of
		 * brackets and parentheses. That is expressions
		 * are put in []. 5(%eax) is 5 past %eax.
		 * bswitch flips this back. Keeps counts to improve
		 * the syntax error message.
		 */
		case '[':
			bcnt++;
			RET(bswitch ? LBRACK : LPAREN);
		case ']':
			bcnt--;
			RET(bswitch ? RBRACK : RPAREN);
		case '(':
			pcnt++;
			RET(bswitch ? LPAREN : LBRACK);
		case ')':
			pcnt--;
			RET(bswitch ? RPAREN : RBRACK);
		case '@':
			RET(AT);
		case '"':
			psw = WHITE;
			tp = token;
			NEWS(INSTR)
		case '\'':
			psw = WHITE;
			tp = token;
			NEWS(INCHAR)
		default:
			invChar(c, "as token start");
			RSTATE(NL, INITIAL)
		}

	case ONUM:
		if (isdigit(c)) {
			if (c > '7')
				yyerror("Error in octal number"); /**/
			number *= 8;
			number += c - '0';
			continue;
		}
		bp--;
		yylval.val = number;
		RSTATE(NUMBER, WHITE)

	case BNUM:
		if (isdigit(c)) {
			if (c > '1')
				yyerror("Error in binary number"); /**/
			number *= 2;
			number += c - '0';
			continue;
		}
		bp--;
		yylval.val = number;
		RSTATE(NUMBER, WHITE)

	case XNUM:
		if (isxdigit(c)) {
			number *= 16;
			if (isdigit(c))
				number += c - '0';
			else if (islower(c))
				number += c - ('a' - 10);
			else
				number += c - ('A' - 10);
			continue;
		}
		bp--;
		yylval.val = number;
		RSTATE(NUMBER, WHITE)
		
	case DNUM:
		if (isdigit(c)) {
			number *= 10;
			number += c - '0';
			continue;
		}
		if (('.' == c) || ('e' == c) || ('E' == c)) {
			yylval.dbl = strtod(backOff, &bp);
			RSTATE(FNUM, WHITE)
		}
		bp--;
		yylval.val = number;
		RSTATE(NUMBER, WHITE)

	case LZERO:
		switch(c) {
		case '.':
			yylval.dbl = strtod(backOff, &bp);
			RSTATE(FNUM, WHITE)
		case 'x':
		case 'X':
			NEWS(XNUM)
		case 'b':
		case 'B':
			NEWS(BNUM)
		default:
			bp--;
			if (isdigit(c))
				NEWS(ONUM)
			yylval.val = number;
			RSTATE(NUMBER, WHITE)
		}

	case LTS:	/* < seen */
		switch(c) {
		case '<':
			RSTATE(LSHIFT, WHITE)
		case '=':
			yylval.val = (LT|EQ);
			break;
		default:
			--bp;
			yylval.val = LT;
		}
		RSTATE(COMPARISON, WHITE)		

	case GTS:	/* > seen */
		switch(c) {
		case '=':
			yylval.val = (GT|EQ);
			break;
		case '>':
			RSTATE(RSHIFT, WHITE)
		default:
			--bp;
			yylval.val = GT;
		}
		RSTATE(COMPARISON, WHITE)

	case EQUS:	/* = seen take = or == */
		if (OPCODE == pos) {
			--bp;
			opIx = opLookUp(".equ");
			return(doOp(opIx));
		}
		if ('=' != c)	
			--bp;
		yylval.val = EQ;
		RSTATE(COMPARISON, WHITE)

	case BANGS:	/* ! seen */
		if ('=' == c) {
			yylval.val = (GT|LT);
			RSTATE(COMPARISON, WHITE)
		}
		--bp;
		RSTATE(BANG, WHITE)

	case INSTR:
	case INCHAR:
		switch(c) {
		case 0:
		case '\n':
			--bp;
			if (state == INSTR) {
				yyerror("End of line detected in string"); /**/
				*tp = '\0';
				yylval.t = token;
				RSTATE(TOKEN, psw)
			}
			yyerror("End of line detected in character constant");
			/**/
		case '\'':
			if (state != INCHAR)
				break;
			*tp = '\0';
			if (strlen(token) != 1)
				yyerror("Character constant %d long",
					strlen(token));
				/* Character constants must be one byte long. */
			yylval.val = token[0];
			RSTATE(NUMBER, psw)
		case '"':
			if (state != INSTR)
				break;
			*tp = '\0';
			yylval.t = token;
			RSTATE(TOKEN, psw)
		case '\\':
			pstate = state;
			NEWS(BSL)
		}
		*tp++ = c;
		continue;

	case BSL:	/* back slash */
		switch(c) {
		case 0:
		case '\n':
			--bp;
			yyerror("End of line after backslash");
			/**/
			break;
		case 'n':
			*tp++ = '\n'; break;
		case 'r':
			*tp++ = '\r'; break;
		case 't':
			*tp++ = '\t'; break;
		case 'b':
			*tp++ = '\b'; break;
		case 'f':
			*tp++ = '\f'; break;
		default:
			if ((c >= '0') && (c <= '7')) {
				number = 0;
				bp--;
				NEWS(BSLDIG)
			}
			*tp++ = c;
		}
		NEWS(pstate)

	case BSLDIG:	/* accumulate octal number */
		if ((c >= '0') && (c <= '7')) {
			number *= 8;
			number += c - '0';
			continue;
		}
		if (number > 255) {
			yyerror("Octal number %ld truncated to char", number);
			/* An octal number in a string was too big. */
			number &= 255;
		}
		if (!number) {
			yyerror("Cannot insert \\0 in string");
			/* NUL (\e0) terminates strings.
			 * Instead of
			 * .DM
			 *	.byte	"hello\en\e0"
			 * .DE
			 * use:
			 * .DM
			 *	.byte	"hello\en", 0
			 * .DE */
		}
		*tp++ = number;
		bp--;
		NEWS(pstate)

	case ENDLINE:
		RSTATE(NL, INITIAL)
	}
    }
}
