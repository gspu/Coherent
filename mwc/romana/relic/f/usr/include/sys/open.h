#ifndef	__SYS_OPEN_H__
#define	__SYS_OPEN_H__

/*
 * This file contains constants and definitions used by the System V DDI/DKI.
 *
 * Note that the information defined here has been compiled from published
 * information, so it is likely to be incomplete. Futhermore, the exact binary
 * values of the constants are not defined in any currently published
 * documentation, so we have assigned values that will almost certainly not
 * match the System V definitions.
 */

/*
 * A value indicating the type of open (or close) being performed on a device
 * driver under the DDI/DKI.
 */

enum {
	OTYP_BLK,		/* open through the block interface */
	OTYP_CHR,		/* open through the character interface */
	OTYP_LYR		/* layered open; opened by another driver */
};

#endif	/* ! defined (__SYS_OPEN_H__) */
