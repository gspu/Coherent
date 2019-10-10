/* $Header: /newbits/kernel/USRSRC/coh/RCS/seg.c,v 1.4 91/07/24 07:51:50 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Coherent.
 * Segment manipulation.
 *
 * $Log:	seg.c,v $
 * Revision 1.4  91/07/24  07:51:50  bin
 * update prov by hal
 * 
 * 
 * Revision 1.1	88/03/24  16:14:20	src
 * Initial revision
 * 
 * 88/02/26	Allan Cornish	/usr/src/sys/coh/seg.c
 * swapio() now avoids 64 Kbyte page [dma] straddles.
 *
 * 88/01/22	Allan Cornish	/usr/src/sys/coh/seg.c
 * salloc() now invokes krunch(1000) if initial allocation fails.
 * sfree() now invokes krunch(0).
 *
 * 88/01/21	Allan Cornish	/usr/src/sys/coh/seg.c
 * sfree() modified to eliminate critical race on ref cnts and segment gate.
 * segfinm() now properly maintains segment reference counts.
 *
 * 87/11/13	Allan Cornish	/usr/src/sys/coh/seg.c
 * Support for protected mode segmentation added.
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <errno.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <sys/uproc.h>

/*
 * Initialisation code.
 */
seginit()
{
	/*
	 * Create empty circular-list of memory segments.
	 */
	segmq.s_forw = &segmq;
	segmq.s_back = &segmq;

	/*
	 * Create empty circular-list of disk segments.
	 */
	segdq.s_forw = &segdq;
	segdq.s_back = &segdq;

	if ( holebot != holetop ) {
		/*
		 * Define the I/O mem hole between low memory and extended mem.
		 * NOTE: Setting lrefc to urefc+1 stopx segment from moving.
		 */
		segiom.s_paddr = holebot;
		segiom.s_size  = holetop - holebot;
		segiom.s_flags = SFCORE | SFSYST;
		segiom.s_urefc = 1;
		segiom.s_lrefc = 2;

		/*
		 * Insert I/O memory segment into memory list.
		 */
		segiom.s_forw = &segmq;
		segiom.s_back = &segmq;
		segmq.s_forw  = &segiom;
		segmq.s_back  = &segiom;
	}
}

/*
 * Given an inode, `ip', and flags, `ff', describing a segment associated
 * with the inode, see if the segment already exists and if so, return a
 * copy.  If the segment does not exists, allocate the segment having size
 * `ss', and read the segment using the inode at seek offset `dq' with a
 * size of `ds'.
 */
SEG *
ssalloc(rp, ip, ff, ss, dq, ds)
int *rp;
register INODE *ip;
fsize_t ss;
fsize_t dq;
fsize_t ds;
{
	register SEG *sp;
	register int f;

	*rp = -1;
	if (ss == 0) {
		*rp = 1;
		return (NULL);
	}
	lock(seglink);
	f = ff & (SFSHRX|SFTEXT);

	/*
	 * Look for the segment in the memory queue.
	 */
	for (sp=segmq.s_forw; sp!=&segmq; sp=sp->s_forw) {
		if (sp->s_ip==ip && (sp->s_flags&(SFSHRX|SFTEXT))==f) {
			unlock(seglink);
			if ((sp = segdupl(sp)) != NULL) {
				segfinm(sp);
				*rp = 1;
			}
			return (sp);
		}
	}

	/*
	 * Look for the segment on the disk queue.
	 */
	for (sp=segdq.s_forw; sp!=&segdq; sp=sp->s_forw) {
		if (sp->s_ip==ip && (sp->s_flags&(SFSHRX|SFTEXT))==f) {
			unlock(seglink);
			if ((sp = segdupl(sp)) != NULL) {
				segfinm(sp);
				*rp = 1;
			}
			return (sp);
		}
	}
	unlock(seglink);

	/*
	 * Allocate and create the segment.
	 */
	if ((sp=salloc(ss, ff)) == NULL)
		return (NULL);
	if (exsread(sp, ip, ds, dq, (fsize_t)0) == 0) {
		sfree(sp);
		return (NULL);
	}
	if ((ff&SFSHRX) != 0) {
		sp->s_ip = ip;
		ip->i_refc++;
	}
	*rp = 0;
	return (sp);
}

/*
 * Given a pointer to a newly created process, copy all of our segments
 * into the given process.
 */
segadup(cpp)
register PROC *cpp;
{
	register SEG *sp;
	register int n;
	register PROC *pp;

	pp = SELF;
	cpp->p_flags |= PFSWIO;
	for (n=0; n<NUSEG; n++) {
		if ((sp=pp->p_segp[n]) == NULL)
			continue;
		if ((sp=segdupl(sp)) == NULL)
			break;
		cpp->p_segp[n] = sp;
		if ((sp->s_flags&SFCORE) == 0)
			cpp->p_flags &= ~PFCORE;
	}
	if (n < NUSEG) {
		while (n > 0) {
			if ((sp=cpp->p_segp[--n]) != NULL) {
				cpp->p_segp[n] = NULL;
				sfree(sp);
			}
		}
	}
	cpp->p_flags &= ~PFSWIO;
	return (n);
}

/*
 * Duplicate a segment.
 */
SEG *
segdupl(sp)
register SEG *sp;
{
	register SEG *sp1;

	if ((sp->s_flags&SFSHRX) != 0) {
		sp->s_urefc++;
		sp->s_lrefc++;
		return (sp);
	}
	if ((sp->s_flags&SFCORE) == 0)
		panic("Cannot duplicate non shared swapped segment");
	if ((sp1=salloc(sp->s_size, sp->s_flags|SFNSWP|SFNCLR)) == NULL)
		sp1 = segdupd(sp);
	else {
		sp1->s_flags = sp->s_flags;
		plrcopy( sp->s_paddr, sp1->s_paddr, sp->s_size );
	}
	return (sp1);
}

/*
 * Allocate a segment `n' bytes long.  `f' contains some pseudo flags.
 */
SEG *
salloc(n, f)
fsize_t n;
{
	register SEG *sp;
	register int r;

	r = (f&(SFSYST|SFHIGH|SFTEXT|SFSHRX|SFDOWN)) | SFCORE;
	n +=  (BSIZE-1);
	n &= ~(BSIZE-1);

	lock(seglink);
	sp = sxalloc(n, f);
	unlock(seglink);

	if ( sp == NULL ) {
		krunch(1000);
		lock(seglink);
		sp = sxalloc(n, f);
		unlock(seglink);
	}

	if (sp != NULL) {
		sp->s_flags = r;
		vremap( sp );
	}
	else {
		if ((f&SFNSWP) != 0)
			return (NULL);
		if ((sp=kalloc(sizeof(SEG))) == NULL)
			return (NULL);
		sp->s_forw = sp;
		sp->s_back = sp;
		sp->s_flags = r;
		sp->s_urefc = 1;
		sp->s_lrefc = 1;
		if (segsext(sp, n) == NULL) {
			kfree(sp);
			return (NULL);
		}
	}
	if ((f&SFNCLR) == 0)
		pclear( sp->s_paddr, n );
	return (sp);
}

/*
 * Free the given segment pointer.
 */
sfree(sp)
register SEG *sp;
{
	register INODE *ip;

	if ( sp->s_urefc != 1 ) {
		sp->s_urefc--;
		sp->s_lrefc--;
		return;
	}

	lock(seglink);
	--sp->s_lrefc;
	if (--sp->s_urefc != 0) {
		unlock(seglink);
		return;
	}

	sp->s_back->s_forw = sp->s_forw;
	sp->s_forw->s_back = sp->s_back;
	unlock(seglink);

	if (sp->s_lrefc != 0)
		panic("Bad segment count");
	if ((ip=sp->s_ip) != NULL)
		ldetach(ip);
	vrelse( sp->s_faddr );
	kfree(sp);
	krunch(0);
}

/*
 * Grow or shrink the segment `sp' so that it has size `n'.
 */
seggrow(sp, n)
register SEG *sp;
fsize_t n;
{
	register SEG *sp1;
	register fsize_t  d;
	register paddr_t pb;
	register paddr_t nb;
	register int dowflag;

	dowflag = sp->s_flags&SFDOWN;

	/*
	 * Size of new segment is smaller or the same size as the old
	 * segment.
	 */
	lock(seglink);
	d = n - sp->s_size;
	if (n <= sp->s_size) {
		sp->s_size = n;
		if (dowflag)
			sp->s_paddr -= d;

		vremap( sp );
		unlock(seglink);
		return (1);
	}

	if ((sp1=sp->s_back) == &segmq)
		pb = corebot;
	else
		pb = sp1->s_paddr + sp1->s_size;

	if ((sp1=sp->s_forw) == &segmq)
		nb = coretop;
	else
		nb = sp1->s_paddr;

	/*
	 * If the segment does not grow down, see if there is enough
	 * space after the segment.
	 */
	if (dowflag==0 && nb-sp->s_paddr>=n) {
		pclear(sp->s_paddr+sp->s_size, d);
		sp->s_size = n;
		vremap( sp );
		unlock(seglink);
		return (1);
	}

	/*
	 * If the segment grows down, see if there is enough space
	 * before the segment.
	 */
	if (dowflag!=0 && sp->s_paddr+sp->s_size-pb>=n) {
		sp->s_paddr -= d;
		sp->s_size   = n;
		pclear( sp->s_paddr, d );
		vremap( sp );
		unlock(seglink);
		return (1);
	}

	/*
	 * Is there enough space in total counting the gaps on either
	 * side of us?
	 */
	if (nb-pb >= n) {
		if (dowflag == 0) {
			plrcopy(sp->s_paddr, pb, sp->s_size);
			pclear(pb+sp->s_size, d);
			sp->s_paddr = pb;
		} else {
			prlcopy( sp->s_paddr, nb-sp->s_size, sp->s_size );
			pclear(nb-n, d);
			sp->s_paddr = nb-n;
		}
		sp->s_size  = n;
		vremap( sp );
		unlock(seglink);
		return (1);
	}

	/*
	 * Try to allocate a segment somewhere else on the segment queue
	 * and copy ourselves there.
	 */
	unlock(seglink);
	if ((sp1=salloc((fsize_t)n, sp->s_flags|SFNSWP|SFNCLR)) != NULL) {
		if (dowflag == 0) {
			plrcopy(sp->s_paddr, sp1->s_paddr, sp->s_size);
			pclear(sp1->s_paddr+sp->s_size, d);
		} else {
			plrcopy(sp->s_paddr, sp1->s_paddr+d, sp->s_size);
			pclear(sp1->s_paddr, d);
		}
		lock(seglink);
		satcopy(sp, sp1);
		unlock(seglink);
		return (1);
	}

	/*
	 * Last chance.  Extend the segment by swapping it.
	 */
	if (segsext(sp, n) != NULL) {
		if (dowflag == 0)
			pclear(sp->s_paddr+n-d, d);
		else {
			prlcopy(sp->s_paddr, sp->s_paddr+d, n-d);
			pclear(sp->s_paddr, d);
		}
		return (1);
	}

	/*
	 * At least we tried.
	 */
	return (0);
}

/*
 * Given a segment pointer, `sp' and a segment size, grow the given segment
 * to the given size.
 */
segsize(sp, s2)
register SEG *sp;
vaddr_t s2;
{
	register vaddr_t s1;

	s1 = (vaddr_t) sp->s_size;
	if (seggrow(sp, (fsize_t)s2) == 0) {
		u.u_error = ENOMEM;
		return;
	}
	if (sproto() == 0)
		if (seggrow(sp, (fsize_t)s1)==0 || sproto()==0)
			sendsig(SIGSEGV, SELF);
	segload();
}

/*
 * Grow the segment `sp1' to the size `s' in bytes by swapping it out
 * and back in.  The segment may not be locked.
 */
SEG *
segsext(sp1, s)
register SEG *sp1;
register fsize_t s;
{
	register SEG *sp2;

#ifndef NOMONITOR
	if (swmflag)
		printf("Segsext(%p, %u)\n", SELF, SELF->p_pid);
#endif
	if (sexflag == 0) {
		u.u_error = ENOMEM;
		return (NULL);
	}
	lock(seglink);
	if ((sp2=sdalloc(s)) == NULL) {
		unlock(seglink);
		return (NULL);
	}
	unlock(seglink);
	sp1->s_lrefc++;
	if (sp1->s_size != 0)
		swapio(1, sp1->s_paddr, sp2->s_daddr, sp1->s_size);
	lock(seglink);
	satcopy(sp1, sp2);
	unlock(seglink);
	sp1->s_flags &= ~SFCORE;
	sp1->s_lrefc--;
	vremap(sp1);
	segfinm(sp1);
	return (sp1);
}

/*
 * Force the given segment to be in memory.  One can only force
 * one segment to be in memory at a time.
 */
segfinm(sp)
register SEG *sp;
{
	register PROC *pp;
	register int s;

	if ((sp->s_flags&SFCORE) != 0)
		return;
	pp = SELF;
	sp->s_urefc++;
	sp->s_lrefc++;
	pp->p_segp[SIAUXIL] = sp;
	pp->p_flags &= ~PFCORE;
#ifndef QWAKEUP
	s = sphi();
#endif
	setrun(pp);
	dispatch();
#ifndef QWAKEUP
	spl(s);
#endif
	pp->p_segp[SIAUXIL] = NULL;
	sfree(sp);
}

/*
 * Make a copy of the segment `sp1' which is in memory by writing
 * it out to disk.
 */
SEG *
segdupd(sp1)
register SEG *sp1;
{
	register SEG *sp2;

	if (sexflag == 0)
		return (NULL);
	lock(seglink);
	if ((sp2=sdalloc(sp1->s_size)) == NULL) {
		unlock(seglink);
		return (NULL);
	}
	sp1->s_lrefc++;
	unlock(seglink);
	swapio(1, sp1->s_paddr, sp2->s_daddr, sp1->s_size);
	sp1->s_lrefc--;
	sp2->s_flags = sp1->s_flags & ~SFCORE;
	sp2->s_size  = sp1->s_size;
	vremap( sp2 );
	return (sp2);
}

/*
 * Given a flag, a physical core address, a disk address and a count in
 * bytes, perform an I/O operation between core and disk.  If `flag' is
 * set, the transfer is to the disk otherwise it is to memory.  As you may
 * have guessed, this is used by the swapper.
 */
swapio(f, p, d, n)
paddr_t p;
daddr_t d;
fsize_t  n;
{
	register BUF * bp;
	register SEG * sp;
	register int s;
	register int nb;
	static SEG swapseg;	/* NOTE: FP_SEL(swapseg.s_faddr) must stay */

#ifndef NOMONITOR
	if (swmflag > 1)
		printf("swapio(%s,%x,%x,%x)\n",f?"out":"in",(int)p,(int)d,n);
#endif
	if (d < swapbot || d+(n/BSIZE) > swaptop
	 || p < corebot || p+n > coretop)
		panic("Swapio bad parameter");

	bp = &swapbuf;
	sp = &swapseg;
	lock(bp->b_gate);
	SELF->p_flags |= PFSWIO;
	sp->s_flags = SFCORE;
	sp->s_paddr = p;
	sp->s_size  = n;
	vremap( sp );
	bp->b_faddr = sp->s_faddr;

	while (n != 0) {
		nb = (n > SCHUNK) ? SCHUNK : n;
		/*
		 * Prevent I/O transfer from crossing 64 Kbyte boundary.
		 */
		if ( (p & 0xFFFF0000L) != ((p+nb) & 0xFFFF0000L) )
			nb = 0x10000L - (p & 0x0000FFFFL);
		bp->b_flag  = BFNTP;
		bp->b_req   = f ? BWRITE : BREAD;
		bp->b_dev   = swapdev;
		bp->b_bno   = d;
		bp->b_paddr = p;
		bp->b_count = nb;
		s = sphi();
		dblock(swapdev, bp);
		while ((bp->b_flag&BFNTP) != 0)
			sleep((char *)bp, CVBLKIO, IVBLKIO, SVBLKIO);
		spl(s);
		if ((bp->b_flag&BFERR) != 0)
			panic("Swapio error");
		FP_OFF(bp->b_faddr) += nb;
		p += nb;
		d += nb / BSIZE;
		n -= nb;
	}
	sp->s_flags = 0;
	vremap( sp );
	unlock(bp->b_gate);
	SELF->p_flags &= ~PFSWIO;
}

/*
 * Make the segment descriptor pointed to by `sp1' have the attributes
 * of `sp2' including it's position in the segment queue and release
 * `sp2'.  `seglink' must be locked when this routine is called.
 */
satcopy(sp1, sp2)
register SEG *sp1;
register SEG *sp2;
{
	if ( FP_SEL(sp2->s_faddr) != 0 )
		vrelse( sp2->s_faddr );

	sp1->s_back->s_forw = sp1->s_forw;
	sp1->s_forw->s_back = sp1->s_back;
	sp2->s_back->s_forw = sp1;
	sp1->s_back = sp2->s_back;
	sp2->s_forw->s_back = sp1;
	sp1->s_forw  = sp2->s_forw;
	sp1->s_size  = sp2->s_size;
	sp1->s_paddr = sp2->s_paddr;
	sp1->s_daddr = sp2->s_daddr;
	vremap(sp1);
	kfree(sp2);
}

/*
 * Allocate a segment on disk that is `n' bytes long.
 * The `seglink' gate should be locked before this routine is called.
 */
SEG *
sdalloc( s )
fsize_t s;
{
	register SEG *sp1;
	register SEG *sp2;
	register daddr_t d;
	register daddr_t d1;
	register daddr_t d2;

	d  = s / BSIZE;
	d1 = swapbot;
	sp1 = &segdq;
	do {
		if (d1 >= swaptop)
			return (NULL);
		if ((sp1=sp1->s_forw) != &segdq)
			d2 = sp1->s_daddr;
		else
			d2 = swaptop;
		if (d2-d1 >= d) {
			if ((sp2=kalloc(sizeof(SEG))) == NULL)
				return (NULL);
			sp1->s_back->s_forw = sp2;
			sp2->s_back = sp1->s_back;
			sp1->s_back = sp2;
			sp2->s_forw = sp1;
			sp2->s_urefc = 1;
			sp2->s_lrefc = 1;
			sp2->s_size  = s;
			sp2->s_daddr = d1;
			return (sp2);
		}
		d1 = sp1->s_daddr + (sp1->s_size / BSIZE);
	} while (sp1 != &segdq);
	return (NULL);
}

/*
 * Allocate a segment in memory that is `n' bytes long.
 * The `seglink' gate should be locked before this routine is called.
 */
SEG *
smalloc(s)
fsize_t s;
{
	register SEG *sp1;
	register SEG *sp2;
	paddr_t p1;
	paddr_t p2;

	p1  = corebot;
	sp1 = &segmq;
	do {
		if ((sp1=sp1->s_forw) != &segmq)
			p2 = sp1->s_paddr;
		else
			p2 = coretop;

		if (p2-p1 >= s) {
			if ((sp2=kalloc(sizeof (SEG))) == NULL)
				return (NULL);
			sp1->s_back->s_forw = sp2;
			sp2->s_back = sp1->s_back;
			sp1->s_back = sp2;
			sp2->s_forw = sp1;
			sp2->s_urefc = 1;
			sp2->s_lrefc = 1;
			sp2->s_size  = s;
			sp2->s_paddr = p1;
			/*   s_faddr = 0; */
			/*   s_flags = 0; */
			vremap( sp2 );
			return (sp2);
		}
		p1 = sp1->s_paddr + sp1->s_size;
	} while (sp1 != &segmq);
	return (NULL);
}

/*
 * Allocate a segment from the high end of memory that is `n' bytes long.
 * The `seglink' gate should be locked before this routine is called.
 */
SEG *
shalloc( s )
fsize_t s;
{
	register SEG *sp1;
	register SEG *sp2;
	paddr_t p1;
	paddr_t p2;

	sp1 = &segmq;
	p2  = coretop;
	do {
		if ((sp1=sp1->s_back) != &segmq)
			p1 = sp1->s_paddr + sp1->s_size;
		else
			p1 = corebot;

		if (p2-p1 >= s) {
			if ((sp2=kalloc(sizeof (SEG))) == NULL)
				return (NULL);
			sp1->s_forw->s_back = sp2;
			sp2->s_forw = sp1->s_forw;
			sp1->s_forw = sp2;
			sp2->s_back = sp1;
			sp2->s_urefc = 1;
			sp2->s_lrefc = 1;
			sp2->s_size  = s;
			sp2->s_paddr = p2-s;
			/*   s_faddr = 0; */
			/*   s_flags = 0; */
			vremap( sp2 );
			return (sp2);
		}
		p2 = sp1->s_paddr;
	} while (sp1 != &segmq);
	return (NULL);
}

/*
 * Set up `SR' structure in user area from segments descriptors in
 * process structure.  Also set up the user segmentation registers.
 */
sproto()
{
	register int n;
	register SEG *sp;

	kclear(u.u_segl, sizeof(u.u_segl));
	for (n=0; n<NUSEG; n++) {
		if ((sp=SELF->p_segp[n]) == NULL)
			continue;
		if (n == SIUSERP)
			u.u_segl[n].sr_base = &u;
		else
			u.u_segl[n].sr_flag |= SRFPMAP;
		if (n!=SISTEXT && n!=SISDATA)
			u.u_segl[n].sr_flag |= SRFDUMP;
		if (n!=SIUSERP && n!=SISTEXT && n!=SIPTEXT)
			u.u_segl[n].sr_flag |= SRFDATA;
		u.u_segl[n].sr_size = sp->s_size;
		u.u_segl[n].sr_segp = sp;
	}
	return (mproto());
}

/*
 * Search for a busy text inode.
 */
sbusy(ip)
register INODE *ip;
{
	register SEG *sp;

	lock(seglink);
	/*
	 * Look for the segment in the memory queue.
	 */
	for (sp=segmq.s_forw; sp!=&segmq; sp=sp->s_forw) {
		if (sp->s_ip==ip
		 && (sp->s_flags&(SFSHRX|SFTEXT))==(SFSHRX|SFTEXT)) {
			unlock(seglink);
			return (1);
		}
	}

	/*
	 * Look for the segment on the disk queue.
	 */
	for (sp=segdq.s_forw; sp!=&segdq; sp=sp->s_forw) {
		if (sp->s_ip==ip
		 && (sp->s_flags&(SFSHRX|SFTEXT))==(SFSHRX|SFTEXT)) {
			unlock(seglink);
			return (1);
		}
	}
	unlock(seglink);
	return (0);
}

/*
 * Segment consistency checks for the paranoid.
segchk()
{
	register SEG *sp;
	register int nbad;
	fsize_t s;
	daddr_t d;

	nbad = 0;
	sp = &segmq;
	s = corebot;
	while ((sp=sp->s_forw) != &segmq) {
		if (sp->s_paddr < s)
			nbad += badseg("mem", sp->s_paddr, 0);
		s = sp->s_paddr + sp->s_size;
	}
	if (coretop < s)
		nbad += badseg("mem", sp->s_back->s_paddr, sp->s_back->s_size);
	sp = &segdq;
	d = swapbot;
	while ((sp=sp->s_forw) != &segdq) {
		if (sp->s_daddr < d)
			nbad += badseg("disk", (int)sp->s_daddr, 0);
		d = sp->s_daddr + (sp->s_size / BSIZE);
	}
	if (swaptop < d)
		nbad += badseg("disk", sp->s_back->s_daddr, sp->s_back->s_size);
}

badseg(t, b, s)
char *t;
daddr_t b;
fsize_t s;
{
	printf( "Bad %s segment at %X of len %X\n", t, b, s );
	return (1);
}
*/
