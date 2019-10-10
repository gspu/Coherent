/*
 *	phase 1 of fsck - Check Blocks and Sizes
 */

#include "fsck.h"

char	databuf[BSIZE];		/* buffer for blocks */

int	clrflg;			/* indicates when to clear an inode */
int	dupflag;		/* indicates whether there are any dups */

int	numbad;			/* number of bad blocks in the inode   */
int	numdup;			/* number of duplicate blocks in inode */
long	numblks;		/* number of data blks for size check */
long	sparsecnt;		/* count for sparse blocks for size check */

/*  The following is a table of the number of direct blocks pointed at by the
 *  four types of blocks direct, ..., triple-indirect, to help count sparse
 *  blocks.
 */

daddr_t	blockcnt[] = {1, (daddr_t)NBN, (daddr_t)NBN*NBN, (daddr_t)NBN*NBN*NBN};

phase1()
{
	if (!qflag)
		printf("Phase 1 : Check Blocks and Sizes\n");
	dupflag = FALSE;
	inodescan();
	if (dupflag)
		phase1b();	
}

inodescan()
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
		clrflg = FALSE;
		dip = (struct dinode *) databuf;
		for (i=0; i<INOPB; i++) {
			candino(dip);
			if (inuse(dip) == TRUE) {
				if (!fflag) 
					checkmode(dip, ino);
				if ( checkblks(dip, ino) != STOP ) 
					if ( qflag == FALSE )
						checksize(dip, ino);
			}
			candino(dip);
			ino++;
			dip++;
		}
		if (clrflg)
			bwrite(bn, databuf);
	}
}

/*
 *  	Determine if the given inode is in use.
 */

inuse(dip)
register struct dinode *dip;
{
#ifdef NOT_ALL_ZERO_INODE

	if ( (dip->di_mode != 0) || (dip->di_nlink != 0) )
		return(TRUE);
	else
		return(FALSE);
#endif
#ifdef ALL_ZERO_INODE

	register char *ptr;
	register struct dinode *next;

	ptr = (char *) dip;
	next = dip + 1;
	while (ptr < next)
		if (*ptr++ != 0)
			return(TRUE);

	return(FALSE);
#endif
}

/*
 *	Check the mode of the given inode
 */

checkmode(dip, ino)
register struct dinode *dip;
register ino_t	ino;
{
	register unsigned short	mode;

	mode = dip->di_mode & IFMT;

	switch (mode) {
	case IFREG:
		setflags(ino, IREG);
		return;
	case IFDIR:
		setflags(ino, IDIR);
		return;
	case IFCHR:
		setflags(ino, ICHR);
		return;
	case IFBLK:
		setflags(ino, IBLK);
		return;
	case IFPIPE:
		setflags(ino, IPIPE);
		return;
	default:
		setflags(ino, UNKNOWN);
		break;
	}

	switch ( query("Unknown File Type i-number = %u (Clear)", ino) ){
	case NO:
		return;
	case YES:
		zeroinode(dip);
		setflags(ino, UNALLOC);
		clrflg = TRUE;
		break;
	}
}

/*
 *	Zero the given inode
 */

zeroinode(dip)
struct dinode *dip;
{
	register char *ptr;
	register struct dinode *next;

	lostsize += dip->di_size;
	next = dip+1;
	ptr = (char *) dip;

	while (ptr < next)
		*ptr++ = 0;

}

/*
 *	Check the blocks associated with the given inode to determine
 *	if any are bad or duplicate
 */

checkblks(dip, ino)
struct dinode *dip;
ino_t	ino;
{
	daddr_t	addrs[NADDR];
	register daddr_t bn;
	register int i, lev, naddr;
	register int mode;

	mode = dip->di_mode & IFMT;

	if ( (mode == IFREG) || (mode == IFDIR) )
		l3tol(addrs, dip->di_addr, naddr=NADDR);
	else if ( mode == IFPIPE )
		l3tol(addrs, dip->di_addp, naddr=ND);
	else
		return(STOP);

	numbad = 			/* number of bad blocks so far */
	numdup = 			/* num dup blocks so far THIS INODE */
	numblks =			/* num used data blocks for size chk */
	sparsecnt = 0;			/* count of missed data blocks	*/

	for(i=0; i<naddr; i++)
		for (lev=0; lev<4; lev++) 
			if (i < offsets[lev]) {
				if ( (bn=addrs[i]) != 0 ) {
					if (doblocks(bn, ino, lev) == STOP)
						return(STOP);
				} else
					sparsecnt += blockcnt[lev];
				break;
			}
	return(OK);
}

/*
 *	Checks recursively the blocks pointed at via
 *	the inode list of blocks.  'bn' is the block number,
 *	'ino' is the inode referencing it, and 'lev' is the
 *	level 0 == direct ... 3 = triple-indirect
 */

doblocks(bn, ino, lev)
register daddr_t bn;
ino_t	ino;
int	lev;
{
	char buf[BSIZE];
	register daddr_t *bnptr;
	register char *end;
	register int flag;

	if (lev-- == 0)	{		/* we have a direct block */
		numblks += sparsecnt + 1;
		sparsecnt = 0;
		return(dodirect(bn, ino));
	} else {
		end = &buf[BSIZE];
		if ( (flag=dodirect(bn, ino)) == OK ) {
			bread(bn, buf);
			bnptr = (long *) buf;
			while ( bnptr < end ) {
				bn = *bnptr++;
				candaddr(bn);
				if ( bn == 0 ) {
					sparsecnt += blockcnt[lev];
					continue;
				}
				if ( doblocks(bn, ino, lev) == STOP )
					return(STOP);
			}
			return(OK);
		} else 
			return(flag);
	}
}
			
/*
 *	Check the given block to determine if it is bad
 * 	or if it is a duplicate.  'ino' is the inode referencing it
 */

dodirect(bn, ino)
register daddr_t bn;
register ino_t ino;
{
	register int flag;
	
	if ( (flag=checkbad(bn, ino)) == OK ) 
		return( checkdup(bn, ino) );
	else
		return(flag);
}

/*
 *	Check the given block number for being bad.
 */

checkbad(bn, ino)
register daddr_t bn;
ino_t	ino;
{
	if ( (bn>=isize) && (bn<fsize) )
		return(OK);

	else if ( (bn<isize) && (bn>=INODEI) && (ino == 1) ) {
							/* bad block is in  */
		totfree++;				/* the inode blocks */
		return(OK);
	}
		
	if (!fflag)
		orflags(ino, IBAD_IDUP);

	printf("Bad block %lu, i-number = %u\n", bn, ino);

	if (numbad++ < MAXBADOK)
		return(BAD_DUP);

	switch ( query("Excessive Bad Blocks i-number = %u (Continue)", ino) ){
	case NO:
		abort();
	case YES:
		return(STOP);
	}
}


/*
 *	Check the given block number for duplicate reference.
 */

checkdup(bn, ino)
register daddr_t bn;
ino_t	ino;
{
	if ( !testblock(bn) ) {
		markblock(bn);
		totfree--;
		return(OK);
	}

	dupflag = TRUE;
	if (!fflag)
		orflags(ino, IBAD_IDUP);
	printf("Dup Block %lu, i-number = %u\n", bn, ino);

	if (totdups < DUPTBLSIZE) 
		dupblck[totdups++] = bn;
	else {
		switch ( query("DUP Table Overflow (Continue)") ) {
		case NO:
			abort();
		case YES:
			return(BAD_DUP);
		}
	}

	if (numdup++ < MAXDUPOK)
		return(BAD_DUP);

	switch ( query("Excessive Dup Blocks i-number = %u (Continue)", ino) ) {
	case NO:
		abort();
	case YES:
		return(STOP);
	}
}

/*
 *	Check For Possible File Size Error 
 */

checksize(dip, ino)
register struct dinode *dip;
ino_t	ino;
{
	register int mode;
	register fsize_t size;

	mode = dip->di_mode & IFMT;
	size = dip->di_size;
	
	if ( mode == IFREG )
		filesize(ino, size);
	else if ( mode == IFDIR )
		dirsize(dip, ino, size);
	else if ( mode == IFPIPE )
		pipesize(dip, ino, size);
}

dirsize(dip, ino, size)
struct dinode *dip;
ino_t ino;
register fsize_t size;
{
	if ( size <= 0 ) {
		switch ( query("\
Bad Directory Size, size = %D, i-number = %u (Clear i-node)", size, ino) ) {
		case NO:
			break;
		case YES:
			zeroinode(dip);
			setflags(ino, UNALLOC);
			clrflg = TRUE;
			return;
		}
	}

	if ( size < (2*DSIZE) )
		printf("Directory Size too small i-number = %u\n", ino);

	if ( size % sizeof(struct direct) != 0 )
		printf("Directory Misaligned i-number = %u\n", ino);

	if (sizerr(size))
		printf("Possible Directory Size Error i-number = %u\n", ino);
}

filesize(ino, size)
ino_t ino;
register fsize_t size;
{
	if (sizerr(size))
		printf("Possible File Size Error i-number = %u\n", ino);
}

pipesize(dip, ino, size)
struct dinode *dip;
ino_t ino;
register fsize_t size;
{
	if (sizerr(size))
		printf("Possible PIPE Size Error i-number = %u\n", ino);
	return;
}

sizerr(size)
register fsize_t size;
{
	register fsize_t calc;

	calc = (unsigned long)numblks*BSIZE;
	if ( (size > calc) || ( calc >= (size+BSIZE) ) ) 
		return(TRUE);
	else
		return(FALSE);
}
