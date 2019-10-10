/*
 * (User) Shared Memory Control Functions
 *
 *	Note: shmget() must be first function called.
 *
 * $Log:	/newbits/lib/libc/sys/RCS/shmop.c,v $
 * Revision 1.1	91/04/22  13:23:59 	bin
 * Initial revision
 * 
 * 87/04/26	Allan Cornish
 * Support for 32 bit 'key' added to shmget.
 */

#include <sys/shm.h>

int  shmfd   = -1;
static char shmdev[] = "/dev/shm";

/*
 * Shared Memory Control Operations.
 */

shmctl( shmid, cmd, buf )

int shmid;
int cmd;
int buf;

{
	int parm[4];

	if ( shmfd < 0 )
		return -1;

	parm[0] = -1;
	parm[1] = shmid;
	parm[2] = cmd;
	parm[3] = buf;

	ioctl( shmfd, SHMCTL, parm );
	return parm[0];
}

/*
 * Get Shared Memory Segment.
 */

shmget( key, size, shmflg )

key_t key;
int size;
int shmflg;

{
	int parm[5];

	if ( shmfd < 0 ) {

		if ( (shmfd = open(shmdev, 2)) < 0 ) {

			perror( shmdev );
			return -1;
		}
	}

	parm[0] = -1;
	parm[1] = key;
	parm[2] = key >> 16;
	parm[3] = size;
	parm[4] = shmflg;

	ioctl( shmfd, SHMGET, parm );
	return parm[0];
}
