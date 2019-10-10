/*
 *	general utility functions for phases of fsck
 */

#include "fsck.h"

struct stat stats;			/* Used by statit() */

statit(name, probfun)
char *name;
int (*probfun)();
{
	
	if ( stat(name, &stats) == -1 ) {
		bclear(&stats, sizeof(struct stat));
		(*probfun)("Can not stat %s", name);
	}
}


query(x)
{
	while(1) {
		printf("%r [yes/no]: ", &x);
	
		switch (daction) {
		case YES:
			printf("[default - YES]\n");
			return(YES);
			break;
		case NO:
			printf("[default - NO]\n");
			return(NO);
			break;
		case ASK:
			switch( nextchar() ) {
			case 'Y':
				return(YES);
			case 'N':
				return(NO);
			}
		}
	}
}

action(msg)
char *msg;
{
	switch( query(msg) ) {
	case YES:
		return(TRUE);
	case NO:
		return(FALSE);
	}
}

char
nextchar()
{
	char buff[8];
	int i, n, ch;

	if ( ((n=read(0,buff,8)) <= 0) || (n == 8) )
		fatal("Invalid Response");

	for(i=0; i<n; i++)
		if ( ((ch=buff[i])!=' ') && (ch!='\t') )
			break;

	if ( (ch>='a') && (ch<='z') )
		ch += 'A' - 'a';

	return(ch);
}

char	*nfile	= "FILE";
char	*ndir	= "DIR";
char	*nchr	= "Character Special";
char	*nblk	= "Block Special";
char	*npipe	= "PIPE";
char	*nunkwn = "Unknown I-node type";

char *
typename(ino)
ino_t ino;
{
	switch( flags(ino)&MODEMASK ) {
	case IREG:
		return(nfile);
	case IDIR:
		return(ndir);
	case ICHR:
		return(nchr);
	case IBLK:
		return(nblk);
	case IPIPE:
		return(npipe);
	default:
		return(nunkwn);
	}
}

abort()
{
	fatal("abort.");
}

fatal(x)
{
	printf("fsck: %r\n", &x);
	_exit(1);
}

nonfatal(x)
{
	printf("fsck: %r\n", &x);
	errflag = TRUE;
}

/*
 *	Inode Manipulation Routines
 *	ptrino:	returns a pointer to the wanted inode using the given
 *		buffer to read in the block performing canonicalization.
 *	writeino: writes the block containing the given inode to the
 *		disk performing the necessary canonicalization
 *	candino: canonicalize an inode
 *
 */

struct dinode *
ptrino(ino, buf)
register ino_t ino;
register char *buf;
{
	register daddr_t bn;
	register struct dinode *dip;

	bn = iblockn(ino);
	if ( (testblock(bn)) && (bn!=INODEI) )		/* bad inode block */
		return(NULL);
	bread(bn, buf);
	dip = ((struct dinode *) buf) + iblocko(ino);
	candino(dip);
	return(dip);
}

writeino(ino, buf)
ino_t ino;
char *buf;
{
	struct dinode *dip;

	dip = ((struct dinode *) buf) + iblocko(ino);
	candino(dip);
	bwrite((daddr_t) iblockn(ino), buf);
}

candino(dip)
register struct dinode *dip;
{
	canshort(dip->di_mode);
	canshort(dip->di_nlink);
	canshort(dip->di_uid);
	canshort(dip->di_gid);
	cansize(dip->di_size);
	cantime(dip->di_atime);
	cantime(dip->di_mtime);
	cantime(dip->di_ctime);
}

/*
 *	Transfer a logical block number for an inode to a
 *	physical block number, chasing indirects as necessary.
 *	The inefficiency of this method is put up with since
 *	this is only used for directory inodes, and they tend
 *	not to use indirects.  Pass in 'addrs' which is a pointer
 *	to the 13 daddr_s in the inode, and the logical block
 *	number you would like.  (Stolen from dcheck.c)
 */

static	daddr_t	ranges[] = {
	ND,
	ND + (daddr_t)NI*NBN,
	ND + (daddr_t)NI*NBN + (daddr_t)NII*NBN*NBN,
	ND + (daddr_t)NI*NBN + (daddr_t)NII*NBN*NBN + (daddr_t)NIII*NBN*NBN*NBN
};

static	char	index[] = {
	0,
	ND,
	ND+NI,
	ND+NI+NII
};

static	daddr_t	coeff[] = {
	1,	(daddr_t)NBN,	(daddr_t)NBN*NBN,	(daddr_t)NBN*NBN*NBN
};

daddr_t
imap(addrs, block)
daddr_t *addrs;
register daddr_t block;
{
	int i;
	register daddr_t bpos;
	register daddr_t *bp;
	register daddr_t bn;

	for (i=0; i<4; i++)
		if (block < ranges[i]) {
			if (i != 0)
				block -= ranges[i-1];
			bpos = block/coeff[i];
			block %= coeff[i];
			bp = &addrs[(int)bpos + index[i]];
			if ( (bn=*bp) != 0 ) {
				/*
				 * Map through indirects here
				 */
				while (i-- > 0) {
					bread(bn, databuf);
					bpos = block/coeff[i];
					block %= coeff[i];
					bp = (daddr_t *)databuf + bpos;
					if ( (bn=*bp) == 0 )
						break;
					bn = *bp;
					candaddr(bn);
				}
			}
			return(bn);
		}
	return(0);
}

/*
 *	Read the specified block number into the given buffer
 */

char *rgblkerr = "Out of Range Block number: %U  (CONTINUE)";
char *skblkerr = "Can not Seek: Blk num: %U  (CONTINUE)";
char *rdblkerr = "Can not Read: Blk num: %U  (CONTINUE)";
char *wtblkerr = "Can not Write: Blk num: %U  (CONTINUE)";

bread(bn, buf)
register daddr_t bn;
register char *buf;
{
#ifdef	DEBUG
	printf("Reading block number: %ld\n", bn);
#endif
	if (bn<0 || bn >= fsize ) 
		switch( query(rgblkerr, bn) ) {  
		case YES:
			bclear(buf, BSIZE);
			return;
		case NO:
			abort();
		}

	if ( (bn<isize) && getcache(bn, buf) ) {
#if DEBUG
		bdump(buf);
#endif
		return;
	}
	
	if ( lseek(fsfd, (unsigned long)bn*BSIZE, 0) == (-1L) )
		switch( query(skblkerr, bn) ) {
		case YES:
			bclear(buf, BSIZE);
			return;
		case NO:
			abort();
		}

	if ( read(fsfd, buf, BSIZE) != BSIZE ) 
		switch( query(rdblkerr, bn) ) {
		case YES:
			bclear(buf, BSIZE);
			return;
		case NO:
			abort();
		}
#if DEBUG
	bdump(buf);
#endif
}

/*
 *	Write block number bn with data in buf
 */

bwrite(bn, buf)
daddr_t bn;
char *buf;
{
#ifdef	DEBUG
	printf("Writing block number: %ld\n", bn);
#endif
	if (!writeflg)
		return;

	if (bn<0 || bn>=fsize) 
		switch( query(rgblkerr, bn) ) {
		case YES:
			return;
		case NO:
			abort();
		}
	
	if ( lseek(fsfd, (fsize_t)BSIZE*bn, 0) == (-1L) )
		switch( query(skblkerr, bn) ) {
		case YES:
			return;
		case NO:
			abort();
		}

	if ( write(fsfd, buf, BSIZE) != BSIZE ) 
		switch( query(wtblkerr, bn) ) {
		case YES:
			return;
		case NO:
			abort();
		}

	if ( bn <= isize )
		chgcache(bn, buf);
	changeflg = TRUE;
}

/*
 *	i-node block area caching routines
 */

static char	cache[NBLK * BSIZE];
static int	low = 0; high = -1;

getcache(bn, buf)
register daddr_t bn;
register char *buf;
{
	int totsize;

	if ( bn > isize )
		return(0);

	if ( (bn>=low) && (bn<=high) ) {
		blkmove(buf, &cache[BSIZE * (bn-low)]);
		return(1);
	}

	low = bn;
	if ( (high=bn+NBLK-1) > isize )
		high = isize;
	totsize = BSIZE * (high-low+1);

	if ( (lseek(fsfd, (unsigned long)bn*BSIZE, 0) == (-1L)) ||
	     (read(fsfd, cache, totsize) != totsize) ) {
		low = 0; high = -1;
		return(0);
	}

	return( getcache(bn, buf) );
}

chgcache(bn, buf)
register daddr_t bn;
register char *buf;
{
	if ( (bn>=low) && (bn<=high) )
		blkmove(&cache[BSIZE * (bn-low)], buf);
}

blkmove(to, from)
register char *to, *from;
{
	register int num=BSIZE;

	while ( num-- )
		*to++= *from++;
}

/*
 *	Clear the given block of memory
 */

bclear(bp, nb)
register char *bp;
register unsigned nb;
{
	if (nb)
		do {
			*bp++ = 0;
		} while (--nb);
}


/*
 *	Determine if the given block is bad
 */

bad(bn)
daddr_t bn;
{
	if ( (bn >= isize) && (bn < fsize) )
	 	return(GOOD);
	else
		return(BAD);
}


#if DEBUG

/*
 *	Debug dump a block of data.
 */

#define NUMPLIN	16

bdump(buf)
register unsigned char *buf;
{
	register int i;

	for (i=0; i<BSIZE; i++) {
		if ( (i % NUMPLIN) == 0 )
			printf("\n0x%04x:  ", i);
		printf(" %02x", *buf++);
	}
	printf("\n\n");
}

#endif
