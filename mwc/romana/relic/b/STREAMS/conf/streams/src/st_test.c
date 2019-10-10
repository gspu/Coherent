#include <stdio.h>
#include <stdlib.h>

#include <kernel/st_alloc.h>


/*
 * Work around some deficiencies in the UNIX library w.r.t. Borland.
 */

#if	__BORLANDC__

#define	RANDOM(n)	random (n)
#define	RANDOMIZE()	randomize ()

#else

/*
 * While this function is not part of POSIX, many implementations provide
 * it; random (n) returns a random integer in the range of 0 to (n - 1).
 *
 * Note that (rand () % n) is not a sufficient implementation, as the
 * distribution of (rand () % n) will not be uniform for n not a power of 2.
 * For instance, with n = 2*RAND_MAX/3, using the modulus will cause the
 * numbers from 0 to RAND_MAX/2 - 1 to occur twice as often as the second
 * half of the range.
 *
 * The expression "(int) ((long) rand () * range / (RAND_MAX + 1))" as used
 * by Borland works to distribute the error uniformly across the range 0 to
 * n-1, but still can cause certain numbers to have a higher probability
 * than their neighbours.
 *
 * The following system minimizes the error at the expense of execution
 * time by simply throwing away those numbers that cause the error, ie those
 * numbers from RAND_MAX down to RAND_MAX - ((RAND_MAX + 1) % n). A more
 * efficient system might use more bits from the LCG, but this works fine.
 */

static int RANDOM (int range) {
	int error, num;

	if (range <= 0)
		return 0;

	error = ((unsigned) RAND_MAX + 1) % range;

	while ((num = rand ()) > RAND_MAX - error)
		;

	return num % range;
}

#include <time.h>

/*
 * And similarly for the randomize () function, which kicks off random number
 * generation.
 */

static void RANDOMIZE (void) {
	srand ((int) time (NULL));
}

#endif	/* ! __BORLANDC__ */


/*
 * Dump some arena internals out...
 */

void st_dump (_ST_HEAP_CONTROL_P q) {
	_ST_ADDR_T	scan = _ST_HEAP_FIRST (q, 0);
	int	count, size, cntl;

	do {
		size = count = 0;
		while (! _ST_BLOCK_FREE (cntl = _ST_BLOCK_CONTROL (q, scan))) {
                        size += _ST_BLOCK_SIZE (cntl);
			count ++;
			if (_ST_HEAP_NEXT (scan, cntl) >= q->_arena_end)
				break;
			scan = _ST_HEAP_NEXT (scan, cntl);
		}
		printf ("%d allocated blocks of total size %d\n", count, size);

		if (! _ST_BLOCK_FREE (cntl))
			break;

		size = count = 0;
		while (_ST_BLOCK_FREE (cntl = _ST_BLOCK_CONTROL (q, scan))) {
			size += _ST_BLOCK_SIZE (cntl);
			count ++;
			if (_ST_HEAP_NEXT (scan, cntl) >= q->_arena_end)
				break;
			scan = _ST_HEAP_NEXT (scan, cntl);
		}
		printf ("%d free blocks of size %d\n", count, size);
	} while (_ST_HEAP_NEXT (scan, cntl) < q->_arena_end);
}

/*
 * Dump a small selection of buckets in great detail.
 */

void st_detail (_ST_HEAP_CONTROL_P q, int frombkt, int tobkt) {
	_ST_ADDR_T	scan = _ST_HEAP_FIRST (q, frombkt);
	int	i;

	while (scan < _ST_HEAP_FIRST (q, tobkt + 1)) {
		if ((i = _ST_HEAP_BUCKET (q, scan)) >= frombkt)
			while (frombkt <= i)
				printf ("\nBucket %d, %d biggest free, entry @ %04x : ",
					frombkt ++,
					_ST_HEAP_BIGGEST (q, frombkt + q->_buckets_inuse),
					scan);
		i = _ST_BLOCK_CONTROL (q, scan);
		printf (_ST_BLOCK_FREE (i) ? "Free %04x (%d) : " :
                 			     "Used %04x (%d) : ",
			scan, _ST_BLOCK_SIZE (i));
		scan = _ST_HEAP_NEXT (scan, i);
	}
}


/*
 * An assertion mechanism for testing the allocator.
 */

#define	ASSERT(q,x,i)	(! (x) ? st_fatal (q,i, __LINE__) : (void) 0)

static void st_fatal (_ST_HEAP_CONTROL_P q, int i, int line) {

	printf ("Assertion failure at line %d of file " __FILE__ "\n", line);
	printf ("Iteration number #%d\n", i);

	st_dump (q);

	abort ();
}


/*
 * Exercise the heap allocator. We exercise the allocator down to the last
 * byte, and define the following relationships so we can assume that by the
 * time we have done TEST_ALLOCS allocations of TEST_ALLOC_SIZE, then we have
 * consumed all free memory.
 *
 * The _ST_WORD_T factor in the TOTALWORDS calculation is for the block
 * headers used internally by the allocator. The _ST_ADDR_T factor is for the
 * addr [] array, which is also taken from the managed area. The "+ 2" factor
 * is for the block headers for (i) the dummy block at the start of the arena
 * and (ii) the block header for the addr [] block.
 */

#define	TEST_ALLOCS	2000		/* number of allocations */
#define	TEST_ALLOC_SIZE	16		/* in bytes */
#define	SEGMENTS	256		/* how many partitions in the space */

#define	TOTALWORDS	(TEST_ALLOCS * (TEST_ALLOC_SIZE + sizeof (_ST_WORD_T)\
					+ sizeof (_ST_ADDR_T)) / sizeof (_ST_WORD_T)\
			 + 2)

void main (int argc, char ** argv) {
	_ST_HEAP_CONTROL_P q;
	void * mem;
	_ST_ADDR_T * addr;
	int i, base, leave;
	int forward = 0, back = 0, shuffle = 1;
	int growup = 1, growdown = 1, growboth = 1, growmove = 1, shrink = 1;

	RANDOMIZE ();

	printf ("TOTALWORDS = %d\n", TOTALWORDS);

	for (i = 1 ; i < argc ; i ++)
		if (argv [i][0] == '-')
			switch (argv [i][1]) {

			case 'f':
				forward = argv [i][2] == 0 ? 1 : atoi (argv [i] + 2);
				break;

			case 'b':
				back = argv [i][2] == 0 ? 1 : atoi (argv [i] + 2);
				break;

			case 'r':
				shuffle = argv [i][2] == 0 ? 1 : atoi (argv [i] + 2);
				break;

			case 'l':
				leave = argv [i][2] == 0 ? 1 : atoi (argv [i] + 2);
				break;
			}

	q = (_ST_HEAP_CONTROL_P) malloc (_ST_HEAP_CONTROL_SIZE (SEGMENTS));
	mem = malloc (TOTALWORDS * sizeof (_ST_WORD_T));

	st_ctor (q, SEGMENTS, TOTALWORDS, (_ST_ADDR_T) mem);

	st_assert (q);

	printf ("Using qheap : ");

	/*
	 * Initially, why not get this space from the test arena ?
	 */

	addr = (_ST_ADDR_T *) st_new (q, TEST_ALLOCS * sizeof (* addr));


	/*
	 * Exercises for realloc (), growing blocks up.
	 */

	while (growup -- > 0) {
		for (i = 0 ; i < TEST_ALLOCS ; i ++) {
			addr [i] = st_new (q, TEST_ALLOC_SIZE);
			ASSERT (q, addr [i] != NULL, i);
		}

		for (i = 0 ; i < TEST_ALLOCS ; i += 2) {
			ASSERT (q, st_disp (q, addr [i + 1]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);

			addr [i + 1] = st_realloc (q, addr [i], 2 * TEST_ALLOC_SIZE
						   ST_FREE_SIZE (TEST_ALLOC_SIZE));
			ASSERT (q, addr [i + 1] == addr [i], i);

			ASSERT (q, st_disp (q, addr [i]
					 ST_FREE_SIZE (2 * TEST_ALLOC_SIZE)) == 0, i);
		}
	}

	st_dump (q);


	/*
	 * Exercises for realloc (), growing blocks down.
	 */

	while (growdown -- > 0) {
		_ST_ADDR_T	temp;

		for (i = 0 ; i < TEST_ALLOCS ; i ++) {
			addr [i] = st_new (q, TEST_ALLOC_SIZE);
			ASSERT (q, addr [i] != NULL, i);
		}

		temp = addr [0];

		for (i = 0 ; i < TEST_ALLOCS - 1 ; i += 2) {
			ASSERT (q, st_disp (q, addr [i]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);

			/* put a pattern in the block to be moved */

			* addr [i + 1] = (_ST_WORD_T) addr [i + 1];

			ASSERT (q,
				st_realloc (q, addr [i + 1], 2 * TEST_ALLOC_SIZE
					    ST_FREE_SIZE (TEST_ALLOC_SIZE))
				== temp, i);

			/* check that the data was moved correctly */

			ASSERT (q, * temp == (_ST_WORD_T) addr [i + 1], i);

			ASSERT (q, st_disp (q, temp
					 ST_FREE_SIZE (2 * TEST_ALLOC_SIZE)) == 0, i);

			if (st_assert (q) != 0) {
				printf ("error %s, i = %d\n",
					_ST_HEAP_ERROR (q), i);
				st_dump (q);
				return;
			}

		}
	}

	st_dump (q);


	/*
	 * Exercises for realloc (), growing blocks both up and down.
	 */

	while (growboth -- > 0) {

		for (i = 0 ; i < TEST_ALLOCS ; i ++) {
			addr [i] = st_new (q, TEST_ALLOC_SIZE);
			ASSERT (q, addr [i] != NULL, i);
		}

		for (i = 0 ; i < TEST_ALLOCS - 3 ; i += 4) {
			ASSERT (q, st_disp (q, addr [i]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);

			if (st_assert (q) != 0) {
				printf ("st_disp () #1 error %s, i = %d\n",
					_ST_HEAP_ERROR (q), i);
				st_dump (q);
				return;
			}

			ASSERT (q, st_disp (q, addr [i + 2]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);

			if (st_assert (q) != 0) {
				printf ("st_disp () #2 error %s, i = %d\n",
					_ST_HEAP_ERROR (q), i);
				st_dump (q);
				return;
			}

			/* put a pattern in the block to be moved */

			* addr [i + 1] = (_ST_WORD_T) addr [i + 1];

			ASSERT (q, st_realloc (q, addr [i + 1], 3 * TEST_ALLOC_SIZE
					    ST_FREE_SIZE (TEST_ALLOC_SIZE))
				== addr [i], i);

			/* check that the data was moved correctly */

			ASSERT (q, * addr [i] == (_ST_WORD_T) addr [i + 1], i);

			if (st_assert (q) != 0) {
				printf ("error %s, i = %d\n",
					_ST_HEAP_ERROR (q), i);
				st_dump (q);
				return;
			}
		}

		for (i = 0 ; i < TEST_ALLOCS - 3 ; i += 4) {
			ASSERT (q, st_disp (q, addr [i]
					 ST_FREE_SIZE (3 * TEST_ALLOC_SIZE)) == 0, i);
			ASSERT (q, st_disp (q, addr [i + 3]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);
		}
	}

	st_dump (q);


	/*
	 * Perform a given number of allocations and shuffled decallocations,
	 * leaving behind a certain number of allocated blocks to clutter the
	 * arena for the next set.
	 */

	leave = base = 0;
	while (shuffle -- > 0) {
		for (i = base ; i < TEST_ALLOCS ; i ++) {
			addr [i] = st_new (q, TEST_ALLOC_SIZE);
			if (st_assert (q) != 0) {
				printf ("Alloc error %s, %d blocks remaining",
					_ST_HEAP_ERROR (q), i);
				return;
			}
			ASSERT (q, addr [i] != NULL, i);
		}

		base = shuffle > 0 ? leave : 0;

	        for (i = TEST_ALLOCS ; i -- > base ;) {

			/*
			 * Pick an element to free, then exchange that for
			 * the last element, thereby shortening the list.
			 */

			int elem = RANDOM (i + 1);
			ASSERT (q, st_disp (q, addr [elem]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);
			if (st_assert (q) != 0) {
				printf ("Free error %s, %d blocks remaining",
					_ST_HEAP_ERROR (q), i);
				return;
			}
                        addr [elem] = addr [i];
		}
	}

        st_dump (q);


	/*
	 * Deallocate from the bottom to the top.
	 */

	while (forward -- > 0) {
		for (i = 0 ; i < TEST_ALLOCS ; i ++) {
			addr [i] = st_new (q, TEST_ALLOC_SIZE);
			ASSERT (q, addr [i] != NULL, i);
		}

		for (i = 0 ; i < TEST_ALLOCS ; i ++)
			ASSERT (q, st_disp (q, addr [i]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);
	}


	/*
	 * Deallocate from the top to the bottom.
	 */

	while (back -- > 0) {
		for (i = 0 ; i < TEST_ALLOCS ; i ++) {
			addr [i] = st_new (q, TEST_ALLOC_SIZE);
			ASSERT (q, addr [i] != NULL, i);
		}

		for (i = TEST_ALLOCS ; i -- > 0 ;)
			ASSERT (q, st_disp (q, addr [i]
					 ST_FREE_SIZE (TEST_ALLOC_SIZE)) == 0, i);
	}
}
