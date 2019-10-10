/*
 * bad.c
 * 11/6/92
 * Usage: bad option filesystem [ block ... ]
 * Manipulate bad block list.
 */

#include <stdio.h>
#include <canon.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>

#define	USAGE	"\
Usage:	bad option filesystem [ block ... ]\n\
Options:\n\
	a	Add blocks\n\
	c	Clear bad-block list\n\
	d	Delete blocks\n\
	l	List blocks\n"

/* Forward. */
int	addlist();
long	atol();
char	*bread();
char	*bclaim();
char	*index();
char	*realloc();
daddr_t	balloc();
int	dellist();

/* Globals. */
struct	filsys supb;			/* Super block */
INODE	inol;				/* Bad block inode */
char	bufl[1+NI][BSIZE];		/* Buffers for bread */
daddr_t	bnol[1+NI];			/* Block numbers */
daddr_t	*badl;				/* Bad block list */
int	badn;				/* Number of bad blocks in list */
int	badm;				/* Maximum number of bad blocks */
int	filf;				/* Filesystem file descriptor */

main(argc, argv)
char *argv[];
{
	register int i;

	if (argc < 3)
		usage();
	if ((filf=open(argv[2], (argv[1][0] == 'l') ? 0 : 2)) < 0)
		panic("cannot open \"%s\"", argv[2]);
	badm = 64;
	if ((badl=malloc(badm*sizeof(*badl))) == NULL)
		panic("out of memory");
	switch (argv[1][0]) {
	case 'a':
		gather();
		for (i=3; i<argc; i++)
			route(argv[i], addlist);
		update();
		break;
	case 'c':
		update();
		break;
	case 'd':
		gather();
		for (i=3; i<argc; i++)
			route(argv[i], dellist);
		update();
		break;
	case 'l':
		gather();
		display();
		break;
	default:
		usage();
	}
}

/*
 * Print out usage.
 */
usage()
{
	fprintf(stderr, USAGE);
	exit(1);
}

/*
 * Given a string and a pointer to a function, extract a list of block
 * numbers from the string and call the function with them.
 */
route(cp1, f)
register char *cp1;
int (*f)();
{
	register char *cp2;
	register long b;
	register long l;

	if ((cp2=index(cp1, '-')) == NULL)
		(*f)(atol(cp1));
	else {
		*cp2++ = '\0';
		l = atol(cp2);
		for (b=atol(cp1); b<=l; b++)
			(*f)(b);
	}
}

/*
 * List the set of bad blocks.
 */
display()
{
	register int i;

	for (i=0; i<badn; i++)
		printf("%ld\n", badl[i]);
}

/*
 * Read the list of bad blocks into the bad block list.
 */
gather()
{
	register int n;
	register daddr_t b;

	copydm();
	for (n=0; n<ND; n++) {
		if ((b=inol.i_a.i_addr[n]) != 0)
			addlist(b);
	}
	for (n=ND; n<ND+NI; n++) {
		if ((b=inol.i_a.i_addr[n]) != 0)
			indlist(b, 1+n-ND);
	}
}

/*
 * Given an indirect block, `b', with level of indirection, `l', add all
 * allocated blocks onto the bad block list.
 */
indlist(b, l)
daddr_t b;
register unsigned l;
{
	register int i;
	register char *bp;
	daddr_t b1;

	if (b == 0)
		return;
	if (l-- > 0) {
		bp = bread(l, b);
		for (i=0; i<NBN; i++) {
			if ((b1=((daddr_t *)bp)[i]) == 0)
				continue;
			candaddr(b1);
			if (l == 0)
				addlist(b1);
			else
				indlist(b1, l);
		}
	}
}

/*
 * Add the given block onto the end of the bad block list.
 */
addlist(b)
daddr_t b;
{
	register int i;
	register int n;

	if (badn >= badm) {
		if ( badm >= (2*badm) )		/* catch wordlength rollover */
			panic("too many bad blocks");
		badm *= 2;
		if ((badl=realloc(badl, badm*sizeof(*badl))) == NULL)
			panic("out of memory");
	}
	for (i=0; i<badn; i++) {
		if (b == badl[i])
			panic("duplicate bad block %ld", b);
		if (b < badl[i])
			break;
	}
	for (n=badn; n>i; --n)
		badl[n] = badl[n-1];
	badl[i] = b;
	badn++;
}

/*
 * Delete the given bad block from the bad block list.
 */
dellist(b)
daddr_t b;
{
	register int n;

	for (n=0; n<badn; n++) {
		if (b == badl[n]) {
			for (--badn; n<badn; n++)
				badl[n] = badl[n+1];
			return;
		}
	}
	panic("cannot find block %ld", b);
}

/*
 * Update the bad block file.
 */
update()
{
	register char *bp;
	register int i;

	bp = bread(0, (daddr_t)SUPERI);
	strcopy(bp, (char *)&supb, sizeof(struct filsys));
	cansuper(&supb);
	for (i=0; i<ND+NI; i++)
		inol.i_a.i_addr[i] = 0;
	for (i=0; i<badn; i++)
		block(i, badl[i]);
	copymd();
	bp = bread(0, (daddr_t)SUPERI);
	strcopy((char *)&supb, bp, sizeof(supb));
	cansuper((struct filsys *)bp);
	bwrite(bp, (daddr_t)SUPERI);
}

/*
 * Canonize the super block.
 */
cansuper(sbp)
register struct filsys *sbp;
{
	register int i;

	canint(sbp->s_isize);
	candaddr(sbp->s_fsize);
	canshort(sbp->s_nfree);
	for (i=0; i<NICFREE; i++)
		candaddr(sbp->s_free[i]);
	canshort(sbp->s_ninode);
	for (i=0; i<NICINOD; i++)
		canino(sbp->s_inode[i]);
	cantime(sbp->s_time);
	candaddr(sbp->s_tfree);
	canino(sbp->s_tinode);
	canshort(sbp->s_m);
	canshort(sbp->s_n);
	canlong(sbp->s_unique);
}

/*
 * Copy the bad block inode to memory performing canonization.
 */
copydm()
{
	register struct dinode *dip;
	register char *bp;

	bp = bread(0, (daddr_t)iblockn(BADFIN));
	dip = (struct dinode *)bp+iblocko(BADFIN);
	inol.i_mode = dip->di_mode;
	canshort(inol.i_mode);
	inol.i_nlink = dip->di_nlink;
	canshort(inol.i_nlink);
	inol.i_uid = dip->di_uid;
	canshort(inol.i_uid);
	inol.i_gid = dip->di_gid;
	canshort(inol.i_gid);
	inol.i_size = dip->di_size;
	cansize(inol.i_size);
	l3tol(inol.i_a.i_addr, dip->di_addr, NADDR);
	inol.i_atime = dip->di_atime;
	cantime(inol.i_atime);
	inol.i_mtime = dip->di_mtime;
	cantime(inol.i_mtime);
	inol.i_ctime = dip->di_ctime;
	cantime(inol.i_ctime);
	return (1);
}

/*
 * Copy the bad block inode to disk performing canonization.
 */
copymd()
{
	register struct dinode *dip;
	register char *bp;

	bp = bread(0, (daddr_t)iblockn(BADFIN));
	dip = (struct dinode *)bp+iblocko(BADFIN);
	dip->di_mode = IFREG;
	canshort(dip->di_mode);
	dip->di_nlink = inol.i_nlink;
	canshort(dip->di_nlink);
	dip->di_uid = inol.i_uid;
	canshort(dip->di_uid);
	dip->di_gid = inol.i_gid;
	canshort(dip->di_gid);
	dip->di_size = ((fsize_t)badn)*BSIZE;
	cansize(dip->di_size);
	ltol3(dip->di_addr, inol.i_a.i_addr, NADDR);
	dip->di_atime = inol.i_atime;
	cantime(dip->di_atime);
	dip->di_mtime = inol.i_mtime;
	cantime(dip->di_mtime);
	dip->di_ctime = inol.i_ctime;
	cantime(dip->di_ctime);
	bwrite(bp, (daddr_t)iblockn(BADFIN));
}

/*
 * Make the physical block, `b', logical block, `n' of the bad block file.
 */
block(n, b)
register int n;
register daddr_t b;
{
	register daddr_t pb;
	register char *bp;
	register daddr_t pbb;
	register char *bpp;

	if (n < ND) {
		inol.i_a.i_addr[n] = b;
		return;
	}
	n -= ND;
	if (n < NBN) {
		if ((pb=inol.i_a.i_addr[ND]) != 0)
			bp = bread(1, pb);
		else {
			inol.i_a.i_addr[ND] = pb = balloc();
			bp = bclaim(1);
		}
		((daddr_t *)bp)[n] = b;
		candaddr(((daddr_t *)bp)[n]);
		bwrite(bp, pb);
		return;
	}
	n -= NBN;
	if (n < NBN*NBN) {
		if ((pbb=inol.i_a.i_addr[ND+1]) != 0)
			bpp = bread(2, pbb);
		else {
			inol.i_a.i_addr[ND+1] = pbb = balloc();
			bpp = bclaim(2);
		}
		pb = ((daddr_t *)bpp)[n/NBN];
		candaddr(pb);
		if (pb != 0)
			bp = bread(1, pb);
		else {
			((daddr_t *)bpp)[n/NBN] = pb = balloc();
			candaddr(((daddr_t *)bpp)[n/NBN]);
			bwrite(bpp, pbb);
			bp = bclaim(1);
		}
		((daddr_t *)bp)[n%NBN] = b;
		candaddr(((daddr_t *)bp)[n%NBN]);
		bwrite(bp, pb);
		return;
	}
	panic("bad block file too large");
}

/*
 * Allocate a block from our filesystem.
 */
daddr_t
balloc()
{
	register struct fblk *fbp;
	register daddr_t b;
	register int i;

next:
	if (supb.s_tfree==0 || (b=supb.s_free[--supb.s_nfree])==0)
		panic("out of space on filesystem");
	if (supb.s_nfree == 0) {
		fbp = (struct fblk *)bread(0, b);
		supb.s_nfree = fbp->df_nfree;
		canshort(supb.s_nfree);
		for (i=0; i<sizeof(supb.s_free); i++) {
			supb.s_free[i] = fbp->df_free[i];
			candaddr(supb.s_free[i]);
		}
	}
	--supb.s_tfree;
	if (b>=supb.s_fsize || b<supb.s_isize)
		panic("bad block %u (alloc)", (unsigned)b);
	for (i=0; i<badn; i++)
		if (b == badl[i])
			goto next;
	return (b);
}

/*
 * Given a buffer number, claim the buffer.
 */
char *
bclaim(l)
{
	register char *bp;
	register int n;

	n = BSIZE;
	bp = bufl[l];
	do {
		*bp++ = 0;
	} while (--n);
	return (bufl[l]);
}

/*
 * Given a buffer number, read the given block into the given buffer and
 * return a pointer to the buffer.
 */
char *
bread(l, b)
daddr_t b;
{
	register char *bp;

	bp = bufl[l];
	if (b != bnol[l]) {
		lseek(filf, (long)b*BSIZE, 0);
		if (read(filf, bp, BSIZE) != BSIZE)
			panic("read error on block %ld", b);
		bnol[l] = b;
	}
	return (bp);
}

/*
 * Given a pointer to a buffer, write it out as the given block.
 */
bwrite(bp, b)
char *bp;
daddr_t b;
{

	lseek(filf, (long)b*BSIZE, 0);
	if (write(filf, bp, BSIZE) != BSIZE)
		panic("write error on block %ld", b);
}

/*
 * Copy `n' bytes from `bp1' to `bp2'.
 */
strcopy(bp1, bp2, n)
register char *bp1;
register char *bp2;
register unsigned n;
{
	if (n == 0)
		return;
	do {
		*bp2++ = *bp1++;
	} while (--n);
}

/*
 * Print out an error message and exit.
 */
/* VARARGS */
panic(a1)
char *a1;
{
	fprintf(stderr, "bad: %r\n", &a1);
	exit(1);
}

/* end of bad.c */
