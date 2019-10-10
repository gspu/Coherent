/*
 * File: shm1.c
 *
 * Purpose: System V Compatible Shared Memory Device Driver
 *
 * $Log:	shm1.c,v $
 * Revision 2.3  93/08/09  13:46:24  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  15:32:18  nigel
 * Nigel's R80
 * 
 * Revision 1.3  93/04/14  10:23:10  root
 * r75
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/types.h>
#include <sys/proc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/errno.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define SHMBASE	0x80000000	/* Base shared memory address */
/* These macros should be somewhere in the headers ???*/
#define	DATAST	0x400000	/* Start of the data virtual address */
#define DATAEND	0x7FFFFF	/* End of the data virtual address */
#define STACKST	0x7C000000	/* Start of the stack */
#define	STACKEN	0x7FFFFFFF	/* End of the stack virtual address */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
extern SEG	*shmAlloc();	/* shm0.c */
char		*ushmat();
int		ushmdt();
int		ushmctl();
int		ushmget();
int		iShmPerm();	/* Check permissions */
caddr_t		vCheckReqAdd();	/* Check attach address for shmat */
/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/* Configurable variables - see /etc/conf/mtune. */
extern int	SHMMNI;
extern int 	SHMMAX;

struct shmid_ds	*shmids = NULL;		/* Array of shared memory segments */
SEG		**shmsegs;		/* Array of pointers to segments */
/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * Shmctl - Shared Memory Control Operations.
 */
int
ushmctl(iShmId, iCmd, pstShmId)
int 		iShmId,		/* Shared memory id */
		iCmd;		/* Command */
struct shmid_ds	*pstShmId;	/* User shmid_ds buffer */
{
	register struct shmid_ds *rstIdp;	
	int iRet = 0;

	/* Check if id is in proper range. */
	if (iShmId >= SHMMNI || iShmId < 0) {
		u.u_error = EINVAL;
		return;
	}
	/* Check we did alloc. All allocatable arrays are alloced after
	 * the first ~correct usage of shmget.
	 */
	if (shmids == NULL) {
		u.u_error = EINVAL;
		return;
	}
	rstIdp = &shmids[iShmId];	/* Requested segment */

	/* Check if segment is in used */
	if ((rstIdp->shm_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EINVAL;
		return;
	}

	switch (iCmd) {
	case IPC_STAT:
		/* Check read permission for stat. */
		if (iShmPerm((rstIdp), SHM_R)) {
			u.u_error = EACCES;
			return;
		}
		/* Check if user gives a valid buffer */
		if (!useracc(pstShmId, sizeof(struct shmid_ds), 1)) {
			u.u_error = EFAULT;
			return;
		}
		/* kucopy will set u_error if error occurs */
		kucopy(rstIdp, pstShmId, sizeof(struct shmid_ds));
		break;

	case IPC_SET:
		if ((u.u_uid != 0) && (u.u_uid != rstIdp->shm_perm.uid) &&
				(u.u_uid != rstIdp->shm_perm.cuid)) {
			u.u_error = EPERM;
			iRet = -1;
			break;
		}
		rstIdp->shm_perm.uid   = getuwd(&(pstShmId->shm_perm.uid));
		rstIdp->shm_perm.gid   = getuwd(&(pstShmId->shm_perm.gid));
		rstIdp->shm_perm.mode &= ~0777;
		rstIdp->shm_perm.mode |= getuwd(&(pstShmId->shm_perm.mode)) 
									& 0777;
		break;

	case IPC_RMID:
		if ((u.u_uid != 0) && (u.u_uid != rstIdp->shm_perm.uid) &&
				(u.u_uid != rstIdp->shm_perm.cuid)) {
			u.u_error = EPERM;
			iRet = -1;
			break;
		}
		
		/* SVR3 allows removing an attached segment. Even worse, the
		 * process that has the segment attached can keep using it. 
		 * Some buggy third party software uses this "feature". 
		 * So, we have to make it available too;-(
		 */
		rstIdp->shm_perm.seq = 0;
		rstIdp->shm_perm.mode = 0;

		/* If segment is attached, set flag to be removed */
		if (rstIdp->shm_nattch > 0)
			shmsegs[iShmId]->s_flags |= SRFBERM;
		else	/* remove it otherwise */
			shmFree(shmsegs[iShmId]);
		shmsegs[iShmId] = NULL;
		break;

	/* SHM_LOCK and SHM_UNLOCK: lock/unlock shared memory segement 
	 * in core. Is not a part of iBCS2.
	 * Has no meaning for current 4.0.* release of COHERENT.
	 * Have been done for binary portability.
	 */
	case SHM_LOCK:	
		if (!u.u_uid) {
			u.u_error = EPERM;
			return;
		}
		break;
	case SHM_UNLOCK:	
		if (!u.u_uid) {
			u.u_error = EPERM;
			return;
		}
		break;
	
	default:
		u.u_error = EINVAL;
		iRet = -1;
	}

	return iRet;
}

/*
 * Shmget - Get Shared Memory Segment
 * Return shared memory id if succed, -1 and set u_error otheriwse.
 */
int
ushmget(kShmKey, iShmSize, iShmFlg)
key_t	kShmKey;	/* Shared memory key */
int	iShmSize;	/* Shared memory segment size */
int	iShmFlg;	/* Flags */
{
	register struct shmid_ds *rstShmId;	/* Work pointer */
	register struct shmid_ds *rstOldest = 0;/* Oldest free segment */
	register int		 i;		/* Loop index */
	SEG			 *pstSeg;

	/* Check the requested segment size */
	if (iShmSize < 0 || iShmSize > SHMMAX) {
		u.u_error = EINVAL;
		return;
	}
	/* Init the shared memory on the first shmget. */
	if (shmids == NULL) 
		if (shminit()) {
			u.u_error = ENOSPC;
			return;
		}

	/* Search for desire shared memory segment. */
	for (rstShmId = shmids, i = 0; i < SHMMNI; i++, rstShmId++) {
		/* If segment is not alloced, we will look for the oldest
		 * free segment. We will use it to create a new one.
		 * The "oldest" will increase (a little) system reliability.
		 */
		if ((rstShmId->shm_perm.mode & IPC_ALLOC) == 0) {
			if ((rstOldest == NULL) || 
			   (rstOldest->shm_ctime > rstShmId->shm_ctime))
				rstOldest = rstShmId;
			continue;
		}
		/* Do we need a new segment? */
		if (kShmKey == IPC_PRIVATE)
			continue;
		/* Keep going if key is different. The key is an element
		 * number of shmids
		 */
		if (kShmKey != rstShmId->shm_perm.key)
			continue;

		/* We found the segment with requested key. */
		
		/* Request was for the exclusive segment should fail. */
		if ((iShmFlg & IPC_CREAT) && (iShmFlg & IPC_EXCL)) {
			u.u_error = EEXIST;
			return;
		}

		/* Check the requested size */
		if (rstShmId->shm_segsz < iShmSize) {
			u.u_error = EINVAL;
			return;
		}

		/* Check permissions */
		if (iShmPerm(rstShmId, iShmFlg))
			return;
		return i;
	}

	/* We need to create a new segment */
	if (rstOldest == 0) { /* Check system limits */
		u.u_error = ENOSPC;
		return;
	}
	if (!(iShmFlg & IPC_CREAT)) {
		u.u_error = ENOENT;
		return;
	}
	rstShmId = rstOldest;
	/* Allocate a new shared memory segment */
	pstSeg = shmAlloc(iShmSize);
	if (pstSeg == NULL) {
		u.u_error = ENOSPC;
		return;
	}
	/* Save it in shmsegs */
	shmsegs[rstShmId - shmids] = pstSeg;

	rstShmId->shm_perm.seq = (unsigned short) (rstShmId - shmids);
	rstShmId->shm_segsz = iShmSize;
	rstShmId->shm_atime = 0;
	rstShmId->shm_dtime = 0;
	rstShmId->shm_ctime = timer.t_time;
	rstShmId->shm_cpid  = SELF->p_pid;
	rstShmId->shm_perm.cuid = rstShmId->shm_perm.uid = u.u_uid;
	rstShmId->shm_perm.cgid = rstShmId->shm_perm.gid = u.u_gid;
	rstShmId->shm_perm.mode = (iShmFlg & 0777) | IPC_ALLOC;
	rstShmId->shm_perm.key  = kShmKey;

	if (kShmKey == IPC_PRIVATE)
		rstShmId->shm_perm.mode |= SHM_DEST;

	return rstShmId - shmids;
}

/*
 * Allocate space for shared memory data structures.
 */
int
shminit()
{
	shmids = (struct shmid_ds *) kalloc(sizeof(struct shmid_ds) * SHMMNI);
	if (shmids == NULL)
		return -1;

	/* Allocate array of shared memory segments. We do not have to
	 * initalise it
	 */
	shmsegs = (SEG *) kalloc(sizeof(SEG *) * SHMMNI);
	if (shmsegs == NULL)
		return -1;
	return 0;
}

/*
 * Attach shared memory segment.
 */
char *
ushmat(iSysId, pcShmAddr, iShmFlg)
int	iSysId;		/* System segment id */
char	*pcShmAddr;	/* Address to attach */
int	iShmFlg;	/* Flags */
{
	register PROC		*rpstProc;	/* Current process */
	register struct sr	*rpstSr;	/* Shared memory segments */
	struct sr		*pstSrTmp;
	SEG			*pstSegSh;	/* Segment to attach */
	struct shmid_ds		*pstShmId;	/* Pointer to a system segment*/
	unsigned int		uSegId;		/* Segment id */
	caddr_t			vAttAddr;	/* Address to attach */
	int			i;		/* Loop index */
	int			iReadOnly = 0;	/* 1 - read only, 0 - rw */
		
	/* Check if iSysId is a valid shared memory id. */
	if (iSysId < 0 || iSysId > SHMMNI) {
		u.u_error = EINVAL;
		return;
	}
	/* Do we really have this segment? */
	pstShmId = shmids + iSysId;
	if (pstShmId->shm_perm.seq != iSysId) {
		u.u_error = EINVAL;
		return;
	}
	/* Check permissions. */
	if (iShmFlg & SHM_RDONLY)
		iReadOnly = 1;
	if (iReadOnly) {
		if (iShmPerm(pstShmId, 0444)) {
			u.u_error = EACCES;
			return;
		}
	} else {
		if (iShmPerm(pstShmId, 0666)) {
			u.u_error = EACCES;
			return;
		}
	}
	/* Check if process has free shm index. */
	rpstProc = SELF;
	/* We will go through all NSHMSEG segments to see if any is free. */
	for (rpstSr = rpstProc->p_shmsr; rpstSr < rpstProc->p_shmsr + NSHMSEG; 
								rpstSr++)
		if (rpstSr->sr_segp == NULL)
			break;
	/* The segment id is just an element number. */
	uSegId = rpstSr - rpstProc->p_shmsr;
	/* If segment id is >= NSHMSEG we cannot attach any new segment. */
	if (uSegId >= NSHMSEG) {
		u.u_error = EMFILE;
		return;
	}
	/* Get the pointer to the shared memory segment */
	pstSegSh = shmsegs[iSysId];

	/* Find an address to attach.
	 * There are two cases: process does not request the address,
	 * process requests the address.
	 * In the first case we have to take the first available free address.
	 * We will try to put a free page between the segments.
	 * In the second case we have to check if address is an available and
	 * attach to this address.
	 */
	/* First case. We have to find a free address. */
	if (pcShmAddr == NULL) {
		/* Find a free space to attach.  */
		for (pstSrTmp = rpstProc->p_shmsr, i = 0; i < NSHMSEG; 
							i++, pstSrTmp++) 
			if (pstSrTmp->sr_base == NULL)
				break;
		/* Check limit of attaches per process */
		if (i >= NSHMSEG) {
			u.u_error = EINVAL;
			return;
		}
		/* We will use the addresses starting from SHMBASE. 
		 * Each new address can be SHMMAX + NBPC appart.
		 */
		vAttAddr = (caddr_t) (SHMBASE + i * (SHMMAX + NBPC));
	} else {
		/* Requst attach to a specific address. This is none portable 
		 * way to use a shared memory. 
	 	 */
		if ((vAttAddr = vCheckReqAdd(pcShmAddr, iReadOnly)) < 0) {
			printf("%s: attempt attach to 0x%x\n", 
			u.u_comm, pcShmAddr);
			u.u_error = EINVAL;
			return;
		} 
	}

	if (!shmAtt(uSegId, vAttAddr, pstSegSh, iReadOnly)) {
		u.u_error = EINVAL;
		return;
	}
	pstShmId->shm_lpid = SELF->p_pid;
	pstShmId->shm_atime = timer.t_time;
	pstShmId->shm_dtime = 0;
	pstShmId->shm_nattch = pstSegSh->s_urefc - 1;
	/* Keep all attached addresses. We will need them for detach */
	return (char *) vAttAddr;
}

/*
 * Check requested address for attach.
 * Just fail for the first release.
 */
caddr_t
vCheckReqAdd(pcAdd, iFlg)
char	*pcAdd;	/* Address to atatch */
int	iFlg;	/* Mode flag */
{
	return (caddr_t) -1;
}

/*
 * Check permissions of the shared memory segment.
 * Return 0 on success, -1 and set errno on error.
 */
int
iShmPerm(pstShmId, iShmFlg)
struct shmid_ds	*pstShmId;
int		iShmFlg;
{
	int	iShmMode;

	/* We need 9 lower order bits. There is a question what we have to do
	 * if someone sets an execute bits on. At this point we just ignore 
	 * them.
	 */
	iShmMode = iShmFlg & 0666;

	/* For superuser or if mode is 0 */
	if (u.u_uid == 0 || !iShmMode) 
		return 0;
	/* For owner or creator */
	if (u.u_uid == pstShmId->shm_perm.uid || u.u_uid 
						== pstShmId->shm_perm.cuid) {
		if ((iShmMode & pstShmId->shm_perm.mode) & 0600)
			return 0;
		else {
			u.u_error = EACCES;
			return -1;
		}
	}
	/* For group */		
	if (u.u_gid == pstShmId->shm_perm.gid 
					|| u.u_gid == pstShmId->shm_perm.cgid) {
		if ((iShmMode & pstShmId->shm_perm.mode) & 060)
			return 0;
		else {
			u.u_error = EACCES;
			return -1;
		}
	}
	/* For the rest of the world */
	if ((iShmMode & pstShmId->shm_perm.mode) & 06) 
		return 0;
	else {
		u.u_error = EACCES;
		return -1;
	}
	/* We should never come here */
	u.u_error = EACCES;
	return -1;
}

/*
 * ushmdt() - Detach shared memory segment.
 * Find segment number and call shmDetach() (shm0.c).
 */
int
ushmdt(cpShmAddr)
char	*cpShmAddr;	/* Pointer to a segment */
{
	register PROC		*rpstProc;	/* Current process */
	register struct sr	*rpstSr;	/* Shared memory segments */
	int			i;		/* Loop indexe */

	rpstProc = SELF;	/* Get pointer to our process */

	/* Go through all segments. */
	for (rpstSr = rpstProc->p_shmsr, i = 0; i < NSHMSEG; i++, rpstSr++) {
		if (rpstSr->sr_base == (caddr_t) cpShmAddr) {
 			shmDetach(i);
                        return 0;
		}
	}

	/* We can come here only if we have invalid address */
	u.u_error = EINVAL;
	return;
}

/*
 * shmSetDs(). Called from shm0.c.
 *
 * Given a pointer to shared memory segment, set shmid_ds.
 */
void
shmSetDs(rpstSeg)
register SEG	*rpstSeg;
{
	struct shmid_ds		*pstShmId;	/* Shared memory structure */
	int			iShmId;		/* Shared memory id */
	int			j;		/* Loop indexe */

	for (j = 0; j < SHMMNI; j++)
		if (shmsegs[j] == rpstSeg)
			break;

	/* We should have this segment. */
	if (j >= SHMMNI) {
		u.u_error = EINVAL;
		return;
	}

	iShmId = j;
	pstShmId = shmids + iShmId;

	/* Set proper values */
	pstShmId->shm_lpid = SELF->p_pid;
	pstShmId->shm_dtime = timer.t_time;
	pstShmId->shm_nattch = rpstSeg->s_urefc - 1;
	return;
}
