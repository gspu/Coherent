#ifndef	__COMMON___TYPES_H__
#define	__COMMON___TYPES_H__

/*
 * This internal header is intended as the sole point of definition for the
 * internal versions of the DDI/DKI data types 'uchar_t', 'ushort_t', 'uint_t'
 * and 'ulong_t'.
 *
 * Several headers define types that we want to be compatible with the DDI/DKI
 * definitions, but we do not want them always pulling in the entire
 * <sys/types.h> header.
 */

typedef	unsigned char	__uchar_t;
typedef	unsigned short	__ushort_t;
typedef	unsigned int	__uint_t;
typedef	unsigned long	__ulong_t;

#endif	/* ! defined (__COMMON___TYPES_H__) */

