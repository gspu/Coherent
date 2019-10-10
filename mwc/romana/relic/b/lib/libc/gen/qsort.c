/*
 * C general utilities library.
 * qsort()
 * ANSI 4.10.5.2.
 * Quicksort.
 * Algorithm from C. A. R. Hoare, cf. Knuth Vol. 3, pp. 114 ff.
 */

/*
 * qsort chooses a pivot element (Knuth recommends random choice),
 * partitions the input into sets less than and greater than the pivot,
 * puts the pivot into the correct slot in the sorted set, and
 * recursively sorts the two subpartitions. For small numbers of
 * elements, a shell sort is more efficient. The obvious recursive
 * implementation of qsort can easily use a lot of stack space.
 * The nonrecursive implementation here is less obvious but more
 * efficient.  It is based on two observations:
 *
 * (1) Using ordinary recursion on the smaller of the subpartitions and
 * tail recursion on the larger reduces recursion nesting to at most
 * log2(nmemb) levels.  This reduces stack usage enormously.
 *
 * (2) Since the maximum number of recursion levels is known by (1), the
 * recursion can be faked with auto arrays of fixed size. This provides
 * additional efficiency because the constant "size" and "compar" args
 * need not be passed recursively.
 *
 * This version of qsort reacts to conditions that make normal qsort run
 * slowly. These include repetative keys and strange cases like data
 * split into two sorted sets. It does this by checking how well pivot
 * choices do. First choose a center pivot, if there is order this is a
 * good guess and leads to a very efficient qsort. If this is a bad pick
 * we try 1/4 of the way in, then 1/8 of the way in. At some point qsort
 * assumes that all pivots are bad, as in a set of 1000 elements all 7
 * and 8. Qsort then reverts to shellsort. This is better than insertion
 * sort on large sets. This qsort has been tuned to the data provided
 * by tryqsort.c in masstest. I expect that other data might produce
 * other tuning values (M, FC, BC) and might even result in other
 * alternative sorts rather than shellsort.
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>

#define inOrder(h, l) ((*compar)((__VOID__ *)(h), (__VOID__ *)(l)) >= 0)
#define ptrToIx(p) (((p) - (char *)base) / size)
#define ixToPtr(d) (base + ((d) * size))

#ifdef EXPERIMENT
int M, FC, BC;
#else
/* Experimentaly obtained values */
#define M  9	/* if (nmemb < M) shellsort() */
#define FC 2	/* if ((larger >> FC) > smaller) badPivot++ */
#define BC 5	/* if (badPivot >= BC) shellsort() */
#endif

extern void _memxchg();

/* Recursion stack frame */
typedef struct STACK {
	__VOID__ *s_base;
	size_t s_nmemb;
	int s_badPivot;
} STACK;

void
qsort(base, nmemb, size, compar)
__VOID__ *base;
size_t nmemb, size;
int (*compar)();
{
	register char *bot, *top;
	register STACK *sp;
	char *end;
	int badPivot;
	size_t n;
	STACK stack[CHAR_BIT * sizeof(size_t)];

	/*
	 * Initialize the stack, base, nmemb and badness.
	 */
	sp = stack;
	sp->s_base = base;
	sp->s_nmemb = nmemb;
	sp->s_badPivot = 1;	/* Lowest badness count */

	while (sp >= stack) {
		/* pop a base and nmemb pair off the stack */
		nmemb = sp->s_nmemb;
		base = sp->s_base;
		badPivot = sp->s_badPivot;
		sp--;

		while ((nmemb >= M) && (badPivot < BC)) {
			/* Select pivot element */
			_memxchg(base, ixToPtr(nmemb >> badPivot), size);

			/* 
			 * Partition set into <= base, == base and >= base.
			 *			     ^            ^
			 *			    top		 bot
			 */
			end = top = ixToPtr(nmemb);
			bot = ixToPtr(0);
			for (;;) {
				while (((top -= size) > base) &&
				       inOrder(top, base))
					;
				while (end > (bot += size) &&
				       inOrder(base, bot))
					;
				if (bot < top)
					_memxchg(bot, top, size);
				else
					break;
			}

			/* Put pivot into its correct place. */
			_memxchg(top, (char *)base, size);

			/*
			 * Recusively sort the partitions pushing stack
			 * for larger, recursion for smaller.
			 */
			sp++;
			if ((n = ptrToIx(top)) < (nmemb -= ptrToIx(bot))) {
				sp->s_base = (__VOID__ *)bot;
				sp->s_nmemb = nmemb;
				nmemb = n;
			} else {
				sp->s_base = base;
				sp->s_nmemb = n;
				base = (__VOID__ *)bot;
			}

			/*
			 * Test for very bad (unequal size) cuts
			 */
			if ((sp->s_nmemb >> FC) > nmemb)
				++badPivot;	/* bad pivot */
			else
				badPivot = 1;	/* good pivot */
			sp->s_badPivot = badPivot;
		}
		/* Use shellsort where qsort isn't working out */
		shellsort(base, nmemb, size, compar);
	}
}
