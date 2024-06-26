/* $Header: /src386/STREAMS/coh.386/RCS/alloc.c,v 2.3 93/08/09 13:35:06 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Coherent.
 * Storage allocator.
 *
 * $Log:	alloc.c,v $
 * Revision 2.3  93/08/09  13:35:06  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  14:28:19  nigel
 * Nigel's R80
 * 
 * Revision 1.4  93/04/14  10:06:13  root
 * r75
 * 
 * Revision 1.2  92/01/06  11:58:31  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:13:25	src
 * Initial revision
 * 
 */

#include <common/ccompat.h>
#include <common/__parith.h>
#include <common/_tricks.h>
#include <kernel/param.h>
#include <sys/debug.h>

#include <sys/coherent.h>
#include <sys/errno.h>
#include <sys/proc.h>

#include <kernel/alloc.h>

/*
 * Alloc definitions. These used to be in <sys/machine.h> for some unknown
 * and unknowable reason. They belong here, so now here they are. Since the
 * person(s) who wrote this stuff neglected to mention what the alignment
 * issues are, we'll stay with what they did.
 *
 * This stuff is aligned on double-byte boundaries and the pointer to the
 * next block in a circular list is tagged with the status of the current
 * block. Blocks are not coalesced when freed, that is done by the allocator
 * when trying to locate a sufficiently large free block.
 *
 * As an extra twist, you might have wondered why alloc () tries to loop
 * twice over the whole arena. It does that because it looks for an exact fit
 * (after coalescing). The allocator has no memory because there is no actual
 * overall arena structure, so every call to alloc () will try to coalesce the
 * entire arena unless there is an exact-sized hole.
 */

enum {
	BLOCK_FREE	= 0,
	BLOCK_USED
};

#define	ALIGN_MASK	1
#define	align(p)	((ALL *) ((__ptr_arith_t) (p) & ~ ALIGN_MASK))
#define	link(p)		align ((p)->a_link)
#define	tstfree(p)	(((p)->a_link & BLOCK_USED) == BLOCK_FREE)

#define	MAKE_LINK(a,f)	((__ptr_arith_t) a + (f))
#define	MAKE_FREE(a)	((a)->a_link &= ~ BLOCK_USED)
#define	MAKE_USED(a)	((a)->a_link |= BLOCK_USED)


typedef union all_u {
	__ptr_arith_t	a_link;
} ALL;


#define	NEXT_FIT	1

#if	NEXT_FIT

struct _heap {
	ALL	      *	_next_block;
};

#define	HEAP_CONTROL_SIZE	sizeof (heap_t)
#define	START_BLOCK(heap)	((heap)->_next_block)
#define	SET_START_BLOCK(heap,newstart) \
				((heap)->_next_block = (newstart))
#else

#define	HEAP_CONTROL_SIZE	0
#define	START_BLOCK(heap)	((ALL *) (heap))
#define	SET_START_BLOCK(heap,newstart)	((void) 0)

#endif

#ifndef TEST	/* Do not test setarena() or alloc() or free().  */

/*
 * Create an arena.
 */

heap_t *
setarena(cp, n)
register char *cp;
{
	ALL	      *	first_block;
	ALL	      *	last_block;
	heap_t	      *	heap_control;

	/*
	 * Begin by aligning the memory passed in and rounding down the size.
	 */

	{
		int		align = (__ptr_arith_t) cp & ALIGN_MASK;

		if (align) {
			align = ALIGN_MASK + 1 - align;
			cp += align;
			n -= align;
		}

		n &= ~ sizeof (ALL *);
	}

	/*
	 * Make room for a heap control area.
	 */

	heap_control = (heap_t *) cp;

	cp += HEAP_CONTROL_SIZE;
	n -= HEAP_CONTROL_SIZE;

	first_block = (ALL *) cp;
	if ((last_block = (ALL *) (cp + n) - 1) < first_block)
		panic("Arena %x too small", (int) cp);

	/*
	 * The initial memory arena consists of a circular list of blocks,
	 * one large free block and one tiny used block at the end. In the
	 * original "design", there was no heap control block.
	 */

	first_block->a_link = MAKE_LINK (last_block, BLOCK_FREE);
	last_block->a_link = MAKE_LINK (first_block, BLOCK_USED);

	SET_START_BLOCK (heap_control, first_block);
	return heap_control;
}


#if	0
/*
 * NIGEL: This code intrigues me... let's keep statistics.
 */

typedef	unsigned long	stat_t;

static	stat_t		_allocations;
static	stat_t		_block_tests;
static	stat_t		_block_fits;
static	stat_t		_exact_fits;

#define	ADD_STAT(stat)	((stat += 1) == 0 ? stat -- : 0)

void dumpstats () {
	printf ("allocations = %d\ntotal tests = %d\n"
		"total matches =  %d\nexact fits = %d\n",
		_allocations, _block_tests, _block_fits, _exact_fits);
}
#else
# define	ADD_STAT(stat)	((void) 0)
#endif

/*
 * Allocate `l' bytes of memory.
 */

__VOID__ *
alloc (heap_control, size)
heap_t	      *	heap_control;
size_t		size;
{
	register ALL *scan_block;
	register ALL *next_block;
	register unsigned i;
	register unsigned n;
	register unsigned s;

	ADD_STAT (_allocations);

	n = 1 + __DIVIDE_ROUNDUP (size, sizeof (ALL));

#if	EXACT_FIT
	for (i = 0 ; i < 2 ; i ++) {
#endif
		for (scan_block = START_BLOCK (heap_control) ;
		     link (scan_block) != START_BLOCK (heap_control) ;
		     scan_block = link (scan_block)) {
			ASSERT (vtop (scan_block) != NULL);
			ADD_STAT (_block_tests);

			if (! tstfree (scan_block))
				continue;

		       for (next_block = link (scan_block) ;
			    tstfree (next_block) ;
			    next_block = link (next_block))
				if (next_block == START_BLOCK (heap_control))
					break;

			scan_block->a_link = MAKE_LINK (next_block,
							BLOCK_FREE);
			if ((s = next_block - scan_block) < n)
				continue;

			ADD_STAT (_block_fits);

			if (s > n) {
#if	EXACT_FIT
	/*
	 * This innocent-looking line of code is what makes this system prefer
	 * exact fits (which only happen about 10% of the time from the
	 * statistics which I have collected).
	 */
				if (i == 0)
					continue;
#endif
				(scan_block + n)->a_link =
					MAKE_LINK (next_block, BLOCK_FREE);
				next_block = scan_block + n;
				scan_block->a_link = MAKE_LINK (next_block,
								BLOCK_FREE);
			}
			MAKE_USED (scan_block);
			SET_START_BLOCK (heap_control, next_block);
#if	0
			memset (scan_block + 1, 0, size);
#endif
#if	EXACT_FIT
			if (i == 0)
				ADD_STAT (_exact_fits);
#endif
			return (__VOID__ *) (scan_block + 1);
		}
#if	EXACT_FIT
	}
#endif
	u.u_error = ENOSPC;
	return NULL;
}

/*
 * Free memory.
 */
free(cp)
char *cp;
{
	register ALL *ap;
	extern char __end;

#if 0
	ap = ((ALL *)cp) - 1;
	if (ap<(ALL *)&__end || tstfree(ap))
		panic("Bad free %x\n", (unsigned)cp);
#else
	ap = ((ALL *)cp) - 1;
	if (ap<(ALL *)&__end) {
		int *r = (int *)(&cp);	/* return address */
		printf("cp=%x ap=%x &__end=%x\n", cp, ap, &__end);
		panic("Bad free() from eip=%x\n", *(r-1));
	}
	if (tstfree(ap)) {
		int *r = (int *)(&cp);	/* return address */
		printf("cp=%x tstfree(%x)=%x\n", cp, ap, tstfree(ap));
		panic("Bad free() from eip=%x\n", *(r-1));
	}
#endif
	MAKE_FREE (ap);
}

#endif /* TEST */

#ifdef _I386
/*
 * unsigned char *palloc(int size);
 *
 * Allocate 'size' bytes of kernel space, which does not cross a click
 * boundary.  Returns a pointer to the space allocated on success,
 * NULL on failure.
 *
 * Allocate twice as much memory as we need, and then return a chunk that
 * does not cross a click boundary.  Immediately before the chunk that
 * we return, we store the true address of the chunk that was kalloc()'d.
 *
 * Since this routine is for relatively small short-lived objects,
 * which we expect to allocate frequently, speed is more important than
 * space overhead.
 *
 * We assume that kalloc() returns word aligned addresses.
 *
 * There are two cases:
 * There is enough room before the click boundary (or there is no click
 * 	boundary) for the pointer and the memory we need.
 * Otherwise, return the chunk starting at the click boundary, storing
 *	the pointer right before the click boundary.  This trick allows
 *	us to allocate up to 1 full click.
 *
 * If kalloc() did NOT return word aligned chunks, then there would be
 * a third case, where there might not be enough space for the pointer
 * before the click boundary.
 */

#define c_boundry(x)	ctob(btoc((x)+1)) /* Next click boundary above x.  */
#define VOID	unsigned char

#ifdef TEST
#undef kalloc
#undef kfree
VOID *kalloc();
void kfree();
#endif /* TEST */

VOID *
palloc(size)
	int size;	/* Size in bytes of area to allocate.  */
{
	VOID *local_arena;	/* Value returned by kalloc().  */
	VOID *boundry;		/* Next click boundry above local_arena.  */
	VOID *retval;		/* What we give back to our caller.  */

	if (size > NBPC)
		panic("palloc(%x): can not palloc more than 1 click.", size);

	/* Fetch twice as much space as requested, plus a pointer.  */
	if ((local_arena = (VOID *) kalloc (sizeof (VOID *) + (2 * size)))
	    == NULL)
		return NULL;
	
	boundry = (VOID *) c_boundry (local_arena);

	T_PIGGY(0x2000, printf("b: %x ", boundry));

	/* First case:  enough space before the boundry.  */
	if ( (boundry - local_arena) >= (size + sizeof(VOID *)) ) {

		T_PIGGY(0x2000, printf("c1 "));

		* (VOID **)local_arena = local_arena;
		retval = local_arena + sizeof(VOID *);
	} else if ((boundry - local_arena) < sizeof(VOID *)) {
		/*
		 * Second case: There is not enough space before the
		 * boundry for the whole pointer.
		 */
		T_PIGGY(0x2000, printf("c2 "));

		* (VOID **)local_arena = local_arena;
		retval = local_arena + sizeof(VOID *);
	} else {

		T_PIGGY(0x2000, printf("c3: %x ", (boundry - local_arena)));

		* (VOID **)(boundry - sizeof(VOID *)) = local_arena;
		retval = boundry;
	}

	T_PIGGY( 0x2000,
		printf("palloc(%x) = %x:%x (was %x:%x), ",
			size, retval, (retval+size)-1,
			local_arena, (local_arena+(2*size)+sizeof(VOID *))-1)
	);

#if	0
	/*
	 * NIGEL: Things in trace macros must now be expressions. These ones
	 * weren't worth cleaning up.
	 */
	T_PIGGY( 0x2000,
		if ((retval+size)-1 > (local_arena+(2*size)+sizeof(VOID *))-1) {
			printf("\npalloc() overrun\n");
		}
		if (retval < local_arena) {
			printf("\npalloc() underrun\n");
		}
	);
#endif

	return (VOID *) retval;
} /* palloc() */

/*
 * void pfree(VOID *ptr);
 * Free the chunk of memory 'ptr' allocated by palloc().
 *
 * Note that 'ptr' is really a VOID *, but we call it VOID **
 * to simplify arithmetic.
 *
 * The address returned by kalloc() is stored immediately
 * before the chunk returned by palloc().
 */
void
pfree(ptr)
	VOID *ptr[];
{
	T_PIGGY(0x2000, printf("pfree(%x):kfree(%x), ", ptr, *(ptr-1)));
	kfree(*(ptr-1));
} /* pfree() */


#ifdef TEST

#include <sys/compat.h>
#include <stdio.h>
#include <stdarg.h>

#define FOURK	4096	/* How many bytes in 4K?  */
#define NUM_TESTS 40	/* How many tests do we run?  */
#define SMALL_NUMBER 6	/* A small number whose exact value we don't care about.  */
#define HUGE	(100*FOURK)	/* Allocate from this pool.  */
#define IGNORE(v)	(v==v)	/* Lint food.  */

unsigned t_piggy = 0x2000;	/* Turn on TRACER bits.  */

main()
{
	int i;
	VOID *chunk;

	for (i = 0; i < NUM_TESTS; ++i) {
		if (NULL == (chunk = palloc(SMALL_NUMBER))) {
			printf("No more fake memory to eat.\n");
			printf("This is probably a bug.\n");
			exit(1);
		}

		printf("chunk: %x\n", chunk);
	}
} /* main() for TEST */

/*
 * Print a message and die.
 */

panic(format)
char * format;
{
	va_list	args;
	va_start (args, format);
	vprintf (format, args);
	va_end (args);
	exit(1);
}

/*
 * Fake kalloc() for use by palloc().
 * Allocate a chunk of some non-existant memory space.
 */
VOID *
kalloc(size)
	int size;
{
	static VOID *base = NULL;
	static VOID *top_free = NULL;
	VOID *retval;


	/*
	 * First time through, allocate a nice big chunk of memory
	 * to carve up.
	 */
	if (NULL == base) {
		if (NULL == (base = malloc(HUGE))) {
			printf("Can not malloc %d bytes.\n", HUGE);
			exit(1);
		}
		/* Make sure we start close to a click boundry.  */
		top_free = c_boundry(base) + SMALL_NUMBER;
	}

	retval = top_free;
	/*
	 * We want to encourage test addresses to migrate accross
	 * click boundries.
	 */
	if (size < (FOURK - 1)) {
		top_free += (FOURK - 1);
	} else {
		top_free += size;
	}

	return(retval);
} /* kalloc() */

/*
 * Fake kfree for pfree() to use.
 */
void
kfree(addr)
	VOID *addr;
{
	IGNORE(addr);
	/* Do nothing!  */
} /* kfree() */

#endif /* TEST */

#endif /* _I386 */
