/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/sem.h
 *
 * IPC Semaphore Facility
 *
 * Revised: Mon Apr 19 13:52:49 1993 CDT
 */
#ifndef __SYS_SEM_H__
#define __SYS_SEM_H__

#include <sys/ipc.h>
#include <common/_time.h>

/*
 * Permission Definitions.
 */
#define	SEM_A	0200		/* alter permission */
#define	SEM_R	0400		/* read permission */

/*
 * Semaphore Operation Flags.
 */
#define SEM_UNDO 010000		/* set up adjust on exit entry */

/*
 * Semctl Command Definitions.
 */
#define	GETNCNT	3		/* get semncnt */
#define	GETPID	4		/* get sempid  */
#define	GETVAL	5		/* get semval  */
#define	GETALL	6		/* get all semval's */
#define	GETZCNT 7		/* get semzcnt */
#define	SETVAL	8		/* set semval  */
#define	SETALL	9		/* set all semval's */

/*
 * Structure Definitions.
 */

/*
 * There is one semaphore structure for each semaphore in the system.
 */
struct sem {
	unsigned short	semval;		/* semaphore value */
	short		sempid;		/* pid of last operation */
	unsigned short	semncnt;	/* # awaiting semval > cval */
	unsigned short	semzcnt;	/* # awaiting semval = 0 */
};

/*
 * There is one semaphore id data structure for each set of semaphores
 * in the system.
 */
struct semid_ds {
	struct ipc_perm	sem_perm;	/* semaphore permission struct */
	struct sem	*sem_base;	/* pointer to first semaphore in set */
	unsigned short	sem_nsems;	/* # of semaphores in set */
	time_t		sem_otime;	/* last semop time */
	time_t		sem_ctime;	/* last change time */
};

/*
 * There is one link list of undo structures per process.
 */
struct sem_undo {
	struct sem_undo *un_np;		/* ptr to next active undo structure */
	short		un_aoe;		/* adjust on exit values */
	short		un_num;		/* semaphore # */
	int		un_id;		/* semid */
};

/*
 * User semaphore template for semop system calls.
 */
struct sembuf {
	unsigned short	sem_num;	/* semaphore # */
	short		sem_op;		/* semaphore operation */
	short		sem_flg;	/* operation flags */
};

/*
 * Ioctl commands issued to Semaphore device driver (Coherent 286 specific).
 */
#define	SEMIOC	('S'<<8)
#define	SEMCTL	(SEMIOC|0)
#define	SEMGET	(SEMIOC|1)
#define	SEMOP	(SEMIOC|2)

#endif /* __SYS_SEM_H__ */
