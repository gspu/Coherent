/*
 *	phase 6 of fsck - Salvage Free List
 */

#include "fsck.h"

static	unsigned char *maptab;	/* table of interleave offsets	*/
static	daddr_t	mapbot;		/* minimum block number to interleave map */
static	daddr_t	maptop;		/* maximum block number to interleave map */
static	short	m,n;		/* interleave factors	*/
#define	DEF_N	1		/* default values 	*/
#define	DEF_M	1		
daddr_t	freeblks;		/* number free blocks left to write out */
daddr_t	blknm;			/* current block number we are working with */
struct	fblk *fblkp;		/* pointer to free block list */
daddr_t	*dp;			/* pointer to block entry in free list block */

phase6()
{
	if (!fixerup) {
		return;
	}

	if (!qflag)
		printf("Phase 6 : Salvage Free List\n");
	if ( maketable() == BAD )
		return;
	writefree();
	free(maptab);
}

maketable()
{
	register int i;
	register int ratio;

	m = sbp->s_m;
	n = sbp->s_n;

	if ( (n > MAXINTN) || (m > n) || (n%m != 0) ) {
		printf("Invalid interleave factors in superblock.\n");
		printf("Default free-block list spacing assumed.\n");
		sbp->s_n = n = DEF_N;
		sbp->s_m = m = DEF_M;
	}

	if ( (maptab = malloc(n)) == NULL ) {
		printf("Can't malloc space for interleave table.\n");
		printf("Free-block list is not rebuilt.\n");
		return(BAD);
	}

	mapbot = (isize + n - 1)/n*n;
	maptop = fsize/n*n;
	ratio = n/m;
	for (i=0; i<n; i++)
		maptab[i] = (i/ratio) + (i%ratio)*m;

	return(GOOD);
}

writefree()
{
	register daddr_t bn;
	daddr_t blk, bmap();
	short num;

	sbpfix = TRUE;
	freeblks = totfree;
	blknm = SUPERI;
	fblkp = (struct fblk *) &sbp->s_nfree;
	fblkp->df_nfree = num = (freeblks < NICFREE) ? freeblks : NICFREE;
	freeblks -= num;
	dp = &fblkp->df_free[num];
	for (bn=isize; bn<fsize; bn++) {
		blk = bmap(bn);
		if ( !testblock(blk) ) 
			if ( addfree(blk) == STOP )
				break;
	}
}

addfree(bn)
daddr_t bn;
{
	short num;

	*--dp = bn;
	if (dp > &fblkp->df_free[0])
		return(OK);

	if ( blknm != SUPERI ) {
		canwfblk(fblkp);
		bwrite(blknm, databuf);
	}

	bclear(databuf, BSIZE);
	if (freeblks == 0) {
		bwrite(bn, databuf);
		return(STOP);
	}
	blknm = bn;
	fblkp = (struct fblk *) databuf;
	fblkp->df_nfree = num = (freeblks < NICFREE) ? freeblks : NICFREE;
	freeblks -= num;
	dp = &fblkp->df_free[num];
	return(OK);
}

daddr_t
bmap(blk)
register daddr_t blk;
{
	short i;

	if ( (blk>=mapbot) && (blk<maptop) ) {
		i = blk%n;
		blk -= i;
		blk += maptab[i];
	}
	return(blk);
}

canwfblk(fbp)
register struct fblk *fbp;
{
	register short i;

	for (i=0; i<fbp->df_nfree; i++)
		candaddr(fbp->df_free[i]);
	canshort(fbp->df_nfree);
}
