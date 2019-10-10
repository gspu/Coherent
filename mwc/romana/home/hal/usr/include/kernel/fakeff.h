/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __KERNEL_FAKEFF_H__
#define __KERNEL_FAKEFF_H__

#include <common/ccompat.h>

typedef	unsigned long	faddr_t;


/*
 * Read a byte from far address 'fp'.  Note that if an address fault
 * occurs, the system will panic.
 */

#define ffbyte(fp)	(* (__VOLATILE__ unsigned char *) (fp))


/*
 * Read a word from far address 'fp'.  Note that if an address fault
 * occurs, the system will panic.
 */

#define ffword(fp)	(* (__VOLATILE__ unsigned short *) (fp))


/*
 * Copy 'n' bytes from far address 'fp' to address 'k' in the kernel
 * data segment.  Returns the number of bytes copied.
 * Note that if an address fault occurs, the system will panic.
 */

#define fkcopy(fp, k, n)	kkcopy(fp, k, n)


/*
 * Copy 'count' bytes from 'from_fp' to 'to_fp'.
 * Note that if an address fault occurs, the system will panic.
 */

#define ffcopy(from_fp, to_fp, count)	(void) kkcopy(from_fp, to_fp, count)


/*
 * Write byte 'b' to address 'fp'.  Note that if an address
 * fault occurs, the system will panic.
 */

#define sfbyte(fp, b)	((void) (* (__VOLATILE__ unsigned char *)(fp) = \
				 (unsigned char) (b)))


/*
 * Write word 'w' to address 'fp'.  Note that if an address
 * fault occurs, the system will panic.
 */

#define sfword(fp, w)	((void) (* (__VOLATILE__ unsigned short *) (fp) = \
				 (unsigned short) (w)))

#endif	/* ! defined (__KERNEL_FAKEFF_H__) */
