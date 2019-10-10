#ifndef	BUILDOBJ_H
#define	BUILDOBJ_H

/*
 * This file declares structures and function prototypes for some routines
 * useful for building up objects a piece at a time.
 */
/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		VOID
 *		PROTO ()
 *	<stddef.h>
 *		size_t
 */

#include <sys/compat.h>
#include <stddef.h>

/*
 * The following code can be used as the basis of a system for building up
 * objects such as symbol-table entries incrementally. It's kind of like the
 * GNU obstack system (and clearly inspired by it), but it's not free
 * software. It's also a little more clearly laid out, and easier to
 * understand and modify, and slower.
 *
 * The simple flow of usage for this library appears like this, with
 * ... indicating that the item or block above may be repeated.
 *	builder_alloc ()
 *		build_begin ()
 *			build_addchar ()
 *			...
 *			build_add ()
 *			...
 *		build_end ()
 *		...
 *	builder_free ()
 *
 * As a general convention, functions that return pointer values will return
 * the NULL pointer to indicate some kind of failure (an inability to comply
 * with the request due to system resource shortages or incorrect parameter
 * values for the request). Otherwise, functions return an object of type
 * builderr_t to describe the success or failure of the request. The function
 * build_error () can be used to obtain a character pointer that can be used
 * to describe the error in human-readable form.
 *
 * An additional convention relates to user data pointers passed to the
 * build_begin (), build_add (), and build_malloc () functions, which may be
 * passed either a NULL pointer (indicating that the memory allocated to the
 * object should be left uninitialized), the special value INIT_ZERO (which
 * indicates that the allocated memory should be cleared), or any other
 * pointer value which is assumed to point at data to initialize the allocated
 * space with.
 *
 * For any function in this library that accepts an allocation anchor as an
 * argument, passing a pointer that is NULL or was not allocated using the
 * builder_alloc () function results in undefined behaviour.
 *
 * Basic functions:
 *
 * builder_alloc () allocates the anchoring data structure which maintains
 * information about the allocations, including the state of any object being
 * built and the location of all the memory allocated to objects that have
 * been built using this anchor. The user-supplied alignment restriction is
 * given here so that it may be globally applied to all total object
 * allocations; that is, the starting address of any object returned via
 * build_end () is guaranteed to be aligned as strictly as requested. However,
 * internal object alignment is not maintained automatically, and the user of
 * the library must ensure that data is added to objects in such as way as not
 * to violate system alignment restrictions.
 *
 * builder_free () deallocates the anchoring data structure, discarding any
 * partially built object and releasing the memory allocated for all of the
 * objects that were built using the particular anchor. This means that all
 * pointers returned by successful calls to build_end () with this anchor may
 * no longer be used to reference memory.
 *
 * build_begin () initiates construction of an object, associating it with a
 * particular anchor. State information is maintained and checked so that this
 * routine can only be used successfully if there is no object currently being
 * constructed, i.e. build_begin () will not initiate construction of nested
 * objects.
 *
 * build_end () finishes construction of an object, returning the base address
 * of the memory allocated to the object, and optionally the length of the
 * object. State information is maintained and checked so that this routine
 * can only be used successfully after object construction has been properly
 * initiated, e.g., using build_begin (). Note in particular that objects of
 * length 0 can be created, but distinctness of the resulting return value in
 * that special case is not guaranteed.
 *
 * build_add () and build_addchar () provide data which is concatenated to any
 * previous data supplied for the object currently being built. These functions
 * can only be successfully called if object construction has been properly
 * initiated, e.g. using build_begin ().
 *
 * Intermediate functions:
 *
 * build_malloc () combines the effects of build_begin () and build_end ()
 * into a single function useful for allocating and optionally initializing
 * an object in storage tracked by the anchor.
 *
 * build_abandon () abandons construction of the current object. This function
 * can only be successfully called if object construction has been properly
 * initiated, e.g. using build_begin (). Once abandoned, the anchor is in a
 * state where build_begin () may be successfully called.
 *
 * build_resume () allows object construction to be resumed following a call
 * to build_end (), as if build_end () was never called. This function can
 * only be called successfully if no other functions modify the state of the
 * anchor between the call to build_end () and the call to build_resume ().
 * Once this function is called, the object base address returned by
 * build_end () will remain valid only so long as the anchor is not used with
 * any of these library functions, including build_push ().
 *
 * build_reduce () reduces the amount of memory currently allocated to an
 * object by the indicated number of bytes. This function can only be called
 * successfully if object construction has been properly initiated, e.g. using
 * build_begin ().
 *
 * build_release () requests that the memory allocated to the object whose
 * address is passed by the user be released. At present, this function will
 * only succeed if the object address passed is the most recent return value
 * from build_end (), and no other function has been called to modify the
 * state of the anchor. It may be that future versions of this system support
 * an allocation policy allowing stack-like behaviour where all allocations
 * after a particular one may be released, but this is not supported by the
 * current policy. If the object address passed to this function is not one
 * that has been returned from build_end () for an object allocation on the
 * same anchor passed to build_release (), then the behaviour is undefined.
 *
 * build_offset () returns the current size of the object under construction.
 * If there is no object currently under construction, return 0.
 *
 * Advanced functions:
 *
 * build_push () constructs a nested allocation context on an anchor; if any
 * object construction is under way, it is suspended. The anchor is placed in
 * a state where the build_begin () function may be called successfully. In
 * addition to creating a context where construction of a nested object can
 * be successfully begun, the build_push () function will optionally accept
 * additional user data which is recorded such that a corresponding call to
 * build_pop () can retrieve the data. Under the current allocation policy,
 * only one complete object may be constructed in the nested context created
 * by a call to build_push (), i.e. after a successful call to build_end ()
 * in a nested context, build_begin () will fail. However, calls to the
 * build_push () function may be nested arbitrarily with no restriction beyond
 * system memory limits.
 *
 * build_pop () discards the nested context created by the corresponding call
 * to build_push (), implicitly abandoning any objects that were partially or
 * completely constructed in the nested context and restoring the state of the
 * anchor to that prior to the corresponding build_push () call. Optionally,
 * the user may retrieve any data stored with the build_push () call by
 * providing a data buffer and a length. Note that the length of the buffer
 * provided to build_pop () is given via a pointer which is used to overwrite
 * the retrieval buffer length with the length of the data actually stored by
 * the corresponding call to build_push ().
 */

#ifndef	BUILD_T
#define	BUILD_T
typedef struct builder		build_t;
#endif


/*
 * Return values from build functions.
 */

typedef	int		builderr_t;

enum {
	BUILD_TOO_BIG = -11,
	BUILD_CORRUPT = -10,
	BUILD_STACK_EMPTY = -9,
	BUILD_BAD_NESTING = -8,
	BUILD_NOT_LAST = -7,
	BUILD_SIZE_OVERFLOW = -6,
	BUILD_NO_MEMORY = -5,
	BUILD_NO_OBJECT = -4,
	BUILD_OBJECT_BEGUN = -3,
	BUILD_NULL_BASE = -2,
	BUILD_NULL_HEAP = -1,
	BUILD_OK
};

/*
 * Special value for "init" parameter in functions below that causes newly
 * allocated memory to be cleared.
 */

#define	INIT_ZERO	((VOID *) 1)


EXTERN_C_BEGIN

build_t	      *	builder_alloc	PROTO ((size_t _chunksize, size_t _align));
void		builder_free	PROTO ((build_t * _heap));

VOID	      *	build_malloc	PROTO ((build_t * _heap, size_t _size,
					CONST VOID * _init));

builderr_t	build_begin	PROTO ((build_t * _heap, size_t _size,
					CONST VOID * _init));
builderr_t	build_add	PROTO ((build_t * _heap, size_t _size,
					CONST VOID * _init));
builderr_t	build_addchar	PROTO ((build_t * _heap, unsigned char _ch));
VOID	      *	build_end	PROTO ((build_t * _heap, size_t * _size));
builderr_t	build_reduce	PROTO ((build_t * _heap, size_t _size));
builderr_t	build_release	PROTO ((build_t * _heap, CONST VOID * _base));
builderr_t	build_abandon	PROTO ((build_t * _heap));
builderr_t	build_resume	PROTO ((build_t * _heap));

CONST char    *	build_error	PROTO ((builderr_t _errcode));

builderr_t	build_push	PROTO ((build_t * _heap, VOID * _data,
					size_t _size));
builderr_t	build_pop	PROTO ((build_t * _heap, VOID * _data,
					size_t * _sizep));

size_t		build_offset	PROTO ((CONST build_t * _heap));
void		build_dump	PROTO ((build_t * _heap));

EXTERN_C_END

#endif	/* ! defined (BUILDOBJ_H) */
