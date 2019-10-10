/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_X86LOCK_H__
#define	__KERNEL_X86LOCK_H__

/*
 * Here we encapsulate all the system-specific basics for dealing with multi-
 * and uni-processor locking in an i386 environment. Note that the functions
 * defined here for the user have names that are too long for common ISO C
 * external linkage limitations; this should not be an obstacle, since the
 * functions defined here will normally have internal linkage.
 *
 * Note that none of the functions here are defined in the DDI/DKI. This
 * header is purely for internal purposes and for this reason does not follow
 * the usual DDI/DKI header binary-compatibility structure with external-
 * linkage versions always available.
 */

#include <common/ccompat.h>
#include <common/xdebug.h>
#include <common/__types.h>
#include <kernel/__pl.h>


/*
 * On the 386, there are no "compare and swap" instructions as in the 68020
 * CPU. Unfortunately, this deficiency makes some of the more advanced
 * synchronization algorithms unavailable (and introduces locking requirements
 * where no locking is needed in the 68020).
 *
 * The facilities that do exist are an atomic fetch-and-set via XCHG, and
 * atomic arithmetic via the LOCK instruction prefix. Neither facility is
 * directly available in C, so this header file encapsulates the system-
 * specific methods for gaining access to these atomic operations.
 *
 * We define a wide range of atomic data types, so that in cases where a
 * machine might not be able to support a high-level atomic operation on a
 * data type it will be possible to build the high-level operation out of
 * primitive locking facilities like test-and-set.
 *
 * We use the single-element-array declarations for two reasons: 1) we may
 * as well have implicit reference-passing semantics, and 2) it seems like
 * the easiest way (until C++, at least) of preventing the accidental misuse
 * of data items of these types.
 *
 * Each atomic type is guaranteed to have the same range as the <limits.h>
 * header specifies, but may be considerably larger (and have different
 * alignment requirements) than objects of the base non-atomic type.
 *
 * Dealing with atomic pointers: with C++, there is currently no guarantee
 * equivalent to the ISO C requirement that any pointer type can be converted
 * to and from a value of type "void *" successfully without loss of
 * information. For now, we operate without this guarantee rather than
 * introducing possibly unnecessary complexity via a "generic" structure-
 * pointer type. In point of fact, we don't support the only known case where
 * this doesn't work anyway (Borland C++ medium model, SS != DS) since we
 * assume SS = DS in those small-data models for simplicity anyway.
 *
 * To make things easier for the Borland C++, all the functions defined here
 * are permitted to evaluate their "lock" argument multiple times, which is
 * why the names have been changed to all upper-case.
 */

/*
 * The split between the user-visible (implicit reference) and internal
 * (volatile, decayed pointer) types was effectively forced by differences
 * in the way the available translators dealt with type-qualifiers and casts
 * to values of array type.
 *
 * As it happens, this split appears to have produced the most aesthetically
 * pleasing code forms, much to my surprise, as well as pointing up a lurking
 * problem (a misplaced "volatile") in the treatment of atomic_ptr_t.
 */

typedef signed char			atomic_char_t [1];
typedef	short				atomic_short_t [1];
typedef	int				atomic_int_t [1];
typedef	long				atomic_long_t [1];

typedef	__uchar_t			atomic_uchar_t [1];
typedef __ushort_t			atomic_ushort_t [1];
typedef __uint_t			atomic_uint_t [1];
typedef __ulong_t			atomic_ulong_t [1];

typedef	__VOID__		      *	atomic_ptr_t [1];

typedef	__VOLATILE__ signed char      *	__atomic_char_t;
typedef	__VOLATILE__ __uchar_t	      *	__atomic_uchar_t;
typedef	__VOLATILE__ short	      *	__atomic_short_t;
typedef	__VOLATILE__ __ushort_t	      *	__atomic_ushort_t;
typedef	__VOLATILE__ int	      *	__atomic_int_t;
typedef	__VOLATILE__ __uint_t	      *	__atomic_uint_t;
typedef	__VOLATILE__ long	      *	__atomic_long_t;
typedef	__VOLATILE__ __ulong_t	      *	__atomic_ulong_t;
typedef	__VOID__       * __VOLATILE__ *	__atomic_ptr_t;


/*
 * Here we define C-linkage prototypes for the functions that we want to
 * define (possibly as macros) here, so that it's easy to suppress any macro
 * definitions given below and get full type-checking even for compilers that
 * need macros to get the inline versions.
 *
 * Note that while ATOMIC_TEST_AND_SET_UCHAR has () functionality that is
 * completely availble through ATOMIC_FETCH_AND_STORE_UCHAR (), we define it
 * as a separate function so as not to overly constrain implementations on
 * machines such as the 60820, which has the powerful "compare and swap"
 * operation and the basic "test and set operation", but no basic exchange
 * (whereas the 88100 implements only exchange, and only on bytes and 32-bit
 * long words).
 *
 * Being verbose here is a small price to pay for the later flexibility.
 */


#if	! (__GNUC__ && (defined (i386) || _I386))

__EXTERN_C_BEGIN__

char	   ATOMIC_FETCH_AND_STORE_CHAR	__PROTO ((__atomic_char_t _lock,
						  char _newvalue));
__uchar_t  ATOMIC_FETCH_AND_STORE_UCHAR	__PROTO ((__atomic_uchar_t _lock,
						  __uchar_t _newvalue));
short	   ATOMIC_FETCH_AND_STORE_SHORT	__PROTO ((__atomic_short_t _lock,
						  short _newvalue));
__ushort_t ATOMIC_FETCH_AND_STORE_USHORT __PROTO ((__atomic_ushort_t _lock,
						   __ushort_t _newvalue));
int	   ATOMIC_FETCH_AND_STORE_INT	__PROTO ((__atomic_int_t _lock,
						  int _newvalue));
__uint_t   ATOMIC_FETCH_AND_STORE_UINT	__PROTO ((__atomic_uint_t _lock,
						  __uint_t _newvalue));
long	   ATOMIC_FETCH_AND_STORE_LONG	__PROTO ((__atomic_long_t _lock,
						  long _newvalue));
__ulong_t  ATOMIC_FETCH_AND_STORE_ULONG	__PROTO ((__atomic_ulong_t _lock,
						  __ulong_t _newvalue));

__VOID__ * ATOMIC_FETCH_AND_STORE_PTR	__PROTO ((__atomic_ptr_t _lock,
						  __VOID__ * _newvalue));

char	   ATOMIC_FETCH_CHAR		__PROTO ((__atomic_char_t _lock));
__uchar_t  ATOMIC_FETCH_UCHAR		__PROTO ((__atomic_uchar_t _lock));
short	   ATOMIC_FETCH_SHORT		__PROTO ((__atomic_short_t _lock));
__ushort_t ATOMIC_FETCH_USHORT		__PROTO ((__atomic_ushort_t _lock));
int	   ATOMIC_FETCH_INT		__PROTO ((__atomic_int_t _lock));
__uint_t   ATOMIC_FETCH_UINT		__PROTO ((__atomic_uint_t _lock));
long	   ATOMIC_FETCH_LONG		__PROTO ((__atomic_long_t _lock));
__ulong_t  ATOMIC_FETCH_ULONG		__PROTO ((__atomic_ulong_t _lock));
__VOID__ * ATOMIC_FETCH_PTR		__PROTO ((__atomic_ptr_t _lock));

int	   ATOMIC_TEST_AND_SET_UCHAR	__PROTO ((__atomic_uchar_t _lock));

void	   ATOMIC_STORE_PTR		__PROTO ((__atomic_ptr_t _lock,
						  __VOID__ * _newvalue));

void	   ATOMIC_CLEAR_UCHAR		__PROTO ((__atomic_uchar_t _lock));
void	   ATOMIC_CLEAR_USHORT		__PROTO ((__atomic_ushort_t _lock));
void	   ATOMIC_CLEAR_PTR		__PROTO ((__atomic_ptr_t _lock));

__EXTERN_C_END__


#else	/* __GNUC__ && (defined (i386) || _I386) */


__LOCAL__ __INLINE__
char ATOMIC_FETCH_AND_STORE_CHAR (__atomic_char_t _lock,
				  char _newvalue) {
	__NON_ISO (asm) volatile ("xchgb %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
__uchar_t ATOMIC_FETCH_AND_STORE_UCHAR (__atomic_uchar_t _lock,
					__uchar_t _newvalue) {
	__NON_ISO (asm) volatile ("xchgb %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
short ATOMIC_FETCH_AND_STORE_SHORT (__atomic_short_t _lock,
				    short _newvalue) {
	__NON_ISO (asm) volatile ("xchgw %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
__ushort_t ATOMIC_FETCH_AND_STORE_USHORT (__atomic_ushort_t _lock,
					  __ushort_t _newvalue) {
	__NON_ISO (asm) volatile ("xchgw %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
int ATOMIC_FETCH_AND_STORE_INT (__atomic_int_t _lock,
				int _newvalue) {
	__NON_ISO (asm) volatile ("xchgl %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
__uint_t ATOMIC_FETCH_AND_STORE_UINT (__atomic_uint_t _lock,
				      __uint_t _newvalue) {
	__NON_ISO (asm) volatile ("xchgl %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
long ATOMIC_FETCH_AND_STORE_LONG (__atomic_long_t _lock,
				  long _newvalue) {
	__NON_ISO (asm) volatile ("xchgl %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
__ulong_t ATOMIC_FETCH_AND_STORE_ULONG (__atomic_ulong_t _lock,
					__ulong_t _newvalue) {
	__NON_ISO (asm) volatile ("xchgl %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__
__VOID__ * ATOMIC_FETCH_AND_STORE_PTR (__atomic_ptr_t _lock,
				       __VOID__ * _newvalue) {
	__NON_ISO (asm) volatile ("xchgl %0, %2" : "=r" (_newvalue) :
				  "0" (_newvalue), "m" (_lock [0]));
	return _newvalue;
}

__LOCAL__ __INLINE__ char ATOMIC_FETCH_CHAR (__atomic_char_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ __uchar_t ATOMIC_FETCH_UCHAR (__atomic_uchar_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ short ATOMIC_FETCH_SHORT (__atomic_short_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ __ushort_t ATOMIC_FETCH_USHORT (__atomic_ushort_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ int ATOMIC_FETCH_INT (__atomic_int_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ __uint_t ATOMIC_FETCH_UINT (__atomic_uint_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ long ATOMIC_FETCH_LONG (__atomic_long_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ __ulong_t ATOMIC_FETCH_ULONG (__atomic_ulong_t _lock) {
	return _lock [0];
}

__LOCAL__ __INLINE__ __VOID__ * ATOMIC_FETCH_PTR (__atomic_ptr_t _lock) {
	return (__VOID__ *) _lock [0];
}

__LOCAL__ __INLINE__ int ATOMIC_TEST_AND_SET_UCHAR (__atomic_uchar_t _lock) {
	return ATOMIC_FETCH_AND_STORE_UCHAR (_lock, (__uchar_t) -1);
}

__LOCAL__ __INLINE__ void ATOMIC_CLEAR_UCHAR (__atomic_uchar_t _lock) {
	_lock [0] = 0;
}

__LOCAL__ __INLINE__ void ATOMIC_CLEAR_USHORT (__atomic_ushort_t _lock) {
	_lock [0] = 0;
}

__LOCAL__ __INLINE__ void ATOMIC_CLEAR_PTR (__atomic_ptr_t _lock) {
	_lock [0] = 0;
}

__LOCAL__ __INLINE__
void ATOMIC_STORE_UCHAR (__atomic_uchar_t _lock, __uchar_t _newvalue) {
	_lock [0] = _newvalue;
}

__LOCAL__ __INLINE__
void ATOMIC_STORE_PTR (__atomic_ptr_t _lock, __VOID__ * _newvalue) {
	_lock [0] = _newvalue;
}

#endif


#if	__BORLANDC__

void	__emit__	(unsigned char __byte, ...);

/*
 * Borland C++ has many restrictions on assembly and in-line functions. To
 * get around some of these, we use the 32-bit-register pseudovariables that
 * are available when targetting the 386. Note how we convert 32-bit values
 * into 16-bit seg:offset pairs and back via the stack; this avoids several
 * of the nastier bugs in the 32-bit support.
 */

#define	__USE_ES__		0x26
#define	__XCHG_AL_ESBX__	__USE_ES__, 0x86, 0x07
#define	__XCHG_AX_ESBX__	__USE_ES__, 0x87, 0x07
#define	__XCHG_EAX_ESBX__	__USE_ES__, 0x66, 0x87, 0x07
#define	__MOV_AX_ESBX__		__USE_ES__, 0x8B, 0x07
#define	__MOV_EAX_ESBX__	__USE_ES__, 0x66, 0x8B, 0x07

#if	defined (__LARGE__) || defined (__HUGE__) || defined (__COMPACT__)
#define	__lockaddr(l)		(_ES = (unsigned) (void __seg *) \
						(void __far *) (l),\
				 _BX = (unsigned) (l))
#else
#define	__lockaddr(l)		(_ES = _DS, _BX = (unsigned) (l))
#endif


#define ATOMIC_FETCH_AND_STORE_CHAR(l,v) (__lockaddr (l), _AL = (v),\
					  __emit__ (__XCHG_AL_ESBX__),\
					  (char) _AL)
#define	ATOMIC_FETCH_AND_STORE_UCHAR(l,v) (__lockaddr (l), _AL = (v),\
					   __emit__ (__XCHG_AL_ESBX__),\
					   (__uchar_t) _AL)
#define ATOMIC_FETCH_AND_STORE_SHORT(l,v) (__lockaddr (l), _AX = (v),\
					   __emit__ (__XCHG_AX_ESBX__),\
					   (short) _AX)
#define	ATOMIC_FETCH_AND_STORE_USHORT(l,v) (__lockaddr (l), _AX = (v),\
					    __emit__ (__XCHG_AX_ESBX__),\
					    (__ushort_t) _AX)
#define ATOMIC_FETCH_AND_STORE_INT(l,v)	(__lockaddr (l), _AX = (v),\
					 __emit__ (__XCHG_AX_ESBX__),\
					 (int) _AX)
#define	ATOMIC_FETCH_AND_STORE_UINT(l,v) (__lockaddr (l), _AX = (v),\
					  __emit__ (__XCHG_AX_ESBX__),\
					  (__uint_t) _AX)
#define ATOMIC_FETCH_AND_STORE_LONG(l,v) (__lockaddr (l), _EAX = (v),\
					  __emit__ (__XCHG_EAX_ESBX__),\
					  (long) _EAX)
#define	ATOMIC_FETCH_AND_STORE_ULONG(l,v) (__lockaddr (l), _EAX = (v),\
					   __emit__ (__XCHG_EAX_ESBX__),\
					   (__ulong_t) _EAX)

#define	ATOMIC_FETCH_CHAR(l)		(((__atomic_char_t) (l)) [0])
#define	ATOMIC_FETCH_UCHAR(l)		(((__atomic_uchar_t) (l)) [0])
#define	ATOMIC_FETCH_SHORT(l)		(((__atomic_short_t) (l)) [0])
#define	ATOMIC_FETCH_USHORT(l)		(((__atomic_ushort_t) (l)) [0])
#define	ATOMIC_FETCH_INT(l)		(((__atomic_int_t) (l)) [0])
#define	ATOMIC_FETCH_UINT(l)		(((__atomic_uint_t) (l)) [0])
#define	ATOMIC_FETCH_LONG(l)		((long) (_EAX = ((__atomic_long_t) (l)) [0]))
#define	ATOMIC_FETCH_ULONG(l)		((__ulong_t) (_EAX = ((__atomic_ulong_t) (l)) [0]))

#define	ATOMIC_TEST_AND_SET_UCHAR(l)	ATOMIC_FETCH_AND_STORE_UCHAR (l, (__uchar_t) -1)

#define	ATOMIC_CLEAR_UCHAR(l)		((void) (((__atomic_uchar_t) (l)) [0] = 0))
#define	ATOMIC_CLEAR_USHORT(l)		((void) (((__atomic_ushort_t) (l)) [0] = 0))

#define	ATOMIC_STORE_UCHAR(l,v)		((void) (((__atomic_uchar_t) (l)) [0] = (v)))

#if	defined (__LARGE__) || defined (__HUGE__) || defined (__COMPACT__)

/*
 * The ATOMIC_FETCH_AND_STORE_PTR () operation cannot be performed in-line in
 * BC++ 3.1 in large data models due to code-generation bugs that appear to
 * be caused by a mix of problems casting 32-bit registers to pointers and
 * other problems dealing with far-pointer-valued returns from in-line
 * assembly.
 */

#define	ATOMIC_STORE_PTR(l,v)		(__lockaddr (l), _EAX = (long) (v),\
					 __emit__ (__XCHG_EAX_ESBX__))

#define	ATOMIC_FETCH_PTR(l)		((void *) (_EAX = ((__atomic_ulong_t) (l)) [0]))

#define	ATOMIC_CLEAR_PTR(l)		ATOMIC_STORE_PTR (l, 0)

#else	/* use simpler small-data version */

#define	ATOMIC_FETCH_AND_STORE_PTR(l,v)	(__lockaddr (l), _AX = (unsigned) (v),\
					 __emit__ (__XCHG_AX_ESBX__), \
					 (void *) _AX)

#define	ATOMIC_STORE_PTR(l,v)		(__lockaddr (l), _AX = (unsigned) (v),\
					 __emit__ (__XCHG_AX_ESBX__))

#define	ATOMIC_FETCH_PTR(l)		(((__atomic_ptr_t) (l)) [0])

#define	ATOMIC_CLEAR_PTR(l)		ATOMIC_STORE_PTR (l, 0)

#endif	/* small data model */


#elif	__COHERENT__


#define	ATOMIC_FETCH_CHAR(l)		(((__atomic_char_t) (l)) [0])
#define	ATOMIC_FETCH_UCHAR(l)		(((__atomic_uchar_t) (l)) [0])
#define	ATOMIC_FETCH_SHORT(l)		(((__atomic_short_t) (l)) [0])
#define	ATOMIC_FETCH_USHORT(l)		(((__atomic_ushort_t) (l)) [0])
#define	ATOMIC_FETCH_INT(l)		(((__atomic_int_t) (l)) [0])
#define	ATOMIC_FETCH_UINT(l)		(((__atomic_uint_t) (l)) [0])
#define	ATOMIC_FETCH_LONG(l)		(((__atomic_long_t) (l)) [0])
#define	ATOMIC_FETCH_ULONG(l)		(((__atomic_ulong_t) (l)) [0])
#define	ATOMIC_FETCH_PTR(l)		(((__atomic_ptr_t) (l)) [0])

#define	ATOMIC_TEST_AND_SET_UCHAR(l)	ATOMIC_FETCH_AND_STORE_UCHAR (l, (__uchar_t) -1)

#define	ATOMIC_CLEAR_UCHAR(l)		((void) (((__atomic_uchar_t) (l)) [0] = 0))
#define	ATOMIC_CLEAR_USHORT(l)		((void) (((__atomic_ushort_t) (l)) [0] = 0))
#define	ATOMIC_CLEAR_PTR(l)		((void) (((__atomic_ptr_t) (l)) [0] = 0))

#define	ATOMIC_STORE_UCHAR(l,v)		((void) (((__atomic_uchar_t) (l)) [0] = (v)))
#define	ATOMIC_STORE_PTR(l,v)		((void) (((__atomic_ptr_t) (l)) [0] = (v)))

#endif	/* __COHERENT__ */


#ifdef	_CHECK_LOCK_TYPES

#undef	ATOMIC_FETCH_AND_STORE_CHAR
#undef	ATOMIC_FETCH_AND_STORE_UCHAR
#undef	ATOMIC_FETCH_AND_STORE_SHORT
#undef	ATOMIC_FETCH_AND_STORE_USHORT
#undef	ATOMIC_FETCH_AND_STORE_INT
#undef	ATOMIC_FETCH_AND_STORE_UINT
#undef	ATOMIC_FETCH_AND_STORE_LONG
#undef	ATOMIC_FETCH_AND_STORE_ULONG
#undef	ATOMIC_FETCH_AND_STORE_PTR
#undef	ATOMIC_FETCH_CHAR
#undef	ATOMIC_FETCH_UCHAR
#undef	ATOMIC_FETCH_SHORT
#undef	ATOMIC_FETCH_USHORT
#undef	ATOMIC_FETCH_INT
#undef	ATOMIC_FETCH_UINT
#undef	ATOMIC_FETCH_LONG
#undef	ATOMIC_FETCH_ULONG
#undef	ATOMIC_FETCH_PTR

#undef	ATOMIC_CLEAR_UCHAR
#undef	ATOMIC_CLEAR_USHORT
#undef	ATOMIC_CLEAR_PTR

#undef	ATOMIC_STORE_UCHAR
#undef	ATOMIC_STORE_PTR

#endif	/* defined (_CHECK_LOCK_TYPES) */


__EXTERN_C_BEGIN__

__pl_t	TEST_AND_SET_LOCK	__PROTO ((__atomic_uchar_t _locked,
					  __pl_t _pl,
					  __CONST__ char * _name));

__EXTERN_C_END__


#endif	/* ! defined (__KERNEL_X86LOCK_H__) */
