/* (-lgl
 * 	COHERENT Version 4.1.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * ipcs.h
 */
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>


#define KERNEL			/* Necessary for the fifo stuff to read */
#include <sys/typed.h>		/* /dev/boot_gift.			*/

#define DEFAULT_KERNEL	"/coherent"	/* default kernel image to read */
#define OBSCENE_VALUE	100

#define VERSION	"0.02"

/*
 * shared memory stuff 
 */
struct shmid_ds	*shmid;			/* array of shmid_ds structs */
int 		*valid_shmid;		/* keep track of valid shared memory
					   segments found */
/* 
 * semaphore stuff 
 */
struct semid_ds semid[OBSCENE_VALUE];	/* array of semaphore structs */
int valid_semid[OBSCENE_VALUE];		/* track validly located semaphores */

/*
 * message queue stuff
 */
extern struct msqid_ds	*msqbuf;	/* array of message queue structs */

/* Functions */
extern int	get_data();	 /* Get ipc data */
extern int	get_msg_stats(); /* get message queue stats */
extern int	get_shm_stats(); /* get shared memory stats */
extern int	get_sem_stats(); /* get semaphore stats */
extern int	print_q();	 /* print message queue stats */
extern int	print_m();	 /* print shared memory stats */
extern int	print_s();	 /* printf semaphore stats */
extern char *	pick_nfile();	 /* get name of boot kernel from /tboot */
extern FIFO *	fifo_open();	 /* open /dev/boot_gift */
extern int	fifo_close();	 /* close boot_gift */
extern typed_space *	fifo_read();	/* read boot_gift */
extern int	iMemSeek();	/* Seek in the virtual memory */
/* Option's flags. See man pages for more info */
extern short	qflag,	/* message q */
		mflag,	/* shared memory */
		sflag,	/* semaphores */
		bflag,	/* biggest */
		cflag,	/* creator name */
		oflag,	/* outstanding usage */
		pflag,	/* process ID */
		tflag,	/* time */
		aflag,	/* include b, c, o, p, & t */
		Cflag,	/* corefile */
		Nflag;	/* namelist */

extern int	total_shmids, 	/* total valid shared memory segs found */
		total_sems,	/* total valid semaphores found */
		usemsqs;	/* is msqs in use */

extern int	SHMMNI,		/* max # of shared memory segments allowed */
		SEMMNI,		/* max # of semaphores allowed */
		NMSQID;		/* max # of message queues */

extern char	*fname;		/* name of the namelist file */
extern struct msqid_ds	*msqbuf;/* pointer to the message q data */
