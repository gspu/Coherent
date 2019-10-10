/*
 *	Cleanup functions for phases of fsck
 */

#include "fsck.h"

cleanup()
{
	int retval = 0;

	if (sbpfix == TRUE) 
		writesuper();
	prtstats();
	if (changeflg == TRUE)
		retval = advise();
	close(fsfd);
#if SMALLMODEL
	cleanV();
#endif
	return(retval);
}

writesuper()
{
	register int i;

	canshort(sbp->s_isize);
	candaddr(sbp->s_fsize);
	canshort(sbp->s_nfree);
	for(i=0; i<NICFREE; i++)
		candaddr(sbp->s_free[i]);
	canshort(sbp->s_ninode);
	for(i=0; i<NICINOD; i++)
		canino(sbp->s_inode[i]);
	cantime(sbp->s_time);
	candaddr(sbp->s_tfree);
	canino(sbp->s_tinode);
	canshort(sbp->s_m);
	canshort(sbp->s_n);
	canlong(sbp->s_unique);

	bwrite((daddr_t)SUPERI, sbp);
}

prtstats()
{
	if (!fflag)
		printf("%u files ", numfiles);
	printf("%U blocks ", fsize - isize - totfree);
	printf("%U free\n", totfree);
	if ( lostsize ) {
		lostsize = (lostsize + BSIZE - 1)/BSIZE;
		printf("Expect roughly %U missing block%s next\n", lostsize,
				(lostsize!=1) ? "s" : "" );
		printf("time fsck is run as a result of inodes ");
		printf("being cleared.\n");
	}
}

advise()
{
	int retval = 0;

	printf(" ***** File System %s was modified *****\n", fsname);
	if ( mounted ) {
		printf(" ***** BOOT Coherent (NO SYNC!) *****\n");
		retval = 1;
	}
	return(retval);
}
