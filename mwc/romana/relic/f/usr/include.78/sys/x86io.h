#ifndef	__SYS_X86IO_H__
#define	__SYS_X86IO_H__

/*
 * The system V DDI/DKI for Intel processors defines a C-language interface to
 * the machine I/O instructions available through <sys/types.h>.
 *
 * For simplicity, this has been factored out into a separate file for this
 * implementation, and <sys/types.h> pulls this file in.
 */
/*
 *-IMPORTS:
 *	<sys/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__PROTO ()
 *		__NON_ISO ()
 *		__INLINE__
 *	<sys/xdebug.h>
 *		__LOCAL__
 *	<sys/types.h>
 *		uchar_t
 *		ushort_t
 *		ulong_t
 */

#include <sys/ccompat.h>
#include <sys/xdebug.h>
#include <sys/v_types.h>


/*
 * Get these into a useful form in a similar manner to the way the spl... ()
 * functions are defined in <sys/inline.h>
 */

#if	defined (__GNUC__) && defined (i386)	/* 80386 with GNU CC */


/*
 * Note that the definitions for the string-input instructions assume that
 * ES: is equivalent to DS:.
 */

__LOCAL__ __INLINE__ uchar_t __inb (int _port) {
	uchar_t		_result;
	__NON_ISO (asm) volatile ("in %1,%0" :
				  "=a" (_result) : "d" ((ushort_t) _port));
	return _result;
}

__LOCAL__ __INLINE__ ulong_t __inl (int _port) {
	ulong_t		_result;
	__NON_ISO (asm) volatile ("in %1,%0" :
				  "=a" (_result) : "d" ((ushort_t) _port));
	return _result;
}

__LOCAL__ __INLINE__ ushort_t __inw (int _port) {
	ushort_t	_result;
	__NON_ISO (asm) volatile ("in %1,%0" :
				  "=a" (_result) : "d" ((ushort_t) _port));
	return _result;
}

__LOCAL__ __INLINE__ void __outb (int _port, uchar_t value) {
	__NON_ISO (asm) volatile ("out %1,%0" : :
				  "d" ((ushort_t) _port), "a" (value));
}

__LOCAL__ __INLINE__ void __outl (int _port, ulong_t value) {
	__NON_ISO (asm) volatile ("out %1,%0" : :
				  "d" ((ushort_t) _port), "a" (value));
}

__LOCAL__ __INLINE__ void __outw (int _port, ushort_t value) {
	__NON_ISO (asm) volatile ("out %1,%0" : :
				  "d" ((ushort_t) _port), "a" (value));
}

__LOCAL__ __INLINE__ void __repinsb (int _port, uchar_t * _addr, int _cnt) {
	__NON_ISO (asm) volatile ("mov %1,%%edi;rep;insb" : :
				  "d" ((ushort_t) _port), "g" (_addr),
				  "c" (_cnt) : "di");
}

__LOCAL__ __INLINE__ void __repinsd (int _port, ulong_t * _addr, int _cnt) {
	__NON_ISO (asm) volatile ("mov %1,%%edi;rep;insl" : :
				  "d" ((ushort_t) _port), "g" (_addr),
				  "c" (_cnt) : "di");
}

__LOCAL__ __INLINE__ void __repinsw (int _port, ushort_t * _addr, int _cnt) {
	__NON_ISO (asm) volatile ("mov %1,%%edi;rep;insw" : :
				  "d" ((ushort_t) _port), "g" (_addr),
				  "c" (_cnt) : "di");
}

__LOCAL__ __INLINE__ void __repoutsb (int _port, uchar_t * _addr, int _cnt) {
	__NON_ISO (asm) volatile ("mov %1,%%esi;rep;outsb" : :
				  "d" ((ushort_t) _port), "g" (_addr),
				  "c" (_cnt) : "si");
}

__LOCAL__ __INLINE__ void __repoutsd (int _port, ulong_t * _addr, int _cnt) {
	__NON_ISO (asm) volatile ("movl %1,%%esi;rep;outsl" : :
				  "d" ((ushort_t) _port), "g" (_addr),
				  "c" (_cnt) : "si");
}

__LOCAL__ __INLINE__ void __repoutsw (int _port, ushort_t * _addr, int _cnt) {
	__NON_ISO (asm) volatile ("movl %1,%%esi;rep;outsw" : :
				  "d" ((ushort_t) _port), "g" (_addr),
				  "c" (_cnt) : "si");
}

#define __FORWARD_inb__
#define __FORWARD_inl__
#define __FORWARD_inw__
#define	__FORWARD_outb__
#define	__FORWARD_outl__
#define	__FORWARD_outw__
#define	__FORWARD_repinsb__
#define	__FORWARD_repinsd__
#define	__FORWARD_repinsw__
#define	__FORWARD_repoutsb__
#define	__FORWARD_repoutsd__
#define	__FORWARD_repoutsw__

#elif	defined (__BORLANDC__)

/*lint +fva *//* __emit__ () is truly varadic */
void	__emit__	(__ANY_ARGS__);
/*lint -fva */

/*
 * Note the use of the 0x66 operand-size override prefix below, which is built
 * on the assumption that Borland generates 16-bit code segments.
 */

#define	__REP__		0xF3		/* repeat-prefix */

#define	__INB__		0xEC
#define	__INW__		0xED
#define	__INL__		0x66, __INW__	/* size-override for 32-bit form */

#define	__OUTB__	0xEE
#define	__OUTW__	0xEF
#define	__OUTL__	0x66, __OUTW__	/* size-override for 32-bit form */

#define	__INSB__	0x6C
#define	__INSW__	0x6D
#define	__INSL__	0x65, __INSW__

#define	__OUTSB__	0x6E
#define	__OUTSW__	0x6F		/* word/long depending on CS: */
#define	__OUTSL__	0x66, __OUTSW__


/*
 * If we are willing to restrict ourselves to i386's, Borland C++ 3.1 allows
 * support for 32-bit register-pseudovariables if 386 code generation has
 * been enabled.
 *
 * As it turns out, there are all kinds of bugs in the 386 support in BC++ 3.1
 * but enough works that we can even get a far pointer into ES:whatever via
 * a 32-bit register, as long as the intermediate register isn't _ESI or _EDI
 * since _EAX = _ESI or _EAX = _EDI only moves the low 16 bits. Being able to
 * load ES:whatever via a 32-bit register allows us to write macros that only
 * reference each argument exactly once.
 *
 * While the 32-bit-pointer version still works in the near models, it's not
 * exactly optimal, so we use a more straightforward version there.
 */

#define	__inb(port)		(_DX = (port), __emit__ (__INB__), (uchar_t) _AL)
#define	__inl(port)		(_DX = (port), __emit__ (__INL__), _EAX)
#define	__inw(port)		(_DX = (port), __emit__ (__INW__), (ushort_t) _AX)

#define	__outb(port,value)	(_DX = (port), _AL = (value), __emit__ (__OUTB__))
#define	__outl(port,value)	(_DX = (port), _EAX = (value), __emit__ (__OUTL__))
#define	__outw(port,value)	(_DX = (port), _AX = (value), __emit__ (__OUTW__))

#if	defined (__LARGE__) || defined (__HUGE__) || defined (__COMPACT__)

#define	__repins(p,a,c)		(_EDX = (long) (void __far *) (a), _EAX = _EDX,\
				 _ES = (unsigned) (_EAX >> 16),\
				 _DI = _DX, _CX = (c), _DX = (p))
#define	__repouts(p,a,c)	(_EDX = (long) (void _far *) (a), _EAX = _EDX,\
				 _ES = (unsigned) (_EAX >> 16),\
				 _SI = _DX, _CX = (c), _DX = (p))

#else	/* use simpler small-data versions */

#define	__repins(p,a,c)		(_ES = _DS, _DI = (unsigned) (a), _CX = (c), _DX = (p))
#define	__repouts(p,a,c)	(_ES = _DS, _SI = (unsigned) (a), _CX = (c), _DX = (p))

#endif

#define	__repinsb(p,a,c)	(__repins (p, a, c),\
				 __emit__ (__REP__, __INSB__))
#define	__repinsd(p,a,c)	(__repins (p, a, c),\
				 __emit__ (__REP__, __INSL__))
#define	__repinsw(p,a,c)	(__repins (p, a, c),\
				 __emit__ (__REP__, __INSW__))

#define	__repoutsb(p,a,c)	(__repouts (p, a, c),\
				 __emit__ (__REP__, __OUTSB__))
#define	__repoutsd(p,a,c)	(__repouts (p, a, c),\
				 __emit__ (__REP__, __OUTSL__))
#define	__repoutsw(p,a,c)	(__repouts (p, a, c),\
				 __emit__ (__REP__, __OUTSW__))


#define __FORWARD_inb__
#define __FORWARD_inl__
#define __FORWARD_inw__
#define	__FORWARD_outb__
#define	__FORWARD_outl__
#define	__FORWARD_outw__
#define	__FORWARD_repinsb__
#define	__FORWARD_repinsd__
#define	__FORWARD_repinsw__
#define	__FORWARD_repoutsb__
#define	__FORWARD_repoutsd__
#define	__FORWARD_repoutsw__

#elif	defined (COHERENT)

/*
 * Under Coherent, the inb ()/inw ()/inl () and outb ()/outw ()/outl ()
 * operations are already available under those names in the standard system.
 *
 * The rep... () operations are provided in a supplemental file that should be
 * linked in if you are building a DDI/DKI kernel.
 */

#endif


__EXTERN_C_BEGIN__

uchar_t		inb		__PROTO ((int _port));
ulong_t		inl		__PROTO ((int _port));
ushort_t	inw		__PROTO ((int _port));
void		outb		__PROTO ((int _port, uchar_t _value));
void		outl		__PROTO ((int _port, ulong_t _value));
void		outw		__PROTO ((int _port, ushort_t _value));
void		repinsb		__PROTO ((int _port, uchar_t * _addr,
					  int _cnt));
void		repinsd		__PROTO ((int _port, ulong_t * _addr,
					  int _cnt));
void		repinsw		__PROTO ((int _port, ushort_t * _addr,
					  int _cnt));
void		repoutsb	__PROTO ((int _port, uchar_t * _addr,
					  int _cnt));
void		repoutsd	__PROTO ((int _port, ulong_t * _addr,
					  int _cnt));
void		repoutsw	__PROTO ((int _port, ushort_t * _addr,
					  int _cnt));

__EXTERN_C_END__


#ifdef	__FORWARD_inb__
# define	inb(port)	__inb (port)
#endif

#ifdef	__FORWARD_inl__
# define	inl(port)	__inl (port)
#endif

#ifdef	__FORWARD_inw__
# define	inw(port)	__inw (port)
#endif

#ifdef	__FORWARD_outb__
# define	outb(port,val)	__outb (port, val)
#endif

#ifdef	__FORWARD_outl__
# define	outl(port,val)	__outl (port, val)
#endif

#ifdef	__FORWARD_outw__
# define	outw(port,val)	__outw (port, val)
#endif

#ifdef	__FORWARD_repinsb__
# define	repinsb(p,a,c)	__repinsb (p, a, c)
#endif

#ifdef	__FORWARD_repinsd__
# define	repinsd(p,a,c)	__repinsd (p, a, c)
#endif

#ifdef	__FORWARD_repinsw__
# define	repinsw(p,a,c)	__repinsw (p, a, c)
#endif

#ifdef	__FORWARD_repoutsb__
# define	repoutsb(p,a,c)	__repoutsb (p, a, c)
#endif

#ifdef	__FORWARD_repoutsd__
# define	repoutsd(p,a,c)	__repoutsd (p, a, c)
#endif

#ifdef	__FORWARD_repoutsw__
# define	repoutsw(p,a,c)	__repoutsw (p, a, c)
#endif


#undef	__FORWARD_inb__
#undef	__FORWARD_inl__
#undef	__FORWARD_inw__
#undef	__FORWARD_outb__
#undef	__FORWARD_outl__
#undef	__FORWARD_outw__
#undef	__FORWARD_repinsb__
#undef	__FORWARD_repinsd__
#undef	__FORWARD_repinsw__
#undef	__FORWARD_repoutsb__
#undef	__FORWARD_repoutsd__
#undef	__FORWARD_repoutsw__

#endif	/* ! defined (__SYS_X86IO_H__) */
