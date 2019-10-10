/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__GREGSET_H__
#define	__COMMON__GREGSET_H__

/*
 * This file defines a "register set" type along the lines of the "gregset_t"
 * type defined for the process file-system in System V, Release 4.  This
 * subsumes the function of the <sys/reg.h> header in the iBCS2 specification,
 * which describes the layout of data on the signal stack and also within the
 * kernel (in COHERENT, via the "u.u_regl" pointer, and in SCO via "u.u_ar0")
 * in terms of an array of "int".  With the introduction of this type, the U
 * area structure has been removed as an anachronism.
 *
 * This file defines the structure layout; an incomplete type declaration is
 * present in another file for use by code that has no need to know the
 * internal structure of such items.
 */

#include <common/__types.h>
#include <common/gregset.h>

/*
 * Definitions for dealing with the flag register.  The flag register should
 * only be accessed by the following macros; for public use we declare a
 * non-arithmetic type.  For the 286 part of the structure, we define a
 * suitable non-arithmetic type, but no macros; such elements may be copied,
 * but not manipulated.
 */

/*
 * Codes for I/O privelege levels.  This level specifies a cut-off processor
 * privelege level (as given by the CPL, the privelege level of the currently
 * executing code segment) above which certain instructions require special
 * privelege.
 */

enum __iopl {
	_IOPL_0, _IOPL_1, _IOPL_2, _IOPL_3,
	__FLAG_IOPL_MASK = _IOPL_3
};
typedef	enum __iopl	__iopl_t;


typedef	struct __flag_reg     *	__flag_reg_t;
typedef	__ulong_t		__flag_arith_t;
typedef	struct {
	__ushort_t	__286;
} __286_flags_t;


enum {
	__CARRY_FLAG_SHIFT,
	__PARITY_FLAG_SHIFT = 2,
	__AUXCARRY_FLAG_SHIFT = 4,
	__ZERO_FLAG_SHIFT = 6,
	__SIGN_FLAG_SHIFT,
	__TRAP_FLAG_SHIFT,
	__INTERRUPT_FLAG_SHIFT,
	__DIRECTION_FLAG_SHIFT,
	__OVERFLOW_FLAG_SHIFT,
	__IOPL_SHIFT,
	__NESTED_FLAG_SHIFT = 14,
	__RESUME_FLAG_SHIFT = 16,
	__V8086_FLAG_SHIFT,
	__ALIGNCHECK_FLAG_SHIFT
};


/*
 * Except for the IOPL field, all the currently defined members of the flag
 * word are single bits; here we simply define a general framework for dealing
 * with them and the shift positions for the most common flags.
 */

#define	__FLAG_MASK(value, shift) \
		((__flag_arith_t) (value) << \
		 (__CONCAT (shift, _FLAG_SHIFT) + 0U))

#define	__FLAG_GET_FLAG(flags, shift) \
		((__flag_arith_t) (flags) >> \
		 (__CONCAT (shift, _FLAG_SHIFT) & 1) != 0)
#define	__FLAG_SET_FLAG_TO(flags, value, shift) \
		((__flag_reg_t) \
		 (((__flag_arith_t) (flags) & ~ __FLAG_MASK (1, shift)) | \
		  __FLAG_MASK (value, shift)))
#define	__FLAG_SET_FLAG(flags, shift)	__FLAG_SET_FLAG_TO (flags, 1, shift)
#define	__FLAG_CLEAR_FLAG(flags, shift)	__FLAG_SET_FLAG_TO (flags, 0, shift)

#define	__FLAG_GET_IOPL(flags) \
		((__iopl_t) ((__flag_arith_t) (flags) >> __IOPL_SHIFT) & \
		 __IOPL_MASK)

/*
 * Special case for clearing all the status (i.e., user-modifiable) flags to
 * a known state.
 */

#define	__FLAG_STATUS_MASK	(__FLAG_MASK (1, __OVERFLOW) | \
			         __FLAG_MASK (1, __SIGN) | \
				 __FLAG_MASK (1, __ZERO) | \
				 __FLAG_MASK (1, __AUXCARRY) | \
				 __FLAG_MASK (1, __PARITY) | \
				 __FLAG_MASK (1, __CARRY))
#define	__FLAG_CLEAR_STATUS(flags) \
		((__flag_reg_t) ((__flag_arith_t) (flags) & \
				 ~ __FLAG_STATUS_MASK))

/*
 * Type for describing an Intel-style selector. We have few choices as to how
 * to define this type, because the size of a selector does not match that of
 * a pointer for the usual pointer-to-incomplete-type technique.
 *
 * However, because selector access relates only to the i386 processor family,
 * this is actually desirable as we do not want access to these items to be
 * made directly in portable code anyway.
 */

typedef	__ushort_t	__sel_arith_t;
typedef	struct {
	__sel_arith_t	__selector;
} __selector_t;

#define	__SELECTOR_ARITH(selp)	((selp).__selector)

/*
 * Describe privilege rings verbosely.
 */

enum {
	_PRIVILEGE_RING_0,
	_PRIVILEGE_RING_1,
	_PRIVILEGE_RING_2,
	_PRIVILEGE_RING_3,
	_USER_PRIVILEGE = _PRIVILEGE_RING_3,
	__PRIVILEGE_RING_MASK = _PRIVILEGE_RING_3
};

/*
 * A bit in the selector selects whether the selector refers to a descriptor
 * in the local descriptor table or the global descriptor table.
 */

enum {
	_GLOBAL_DESCRIPTOR,
	_LOCAL_DESCRIPTOR,
	__DESCRIPTOR_TABLE_MASK = _LOCAL_DESCRIPTOR
};

/*
 * A part of the selector forms an index into one of the selected descriptor
 * tables.  Here, we only deal with the generic aspect of this; index values
 * for particular segments are described elsewhere.
 */

enum {
	__DESCRIPTOR_INDEX_MASK = 0x1FFF
};

/*
 * Shift values for the various components of a selector.
 */

#define	__SELECTOR_PRIVILEGE_SHIFT	0U
#define	__SELECTOR_TABLE_SHIFT		2U
#define	__SELECTOR_INDEX_SHIFT		3U

#define	__SELECTOR_PRIVILEGE(selp) \
		((__SELECTOR_ARITH (selp) >> __SELECTOR_PRIVILEGE_SHIFT) & \
		 __PRIVILEGE_RING_MASK)
#define	__SELECTOR_TABLE(selp) \
		((__SELECTOR_ARITH (selp) >> __SELECTOR_TABLE_SHIFT) & \
		 __DESCRIPTOR_TABLE_MASK)
#define	__SELECTOR_DESCRIPTOR(selp) \
		((__SELECTOR_ARITH (selp) >> __SELECTOR_INDEX_SHIFT) & \
		 __DESCRIPTOR_INDEX_MASK)
#define	__MAKE_SELECTOR_ARITH(index,table,privelege) \
		(((index) << __SELECTOR_INDEX_SHIFT) | \
		 ((table) << __SELECTOR_TABLE_SHIFT) | \
		 ((privelege) << __SELECTOR_PRIVILEGE_SHIFT))
#define	__SET_SELECTOR(selp,arith) \
		((void) (__SELECTOR_ARITH (selp) = (arith)))

/*
 * General register-set description structure, without padding.
 */

union __gregset {
	/*
	 * Frame for i386 task.
	 */
	struct {
		/*
		 * Pushed by the trap handler.
		 */
		__selector_t	_gs;
		__ushort_t	_pad1;
		__selector_t	_fs;
		__ushort_t	_pad2;
		__selector_t	_es;
		__ushort_t	_pad3;
		__selector_t	_ds;
		__ushort_t	_pad4;
		/*
		 * Pushed by the trap handler with a "pusha" intruction.
		 */
		__ulong_t	_edi;
		__ulong_t	_esi;
		__ulong_t	_ebp;
		__ulong_t	_esp;
		__ulong_t	_ebx;
		__ulong_t	_edx;
		__ulong_t	_ecx;
		__ulong_t	_eax;
		/*
		 * Created by the i386 when handling exceptions, or by the
		 * trap code when handling software/hardware interrupts.
		 */
		__ulong_t	_trapno;
		__ulong_t	_err;
		/*
		 * General i386 inter-level interrupt return frame.
		 */
		__ulong_t	_eip;
		__selector_t	_cs;
		__ushort_t	_pad5;
		__flag_reg_t	_eflags;
		__ulong_t	_uesp;
		__selector_t	_ss;
		__ushort_t	_pad6;
	} _i386;
	/*
	 * Frame for i286 task, created from the same code as above but with
	 * only the low 16 bits of general registers containing valid data.
	 */
	struct {
		/*
		 * Pushed by the trap handler.
		 */
		__selector_t	_gs;
		__ushort_t	_pad1;
		__selector_t	_fs;
		__ushort_t	_pad2;
		__selector_t	_es;
		__ushort_t	_pad3;
		__selector_t	_ds;
		__ushort_t	_pad4;
		/*
		 * Pushed by the trap handler with a "pusha" intruction.
		 */
		__ushort_t	_di;
		__ushort_t	_pad6;
		__ushort_t	_si;
		__ushort_t	_pad7;
		__ushort_t	_bp;
		__ushort_t	_pad8;
		__ulong_t	_esp;		/* kernel %esp */
		__ushort_t	_bx;
		__ushort_t	_pad9;
		__ushort_t	_dx;
		__ushort_t	_pad10;
		__ushort_t	_cx;
		__ushort_t	_pad11;
		__ushort_t	_ax;
		__ushort_t	_pad12;

		/*
		 * Created by the i386 when handling exceptions, or by the
		 * trap code when handling software/hardware interrupts.
		 */
		__ulong_t	_trapno;
		__ulong_t	_err;

		/*
		 * General i386 inter-level interrupt return frame.
		 */
		__ushort_t	_ip;
		__ushort_t	_pad13;
		__selector_t	_cs;
		__ushort_t	_pad14;
		__286_flags_t	_flags;
		__ushort_t	_pad15;
		__ushort_t	_usp;
		__ushort_t	_pad16;
		__selector_t	_ss;
		__ushort_t	_pad17;
	} _i286;
};

/*
 * The notion of "flags" and "program counter" are fairly universal, although
 * details vary wildly.  The notion of a "flags" register encompasses condition
 * codes on CISC-style processors, but is more oriented towards processor
 * control functions such as endianness on RISC processors.
 *
 * Program counters are fairly universal, though. We also provide a predicate
 * for testing whether a context is a saved user context or system context.
 */

#define	__FLAG_REG(rp)		((rp)->_i386._eflags)
#define	__PC_REG(rp)		((rp)->_i386._eip)
#define	__IS_USER_REGS(rp)	(__SELECTOR_PRIVILEGE ((rp)->_i386._cs) \
				 == _USER_PRIVILEGE)

#endif	/* ! defined (__COMMON__GREGSET_H__) */

