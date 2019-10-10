/*
 * User Semaphore Functions.
 *
 *	Note: Semget() must be first function called.
 *
 * $Log:$
 * 87/04/26	Allan Cornish
 * Support for 32 bit 'key' added to semget.
 */
 
#include <sys/sem.h>

static int  semfno   = -1;
static char semdev[] = "/dev/sem";

/*
 * Semaphore Control Operations.
 */

semctl( semid, semnum, cmd, arg )

int semid;
int semnum;
int cmd;
int arg;

{
	int parm[5];

	if ( semfno < 0 )
		return -1;

	parm[0] = -1;
	parm[1] = semid;
	parm[2] = semnum;
	parm[3] = cmd;
	parm[4] = arg;

	ioctl( semfno, SEMCTL, parm );
	return parm[0];
}

/*
 * Get Set of Semaphores.
 */

semget( key, nsems, semflg )

key_t key;
int nsems;
int semflg;

{
	int parm[5];

	if ( semfno < 0 ) {

		if ( (semfno = open(semdev, 0)) < 0 ) {

			perror( semdev );
			return -1;
		}
	}

	parm[0] = -1;
	parm[1] = key;
	parm[2] = key >> 16;
	parm[3] = nsems;
	parm[4] = semflg;

	ioctl( semfno, SEMGET, parm );
	return parm[0];
}

/*
 * Semaphore Operations.
 */
 
semop( semid, sops, nsops )

int semid;
struct sembuf (*sops)[];
int nsops;

{
	int parm[4];

	if ( semfno < 0 )
		return -1;

	parm[0] = -1;
	parm[1] = semid;
	parm[2] = sops;
	parm[3] = nsops;

	ioctl( semfno, SEMOP, parm );
	return parm[0];
}
