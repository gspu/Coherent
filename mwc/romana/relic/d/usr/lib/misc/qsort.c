/*
 * C general utilities library.
 * qsort()
 * ANSI 4.10.5.2.
 * Quicksort.
 * Algorithm from C. A. R. Hoare, cf. Knuth Vol. 3, pp. 114 ff.
 *
 * This qsort is specially designed to react to conditions that make
 * the normal qsort run slowly. These include repative keys and
 * strange cases like data split into two sorted sets.
 */
#include <stdio.h>
#ifndef TESTLIB
#define CHAR_BIT 8	/* bits in a char */
typedef char Void;
typedef unsigned short size_t;

#define ORD(h, l) ((*compar)((Void *)(h), (Void *)(l)) >= 0)
#define INDEX(p) (((p) - (char *)base) / size)
#define BASE(d) (base + ((d) * size))

/* Experimentaly obtained values, qsort() may be tuned for special cases */
#define M  6	/* if (nmemb < M) shellsort() */
#define FC 3	/* if ((larger >> FC) > smaller) badPivot++ */
#define BC 4	/* if (badPivot >= BC) shellsort() */

extern void memxch();

void
qsort(base, nmemb, size, compar)
Void *base;
size_t nmemb, size;
int (*compar)();
{
	register char *bot, *top;
	int badPivot = 1;
	size_t n;
	struct rLevel {
		Void *b;
		size_t n;
		int sw;
	} *sp, stack[CHAR_BIT * sizeof(size_t)];

	/* check validity of args */
	if (!(n = size * nmemb) || (nmemb != n / size))
		return;

	stack[0].b = NULL;	/* set stopper to avoid 8086 pointer compares */
	for (sp = stack + 1;	/* set stack above recursion stopper */
	    NULL != base;	/* until we pop to stopper */
	    (nmemb = (--sp)->n), (base = sp->b), badPivot = sp->sw) { /* pop */
		/* qsort normally, shellsort for small sets or
		   if qsort is doing badly */
		for (;(nmemb >= M) && (badPivot < BC); sp++) {
			/* Use middle element as pivot move to *base normally.
			 * If there is order the middle is a good guess
			 * for the pivot, if there is no order any guess is
			 * about the same. However sometimes the middle is a
			 * very bad pick then we try something else. Sometimes
			 * all pivots are bad, for example a set of 1000 items
			 * with only two keys, we then shift to shellsort.
			 *
			 * Knuth suggests a random pivot, this misses the
			 * problem of having all pivots bad, and the great
			 * benefits of center pivot for ordered sets. Also
			 * getting random numbers is a lot of overhead.
			 */
			memxch(base, BASE(nmemb >> badPivot), size);

			/* Partition the set so the bottom has items <=
			 * pivot and the top has items >= the pivot. Then
			 * use qsort to sort the bottom and top.
			 */
			bot = (char *)base;
			top = BASE(nmemb);
			for (;;) {
				for (;ORD(top -= size, base) && top > base;)
					;
				for (;ORD(base, bot += size) && top > bot;)
					;
				if (bot < top)
					memxch(bot, top, size);
				else
					break;
			}

			/* Put pivot into its correct place. */
			memxch(top, (char *)base, size);

			/* Recusively sort the partitions pushing stack
			 * for larger and tail recursion for smaller.
			 * This means we cut the list at least in half
			 * for each push. Thus the depth can be no
			 * larger than the number of bits in size_t
			 * minus log2(M). To get here top < bot.
			 */
			if ((n = INDEX(top)) < (nmemb -= INDEX(bot))) {
				sp->b = (Void *)bot;
				sp->n = nmemb;
				nmemb = n;
			} else {
				sp->b = base;
				sp->n = n;
				base = (Void *)bot;
			}
			/* test for very bad cuts */
			if ((sp->n >> FC) > nmemb)
				++badPivot;
			else
				badPivot = 1;	/* good pivot */
			sp->sw = badPivot;
		}
		{	/* shellsort */
			unsigned gap, i;

			for (n = gap = size * nmemb;;) {
				if (nmemb & 1)
					gap -= size;
				if (!(nmemb >>= 1))
					break;
				for (i = gap >>= 1; i < n; i += size) {
					register j;
					
					for (j = i - gap; j >= 0; j -= gap) {
 						bot = base + j;
						if (ORD(bot + gap, bot))
							break;					
						memxch(bot, bot + gap, size);
					}
				}
			}
		}
	}
}
#endif
#ifdef TEST
#include <sys/times.h>
#include <sys/const.h>
static double total;

#define SIZE 6000

struct item {
	int key;
	int data;
} *Array, *stuff, *offside;

static char boundMsg[]  = "Comparison out of array bounds.\n";
static char display[]	= "%24s %6.2f\n";
static char showError[]	= "Error at %d\n";

int
compare0(p1, p2)
struct item *p1, *p2;
{
	if ((p1->data == -1) || (p2->data == -1))
		printf(boundMsg);
	return (p1->key - p2->key);
}

int
compare1(p1, p2)
struct item *p1, *p2;
{
	if ((p1->data == -1) || (p2->data == -1))
		printf(boundMsg);
	return (p1->data - p2->data);
}

int
compare2(p1, p2)
struct item *p1, *p2;
{
	if ((p1->data == -1) || (p2->data == -1))
		printf(boundMsg);
	return (p2->data - p1->data);
}

/*
 * call qsort and do timings.
 */
static
callSort(msg, compare)
char *msg;
int (*compare)();
{
	struct tbuffer start, stop;
	double end;

	Array->data = offside->data = -1;

	times(&start);
	qsort(stuff, SIZE, sizeof(struct item), compare);
	times(&stop);

	if (-1 != offside->data)
		printf("Sort passed limits.\n");
	if (-1 != Array->data)
		printf("Sort ate previous variable.\n");

	total += end = (double)(stop.tb_utime - start.tb_utime) / HZ;
	printf(display, msg, end);
}

main()
{
	register int i, j;

	total = 0.0;

	if (NULL == (Array = malloc(sizeof(*Array) * (SIZE + 2)))) {
		printf("Out of space");
		exit(1);
	}
	stuff = Array + 1; /* markers for out of bounds compares */
	offside = Array + SIZE + 1;

	/* sort data with lots of dups */
	for (i = 0; i < SIZE; i++)
		stuff[i].data = (rand() % 10);

	callSort("data = rand() % 10", compare2);
	for (i = 0; i < (SIZE - 1); i++)
		if (stuff[i].data < stuff[i+1].data)
			printf(showError, i);

	/* sort data with lots of dups */
	for (i = 0; i < SIZE; i++)
		stuff[i].data = (rand() % 100);

	callSort("data = rand() % 100", compare2);

	for (i = 0; i < (SIZE - 1); i++)
		if (stuff[i].data < stuff[i+1].data)
			printf(showError, i);

	/* sort random data */
	for (srand(i = 0); i < SIZE; i++) {
		stuff[i].key = rand();
		stuff[i].data = i;
	}

	callSort("Random data", compare0);

	for (i = 0; i < (SIZE - 1); i++)
		if (stuff[i].key > stuff[i+1].key)
			printf(showError, i);

	/* sort it back so we can be very sure */
	callSort("Random data", compare1);

	for (i = 0; i < SIZE; i++)
		if (stuff[i].data != i)
			printf(showError, i);

	/* sort data that is already sorted */
	callSort("Presorted data", compare1);
	for (i = 0; i < SIZE; i++)
		if (stuff[i].data != i)
			printf(showError, i);

	/* sort backward data */
	callSort("Backward data", compare2);

	for (i = 0; i < SIZE; i++)
		if (stuff[i].data != ((SIZE - 1) - i))
			printf(showError, i);

	/* sort data that is almost already sorted */
	for (i = 0; i < SIZE; i += 10)
		stuff[i].data = rand();

	callSort("Nearly sorted data", compare2);

	for (i = 0; i < (SIZE - 1); i++)
		if (stuff[i].data < stuff[i+1].data)
			printf(showError, i);

	/* really bad case */
	for (j = i = 0; i < SIZE; i += 2)
		stuff[j++].data = i;
	for (i = 1; i < SIZE; i += 2)
		stuff[j++].data = i;

	callSort("Merging backward decks", compare2);

	for (i = 0; i < SIZE; i++)
		if (stuff[i].data != ((SIZE - 1) - i))
			printf(showError, i);

	/* The worst case so far */
	for (j = i = 0; i < SIZE; i += 2)
		stuff[j++].data = i;
	for (i = 1; i < SIZE; i += 2)
		stuff[j++].data = i;

	callSort("Merging decks", compare1);

	for (i = 0; i < SIZE; i++)
		if (stuff[i].data != i)
			printf(showError, i);
	printf(display, "Total", total);
}
#endif
