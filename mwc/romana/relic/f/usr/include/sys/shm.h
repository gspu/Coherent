/*
 * /usr/include/sys/shm.h
 *
 * Shared memory support.
 *
 * Revised: Thu Jul 15 14:05:25 1993 CDT
 */
#ifndef __SYS_SHM_H__
#define	__SYS_SHM_H__
/*
**	IPC Shared Memory Facility.
*/
#include <sys/ipc.h>
#include <common/_time.h>

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
#define SHM_RND		020000	/* round attach address to SHMLBA */

/* 
 * Shared memory control operations (are not included in iBCS2)
 * COHERENT 4.0.x does not use a swapper.
 */
#define	SHM_LOCK	3	/* lock shared memory segment in core */
#define SHM_UNLOCK	4	/* unlock shared memory segment */

#if _I386

/*
**	There is a shared mem id data structure for each segment in the system.
*/
struct shmid_ds {
	struct ipc_perm	shm_perm;	/* operation permission struct */
	int		shm_segsz;	/* segment size */
	unsigned short	shm_cpid;	/* pid of creator */
	unsigned short	shm_lpid;	/* pid of last shmop */
	unsigned short	shm_nattch;	/* current # attached */
	time_t		shm_atime;	/* last shmat time */
	time_t		shm_dtime;	/* last shmdt time */
	time_t		shm_ctime;	/* last change time */
};

#ifdef __KERNEL__

/* Patchable kernel values. */

extern int SHMMAX;	/* Max size in bytes of shared memory segment */
			/* Not more than 4 Megs, please! */

extern int SHMMNI;	/* Maximum # of shared memory segments, systemwide */

#endif

#else /* !_I386 */

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


#endif /* !_I386 */

#endif /* !defined(__SYS_SHM_H__) */
