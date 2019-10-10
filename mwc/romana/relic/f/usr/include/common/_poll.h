#ifndef	__COMMON__POLL_H__
#define	__COMMON__POLL_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "poll_t". This data type is not intended for public
 * use, and would not normally be visible; however, in order to maintain
 * binary and source compatibility with earlier releases of Coherent, this
 * type is imported into a structure whose definition was known to driver code
 * in earlier releases.
 *
 * Do not rely on the contents or even existence of this header across
 * releases of the Coherent operating system.
 */

/*
 * Polling is one of the few places using a circular list makes sense.
 * To keep the circular-list code sane, we still distinguish list head from
 * list node and put the first/next pointers in a special node item used by
 * both.
 */

typedef	struct pollnode	poll_t;

struct pollnode {
	poll_t	      *	pn_next;
	poll_t	      *	pn_prev;
};

#endif	/* ! defined (__COMMON__POLL_H__) */
