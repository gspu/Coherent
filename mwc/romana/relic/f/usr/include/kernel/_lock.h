#ifndef	__KERNEL__LOCK_H__
#define	__KERNEL__LOCK_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "__lock_t", exactly equivalent to the basic lock
 * type "lock_t", but given an internal name so that header files may refer to
 * this type without exporting the type name into the user's namespace.
 */

typedef struct __basic_lock	__lock_t;


/*
 * Range of valid lock hierarchy values. Since DDI/DKI library routines often
 * need locks of their own, we also define hierarchy values here that are
 * suitable for those other operations and which are greater than any
 * hierarchy value we will permit a driver.
 *
 * The hierarchy range is here because it is fixed, and because it may be
 * convenient to use the following definitions as the basis of other
 * definitions within kernel headers without importing <sys/ksynch.h>
 *
 * Note that the maximum values are inclusive.
 */

enum {
	__MIN_HIERARCHY__ = 1,
	__MAX_DDI_HIERARCHY__ = 32,
	__MAX_HIERARCHY__ = 40
};


/*
 * The official DDI/DKI type of a lock hierarchy value is "unsigned char". To
 * make things clearer in function prototypes, we define our own internal type
 * for lock hierarchy values.
 */

typedef	unsigned char		__lkhier_t;

#endif	/* ! defined (__KERNEL__LOCK_H__) */
