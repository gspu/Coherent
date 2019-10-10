/*
 * Symbol table management.
 */
#ifdef	vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * Lookup a global (or static external)
 * identifier in the symbol table. Return a pointer
 * to the symbol structure. If it is not there add
 * it. This table exists only to save space and to make
 * the TREE nodes all the same size.
 */
SYM *
gidpool(id)
char	*id;
{
	register SYM	*sp;
	register char	*p;
	register int	c;
	register int	h;

	h = 0;
	p = id;
	while ((c = *p++) != '\0')
		h += c;
	h &= SHMASK;
	sp = hash1[h];
	while (sp != NULL) {
		if (strcmp(id, sp->s_id) == 0)
			return (sp);
		sp = sp->s_fp;
	}
	if ((sp=(SYM *)malloc(sizeof(SYM)+strlen(id)+1)) == NULL)
		cnomem("gidpool");
	sp->s_fp = hash1[h];
	hash1[h] = sp;
	strcpy(sp->s_id, id);
	return (sp);
}

#if OVERLAID
/*
 * Free all symbols.
 */
freegsym()
{
	register SYM	*sp1, *sp2;
	register int	i;

	for (i=0; i<NSHASH; ++i) {
		sp1 = hash1[i];
		while (sp1 != NULL) {
			sp2 = sp1->s_fp;
			free((char *) sp1);
			sp1 = sp2;
		}
		hash1[i] = NULL;
	}
}
#endif
