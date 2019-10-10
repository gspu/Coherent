/*
 * Print out received ipc data corresponding to print optoins.
 */
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "ipcs.h"

struct group *grp;
struct passwd *pstp;

struct msqid_ds	*msqbuf;	/* message queue data */
/* 
 * Print information about active message queues
 */
print_q() 
{
	int 	x;		/* counter */
	char 	date[30];	/* date string */

	printf("MESSAGE QUEUES:\n");

	/* Check if msq is in use */
	for (x = 0 ; x < NMSQID; x++)
		if (msqbuf[x].msg_perm.mode & IPC_ALLOC)
			break;
	if (x >= NMSQID)
		return; 

	printf("T  ID         KEY  MODE  OWNER  GROUP");

	if (cflag) 
		printf(" CREATOR CGROUP");

	if (oflag) 
		printf(" CBYTES QNUM");

	if (bflag)
		fputs(" QBYTES", stdout);

	if (pflag)
		fputs(" LSPID LRPID", stdout);

	if (tflag)
		fputs("   STIME    RTIME    CTIME ", stdout);

	puts("");

	for (x = 0 ; x < NMSQID; x++) {
		if (!(msqbuf[x].msg_perm.mode & IPC_ALLOC))
			continue;
		/* id, mode & key */
		printf("q%4d%12d", msqbuf[x].msg_perm.seq,
			msqbuf[x].msg_perm.key);
	
		if (msqbuf[x].msg_perm.mode & MSG_RWAIT) {
			printf(" R"); /* pid waiting for msgrcv() */
		} else {
			if (msqbuf[x].msg_perm.mode & MSG_WWAIT) {
				printf(" S"); /* waiting for msgsnd() */
			} else {
				printf(" -"); /* no flag set */
			}	
		}
		printf("0%o", msqbuf[x].msg_perm.mode&0777);			
		/* get owner's name from /etc/passwd */
		if ((pstp = getpwuid(msqbuf[x].msg_perm.uid)) == NULL) {
			perror("ipcs: cannot get owner name:");
			exit(1);
		}
		printf("%7s", pstp->pw_name);
	
		/* get group name of owner */
		if ((grp = getgrgid(msqbuf[x].msg_perm.gid)) == NULL) {
			perror("ipcs: cannot get group name");
			exit(1);
		}
		printf("%7s", grp->gr_name);

		if (cflag) {
		/* get creator's name from /etc/passwd */
			if ((pstp = getpwuid(msqbuf[x].msg_perm.cuid)) == NULL){
				perror("ipcs: cannot get creator name");
				exit(1);
			}
			printf("%8s", pstp->pw_name);

			/* get group name of creator */
			if ((grp = getgrgid(msqbuf[x].msg_perm.cgid)) == NULL){
				perror("ipcs: cannot get creator group");
			exit(1);
			}
			printf("%7s",grp->gr_name);
		}

		/* current bytes & # of messages */
		if (oflag) 
			printf("%7d%5d", msqbuf[x].msg_cbytes,
						msqbuf[x].msg_qnum);

		/* max # bytes on queue */
		if (bflag)
			printf("%7d",msqbuf[x].msg_qbytes);

		/* last send & receive processes */
		if (pflag) 
			printf("%6d%6d", msqbuf[x].msg_lspid,
							msqbuf[x].msg_lrpid);

		/* times of last send and receive and modification*/
		if (tflag) {
			sprintf(date,"%s", ctime(&msqbuf[x].msg_stime));
			printf(" %.8s",date + 11);
			sprintf(date,"%8s", ctime(&msqbuf[x].msg_rtime));
			printf(" %.8s",date + 11);
			sprintf(date,"%s", ctime(&msqbuf[x].msg_ctime));
			printf(" %.8s",date + 11);
		}
		printf("\n");
	}
	printf("\n");
}

/* 
 * Print information about active shared memory segments
 */
print_m()
{
	int x;		/* counter for print loop */
	char date[30];	/* used to hold the date string */

	printf("SHARED MEMORY:\n");

	if (!total_shmids) 
		return;
	printf("T  ID         KEY  MODE  OWNER  GROUP");
	if (cflag) 
		printf(" CREATOR CGROUP");
	
	if (oflag) 
		printf(" NATTCH");

	if (bflag) 
		printf(" SEGSZ");
	if (pflag) 
		printf("  CPID LPID");

	if (tflag)
		printf("    ATIME    DTIME    CTIME");

	printf("\n");


	for (x = SHMMNI - 1; x >= 0 ; x--) {
		if (!valid_shmid[x])
			continue;
		/* id, mode & key */
		printf("m%4d%12d  0%o", x, shmid[x].shm_perm.key,
					shmid[x].shm_perm.mode & 0777);
		/* get owner's name from /etc/passwd */
		if ((pstp = getpwuid(shmid[x].shm_perm.uid)) == NULL) {
			perror("ipcs: cannot get owner name");
			exit(1);
		}
		printf("%7s",pstp->pw_name);
	
		/* get group name of owner */

		if ((grp = getgrgid(shmid[x].shm_perm.gid)) == NULL) {
			perror("ipcs: cannot get owner group");
			exit(1);
		}
		printf("%7s",grp->gr_name);

		if (cflag) {
			/* get creator's name from /etc/passwd */
			if ((pstp = getpwuid(shmid[x].shm_perm.cuid)) == NULL) {
				perror("ipcs: cannot get creator name");
				exit(1);
			}
			printf("%8s",pstp->pw_name);
	
			/* get group name of creator */
			if ((grp = getgrgid(shmid[x].shm_perm.cgid)) == NULL) {
				perror("ipcs: cannot get creator group");
				exit(1);
			}
			printf("%7s",grp->gr_name);
		}

		if (oflag) {	/* attached segments */
			printf("%7d", shmid[x].shm_nattch);
		}
		
		if (bflag) {	/* segment size */
			printf("%6d",shmid[x].shm_segsz);
		}

		if (pflag)	/* processs id of last op */
			printf("%6d%5d", shmid[x].shm_cpid, 
					shmid[x].shm_lpid);
		/* Time */	
		if (tflag) {  	
			/* attach time */
			if (!shmid[x].shm_atime)
				printf(" no-entry ");
			else {
				sprintf(date,"%s", ctime(&shmid[x].shm_atime));
				printf(" %.8s", date + 11);
			}
			/* detach time */
			if (!shmid[x].shm_dtime)
				printf(" no-entry");
			else {
				sprintf(date,"%s", ctime(&shmid[x].shm_dtime));
				printf(" %.8s", date + 11);
			}
			/* creat time */
			sprintf(date,"%s", ctime(&shmid[x].shm_ctime));
			printf(" %.8s", date + 11);
		}
		printf("\n");
	}	
	printf("\n");
}

/* 
 * Print information about active semaphores
 */
print_s()
{
	int x;		/* loop counter */
	char date[30];	/* holds our date string */

	printf("SEMAPHORES:\n");

	if (!total_sems)
		return;

	printf("T  ID         KEY MODE  OWNER  GROUP");
	if (cflag) {
		printf(" CREATOR CGROUP");
	}

	if (bflag) {
		printf(" NSEMS");
	}

	if (tflag) {
		printf("    OTIME    CTIME");
	}
	printf("\n");

	for (x = 0; x < SEMMNI; x++) {
		if (!valid_semid[x])
			continue;

		printf("s%4d%12d 0%o",x,  /* mode and key */
			semid[x].sem_perm.key,
			semid[x].sem_perm.mode & 0777);

		/* get owner's name from /etc/passwd */
		if((pstp = getpwuid(semid[x].sem_perm.uid)) == NULL){
			printf("Error reading password file!\n");
			exit(1);
		}
		printf("%7s",pstp->pw_name);
	
		/* get group name of owner */
	
		if((grp = getgrgid(semid[x].sem_perm.gid)) == NULL){
			printf("Error reading group file!\n");
			exit(1);
		}
		printf("%7s",grp->gr_name);

		if (cflag) {
		/* get creator's name from /etc/passwd */
			if((pstp = getpwuid(semid[x].sem_perm.cuid)) == NULL){
				printf("Error reading password file!\n");
				exit(1);
			}
			printf("%8s",pstp->pw_name);

			/* get group name of creator */
			if((grp = getgrgid(semid[x].sem_perm.cgid)) == NULL){
				printf("Error reading group file!\n");
				exit(1);
			}
			printf("%7s",grp->gr_name);
		}

		if (bflag) {	/* number of semaphore elements */	
				printf("%6d",semid[x].sem_nsems);
		}

		if (tflag) {	/* time */
			if (semid[x].sem_otime) {/* semop time */
				sprintf(date,"%s", ctime(&semid[x].sem_otime));
				printf(" %.8s",date + 11);
			} else
				printf(" no-entry");
			if (semid[x].sem_ctime) { /* Change time */
				sprintf(date,"%s", ctime(&semid[x].sem_ctime));
				printf(" %.8s",date + 11);
			} else
				printf(" no-entry");
		}
		printf("\n");
	}
}
