/*
 * 80386 assembler commands.
 * ytype	function	args
 * CMD		docmd		parms
 * DCMD		docmd		names
 * ECMD		ecmd		name, expr
 * NCMD		docmd
 * ICMD		ncmd		expr
 * DATA		dcmd		data list
 */
#include <asm.h>
#include <symtab.h>

static unsigned short ct;

/*
 * Check parm count. Print msg if one required.
 */
static
ckCount(no)
{
	if(no == ct)
		return(0);
	yyerror("Found %d parms expected %d", ct, no); /**/
	return(1);
}

/*
 * Set up while
 */
static void
setUpWhile()
{
	register macro *tmp;
	macline *newl;

	newl = (macline *)alloc((unsigned)(sizeof(*newl) + strlen(lastL)));
	strcpy(newl->line, lastL);

	newLevel(INWDEF);

	tmp = new(macro);
	inMacDef = tmp;
	lastDef = inMacDef->first = newl;
}

/*
 * Do commands with name, expr.
 */
ecmd(label, op, p, item)
opc *op;
parm *label, *p;
data *item;
{
	long n = 0;

	labelIgnored(label);

	if (NULL != item) {
		switch (item->type) {
		case 'l':
			n = item->d.l;
			break;
		case 'y':
			if (op->kind == S_SET)
				break;
		default:
			yyerror("Invalid operand type"); /**/
			return;
		}
	}

	switch (op->kind) {
	case S_SEGMENT:	/* change segments */
		segment(op, p, n);
		break;

	case S_COMM:	/* allocate data in a segment */
		comm(op, p, n);
		break;

	case S_SET:
		if(NULL == p)
			yyerror("Name required");
			/* The format of \fBset\fR is
			 * \fB.set name, value\fR */
		else if (!strcmp(p->str, "."))
			doOrg(NULL, item);
		else if (item->type == 'l')
			symLookUp(p->str, S_XSYM, n, 0);
		else
			symLookUp(p->str, S_LOCAL, 
				  item->d.y->loc, item->d.y->sg);
		break;

	}
	return 0;
}

/*
 * Do commands with string parms.
 */
docmd(label, op, p)
opc *op;
parm *p, *label;
{
	ct = countList(p);

	switch(op->kind) {
	/*
	 * This may be name value in some formats to avoid syntax error
	 * we bring it in as ytype CMD. We don't do dubug for those formats.
	 */
	case S_DEF:
		labelIgnored(label);
		coffDef(p);
		break;

	case S_TAG:
		labelIgnored(label);
		coffTag(p);
		break;

	case S_FILE:
		labelIgnored(label);
		coffFile(p);		
		break;

	case S_ENDF:
		coffEndef();
		break;

	case S_INCLUDE:
		if(ckCount(1))
			return(1);
		fileOpen(p->str);
		break;

	case S_MACRO:
		if(NULL == label) {
			yyerror("Macro definition must have a label"); /**/
			defMac("***", p, MACTYPE);
		}
		else
			defMac(label->str, p, MACTYPE);
		break;

	case S_EQUS:
		defCt++;
		if(NULL == label) {
			yyerror(".define must have a label"); /**/
			defMac("***", p, MACSTR);
		}
		else
			defMac(label->str, p, MACSTR);
		break;

	case S_UNDEFINE:	/* kill macro or define */
		for(; NULL != p; p = p->next) {
			opDelete(p->str);
			macDelete(p->str, MACSTR);
			macDelete(p->str, MACTYPE);
		}
		break;

	case S_MEXIT: {
		macctl *m;

		if(NULL == trueMac) {
			yyerror(".mexit not in macro"); /**/
			break;
		}
		m = macExp;	/* break out of any whiles */
		do {
			m->curr = NULL;	/* no more lines */
		} while(m->type != MACTYPE);
		break;
		}

	case S_ENDM:
		yyerror("Unexpected .endm ignored"); /**/
		break;

	case S_MAC:
		{
		register macctl *tmp;

		umList(p);	/* dont trash parms at end of line */
		tmp = new(macctl);
		tmp->curr = macFound->first;
		tmp->parms = p;
		tmp->names = macFound->names;
		tmp->expno = ++macNo;	/* unique non zero macro expansion ct */
		tmp->type  = MACTYPE;
		tmp->prev = macExp;
		trueMac = macExp = tmp;
		if(NULL != label)
			symLookUp(label->str, S_LOCAL, dot.loc, dot.sg);
		}
		break;

	case S_ERRATA:
		nswitch = op->code;
		break;

	case S_WARNINGS:
		wswitch = op->code;
		break;

	case S_OPORDER:
		fswitch = op->code;
		break;

	case S_BORDER:
		bswitch = op->code;
		break;

	case S_LMODE:
		longMode = op->code;
		break;

	case S_LIST:
		if (pass == 2)
			lswitch = op->code & lswitchX;
		break;

	case S_ALIGNON:
		alignon = op->code;
		break;

	case S_EJECT:
		if (lswitch && (2 == pass) && pswitch)
			while (linect) {
				sTitle();
				putchar('\n');
			}
		break;

	case S_PAGE:
		pswitch = op->code;
		break;

	case S_MLIST:
		if (pass == 2) {
			if(!strcmp("on", p->str))
				mlistsw = 1;
			else if(!strcmp("off", p->str))
				mlistsw = 0;
			else
				yyerror(
				"Invalid .mlist option must be on or off");
				/**/
		}
		break;

	case S_ERROR:
		if (op->code)
			yyerror("%s", p->str); /* NODOC */
		else
			yywarn("%s", p->str); /* NODOC */
		break;

	case S_CMNT:	/* ident and version */
		if (2 == pass)
			cmnt(op, p);
		break;

	case S_SECTION:
		if (!ct) {
			yyerror(".section must have name");	/**/
			return;
		}
		section(p->str);
		break;

	case S_TITLE: {
		static short i = 0;

		/* Ignore all but first title on pass 0 */
		if((2 == pass) || !i) {
			pswitch = i = 1;
			free(title);
			title = scpy(p->str, 0);
		}
		break;
	}

	case S_ENDW:
		if((NULL != macExp) && (WHILETYPE == macExp->type))
			macExp->curr = macExp->first;
		else
			yyerror("Unexpected .endw"); /**/
		break;

	case S_ENDI:
		switch(logic->type) {
		case INIF1:
			freeLevel();
			break;
		case NORMAL:
			yyerror("Unexpected .endi statement"); /**/
			break;
		default:
			fatal(".endi detected logic type %d", logic->type);
			/* TECH */
		}
		break;

	case S_ELSE:
		switch(logic->type) {
		case INIF1:
			logic->type = INIF0;
			break;
		case NORMAL:
			yyerror("Unexpected .else statement"); /**/
			break;
		default:
			fatal(".else detected logic type %d", logic->type);
			/* TECH */
		}
		break;

	default:
		kindErr((unsigned short)op->kind);
	}

	return(0);
}

/*
 * Commands with a numexp as parm.
 */
ncmd(label, op, item)
parm *label;
opc *op;
data *item;
{
	long n;
	data oper;

	switch (item->type) {
	case 'l':
		n = item->d.l;
		break;
	case 'y':
		if ((op->kind == S_EQU) || (op->kind == S_VAL))
			break;
	default:
		yyerror("Invalid operand type"); /* NODOC */
		return;
	}

	switch(op->kind) {
	case S_TYPE:
		coffType(n);
		break;		
	case S_VAL:
		coffVal(item);
		break;
	case S_LN:
		coffLn(n);
		break;
	case S_LINE:
		coffLine(n);
		break;
	case S_SIZE:
		coffSize(n);
		break;
	case S_SCL:
		coffScl(n);
		break;
	case S_ZERO:
		buildlab(label);
		while(n--)
			outab(0);
		break;

	case S_BLKB:
		buildlab(label);
		oper.d.l = dot.loc + n;
		oper.type = 'l';
		doOrg(NULL, &oper);
		return;
	case S_SHIFT:
		doShift((short)n);
		break;
	case S_EQU:
		if (NULL == label)
			yyerror("Label required"); /**/
		else if (!strcmp(label->str, "."))
			doOrg(NULL, item);
		else if (item->type == 'l')
			symLookUp(label->str, S_XSYM, n, 0);
		else {
			symLookUp(label->str, S_LOCAL, 
				  item->d.y->loc, item->d.y->sg);

			/* forward equate make sure there is an extra pass */
			if (!pass && (item->d.y->flag & S_UNDEF))
				xpass = 1;

			if (item->d.y->flag & (S_COMMON|S_EXREF))
			  yyerror("Cannot equate to externally defined symbol");
			  /* This would mean creating another symbol.
			   * Try using \fB.define\fR. */
		}
		return;
	case S_PLEN:	/* set page length */
		nlpp = n - 10;
		break;
	case S_LLEN:	/* set line length */
		lineSize = n;
		break;
	case S_IF:
		newLevel(n ? INIF1 : INIF0);
		break;
	case S_WHILE:
		if((NULL != macExp) &&
		   (WHILETYPE == macExp->type) &&
		   (macExp->curr == macExp->first->next)) {
			if(!n) {
				freeList((parm *)macExp->first);
				macExp->curr = NULL;
			}
		}
		else
			setUpWhile();
		break;
	default:
		kindErr((unsigned short)op->kind);
	}

	labelIgnored(label);
}
/*
 * Make an expression for dataOut.
 */
expr *
makeExp(addr)
data *addr;
{
	static expr x;

	x.ref = addr->d.y;
	x.exp = x.ref->loc;
	if (x.ref->flag & (S_COMMON|S_EXREF))
		x.exp <<= 1;	/* double we will loose it back */
	return(&x);
}

/*
 * Data output for .byte .word .long .string .float .double
 */
static void
dataOut(op, oper)
opc *op;
data *oper;
{
	register char *str, c;
	union {
		long l[2];
		float f[2];
		double d;
	} u;

	if(op->code && (op->code != 5) && ('s' == oper->type)) {
		yyerror("String must be on .byte");
		/* For example:
		 * .DM
		 * 	.byte "This is how we place a string", 0
		 * .DE */
		return;
	}
	while(oper->count--) {	/* honor repeat count */
		switch(op->code) {	/* set by option */
		case 5:			/* .string */
			if ('s' != oper->type) {
				yyerror("Improper .string operand");
				return;
			}
			for(str = oper->d.s; c = *str++; )
				outab((unsigned short)c);
			outab(0);
			break;
		case 0:			/* .byte */
			switch(oper->type) {
			case 's':
				for(str = oper->d.s; c = *str++; )
					outab((unsigned short)c);
				break;
			case 'y':
				outrb(makeExp(oper), 0);
				break;
			case 'l':
				outab((unsigned)oper->d.l);
				break;
			case 'd':
				outab((unsigned)oper->d.d);
				break;
			}
			break;
		case 1: 	/* .word or .value */
			switch(oper->type) {
			case 'y':
				outrw(makeExp(oper), 0);
				break;
			case 'l':
				outaw((unsigned)oper->d.l);
				break;
			case 'd':
				outaw((unsigned)oper->d.d);
				break;
			}
			break;
		case 2:		/* .long */
			switch(oper->type) {
			case 'y':
				outrl(makeExp(oper), 0);
				break;
			case 'l':
				outal(oper->d.l);
				break;
			case 'd':
				outal((long)oper->d.d);
				break;
			}
			break;
		case 3:		/* .float */
			switch(oper->type) {
			case 'y':
				yyerror("Symbol may not be float");
				/* You may not convert a symbol to
				 * a floating-point value. */
				return;
			case 'l':
				u.f[0] = oper->d.l;
				break;
			case 'd':
				u.f[0] = oper->d.d;
			}
#if _DECVAX
			ieee_f(u.f, u.f);	/* DECVAX to IEEE */
#endif
			outal(u.l[0]);
			break;
		case 4:		/* .double */
			switch(oper->type) {
			case 'y':
				yyerror("Symbol may not be double");
				/* You may not convert a symbol to
				 * a floating-point value. */
				return;
			case 'l':
				u.d = oper->d.l;
				break;
			case 'd':
				u.d = oper->d.d;
			}
#if _DECVAX
			ieee_d(&u.d, &u.d);	/* DECVAX to IEEE */
#endif
			outal(u.l[0]);
			outal(u.l[1]);

			break;
		}
	}
}

/*
 * Output alignment bytes.
 */
static int
alignOut(seg, byte, n)
{
	int rv = 0;

	switch (seg) {
	case 0:	/* fill selected by user */
		while (dot.loc & n) {
			rv = 1;
			outab(byte);
		}
		break;
	case 1:	/* text */
		while (dot.loc & n) {
			rv = 1;
			outab(0x90);
		}
		break;
	case 2:	/* data */
		while (dot.loc & n) {
			rv = 1;
			outab(0);
		}
		break;
	}
	return rv;
}

/*
 * Commands with a list of data as parm.
 */
dcmd(label, op, oper)
parm *label;
opc *op;
data *oper;
{
	register sym *sp = NULL;
	register sym *y;
	long start, n;
	char b, s;

	switch (op->kind) {
	case S_ORG:
		return(doOrg(label, oper));

	case S_EVEN:	/* this is here to fall into .align */
		s = dot.sg;
		n = 1;
		goto even;

	case S_ALIGN:
		if (NULL == oper) {
			yyerror("Missing operand");
			/* \fB\&.align\fR must have 1 or 2 operands */
			return(1);
		}
		switch (oper->type) {
		case 'l':
			n = oper->d.l;
			break;
		default:
			yyerror("Invalid operand type"); /* NODOC */
			return(1);
		}
		if ((3 != (s = dot.sg)) && (NULL != (oper = oper->next))) {
			switch (oper->type) {
			case 'l':
				s = 0;
				b = oper->d.l;
				break;
			default:
				yyerror("Invalid operand type"); /* NODOC */
				return(1);
			}
		}

		if (!--n)
			return;
		if ((1 != n) && (3 != n)) {
			yyerror(".align must be 1, 2 or 4");
		/* \fB\&.align\fR must work after the link.
		 * These are the only values for which this can be true. */
			return(1);
		}

	even:	if (3 == dot.sg) {
			oper.d.l = (dot.loc + n) & ~n;
			oper.type = 'l';
			doOrg(NULL, &oper);
		}
		else
			alignOut(s, b, n);
		return(0);

	case S_DATA:
		/* If alignment on and data and not .string or .byte */
		if (!alignon)
			break;
		switch (op->code) {
		case 1:
			s = alignOut(dot.sg, 0, 1);
			break;
		case 2:
		case 3:
		case 4:
			s = alignOut(dot.sg, 0, 3);
		}
		if (s)	/* Alignment stuff went out */
			outLine("", ' ');
	}

	if(NULL != label) {
		switch (op->kind) {
		case S_DATA:
		case S_UDATA:
			sp = symLookUp(label->str, S_LOCAL,
				(start = dot.loc), dot.sg);
			break;
		default:
			labelIgnored(label);
		}
	}

	for(b = 0; NULL != oper; oper = oper->next) {
		switch (op->kind) {
		case S_DATA:
		case S_UDATA:
			dataOut(op, oper);
			continue;
		case S_DIM:
			if ('l' != oper->type) {
				yyerror("Invalid data type, must be number");
					 /**/
				continue;
			}
			coffDim(oper->d.l, b++);
			continue;
		case S_GLOBL:	/* .globl */
			if('y' != oper->type) {
				yyerror("Invalid data type, must be symbol");
					 /**/
				continue;
			}
			y = oper->d.y;
			if(!(y->flag & S_ASYM)) {
				yyerror("Illegal use of of predefined symbol %s.",
					(psym *)y->name); /**/
				continue;
			}
			if (NULL != strchr(y->name, ';')) {
				yyerror("Illegal use of local symbol"); /**/
				continue;
			}
			if (y->flag & S_UNDEF) {
				y->loc = 0;
				y->sg = 0;
				y->flag = S_EXREF;
			}
			else
				y->flag |= S_EXDEF;
			continue;
			default:
				kindErr((unsigned short)op->kind);
				return(1);
		}
	}

	if(NULL != sp) /* finish label building */
		sp->size = dot.loc - start;
	return(0);
}
