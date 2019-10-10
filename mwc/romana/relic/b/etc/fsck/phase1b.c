/*
 *	Phase 1b of fsck - Rescan for more Dups
 */

#include "fsck.h"

phase1b()
{
	if (!qflag)
		printf("Phase 1b: Rescan for more Dups\n");
	buildtable();
	iscanb();
}

buildtable()
{
	unsigned cntr=0;
	daddr_t bn;

	while (cntr<totdups) {
		bn = dupblck[cntr++];
		markdup(bn);
	}
}

iscanb()
{
	register daddr_t bn;
	register struct dinode *dip;
	register ino_t	ino;
	int i;

	ino = 1;

	for (bn=INODEI; bn<isize; bn++) {
		if (testblock(bn)) {		/* block is bad via inode 1 */
			ino += INOPB;
			continue;
		}
		bread(bn, databuf);
		dip = (struct dinode *) databuf;
		for (i=0; i<INOPB; i++) {
			candino(dip);
			if (inuse(dip) == TRUE) 
				ckblksb(dip, ino);
			ino++;
			dip++;
		}
	}
}

/*
 *	Check the blocks associated with the given inode to find the
 *	remaining duplicate blocks
 */

ckblksb(dip, ino)
register struct dinode *dip;
register ino_t	ino;
{
	daddr_t	addrs[NADDR], bn;
	int i, lev, naddr;
	int mode;

	mode = dip->di_mode & IFMT;

	if ( (mode == IFREG) || (mode == IFDIR) )
		l3tol(addrs, dip->di_addr, naddr=NADDR);
	else if ( mode == IFPIPE )
		l3tol(addrs, dip->di_addr, naddr=ND);
	else
		return;

	for(i=0; i<naddr; i++)
		for (lev=0; lev<4; lev++) 
			if (i < offsets[lev]) {
				if ( (bn=addrs[i]) != 0 ) {
					dblocksb(bn, ino, lev);
					break;
				}
			}
}

/*
 *	Checks recursively the blocks pointed at via
 *	the inode list of blocks.  'bn' is the block number,
 *	'ino' is the inode referencing it, and 'lev' is the
 *	level 0 == direct ... 3 = triple-indirect
 */

dblocksb(bn, ino, lev)
daddr_t	bn;
ino_t	ino;
int	lev;
{
	char buf[BSIZE];
	int  i;
	daddr_t	*bnptr;

	if ( cdupb(bn, ino) == OK ) {
		if (lev--==0)
			return(OK);
		bread(bn, buf);
		bnptr = (long *) buf;
		for (i=0; i<NBN; i++) {
			bn = bnptr[i];
			candaddr(bn);
			if ( bn != 0 )
				dblocksb(bn, ino, lev);
		}
	}
}

/*
 *	Check the given block number for duplicate reference.
 */

cdupb(bn, ino)
daddr_t	bn;
ino_t	ino;
{
	if ( bn >= fsize )
		return(STOP);
	if ( !testdup(bn) ) {
		if ( ino == 1 )		/* bad block inode, let's mark it */
			markdup(bn);	/* so we never read it again	  */
		return(OK);
	}

	if (!fflag)
		orflags(ino, IBAD_IDUP);
	printf("Dup Block %lu, i-number = %u\n", bn, ino);

	return(STOP);
}
