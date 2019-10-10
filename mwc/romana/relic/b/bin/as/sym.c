/*
 * Symbol table functions.
 */
#include <asm.h>
#include <symtab.h>
#include <y_tab.h>

#define SHASH	((unsigned short)1021)	/* symbol table hash */
sym 	**symhash;			/* macros equs and symbols */
static	char	*lastSym;		/* last symbol defined */

/*
 * This routine is called early in the
 * game to set up the hashtable.
 */
void
symInit()
{
	register psym  *sp;
	register unsigned short ht;

	symhash = (sym **)alloc(SHASH * sizeof(sp));

	/* init symbol table */
	for(sp = symtab;  sp < symtab  + SYMCOUNT; sp++) {
		if (rswitch && ('%' == sp->name[0]) && sp->name[1])
			sp->name++;
		sp->next = symhash[ht = hash(sp->name) % SHASH];
		symhash[ht] = (sym *)sp;
	}

	lastSym = "";	/* last symbol defined is null */
}

/*
 * Free macro space.
 */
freeMac(mp)
register macro *mp;
{
	freeList((parm *)mp->first);	/* free lines */
	freeList(mp->names);		/* free parms */
	free((char *)mp);		/* free macro */
}

/*
 * Returns a 1 if this symbol should not be output
 * to the symbol table.
 */
static
notSym(sp)
register sym *sp;
{
	/* don't out put symbols given numbers already */
	if (sp->num)
		return (1);

	/* don't output unused .globls */
	if ((sp->flag & S_EXREF) && !(sp->flag & S_USED))
		return (1);

	if (!(sp->flag & (S_EXREF|S_EXDEF))) {
		register char *p;

		/* don't output non global symbols if -x option */
		if (xswitch)
			return (1);

		p = SYMNAME(sp);

		/* don't output symbols starting .L is -X option */
		if (Xswitch && (p[0] == '.') && (p[1] == 'L'))
			return (1);

		/* don't output local symbols */
		if (NULL != strchr(p, ';'))
			return (1);
	}

	/* don't output dot symbol */
	if (&dot == (psym *)sp)
		return (1);

	return (0);
}

/*
 * Give numbers to all identifier symbols.
 * Mark undefined symbols as exref if -g.
 * Delete symbols defined by macro, equs and equ.
 * Called at the end of pass 0 and 1.
 */
unsigned short
symGlob(number)
{
	register sym *sp, **psp;
	register unsigned short i;

	for(i = 0; i < SHASH; i++) {
		for(psp = symhash + i; NULL != (sp = *psp); ) {
			switch(sp->type) {
			case MACTYPE: /* delete macro */
			case MACSTR:  /* delete define */
				*psp = sp->next;	/* rechain list */
				freeMac((macro *)sp);
				continue;

			case NUMBER: /* delete equ */
				if ((2 == pass) && (sp->flag & S_EXDEF))
					sp->num = number++;
				sp->type = IDENTIFIER;
				sp->flag = S_UNDEF;
				break;

			case IDENTIFIER:
				if((sp->flag & S_UNDEF) && gswitch)
					sp->flag = (S_EXREF | S_USED);

				if((2 == pass) && !notSym(sp))
					sp->num = number++;
			}
			psp = &(sp->next);	/* follow list */
		}
	}
	lastSym = "";
	return(number);
}

/*
 * Dump symbol data
 */
void
symDump(output, limit)
int (*output)();
long limit;
{
	register sym *sp;
	register i;

	for(i = 0; i < SHASH; i++)
		for(sp = symhash[i]; NULL != sp; sp = sp->next) {
			if(sp->num > limit) {
				switch(sp->type) {
				case NUMBER:
				case IDENTIFIER:
					(*output)(sp);
				}
			}
		}
}

/*
 * This is called when debug records are created. It repoints
 * the symbol number to one of the debug records. symDump then
 * uses this information to avoid dumping the symbol twice.
 * This means debug data can seriously screw up an output file
 * if it is wrong.
 */
symReNumber(id, number)
char *id;
int number;
{
	short i;
	register sym *sp;

	for(sp = symhash[i = hash(id) % SHASH];
	    sp != NULL;
	    sp = sp->next) {
		if(!strcmp(id, SYMNAME(sp))) {
			sp->num = number;
			break;
		}
	}
}

/*
 * Undefined symbol message.
 */
static void
unDefMsg(id)
char *id;
{
	char *pt;

	if(NULL != (pt = strchr(id, ';'))) {
		*pt = '\0';
		yyerror("Undefined symbol '?%s'", id);
		/* NODOC */
		*pt = ';';
	}
	else
		yyerror("Undefined symbol '%s'", id);
		/* A symbol was used without defining it or using
		 * a \fB-g\fR option.
		 * You must define local symbols. */
}

/*
 * Redefined symbol message.
 */
static void
redef(id)
char *id;
{
	yyerror("Redefinition of '%s'", id);
	/* An assembler internal symbol is being redefined. */
}

/*
 * Duplicate symbol message.
 */
static void
dupsym(id)
register char *id;
{
	register char *pt;

	if(NULL != (pt = strchr(id, ';'))) {
		*pt = '\0';
		yyerror("Duplicate symbol '?%s'", id);
		/* NODOC */
		*pt = ';';
	}
	else
		yyerror("Duplicate symbol '%s'", id);
	/* \fIsymbol\fR is defined on two different lines. */
}

/*
 * Lookup the name `id' in a hashtable.
 * If it is not found build it.
 */
sym *
symLookUp(id, flag, loc, sg)
long loc;
char *id;
{
	register sym *sp;
	char *locSym;
	short i;

	if('?' == id[0] && id[1]) {	/* local symbol */
		locSym = galloc((unsigned)(strlen(id) + strlen(lastSym) + 1));
		sprintf(locSym, "%s;%s", id + 1, lastSym);
		id = locSym;
	}
	else
		locSym = NULL;

	for(sp = symhash[i = hash(id) % SHASH];
	    sp != NULL;
	    sp = sp->next) {
		if((sp->type > MACSCAN) && !strcmp(id, SYMNAME(sp))) {
			if(2 == pass) {
				switch(flag) {
				case S_UNDEF: /* we are looking for it */
					if((S_UNDEF & sp->flag) &&
					   (DEFINED != lastToken) &&
					   (S_GLOBL != kind))
						unDefMsg(id);
					if (sp->flag & S_ASYM)
						sp->flag |= S_USED;
					return(sp);

				case S_LOCAL: /* being defined here */
					if(!(sp->flag & S_ASYM))
						redef(id);

					else if(sp->flag & S_EXREF) {
						sp->flag &= ~S_EXREF;
						sp->flag |= S_EXDEF|S_LOCAL;
					}
					else if(statement != sp->statement)
						dupsym(id);
					else if((sp->loc != loc) ||
						(sp->sg != sg))
						yyerror("Phase error '%s'",
							id);
		/* A symbol is defined one way in one phase of the assembly
		 * and another way in the next phase. */
					break;

				case S_XSYM:  /*  a number */
					if(!(sp->flag & S_ASYM)) {
						redef(id);
						return(sp);
					}
					if (sp->flag & S_LOCAL)
						dupsym(id);
					if (sp->flag & S_EXREF) {
						sp->flag &= ~S_EXREF;
						sp->flag |= S_EXDEF;
					}
					sp->flag &= ~(S_UNDEF|S_LOCAL);
					sp->flag |= S_XSYM;
					sp->type = NUMBER;
					sp->loc  = loc;
					sp->sg   = -1;	/* absolute */
					return(sp);
				}
				break;
			}
			else {	/* pass 0 and 1 */
				switch(flag) {
				case S_UNDEF:	/* looking for it */
					if (sp->flag & S_ASYM)
						sp->flag |= S_USED;
					return(sp);

				case S_LOCAL:	/* being defined here */
					if(!(sp->flag & S_ASYM))
						return(sp);

					sp->statement = statement;

					if(sp->flag & S_EXREF) {
						sp->flag &= ~S_EXREF;
						sp->flag |= S_EXDEF|S_LOCAL;
					}
					else {
						sp->flag &= ~S_UNDEF;
						sp->flag |= flag;
					}
					break;

				case S_XSYM:  /* a number */
					if(!(sp->flag & S_ASYM))
						return(sp);
					if (sp->flag & S_EXREF) {
						sp->flag &= ~S_EXREF;
						sp->flag |= S_EXDEF;
					}
					sp->flag &= ~(S_UNDEF|S_LOCAL);
					sp->flag |= S_XSYM;
					sp->type = NUMBER;
					sp->loc  = loc;
					sp->sg   = -1;	/* absolute */
					return(sp);
				}
				break;
			}
		}
	}

	if(NULL == sp) 	{
		if (DEFINED == lastToken) {
			static psym sy = {
				NULL, IDENTIFIER, 0, 0, 0, S_UNDEF,
				NULL, 0, 0, NULL
			};

			return (&sy);
		}
		sp = (sym *) scpy(id, offset(sym, name));
		sp->next = symhash[i];
		sp->ref = symhash[i] = sp;
		sp->flag = flag;
	}
	sp->type = IDENTIFIER;
	switch(flag) {
	case S_UNDEF:
		if (S_GLOBL != kind)
			sp->flag |= S_USED;
		break;
	case S_XSYM:
		sp->type = NUMBER;
		sp->loc = loc;
		sp->sg   = -1;	/* absolute */
		break;
	case S_LOCAL:
		sp->loc = loc;
		sp->sg = sg;
		sp->statement = statement;
		if(NULL == locSym)
			lastSym = sp->name;
	}
	return(sp);
}

/*
 * Lookup a name in opcodes.
 * return it's index or -1.
 */
short
opLookUp(id)
char *id;
{
	register nhash *op;
	short i, l;

	for (l = strlen(id), i = hash(id) % OPCOUNT;;) {
		if ((l == (op = hashCodes + i)->nlen) &&
		   !memcmp(id, (charLump + op->nameIx), l)) {
			choices = op->count;
			return(op->prefIx);
		}
		if (-1 == (i = op->next))
			return (-1);
	}
}

/*
 * Remove an entry from the opcode table.
 */
void
opDelete(id)
char *id;
{
	register nhash *op;
	short i, l;

	l = strlen(id);
	for(i = hash(id) % OPCOUNT; -1 != i; i = op->next) {
		if((l == (op = hashCodes + i)->nlen) &&
		   !memcmp(id, (charLump + op->nameIx), l)) {
			op->nlen = 0;
			return;
		}
	}
	return;
}

/*
 * Look up macros and equs symbols. Shares table with symbols.
 */
macro *
macLookUp(id, type)
char *id;
{
	register macro *mp;

	for(mp = (macro *)symhash[hash(id) % SHASH];
	    mp != NULL;
	    mp = mp->next)
		if((mp->type == type) && !strcmp(id, mp->name))
			return(mp);
	return(NULL);
}

/*
 * Delete a macro or equs.
 */
macDelete(s, t)
char *s;
short t;
{
	register macro *mp, **pmp;
	short ht;

	for(pmp = (macro **)(symhash + (ht = hash(s) % SHASH));
	    NULL != (mp = *pmp);
	    pmp = &(mp->next)) {
		if((mp->type == t) && !strcmp(s, mp->name)) {
			*pmp = mp->next;	/* unChain */
			freeMac(mp);
			break;
		}
	}
	return(ht);
}

/*
 * Define macro or equs symbol.
 * Shares symbol table with other symbols.
 */
void
defMac(s, p, t)
char *s;
parm *p;
short t;
{
	register macro *tmp;
	short ht;

	ht = macDelete(s, t);	/* remove old form */
	tmp = (macro *)scpy(s, offset(macro, name));

	tmp->names = p;	/* parm names */
	umList(p);

	tmp->next = (macro *)symhash[ht];
	symhash[ht] = (sym *)tmp;
	if(MACTYPE == (tmp->type = t)) {
		newLevel(INMACDEF);
		inMacDef = tmp;
	}
}
