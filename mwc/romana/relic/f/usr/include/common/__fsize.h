#ifndef	__COMMON___FSIZE_H__
#define	__COMMON___FSIZE_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "__fsize_t". This type is intended to abstract the
 * notion of file size for Coherent utilities to aid in migration to file
 * systems capable of representing files more than 2^32-1 bytes in size.
 *
 * This definition is the basis of the Coherent data type "fsize_t", but given
 * an internal name so that headers may use this type internally without
 * exporting it to the user unnecessarily.
 *
 * Macros for accessing items of this type portably are not yet finalised.
 * Users of this type should be aware that it may not remain a scalar type.
 * Use of this type in portable software is strongly discouraged.
 */

typedef	unsigned long	__fsize_t;

#endif	/* ! defined (__COMMON___FSIZE_H__) */
