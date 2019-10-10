/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef SHM_H
#define	SHM_H
/*
**	IPC Shared Memory Facility.
*/

#include <sys/ipc.h>
#include <sys/types.h>

extern int shmfd;		/* file descriptor to access shared memory */

/*
**	Permission Definitions.
*/
#define	SHM_R	0400		/* read permission */
#define	SHM_W	0200		/* write permission */

/*
**	ipc_perm Mode Definitions.
*/
#define	SHM_CLEAR	01000	/* clear segment on next attach */
#define	SHM_DEST	02000	/* destroy segment when # attached = 0 */

/*
**	Message Operations Flags.
*/
#define	SHM_RDONLY	010000	/* attach read-only (else read-write) */

/*
**	There is a shared mem id data structure for each segment in the system.
*/
struct shmid_ds {
	struct ipc_perm	shm_perm;	/* operation permission struct */
	int		shm_segsz;	/* segment size */
	unsigned short	shm_lpid;	/* pid of last shmop */
	unsigned short	shm_cpid;	/* pid of creator */
	unsigned short	shm_nattch;	/* current # attached */
	unsigned short	shm_cnattch;	/* in memory # attached */
	time_t		shm_atime;	/* last shmat time */
	time_t		shm_dtime;	/* last shmdt time */
	time_t		shm_ctime;	/* last change time */
};

/*
**	Ioctl Commands issued to Shared Memory Device Driver (on Coherent).
*/
#define	SHMIOC	('H'<<8)
#define	SHMCTL	(SHMIOC+'C')
#define	SHMGET	(SHMIOC+'G')
#define	SHMAT	(SHMIOC+'A')
#define	SHMDT	(SHMIOC+'D')

#endif
