/*
 * System V Compatible Shared Memory Device Driver
 *
 *	This device driver provides System V compatible shared memory operations.
 *	Operations are performed through the shared memory device (/dev/shm).
 *	and are implemented as ioctl calls from shmctl, shmget, shmat, shmdt
 *	utilities.
 *
 *	Author: Allan Cornish.
 *
 */

#include <sys/coherent.h>
#include <sys/sched.h>
#include <sys/types.h>
#include <sys/uproc.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/con.h>
#include <sys/seg.h>
#include <sys/shm.h>
#ifndef _I386
#define getusd(arg)		getuwd(arg)
#define putusd(arg1, arg2)	putuwd(arg1, arg2)
#endif

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
	
	if (NSHMID == 0)
		return 0;

	wanted = NSHMID * (sizeof(struct shmid_ds) + sizeof(struct seg *));

	/* allocate space for structs */

	if ((shmids = kalloc(wanted)) == 0) {
		printf("couldn't kalloc %u shared memory ids\n", NSHMID);
		NSHMID = 0;
		return 0;
	}
	shmsegs = (struct seg *) &shmids[ NSHMID ];

	for (idp = &shmids[NSHMID]; --idp >= shmids;)
		idp->shm_perm.mode = 0;

	return 0;
}

/*
** Shared Memory Read.
*/



static
shmread(dev, iop)

dev_t dev;
register IO *iop;

{
	register struct shmid_ds *idp;
	int shmid;
	unsigned off;
#ifndef _I386
	faddr_t faddr; 
#endif

#ifdef _I386
	off   = ((unsigned short *) &iop->io_seek)[0];
	shmid = ((unsigned short *) &iop->io_seek)[1];
#else
	off   = ((unsigned  *) &iop->io_seek)[0];
	shmid = ((unsigned  *) &iop->io_seek)[1];
#endif

	/* is shmid valid? */
	if (shmid >= NSHMID) {
		u.u_error = EFAULT;
		return -1;
	}

	idp = &shmids[shmid]; /* point to struct _ds being used */

	/* check mode */
	if ((idp->shm_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EIDRM;
		return -1;
	}

	/* check permissions */
	if ((ipcaccess(&idp->shm_perm) & SHM_R) == 0) {
		u.u_error = EACCES;
		return -1;
	}

	/* are we about to exceed our segment size? */
	if (((long) off + iop->io_ioc) > idp->shm_segsz) {
		u.u_error = EFAULT;
		return -1;
	}

	/* copy from kernel data to user data */
#ifndef _I386
	FP_SEL(faddr) = FP_SEL(shmsegs[shmid]->s_faddr);
	FP_OFF(faddr) = off;
#endif
#ifdef _I386
	if (!kucopy(shmsegs[shmid], iop->io.vbase,
	             iop->io_ioc)){
#else
	if (! fucopy(faddr, iop->io_base, iop->io_ioc)) {
#endif
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
shmwrite(dev, iop)

dev_t dev;
register IO *iop;

{
	register struct shmid_ds *idp;
	int shmid;
	unsigned off;
#ifndef _I386
	faddr_t faddr;
#endif


#ifdef _I386
	off   = ((unsigned short *) &iop->io_seek)[0];
	shmid = ((unsigned short *) &iop->io_seek)[1];
#else
	off   = ((unsigned *) &iop->io_seek)[0];
	shmid = ((unsigned *) &iop->io_seek)[1];
#endif

	/* validate id # */
	if (shmid >= NSHMID) {
		u.u_error = EFAULT;
		return -1;
	}

	idp = &shmids[shmid];  /* set our pointer to struct we're working on*/

	/* verify mode */
	if ((idp->shm_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EIDRM;
		return -1;
	}

	/* verify Permissions */
	if ((ipcaccess(&idp->shm_perm) & SHM_W) == 0) {
		u.u_error = EFAULT;
		return -1;
	}

	/* verify that we won't overwrite our space */
	if (((long) off + iop->io_ioc) > idp->shm_segsz) {
		u.u_error = EFAULT;
		return -1;
	}

	/* copy from user memory to shared segment */
#ifndef _I386
	FP_SEL(faddr) = FP_SEL(shmsegs[shmid]->s_faddr);
	FP_OFF(faddr) = off;
#endif
#ifdef _I386
	if (!ukcopy( iop->io.vbase, shmsegs[shmid],
	             iop->io_ioc)){
#else
	if (! ufcopy(iop->io_base, faddr, iop->io_ioc)) {
#endif
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
shmioctl(dev, com, vec)

dev_t dev;
int com;
int *vec;

{
	switch (com) {

	case SHMCTL:
		putusd(vec+0,
			ushmctl(getusd(vec+1),
				 getusd(vec+2),
				 getusd(vec+3)));
		break;

	case SHMGET:
		putusd(vec+0,
			ushmget(getusd(vec+1),
				 getusd(vec+2),
				 getusd(vec+3),
				 getusd(vec+4)));
		break;

	default:
		u.u_error = EINVAL;
		break;
	}
}
