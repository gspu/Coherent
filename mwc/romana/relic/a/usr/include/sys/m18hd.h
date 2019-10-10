/* $Header: /src386/usr/include/sys/RCS/m18hd.h,v 1.1 92/07/31 16:07:19 root Exp $
 *
 * Olivetti M18 Personal Computer
 * Hard Disk Constants
 *
 *			Allan Cornish, INETCO Systems Ltd., Aug 1984
 *
 * $Log:	m18hd.h,v $
 * Revision 1.1  92/07/31  16:07:19  root
 * Initial revision
 * 
 * Revision 1.1	88/03/24  17:48:25	src
 * Initial revision
 * 
 */
#ifndef M18HD_H
#define	M18HD_H

/*
 * Register Addresses
 */

#define	SEL_REG		0x300		/* Select Register (write only)	*/
#define	STAT_REG	0x300		/* Status Register (read  only) */
#define	ENA_REG		0x301		/* Enable Register (write only) */
#define	DATA_REG	0x302		/* Data   Register (read/write) */
#define	CMD_REG		0x303		/* Commnd Register (read,write) */
#define	AUX_REG		0x304		/* Auxiliary Reg   (read  only) */
 
/*
 * Select Register (Write Only)
 */

#define	HEAD_SEL(n)	((n)<<2)	/* Head Select macro		*/
#define	STEP_SEL	0x20		/* Set to step			*/
#define	STEP_IN		0x40		/* Set to step in		*/
#define	RWC_SEL		0x80		/* Reduced Write Curr		*/
 
/*
 * Status Register (Read Only) - bits 6..7 unused
 */

#define	DTA_ST		0x01		/* Drive Type, Jumper A 	*/
#define	RDY_ST		0x02		/* Drive Ready			*/
#define	TK0_ST		0x04		/* At Track 0			*/
#define	WFLT_ST		0x08		/* Write Fault on drive 	*/
#define	SKC_ST		0x10		/* Seek Complete		*/
#define	SYNC_ST		0x20		/* Sector Synchronization	*/

/*
 * Enable Register (Write Only) - bits 0..4 select sector (0..31)
 */

#define	READ_ENA	0x20		/* Enable read as next command	*/
#define	ECC_ENA		0x40		/* Enable ecc on next command	*/
#define	WPRE_ENA	0x80		/* Enable write precompensation	*/

/*
 * Auxiliary Register (Read Only) - bits 2..7 unused
 */

#define	BUSY_AUX	0x01		/* Drive Busy			*/
#define	DTC_AUX		0x02		/* Drive Type, Jumper C 	*/

/*
 * Drive Specific Constants
 */

#define	NSPT		32
#define	NHEAD		4
#define	NCYL		306
#define	WPCC		64		/* Write Precompensation cyl	*/
#define	RWCC		128		/* Reduced Write Current cyl	*/

/*
 * Error Returns from hdget(), hdput()
 */

#define	ECC_ERR		0x00ff		/* Bad ECC read error mask	*/
#define	SKNC_ERR	0x0100		/* Seek Not Complete Error	*/
#define	BUSY_ERR	0x0200		/* Controller Busy Error	*/
#define	SYNC_ERR	0x0400		/* Synchronization Error	*/
#define	PRFX_ERR	0x0800		/* Bad Prefix Read Error	*/
#define	HEAD_ERR	0x1000		/* Bad Head Read Error		*/
#define	CYL_ERR		0x2000		/* Bad Cylinder Read Error	*/
#define	SEC_ERR		0x4000		/* Bad Sector ID Read Error	*/
#define	NRDY_ERR	0x8000		/* Drive Not Ready Error	*/


#endif
