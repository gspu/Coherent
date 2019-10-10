/*
 * Implementation of functions prototyped in "buildobj.h"
 */
/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		USE_PROTO
 *		VOID
 *		ARGS ()
 *	<stdlib.h>
 *		NULL
 *		size_t
 *		free ()
 *		malloc ()
 *	<string.h>
 *		memcpy ()
 *		memset ()
 */

#include <stdio.h>
#include <sys/compat.h>
#include <stdlib.h>
#include <string.h>

#include "buildobj.h"


/*
 * Structures that are only really visible inside this compilation unit.
 *
 * Note that "o_prev" is used only for nested build contexts, which are
 * special, although it could be used for all objects if we wanted to spend
 * the space on allowing stacklike allocation (which we don't, normally).
 */

typedef	struct buildchunk	bchunk_t;
typedef	struct objinfo		obj_t;

struct objinfo {
	size_t	      *	o_prev;		/* previous object info */
	size_t	      *	o_base;		/* base of object */
	size_t	      *	o_last;		/* last object that was built */
};

#define	OBJ_OVERHEAD	(sizeof (size_t))

struct builder {
	bchunk_t      *	b_first;	/* first allocated chunk */
	bchunk_t      *	b_last;		/* last allocated chunk */
	size_t		b_chunksize;	/* recommended chunk size */
	size_t		b_align;	/* allocation alignment factor */

	obj_t		b_obj;		/* current object */
	bchunk_t      *	b_chunkp;	/* chunk of last/current object */
};

struct buildchunk {
	bchunk_t      *	bc_next;	/* next memory chunk */
	unsigned char *	bc_base;	/* base of chunk space */
	size_t		bc_free;	/* amount of free space */
};


/*
 * This function transfers the notion of "current" chunk to a different chunk,
 * transferring any partially built object along with it.
 */

#if	USE_PROTO
void (BUILD_SELECT) (build_t * heap, bchunk_t * bchunkp)
#else
void
BUILD_SELECT ARGS ((heap, bchunkp))
build_t	      *	heap;
bchunk_t      *	bchunkp;
#endif
{
	/*
	 * Copy any previously existing data to the new chunk.
	 */

	if (heap->b_obj.o_base != NULL) {
		size_t		size = * heap->b_obj.o_base;

		memcpy (bchunkp->bc_base, heap->b_obj.o_base, size);

		heap->b_chunkp->bc_base -= size;	/* free old space */
		heap->b_chunkp->bc_free += size;
		memset (heap->b_chunkp->bc_base, 0, size);

		heap->b_obj.o_base = (size_t *) bchunkp->bc_base;

		bchunkp->bc_base += size;
		bchunkp->bc_free -= size;
	} else
		heap->b_obj.o_base = (size_t *) bchunkp->bc_base;

	heap->b_chunkp = bchunkp;
}


/*
 * This internal function allocates a new chunk of memory to be parcelled out
 * to clients. Allocate "b_chunksize" bytes of memory to give out, and copy
 * any partial allocation over to the new chunk (since we typically only
 * allocate new chunks when a request to expand a previous allocation
 * overflows the biggest available block of space).
 */

#if	USE_PROTO
bchunk_t * (BUILD_CHUNK) (build_t * heap, size_t required)
#else
bchunk_t *
BUILD_CHUNK ARGS ((heap, required))
build_t	      *	heap;
size_t		required;
#endif
{
	bchunk_t      *	bchunkp;

	/*
	 * Ok, let's add a new chunk.
	 */

	if ((required += required >> 1) > heap->b_chunksize)
		heap->b_chunksize = required;

#if	MALLOC_TRACE
	fprintf (stderr, "<%d> mallocing %d bytes\n", getpid (),
		 heap->b_chunksize + sizeof (bchunk_t));
#endif
	if ((bchunkp = (bchunk_t *) malloc (heap->b_chunksize +
					    sizeof (bchunk_t))) == NULL)
		return NULL;

	bchunkp->bc_next = NULL;

	if (heap->b_first == NULL)
		 heap->b_first = bchunkp;
	else
		heap->b_last->bc_next = bchunkp;

	heap->b_last = bchunkp;
	bchunkp->bc_base = (unsigned char *) (bchunkp + 1);
	bchunkp->bc_free = heap->b_chunksize;

	/*
	 * Make the new chunk the current chunk.
	 */

	BUILD_SELECT (heap, bchunkp);
	return bchunkp;
}


/*
 * This internal function increases the size of an allocation. If a request
 * causes an allocation to expand beyond the end of the current chunk, try and
 * find another chunk that can hold it, and if that fails allocate a new chunk
 * which is guaranteed to be large enough.
 *
 * The client's data is copied to the new space, appended to any previously
 * allocated data in the same group.
 */

#if	USE_PROTO
builderr_t (BUILD_ADD) (build_t * heap, size_t size, CONST VOID * init)
#else
builderr_t
BUILD_ADD ARGS ((heap, size, init))
build_t	      *	heap;
size_t		size;
CONST VOID    *	init;
#endif
{
	bchunk_t      *	bchunkp;
	size_t		total;

	if ((total = size) == 0)
		return BUILD_OK;

	if (heap->b_obj.o_base != NULL)
		total += * heap->b_obj.o_base;

	if (total < size || total + sizeof (bchunk_t) < total)
		return BUILD_SIZE_OVERFLOW;

	/*
	 * First, try to fit the expanded allocation in the current chunk.
	 * If there are no chunks allocated yet, let's start.
	 */

	if ((bchunkp = heap->b_chunkp) == NULL || bchunkp->bc_free < size) {
		/*
		 * First, try and find an already allocated chunk that will
		 * hold the expanded allocation.
		 */

		while (bchunkp != NULL) {

			if (bchunkp->bc_free >= total) {

				BUILD_SELECT (heap, bchunkp);
				goto gotmem;
			}

			bchunkp = bchunkp->bc_next;
		}

		if ((bchunkp = BUILD_CHUNK (heap, total)) == NULL)
			return BUILD_NO_MEMORY;
	}

	if (heap->b_obj.o_base == NULL)
		heap->b_obj.o_base = (size_t *) bchunkp->bc_base;

gotmem:
	/*
	 * OK, let's initialise the new space.
	 */

	if (init == INIT_ZERO) {
		/*
		 * The client passed us the special pointer value that means
		 * we should zero the new space.
		 */

		memset (bchunkp->bc_base, 0, size);
	} else if (init != NULL) {
		/*
		 * The client has given us explicit initialisation values.
		 */

		memcpy (bchunkp->bc_base, init, size);
	}

	bchunkp->bc_base += size;
	bchunkp->bc_free -= size;
	(* heap->b_obj.o_base) += size;

	return BUILD_OK;
}


/*
 * At some stage we need to deal with alignment issues resulting from a chunk
 * allocation by rounding off the base and free size of a chunk's remaining
 * space. At present, we do this when we are about to build a new object,
 * since only then are we totally sure that we are finished with the old one.
 */

#if	USE_PROTO
builderr_t (BUILD_ALIGN) (build_t * heap)
#else
builderr_t
BUILD_ALIGN ARGS ((heap))
build_t	      *	heap;
#endif
{
	if (heap->b_chunkp != NULL) {
		size_t		adjust;

		adjust = heap->b_chunkp->bc_free & (heap->b_align - 1);

		/*
		 * Waste some space at the end to make the alignment happen.
		 */

		if (adjust) {
			adjust = heap->b_align - adjust;

			heap->b_chunkp->bc_base += adjust;
			heap->b_chunkp->bc_free -= adjust;
		}
	}

	/*
	 * Since we do this only when we are about to do something new, set up
	 * for a new allocation.
	 */

	heap->b_chunkp = heap->b_first;
	heap->b_obj.o_base = NULL;
	heap->b_obj.o_last = NULL;

	return BUILD_ADD (heap, OBJ_OVERHEAD, INIT_ZERO);
}


/*
 * Local function which recovers a chunk number from an object address. Note
 * that we *cannot* do this by merely scanning the chunks looking for one that
 * encloses the object pointer we have, because relational comparisons between
 * distinctly-allocated objects yields an undefined result (ie, we could get
 * false positives).
 *
 * However, we can solve the problem by requiring the object pointer to be the
 * most recent allocation in a given chunk, and passing in the size of the
 * object. This way, we can test for a match via equality, which is a stronger
 * test, because two object pointers compare equal if and only if they point
 * at the same object.
 *
 * [ Note that the converse is controversial; it does not appear to be
 *   mandated by ISO C, and the notion of object identity is hotly debated
 *   in C++ circles. Since we will be using pointers that have been derived
 *   via convential means we can ignore such notions of aliasing. ]
 *
 * In order to deal with alignment, we have changed this function a little; it
 * recovers a chunk address for the given object and makes it current, undoing
 * any alignment done to the chunk since the object in question was left
 * there. Return -1 on failure, 0 on success.
 */

#if	USE_PROTO
int (FIND_CHUNK) (build_t * heap, size_t * obj)
#else
int
FIND_CHUNK ARGS ((heap, obj))
build_t	      *	heap;
size_t	      *	obj;
#endif
{
	bchunk_t      *	scan;

	for (scan = heap->b_first ; scan != NULL ; scan = scan->bc_next) {
		size_t	      *	temp;
		size_t		adjust;

		/*
		 * When we do this we have to watch out for the fact that a
		 * chunk may or may not have been aligned. If the chunk has
		 * been aligned, dealign it.
		 */

		adjust = (scan->bc_free + * obj) & (heap->b_align - 1);
		temp = (size_t *) (scan->bc_base - * obj - adjust);

		if (temp == obj && * temp == * obj) {
			heap->b_chunkp = scan;
			scan->bc_free += adjust;
			scan->bc_base -= adjust;
			return 0;
		}
	}

	return -1;
}


#if	0
/*
 * This function dumps a variety of information about a heap.
 */

#if	USE_PROTO
void (build_dump) (build_t * heap)
#else
void
build_dump ARGS ((heap))
build_t	      *	heap;
#endif
{
	bchunk_t      *	scan;

	printf ("Heap b_first = %x b_last = %x chunksize = %d align = %d\n",
		heap->b_first, heap->b_last, heap->b_chunksize, heap->b_align);

	for (scan = heap->b_first ; scan != NULL ; scan = scan->bc_next) {
		size_t	      *	obj = (size_t *) (scan + 1);

		printf ("Chunk at %x, %x free:\n", scan, scan->bc_free);
		while ((unsigned char *) obj < scan->bc_base) {
			printf ("  Object at %x, size %d\n", obj, * obj);
			obj = (size_t *) ((unsigned char *) obj + * obj);
		}
	}
}
#endif


/*
 * This function allocates a control block for building variable-sized
 * objects with. The "chunksize" is the default amount of memory to request
 * from the C library in a block which other functions will parcel out to
 * clients.
 */

#if	USE_PROTO
build_t * (builder_alloc) (size_t chunksize, size_t align)
#else
build_t *
builder_alloc ARGS ((chunksize, align))
size_t		chunksize;
size_t		align;
#endif
{
	build_t	      *	buildp;

	if (chunksize < 256)
		chunksize = 256;


#if	MALLOC_TRACE
	fprintf (stderr, "<%d> mallocing %d bytes\n", getpid (),
		 sizeof (build_t));
#endif
	if ((buildp = (build_t *) malloc (sizeof (build_t))) != NULL) {

		buildp->b_chunkp = buildp->b_first = NULL;
		buildp->b_align = align == 0 ? 1 : align;
		buildp->b_chunksize = chunksize & ~ (buildp->b_align - 1);

		buildp->b_obj.o_prev = NULL;	/* no previous object */
		buildp->b_obj.o_base = NULL;	/* not building an object */
	}

	return buildp;
}


/*
 * Free the basic object builder structure and all the chunk memory that it
 * attached to itself during its lifetime.
 */

#if	USE_PROTO
void (builder_free) (build_t * heap)
#else
void
builder_free ARGS ((heap))
build_t	      *	heap;
#endif
{
	bchunk_t      *	scan;
	bchunk_t      *	next;

	if (heap == NULL)
		return;

	for (scan = heap->b_first ; scan != NULL ; scan = next) {

		next = scan->bc_next;
#if	MALLOC_TRACE
		fprintf (stderr, "<%d> freeing chunk\n", getpid ());
#endif
		free (scan);
	}

#if	MALLOC_TRACE
	fprintf (stderr, "<%d> freeing control block\n", getpid ());
#endif
	free (heap);
}


/*
 * Simple function to allocate just a single chunk.
 */

#if	USE_PROTO
VOID * (build_malloc) (build_t * heap, size_t size, CONST VOID * init)
#else
VOID *
build_malloc ARGS ((heap, size, init))
build_t	      *	heap;
size_t		size;
CONST VOID    *	init;
#endif
{
	if (heap == NULL || heap->b_obj.o_base != NULL ||
	    (heap->b_obj.o_last != NULL && heap->b_obj.o_prev != NULL))
		return NULL;

	if (BUILD_ALIGN (heap) != BUILD_OK ||
	    BUILD_ADD (heap, size, init) != BUILD_OK)
		return NULL;

	heap->b_obj.o_last = heap->b_obj.o_base;
	heap->b_obj.o_base = NULL;

	return (VOID *) (heap->b_obj.o_last + 1);
}


/*
 * Begin building some variable-sized object with an initial allocation of
 * "size" bytes. All previous allocations must be complete before beginning a
 * new allocation.
 */

#if	USE_PROTO
builderr_t (build_begin) (build_t * heap, size_t size, CONST VOID * init)
#else
builderr_t
build_begin ARGS ((heap, size, init))
build_t	      *	heap;
size_t		size;
CONST VOID    *	init;
#endif
{
	int		error;

	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (heap->b_obj.o_base != 0)
		return BUILD_OBJECT_BEGUN;

	/*
	 * We only allow one object to be built per inner nesting level; since
	 * object descriptors are normally not needed, levels that can be
	 * unwound have to be pushed/popped manually. However, only one object
	 * can be built at an inner level or else the stack could not be
	 * unwound.
	 */

	if (heap->b_obj.o_last != NULL && heap->b_obj.o_prev != NULL)
		return BUILD_BAD_NESTING;

	if ((error = BUILD_ALIGN (heap)) != BUILD_OK)
		return error;

	return BUILD_ADD (heap, size, init);
}


/*
 * Add "size" bytes to the current variable-sized object.
 */

#if	USE_PROTO
builderr_t (build_add) (build_t * heap, size_t size, CONST VOID * init)
#else
builderr_t
build_add ARGS ((heap, size, init))
build_t	      *	heap;
size_t		size;
CONST VOID    *	init;
#endif
{
	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (heap->b_obj.o_base == NULL)
		return BUILD_NO_OBJECT;

	return BUILD_ADD (heap, size, init);
}


/*
 * Special-case function to add a single character to an object efficiently.
 * This is useful for functions which must build strings on the fly without
 * any easy way of predetermining the size.
 */

#if	USE_PROTO
builderr_t (build_addchar) (build_t * heap, unsigned char ch)
#else
builderr_t
build_addchar ARGS ((heap, ch))
build_t	      *	heap;
unsigned char	ch;
#endif
{
	bchunk_t      *	bchunkp;

	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (heap->b_obj.o_base == NULL)
		return BUILD_NO_OBJECT;

	/*
	 * Like BUILD_ADD (), but faster in the simplest case.
	 */

	if ((bchunkp = heap->b_chunkp) == NULL || bchunkp->bc_free == 0) {
		bchunkp = BUILD_CHUNK (heap, * heap->b_obj.o_base + 1);
		if (bchunkp == NULL)
			return BUILD_NO_MEMORY;
	}

	* bchunkp->bc_base ++ = ch;
	bchunkp->bc_free --;
	(* heap->b_obj.o_base) ++;

	return BUILD_OK;
}


/*
 * Reduce the space allocated to an object.
 */

#if	USE_PROTO
builderr_t (build_reduce) (build_t * heap, size_t size)
#else
builderr_t
build_reduce ARGS ((heap, size))
build_t	      *	heap;
size_t		size;
#endif
{
	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (heap->b_obj.o_base == NULL)
		return BUILD_NO_OBJECT;
	if (size + OBJ_OVERHEAD > * heap->b_obj.o_base)
		return BUILD_SIZE_OVERFLOW;

	heap->b_chunkp->bc_base -= size;
	heap->b_chunkp->bc_free += size;
	* heap->b_obj.o_base -= size;

	return BUILD_OK;
}


/*
 * Abandon construction of an object.
 */

#if	USE_PROTO
builderr_t (build_abandon) (build_t * heap)
#else
builderr_t
build_abandon ARGS ((heap))
build_t	      *	heap;
#endif
{
	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (heap->b_obj.o_base == NULL)
		return BUILD_NO_OBJECT;

	heap->b_chunkp->bc_base -= * heap->b_obj.o_base;
	heap->b_chunkp->bc_free += * heap->b_obj.o_base;

	heap->b_obj.o_base = NULL;
	return BUILD_OK;
}


/*
 * End construction of an object, optionally returning the size in bytes
 * occupied.
 */

#if	USE_PROTO
VOID * (build_end) (build_t * heap, size_t * size)
#else
VOID *
build_end ARGS ((heap, size))
build_t	      *	heap;
size_t	      *	size;
#endif
{
	if (heap == NULL || heap->b_obj.o_base == NULL)
		return NULL;

	if (size != NULL)
		* size = * heap->b_obj.o_base - OBJ_OVERHEAD;

	heap->b_obj.o_last = heap->b_obj.o_base;
	heap->b_obj.o_base = NULL;

	return (VOID *) (heap->b_obj.o_last + 1);
}


/*
 * Resume construction of the last built object.
 */

#if	USE_PROTO
builderr_t (build_resume) (build_t * heap)
#else
builderr_t
build_resume ARGS ((heap))
build_t	      *	heap;
#endif
{
	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (heap->b_obj.o_base != NULL)
		return BUILD_OBJECT_BEGUN;
	if (heap->b_obj.o_last == NULL)
		return BUILD_NOT_LAST;

	heap->b_obj.o_base = heap->b_obj.o_last;
	heap->b_obj.o_last = NULL;

	return BUILD_OK;
}


/*
 * Return the storage of the most recently constructed object to the free
 * pool. Note that the GNU obstack system allows an object heap to be cut back
 * to any arbitrary point in a stack-like fashion; we don't, because we try
 * and squeeze space into end-of-chunk areas that the GNU system would leave
 * alone. Perhaps later we can add options to allow this.
 */

#if	USE_PROTO
builderr_t (build_release) (build_t * heap, CONST VOID * base)
#else
builderr_t
build_release ARGS ((heap, base))
build_t	      *	heap;
CONST VOID    *	base;
#endif
{
	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (base == NULL)
		return BUILD_NULL_BASE;
	if (heap->b_obj.o_base != NULL)
		return BUILD_OBJECT_BEGUN;
	if ((VOID *) (heap->b_obj.o_last + 1) != base)
		return BUILD_NOT_LAST;

	/*
	 * Return object's space to the chunk that the last object was
	 * constructed in. We also forget that the object was ever built,
	 * which will allow nested builds to happen.
	 */

	heap->b_chunkp->bc_base -= * heap->b_obj.o_last;
	heap->b_chunkp->bc_free += * heap->b_obj.o_last;
	heap->b_obj.o_last = NULL;

	return BUILD_OK;
}


/*
 * Temporarily suspend construction of an object and begin nested construction
 * of another object. Once a nested build context has been entered, only one
 * object is allowed to be built (although it may be thrown away and another
 * one begun).
 */

#if	USE_PROTO
builderr_t (build_push) (build_t * heap, VOID * data, size_t size)
#else
builderr_t
build_push ARGS ((heap, data, size))
build_t	      *	heap;
VOID	      *	data;
size_t		size;
#endif
{
	obj_t		temp;
	int		err;

	if (heap == NULL)
		return BUILD_NULL_HEAP;

	/*
	 * We will need to find some space in the heap for the saved build
	 * context. We take a snapshot of the allocation context in a local
	 * variable for eventual storage in the heap, and start a "new"
	 * allocation. Then, calling BUILD_ADD () will find space for the
	 * snapshot and copy it.
	 */

	temp = heap->b_obj;

	if ((err = BUILD_ALIGN (heap)) != BUILD_OK ||
	    (err = BUILD_ADD (heap, sizeof (temp) + size, NULL)) != BUILD_OK) {
		/*
		 * We couldn't create space for the saved context, so restore
		 * the state and return the error to the caller.
		 */

		heap->b_obj = temp;
		(void) FIND_CHUNK (heap, heap->b_obj.o_base);
	} else {
		/*
		 * Copy our own save data plus the user-specified data into
		 * the save record we have allocated. We record the size of
		 * the user data as well.
		 */

		* (obj_t *) (heap->b_obj.o_base + 1) = temp;
		if (size > 0 && data != NULL) {
			memcpy ((unsigned char *) heap->b_obj.o_base +
				OBJ_OVERHEAD + sizeof (obj_t), data, size);
		}


		/*
		 * Record the location where the saved snapshot was stored as
		 * the back-link in the snapshot chain. Then, we clear the
		 * top object record for use in at most one nested allocation.
		 */

		heap->b_obj.o_prev = heap->b_obj.o_base;
		heap->b_obj.o_last = heap->b_obj.o_base = NULL;
	}

	return err;
}


/*
 * Undo the actions of build_push (). If there is an allocation in the current
 * top-level object record, discard it implicitly.
 *
 * The user can also request the retrieval of data stored when the context was
 * saved, if "data" and "sizep" are both non-NULL.
 */

#if	USE_PROTO
builderr_t (build_pop) (build_t * heap, VOID * data, size_t * sizep)
#else
builderr_t
build_pop ARGS ((heap, data, sizep))
build_t	      *	heap;
VOID	      *	data;
size_t	      *	sizep;
#endif
{
	size_t		size;

	if (heap == NULL)
		return BUILD_NULL_HEAP;
	if (heap->b_obj.o_prev == NULL)
		return BUILD_STACK_EMPTY;

	/*
	 * Implicitly discard top object.
	 */

	if (heap->b_obj.o_base != NULL) {
		heap->b_chunkp->bc_base -= * heap->b_obj.o_base;
		heap->b_chunkp->bc_free += * heap->b_obj.o_base;
	}
	if (heap->b_obj.o_last != NULL) {
		heap->b_chunkp->bc_base -= * heap->b_obj.o_last;
		heap->b_chunkp->bc_free += * heap->b_obj.o_last;
	}

	
	/*
	 * Before we retrieve the previous object context, we have to find
	 * the chunk it is stored in (so that we can discard the saved
	 * context record).
	 */

	size = * heap->b_obj.o_prev - (sizeof (obj_t) + OBJ_OVERHEAD);

	if (size > 0) {
		/*
		 * Perhaps retrieve the saved user data.
		 */

		if (sizep == NULL || * sizep < size)
			return BUILD_TOO_BIG;
		* sizep = size;

		if (data != NULL) {
			memcpy (data, (unsigned char *) heap->b_obj.o_prev +
				      sizeof (obj_t) + OBJ_OVERHEAD, size);
		}
	}

	if (FIND_CHUNK (heap, heap->b_obj.o_prev) != 0)
		return BUILD_CORRUPT;

	/*
	 * Free saved record and recover the data stored there.
	 */

	size = * heap->b_obj.o_prev;
	heap->b_obj = * (obj_t *) (heap->b_obj.o_prev + 1);
	heap->b_chunkp->bc_base -= size;
	heap->b_chunkp->bc_free += size;

	
	/*
	 * We don't store the previous chunk pointer in the object record
	 * because we can recover it easily if necessary (and it often isn't).
	 */

	if (heap->b_obj.o_base != NULL) {
		if (FIND_CHUNK (heap, heap->b_obj.o_base) != 0)
			return BUILD_CORRUPT;
	} else
		heap->b_chunkp = NULL;

	return 0;
}


/*
 * Return a human-readable string from an error code.
 */

#if	USE_PROTO
CONST char * (build_error) (builderr_t errcode)
#else
CONST char *
build_error ARGS ((errcode))
builderr_t	errcode;
#endif
{
	static CONST char * errors [] = {
		/*
		 * Error strings to match the error enumeration, from 0 to
		 * the maximum negative error code, plus an extra code for
		 * invalid error codes.
		 */

		"no error",
		"\"heap\" parameter is NULL",
		"\"base\" parameter is NULL",
		"object already under construction",
		"object construction not begun",
		"insufficient memory to satisfy request",
		"overflow of \"size_t\" would result",
		"not most recently built object",
		"can build only one object in inner nesting level",
		"pop of empty saved object stack",
		"heap appears to be corrupt",
		"user data bigger than indicated maximum",
		"<not a valid error>"
	};

#define	ARRAY_LEN(a)	(sizeof (a) / sizeof (* (a)))

	return (errcode > 0 || - errcode >= ARRAY_LEN (errors) ?
			errors [ARRAY_LEN (errors) - 1] : errors [- errcode]);
}


/*
 * Return the current size of the current object; this size is suitable for
 * use in recording subsection positions within structured data (since an
 * object may move while being built, a relative address is more suitable than
 * an absolute one).
 */

#if	__USE_PROTO__
size_t (build_offset) (CONST build_t * heap)
#else
size_t
build_offset __ARGS ((heap))
CONST build_t *	heap;
#endif
{
	return heap == NULL || heap->b_obj.o_base == NULL ?
			0 : * heap->b_obj.o_base - OBJ_OVERHEAD;
}
