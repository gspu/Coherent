/* $Header: /v4a/coh/RCS/seg.c,v 1.2 92/01/06 12:00:15 hal Exp $ */
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
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <errno.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <a.out.h>


#define	min(a, b)	((a) < (b) ? (a) : (b))

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
}

/*
 * Given an inode, `ip', and flags, `ff', describing a segment associated
 * with the inode, see if the segment already exists and if so, return a
 * copy.  If the segment does not exist, allocate the segment having size
 * `ss', and read the segment using the inode at seek offset `dq' with a
 * size of `ds'.
 */
SEG *
ssalloc(ip, ff, ss)
register INODE *ip;
{
	register SEG *sp;
	register int f;

	lock(seglink);
	f = ff & (SFSHRX|SFTEXT);

	/*
	 * Look for the segment in the memory queue.
	 */
	for (sp=segmq.s_forw; sp!=&segmq; sp=sp->s_forw) {
		if (sp->s_ip==ip && (sp->s_flags&(SFSHRX|SFTEXT))==f) {
			unlock(seglink);
			if ((sp = segdupl(sp)) != NULL)
				segfinm(sp);
			return (sp);
		}
	}

	/*
	 * Look for the segment on the disk queue.
	 */
	for (sp=segdq.s_forw; sp!=&segdq; sp=sp->s_forw) {
		if (sp->s_ip==ip && (sp->s_flags&(SFSHRX|SFTEXT))==f) {
			unlock(seglink);
			if ((sp = segdupl(sp)) != NULL)
				segfinm(sp);
			return (sp);
		}
	}
	unlock(seglink);

	/*
	 * Allocate and create the segment.
	 */
	return salloc(roundu(ss, NBPC), ff);
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
		dmacopy( btoc(sp->s_size), sp->s_vmem, sp1->s_vmem) ;
	}
	return sp1;
}

/*
 * Allocate a segment `bytes_wanted' bytes long.
 * `flags' contains some pseudo flags.
 */
SEG *
salloc(bytes_wanted, flags)
int bytes_wanted, flags;
{
	register SEG *sp;
	register int r;

	r = (flags & (SFSYST|SFTEXT|SFSHRX|SFDOWN)) | SFCORE;

/*
#ifdef _I386
	bytes_wanted += (sizeof(char *) - 1);
	bytes_wanted &= ~(sizeof(char *) - 1);
#else
	bytes_wanted += (BSIZE - 1);
	bytes_wanted &= ~(BSIZE - 1);
#endif
*/

	lock(seglink);
	sp = smalloc(bytes_wanted);
	unlock(seglink);

	if (sp) {
		sp->s_flags = r;
	} else {
#if 0
		/* no room now - let the swapper try to grow it */
		if (flags & SFNSWP)
			return 0;
		if ((sp=kalloc(sizeof(SEG))) == NULL)
			return 0;
		sp->s_forw = sp;
		sp->s_back = sp;
		sp->s_flags = r;
		sp->s_urefc = 1;
		sp->s_lrefc = 1;
		if (segsext(sp, bytes_wanted) == NULL) {
			kfree(sp);
			return 0;
		}
#else
		return 0;
#endif
	}
	if ((flags&SFNCLR) == 0)
		dmaclear(sp->s_size, MAPIO(sp->s_vmem, 0), 0L);
	return sp;
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

	c_free(sp->s_vmem, btoc(sp->s_size));

	unlock(seglink);

	if (sp->s_lrefc != 0)
		panic("Bad segment count");
	if ((ip=sp->s_ip) != NULL)
		ldetach(ip);
	kfree(sp);
}

/*
 * Grow or shrink the segment `sp' so that it has size `new_bytes' bytes.
 * 
 * downward growing segments not done yet!
 */
seggrow(sp, new_bytes)
register SEG *sp;
unsigned int new_bytes;
{
	register SEG *sp1;
	register int dowflag;
	unsigned int	old_bytes, common_clicks;

	dowflag = sp->s_flags & SFDOWN;
	old_bytes = sp->s_size;

	T_HAL(0x20, goto dont_c_grow);
	/*
	 * If we want a larger segment AND c_grow() succeeds
	 *	boost segment size to new_bytes
	 */
	if (new_bytes >= old_bytes && c_grow(sp, new_bytes)==0) {
		T_HAL(0x100, printf("c_grow(%d) ", new_bytes));
		sp->s_size = new_bytes;
		dmaclear(new_bytes - old_bytes,
		  MAPIO(sp->s_vmem, old_bytes), 0);
		return 1;
	}
dont_c_grow:

	if (sp1 = salloc(new_bytes, (sp->s_flags|SFNSWP|SFNCLR))) {
		T_HAL(0x100, printf("salloc(%d) ", new_bytes));
		if (dowflag == 0) {
			common_clicks = btoc(min(new_bytes, old_bytes));
			dmacopy(common_clicks, sp->s_vmem, sp1->s_vmem);
			if (new_bytes > old_bytes) {
				dmaclear(new_bytes - old_bytes,
				  MAPIO(sp1->s_vmem, old_bytes), 0);
			}
		} else
			panic("downflag");
		lock(seglink);
		c_free(sp->s_vmem, btoc(old_bytes));
		satcopy(sp, sp1);
		unlock(seglink);

		return 1;
	}

#if 1
	return 0;
#else
	/*
	 * Last chance.  Extend the segment by swapping it.
	 */
	if (!segsext(sp, new_bytes))
		return 0;

	if (dowflag == 0) {
		if (new_bytes > old_bytes)
			dmaclear(new_bytes - old_bytes, MAPIO(sp->s_vmem,old_bytes), 0);
	} else
		panic("downflag");

	return (1);
#endif
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
	if (s2 == 0 || seggrow(sp, (off_t)s2) == 0) {
		SET_U_ERROR( ENOMEM, "can not grow segment" );
		return;
	}
	if (sproto(0) == 0) {
		if (seggrow(sp, (off_t)s1)==0 || sproto(0)==0) {
			T_PIGGY( 0x2000000, printf("auto SEGV\n"); );
			sendsig(SIGSEGV, SELF);
		}
	}
	segload();
}

/*
 * Grow the segment `sp1' to the size `s' in bytes by swapping it out
 * and back in.  The segment may not be locked.
 */
SEG *
segsext(sp1, s)
register SEG *sp1;
register off_t s;
{
#if 0
	register SEG *sp2;

#if	MONITOR
	if (swmflag)
		printf("Segsext(%p, %u)\n", SELF, SELF->p_pid);
#endif
	if (sexflag == 0) {
		SET_U_ERROR( ENOMEM, "can not extend, swapping is off" );
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
		swapio(1, MAPIO(sp1->s_vmem, 0), sp2->s_daddr, sp1->s_size);
	lock(seglink);
	satcopy(sp1, sp2);
	unlock(seglink);
	sp1->s_flags &= ~SFCORE;
	sp1->s_lrefc--;
	segfinm(sp1);
	return (sp1);
#else
	return 0;
#endif
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
	swapio(1, MAPIO(sp1->s_vmem, 0), sp2->s_daddr, sp1->s_size);
	sp1->s_lrefc--;
	sp2->s_flags = sp1->s_flags & ~SFCORE;
	sp2->s_size  = sp1->s_size;
	return (sp2);
}

/*
 * Given a flag, a physical core address, a disk address and a count in
 * bytes, perform an I/O operation between core and disk.  If `flag' is
 * set, the transfer is to the disk otherwise it is to memory.  As you may
 * have guessed, this is used by the swapper.
 *
 */
swapio(f, p, d, n)
paddr_t p;
daddr_t d;
off_t  n;
{
	register BUF * bp;
	register SEG * sp;
	register int s;
	register int nb;

#if	MONITOR
	if (swmflag > 1)
		printf("swapio(%s,%x,%x,%x)\n",f?"out":"in",(int)p,(int)d,n);
#endif
	if (d < swapbot || d+(n/BSIZE) > swaptop)
		panic("Swapio bad parameter");

	bp = &swapbuf;
	lock(bp->b_gate);
	SELF->p_flags |= PFSWIO;
	bp->b_paddr = p;

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
		while ((bp->b_flag&BFNTP) != 0) {
			v_sleep((char *)bp, CVBLKIO, IVBLKIO, SVBLKIO, "swap");
			/* Sleeping in the swapper.  */
		}
		spl(s);
		if ((bp->b_flag&BFERR) != 0)
			panic("Swapio error");
		bp->b_vaddr += nb;
		p += nb;
		d += nb / BSIZE;
		n -= nb;
	}
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
	sp1->s_back->s_forw = sp1->s_forw;
	sp1->s_forw->s_back = sp1->s_back;
	sp2->s_back->s_forw = sp1;
	sp1->s_back = sp2->s_back;
	sp2->s_forw->s_back = sp1;
	sp1->s_forw  = sp2->s_forw;
	sp1->s_daddr = sp2->s_daddr;
	sp1->s_size = sp2->s_size;
	sp1->s_vmem = sp2->s_vmem;
	kfree(sp2);
}

/*
 * Allocate a segment on disk that is `n' bytes long.
 * The `seglink' gate should be locked before this routine is called.
 */
SEG *
sdalloc( s )
off_t s;
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
 * Allocate a segment in memory that is `bytes_wanted' bytes long.
 * The `seglink' gate should be locked before this routine is called.
 *
 * if successful, return allocated SEG *
 * else, return 0
 */
SEG *
smalloc(bytes_wanted)
off_t bytes_wanted;
{
	register SEG *sp1;
	register SEG *new_seg;
	unsigned	clicks_wanted;

	clicks_wanted = btoc(bytes_wanted);

	/*
	 * Estimate space needed for new segment and its overhead.
	 * Fail if not enough free RAM available.
	 */
	if (countsize(clicks_wanted) > allocno())
		return 0;
	/*
	 * Allocate a new SEG struct to keep track of the segment, if possible.
	 */
	if ((new_seg = kalloc(sizeof (SEG))) == NULL)
		return 0;

	if ((new_seg->s_vmem = c_alloc(clicks_wanted)) == 0) {
		kfree(new_seg);
		return 0;
	}

	/* link new_seg in at start of segmq */
	sp1 = segmq.s_forw;
	sp1->s_back->s_forw = new_seg;
	new_seg->s_back = sp1->s_back;
	sp1->s_back = new_seg;
	new_seg->s_forw = sp1;

	new_seg->s_urefc = 1;
	new_seg->s_lrefc = 1;
	new_seg->s_size  = bytes_wanted;

	return new_seg;
}

/*
 * Set up `SR' structure in user area from segments descriptors in
 * process structure.  Also set up the user segmentation registers.
 */
sproto(xhp)
struct xechdr *xhp;
{
	register int n;
	register SEG *sp;

	for (n=0; n<NUSEG; n++) {
		u.u_segl[n].sr_flag = u.u_segl[n].sr_size = 0;
		u.u_segl[n].sr_segp = 0;
		if ((sp=SELF->p_segp[n]) == NULL)
			continue;
		if (n == SIUSERP)
			u.u_segl[n].sr_base = &u;
		else {
			if (xhp)
				u.u_segl[n].sr_base = xhp->segs[n].mbase;
			u.u_segl[n].sr_flag |= SRFPMAP;
		}
		if (n!=SISTEXT)
			u.u_segl[n].sr_flag |= SRFDUMP;
		if (n!=SIUSERP && n!=SISTEXT)
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
	return 0;
}

/*
 * Segment consistency checks for the paranoid.
segchk()
{
	register SEG *sp;
	register int nbad;
	off_t s;
	daddr_t d;

	nbad = 0;
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
off_t s;
{
	printf( "Bad %s segment at %X of len %X\n", t, b, s );
	return (1);
}
*/
