/*
 * Main C routines for the quick heap manager.
 *
 * This implementation is directly derived from the article in which this
 * algorithm first appeared ;
 *
 *	"Efficient Implementation of the First-Fit Strategy for Dynamic
 *	 Storage Allocation"
 *	R. P. Brent, Australian National University
 *	ACM Transactions on Programming Languages and Systems
 *	Volume 11, No. 3, July 1989 pp 388-403.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__CONST__
 *		__USE_PROTO__
 *		__ARGS ()
 *	<common/xdebug.h>
 *		__LOCAL__
 *	<stddef.h>
 *		NULL
 */

#include <common/ccompat.h>
#include <common/xdebug.h>
#include <stddef.h>

#include <kernel/st_alloc.h>


/*
 * Since this code can be used in an embedded environment, we may not want to
 * use the C library version of assert () to report errors. On the other hand,
 * in a hosted environment we might.
 */

#if	_HOSTED

#include <assert.h>

#define	ASSERT(x)	assert (x)

#else

#include <sys/debug.h>

#endif


/*
 * In several places in the code we need to adjust pointer values by +1 or -1
 * to account for the space taken up the by the links between adjacent
 * blocks, allocated or not.
 *
 * Here we define some private macros to help deal with this. While it is
 * correct and reasonably documented as it stands, we will likely want to
 * implement a variant algorithm that does not keep allocated blocks on a
 * list. This should reduce both search time and space overhead, with the
 * expense of requiring the user to supply the original block size. For now,
 * we pay both penalties.
 */

#define _ST_WORD_S	sizeof (_ST_WORD_T)

#define	_ST_AHDR_SIZE	1		/* word size of an allocated header */

#define	_ST_BYTE2WORD(s) ((s + (1 + _ST_AHDR_SIZE) * _ST_WORD_S - 1) / _ST_WORD_S)
					/*
					 * _ST_BYTE2WORD () converts a size
					 * passed in by a client into a block
					 * word size, which includes space
					 * for the allocated-block header.
					 */
#define	_ST_WORD2BYTE(s) ((s - _ST_AHDR_SIZE) * _ST_WORD_S)
					/*
					 * _ST_WORD2BYTE () reverses the
					 * mapping given by _ST_BYTE2WORD ().
					 */
#define	_ST_ADDR2PTR(q,a)	((__VOID__ *) a)
					/*
					 * Mapping from an _ST_ADDR_T to a
					 * user pointer to real memory.
					 */
#define	_ST_PTR2ADDR(q,a)	((_ST_WORD_T *) a)
					/*
					 * Mapping from a user pointer to
					 * an _ST_ADDR_T.
					 */

#define	_ST_BUCKET_BASE(q,b) (q->_arena_base + (b) * q->_words_per_bucket)

/*
 * This function does some general checks on the consistency of the contents
 * of the _ST_HEAP_CONTROL block passed as "q". If the contents of the control
 * block (and the auxiliary arrays which are part of it) do not pass muster,
 * then the _qheap_error member of the control block is set, and the function
 * returns an error indication.
 *
 * The return value is 0 if the control block tested OK, -1 if it did not.
 */

#if	__USE_PROTO__
__LOCAL__ int (st_assert) (_ST_HEAP_CONTROL_P q)
#else
__LOCAL__ int
st_assert __ARGS ((q))
_ST_HEAP_CONTROL_P	q;
#endif
{
	_ST_ADDR_T	scan;
	int		count, cntl, bucket;

	scan = _ST_BUCKET_BASE (q, 0);

	for (bucket = 0 ; bucket < q->_buckets_inuse ; bucket ++) {
		_ST_ADDR_T	end;
		int		max;


		if ((end = _ST_BUCKET_BASE (q, bucket + 1)) > q->_arena_end)
			end = q->_arena_end;

		/*
		 * Verify that _ST_HEAP_FIRST () points within the bucket,
		 * to the current value of scan if there are blocks in this
		 * bucket.
		 */


		if (scan < end &&
		    _ST_HEAP_FIRST (q, bucket) != scan) {

			_ST_SET_ERROR (q, "Bad first block pointer in bucket");
			return -1;
		} else if (scan >= end && _ST_HEAP_FIRST (q, bucket) < end) {

			_ST_SET_ERROR (q, "First block pointer should be pointing past bucket end");
			return -1;
		}


		/*
		 * Walk over the contents of the bucket to verify the maximum
		 * block size within the bucket. We also check here for
		 * consecutive unmerged free blocks.
		 */

		count = max = 0;

		while (scan < end) {

			cntl = _ST_BLOCK_CONTROL (q, scan);

			if (_ST_BLOCK_FREE (cntl)) {	/* block is free */

				if (cntl > max)
					max = cntl;

				if (++ count > 1) {

					_ST_SET_ERROR (q, "Adjacent free blocks not merged");
					return -1;
				}
			} else				/* block in use */
				count = 0;

			scan = _ST_HEAP_NEXT (scan, cntl);
		}

		if (_ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse) != max) {

			_ST_SET_ERROR (q, "Incorrect maximum free block size");
			return -1;
		}


		/*
		 * Check for unmerged free blocks between bucket boundaries.
		 */

		if (count != 0 && scan < q->_arena_end &&
		    _ST_BLOCK_FREE (_ST_BLOCK_CONTROL (q, scan))) {

			_ST_SET_ERROR (q, "Unmerged free blocks between block boundaries");
			return -1;
		}
	}


	if (scan != q->_arena_end) {

		_ST_SET_ERROR (q, "Last block too large!");
		return -1;
	}

	return 0;
}


/*
 * st_double () doubles the depth of the index heap, assuming that
 * _buckets_inuse < _buckets_maximum / 2
 *
 * For internal use only.
 */

#if	__USE_PROTO__
__LOCAL__ void (st_double) (_ST_HEAP_CONTROL_P q)
#else
__LOCAL__ void
st_double __ARGS ((q))
_ST_HEAP_CONTROL_P	q;
#endif
{
	int i,k;

	ASSERT (q->_buckets_inuse * 2 <= q->_buckets_maximum);


	/*
	 * The new buckets don't have any blocks starting within them, so
	 * set the base address over the top.
	 */

	k = q->_buckets_inuse;
	for (i = 0 ; i < k ; i ++)
		q->_bucket_first [i + k] = q->_arena_end;

	/*
	 * The bucket size heap needs to be expanded by copying the maximum
	 * sizes down for the preexisting buckets, and inserting zeroes for
	 * the new buckets.
	 *	eg	max(b1,b2) b1 b2
	 *	becomes	max(b1,b2) max(b1,b2) 0 b1 b2 0 0
	 */

	for (; k > 0 ; k >>= 1)
		for (i = 0 ; i < k ; i ++) {

			_ST_HEAP_BIGGEST (q, 2 * k + i) = _ST_HEAP_BIGGEST (q, k + i);
			_ST_HEAP_BIGGEST (q, 3 * k + i) = 0;
		}

	q->_buckets_inuse <<= 1;
}


/*
 * st_reduced () does the housekeeping necessary if a block with control word
 * at "a" has been allocated, or merged with a block on it's left in a
 * different segment. Note that we count on a block having been freed/
 * allocated and having a valid control word.
 *
 * It should be that case that the _ST_HEAP_FIRST () has already been updated
 * in order for this routine to correctly calculate the sizes
 *
 * This implementation modifies this function, in that a test
 *	if (BlockIsFree && CurrentMaximum > SizeOfNewBlock)
 *		return;
 * from the C transliteration of the published algorithm has been moved out
 * to the callers of this function. This allows the callers to use a block
 * size value cached in their locals to perform the calculation, rather than
 * making this function fetch the control word for the block. This is NOT
 * an important size/speed optimisation (although parts of the original test
 * have been elided in the broken-out version), rather it allows algorithms
 * like realloc () considerably more latitude in the order in which things
 * are done.
 *
 * This function was called blfix1 () in the published algorithm. The original
 * algorithm passed in the block address, but since we have moved the check
 * discussed above outside, we only need the bucket number.
 */

#if	__USE_PROTO__
__LOCAL__ void (st_reduced) (_ST_HEAP_CONTROL_P q, int bucket)
#else
__LOCAL__ void
st_reduced __ARGS ((q, bucket))
_ST_HEAP_CONTROL_P	q;
int			bucket;
#endif
{
	_ST_ADDR_T first, next;
	_ST_WORD_T max;

#if 0	/* this test moved out to our callers */
	/*
	 * if (BlockIsFree && CurrentMaximum > SizeOfNewBlock) then there
	 * is nothing to be done to segment "bucket".
	 */

	max = _ST_BLOCK_CONTROL (q, a);
	if (_ST_BLOCK_FREE (max) &&
	    _ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse) > _ST_BLOCK_SIZE (max))
		return;
#endif

	/*
	 * Work out address of first block in "current" bucket, and the
	 * base address of the "next" bucket. Note that we have to test to
	 * see if "next" winds up past the end of space because the total
	 * size of the arena may not divide evenly into buckets..
	 */

	first = _ST_HEAP_FIRST (q, bucket);
	next = q->_arena_base + (bucket + 1) * q->_words_per_bucket;

	if (next >= q->_arena_end)
		next = q->_arena_end;

	/*
	 * Calculate (by walking the block chain) the new maximum size free
	 * block in this bucket, which may now be empty.
	 */

	max = 0;
	while (first < next) {	/* There is a block starting in this bucket */
		_ST_WORD_T cntl = _ST_BLOCK_CONTROL (q, first);

		ASSERT (cntl != 0);

		if (max < cntl)
			max = cntl;

		first = _ST_HEAP_NEXT (first, cntl);
	}


	/*
	 * Now propagate the new maximum size information up the heap.
	 */

	bucket += q->_buckets_inuse;
	_ST_HEAP_BIGGEST (q, 0) = 0;	/* sentinel */

	while (_ST_HEAP_BIGGEST (q, bucket) > max) {
		int temp;

		_ST_HEAP_BIGGEST (q, bucket) = max;

		temp = _ST_HEAP_BIGGEST (q, _ST_BUCKET_SIBLING (bucket));
		if (max < temp)
			max = temp;

		bucket = _ST_BUCKET_PARENT (bucket);
	}
}


/*
 * st_grown () does the housekeeping necessary after a block with control word
 * at "a" is freed, or merged with a block on its right, or created by
 * splitting (with "a" on the right of the split, in a different bucket than
 * the start of the original block that was split)
 *
 * This function was called blfix2 () in the published algorithm.
 */

#if	__USE_PROTO__
__LOCAL__ void (st_grown) (_ST_HEAP_CONTROL_P q, _ST_ADDR_T a, int bucket)
#else
__LOCAL__ void
st_grown __ARGS ((q, a, bucket))
_ST_HEAP_CONTROL_P	q;
_ST_ADDR_T		a;
int			bucket;
#endif
{
	int 	max;

	ASSERT (bucket == _ST_HEAP_BUCKET (q, a));


	/*
	 * Expand the number of buckets in the heap if necessary.
	 */

	while (bucket >= q->_buckets_inuse)
		st_double (q);

	/*
	 * This may be a new first free block.
	 */

	if (_ST_HEAP_FIRST (q, bucket) > a)
		_ST_HEAP_FIRST (q, bucket) = a;

	/*
	 * Propagate changed block-size information upwards through heap.
	 */

	bucket += q->_buckets_inuse;
	max = _ST_BLOCK_CONTROL (q, a);
	_ST_HEAP_BIGGEST (q, 0) = max;	/* sentinel */

	while (_ST_HEAP_BIGGEST (q, bucket) < max) {

		_ST_HEAP_BIGGEST (q, bucket) = max;

		bucket = _ST_BUCKET_PARENT (bucket);
	}
}


/*
 * Returns the predecessor block to "a", which is guaranteed to exist thanks
 * to the dummy first block.
 *
 * If the address passed in does not belong to a valid block then the value
 * returned is the same value that was passed in, ie "a".
 */

#if	__USE_PROTO__
__LOCAL__ _ST_ADDR_T (st_pred) (_ST_HEAP_CONTROL_P q, _ST_ADDR_T a, int bucket)
#else
__LOCAL__ _ST_ADDR_T
st_pred __ARGS ((q, a, bucket))
_ST_HEAP_CONTROL_P	q;
_ST_ADDR_T		a;
int			bucket;
#endif
{
	_ST_ADDR_T prev, scan;

	ASSERT (bucket == _ST_HEAP_BUCKET (q, a));


	/*
	 * If the passed-in block is the first in the bucket, then the
	 * predecessor lives in the rightmost non-empty bucket to the left.
	 */

	if (_ST_HEAP_FIRST (q, bucket) == a) {
		/*
		 * We walk the heap to find the nearest bucket on the left
		 * that has a free block in it, as buckets that do not
		 * contain free blocks may not contain a valid "first block"
		 * entry, since due to block coalescence these entries may
		 * refer to blocks which have been combined with others.
		 *
		 * st_pred () is the only code which is affected by this.
		 */

		bucket += q->_buckets_inuse;

		_ST_HEAP_BIGGEST (q, 0) = 1;	/* boundary */

		while (_ST_HEAP_BIGGEST (q, bucket - 1) == 0)
			bucket = _ST_BUCKET_PARENT (bucket);

		/*
		 * Code here fixes two known "defects" in the original
		 * published algorithm.
		 *  (i) In the case where "bucket" above could reach the root
		 *	node (eg, every word of storage had been previously
		 *	allocated), the "bucket" would go to zero and the
		 *	descend code would loop indefinitely.
                 * (ii) In the case where storage in the left subtree of the
                 *	heap relative to the initial point is completely
		 * 	allocated, the code to climb the heap will eventually
		 *	look into the next higher level of the heap once
                 *	"bucket" became a power of two (unless defect (i)
		 *	occurred instead) and as a result would descend into
		 *	the wrong part of the heap.
		 *
		 * We check here for "bucket" being a power of two, in which
		 * case we snap the result of the search to the lowest bucket
		 * on the left.
		 */

#define	IS_POWER_OF_TWO(n) (((n - 1) ^ n) >= n)

		if (! IS_POWER_OF_TWO (bucket)) {

			bucket --;

			while (bucket < q->_buckets_inuse) {

				bucket = 2 * bucket + 1;
				if (_ST_HEAP_BIGGEST (q, bucket) <= 0)
					bucket --;
			}

			bucket -= q->_buckets_inuse;
		} else
			bucket = 0;
	}


	/*
	 * Either way, find the predecessor block by following the internal
	 * block linkage within this bucket.
	 */

	scan = _ST_HEAP_FIRST (q, bucket);
	do {
		prev = scan;
		scan = _ST_HEAP_NEXT (scan, _ST_BLOCK_CONTROL (q, scan));

		ASSERT (prev != scan);
	} while (scan < a);

	if (scan != a) {

		_ST_SET_ERROR (q, "Unable to find previous for block");
		return a;	/* flag error by returning same */
	}

	return prev;
}


/*
 * Returns the index of a block of at least "size" words, or 0 if no such
 * block exists.
 */

#if	__USE_PROTO__
__VOID__ * (st_alloc) (_ST_HEAP_CONTROL_P q, size_t size)
#else
__VOID__ *
st_alloc __ARGS ((q, size))
_ST_HEAP_CONTROL_P	q;
size_t			size;
#endif
{
	int		n, bucket, cntl;
	_ST_ADDR_T	scan;

	/*
	 * Before we begin, convert the size_t passed in into a word count.
	 * Note that we assume that the integer division of a size_t will
	 * be optimised by the compiler into an appropriate number of right-
	 * shifts (since a size_t is always unsigned, right?).
	 *
	 * We store the result into an integer because the free/used
	 * comparison means we involve the sign bit.
	 */

	n = _ST_BYTE2WORD (size);	/* includes header size */


	/*
	 * Since the node at the top of the size heap contains the size of
	 * the largest available block, quickly determine whether or not
	 * this request can be satisfied at all.
	 */

	if (_ST_HEAP_BIGGEST (q, 1) < n)
		return 0;


	/*
	 * Now traverse the heap to find the first bucket containing a block
	 * of sufficient size to satisfy the request.
	 */

	bucket = 1;

	while (bucket < q->_buckets_inuse) {

		bucket = 2 * bucket;
		if (_ST_HEAP_BIGGEST (q, bucket) < n)
			bucket ++;
	}

	bucket -= q->_buckets_inuse;


	/*
	 * Now traverse the internal linkage within the bucket to find the
	 * first block of the requisite size.
	 */

	scan = _ST_HEAP_FIRST (q, bucket);

	while ((cntl = _ST_BLOCK_CONTROL (q, scan)) < n) {

		ASSERT (cntl != 0);
		scan = _ST_HEAP_NEXT (scan, cntl);
	}


	/*
	 * Now "scan" contains the index of the control word of the
	 * desired block.
	 */

	_ST_BLOCK_SET_USED (q, scan, n);


	/*
	 * The published algorithm used a variable here to hold the result
	 * of a test on the basis that the call to st_grown () below might
	 * change the result. Actually, st_grown () would not change the
	 * result, but since st_grown () may call st_double () and alter
	 * the offset of the leaf layer of the block heap, that would
	 * invalidate his test. Since our "bucket" does not have that offset
	 * built into it, we perform the test when needed.
	 *
	 * Note that we still perform st_grown () before st_reduced (), as
	 * in the original. The reason for this was not explicated, but
	 * appears to be because doing it in this order may reduce the
	 * average number of heap nodes visited due to the particular heap
	 * update termination conditions.
	 */

	/*
	 * If necessary, split block; this may require a call to st_grown ()
	 * if the block created by the split is not in the same bucket.
	 */

	if (cntl > n) {
		_ST_ADDR_T	next = _ST_HEAP_NEXT (scan, n);
		int		next_bucket;

		_ST_BLOCK_SET_FREE (q, next, cntl - n);

		if ((next_bucket = _ST_HEAP_BUCKET (q, next)) > bucket)
			st_grown (q, next, next_bucket);
	}

	if (cntl == _ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse))
		st_reduced (q, bucket);

	return _ST_ADDR2PTR (q, _ST_HEAP_NEXT (scan, _ST_AHDR_SIZE));
}


/*
 * Release a block of memory obtained using st_alloc (), where "a" is the
 * memory word index that was returned by st_alloc ().
 *
 * Note that this function comes in two flavours, depending on whether you
 * want clients to have to pass in the block-size.
 */

#if	__USE_PROTO__
int (st_free) (_ST_HEAP_CONTROL_P q, __VOID__ * a ST_FREE_SIZE (size_t size))
#else
int
#ifdef	USE_ST_SIZE
st_free __ARGS ((q, a, size))
size_t			size;
#else
st_free __ARGS ((q, a))
#endif
_ST_HEAP_CONTROL_P	q;
__VOID__	      *	a;
#endif
{
	int		bucket, cntl, temp;
	_ST_ADDR_T	prev, next, addr;
	int		reduce = 0;	/* optimisation flag, see below */

	addr = _ST_HEAP_NEXT_RAW (_ST_PTR2ADDR (q, a), - _ST_AHDR_SIZE);

	cntl = _ST_BLOCK_CONTROL (q, addr);

	if (_ST_BLOCK_FREE (cntl))
		return -1;		/* Block already free */

	cntl = _ST_BLOCK_SIZE (cntl);

#ifdef	USE_ST_SIZE
	/*
	 * As discussed in st_alloc (), we convert a passed-in byte count
	 * into a word count to isolate the clients from the notion of
	 * what we are using as a "word".
	 *
	 * The expression we want to test is
	 *	cntl == ceil (size / sizeof (_ST_WORD_T)) + 1
	 * where the + 1 factor is for the block size header.
	 */

	if (cntl != _ST_BYTE2WORD (size))
		return -2;		/* Block size mismatch */
#endif

	bucket = _ST_HEAP_BUCKET (q, addr);


	/*
	 * Locate the previous block. Note that we attempt this operation
	 * considerably earlier than we really need to; this is done since
	 * it is the only really reliable way of verifying that the address
	 * given to this routine really does belong to a block that was
	 * allocated with st_alloc ().
	 *
	 * (Note that the above statement is only true for versions of the
	 * algorithm that maintain allocated blocks on the block list)
	 */

	if ((prev = st_pred (q, addr, bucket)) == addr)
		return -3;		/* not a valid block */

	/*
	 * Now that we have performed some sanity checks, free the block.
	 */

	_ST_BLOCK_SET_FREE (q, addr, cntl);


	/*
	 * Check the next rightmost block to see if we should merge with it.
	 */

	next = _ST_HEAP_NEXT (addr, cntl);

	if (next < q->_arena_end &&
	    _ST_BLOCK_FREE (temp = _ST_BLOCK_CONTROL (q, next))) {
		/*
		 * Merge the new block with its immediate neighbour on the
		 * right. Note that we elide the _ST_BLOCK_SIZE () of temp
		 * immediately below because _ST_BLOCK_SET_FREE masks the
		 * third argument anyway.
		 */

		_ST_BLOCK_SET_FREE (q, addr, cntl += temp);

		temp = _ST_HEAP_BUCKET (q, next);

		/*
		 * Do we need to recalculate the maximum block size
		 * for the block that "next" is in ? Yes, iff
		 * heap_biggest (temp) == block_size (next).
		 * We elide the call to _ST_BLOCK_SIZE below since
		 * we know the block is free, hence needs no masking.
		 */

		reduce = _ST_HEAP_BIGGEST (q, temp + q->_buckets_inuse) ==
				_ST_BLOCK_CONTROL (q, next);
		ASSERT (_ST_HEAP_BIGGEST (q, temp + q->_buckets_inuse) >=
				_ST_BLOCK_CONTROL (q, next));
		if (temp > bucket) {

			_ST_HEAP_FIRST (q, temp) = _ST_HEAP_NEXT (addr, cntl);

			if (reduce) {

				st_reduced (q, temp);
				reduce = 0;
			}
		} else
			ASSERT (_ST_HEAP_FIRST (q, temp) != next);
	}


	/*
	 * Check the next leftmost block to see if we should merge with it.
	 */
	/*
	 * Optimisation note: the published version of the algorithm
	 * does a normal call to st_reduced () below. However, since we
	 * are freeing a block, we note that the only circumstance where
	 * this will be at all necessary is when the block we are freeing
	 * was merged with a block on it's right which was the previous
	 * largest free block (or at least the same size). We can thus
	 * reduce the number of calls to st_reduced () by putting in an
	 * extra guard condition with a flag set above.
	 *
	 * Is this worth the effort? Let's profile it and see.
	 */

	if (_ST_BLOCK_FREE (temp = _ST_BLOCK_CONTROL (q, prev))) {

		_ST_BLOCK_SET_FREE (q, prev, cntl += temp);

		/*
		 * If we are merging with a block in a previous bucket, then
		 * we must adjust our "first block" and call st_reduced () to
		 * update the size heap.
		 */

		if (_ST_HEAP_FIRST (q, bucket) == addr) {

			_ST_HEAP_FIRST (q, bucket) = _ST_HEAP_NEXT (prev, cntl);

			/*
			 * As discussed above, we call st_reduced () iff
			 * we have merged with a block on the right of a
			 * size that indicates recomputing is necessary.
			 */

			if (reduce) {

				ASSERT (_ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse) <=
					_ST_BLOCK_CONTROL (q, addr));
				st_reduced (q, bucket);
			}

			/*
			 * We cannot incrementally update "bucket" since
			 * "prev" may refer to address a number of buckets
			 * prior to "addr".
			 */

			bucket = _ST_HEAP_BUCKET (q, prev);
		}

		st_grown (q, prev, bucket);

	} else if (cntl > _ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse)) {
		/*
		 * The total size of the newly freed block exceeds the
		 * previous maximum block size of the current bucket, so call
		 * st_grown () to update the size heap.
		 *
		 * Note that st_alloc () calls st_grown () without performing
		 * the size test like we do, because st_alloc () may want the
		 * heap size doubled. Here, that cannot happen, so we avoid
		 * the call.
		 */

		st_grown (q, addr, bucket);
	}

	return 0;
}


#ifndef	_ST_BLOCK_COPY

/*
 * Helper function which supplies a default block-copy routine for the
 * st_realloc () function in case there is no special-purpose copy routine
 * in the target environment.
 */

#if	__USE_PROTO__
__LOCAL__  void (st_copy) (_ST_WORD_T * dest, __CONST__ _ST_WORD_T * src,
			   size_t copywords)
#else
__LOCAL__ void
st_copy __ARGS ((dest, src, copywords))
_ST_WORD_T	      * dest;
_ST_WORD_T	      * src;
size_t			copywords;
#endif
{
	while (copywords --)
		* dest ++ = * src ++;
}

#define	_ST_BLOCK_COPY(q,d,s,n)	st_copy (_ST_HEAP_ADDR (q, d), _ST_HEAP_ADDR(q, s), n)

#endif	/* ! defined (_ST_BLOCK_COPY) */


/*
 * Request that the block of memory at address "a" be grown (or shrunk) in
 * size to "newsize" bytes (possibly from "oldsize" bytes). The returned
 * value is the base address of the new block of memory, which may be
 * different than the old address. If the block was moved, then the contents
 * are guarenteed to be preserved bit-for-bit, but the client must take
 * responsibility for relocating pointers.
 *
 * If it is not possible for the allocator to either grow or relocate the
 * block due to a lack of space, st_realloc () returns 0, which for us can
 * never be a valid return address. In this case, the original block has been
 * left untouched.
 *
 * Note that this function comes in two flavours, depending on whether you
 * want clients to have to pass in the block-size.
 *
 * --------------------------------------------------------------------------
 *
 * Design note: there are many, many ways that this function could be
 * implemented, depending on how sensitive you are to issues of code size,
 * execution speed, or arena space efficiency.
 *
 * The possible checks are (in order of execution time reduction)
 *   (i) Check to see if the block can be extended upwards,
 *  (ii) Find the previous block to see if it can be extended downwards,
 * (iii) See if we can grow both down and up to fulfil the request,
 *  (iv) Find any other block of sufficient size to fulfil the request.
 *
 * The primary criterion I have set for this routine is that it must always
 * succeed in finding memory to saisfy the request. Note that the number of
 * cases that this produces as a result is extremely large, but that it seems
 * better to fulfil a user's request than to fail it simply on the basis that
 * we want the common case to be fast.
 *
 * For now, we'll prefer not to copy, and reduce our code size by just
 * relying on st_alloc () and st_free () for the worst case... but be aware
 * that it might be a good idea to set a "realloc mode" in the heap control
 * block that specifies how we order operations in the case where we break
 * out the details of new () and disp ().
 */

#if	__USE_PROTO__
__VOID__ * (st_realloc) (_ST_HEAP_CONTROL_P q, __VOID__ * a, size_t newsize
			 ST_FREE_SIZE (size_t oldsize))
#else
__VOID__ *
#ifdef	USE_ST_SIZE
st_realloc __ARGS ((q, a, newsize, oldsize))
size_t			oldsize;
#else
st_realloc __ARGS ((q, a, newsize))
#endif
_ST_HEAP_CONTROL_P	q;
__VOID__	      *	a;
size_t			newsize;
#endif
{
	int		bucket, cntl, prev_reduce, next_bucket, next_first;
	int		delta;		/* block size change in words */
	_ST_ADDR_T	prev, addr, next;

	addr = _ST_HEAP_NEXT_RAW (_ST_PTR2ADDR (q, a), -1);

	cntl = _ST_BLOCK_CONTROL (q, addr);

	if (_ST_BLOCK_FREE (cntl))
		return (_ST_ADDR_T) -1;	/* Block is free ! */

	cntl = _ST_BLOCK_SIZE (cntl);	/* work with free units */


	/*
	 * Before we begin, let's convert newsize (and optionally oldsize)
	 * to word counts from byte counts. This may mean that both round
	 * to the same word count and we don't need to do anything (big win!)
	 */

	newsize = _ST_BYTE2WORD (newsize);

#ifdef	USE_ST_SIZE
	if (cntl != _ST_BYTE2WORD (oldsize))
		return (_ST_ADDR_T) -2;	/* Block size mismatch */
#endif
	if ((delta = newsize - cntl) == 0)
		return a;		/* Already done! What service! */

	bucket = _ST_HEAP_BUCKET (q, addr);


	/*
	 * Locate the previous block, so that we can see if we can grow
	 * down into it. Note that in the variant system where allocated
	 * blocks are not part of a chain (in order to save space) that by
	 * finding the previous free block we achieve an equivalent result,
	 * since we cannot grow down unless we have an adjacent free block.
	 *
	 * Note also that in the case where allocated blocks are not
	 * chained, finding the previous free block is a prerequisite to
	 * finding the subsequent free block.
	 */

	if ((prev = st_pred (q, addr, bucket)) == addr)
		return (_ST_ADDR_T) -3;	/* not a valid block */


	/*
	 * Now that we have performed some sanity checks, look to see how
	 * we should grow the block.
	 */

	/*
	 * Check the next rightmost block to see if we can grow into it.
	 * If we are shrinking the allocation, this is trivially true.
	 *
	 * To save ourselves a local, we re-use "bucket" here as the amount
	 * of free space in the next rightmost block. We need to keep this
	 * value for the next test so that we can expand both up and down
	 * if necessary.
	 */

	bucket = 0;
	prev_reduce = 0;

	if ((next = _ST_HEAP_NEXT (addr, cntl)) < q->_arena_end &&
	    _ST_BLOCK_FREE (cntl = _ST_BLOCK_CONTROL (q, next)))
		bucket = cntl;	/* available adjacent words */

	if (bucket >= delta) {
		/*
		 * Whether we are growing or shrinking, we have enough room.
		 * Set the block's new size, and skip to the common code below
		 * which adjusts the right edge of a block by "delta" words.
		 */

		ASSERT (next < q->_arena_end ||
			(next == q->_arena_end && delta < 0));

		_ST_BLOCK_SET_USED (q, addr, newsize);


		/*
		 * Go to the common exit sequence for in-place adjustment,
		 * after setting "prev" to be the base of the block that we
		 * will return to the user.
		 */

		prev = addr;

		goto adjust_right;
	}


	/*
	 * Check the next leftmost block to see if we should move down. Note
	 * that we add in the free size of the right block to the calculation
	 * so that we can expand both down and up to fill space.
	 *
	 * Given that we have to copy, however, we'll drop to the bottom of
	 * the available room.
	 */

	if (_ST_BLOCK_FREE (cntl = _ST_BLOCK_CONTROL (q, prev)) &&
	    (cntl + bucket) >= delta) {
		/*
		 * Well, we have enough space. Now, let's make "delta" equal
		 * to the amount by which we need to adjust the block on
		 * the right.
		 */

		delta -= cntl;


		/*
		 * We set the "prev" block as used now for st_reduced ().
		 */

		_ST_BLOCK_SET_USED (q, prev, newsize);


		/*
		 * Since we are vaporising the original block at "addr", we
		 * should deal with checking to see if it was the first block
		 * in it's bucket, otherwise we may wind up with a dangling
		 * pointer. For now, we just point it to the successor to
		 * "addr", and if this this does not turn out to be correct,
		 * the common code to move the LHS below will do the right
		 * thing.
		 */

		if (_ST_HEAP_FIRST (q, (bucket = _ST_HEAP_BUCKET (q, addr)))
		    == addr)
			_ST_HEAP_FIRST (q, bucket) = next;

		/*
		 * If we want to move the block down, then we should update
		 * the heap information related to the block we are moving
		 * into before we move into it.
		 *
		 * Everything relating to what happens to the right edge of
		 * the block will be dealt with below, so all we have to do
		 * here is determine whether to call st_reduced ().
		 *
		 * We don't actually call st_reduced () here, since until the
		 * left edge has been dealt with there can be a temporary
		 * loss of block connectivity.
		 */

		bucket = _ST_HEAP_BUCKET (q, prev);

		if (_ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse) == cntl)
			prev_reduce = 1;

		/*
		 * Now, copy the original data from the block at "addr". How
		 * much to copy? Re-fetch the block size from the control
		 * word for now, although this may have to change to use
		 * "oldsize" if this is changed to use a variant algorithm
		 * where allocated blocks are not part of a list.
		 *
		 * How to perform the copy? Each target system will probably
		 * have it's own routine for performing a word-aligned, word-
		 * counted, upward-only high-speed block copy. Here, we
		 * request the services of that routine, with a default
		 * provided just prior to this code in case there is no
		 * special facility for that purpose.
		 *
		 * Note that we perform the move now because there is no way
		 * that the move can invalidate the block header (if any) in
		 * the rightmost block. We know this because the size that we
		 * are copying by is the size of the original block, and we
		 * are moving down, so we can't write over anything after the
		 * original block.
		 */

		_ST_BLOCK_COPY (q, _ST_HEAP_NEXT (prev, _ST_AHDR_SIZE),
				_ST_HEAP_NEXT (addr, _ST_AHDR_SIZE),
				_ST_BLOCK_SIZE (_ST_BLOCK_CONTROL (q, addr)) - _ST_AHDR_SIZE);

		/*
		 * Now, do the common part.
		 */

		ASSERT (_ST_HEAP_NEXT (next, delta) ==
				_ST_HEAP_NEXT (prev, newsize));

		goto adjust_right;
	}


	/*
	 * We have tried to move the block more-or-less in place, and the
	 * some of the block and both neighbours does not yield enough space,
	 * so we try and realloc () the naive way, using st_alloc () and
	 * st_free (). We re-use these routines and just adjust for the fact
	 * that they measure sizes in bytes rather than duplicating the code.
	 *
	 * Please note that the "prev" returned by st_alloc () has already
	 * had the adjustment by 1 word to skip over the control block.
	 */


	if ((prev = _ST_PTR2ADDR (q, st_alloc (q, _ST_WORD2BYTE (newsize))))
	    == 0)
		return 0;


	/*
	 * Do the block copy of the original contents and release them.
	 * See the discussion above on the block copier.
	 *
	 * Note that we pass the address "a" into st_free (), as "addr" is
	 * adjusted to point at the control word.
	 */

	_ST_BLOCK_COPY (q, prev, _ST_HEAP_NEXT (addr, _ST_AHDR_SIZE),
			_ST_BLOCK_SIZE (_ST_BLOCK_CONTROL (q, addr)) - _ST_AHDR_SIZE);

	if (st_free (q, a ST_FREE_SIZE (oldsize)) != 0) {
		/*
		 * What the duece! Throw the bums out on their ears!
		 */

		ASSERT (1 == 0);
	}


	return _ST_ADDR2PTR (q, prev);


	/*
	 * Perform the necessary adjustments to the index heap for the
	 * event that is happening on the right-hand side of the original
	 * block. The block boundary on the right is being moved by "delta"
	 * words from it's *original* location, ie the block at "next" is
	 * being either grown by -delta or shrunk by delta, or remaining
	 * unaffected. If next is being shrunk, then we know at this point
	 * that "next" is free. If it's being grown, then "next" could be
	 * either free or allocated, so we may have to create a new block.
	 */

adjust_right:

	/*
	 * Common case: fetch the current statistics of the "next" block and
	 * set us up so that we point at the location of the new block.
	 */

	if (next < q->_arena_end) {

		cntl = _ST_BLOCK_CONTROL (q, next);
		bucket = _ST_HEAP_BUCKET (q, next);

		/*
		 * We record whether or not "next" has an _ST_HEAP_FIRST ()
		 * pointer looking at it so we can update it below depending
		 * on how we move things around.
		 */

		next_first = _ST_HEAP_FIRST (q, bucket) == next;
	} else {
		/*
		 * Treat the end of the world as an allocated block.
		 */

		cntl = -1;
		next_first = bucket = 0;
	}

	next = _ST_HEAP_NEXT (next, delta);
	next_bucket = _ST_HEAP_BUCKET (q, next);


	if (delta > 0) {		/* eat into "next", which is free */
		/*
		 * If we need to, make the _ST_HEAP_FIRST () pointer track
		 * the block. Note that there is no harm in this if "next"
		 * actually lives a few buckets along, in fact it's required
		 * that if there's no blocks in the bucket, _ST_HEAP_FIRST ()
		 * must point beyond the end of the bucket.
		 */

		if (next_first)
			_ST_HEAP_FIRST (q, bucket) = next;

		/*
		 * Grow up into our neighbour. This case is basically
		 * identical to the regular allocation algorithm except that
		 * we don't have to reserve space for a new control word.
		 *
		 * The comments in st_alloc () also apply here, see above.
		 *
		 * Note that since we know that "next" is free, we also know
		 * that "cntl" is a positive integer and doesn't need masking.
		 */

		if (cntl > delta) {
			/*
			 * Create a free block, which may cause a call to
			 * st_grown () if it begins in another bucket. Like
			 * st_alloc (), we call st_grown () whether or not
			 * it needs to propagate size information up the
			 * index heap, because we may require that the size
			 * of the index heap be doubled as a side-effect of
			 * the overflow into the next bucket.
			 */

			_ST_BLOCK_SET_FREE (q, next, cntl - delta);

			if (next_bucket > bucket)
				st_grown (q, next, next_bucket);
		} else {
			/*
			 * In this case, "next" has been completely eaten
			 * up, so we just let the common code below deal with
			 * shrinking the "biggest free".
			 *
			 * I make lots of gratuitous assertions, don't I?
			 */

			ASSERT (cntl == delta);
		}


		/*
		 * Regardless of whether we have created a new block, we
		 * should see whether we should adjust the "largest free"
		 * for the bucket, according to the usual rules.
		 */

		if (_ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse) == cntl)
			st_reduced (q, bucket);
	} else if (delta < 0) {		/* "-delta" free words below "next" */
		/*
		 * This code is the inverse of the above, except that we
		 * may not be affecting the actual "next" block if it is
		 * in use.
		 *
		 * Note that in this scenario, "next_bucket" has the inverse
		 * of the normal relation to "bucket", since it may be below
		 * "bucket".
		 */

		_ST_BLOCK_SET_FREE (q, next, (_ST_BLOCK_FREE (cntl) ? cntl : 0)
						- delta);

		if (next_bucket < bucket) {

			_ST_HEAP_FIRST (q, bucket) =
				_ST_HEAP_NEXT (next, _ST_BLOCK_CONTROL (q, next));

			/*
			 * We don't want to call st_reduced () if the original
			 * "next" block wasn't free. It won't hurt things,
			 * but it wastes time. However, since "cntl" is
			 * negative in that case, the test below will always
			 * fail in that situation.
			 */

			if (_ST_HEAP_BIGGEST (q, bucket + q->_buckets_inuse) == cntl)
				st_reduced (q, bucket);
		}


		if (_ST_BLOCK_CONTROL (q, next) > _ST_HEAP_BIGGEST (q, next_bucket + q->_buckets_inuse))
			st_grown (q, next, next_bucket);
	}

	if (prev_reduce != 0)
		st_reduced (q, _ST_HEAP_BUCKET (q, prev));


	return _ST_ADDR2PTR (q, _ST_HEAP_NEXT (prev, 1));
}


/*
 * Initialise an arena.
 */

#if	__USE_PROTO__
void (st_init) (_ST_HEAP_CONTROL_P q)
#else
void
st_init __ARGS ((q))
_ST_HEAP_CONTROL_P	q;
#endif
{
	q->_heap_error = NULL;
	q->_buckets_inuse = 1;
	q->_words_per_bucket = (q->_arena_size + q->_buckets_maximum - 1)
					/ q->_buckets_maximum;

	_ST_HEAP_BIGGEST (q, 1) = q->_arena_size;

	_ST_HEAP_FIRST (q, 0) = q->_arena_base;

	_ST_BLOCK_SET_FREE (q, q->_arena_base, q->_arena_size);

	/*
	 * Create dummy sentinel block.
	 */

	st_alloc (q, 0);

	return;
}


/*
 * As above, but a full construction.
 */

#if	__USE_PROTO__
void (st_ctor) (_ST_HEAP_CONTROL_P q, int segs, size_t arensize,
		_ST_ADDR_T arenabase)
#else
void
st_ctor __ARGS ((q, segs, arensize, arenabase))
_ST_HEAP_CONTROL_P	q;
int			segs;
size_t			arensize;
_ST_ADDR_T		arenabase;
#endif
{
	q->_buckets_maximum = segs;
	q->_arena_size = arensize;
	q->_arena_base = (_ST_ADDR_T) arenabase;
	q->_arena_end = _ST_HEAP_NEXT (arenabase, arensize);

	q->_bucket_biggest = (_ST_WORD_T *) (q + 1);
	q->_bucket_first = (_ST_ADDR_T *) (q->_bucket_biggest + segs * 2);

	/*
	 * Perform regular initialisation.
	 */

	st_init (q);
}


/*
 * Return a size suitable for requesting the largest currently available
 * block of memory.
 *
 * Thanks to the way the index heap is constructed, this is in fact
 * trivially implementable.
 */

#if	__USE_PROTO__
size_t (st_maxavail) (_ST_HEAP_CONTROL_P q)
#else
size_t
st_maxavail __ARGS ((q))
_ST_HEAP_CONTROL_P	q;
#endif
{
	return _ST_WORD2BYTE (_ST_HEAP_BIGGEST (q, 0));
}
