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
#include <stdlib.h>

#ifndef	EIDRM
#define	EIDRM	EDOM
#endif


extern	unsigned NSHMID;
extern	struct shmid_ds *shmids;
extern	struct seg **shmsegs;

/*
 * Shmctl - Shared Memory Control Operations.
 */

ushmctl(shmid, cmd, buf)
#ifdef _I386
int shmid, cmd;
#else
unsigned shmid;
int cmd;
#endif
struct shmid_ds *buf;

{
	register struct shmid_ds *idp;
	int ret = 0;

	if (u.u_error)
		return -1;

	if (shmid >= NSHMID) {
		u.u_error = EINVAL;
		return -1;
	}

	idp = &shmids[shmid];

	if ((idp->shm_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EINVAL;
		return -1;
	}

	switch (cmd) {

	case IPC_STAT:
		if ((ipcaccess(&idp->shm_perm) & SHM_R) == 0) {
			u.u_error = EACCES;
			return -1;
		}
		kucopy(idp, buf, sizeof(struct shmid_ds));
		ret = 0;
		break;

	case IPC_SET:
		if ((u.u_uid != 0) && (u.u_uid != idp->shm_perm.uid)) {
			u.u_error = EPERM;
			ret = -1;
			break;
		}
		idp->shm_perm.uid   = getuwd(&(buf->shm_perm.uid));
		idp->shm_perm.gid   = getuwd(&(buf->shm_perm.gid));
		idp->shm_perm.mode &= ~0777;
		idp->shm_perm.mode |= getuwd(&(buf->shm_perm.mode)) & 0777;
		ret = 0;
		break;

	case IPC_RMID:
		if ((u.u_uid != 0) && (u.u_uid != idp->shm_perm.uid)) {
			u.u_error = EPERM;
			ret = -1;
			break;
		}
		idp->shm_perm.seq++;
		kfree(shmsegs[shmid]);
		idp->shm_perm.mode = 0;
		ret = 0;
		break;

	default:
		u.u_error = EINVAL;
		ret = -1;
	}

	return ret;
}

/*
 * Shmget - Get Shared Memory Segment
 */

ushmget(skey, size, shmflg)

key_t skey;
#ifdef _I386
int size, shmflg;
#else
unsigned size;
int shmflg;
#endif
{
	register struct shmid_ds *idp;
	struct shmid_ds *freeidp = 0;

	if (u.u_error)
		return -1;

	for (idp = &shmids[NSHMID]; --idp >= shmids;) {

		if ((idp->shm_perm.mode & IPC_ALLOC) == 0) {

			if ((freeidp == 0) ||
			    (freeidp->shm_ctime > idp->shm_ctime))
				freeidp = idp;
			continue;
		}

#ifdef	IPC_PRIVATE
		if (skey == IPC_PRIVATE)
			continue;
#endif

		if (skey == idp->shm_perm.key) {		/* found! */

			if ((shmflg & IPC_CREAT) && (shmflg & IPC_EXCL)) {

				u.u_error = EEXIST;
				return -1;
			}

			if ((idp->shm_perm.mode & shmflg) != (shmflg&0777)) {

				u.u_error = EACCES;
				return -1;
			}

			if (idp->shm_segsz < size) {

				u.u_error = EINVAL;
				return -1;
			}

			return idp - shmids;
		}
	}

	if (!(shmflg & IPC_CREAT)) {
		u.u_error = ENOENT;
		return -1;
	}

	if (freeidp == 0) { /* error if no structs available */
		u.u_error = ENOSPC;
		return -1;
	}

	idp = freeidp;

	/* allocate space for shared memory segment */
	if ((shmsegs[idp - shmids] = kalloc((size_t) size)) == NULL){
		u.u_error = ENOSPC;
		return -1;
	}


	idp->shm_segsz = size;
	idp->shm_atime = 0;
	idp->shm_dtime = 0;
	idp->shm_ctime = timer.t_time;
	idp->shm_cpid  = SELF->p_pid;
	idp->shm_perm.cuid = idp->shm_perm.uid = u.u_uid;
	idp->shm_perm.cgid = idp->shm_perm.gid = u.u_gid;
	idp->shm_perm.mode = (shmflg & 0777) | IPC_ALLOC;
	idp->shm_perm.key  = skey;

#ifdef	IPC_PRIVATE
	if (skey == IPC_PRIVATE)
		idp->shm_perm.mode |= SHM_DEST;
#endif

	return idp - shmids;
}
