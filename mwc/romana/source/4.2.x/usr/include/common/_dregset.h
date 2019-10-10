/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__DREGSET_H__
#define	__COMMON__DREGSET_H__

/*
 * This file defines a structure for the debugging facilities made available
 * by the kernel for debugger use.  In COHERENT, this means the raw debug
 * registers.
 *
 * This file defines the structure layout.  Another file contains a incomplete
 * type declaration, for use by code that has no need to know the internal
 * structure of such items.
 */

#include <common/__types.h>
#include <common/dregset.h>
#include <common/__linear.h>

#if	! _I386
# error	You must be compiling for the _I386 to use this header!
#endif

/*
 * Definitions for the internal structure of the debug registers.
 */

/*
 * Codes for the types of fetches that a breakpoint can match.
 */

enum __break_match {
	_BREAK_EXECUTE,			/* break on execute, len must == 0 */
	_BREAK_WRITE,			/* break on write */
	_BREAK_RESERVED_DO_NOT_USE,
	_BREAK_READ_WRITE,		/* break on read or write */
	__BREAK_MATCH_MASK = _BREAK_READ_WRITE
};

typedef enum __break_match	__break_match_t;


/*
 * Codes for the length of the region matched by a breakpoint; note that the
 * address must be correspondingly aligned on a byte, word, or double-word
 * boundary.
 */

enum __break_len {
	_BREAK_LEN_BYTE,
	_BREAK_LEN_WORD,
	_BREAK_LEN_RESERVED_DO_NOT_USE,
	_BREAK_LEN_DWORD,
	__BREAK_LEN_MASK = _BREAK_LEN_DWORD
};

typedef enum __break_len	__break_len_t;


/*
 * Codes for the level at which the breakpoints are enabled.  If the
 * breakpoint is enabled locally, it will be disabled by the first hardware
 * task-switch to occur.
 *
 * Note that the effect of this at user-level within the COHERENT system may
 * not match your expectation ...
 */

enum __break_level {
	_BREAK_NONE,
	_BREAK_LOCAL,
	_BREAK_GLOBAL,
	_BREAK_BOTH,
	__BREAK_LEVEL_MASK = _BREAK_BOTH
};

typedef enum __break_level	__break_level_t;


/*
 * Codes for controlling whether pipelining in the i386 execution unit should
 * be reduced to a level that allows breakpoint matches to be able to report
 * the precise instruction address that caused the breakpoint match.  On the
 * i486, these bits are ignored and the same effect happens automatically.
 */

enum __break_exact {
	_BREAK_NORMAL,
	_BREAK_SLOW_LOCAL,
	_BREAK_SLOW_GLOBAL,
	_BREAK_SLOW_BOTH,
	__BREAK_EXACT_MASK = _BREAK_SLOW_BOTH
};

typedef	enum __break_exact	__break_exact_t;


/*
 * Codes to control global breakpoint enabling.
 */

enum __break_enab {
	_BREAK_DISABLED,
	_BREAK_ENABLED,
	__BREAK_ENABLE_MASK = _BREAK_ENABLED
};

typedef	enum __break_enab	__break_enab_t;


/*
 * Miscellaneous info about the breakpoints detected.
 */

#define	__BREAK_0_MATCH	0x0001U		/* breakpoint set 0 match */
#define	__BREAK_1_MATCH	0x0002U		/* breakpoint set 1 match */
#define	__BREAK_2_MATCH	0x0004U		/* breakpoint set 2 match */
#define	__BREAK_3_MATCH	0x0008U		/* breakpoint set 3 match */
#define	__BREAK_DEBUG	0x2000U		/* breakpoint contention */
#define	__BREAK_STEP	0x4000U		/* break on single-step */
#define	__BREAK_TASK	0x8000U		/* break on task-switch */


/*
 * The contents of DR6/DR7 are an unsigned long in the _I386 architecture, but
 * we declare the externally-visible type as an incomplete structure pointer
 * to prevent accidental misuse as a scalar type. DR6 should be treated as a
 * read-only object; we only allow construction of an empty version.
 */

typedef	struct __break_stat   *	__break_stat_t;

#define	_MAKE_DEBUG_STATUS()	((__break_stat_t) 0)

#define	__IS_BREAK(stat,mask)	(((__break_arith_t) (stat) & (mask)) != 0)
#define _IS_BREAK_0_MATCH(stat)	__IS_BREAK (stat, __BREAK_0_MATCH)
#define	_IS_BREAK_1_MATCH(stat)	__IS_BREAK (stat, __BREAK_1_MATCH)
#define	_IS_BREAK_2_MATCH(stat)	__IS_BREAK (stat, __BREAK_2_MATCH)
#define	_IS_BREAK_3_MATCH(stat)	__IS_BREAK (stat, __BREAK_3_MATCH)
#define	_IS_BREAK_DEBUG(stat)	__IS_BREAK (stat, __BREAK_DEBUG)
#define	_IS_BREAK_STEP(stat)	__IS_BREAK (stat, __BREAK_STEP)
#define	_IS_BREAK_TASK(stat)	__IS_BREAK (stat, __break_point_tASK)

#define	__CLEAR_BREAK(stat,mask) ((__stat_t) ((__ulong_t) (stat) & ~ (mask)))
#define _CLEAR_BREAK_0_MATCH(stat) __CLEAR_BREAK (stat, __BREAK_0_MATCH)
#define	_CLEAR_BREAK_1_MATCH(stat) __CLEAR_BREAK (stat, __BREAK_1_MATCH)
#define	_CLEAR_BREAK_2_MATCH(stat) __CLEAR_BREAK (stat, __BREAK_2_MATCH)
#define	_CLEAR_BREAK_3_MATCH(stat) __CLEAR_BREAK (stat, __BREAK_3_MATCH)
#define	_CLEAR_BREAK_DEBUG(stat) __CLEAR_BREAK (stat, __BREAK_DEBUG)
#define	_CLEAR_BREAK_STEP(stat)	__CLEAR_BREAK (stat, __BREAK_STEP)
#define	_CLEAR_BREAK_TASK(stat)	__CLEAR_BREAK (stat, __break_point_tASK)


typedef	struct __break_cntl   *	__break_cntl_t;
typedef	struct __break_point  *	__break_point_t;
typedef	__ulong_t		__break_arith_t;

enum {
	__BREAK_LEVEL_SHIFT,
	__BREAK_EXACT_SHIFT = 8,
	__BREAK_ENABLE_SHIFT = 13,
	__BREAK_MATCH_SHIFT = 16,
	__BREAK_LEN_SHIFT = 18
};


/*
 * The following macros wrap up basic shifting and masking such that we don't
 * have to worry about casting (or signedness) below.  Note that we use the
 * "+ 0U" trick because we know the bounds of the shift count, but we perform
 * an actual cast of the LHS because it will often have a nonarithmetic
 * operand.
 */

#define	__BREAK_SHIFT_LEFT(x, y) ((__break_arith_t) (x) << ((y) + 0U))
#define	__BREAK_SHIFT_RIGHT(x, y) ((__break_arith_t) (x) >> ((y) + 0U))
#define	__BREAK_MASK(x, y)	((__break_arith_t) (x) & (y))

#define	__BREAK_SHIFT_AND_MASK(x,direction,name) \
		(__BREAK_SHIFT_RIGHT (x, \
				      __CONCAT3 (__BREAK_, name, _SHIFT)) & \
		 __CONCAT3 (__BREAK_, name, _MASK))

#define	__BREAK_SHIFTED_MASK(name) \
		__BREAK_SHIFT_LEFT (__CONCAT3 (__BREAK_, name, _MASK), \
				    __CONCAT3 (__BREAK_, name, _SHIFT))

#define	__BREAK_BREAK_MASK(n)	\
		((__break_arith_t) \
		 _MAKE_BREAK (__BREAK_MATCH_MASK, __BREAK_LEN_MASK, \
			      __BREAK_LEVEL_MASK))
#define	__BREAK_SHIFT_MASK(n)	__BREAK_SHIFT_LEFT (__BREAK_BREAK_MASK (n), \
						    2 * (n))

#define	_MAKE_BREAK(match,len,level) \
		((__break_point_t) \
		 (__BREAK_SHIFT_LEFT (match, __BREAK_MATCH_SHIFT) | \
		  __BREAK_SHIFT_LEFT (len, __BREAK_LEN_SHIFT) | \
		  __BREAK_SHIFT_LEFT (level, __BREAK_LEVEL_SHIFT)))

#define	_GET_BREAK_MATCH(break) \
		((__break_match_t) __BREAK_SHIFT_AND_MASK (break, MATCH))
#define	_GET_BREAK_LEN(break) \
		((__break_match_t) __BREAK_SHIFT_AND_MASK (break, LEN))
#define	_GET_BREAK_LEVEL(break) \
		((__break_match_t) __BREAK_SHIFT_AND_MASK (break, LEVEL))

#define	_MAKE_DEBUG_CONTROL(br0,br1,br2,br3,exact) \
		((__break_cntl_t) \
		 (__BREAK_SHIFT_LEFT (br0, 0) | \
		  __BREAK_SHIFT_LEFT (br1, 2) | \
		  __BREAK_SHIFT_LEFT (br2, 4) | \
		  __BREAK_SHIFT_LEFT (br3, 6) | \
		  __BREAK_SHIFT_LEFT (exact, __BREAK_EXACT_SHIFT) | \
		  __BREAK_SHIFT_LEFT (__BREAK_ENABLED, __BREAK_ENABLE_SHIFT)))

#define	_MAKE_DEBUG_DISABLED() \
		((__break_cntl_t) \
		 __BREAK_SHIFT_LEFT (__BREAK_DISABLED, __BREAK_ENABLE_SHIFT))

#define	_IS_BREAK_ENABLED(cntl) \
		(((__break_arith_t) (cntl) & \
		  __BREAK_SHIFT_LEFT (__BREAK_ENABLE_MASK, \
				      __BREAK_ENABLE_SHIFT)) != \
		 _BREAK_DISABLED)
#define	_SET_BREAK_ENABLE(cntl, flag) \
		((__break_cntl_t) \
		 (__BREAK_MASK (cntl, __BREAK_SHIFTED_MASK (ENABLE)) | \
		  __BREAK_SHIFT_LEFT (flag, __BREAK_ENABLE_SHIFT))

#define	_BREAK_SET_BREAK(cntl,n,break) \
		((__break_cntl_t) \
		 (__BREAK_MASK (cntl, ~ __BREAK_SHIFT_MASK (n)) | \
		  __BREAK_SHIFT_LEFT (break, 2 * n)))
#define	_BREAK_SET_EXACT(cntl, exact) \
		((__break_cntl_t) \
		 (__BREAK_MASK (cntl, __BREAK_SHIFTED_MASK (EXACT)) | \
		  __BREAK_SHIFT_LEFT (exact, __BREAK_EXACT_SHIFT)))
#define	_BREAK_GET_BREAK(cntl,n) \
		((__break_point_t) (__BREAK_SHIFT_RIGHT (cntl, 2 * n) & \
				    __BREAK_BREAK_MASK (n)))
#define	_BREAK_GET_EXACT(cntl) \
		((__break_exact_t) __BREAK_SHIFT_AND_MASK (cntl, EXACT))

struct __dregset {
	__linear_t	_break_address [4];	/* DR0-DR3 */
	__ulong_t	_break_reserved [2];	/* DR4-DR5 */
	__break_stat_t	_break_status;		/* DR6 */
	__break_cntl_t	_break_control;		/* DR7 */
};

#endif	/* ! defined (__COMMON__DREGSET_H__) */
