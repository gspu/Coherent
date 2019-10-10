/*
 * Parts of diff that depend on the minimal common
 * subsequence algorithm of Hirschberg.
 */

#include "diff.h"
#include <sys/mdata.h>

#define	NSBRK	512		/* Size of each sbrk -- allocator */
#define BADSBRK ((char *)-1)		/* fail return from sbrk */
	/* Marks last of each hash-equivalence class */
#define	LAST	((unsigned)MININT)
				/* Assume MININT is a single bit */

static	char	partial[] = "diff: partial line omitted from %s\n";
static	char	jackpot[] = "Jackpot %d %d\n";

/*
 * Tables for the table-driven CRC16 algorithm.
 * This is used as the hash of the input lines
 * and should be relatively uniform statistically.
 */
static	unsigned short crctab1[] = {
	0000000,	0140301,	0140601,	0000500,
	0141401,	0001700,	0001200,	0141101,
	0143001,	0003300,	0003600,	0143501,
	0002400,	0142701,	0142201,	0002100
};

static	unsigned short crctab2[] = {
	0000000,	0146001,	0154001,	0012000,
	0170001,	0036000,	0024000,	0162001,
	0120001,	0066000,	0074000,	0132001,
	0050000,	0116001,	0104001,	0043000
};

typedef	struct	LINES {
	unsigned	l_num;
	unsigned short	l_hash;
}	LINES;

/*
 * A K-candidate entry
 */
typedef	struct	CAND {
	unsigned	a;
	unsigned	b;
	struct CAND	*prev;
}	CAND;

int	compar();
unsigned short inhash();
CAND	*candidate();

char	line1[LSIZE];		/* First file's input line buffer */
char	line2[LSIZE];
char	nomem[] = "Out of memory";

int	*allrp;
int	*allep;

/*
 * Called to invoke heuristic version of
 * diff.
 */
diffh(args)
char **args;
{
#ifdef COHERENT
	execv("/usr/lib/diffh", args);
#endif
#ifdef MSDOS
#define	MAXTAIL	128		/* Maximum command tail length */
	char tail[MAXTAIL];

	tail[0] = '\0';
	while (*++args != NULL) {
		if (strlen(tail) + strlen(*args) + 2 > MAXTAIL)
			cerr("command tail too long");
		if (tail[0] != '\0')
			strcat(tail, " ");
		strcat(tail, *args);
	}
	exit(execall("diffh", tail));
#endif
	cerr("-h doesn't work");
}

/*
 * Routine is given two file streams and
 * it produces the minimal list of
 * differences by the Hirschberg algorithm
 */
diff(fp1, fp2)
FILE *fp1, *fp2;
{
	register unsigned short hash;
	register unsigned ln1;
	register unsigned ln2;
	register LINES *V;
	register unsigned *P;
	register unsigned *E;
	register unsigned *K;
	register unsigned *J;

	/*
	 * Calculate the hash tables for
	 * the second file
	 * and sort with hash as primary key,
	 * line-number secondary.
	 */
	V = (LINES *)alloc(sizeof (LINES));
	V->l_num = V->l_hash = 0;	/* unneeded? */
	ln2 = 0;
	while ((hash = inhash(fp2, fn2)) != 0) {
		register LINES *Vp;

		Vp = (LINES *)alloc(sizeof (LINES));
		Vp->l_num = ++ln2;
		Vp->l_hash = hash;
	}
	qsort(V+1, ln2, sizeof (LINES), compar);
	/*
	 * Read first file, building
	 * a table per line of
	 * pointers to the first elements
	 * of the hash-equivalent list in file2.
	 */
	P = (unsigned *)alloc(sizeof (unsigned));
	for (ln1=0; (hash = inhash(fp1, fn1))!=0; ln1++) {
		register unsigned *Pp;
		register unsigned lo, mid, hi;

		Pp = (unsigned *)alloc(sizeof(unsigned));
		lo = 1;
		hi = ln2;
		while (lo <= hi) {
			mid = (lo+hi)/2;
			if (hash <= V[mid].l_hash)
				hi = mid-1; 
			else
				lo = mid+1;
		}
		if (hi!=0 && V[hi].l_hash==V[hi+1].l_hash)
			cerr("fatal search botch");
		if (hash == V[++hi].l_hash)
			*Pp = hi; else
			*Pp = 0;
	}

	/*
	 * Throw away the hash values.
	 * Mark the last line of each hash-equivalent
	 * class of lines and fake line 0.
	 */
	{
		register unsigned *Ep, *eEp;
		register LINES *Vp;

		E = V;
		E[0] = LAST;
		for (Ep=E+1, Vp=V+1, eEp=&E[ln2]; Ep <= eEp; Vp++, Ep++) {
			*Ep = Vp->l_num;
			if (Ep==eEp || Vp->l_hash!=(Vp+1)->l_hash)
				*Ep |= LAST;
		}
	}
	{
		register int i;
		register unsigned *Pp;

		Pp = &E[ln2+3];
		for (i=1; i<=ln1; i++)
			Pp[i] = P[i];
		P = Pp;
		ralloc(&Pp[ln1+1]);
	}
	/*
	 * Set up to build list of K-candidates for
	 * all k.  The K table overwrites P (when
	 * we add K[k+2] to the table we no longer
	 * need P[k].
	 */
	{
		register int k;
		register int i;
		register CAND *cp;

		K = &E[ln2+1];
		K[0] = candidate(0, 0, NULL);
		K[1] = candidate(ln1+1, ln2+1, NULL);
		k = 0;
		for (i=1; i<=ln1; i++)
			if (P[i] != 0)
				k = merge(K, k, i, E, P[i]);

		/*
		 * `k' is the length of the maximal
		 * common subsequence, thus K[k] points
		 * to the list of lines in the subsequnce.
		 * Store the list in `J' in a handier
		 * format.  E, K (all but K[k]) and P
		 * are redundant so J overlays them.
		 */
		cp = K[k];
		J = E;
		for (i=0; i<=ln1; i++)
			J[i] = 0;
		J[ln1+1] = ln2+1;	/* fence */
		while (cp != NULL) {
			J[cp->a] = cp->b;
			cp = cp->prev;
		}
	}
	{
		register unsigned i, j, k, l;

		/*
		 * Matching lines are now stored as pairs
		 * (i, J[i]), J[i]!=0.  Produce output
		 * scripts and look for jackpots.
		 */
		rewind(fp1);
		rewind(fp2);
		fgets(line1, LSIZE, fp1);
		fgets(line2, LSIZE, fp2);
		for (i=j=1; i<=ln1 || j<=ln2; ) {
			if (j == J[i]) {
				if (!(*equal)(line1, line2)) {
					/*
					 * This can be avoided at
					 * the expense of another
					 * pass.  So we settle for this
					 * potentially non-minimal result.
					 * Possibly this printout could
					 * disappear.
					 */
					fprintf(stderr, jackpot, i, j);
					change(i, i, j, j);
					text1(line1);
					prsep();
					text2(line2);
					prend();
				} else
					text(line1);
				i++;
				fgets(line1, LSIZE, fp1);
				j++;
				fgets(line2, LSIZE, fp2);
				continue;
			}
			for (k=i; J[k]==0; k++)
				;
			l = J[k];
			if (i == k)
				append(i, j, l-1);
			else if (j == l)
				delete(i, k-1, j);
			else
				change(i, k-1, j, l-1);
			while (i != k) {
				text1(line1);
				i++;
				fgets(line1, LSIZE, fp1);
			}
			prsep();
			while (j != l) {
				text2(line2);
				j++;
				fgets(line2, LSIZE, fp2);
			}
			prend();
		}
	}
}

/*
 * Read input characters and hash for each
 * line.  Return the hash value computed
 * using CRC-16 methods.  A zero value
 * means EOF or error and thus will not
 * be returned as a hash value.
 */
unsigned short
inhash(fp, fn)
register FILE *fp;
char *fn;
{
	register int c;
	register int tmp;
	register int short hash;
	register int space;

	space = hash = 0;
	while ((c = getc(fp)) != '\n') {
		if (c == EOF) {
			if (hash != 0)
				fprintf(stderr, partial, fn);
			return (0);
		}
		if (bflag && (c==' ' || c=='\t')) {
			space++;
			continue;
		}
	compute:
		tmp = c^hash;
		hash = (hash>>8) ^ crctab1[tmp&017] ^ crctab2[(tmp&0360)>>4];
		if (space) {
			c = ' ';
			space = 0;
			goto compute;
		}
	}
	if (hash == 0)
		hash++;
	return (hash);
}

/*
 * Sort comparison routine.
 * Hash value is primary key,
 * line number secondary.
 */
compar(a, b)
register LINES *a, *b;
{
	if (a->l_hash < b->l_hash)
		return (-1);
	if (a->l_hash > b->l_hash)
		return (1);
	if (a->l_num < b->l_num)
		return (-1);
	if (a->l_num > b->l_num)
		return (1);
	return (0);
}

/*
 * Build an entry in the K-candidates
 * table
 */
CAND *
candidate(a, b, prev)
unsigned a;
unsigned b;
CAND *prev;
{
	register CAND *v;

	v = (CAND *)alloc(sizeof(CAND));
	v->a = a;
	v->b = b;
	v->prev = prev;
	return (v);
}

/*
 * The merge step, called for
 * each index in file 1.
 * `k' is index of last filled element
 * of `K', `i' is current index in
 * file 1, and `p' is first element
 * of class of lines in file 2
 * equivalent to line `i' by hash value.
 */
merge(K, k, i, E, p)
CAND **K;
unsigned k, i;
unsigned *E;
unsigned p;
{
	register unsigned r;
	register CAND *c;

	r = 0;
	c = K[0];
	do {
		register unsigned lo, mid, hi;
		register unsigned j;

		j = E[p]&~LAST;
		lo = r;
		hi = k;
		if (K[lo]->b >= j)
			continue;
		while (lo <= hi) {
			mid = (lo+hi)/2;
			if (j <= K[mid]->b)
				hi = mid-1; else
				lo = mid+1;
		}
		if (K[hi]->b<j && j<=K[hi+1]->b) {
			if (j < K[hi+1]->b) {
#if SIMUL
				register CAND *prev;
				prev = K[hi];
#endif
				K[r] = c;
				r = hi+1;
#if SIMUL
				c = candidate(i, j, prev);
#else
				c = candidate(i, j, K[hi]);
#endif
			}
			if (hi == k) {
				K[k+2] = K[k+1];
				k++;
				break;
			}
		}
	} while (!(E[p++] & LAST));
	K[r] = c;
	return (k);
}

/*
 * This allocator simply does sbrk calls so that
 * it can grow (easily) the space as it needs without
 * threading it.  It also checks and prints and error
 * if out of space.
 */
char *
alloc(size)
register unsigned size;
{
	register int *cp;

	if ((size = (size+sizeof(int)-1)/sizeof(int)) == 0)
		cerr(nomem);
	if (allrp==NULL || allrp+size>=allep) {
		if ((allep = (int *)sbrk(NSBRK)) == BADSBRK)
			cerr(nomem);
		if (allrp == NULL)
			allrp = allep;
		allep += NSBRK/sizeof(int);
	}
	cp = allrp;
	allrp += size;
	return (cp);
}

/*
 * Like the brk system call, reset
 * current end of memory to `ep'.
 */
ralloc(ep)
register int *ep;
{
	allrp = ep;
}
