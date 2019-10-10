/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef SEM_H
#define SEM_H

/*
**	IPC Semaphore Facility
*/

#include <sys/ipc.h>
#include <sys/types.h>

/*
**	Permission Definitions.
*/

#define	SEM_A	0200		/* alter permission */
#define	SEM_R	0400		/* read permission */

/*
**	Semaphore Operation Flags.
*/

/* #define SEM_UNDO 010000 */	/* set up adjust on exit entry */

/*
**	Semctl Command Definitions.
*/

#define	GETNCNT	3		/* get semncnt */
#define	GETPID	4		/* get sempid  */
#define	GETVAL	5		/* get semval  */
#define	GETALL	6		/* get all semval's */
#define	GETZCNT 7		/* get semzcnt */
#define	SETVAL	8		/* set semval */
#define	SETALL	9		/* set all semval's */

/*
**	Structure Definitions.
*/

/*
**	There is one semaphore structure for each semaphore in the system.
*/

struct sem {

	unsigned short	semval;		/* semaphore text map address */
	short		sempid;		/* pid of last operation */
	unsigned short	semncnt;	/* # awaiting semval > cval */
	unsigned short	semzcnt;	/* # awaiting semval = 0 */
};

/*
**	There is one semaphore id data structure for each set of semaphores
**		in the system.
*/

struct semid_ds {

	struct ipc_perm	sem_perm;	/* operation permission struct */
	struct sem	*sem_base;	/* pointer to first semaphore in set */
	unsigned short	sem_nsems;	/* # of semaphores in set */
	time_t		sem_otime;	/* last semop time */
	time_t		sem_ctime;	/* last change time */
};

/*
**	There is one undo structure per process in the system.
*/

struct sem_undo {

	struct sem_undo *un_np;		/* ptr to next active undo structure */
	short		un_cnt;		/* # of active entries */
	struct undo {
		short	un_aoe;		/* adjust on exit values */
		short	un_num;		/* semaphore # */
		int	un_id;		/* semid */
	} un_ent[1];			/* undo entries (one minimum) */
};

/*
**	User semaphore template for semop system calls.
*/

struct sembuf {

	unsigned short	sem_num;	/* semaphore # */
	short		sem_op;		/* semaphore operation */
	short		sem_flg;	/* operation flags */
};

/*
** Ioctl commands issued to Semaphore device driver (Coherent specific).
*/

#define	SEMIOC	('S'<<8)
#define	SEMCTL	(SEMIOC|0)
#define	SEMGET	(SEMIOC|1)
#define	SEMOP	(SEMIOC|2)

#endif
