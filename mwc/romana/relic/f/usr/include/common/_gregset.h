#ifndef	__COMMON__GREGSET_H__
#define	__COMMON__GREGSET_H__

/*
 * This file defines a "register set" type along the lines of the "gregset_t"
 * type defined for the process file-system in System V, Release 4. This
 * subsumes the function of the <sys/reg.h> header in the iBCS2 specification,
 * which describes the layout of data on the signal stack and also within the
 * kernel (in Coherent, via the "u.u_regl" pointer, and in SCO via "u.u_ar0")
 * in terms of an array of "int".
 *
 * This file defines the structure layout; an incomplete type declaration is
 * present in another file for use by code that has no need to know the
 * internal structure of such items.
 */

#include <common/__types.h>
#include <common/gregset.h>

union _gregset {
	/*
	 * Frame for i386 task.
	 */
	struct {
		/*
		 * Pushed by the trap handler.
		 */
		__ushort_t	_gs;
		__ushort_t	_pad1;
		__ushort_t	_fs;
		__ushort_t	_pad2;
		__ushort_t	_es;
		__ushort_t	_pad3;
		__ushort_t	_ds;
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
		__ushort_t	_cs;
		__ushort_t	_pad5;
		__ulong_t	_eflags;
		__ulong_t	_uesp;
		__ushort_t	_ss;
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
		__ushort_t	_gs;
		__ushort_t	_pad1;
		__ushort_t	_fs;
		__ushort_t	_pad2;
		__ushort_t	_es;
		__ushort_t	_pad3;
		__ushort_t	_ds;
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
		__ushort_t	_cs;
		__ushort_t	_pad14;
		__ushort_t	_flags;
		__ushort_t	_pad15;
		__ushort_t	_usp;
		__ushort_t	_pad16;
		__ushort_t	_ss;
		__ushort_t	_pad17;
	} _i286;
};

#endif	/* ! defined (__COMMON__GREGSET_H__) */

