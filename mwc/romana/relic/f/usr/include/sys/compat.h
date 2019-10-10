#ifndef	__SYS_COMPAT_H__
#define	__SYS_COMPAT_H__

/*
 * This file provides glue to export the system-private compatibility
 * definitions from <common/ccompat.h> with names that are valid for user-
 * level code to use.
 */

#include <common/ccompat.h>

#if	__USE_PROTO__
# define	USE_PROTO	__USE_PROTO__
#endif

#define	STRING(x)		__STRING (x)
#define	CONCAT(x,y)		__CONCAT (x, y)
#define	CONCAT3(x,y,z)		__CONCAT3 (x, y, z)
#define	CONCAT4(a,b,c,d)	__CONCAT4 (a, b, c, d)
#define	PROTO(p)		__PROTO (p)
#define	CONST			__CONST__
#define	VOLATILE		__VOLATILE__
#define	VOID			__VOID__
#define	NOTUSED(name)		__NOTUSED (name)
#define	REGISTER		__REGISTER__
#define	EXTERN_C		__EXTERN_C__
#define	EXTERN_C_BEGIN		__EXTERN_C_BEGIN__
#define	EXTERN_C_END		__EXTERN_C_END__
#define	NON_ISO(k)		__NON_ISO(k)

#if	__USE_INLINE__
# define	USE_INLINE	__USE_INLINE__
#endif

#if	__USE_INLINEL__
# define	USE_INLINEL	__USE_INLINEL__
#endif

#define	INLINE		__INLINE__
#define	INLINEL		__INLINEL__

#define	ANY_ARGS	__ANY_ARGS__

#define	ARGS(x)		__ARGS(x)

#ifndef	LOCAL
# define	LOCAL		static
#endif

#if	! __PEDANTIC__
# define	NO_RETURN	__NO_RETURN__
#else
# define	NO_RETURN
#endif

#endif	/* ! defined (__SYS_COMPAT_H__) */
