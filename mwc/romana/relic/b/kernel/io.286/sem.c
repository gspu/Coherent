/* $Header: /kernel/kersrc/io.286/sem.c,v 1.1 92/07/17 15:24:44 bin Exp Locker: bin $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1985
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/*
 * System V Compatible Semaphores
 *
 *	This module provides System V compatible semaphore operations.
 *
 *			Author: Allan Cornish, INETCO Systems Ltd., Sep 1984
 *
 * $Log:	sem.c,v $
 * Revision 1.1  92/07/17  15:24:44  bin
 * Initial revision
 * 
 * Revision 2.1	88/09/03  13:11:37	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:06:22	src
 * Initial revision
 * 
 * 85/08/06	Allan Cornish
 * Sem.c split into configuration (semcon.c) and implementation (sem.c).
 * Semload() renamed to seminit().
 *
 * 85/07/22	Allan Cornish
 * Semctl, semget, semop now return immediately if u.u_error is set.
 *
 * 85/07/03	Allan Cornish
 * Replaced use of EDOM with EIDRM.
 * Eliminated semlock() and semunlock() functions.
 * Replaced semaccess() by calls to ipcaccess(), increasing shared ipc code.
 */

#include <coherent.h>
#include <sched.h>
#include <types.h>
#include <errno.h>
#include <stat.h>
#include <con.h>
#include <sem.h>

#ifndef	EIDRM
#define	EIDRM	EDOM
#endif

/*
 *	Semaphore Information
 */

#define	NSEMVAL	32767

static
struct semid_ds *semids = 0;

unsigned NSEMID = 16;
unsigned NSEM   = 16;
		    
/*
 * Semaphore Initialization.
 *
 *	Initialize semaphore ids.
 */

seminit()
{
	register struct semid_ds *semidp;

	if ( semids = kalloc( NSEMID * sizeof(struct semid_ds) ) ) {

		for ( semidp = &semids[NSEMID]; --semidp >= semids; )
			semidp->sem_perm.mode = 0;
	}
	else {
		printf("could not allocate %u semaphore ids\n", NSEMID);
		NSEMID = 0;
	}
}

/*
 * Semctl - Semaphore Control Operations.
 */

usemctl( semid, semnum, cmd, arg )

unsigned semid;
unsigned semnum;
int cmd;
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
} arg;

{
	register struct semid_ds *semidp;
	register struct sem *semp;
	int nsems, val;

	if ( u.u_error )
		return -1;

	if ( semid >= NSEMID ) {
		u.u_error = EINVAL;
		return -1;
	}

	semidp = &semids[semid];

	if ( (semidp->sem_perm.mode & IPC_ALLOC) == 0 ) {
		u.u_error = EINVAL;
		return -1;
	}

	if ((ipcaccess( &semidp->sem_perm ) & SEM_R) == 0) { /* can't read */
		u.u_error = EACCES;
		return -1;
	}

	if ( semnum >= semidp->sem_nsems ) {
		u.u_error = EFBIG;
		return -1;
	}

	semp = &semidp->sem_base[semnum];

	switch ( cmd ) {

	case GETVAL:		/* Return value of semval */
		return semp->semval;

	case SETVAL:						/* Set semval */
		if ((ipcaccess( &semidp->sem_perm ) & SEM_A) == 0) {
			u.u_error = EACCES;	/* can't alter */
			return -1;
		}

		if ( arg.val < 0 ) {			/* illegal value */
			u.u_error = ERANGE;
			return -1;
		}

		if ( semp->semval = arg.val ) {

			if ( semp->semncnt )
				wakeup( &semp->semncnt );
		}
		else {
			if ( semp->semzcnt )
				wakeup( &semp->semzcnt );
		}

		return 0;

	case GETPID:		/* Return value of sempid */
		return semp->sempid;

	case GETNCNT:		/* Return value of semncnt */
		return semp->semncnt;

	case GETZCNT:		/* Return value of semzcnt */
		return semp->semzcnt;

	case GETALL:		/* Return semvals array */
		nsems = semidp->sem_nsems;
		semp  = semidp->sem_base;

		while ( --nsems >= 0 ) {

			putuwd( (arg.array)++, (semp++)->semval );

			if ( u.u_error )
				return -1;
		}
		return 0;

	case SETALL:		/* Set semvals array */
		if ( (ipcaccess( &semidp->sem_perm ) & SEM_A) == 0 ) {
			u.u_error = EACCES;
			return -1;
		}
		nsems = semidp->sem_nsems;
		semp  = semidp->sem_base;

		while ( --nsems >= 0 ) {

			if ( (val = getuwd( arg.array )) < 0 ) {

				if ( u.u_error == 0 )
					u.u_error = ERANGE;
			}
			else
				semp->semval = val;
			arg.array++;
			semp++;
		}
		if ( u.u_error )
			return -1;
		return 0;

	case IPC_STAT:
		kucopy( semidp, arg.buf, sizeof(struct semid_ds) );
		return 0;

	case IPC_SET:
		if ( (u.u_uid != 0) && (u.u_uid != semidp->sem_perm.uid) ) {
			u.u_error = EPERM;
			return -1;
		}
		semidp->sem_perm.uid   = getuwd( &((arg.buf)->sem_perm.uid ) );
		semidp->sem_perm.gid   = getuwd( &((arg.buf)->sem_perm.gid ) );
		semidp->sem_perm.mode  =
			(getuwd(&((arg.buf)->sem_perm.mode))&0777) | IPC_ALLOC;
		return 0;

	case IPC_RMID:
		if ( (u.u_uid != 0) && (u.u_uid != semidp->sem_perm.uid) ) {
			u.u_error = EPERM;
			return -1;
		}
		semidp->sem_perm.seq++;
		semp = &semidp->sem_base[ semidp->sem_nsems ];

		while ( --semp >= semidp->sem_base ) {

			if ( semp->semncnt )
				wakeup( &semp->semncnt );
			if ( semp->semzcnt )
				wakeup( &semp->semzcnt );
		}

		kfree( semidp->sem_base );
		semidp->sem_perm.mode = 0;
		return 0;

	default:
		u.u_error = EINVAL;
		return -1;
	}
}

/*
 * Semget - Get set of semaphores
 */

usemget( skey, nsems, semflg )

key_t skey;
unsigned nsems;
int semflg;

{
	register struct semid_ds *semidp;
	register struct sem *semp;
	struct semid_ds *freeidp = 0;

	if ( u.u_error )
		return -1;

	if ( nsems >= NSEM ) {
		u.u_error = EINVAL;
		return -1;
	}

	for ( semidp = &semids[NSEMID]; --semidp >= semids; ) {

		if ( (semidp->sem_perm.mode & IPC_ALLOC) == 0 ) {

			if ((freeidp == 0) ||
			    (freeidp->sem_ctime > semidp->sem_ctime))
				freeidp = semidp;
			continue;
		}

#ifdef	IPC_PRIVATE
		if (skey == IPC_PRIVATE)
			continue;
#endif

		if (skey == semidp->sem_perm.key) {		/* found! */

			if ( (semflg & IPC_CREAT) && (semflg & IPC_EXCL) ) {
				u.u_error = EEXIST;
				return -1;
			}

			if ((semidp->sem_perm.mode & semflg) != (semflg&0777)) {
				u.u_error = EACCES;
				return -1;
			}

			if ( semidp->sem_nsems < nsems ) {
				u.u_error = EINVAL;
				return -1;
			}

			return semidp - semids;
		}
	}

	if ( !(semflg & IPC_CREAT) ) {
		u.u_error = ENOENT;
		return -1;
	}

	if ( freeidp == 0 ) {
		u.u_error = ENOSPC;
		return -1;
	}

	semidp = freeidp;
	semidp->sem_base = kalloc( nsems * sizeof(struct sem) );

	if (semidp->sem_base == 0 ) {
		u.u_error = ENOSPC;
		return -1;
	}

	semidp->sem_nsems = nsems;
	semidp->sem_otime = 0;
	semidp->sem_ctime = timer.t_time;

	for ( semp = &semidp->sem_base[nsems]; --semp >= semidp->sem_base; )
		semp->semval = semp->sempid = semp->semncnt = semp->semzcnt = 0;

	semidp->sem_perm.cuid = semidp->sem_perm.uid = u.u_uid;
	semidp->sem_perm.cgid = semidp->sem_perm.gid = u.u_gid;
	semidp->sem_perm.mode = (semflg & 0777) | IPC_ALLOC;
	semidp->sem_perm.key  = skey;

	return semidp - semids;
}

/*
 * Semop - Semaphore Operations.
 */

usemop( semid, sops, nsops )

unsigned semid;
struct sembuf *sops;
unsigned nsops;

{
	register struct semid_ds *semidp;
	register struct sem *semp;
	struct sembuf *sp;
	unsigned n, semnum, semflg;
	int semop, oval;

	if ( u.u_error )
		return -1;

	if ( semid >= NSEMID ) {
		u.u_error = EINVAL;
		return -1;
	}

	if ( nsops >= NSEM ) {
		u.u_error = E2BIG;
		return -1;
	}

	semidp = &semids[semid];

	if ( (semidp->sem_perm.mode & IPC_ALLOC) == 0 ) {
		u.u_error = EINVAL;
		return -1;
	}

	if ( (ipcaccess( &semidp->sem_perm ) & SEM_A) == 0 ) {
		u.u_error = EACCES;
		return -1;
	}

	sp = sops;
	n  = nsops;

	while ( n > 0 ) {				/* do semaphore ops  */

		semnum = getuwd( & (sp->sem_num) );
		semop  = getuwd( & (sp->sem_op ) );
		semflg = getuwd( & (sp->sem_flg) );

		if ( (u.u_error != 0) || (semnum >= semidp->sem_nsems) ) {

			while ( --sp >= sops ) {	/* undo prev semops  */

				semnum = getuwd( &sp->sem_num );
				semop  = getuwd( &sp->sem_op  );
				semp   = &semidp->sem_base[ semnum ];
				semundo( semp, semop );
			}

			if ( u.u_error == 0 )
				u.u_error = EFBIG;
			return -1;
		}

		semp = &semidp->sem_base[semnum];

		if ( (oval = semdo( semp, semop )) < 0 ) { /* can't do semop */

			while ( --sp >= sops ) {	/* undo prev semops  */

				unsigned unnum = getuwd( &sp->sem_num );
				int	 unop  = getuwd( &sp->sem_op  );
				semundo( &semidp->sem_base[ unnum ], unop);
			}

			if ( u.u_error )
				return -1;

			if ( semflg & IPC_NOWAIT ) {
				u.u_error = EAGAIN;
				return -1;
			}

			if ( semop < 0 ) {	/* wait for non-zero */

				if (semwait( semidp, &semp->semncnt ) < 0 )
					return -1;
			}
			else {			/* wait for zero */

				if ( semwait( semidp, &semp->semzcnt ) < 0 )
					return -1;
			}

			sp = sops;	/* retry set of semaphore operations */
			n = nsops;
			continue;
		}

		++sp;
		--n;
	}

	for (sp=sops,n=nsops; n > 0; --n,++sp) {/* save pid in each semaphore */

		semnum = getuwd( &sp->sem_num );
		semidp->sem_base[ semnum ].sempid = SELF->p_pid;
	}

	semidp->sem_otime = timer.t_time;	/* ajust operation time */
	return oval;				/* return last prev semval */
}

/*
 * Do a Semaphore Operation.
 *
 *	Input:	semp  = pointer to semaphore
 *		semop = semaphore operation
 *
 *	Action:	If semop < 0 and semval > semop then add semop to semval.
 *		If semop > 0 then add semop to semval.
 *
 *	Return:	Previous semval.
 */

static
semdo( semp, semop )

register struct sem * semp;
register int semop;

{
	int ret;

	ret = semp->semval;

	if ( semop < 0 ) {			/* want to decrement semval */

		semop = -semop;

		if ( semp->semval < semop )	/* can't decrement semval */
			return -1;

		semp->semval -= semop;

		if ( (semp->semval == 0)  && (semp->semzcnt != 0) )
			wakeup( &semp->semzcnt );
	}
	else if ( semop > 0 ) {			/* want to increment semval */

		if ( (semp->semval + semop) > NSEMVAL) {
		
			u.u_error = ERANGE;
			return -1;
		}

		semp->semval += semop;

		if ( semp->semncnt )
			wakeup( &semp->semncnt );
	}
	else /* if ( semop == 0 ) */ {

		if ( semp->semval != 0 )
			return -1;
	}
	return ret;
}

/*
 * Undo a Semaphore Operation.
 */

static
semundo( semp, semop )

register struct sem * semp;
register int semop;

{
	if ( semp->semval -= semop ) {

		if ( semp->semncnt )
			wakeup( &semp->semncnt );
	}
	else {
		if ( semp->semzcnt )
			wakeup( &semp->semzcnt );
	}
}

/*
 * Wait on Semaphore Event
 *
 *	Input:	semidp = pointer to semaphore id structure.
 *		ep     = pointer to semncnt or semzcnt event to wait for.
 *
 *	Action:	Sleep on a semaphore event.
 *		If semaphore id has been deleted, error return.
 *		If signal has been received, error return.
 *
 *	Output:	 0 = Status ok.
 *		-1 = Error occurred, errno is set.
 */

static
semwait( semidp, ep )

register struct semid_ds * semidp;
register unsigned short *ep;
{
	unsigned seqn;

	seqn = semidp->sem_perm.seq;
	++(*ep);
	sleep( ep, CVTTOUT, IVTTOUT, SVTTOUT );

	if (semidp->sem_perm.seq != seqn ) {	/* semaphore id gone */
		u.u_error = EIDRM;
		return -1;
	}

	--(*ep);

	if ( SELF->p_ssig && nondsig() ) {	/* signal received */
		u.u_error = EINTR;
		return -1;
	}
	return 0;
}
