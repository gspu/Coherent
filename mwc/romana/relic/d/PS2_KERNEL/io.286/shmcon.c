/* $Header: /kernel/kersrc/io.286/shmcon.c,v 1.1 92/07/17 15:24:47 bin Exp Locker: bin $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1987, 1985, 1984.
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/*
 * System V Compatible Shared Memory Device Driver
 *
 *	This device driver provides System V compatible shared memory operations.
 *	Operations are performed through the shared memory device (/dev/shm).
 *	and are implemented as ioctl calls from shmctl, shmget, shmat, shmdt
 *	utilities.
 *
 *			Author: Allan Cornish, INETCO Systems Ltd., Sep 1984
 *
 * $Log:	shmcon.c,v $
 * Revision 1.1  92/07/17  15:24:47  bin
 * Initial revision
 * 
 * Revision 2.1	88/09/03  13:12:40	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:06:36	src
 * Initial revision
 * 
 * 87/03/02	Allan Cornish		/usr/src/sys/i8086/drv/shmcon.c
 * Shmioctl() now supports long key [was short] on SHMGET operations.
 * This allows compatability with System V.
 *
 * 85/10/16	Allan Cornish
 * Driver split into shmcon.c, shm.c [driver implementation, system V shm].
 *
 * 85/07/22	Allan Cornish
 * Shmget, shmctl now return immediately if u.u_error is set.
 *
 * 85/07/19	Allan Cornish
 * Separation of io_seek into shmid and off improved through type casting.
 * Errno set to EFAULT if fucopy() or ufcopy() report no bytes transferred.
 * This would occur if an user address fault occurred.
 *
 * 85/07/03	Allan Cornish
 * Replaced use of EDOM with EIDRM.
 * Replaced shmaccess() by calls to ipcaccess(), increasing shared ipc code.
 * Eliminated shmlock() and shmunlock(), as shared memory use is synchronous.
 *
 * 84/09/30	Allan Cornish
 * Initial Revision.
 */

#include <coherent.h>
#include <sched.h>
#include <types.h>
#include <errno.h>
#include <stat.h>
#include <con.h>
#include <seg.h>
#include <shm.h>

#ifndef	EIDRM
#define	EIDRM	EDOM
#endif

/*
 * Functions.
 */

int shmload();
int shmread();
int shmwrite();
int shmioctl();
int nulldev();
int nonedev();

/*
 * Device Configuration.
 */

CON shmcon = {
	DFCHR,			/* Flags			*/
	24,			/* Major Index			*/
	nulldev,		/* Open				*/
	nulldev,		/* Close			*/
	nonedev,		/* Block			*/
	shmread,		/* Read				*/
	shmwrite,		/* Write			*/
	shmioctl,		/* Ioctl			*/
	nulldev,		/* Power fail			*/
	nulldev,		/* Timeout			*/
	shmload,		/* Load				*/
	nulldev			/* Unload			*/
};

unsigned NSHMID = 16;
struct shmid_ds *shmids;
struct seg **shmsegs;

/*
 * Shared Memory Device Load.
 */

static
shmload()
{
	register struct shmid_ds * idp;
	register unsigned wanted;
	
	if ( NSHMID == 0 )
		return 0;

	wanted = NSHMID * (sizeof(struct shmid_ds) + sizeof(struct seg *));

	if ( (shmids = kalloc( wanted )) == 0 ) {

		printf("couldn't kalloc %u shared memory ids\n", NSHMID );
		NSHMID = 0;
		return 0;
	}
	shmsegs = (struct seg *) &shmids[ NSHMID ];

	for ( idp = &shmids[NSHMID]; --idp >= shmids; )
		idp->shm_perm.mode = 0;

	return 0;
}

/*
** Shared Memory Read.
*/

static
shmread( dev, iop )

dev_t dev;
register IO *iop;

{
	register struct shmid_ds *idp;
	int shmid;
	unsigned off;
	faddr_t faddr;

	off   = ((unsigned *) &iop->io_seek)[0];
	shmid = ((unsigned *) &iop->io_seek)[1];

	if ( shmid >= NSHMID ) {
		u.u_error = EFAULT;
		return -1;
	}

	idp = &shmids[shmid];

	if ( (idp->shm_perm.mode & IPC_ALLOC) == 0 ) {
		u.u_error = EIDRM;
		return -1;
	}

	if ( (ipcaccess(&idp->shm_perm) & SHM_R) == 0 ) {
		u.u_error = EACCES;
		return -1;
	}

	if ( ((long) off + iop->io_ioc) > idp->shm_segsz ) {
		u.u_error = EFAULT;
		return -1;
	}

	FP_SEL(faddr) = FP_SEL(shmsegs[shmid]->s_faddr);
	FP_OFF(faddr) = off;

	if ( ! fucopy( faddr, iop->io_base, iop->io_ioc ) ) {
		u.u_error = EFAULT;
		return -1;
	}

	iop->io_ioc = 0;
	return 0;
}

/*
** Shared Memory Write.
*/

static
shmwrite( dev, iop )

dev_t dev;
register IO *iop;

{
	register struct shmid_ds *idp;
	int shmid;
	unsigned off;
	faddr_t faddr;

	off   = ((unsigned *) &iop->io_seek)[0];
	shmid = ((unsigned *) &iop->io_seek)[1];

	if ( shmid >= NSHMID ) {
		u.u_error = EFAULT;
		return -1;
	}

	idp = &shmids[shmid];

	if ( (idp->shm_perm.mode & IPC_ALLOC) == 0 ) {
		u.u_error = EIDRM;
		return -1;
	}

	if ( (ipcaccess(&idp->shm_perm) & SHM_W) == 0 ) {
		u.u_error = EFAULT;
		return -1;
	}

	if ( ((long) off + iop->io_ioc) > idp->shm_segsz ) {
		u.u_error = EFAULT;
		return -1;
	}

	FP_SEL(faddr) = FP_SEL(shmsegs[shmid]->s_faddr);
	FP_OFF(faddr) = off;

	if ( ! ufcopy(iop->io_base, faddr, iop->io_ioc ) ) {
		u.u_error = EFAULT;
		return -1;
	}

	iop->io_ioc = 0;
	return 0;
}

/*
 * Shared Memory Device Ioctl.
 *
 *	Input:	dev = shared memory device (/dev/shm).
 *		com = command to perform (SHMCTL, SHMGET,SHMAT).
 *		vec = pointer to return value, followed by parameters.
 *
 *	Action:	Initiate command.
 *		Save commands return value in *vec.
 */

static
shmioctl( dev, com, vec )

dev_t dev;
int com;
int *vec;

{
	switch ( com ) {

	case SHMCTL:
		putuwd( vec+0,
			ushmctl( getuwd( vec+1 ),
				 getuwd( vec+2 ),
				 getuwd( vec+3 ) ));
		break;

	case SHMGET:
		putuwd( vec+0,
			ushmget( getuwd( vec+1 ),
				 getuwd( vec+2 ),
				 getuwd( vec+3 ),
				 getuwd( vec+4 ) ));
		break;

	default:
		u.u_error = EINVAL;
		break;
	}
}
