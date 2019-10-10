/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_INLINE_H__
#define	__SYS_INLINE_H__

/*
 * This header file exists for compatibility with the System V DDI/DKI.
 * It defines some routines which are commonly kept inlined for
 * efficiency reasons (although under the full DDI/DKI regime they are not
 * in fact inlined).
 */

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/xdebug.h>
#include <kernel/__pl.h>
#include <kernel/ddi_cpu.h>
#include <kernel/x86io.h>


/*
 * This header is made a little more complex by the fact that we want to
 * be able to support inlining for those compilers that can do it, yet we
 * also need to be able to use the #include <sys/ddi.h> mechanism to force
 * all of these to be bound to regular C functions.
 *
 * To support this, the initial compiler-dependent part of the
 * file performs feature tests and sets a __FORWARD_x__ macro for those
 * definitions that are to be "forwarded" via macro to a version whose name
 * has some additional leading underscores prepended.
 *
 * In this way, for systems that have inlining the #include <sys/ddi.h> will
 * suppress the forwarding.
 */

/*
 * Perform feature tests to try and work out how the processor priority
 * level can be manipulated under this machine/compiler.
 */

/*
 * The 80x86 processor and the IBM PC have numerous design problems with
 * regard to interrupt priority. Some are the result of using
 * external hardware to deal with interrupt priority level (not in itself a
 * big deal, but the 8259A on the PC ISA-bus is a problem). Others are a
 * result of the modality of the various processors in the 80x86 family and
 * the fact that none of them can emulate the others very well, by design.
 *
 * The key features are that the sti/cli instructions used in the CPU to mask
 * all interrupts are simply too coarse for regular use, plus the fact that
 * certain instructions which manipulate the processor priority at some
 * privelege levels may trap (or worse, simply do nothing) at others, meaning
 * branches and tests are required for reliable operation.
 *
 * Programming the 8259A interrupt controller directly gives the necessary
 * granularity (but may cause problems due to bus issues) and allows the
 * hard-wired priority levels to be redefined by software.
 */

#if	__GNUC__ && _I386	/* 80386 running GCC */

#define __IBM_PC__
#define	__PICC__	0x20
#define	__PICM__	0x21
#define	__SPICC__	0xA0
#define	__SPICM__	0xA1

#define	__EOI_CMD__	0x20

extern intmask_t	_masktab [];

__LOCAL__ __INLINE__ void __CHEAP_DISABLE_INTS (void) {
	__NON_ISO (asm) ("cli");
}

__LOCAL__ __INLINE__ void __CHEAP_ENABLE_INTS (void) {
	__NON_ISO (asm) ("sti");
}

__LOCAL__ __INLINE__ void __OUT_IPL_MASK (intmask_t _mask) {
	outb (__PICM__, _mask);
	outb (__SPICM__, _mask >> 8);
}

__LOCAL__ __INLINE__ intmask_t __GET_BASE_MASK (void) {
	return ddi_cpu_data ()->dc_base_mask;
}

__LOCAL__ __INLINE__ void __SET_BASE_MASK (intmask_t _mask) {
	__OUT_IPL_MASK (ddi_cpu_data ()->dc_base_mask = _mask);
}

__LOCAL__ __INLINE__ void __SEND_EOI (int _vector) {
	outb (__PICC__, __EOI_CMD__);

	if (_vector >= 8)
		outb (__SPICC__, __EOI_CMD__);
}

#define	__FORWARD_splx__
#define	__FORWARD_splraise__


#elif 	__BORLANDC__


#define __IBM_PC__
#define	__PICC__	0x20
#define	__PICM__	0x21
#define	__SPICC__	0xA0
#define	__SPICM__	0xA1

#define	__EOI_CMD__	0x20

void	__emit__ (unsigned char _byte, ...);

#define	__CLI__		0xFB
#define	__STI__		0xFA

extern intmask_t	_masktab [];

#define	__CHEAP_DISABLE_INTS()		__emit__ (__CLI__)
#define	__CHEAP_ENABLE_INTS()		__emit__ (__STI__)

#define	__OUT_IPL_MASK(mask) \
	(void) (outb (__PICM__, mask), \
		outb (__SPICM__, mask >> 8))

#define	__GET_BASE_MASK()\
		(const intmask_t) (ddi_cpu_data ()->dc_base_mask)
#define	__SET_BASE_MASK(mask)\
		(void) (ddi_cpu_data ()->dc_base_mask = (mask), \
			__OUT_IPL_MASK (ddi_cpu_data ()->dc_base_mask))

#define	__SEND_EOI(vector) \
		(outb (__PICC__, __EOI_CMD__), (vector < 8 ? (void) 0 : \
		   outb (__SPICC__, __EOI_CMD__)))

/*
 * Since BC++ 3.1 has outlawed inline functions in C code, to the point of
 * not even providing an __inline__ keyword, we can only provide in-line
 * definitions for splx () and splraise () under C++
 */

#ifdef	__cplusplus

#define	__FORWARD_splx__
#define	__FORWARD_splraise__

#endif


#elif	__COHERENT__


/*
 * Coherent's C compiler has no extensions.
 */

#define __IBM_PC__
#define	__PICC__	0x20
#define	__PICM__	0x21
#define	__SPICC__	0xA0
#define	__SPICM__	0xA1

#define	__EOI_CMD__	0x20


extern intmask_t	_masktab [];


#define	__GET_BASE_MASK()\
		(const intmask_t) (ddi_cpu_data ()->dc_base_mask)

__EXTERN_C_BEGIN__

void		__CHEAP_DISABLE_INTS	__PROTO ((void));
void		__CHEAP_ENABLE_INTS	__PROTO ((void));
void		__SET_BASE_MASK		__PROTO ((intmask_t _newmask));

__EXTERN_C_END__

#endif	/* ! __COHERENT__ */


/*
 * Generic stuff for the 80x86 IBM PC interrupt architecture.
 */

#ifdef	__IBM_PC__

#ifdef	__FORWARD_splx__

__LOCAL__ __INLINE__ __pl_t __splx (__pl_t _newpl) {
	__pl_t		_prev_pl;
	intmask_t	_mask = __GET_BASE_MASK () | _masktab [_newpl];

	__OUT_IPL_MASK (_mask);

	_prev_pl = (__pl_t) ddi_cpu_data ()->dc_ipl;
	ddi_cpu_data ()->dc_ipl = _newpl;

	return _prev_pl;
}

#endif

#ifdef	__FORWARD_splraise__

__LOCAL__ __INLINE__ __pl_t __splraise (__pl_t _newpl) {
	__pl_t		_prev_pl;

	if (_newpl > (_prev_pl = (__pl_t) ddi_cpu_data ()->dc_ipl)) {
		intmask_t	_mask = __GET_BASE_MASK () |
					_masktab [_newpl];

		__OUT_IPL_MASK (_mask);
		ddi_cpu_data ()->dc_ipl = _newpl;
	}

	return _prev_pl;
}

#endif


#define	__splbase()	splx (plbase)
#define	__spltimeout()	splx (pltimeout)
#define	__spldisk()	splx (pldisk)
#define	__splstr()	splx (plbase)
#define	__splhi()	splx (plhi)

#define	__FORWARD_splbase__
#define	__FORWARD_spltimeout__
#define	__FORWARD_spldisk__
#define	__FORWARD_splstr__
#define	__FORWARD_splhi__

#endif


/*
 * Function versions of these are available by including <sys/ddi.h> or by
 * using a compiler setup that does not permit inlining. The actual function
 * prototypes always live here, <sys/ddi.h> merely knows how to #undef some
 * names.
 */

__EXTERN_C_BEGIN__

__pl_t		splbase		__PROTO ((void));	/* enabled */
__pl_t		spltimeout	__PROTO ((void));	/* timeouts */
__pl_t		spldisk		__PROTO ((void));	/* disk I/O */
__pl_t		splstr		__PROTO ((void));	/* STREAMS */
__pl_t		splhi		__PROTO ((void));	/* disabled */
__pl_t		splx		__PROTO ((__pl_t _pl));	/* restore */

/* Internal functions - not part of DDI/DKI interface */

int		splcmp		__PROTO ((__pl_t _l, __pl_t _r));
							/* compare */
__pl_t		splraise	__PROTO ((__pl_t _pl));	/* raise priority */

__EXTERN_C_END__


/*
 * Define forwarding macros for those functions that need them.
 *
 * Note that the form of these definitions is such that they can be
 * suppressed by enclosing the name in parentheses. Simply defining a
 * token mapping is not a good idea for functions.
 */

#ifdef	__FORWARD_splbase__
# define	splbase()	__splbase ()
#endif

#ifdef	__FORWARD_spltimeout__
# define	spltimeout()	__spltimeout ()
#endif

#ifdef	__FORWARD_spldisk__
# define	spldisk()	__spldisk ()
#endif

#ifdef	__FORWARD_splstr__
# define	splstr()	__splstr ()
#endif

#ifdef	__FORWARD_splhi__
# define	splhi()		__splhi ()
#endif

#ifdef	__FORWARD_splx__
# define	splx(x)		__splx (x)
#endif

#ifdef	__FORWARD_splcmp__
# define	splcmp(l,r)	__splcmp (l,r)
#endif

#ifdef	__FORWARD_splraise__
# define	splraise(x)	__splraise(x)
#endif


/*
 * Clean up the macros we #define
 */

#undef	__IBM_PC__
#undef	__FORWARD_splbase__
#undef	__FORWARD_spltimeout__
#undef	__FORWARD_spldisk__
#undef	__FORWARD_splstr__
#undef	__FORWARD_splhi__
#undef	__FORWARD_splx__
#undef	__FORWARD_splcmp__
#undef	__FORWARD_splraise__

#endif	/* ! defined (__SYS_INLINE_H__) */
