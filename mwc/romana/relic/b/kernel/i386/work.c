/*
 * File:	work.c
 *
 * Purpose:	Manage temporary pages of virtual memory.
 *		Pages are allocated in virtually contiguous pairs to allow
 *		for straddle operations in target code.
 *
 * $Log:	work.c,v $
 * Revision 1.1  93/04/14  10:29:40  root
 * r75
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define	START_WORK	0xFFFFA		/* Highest scratchpad click pair. */
#define MAX_WORK_PAIRS	4		/* Max # in use at one time */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int	workAlloc();
void	workFree();
void	workPoolInit();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static int	numWorkPairs;	/* Number of click pairs in use. */
static int	maxWorkPairs;	/* For monitoring resource use. */

static int	workPool[MAX_WORK_PAIRS];

/*
 * ----------------------------------------------------------------------
 * Code.
 */
void
workPoolInit()
{
	int i, w;

	for (i = 0, w = START_WORK; i < MAX_WORK_PAIRS; i++, w -= 2)
		workPool[i] = w;
}

/*
 * Allocate a click pair of virtual space for temporary use.
 * Panic if none available.
 * Return value is a click number, e.g. 0xFFFFA, suitable for use as
 * an index into ptable1_v[].  The return value plus one is also
 * available for use as a virtual click number.
 */
int
workAlloc()
{
	int s, ret;

	s = sphi();
	if (numWorkPairs >= MAX_WORK_PAIRS)
		panic("Work pair pool empty");
	ret = workPool[numWorkPairs++];
	spl(s);
#if 0
	if (numWorkPairs > maxWorkPairs) {
		maxWorkPairs = numWorkPairs;
		printf("Now using %d work pairs ", maxWorkPairs);
	}
#endif
	return ret;
}

/*
 * Return a click pair of virtual space to the free pool.
 */
void
workFree(w)
int w;
{
	int s;

	if (w > START_WORK || w <= START_WORK - MAX_WORK_PAIRS)
		panic("workFree(%x)", w);
	s = sphi();
	if (numWorkPairs == 0)
		panic("Work pair pool exploded");
	workPool[--numWorkPairs] = w;
	spl(s);
}
