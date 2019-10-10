/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_STROPTS_H__
#define	__SYS_STROPTS_H__

/*
 * Stuff related to STREAMS ioctl ()'s that matter to STREAMS drivers and
 * modules. Constants that were defined in System V Release 3.2 STREAMS have
 * actual values as defined in the Intel iBCS2 documentation, pp6-56 through
 * 6-58.
 *
 * Values added in the System V Release 4 edition of STREAMS have numeric
 * values taken from the System V ABI, Intel386 Supplement pp6-70 through
 * 6-75
 */

/*
 * Stream head read option constants.
 */

#define	RNORM		0x0000		/* Byte-stream mode */
#define	RMSGD		0x0001		/* Message-discard mode */
#define	RMSGN		0x0002		/* Message-nondiscard mode */
#define	__RINVAL	0x0003		/* Invalid */

#define	RMODEMASK	0x0003		/* Separate mode from flag bits below */

#define	RPROTDAT	0x0004          /*
					 * deliver control portion of message
					 * as data for read ()
					 */
#define	RPROTDIS	0x0008          /*
					 * discard the control portion of a
					 * message on user read ()
					 */
#define	RPROTNORM	0x0010		/*
					 * fail read () with EBADMSG on msg
					 * other than M_DATA
					 */

/*
 * What to flush in an M_FLUSH message, also used in I_FLUSH and I_FLUSHBAND
 */

#define FLUSHR          0x01
#define FLUSHW          0x02
#define FLUSHRW         (FLUSHR | FLUSHW)
#define	FLUSHBAND	0x04


#endif	/* ! defined (__SYS_STROPTS_H__) */
