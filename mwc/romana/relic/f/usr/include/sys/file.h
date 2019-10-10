#ifndef	__SYS_FILE_H__
#define	__SYS_FILE_H__

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
 * A bit mask of flags that correspond more-or-less to the definitions in the
 * <sys/fcntl.h> file, except that in <sys/file.h> the read/write information
 * is not in a bitmask as it is below.
 *
 * These values are used in calls to device-driver open () and close () entry
 * points under the DDI/DKI.
 */

enum {
	FREAD		= 1,
	FWRITE		= 2,
	FNDELAY		= 4,		/* == O_NDELAY */
	FNONBLOCK	= 0x80,		/* == O_NONBLOCK */
	FEXCL		= 0x400		/* == O_EXCL */
};


#if	! _SYSV4 || _SYSV3
/*
 * In order to be able to use old-style Coherent facilities without having to
 * try to use the old Coherent header files (it isn't worth it) we make some
 * funky random constants visible here.
 */

/*
 * Permission bits. Beware! There are only 16 bits available.
 */
enum {
	IPE		= 0x01,			/* Execute */
	IPW		= 0x02,			/* Write */
	IPR		= 0x04,			/* Read */

	/*
	 * Modifier bits for fdopen().
	 */

	IPNDLY	 	= 0x08,
	IPAPPEND 	= 0x10,

	IPSYNC	 	= 0x20,
	IPEXCL	 	= 0x40,
	IPNOCTTY 	= 0x80,

	IPNONBLOCK	= 0x100
};

#endif

#endif	/* ! defined (__SYS_FILE_H__) */
