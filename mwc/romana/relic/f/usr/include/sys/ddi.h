#ifndef	__SYS_DDI_H__
#define	__SYS_DDI_H__

/*
 * This header deals in removing macro definitions of functions in order to
 * enhance the portablity of user driver binaries by removing dependencies
 * on many system definitions such as structure lengths. Since many simple
 * functions are implemented as macros to remove the source-level dependency
 * while keeping performance, inclusion of the <sys/ddi.h> header indicates
 * that the user wishes to make trade off speed and time against binary
 * portability.
 *
 * Of course, many DDI/DKI functions exists to remove other dependences, such
 * as on the size and offsets of structure *members*, and on the names and
 * locations of data items. However, this header does not deal with defining
 * prototype or anything else related to these aspects of the DDI/DKI
 * specification, as they either involve changing source code to use these
 * new functions or are old functions (defined in the headers related to the
 * subsystem they were appropriated from) that have been elevated to DDI/DKI
 * preferred status.
 *
 * Clients of this header *should* be using the new DDI/DKI functions so as
 * to reduce dependency on system data structure layout, but here we deal
 * with the simple transformation of macros to function calls, by subsystem.
 */



#ifdef	__SYS_STREAM_H__

/*
 * Our client is also a client of the STREAMS subsystem; undefine macros that
 * are functions under the DDI/DKI regime.
 */

#undef	datamsg
#undef	enableok
#undef	noenable
#undef	OTHERQ
#undef	pcmsg
#undef	putnext
#undef	RD
#undef	WR

#endif	/* defined (__SYS_STREAM_H__) */


#ifdef	__KERNEL_STRMLIB_H__

/*
 * Our client is also using some of the STREAMS extras
 */

#undef	W
#undef	R

#endif	/* defined (__KERNEL_STRMLIB_H__) */


#ifdef	__SYS_INLINE_H__

/*
 * Our client is also a client of the inline interrupt-level manipulation
 * functions.
 */

#undef	splbase
#undef	spltimeout
#undef	spldisk
#undef	splstr
#undef	splhi
#undef	splx

#undef	splcmp
#undef	splraise

#endif	/* defined (__SYS_INLINE_H__) */


#ifdef	__KERNEL_V_TYPES_H__

#undef	btop
#undef	btopr
#undef	ptob

#undef	etoimajor
#undef	getemajor
#undef	geteminor
#undef	getmajor
#undef	getminor
#undef	itoemajor
#undef	makedevice

#endif	/* defined (__KERNEL_V_TYPES_H__) */


#ifdef	__KERNEL_DDI_BASE_H__

#undef	ddi_base_data

#endif	/* defined (__KERNEL_DDI_BASE_H__) */


#ifdef	__KERNEL_DDI_PROC_H__

#undef	ddi_proc_data

#endif	/* defined (__KERNEL_DDI_PROC_H__) */


#ifdef	__KERNEL_DDI_CPU_H__

#undef	ddi_cpu_data

#endif	/* defined (__KERNEL_DDI_CPU_H__) */


#ifdef	__KERNEL_DDI_GLOB_H__

#undef	ddi_glob_data

#endif	/* defined (__KERNEL_DDI_GLOB_H__) */


#ifdef	__KERNEL_X86IO_H__

/*
 * Our client is also a client of the Intel 80x86 I/O instruction bindings.
 *
 * This stuff is normally defined as part of <sys/types.h> but we keep it
 * in a separate header (which <sys/types.h> can pull in if necessary).
 */

#undef	insb
#undef	insl
#undef	insw
#undef	outb
#undef	outl
#undef	outw
#undef	repinsb
#undef	repinsd
#undef	repinsw
#undef	repoutsb
#undef	repoutsd
#undef	repoutsw

#endif	/* defined (__KERNEL_X86IO_H__) */

#endif	/* ! defined (__SYS_DDI_H__) */
