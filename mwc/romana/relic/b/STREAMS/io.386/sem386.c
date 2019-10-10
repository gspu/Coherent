/*
 * File: sem1.c
 *
 * Purpose: This module provides System V compatible semaphore operations.
 *
 * $Log:	sem386.c,v $
 * Revision 2.2  93/07/26  15:32:16  nigel
 * Nigel's R80
 * 
 * Revision 1.1  93/04/09  08:48:23  bin
 * Initial revision
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/sched.h>
#if 0
#include <sys/proc.h>
#endif
#include <sys/types.h>
#include <sys/uproc.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/con.h>
#include <sys/sem.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int	iSemPerm();	/* Check permissions */
int	iSanityCheck();	/* Sanity check */
int	iSemInit();	/* Init semaphores */
void	vClearAdj();	/* Clear the adjust value */
int	iSubAdj();	/* Subtract value from the sem adjust */
/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
/* Patchable values */
int	SEMMNI = 10;	/* max # of the semaphore sets, systemwide */
int	SEMMNS = 60;	/* max # of semaphores, systemwide */
int	SEMMSL = 25;	/* max # of semaphores per set */
int	SEMVMX = 32767;	/* max value of any semaphore */
	
struct semid_ds	*semids = NULL;	/* Array of semaphore sets */
int		iSemTotal = 0;	/* Total number of semaphores, systemwide */
GATE		gSemGate;	/* Semaphore gate. */
unsigned short	usSEM_R = 0444;	/* Permission definition for read */
unsigned short	usSEM_A = 0222;  /* and alter */

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * Semget gets set of semaphores. Returns semaphore set id on suuccess,
 * or sets u.u_error on error.
 */
usemget(skey, nsems, semflg)
key_t 	skey;		/* Semaphore key */
int 	nsems,		/* # of semaphores in the set */
	semflg;		/* Permission flag */
{
	register struct semid_ds	*semidp;	/* Semaphore set */
	register struct sem 		*semp;		/* Semaphores */
	struct semid_ds			*freeidp = 0;	/* Oldest free set */

	if (iSanityCheck(nsems))
		return;

	/* Allocate memmory on the first semget. This memory (~300 bytes)
	 * will stay alloc up to the next reboot. The alloced unused memory
	 * is smaller than code that will allow to manage it more sophisticated.
	 * Allocaton is used to allow patchability of the semaphores. 
	 */
	if (semids == NULL) {
		if (iSemInit()) {
			u.u_error = ENOSPC;
			return;
		}
	}
	lock(gSemGate);			/* Lock semaphore operations */

	/* Now we will go through all semaphores. */
	for (semidp = semids; semidp < semids + SEMMNI; semidp++) {
		/* If set is free look for the oldest */
		if ((semidp->sem_perm.mode & IPC_ALLOC) == 0) {
			if ((freeidp == 0) ||
			    (freeidp->sem_ctime > semidp->sem_ctime))
				freeidp = semidp;
			continue;
		}

		/* Check if a request was for the private set */
		if (skey == IPC_PRIVATE)
			continue;

		if (skey != semidp->sem_perm.key)
			continue; 
		/* Found */
		/* Exclusive set cannot be created */
		if ((semflg & IPC_CREAT) && (semflg & IPC_EXCL)) {
			unlock(gSemGate);
			u.u_error = EEXIST;
			return;
		}

		/* Check the permissions */
		if (iSemPerm(semidp, semflg)) {
			unlock(gSemGate);
			return;
		}

		/* Check the requested number of semaphores */
		if (semidp->sem_nsems < nsems) {
			unlock(gSemGate);
			u.u_error = EINVAL;
			return;
		}
		/* Semaphore set id number is the number of an array element */
		unlock(gSemGate);
		return semidp - semids;
	}

	/* Set does not exist. So, we have to create it */
	/* Now nsems should be > 0 */
	if (nsems < 1) {
		unlock(gSemGate);
		u.u_error = EINVAL;
		return;
	}

	/* Check the total number of semaphores */
	if ((iSemTotal + nsems > SEMMNS)) {
		unlock(gSemGate);
		u.u_error = ENOSPC;
		return;
	}

	/* Check if there is the request for creation */
	if (!(semflg & IPC_CREAT)) {
		unlock(gSemGate);
		u.u_error = ENOENT;
		return;
	}

	/* Out of system limits */
	if (freeidp == 0) {
		unlock(gSemGate);
		u.u_error = ENOSPC;
		return;
	}

	/* Now we have to creat a new set. freeidp points to the oldest free
	 * set which we will use.
	 */
	semidp = freeidp;
	/* Get space for semaphores */
	semidp->sem_base = kalloc(nsems * sizeof(struct sem));
	if (semidp->sem_base == 0) {
		unlock(gSemGate);
		u.u_error = ENOSPC;
		return;
	}

	/* Initialize created set */
	/* ipc_perm */
	semidp->sem_perm.cuid = semidp->sem_perm.uid = u.u_uid;
	semidp->sem_perm.cgid = semidp->sem_perm.gid = u.u_gid;
	semidp->sem_perm.mode = (semflg & 0777) | IPC_ALLOC;
	semidp->sem_perm.key  = skey;
	semidp->sem_perm.seq = semidp - semids;
	
	semidp->sem_nsems = nsems;
	semidp->sem_otime = 0;
	semidp->sem_ctime = timer.t_time;

	/* Increment number of semaphores in used */
	iSemTotal += nsems;

	/* Set values of the semaphores to 0.
	 * SVR3 does not do it and suggests set up sem struct using
	 * semctl() call. SVR4 manual says nothing about it.
	 */
	for (semp = semidp->sem_base; semp < semidp->sem_base + nsems; semp++){
		semp->semval = semp->sempid = semp->semncnt = semp->semzcnt = 0;
	}
	unlock(gSemGate);
	return semidp - semids;
}

/*
 * Semctl provides semaphore control operation as specify by cmd.
 * On success return value depends on cmd, sets u.u_error on error.
 */
usemctl(semid, semnum, cmd, arg)
int	semid,		/* Semaphore set id */
	cmd;		/* Command */
int	semnum;		/* Semaphore # */
union semun {
	int		val;	/* Used for SETVAL only */
	struct semid_ds *buf;	/* Used for IPC_STAT and IPC_SET */
	unsigned short 	*array;	/* Used for IPC_GET_ALL and IPC_SETALL */
} arg;

{
	register struct semid_ds	*semidp;	/* Semaphore set */
	register struct sem		*semp;		/* Semaphore */
	int 				val;		/* Semaphore value */
	int				i;		/* Loopindex */
	unsigned short			*pusUserAr;	/* User array */

	if (iSanityCheck(semnum))
		return;

	/* Check if there was any successfull semget before.
	 * Problem may be if somebody does semctl() before semids was
	 * alloced.
	 */
	if (semids == 0) {
		u.u_error = EINVAL;
		return;
	}

	/* semid cannot be < 0 and more than systemwide limit */
	if (semid < 0 || semid >= SEMMNI) {
		u.u_error = EINVAL;
		return;
	}
	semidp = semids + semid;
	
	/* Check if the requested set is alloced */
	if ((semidp->sem_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EINVAL;
		return;
	}

	/* Check if semnum is a correct semaphore number.
	 * SV would do it only when there is request for a
	 * single semaphore value, as GETVAL or SETVAL.
	 */
	if (semnum >= semidp->sem_nsems) {
		u.u_error = EFBIG;
		return;
	}

	semp = semidp->sem_base + semnum;

	switch (cmd) {
	case GETVAL:		/* Return value of semval */
		if (iSemPerm(semidp, usSEM_R))	/* cannot read */
			return;
		return semp->semval;
	case SETVAL:	/* Set semval. Clear all semadj values on success. */
		if (iSemPerm(semidp, usSEM_A))		/* cannot alter */
			return;
		/* semval always >= 0 */
		if (arg.val > SEMVMX || arg.val < 0) {	/* illegal value */
			u.u_error = ERANGE;
			return;
		}
		/* Set semval and wakeup whatever should be */
		if (semp->semval = arg.val) {
			if (semp->semncnt)
				wakeup(&semp->semncnt);
		} else {
			if (semp->semzcnt)
				wakeup(&semp->semzcnt);
		}
		semidp->sem_ctime = timer.t_time;
		/* Clear corresponding adjust value in all processes */
		vClearAdj(semid, semnum);
		return 0;
	case GETPID:		/* Return value of sempid */
		if (iSemPerm(semidp, usSEM_R))	/* cannot read */
			return;
		return semp->sempid;
	case GETNCNT:		/* Return value of semncnt */
		if (iSemPerm(semidp, usSEM_R))	/* cannot read */
			return;
		return semp->semncnt;
	case GETZCNT:		/* Return value of semzcnt */
		if (iSemPerm(semidp, usSEM_R))	/* cannot read */
			return;
		return semp->semzcnt;
	case GETALL:		/* Return semvals array */
		if (iSemPerm(semidp, usSEM_R))	/* cannot read */
			return;
		/* Copy all values to user array */
		semp  = semidp->sem_base;
		pusUserAr = arg.array;
		for (i = 0; i < semidp->sem_nsems; i++) {
			putusd(pusUserAr, semp->semval);
			if (u.u_error)
				return;
			semp++;
			pusUserAr++;
		}
		return 0;
	case SETALL:		/* Set semvals array */
		if (iSemPerm(semidp, SEM_A))	/* cannot alter */
			return;

		/* Set semvals accoding to the arg.array */
		semp  = semidp->sem_base;
		pusUserAr = arg.array;
		lock(gSemGate);
		for (i = 0; i < semidp->sem_nsems; i++) {
			val = getusd(pusUserAr++);
			if (u.u_error) {
				unlock(gSemGate);
				return;
			}
			if (val < 0 || val > SEMVMX) {
				u.u_error = ERANGE;
				unlock(gSemGate);
				return;
			}
			semp->semval = val;
			/* Clear corresponding adjust value in all processes. */
			vClearAdj(semid, i);
			semp++;
		}
		semidp->sem_ctime = timer.t_time;
		unlock(gSemGate);
		return 0;
	case IPC_STAT:
		if (iSemPerm(semidp, usSEM_R))	/* cannot read */
			return;
		kucopy(semidp, arg.buf, sizeof(struct semid_ds));
		return 0;
	case IPC_SET:
		if (iSemPerm(semidp, SEM_A))	/* cannot alter */
			return;
		semidp->sem_perm.uid   = getusd(&((arg.buf)->sem_perm.uid));
		semidp->sem_perm.gid   = getusd(&((arg.buf)->sem_perm.gid));
		semidp->sem_perm.mode  =
			(getusd(&((arg.buf)->sem_perm.mode))&0777) | IPC_ALLOC;
		semidp->sem_ctime = timer.t_time;
		return 0;
	case IPC_RMID:
		if ((u.u_uid != 0) && (u.u_uid != semidp->sem_perm.uid)
				&& u.u_uid != semidp->sem_perm.cuid) {
			u.u_error = EPERM;
			return;
		}

		/* We have to wake up all waiting proccesses  */
		for (semp = semidp->sem_base; semp < semidp->sem_base +
				semidp->sem_nsems; semp++) {
			if (semp->semncnt)
				wakeup(&semp->semncnt);
			if (semp->semzcnt)
				wakeup(&semp->semzcnt);
		}
		/* We do not cleane up adjust values here */
		iSemTotal -= semidp->sem_nsems;
		semidp->sem_perm.mode = 0;
		kfree(semidp->sem_base);
		return 0;
	default:
		u.u_error = EINVAL;
		return;
	}
}

/*
 * Semop - Semaphore Operations.
 */
usemop(iSemId, pstSops, uNsops)
int		iSemId;		/* Semaphore identifier */
struct sembuf	*pstSops;	/* Array of sem. operations */
unsigned 	uNsops;		/* # of elements in the array */
{
	register struct semid_ds	*rpstSemSet;	/* Semaphore set */
	register struct sem 		*rpstSem;	/* Semaphore */
	struct sembuf 			*pstSemBuf;	/* Operations */
	unsigned short 			usSemNum; 	/* Semaphore number */
	short				sSemFlg;	/* Semaphore flag */
	short 				sSemOper; 	/* Operation */
	int				i;		/* Loop index */
	short				change = 0;	/* Sem was changed */

	/* Check if semids was alloced */
	if (semids == 0) {
		u.u_error = EINVAL;
		return;
	}
	/* iSemId cannot be < 0 and more than systemwide limit */
	if (iSemId < 0 || iSemId >= SEMMNI) {
		u.u_error = EINVAL;
		return;
	}
	if (!useracc(pstSops, sizeof(struct sembuf) * uNsops, 0) || uNsops<1) {
		u.u_error = EFAULT;
		return;
	}
	
	rpstSemSet = semids + iSemId;	/* Requested set */

	if ((rpstSemSet->sem_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EINVAL;
		return -1;
	}
TRY_AGAIN:	/* Repeat the semaphore set */

	/* Lock semaphore system */
	lock(gSemGate); 

	/* do semaphore ops  */
	for (i = 0, pstSemBuf = pstSops; i < uNsops; i++, pstSemBuf++) {
		usSemNum = getusd(&(pstSemBuf->sem_num));
		sSemOper = getusd(&(pstSemBuf->sem_op));
		sSemFlg = getusd(&(pstSemBuf->sem_flg));

		if (sSemOper < -SEMVMX || sSemOper > SEMVMX) {
			semundo(rpstSemSet, pstSops, i);
			u.u_error = E2BIG;
			unlock(gSemGate); 
			return;
		}
		if ((u.u_error != 0) || (usSemNum >= rpstSemSet->sem_nsems)) {
			/* We have falure here. undo all previous 
			 * operations.
			 */
			semundo(rpstSemSet, pstSops, i);
			/* If u_error was not set it means that sem_number 
			 * is bad. So, set error to EFBIG.
			 */
			if (u.u_error == 0)
				u.u_error = EFBIG;
			unlock(gSemGate); 
			return;
		}

		/* Go to proper semaphore */
		rpstSem = rpstSemSet->sem_base + usSemNum;
	
		/* We can have 3 different cases: sSemOper < 0,
		 * sSemOper == 0, & sSemOper > 0.
		 */
		if (sSemOper < 0) {	/* want to decrement semval */
			/* We do not have to undo anything. If we cannot alter
			 * we did not change any value in the set. But we
			 * have to check here because other requests could be
			 * for read (sSemOper=0)
			 */	
			if (iSemPerm(rpstSemSet, SEM_A)) { /* cannot alter */
				unlock(gSemGate); 
				return;
			}

			/* If we can decrement semval, do it. If
			 * semval becomes 0 wakeup all processes
			 * waiting for semval==0.
			 */
			if (rpstSem->semval >= -sSemOper) {
				if (!(rpstSem->semval += sSemOper))
		 			if (rpstSem->semzcnt)
						wakeup(&rpstSem->semzcnt);
				if (sSemFlg & SEM_UNDO) {
					if (iSubAdj(iSemId,usSemNum,sSemOper)) {
						semundo(rpstSemSet, pstSops, i);
						unlock(gSemGate); 
						return;
					}
				}
				change++;
				continue;
			}
			/* Can't decrement. */
			semundo(rpstSemSet, pstSops, i);
			if (sSemFlg & IPC_NOWAIT) {
				if (u.u_error == 0)
					u.u_error = EAGAIN;
				unlock(gSemGate); 
				return;
			} else {/* Go to sleep */
				unlock(gSemGate); 
				if (semwait(iSemId, &rpstSem->semncnt) < 0) {
					return;
				}
				/* Now we can retry semaphore set */
					goto TRY_AGAIN;
			}
		} 
		if (sSemOper == 0) {
			if (iSemPerm(rpstSemSet, SEM_R)) { /* cannot read */
				/* If somebody cannot read it does not
				 * mean that one couldn't alter.
				 */
				if (iSemPerm(rpstSemSet, SEM_A))
					semundo(rpstSemSet, pstSops, i);
				
				unlock(gSemGate); 
				return;
			}
			if (rpstSem->semval == 0) {
				continue;
			}
			/* Semaphore value isn't 0. Undo all previous
			 * operations.
			 */
			semundo(rpstSemSet, pstSops, i);

			if (sSemFlg & IPC_NOWAIT) {
				if (u.u_error == 0)
					u.u_error = EAGAIN;
				unlock(gSemGate); 
				return;
			}

			unlock(gSemGate); 
			if (semwait(iSemId, &rpstSem->semzcnt) < 0) {
				return;
			}
			goto TRY_AGAIN;
		} 
		if (sSemOper > 0) {
			if (iSemPerm(rpstSemSet, SEM_A)) { /* cannot alter */
				unlock(gSemGate); 
				return;
			}
			if (sSemFlg & SEM_UNDO) {
				if (iSubAdj(iSemId, usSemNum, sSemOper)) {
					semundo(rpstSemSet, pstSops, i);
					unlock(gSemGate); 
					return;
				}
			}
			if (rpstSem->semval > SEMVMX - sSemOper) {
				semundo(rpstSemSet, pstSops, i);
				/* semundo would not adjust current semaphore */
				if (sSemFlg & SEM_UNDO) {
					iSubAdj(iSemId, usSemNum, -sSemOper);
				}
				u.u_error = ERANGE;
				unlock(gSemGate); 
				return;
			}
			rpstSem->semval += sSemOper;

			/* Wake up waiting processes */
			if (rpstSem->semncnt)
				wakeup(&rpstSem->semncnt);
			change++;
			continue;
		}
	}
	rpstSemSet->sem_otime = timer.t_time;	/* adjust semop time */
	if (change) /* Semaphore was changed */
		rpstSemSet->sem_ctime = timer.t_time;
	
	/* Go through all set again and set pid of last semop */
	for (i = 0, pstSemBuf = pstSops; i < uNsops; i++, pstSemBuf++) {
		usSemNum = getusd(&(pstSemBuf->sem_num));
		rpstSem = rpstSemSet->sem_base + usSemNum;
		rpstSem->sempid = SELF->p_pid;
	}
	unlock(gSemGate);
	return 0;				/* return last prev semval */
}

/*
 * Wait for an event.
 */
semwait(iSemId, usSleepEvent)
int		iSemId;		/* Semaphore set id */
unsigned short	*usSleepEvent;	/* Could be semcnt or semzcnt */
{
	register struct	semid_ds	*rpstSemSet;	/* Semaphore set */

	(*usSleepEvent)++;
	
	rpstSemSet = semids + iSemId;
	
 	x_sleep(usSleepEvent, pritty, slpriSigCatch, "semwait");

	if (!(rpstSemSet->sem_perm.mode & IPC_ALLOC)) {	/* Semaphore gone */
		u.u_error = EIDRM;
		return -1;
	}
	(*usSleepEvent)--;

	if (nondsig ()) {	/* Signal received */
		u.u_error = EINTR;
		return -1;
	}
	return 0;
}
	
/*
 * Undo a Semaphore Operation. It should undo an adjust values too.
 */
semundo(pstSemSet, pstSemOp, iUndo)
struct semid_ds	*pstSemSet;	/* Pointer to the semaphore set */
struct sembuf	*pstSemOp;	/* Pointer to the undo operation */
int		iUndo;		/* Number of semaphores to undo */
{
	register struct sem	*rpstSem;	/* */
	register struct sembuf	*rpstBuf;	/* */
	register int		i;		/* Loop index */
	int			iSemId;		/* Semaphore id */
	unsigned short		usSemNum;	/* Semaphore number */
	short			sSemOper;	/* Value to undo */
	short			sSemFlg;	/* Semaphore flag */

	rpstSem = pstSemSet->sem_base;
	rpstBuf = pstSemOp;
	iSemId = pstSemSet - semids;
	for (i = 0; i < iUndo; i++) {
		usSemNum = getusd( &(rpstBuf->sem_num) );
		sSemOper  = getusd( &(rpstBuf->sem_op) );
		sSemFlg  = getusd( &(rpstBuf->sem_flg) );
		rpstSem->semval -= sSemOper;
		if (sSemFlg & SEM_UNDO) {
			iSubAdj(iSemId, usSemNum, -sSemOper);
		}
		rpstBuf++;
		rpstSem++;
	}		
}

/*
 * Check permissions of the semaphore set.
 * Return 0 on success, -1 and set errno on error.
 */
int iSemPerm(pstSemId, iSemFlg)
struct semid_ds	*pstSemId;	/* Pointer to the semaphor set */
int		iSemFlg;	/* Requested permissions */
{
	int	iSemMode;

	/* Check if resource is alloced */
	if ((pstSemId->sem_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EINVAL;
		return -1;
	}
		
	/* We need 9 lower order bits. There is a question what we have to do
	 * if someone sets an execute bits on. At this point we just ignore 
	 * them.
	 */
	iSemMode = iSemFlg & 0666;

	/* For superuser or if mode is 0 */
	if (u.u_uid == 0 || !iSemMode) 
		return 0;

	/* For owner or creator */
	if (u.u_uid == pstSemId->sem_perm.uid || u.u_uid 
						== pstSemId->sem_perm.cuid) {
		if ((iSemMode & pstSemId->sem_perm.mode) & 0600)
			return 0;
		else {
			u.u_error = EACCES;
			return -1;
		}
	}
	/* For group */		
	if (u.u_gid == pstSemId->sem_perm.gid 
					|| u.u_gid == pstSemId->sem_perm.cgid) {
		if ((iSemMode & pstSemId->sem_perm.mode) & 060)
			return 0;
		else {
			u.u_error = EACCES;
			return -1;
		}
	}
	/* For the rest of the world */
	if ((iSemMode & pstSemId->sem_perm.mode) & 06) 
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
 * Allocate and clear space for semapohore sets
 * Return 0 on success, -1 and set errno on error.
 */
iSemInit()
{
	unsigned	uSize;		/* Size of the alloc memmory */

	uSize = sizeof(struct semid_ds) * SEMMNI;

	if ((semids = (struct semid_ds *) kalloc(uSize)) == NULL)
		return -1;
	memset((char *) semids, 0, uSize);
	return 0;
}

/*
 * Check if semaphore number is a valid number.
 */
iSanityCheck(iSemNumber)
int	iSemNumber;	/* Requested number of the semaphores in the set */
{
	/* Just to be on safe side */
	if (u.u_error)
		return -1;

	/* Check if we are inside system limits. */
	if (iSemNumber >= SEMMSL || iSemNumber < 0) {
		u.u_error = EINVAL;
		return -1;
	}
	return 0;
}

/*
 * Subtract iValue from the adjust value for the specified
 * semaphore for the specified process.
 */
int iSubAdj(iSemId, usSemNum, sValue)
int		iSemId;		/* Semaphore set id */
unsigned short	usSemNum;	/* Semaphore number */
short		sValue;		/* Adjust value */
{
	PROC		*pp;		/* Current process */
	struct sem_undo	*unPrev, 	/* Previous and next pointers to */
			*unNext;	/* sem_undo structures link list */
	int		newAdjust;	/* New adjust value */

	pp = SELF;	/* Get the current process */

	/* Look if adjust value for semaphore was set */
	for (unNext = pp->p_semu; unNext != NULL; unNext = unPrev->un_np) {
		if (unNext->un_num == usSemNum && unNext->un_id == iSemId) {
			newAdjust = unNext->un_aoe - sValue;
			if (newAdjust < -SEMVMX || newAdjust > SEMVMX) {
				u.u_error = ERANGE;
				return -1;
			}
			unNext->un_aoe = newAdjust; /* Found and adjust */
			return 0;
		}
		unPrev = unNext;
	}
	/* There is no adjust value for semaphore.
	 * We have to allocate space and creat a new adjust value.
	 */
	if ((unNext = kalloc(sizeof(struct sem_undo))) == NULL) {
		u.u_error = ENOSPC;
		return -1;
	}
	/* Set values for the next entry */
	unNext->un_np = NULL;
	unNext->un_aoe -= sValue;
	unNext->un_num = usSemNum;
	unNext->un_id = iSemId;
	
	/* Put new entry at the end of link list */
	if (pp->p_semu == NULL)
		pp->p_semu = unNext;	/* New link list */
	else				/* Add entry to the existing list */
		unPrev->un_np = unNext;	/* unPrev is a last entry in the list */
	return 0;
}

/*
 * Clear adjust value for all process.
 */
void	vClearAdj(iSemId, usSemNum)
int	iSemId;		/* Semaphore set id */
int	usSemNum;	/* Semaphore number */
{
	PROC		*pp;		/* process */
	struct sem_undo	*unNext; 	/* sem_undo structures */

	/* Go through all processes and zero the proper undo entry */
	for (pp = &procq; (pp = pp->p_nforw) != &procq; ) {
		/* Look if adjust value for semaphore was set. */
		for (unNext = pp->p_semu; unNext != NULL; 
						unNext = unNext->un_np) {
			if (unNext->un_num == usSemNum 
						&& unNext->un_id == iSemId)
				unNext->un_aoe = 0;	/* Found */
		}
	}		
}

/*
 * Adjust all semaphores and remove sem_undo link list.
 */
semAllAdjust(pp)
PROC	*pp;
{
	struct sem_undo	*unPrev, 	/* Previous and next pointers to */
			*unNext;	/* sem_undo structures link list */
	struct semid_ds	*pstSemSet;	/* Semaphore set */
	struct sem	*pstSem;	/* Semaphore */

	if ((unNext = pp->p_semu) == NULL)
		return;

	while (unNext != NULL) {
		pstSemSet = semids + unNext->un_id;	/* Requested set */
		pstSem = pstSemSet->sem_base + unNext->un_num;
		pstSem->semval += unNext->un_aoe;
		pstSem->sempid = SELF->p_pid;
		pstSemSet->sem_ctime = timer.t_time;
		unPrev = unNext;
		unNext = unPrev->un_np;
		kfree(unPrev);
	}
	/*kfree(pp->p_semu);*/
	pp->p_semu = NULL;
}
