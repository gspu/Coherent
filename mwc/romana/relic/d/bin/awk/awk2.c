/*
 * AWK - part 2
 * Initialisation, and
 * other compiler support.
 * Symbol table maintenance
 * and lookup.
 */

#include "awk.h"
#include "y.tab.h"

void	keyenter();
void	funcenter();

typedef	struct	FUNC {
	int	(*f_funcp)();
	CHAR	*f_name;
	CHAR	f_minarg;		/* Minimum number of arguments */
	CHAR	f_maxarg;		/* Maximum # args (-1 = variable) */
}	FUNC;

FUNC	functions[] = {
	f_length, "length", 0, 1,
	f_sqrt, "sqrt", 1, 1,
	f_log, "log", 1, 1,
	f_exp, "exp", 1, 1,
	f_int, "int", 1, 1,
	f_substr, "substr", 2, 3,
	f_index, "index", 2, 2,
	f_sprintf, "sprintf", 1, -1,
	f_split, "split", 2, 3,
	f_abs, "abs", 1, 1,
};

typedef	struct	KEYW	{
	int	k_lval;
	CHAR	*k_name;
}	KEYW;

KEYW	keywords[] = {
	BEGIN_, "BEGIN",
	END_, "END",
	PRINT_, "print",
	PRINTF_, "printf",
	IF_, "if",
	ELSE_, "else",
	WHILE_, "while",
	FOR_, "for",
	IN_, "in",
	BREAK_, "break",
	CONTINUE_, "continue",
	NEXT_, "next",
	EXIT_, "exit",
};

/*
 * Initialisation --
 * Turn on buffering for output.
 * Initialse static constant values,
 * install keywords,
 * and install built-in functions.
 */
awkinit()
{
	setbuf(stdin, inbuf);
	setbuf(stdout, outbuf);
	setbuf(stderr, NULL);
	/*
	 * Initialise keywords into
	 * the symbol table.
	 */
	{
		register KEYW *kp;

		for (kp = keywords; kp < endof(keywords); kp++)
			keyenter(kp->k_name, kp->k_lval);
	}
	/*
	 * Install functions into symbol
	 * table.
	 */
	{
		register FUNC *fp;

		for (fp = functions; fp < endof(functions); fp++)
			funcenter(fp->f_name, fp->f_funcp, fp->f_minarg,
			    fp->f_maxarg);
	}
	xzero.t_INT = 0;
	xone.t_INT = 1;
	xone.t_op = xzero.t_op = ATERM;
	xone.t_flag = xzero.t_flag = T_INT|T_NUM;
	xfield0.n_op = AFIELD;
	xfield0.n_O1 = &xzero;
	/*
	 * Set up the built-in variables.
	 */
	NRp = install("NR", (INT)0);
	NFp = install("NF", (INT)0);
	FILENAMEp = lookup("FILENAME");
	sassign(FSp = install("FS", (INT)0), " \t");
	sassign(RSp = install("RS", (INT)0), "\n");
	sassign(OFSp = install("OFS", (INT)0), " ");
	sassign(ORSp = install("ORS", (INT)0), "\n");
	sassign(OFMTp = install("OFMT", (INT)0), "%ld");
}

/*
 * Lookup an identifier - `id' and
 * if it is not found, initialise it
 * with the NULL string.
 * The hash value is the sum of all
 * the characters in the name +
 * the length of the name.
 */
NODE *
lookup(id)
CHAR *id;
{
	register CHAR *ip;
	register TERM *tp;
	register unsigned hash;
	register unsigned nb;

	hash = 0;
	nb = sizeof (CHAR);
	for (ip = id; *ip != '\0'; hash++, nb++)
		hash += *ip++;
	for (tp = symtab[hash%NHASH]; tp != NULL; tp = tp->t_next)
		if (hash==tp->t_hval && tp->t_flag&T_VAR
		    && streq(tp->t_name, id))
			return (tp);
	tp = (TERM *)xalloc(sizeof(NODE) + nb);
	strcpy(tp->t_name, id);
	tp->t_STRING = xalloc(sizeof(CHAR));
	tp->t_STRING[0] = '\0';
	tp->t_op = ATERM;
	tp->t_flag = T_VAR|T_ALLOC;
	tp->t_hval = hash;
	tp->t_ahval = -1;
	tp->t_next = symtab[hash %= NHASH];
	symtab[hash] = tp;
	return ((NODE *)tp);
}

/*
 * Lookup for an array.
 * Arguments are `array' and `index'
 * (both strings).  Hashing  is an
 * extension of that in lookup().
 * New array elements are given value the
 * NULL string.
 */
NODE *
alookup(array, index)
CHAR *array;
CHAR *index;
{
	register CHAR *ip;
	register TERM *tp;
	register unsigned hash;
	register unsigned nba, nbi;
	register unsigned hash2;

	hash = 0;
	nba = nbi = sizeof (CHAR);
	for (ip = array; *ip != '\0'; hash++, nba++)
		hash += *ip++;
	hash2 = hash;
	for (ip = index; *ip != '\0'; hash++, nbi++)
		hash += *ip++;
	for (tp = symtab[hash % NHASH]; tp != NULL; tp = tp->t_next)
		if (hash==tp->t_hval && tp->t_flag&T_ARRAY
		  && streq(tp->t_name, array) && streq(tp->t_name+nba, index))
			return (tp);
	tp = (TERM *)xalloc(sizeof(NODE) + nba + nbi);
	strcpy(tp->t_name, array);
	strcpy(tp->t_name+nba, index);
	tp->t_STRING = xalloc(sizeof(CHAR));
	tp->t_STRING[0] = '\0';
	tp->t_op = ATERM;
	tp->t_flag = T_ARRAY|T_ALLOC;
	tp->t_hval = hash;
	tp->t_ahval = hash2;
	tp->t_next = symtab[hash %= NHASH];
	symtab[hash] = tp;
	return ((NODE *)tp);
}

/*
 * Install a built-in integer
 * name with value `val'.
 */
NODE *
install(id, val)
CHAR *id;
INT val;
{
	register NODE *np;

	np = lookup(id);
	np->t_flag |= T_NUM|T_INT;
	free(np->t_STRING);
	np->t_INT = val;
	return (np);
}

/*
 * Enter a keyword in the symbol table.
 * `lval' is the lexical value of the keyword
 * and the TERM type is `AKEYW'.
 */
void
keyenter(word, lval)
CHAR *word;
int lval;
{
	register CHAR *ip;
	register unsigned hash;
	register TERM *tp;
	register int nb;

	hash = 0;
	nb = sizeof (CHAR);
	for (ip = word; *ip != '\0'; hash++, nb++)
		hash += *ip++;
	tp = (TERM *)xalloc(nb + sizeof(TERM));
	tp->t_hval = hash;
	tp->t_op = AKEYW;
	tp->t_flag = T_ALLOC|T_VAR;
	strcpy(tp->t_name, word);
	tp->t_INT = lval;
	tp->t_next = symtab[hash %= NHASH];
	symtab[hash] = tp;
}

/*
 * Enter a function name into the
 * symbol table.
 * `min' and `max' are the minimum and
 * maximum number of arguments, respectively.
 */
void
funcenter(name, fun, min, max)
CHAR *name;
int (*fun)();
int min, max;
{
	register CHAR *np;
	register unsigned hash;
	register TERM *tp;
	register unsigned nb;

	hash = 0;
	nb = sizeof (CHAR);
	for (np = name; *np != '\0'; hash++, nb++)
		hash += *np++;
	tp = (TERM *)xalloc(nb + sizeof(TERM));
	tp->t_hval = hash;
	tp->t_op = AFUNC;
	tp->t_flag = T_ALLOC|T_VAR;
	strcpy(tp->t_name, name);
	tp->t_FUNC = fun;
	tp->t_MINARG = min;
	tp->t_MAXARG = max;
	tp->t_next = symtab[hash %= NHASH];
	symtab[hash] = tp;
}

/*
 * Assign a new value to a string.
 * It produces a static string.
 */
sassign(np, s)
register NODE *np;
STRING s;
{
	if ((np->t_flag & (T_ALLOC|T_NUM)) == T_ALLOC)
		free(np->t_STRING);
	np->t_flag &= ~(T_ALLOC|T_NUM);
	np->t_STRING = s;
}

/*
 * Assign an INT to a node.
 */
iassign(np, i)
register NODE *np;
INT i;
{
	if ((np->t_flag & (T_ALLOC|T_NUM)) == T_ALLOC)
		free(np->t_STRING);
	np->t_flag &= ~T_ALLOC;
	np->t_flag |= T_INT|T_NUM;
	np->t_INT = i;
}

/*
 * Initialise (or reset) the mapping
 * table for field extraction.  It is
 * set to non-zero for every character
 * that can end a field + '\0'
 * Note that FSMAP is defined over `wordbuf'
 * which is only used lexically and by `sprintf'
 * Therefore this routine must be called if this
 * buffer is disturbed.
 */
fsmapinit(fsp)
register CHAR *fsp;
{
	register CHAR *cp;

	whitesw = !strcmp(fsp, " \t");
	for (cp = FSMAP; cp < &FSMAP[NCSET]; )
		*cp++ = 0;
	while (*fsp != '\0')
		FSMAP[*fsp++] = 1;
}
