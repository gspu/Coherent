/*
 *	phase 4 of fsck - Check Reference Counts
 */

#include "fsck.h"

extern char *reconnect;
fsize_t	lostsize;

phase4()
{
	ino_t i;
	short actlink;

	if (!qflag)
		printf("Phase 4 : Check Reference Counts\n");
	for (i=FIRSTIN; i<=ninodes; i++) {
		if ( badblks(i) )
			if ( bddp(i) == YES )
				continue;	/* inode zeroed */

		if ( (linkctr(i) == 0) &&
		     ((flags(i)&MODEMASK) != IDIR) &&
		     ((flags(i)&ALLOCMASK) != UNALLOC) )
			if ( unreflink(i) == YES )
				continue;	/* inode zeroed */

		if ( (linkctr(i) == 0) &&
		     ((flags(i)&ALLOCMASK) != UNALLOC)  )
			if ( unrefclr(i) == YES )
				continue;	/* inode zeroed */

		if ( (actlink=getlink(i)) == (-1) ) {
			i += INOPB-1;
			continue;		/* bad inode block */
		}

		if ( actlink != linkctr(i) )
			linkprob(i, actlink);	/* fix link count */
	}

	freeilist();
}

short
getlink(inum)
register ino_t inum;
{
	register unsigned offset;
	static ibuf[BSIZE];
	register struct dinode *dip;
	register daddr_t bn;

	offset = iblocko(inum);
	if ( (offset==0) || (inum==FIRSTIN) ) {
		bn = iblockn(inum);
		if ( (testblock(bn)) && (inum!=FIRSTIN) ) 
			return(-1);
		bread(bn, ibuf);
	}
	dip = ((struct dinode *) ibuf) + offset;
	candino(dip);
	return(dip->di_nlink);
}

char *clearit = "(Clear i-node)";

unreflink(ino)
ino_t ino;
{
	fsize_t size;

	if ( (qflag == TRUE) && ((flags(ino)&MODEMASK) == IPIPE) ) {
		doclear(ino);
		return(YES);
	}

	printf("Unref %s\n", typename(ino));
	size = pinfo(ino);
	if ( (daction != NO) && (size == 0) && (mounted == FALSE) 
		&& ((flags(ino)&MODEMASK) == IREG) ) {
		printf("%s [Forced - Yes]\n", clearit);
		doclear(ino);
		return(YES);
	}
	if ( action(reconnect) == TRUE )
		if( connect(ino, IREG) == FAILURE )  /* connect back in the */
			return(askclear(ino));	     /* non dir inode	    */
			
	return(NO);
}

askclear(ino)
ino_t ino;
{
	if ( action(clearit) == TRUE ) {
		doclear(ino);
		return(YES);
	}
	return(NO);
}

doclear(ino)
ino_t ino;
{
	struct dinode *dip;

	dip = ptrino(ino, databuf);
	zeroinode(dip);
	writeino(ino, databuf);
	setflags(ino, UNALLOC);
	setlinkctr(ino, 0);
}

char *adjust = "(Adjust)";

linkprob(ino, actlink)
ino_t ino;
short actlink;
{
	printf("Link Count discrepancy in %s\n", typename(ino));
	pinfo(ino);
	printf("Count = %u, should be %u ", actlink, linkctr(ino));
	if ( action(adjust) == TRUE )
		chglink(ino);
}

chglink(ino)
ino_t ino;
{
	struct dinode *dip;

	dip = ptrino(ino, databuf);
	dip->di_nlink = linkctr(ino);
	writeino(ino, databuf);
}
	
unrefclr(ino)
ino_t ino;
{
	printf("Unref %s\n", typename(ino));
	pinfo(ino);
	return(askclear(ino));
}

bddp(ino)
ino_t ino;
{
	printf("Bad/Dup blocks in %s\n", typename(ino));
	pinfo(ino);
	return(askclear(ino));
}

char *fixit = "(FIX)";

freeilist()
{
	ino_t i, count=0;
	daddr_t bn;

	for (i=FIRSTIN; i<=ninodes; i++)
		if ( (flags(i)&ALLOCMASK) == UNALLOC )
			count++;

	for (bn=INODEI+1; bn<isize; bn++)	/* block # INODEI can't */
		if ( testblock(bn) )		/* be bad since ROOTIN  */
			count -= INOPB;		/* is in it, otherwise  */
						/* big time bad news.	*/
	if ( sbp->s_tinode != count ) 
		if ( (qflag==FALSE) && (daction!=NO) ) {
			printf("Free i-node count wrong in superblock.  ");
			if ( action(fixit) == TRUE ) {
				sbp->s_tinode = count;
				sbpfix = TRUE;
			}
		} else {
			sbp->s_tinode = count;
			sbpfix = TRUE;
		}

	if ( (daction!=NO) &&
	     ( (sflag==TRUE) || (sbpfix==TRUE) || (badilist()==BAD) ) )
	 	rebuild();

}

badilist()
{
	ino_t i, inum;
	unsigned numfree;
	daddr_t bn;

	if ( (numfree=(unsigned) sbp->s_ninode) > NICINOD )
		return(BAD);
	for (i=0; i<numfree; i++) {
		inum = sbp->s_inode[i];
		if ( (inum < FIRSTIN) || (inum > ninodes) )
			return(BAD);
		bn = iblockn(inum);
		if ( (testblock(bn)) && (bn!=INODEI) )
			return(BAD);
		if ( (flags(inum)&ALLOCMASK) != UNALLOC ) 
			return(BAD);
		if ( flags(inum)&IFREELIST )
			return(BAD);		/* dup in ifree list */
		orflags(inum, IFREELIST);
	}
	return(GOOD);
}

rebuild()
{
	daddr_t bn;
	ino_t i;
	int index;

	sbpfix = TRUE;
	for (i=FIRSTIN, index=0; (i<=ninodes) && (index<NICINOD); i++) {
		if ( (flags(i)&ALLOCMASK) == UNALLOC ) {
			bn = iblockn(i);
			if ( (!testblock(bn)) || (bn==INODEI) )
				index++;
		}
	}
	sbp->s_ninode = index;
	for (i=FIRSTIN; (i<=ninodes) && (index>0); i++) {
		if ( (flags(i)&ALLOCMASK) == UNALLOC ) {
			bn = iblockn(i);
			if ( (!testblock(bn)) || (bn==INODEI) )
				sbp->s_inode[--index] = i;
		}
	}
	printf("Free i-node list in superblock rebuilt.\n");
}
