#ifndef	__KERNEL__CPUID_H__
#define	__KERNEL__CPUID_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the DDI/DKI data type "processorid_t". Drivers should not make any attempt
 * to interpret the format of data items of this type, or assume that the size
 * or type of this definition will remain consistent across releases of the
 * operating system. Clients should not perform any operations other than
 * assignment on data of this type.
 */

typedef	unsigned int	processorid_t;


/*
 * The following value is guaranteed never to be a valid processor id, and so
 * can be used to indicate data that are not bound to any specific processor.
 */

#define	NOCPU		((processorid_t) -1)

#endif	/* ! defined (__KERNEL__CPUID_H__) */

