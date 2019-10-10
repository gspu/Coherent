/* 
 * ipcrm: remove message queue, shared memory segment or semaphore set
 * 	  given an identifier or key used to create the queue, segment
 *	  or set.
 */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>

/* 
 * Functions 
 */
void 	rmid();			/* given ipc resource identifier, remove it */
void	rmkey();		/* given ipc resource key, remove it */
void 	usage();		/* usage message */

/* 
 * Globals 
 */
int	errorCount = 0;

/*
 * Program will go though all arguments and perform the job.
 * ipcrm will print message on error and continue process argument list,
 * so, if one or a few of options are not correct and a few are correct
 * it tries to properly process all correct options and give error messages
 * about all uncorrect.
 */
main(argc, argv)
int argc;
char **argv;
{
	extern char	*optarg;
	extern int	optind;
	int		flag;
	int		identifier;	/* ipc resource identifier */
	key_t		key;		/* ipc resource key */

	while ((flag = getopt(argc, argv, "m:M:s:S:q:Q:")) != EOF) {
		switch (flag) {
		case 'm':	/* shmid */
			identifier = atoi(optarg);
			rmid('m', identifier);
			break;
		case 'M':	/* shmkey */
			key = atoi(optarg);
			rmkey('M', key);
			break;
		case 'q':	/* msgqid */
			identifier = atoi(optarg);
			rmid('q', identifier);
			break;
		case 'Q':	/* msgqkey */
			key = atoi(optarg);
			rmkey('Q', key);
			break;
		case 's':	/* semid */
			identifier = atoi(optarg);
			rmid('s', identifier);
			break;
		case 'S':	/* semkey */
			key = atoi(optarg);
			rmkey('S', key);
			break;
		default:
			errorCount++;
			break;
		}
	}
	/* Print usage message on any error(s) but only once. */
	if (errorCount)
		usage();
	exit(errorCount);
}

/* rmid(): take a flag which tells us to remove a message queue, shared memory
 *	   segment or semaphore set and an identifier and perform the proper
 *	   command (msgctl, shmctl or semctl) with IPC_RMID to destroy the
 *	   allocated queue, segment or set.
 *         Print error message and increment error count on error.
 */
void rmid(flag,identifier)
char flag;
int identifier;
{
	switch (flag) {
	case 'm':
		if (shmctl(identifier, IPC_RMID, 0) < 0) {
			printf("ipcrm: shmid(%d): %s\n", identifier, 
							sys_errlist[errno]);
			errorCount++;
		}
		break;
	case 'q':
		if (msgctl(identifier, IPC_RMID, 0)) {
			printf("ipcrm: msgid(%d): %s\n", identifier, 
							sys_errlist[errno]);
			errorCount++;
		}
		break;
	case 's':
		if (semctl(identifier, IPC_RMID, 0, 0)) {
			printf("ipcrm: semid(%d): %s\n", identifier, 
							sys_errlist[errno]);
			errorCount++;
		}
		break;
	}
}

/* 
 * rmkey():	Remove ipc resource by gevien key.
 *		In case of error print error message and increment
 *		errorCount.
 */
void rmkey(flag, key)
char	flag;
key_t 	key;
{
	int	id;	/* ipc id */
	
	switch (flag) {
	case 'M':
		if ((id = shmget(key, 1, 0)) < 0 ) {
			printf("ipcrm: shmkey(%d): %s\n", key, 
							sys_errlist[errno]);
			errorCount++;
			return;
		}
		if (shmctl(id, IPC_RMID, 0) < 0) {
			printf("ipcrm: shmkey(%d): %s\n", key, 
							sys_errlist[errno]);
			errorCount++;
			return;
		}
		return;
	case 'Q':
		if ((id = msgget(key, 0)) < 0 ) {
			printf("ipcrm: msgkey(%d): %s\n", key, 
							sys_errlist[errno]);
			errorCount++;
			return;
		}
		if (msgctl(id, IPC_RMID, 0) < 0) {
			printf("ipcrm: msgkey(%d): %s\n", key, 
							sys_errlist[errno]);
			errorCount++;
			return;
		}
		return;
	case 'S':
		if ((id = semget(key, 0, 0)) < 0 ) {
			printf("ipcrm: semkey(%d): %s\n", key, 
							sys_errlist[errno]);
			errorCount++;
			return;
		}
		if (semctl(id, IPC_RMID, 0, 0) < 0) {
			printf("ipcrm: semkey(%d): %s\n", key, 
							sys_errlist[errno]);
			errorCount++;
			return;
		}
		return;
	}
}

/* usage message */
void usage()
{
	errorCount++;
	puts("usage: ipcrm [ [-m shmId] [-q msgqId] [-s semId]");
	puts("               [-M shmKey] [-Q msgqKey] [-S semKey] ... ]");
}
