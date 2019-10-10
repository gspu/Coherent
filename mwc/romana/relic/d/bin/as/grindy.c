/*
 * The routines in this file
 * completely dominate the execution time
 * of the assembler. If you write tuned
 * versions of them in assembly code, the
 * performance of the assembler will be
 * much better.
 */
#include "asm.h"

/*
 * Read in an identifier.
 * Pack the name, null filled, into
 * the buffer `id'. The character `c'
 * is the first character of the name;
 * if < 0 then a character must be
 * read.
 */
getid(id, c)
register c;
char *id;
{
	register char *e, *p;

	if (c < 0) {
		do {
			if ((c = *ip) != 0)
				++ip;
		} while (c==' ' || c=='\t');
		if (ctype[c] >= 0)	/* Not LETTER */
			qerr("invalid identifier");
	}
	p = id;
	e = p + NCPLN;
	do {
		if (p < e)
			*p++ = c;
		if ((c = *ip) != 0)
			++ip;
	} while (ctype[c] <= 0);	/* LETTER or DIGIT */
	if (c != 0)
		--ip;
	while (p < e)
		*p++ = 0;
}

/*
 * Read a line from the standard I/O
 * stream `sfp' into the buffer `ib' and
 * return true if a line was obtained.
 */
getline()
{
	register char *p;
	register c;

	p = ib;
	while ((c=getc(sfp))!=EOF && c!='\n')
		if (p < &ib[NINPUT-1])
			*p++ = c;
	*p = '\0';
	return (c != EOF);
}

/*
 * Lookup the name `id' in the hashtable.
 * If it is not found either return a
 * `NULL' (`f' is false) or a
 * pointer to a newly created hash table
 * entry (`f' is true).
 */
struct sym *
lookup(id, f)
char *id;
{
	register struct sym *sp;
	int ht;

	{
		register char *p1;
		register nc;

		ht = 0;
		nc = NCPLN;
		p1 = id;
		do {
			ht += *p1++;
		} while (--nc);
	}
	if ((sp = symhash[ht&HMASK]) != NULL) {
		do {
			if (sp->s_total==ht && symeq(id, sp->s_id))
				return (sp);
		} while ((sp = sp->s_sp) != NULL);
	}
	if (f == 0)
		return (NULL);
	sp = (struct sym *) new(sizeof(struct sym));
	sp->s_total = ht;
	ht &= HMASK;
	sp->s_sp = symhash[ht];
	symhash[ht] = sp;
	sp->s_kind = S_NEW;
	sp->s_type = E_ACON;
	sp->s_flag = S_SYMT;
	sp->s_addr = 0;
	symcopy(sp->s_id, id);
	return (sp);
}

/*
 * Compare two symbol names.
 */
symeq(p1, p2)
register char *p1, *p2;
{
	register n;

	n = NCPLN;
	do {
		if (*p1++ != *p2++)
			return (0);
	} while (--n);
	return (1);
}
