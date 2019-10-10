/* $Header: /src386/STREAMS/coh.386/RCS/exec.c,v 2.3 93/08/09 13:35:18 bin Exp Locker: bin $ */
/*
 * Processing of the exec () system call.
 *
 * $Log:	exec.c,v $
 * Revision 2.3  93/08/09  13:35:18  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  15:22:36  nigel
 * Nigel's R80
 * 
 * Revision 2.3  93/07/26  14:47:30  nigel
 * Nigel's R80
 * 
 */

#include <common/_gregset.h>
#include <common/_tricks.h>
#include <kernel/sigproc.h>

#include <sys/coherent.h>
#include <sys/acct.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/file.h>
#include <a.out.h>
#include <l.out.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/fd.h>

/*
 * Round section and segment start address to nearest lower click boundary.
 */

static void
xecrnd(xsp)
struct xecseg * xsp;
{
	int diff;

	diff = xsp->fbase & (NBPC - 1);
	xsp->mbase -= diff;
	xsp->fbase -= diff;
	xsp->size += diff;
}

/*
 * Pass control to an image in a file.
 * Make sure the format is acceptable. Release
 * the old segments. Read in the new ones. Some special
 * care is taken so that shared and (more important) shared
 * and separated images can be run on the 8086.
 */

pexece(np, argp, envp, regsetp)
char	      *	np;
char	      *	argp [];
char	      *	envp [];
gregset_t     *	regsetp;
{
	struct xechdr	head;
	register INODE	*ip;			/* Load file INODE */
	register PROC	*pp;			/* A cheap copy of SELF */
 	register SEG    *segp;
	SEG	*ssegp;
	register int	i;			/* For looping over segments*/
	int roundup;
	int shrdsize;
	struct xecnode * xlist = NULL;		/* list head */
	struct xecnode * xp;
	struct xecseg tempseg;
	unsigned int textSize;
	struct direct	dir;

	pp = SELF;
	memset (& head, 0, sizeof (head)); 
	if ((ip = exlopen (& head, np, & shrdsize, & xlist, & dir)) == NULL)
		goto done;

	roundup = shrdsize & 0xf;
	ssegp = exstack (& head, argp, envp,
			 __xmode_286 (regsetp) ? sizeof (short) :
						 sizeof (int));

	if (! ssegp) {
		idetach(ip);
		goto done;
	}

	/* Release shared memory. */
	shmAllDt ();

	/*
	 * At this point the file has been
	 * validated as an object module, and the
	 * argument list has been built. Release all of
	 * the original segments. At this point we have
	 * committed to the new image. A "sys exec" that
	 * gets an I/O error is doomed.
 	 * NOTE: User-area segment is NOT released.
 	 *	 Segment pointer in proc is erased BEFORE invoking sfree().
	 */

	for (i = 1 ; i < NUSEG ; ++ i) {
 		if ((segp = pp->p_segp [i]) != NULL) {
			pp->p_segp [i] = NULL;
			sfree (segp);
		}
	}
	pp->p_segp [SISTACK] = ssegp;

	/*
	 * Read in the loadable segments.
	 */

	switch (head.magic) {
	case XMAGIC (I286MAGIC, I_MAGIC):
		if ((segp = ssalloc (ip, SFTEXT,
				     head.segs [SISTEXT].size)) == NULL)
			goto out; 
		pp->p_segp [SISTEXT] = segp;

		if (! exsread (segp, ip, & head.segs [SISTEXT], 0))
			goto out;

		if ((segp = ssalloc (ip, 0, roundup +
					    head.segs [SIPDATA].size +
					    head.segs [SIBSS].size)) == NULL)
			goto out;
		pp->p_segp [SIPDATA] = segp;

		if (! exsread(segp, ip, & head.segs [SIPDATA], shrdsize)) {
			goto out;
		}
		head.segs [SIPDATA].size += roundup;
		break;

	case XMAGIC (I386MAGIC, Z_MAGIC):
		/*
		 * Round segment address down to nearest click boundary.
		 * Ciaran did this.  I'm not sure why, but will preserve
		 * it for now. -hws-
		 */

		tempseg = head.segs [SISTEXT];	/* save pre-rounding value */
		xecrnd (head.segs + SISTEXT);
		xecrnd (head.segs + SIPDATA);

		/*
		 * Compute text segment size by taking highest address
		 * seen in any text section.
		 */

		textSize = head.segs [SISTEXT].size +
				head.segs [SISTEXT].mbase;
		for (xp = xlist ; xp ; xp = xp->xn) {
			unsigned int tmpSize;
			if (xp->segtype != SISTEXT)
				continue;
			tmpSize = xp->xseg.size + xp->xseg.mbase;
			if (tmpSize > textSize)
				textSize = tmpSize;
		}

		/* Entry point must be within text segment. */
		if (head.entry >= textSize)
			goto out;

		if ((segp = ssalloc (ip, SFTEXT | SFSHRX,
				     textSize)) == NULL)
			goto out;

		pp->p_segp [SISTEXT] = segp;

		if (segp->s_ip == 0) {
			if (! exsread (segp, ip, & tempseg, 0))
				goto out;

			/* load additional text sections, if any */
			for (xp = xlist ; xp ; xp = xp->xn) {
				if (xp->segtype != SISTEXT)
					continue;
				if (! exsread (segp, ip, & xlist->xseg, 0))
					goto out;
			}

			segp->s_ip = ip;
			ip->i_refc ++;
		}
		if ((segp = ssalloc (ip, 0, head.segs [SIPDATA].size +
					    head.segs[SIBSS].size)) == NULL)
			goto out;

		pp->p_segp [SIPDATA] = segp;

		if (segp->s_ip == 0 &&
		    ! exsread (segp, ip, & head.segs [SIPDATA], 0))
			goto out;

		/* Deallocate nodes hooked into xlist by exlopen. */
		while (xlist != NULL) {
			struct xecnode * tmp = xlist->xn;
			kfree (xlist);
			xlist = tmp;
		}
		break;

	default:
		panic("pexece");
	}

	if (sproto (& head) == 0)
		goto out;

	/*
	 * At this point, and no earlier, we can modify the user register
	 * image for the new process because now we are committed to executing
	 * the new image.
	 *
	 * As a general security thing, we begin by zeroing out the user-level
	 * register image.
	 */

	memset (regsetp, 0, sizeof (* regsetp));

	switch (head.magic) {
	case XMAGIC (I286MAGIC, I_MAGIC):
		regsetp->_i286._cs = SEG_286_UII | R_USR;
		regsetp->_i286._ss = regsetp->_i286._es =
			regsetp->_i286._ds = SEG_286_UD  | R_USR;
		regsetp->_i286._ip = head.entry;
		regsetp->_i286._usp = head.initsp;
		break;

	case XMAGIC (I386MAGIC, Z_MAGIC):
		regsetp->_i386._cs = SEG_386_UI | R_USR;
		regsetp->_i386._ss = regsetp->_i386._es =
			regsetp->_i386._ds = SEG_386_UD | R_USR;
		regsetp->_i386._eip = head.entry;
		regsetp->_i386._uesp = head.initsp;
		break;
	}


	/*
	 * The new image is read in
	 * and mapped. Perform the final grunge
	 * (set-uid stuff, accounting, loading up
	 * registers, etc).
	 */

	u.u_flag &= ~AFORK;

	memcpy (u.u_comm, dir.d_name, sizeof (u.u_comm));

	if (iaccess (ip, IPR) == 0) {	/* Can't read ? no dump or trace */
		pp->p_flags |= PFNDMP;
		pp->p_flags &= ~PFTRAC;
	}


	/*
	 * Norm says Frank says we need to drop this for db to work.
	 */
#if 0
	if (iaccess(ip, IPW) == 0) {	/* Can't write ? no trace */
		pp->p_flags &= ~PFTRAC;
printf("Can't write - no trace! ");
u.u_error = 0;
	}
#endif

	if ((ip->i_mode & ISUID) != 0) {	/* Set user id ? no trace */
		pp->p_uid = u.u_uid = u.u_euid = ip->i_uid;
		pp->p_flags &= ~PFTRAC;
	}

	if ((ip->i_mode & ISGID) != 0) {	/* Set group id ? no trace */
		u.u_egid = u.u_gid = ip->i_gid;
		pp->p_flags &= ~PFTRAC;
	}

	for (i = 0 ; i < NOFILE; i ++) {
		int		j = fdgetflags (i);
		if (j != -1 && (j & FD_CLOEXEC) != 0)
			fdclose (i);	/* close fd on exec bit set */
	}


	/*
	 * Default every signal that is not ignored.
	 */

	for (i = 1 ; i <= NSIG ; ++ i) {
		__sigaction_t	act;

		curr_signal_action (i, NULL, & act);
		if (act.sa_handler != SIG_IGN) {
			act.sa_handler = SIG_DFL;
			act.sa_flags = 0;
			__SIGSET_EMPTY (act.sa_mask);
			curr_signal_action (i, & act, NULL);
		}
	}

	if (pp->p_flags & PFTRAC)	/* Being traced */
		sendsig (SIGTRAP, pp);
	idetach (ip);

	/* initialize u area ndp fields */
	ndpNewProc ();

	segload ();
	goto done;


	/*
	 * Alas, exec() has failed..
	 */
out:
	/* Deallocate nodes hooked into xlist by exlopen. */
	while (xlist != NULL) {
		struct xecnode * tmp = xlist->xn;
		kfree (xlist);
		xlist = tmp;
	}

	/* Release the INODE for the load file. */
	idetach (ip);

	/* If we allocated a text segment, let it go. */
	if ((segp = pp->p_segp [SISTEXT]) != NULL) {
		pp->p_segp [SISTEXT] = NULL;
		sfree (segp);
	}

	/* If we allocated a data segment, let it go. */
	if ((segp = pp->p_segp[SIPDATA]) != NULL) {
		pp->p_segp [SIPDATA] = NULL;
		sfree (segp);
	}

	/*
	 * Return through the "sys exit" code with a "SIGSYS", or with the
	 * signal actually received if we are aborting due to interrupted exec.
	 */

	pexit (u.u_error == EINTR ? curr_signal_pending () : SIGSYS);

done:
	return 0;	
}


/*
 * Open a file, make sure it is l.out, coff, or v86 as well as
 * executable.
 *
 * "xhp" points to a cleared xechdr supplied by the caller.
 * "np" is the file name.
 * "shrds" points to an int that will be written by exlopen().
 *   *shrds is set nonzero only for shared l.out.
 *
 * If file is COFF, there may be multiple text (or data?) sections.
 * Use "xlist" linked structure to keep track of variably many sections
 * after the first text and data sections.
 *
 * return NULL if failure, else return inode pointer for the file.
 */

INODE *
exlopen(xhp, np, shrds, xlist, dirent) 
register struct xechdr *xhp;
char *np;
int *shrds;
struct xecnode ** xlist;
struct direct *	dirent;
{
	register INODE *ip;
	int	i, nscn, hdrsize;
	register BUF *bp;
	unsigned short magic;
	struct ldheader head;
	struct filehdr fhead;
	struct aouthdr ahead;	
	struct scnhdr scnhdr;
	IO		io;

	/*
	 * Make sure the file is executable and read the header. Note that
	 * this about the only case of ftoi () with mode 'r' that actually
	 * uses the resulting filename information.
	 */

	if (ftoi (np, 'r', & io, dirent))
		return NULL;

	ip = u.u_cdiri;
	if (iaccess (ip, IPE) == 0) {
		idetach (ip);
		return NULL;
	}

	if ((ip->i_mode & (IPE | (IPE << 3) | (IPE << 6))) == 0 ||
	    (ip->i_mode & IFMT) != IFREG) {
		u.u_error = EACCES;
		idetach (ip);
		return NULL;
	}

	if ((bp = vread (ip, (daddr_t) 0)) == NULL) {
		u.u_error = ENOEXEC;
		idetach (ip);
		return NULL;
	}


	/*
	 * Copy everything we need from the l.out header and check magic
	 * number and machine type.
	 */

	* shrds = 0;
	magic = * (unsigned short *) bp->b_vaddr;
	canint (magic);

	switch (magic) {
	case L_MAGIC:		/* Coherent 286 format */
		memcpy (& head, bp->b_vaddr, sizeof (struct ldheader));

		canint (head.l_machine);
		if (head.l_machine != M_8086)
			goto bad;

		for (i = 0 ; i < NXSEG ; i ++)
			cansize (head.l_ssize [i]);

		canint (head.l_flag);
		canvaddr (head.l_entry);

		/*
		 * If a shared and separated image
	 	 * has stuff in segments that makes it impossible
		 * to share, give an error immediately so that we don't
		 * lose the parent.
		 */
		head.l_flag &= LF_SHR | LF_SEP | LF_KER;

		if ((head.l_flag & LF_SEP) == 0 ||
		    (head.l_flag & LF_KER) != 0 ||
		    head.l_ssize [L_PRVI] || head.l_ssize [L_BSSI])
			goto bad;

		xhp->magic = XMAGIC (I286MAGIC,I_MAGIC);
		xhp->entry = head.l_entry;

		xhp->segs [SISTEXT].fbase = sizeof (struct ldheader);
		xhp->segs [SISTEXT].mbase = NBPS;
		xhp->segs [SISTEXT].size = head.l_ssize [L_SHRI];

		xhp->segs [SIPDATA].fbase = sizeof (struct ldheader) +
						xhp->segs [SISTEXT].size;
		xhp->segs [SIPDATA].mbase = 0;
		xhp->segs [SIPDATA].size = head.l_ssize [L_SHRD] +
						head.l_ssize [L_PRVD];
		if (head.l_flag & LF_SHR)
			* shrds = head.l_ssize [L_SHRD];

		xhp->segs [SIBSS].fbase = 0;
		xhp->segs [SIBSS].mbase = xhp->segs [SIPDATA].size;
		xhp->segs [SIBSS].size = head.l_ssize [L_BSSD];

		xhp->segs [SISTACK].mbase = ISP_286;	/* size 0, fbase 0 */
		brelease (bp);
		return ip;

	case I386MAGIC:		/* ... COFF */
		memcpy (& fhead, bp->b_vaddr, sizeof (struct filehdr));
		hdrsize = sizeof (ahead) + sizeof (fhead);

		if (fhead.f_opthdr != sizeof (ahead) ||
		    (fhead.f_flags & F_EXEC) == 0 ||
		    fhead.f_nscns * sizeof (scnhdr) > BSIZE)
			goto bad;

		memcpy (& ahead, bp->b_vaddr + sizeof (fhead),
			sizeof (ahead));
		if (ahead.magic != Z_MAGIC)
			goto bad;

		xhp->magic = XMAGIC (I386MAGIC, ahead.magic);
		xhp->entry = ahead.entry;

		for (i = 0 ; i < fhead.f_nscns ; i ++) {
			memcpy (& scnhdr,
				bp->b_vaddr + hdrsize + sizeof (scnhdr) * i,
				sizeof (scnhdr));

			switch ((int) (scnhdr.s_flags)) {
			case STYP_INFO:
				continue;

			case STYP_BSS:
				nscn = SIBSS;
				break;

			case STYP_TEXT:
				nscn = SISTEXT;
				break;

			case STYP_DATA:
				nscn = SIPDATA;
				break;

			default:
				goto bad;
			}

			/* Text/data shouldn't collide with stack. */
			if ((unsigned) scnhdr.s_vaddr >= ISP_386)
				goto bad;

			/* Have we already seen a segment of this type? */
			if (xhp->segs [nscn].size) {
				struct xecnode * tmp;

				if (nscn != SISTEXT)
					goto bad;

				/* insert new node at head of "xlist" */
				tmp = (struct xecnode *)
					  kalloc (sizeof (struct xecnode));

				if (tmp == NULL) {
					printf ("can't kalloc(xecnode)\n");
					goto bad;
				}

				tmp->xn = * xlist;
				* xlist = tmp;
				tmp->segtype = nscn;
				tmp->xseg.mbase = scnhdr.s_vaddr;
				tmp->xseg.fbase = scnhdr.s_scnptr;
				tmp->xseg.size = scnhdr.s_size;
			} else {
				xhp->segs [nscn].mbase = scnhdr.s_vaddr;
				xhp->segs [nscn].fbase = scnhdr.s_scnptr;
				xhp->segs [nscn].size = scnhdr.s_size;
			}
		}

		/* Text and data segments must both be nonempty. */
		if (! xhp->segs [SISTEXT].size || ! xhp->segs [SIPDATA].size)
			goto bad;

		xhp->entry = ahead.entry;

		xhp->segs [SISTACK].mbase = ISP_386;	/* size 0, fbase 0 */
		xhp->magic = XMAGIC (I386MAGIC, ahead.magic);
		brelease (bp);	
		return ip;
	default:
	bad:		
		brelease (bp);
		u.u_error = ENOEXEC;
		idetach (ip);
		return NULL;
	}
}

static SEG *
exsread(sp, ip, xsp, shrdSz)
register SEG *sp;
INODE *ip;
struct xecseg *xsp;
int shrdSz;
{
	register int sa, so;
	IO		io;

	sa = xsp->fbase;
	so = xsp->mbase & (NBPC - 1);

	io.io_seg = IOPHY;
	io.io_seek = sa;
	io.io.pbase = MAPIO (sp->s_vmem, so);
	io.io_flag = 0;

	if (shrdSz) {	/* shared l.out? */

		/* Load SHRD. */
		io.io_ioc = shrdSz;
		sp->s_lrefc ++;
		iread (ip, & io);
		sp->s_lrefc --;

		if ((io.io_ioc = xsp->size - shrdSz) != 0) {

			/* Advance file and RAM offsets past SHRD. */
			sa += shrdSz;
			so += shrdSz;

			/* Advance RAM offset to next 16-byte boundary. */
			so = (so + 15) & ~ 15; /* round up */

			/* Load PRVD. */
			io.io_seg = IOPHY;
			io.io_seek = sa;
			io.io.pbase = MAPIO (sp->s_vmem, so);
			io.io_flag = 0;

			sp->s_lrefc ++;
			iread (ip, & io);
			sp->s_lrefc --;
		}
	} else {	/* NOT shared l.out */
		io.io_ioc = xsp->size;

		sp->s_lrefc ++;
		iread (ip, & io);
		sp->s_lrefc --;
	}

	/*
	 * NIGEL: This perturbs me. This check seems to really belong
	 * somewhere at the top-level, and/or from testing the return values
	 * from the read calls. Why isn't the residual from the read tested?
	 */

	if (curr_signal_pending ())
		u.u_error = EINTR;

	if (u.u_error == 0)
		return sp;
	return NULL;
}


struct adata {		/* Storage for arg and env data */
	int	np;		/* Number of pointers in vector */
	int	nc;		/* Number of characters in strings */
};


/*
 * Given a pointer to a list of arguments and a pointer to a list of
 * environments, return a stack with the arguments and environments on it.
 */

SEG *
exstack(xhp, argp, envp, wdin)
register struct xechdr *xhp;
caddr_t	argp, envp;
{
	register SEG *sp;		/* Stack segment pointer */
	struct sdata {		/* To keep segment pointers */
		caddr_t	vp;		/* Argv[i], envp[i] pointer */
		caddr_t	cp;		/* Argv[i][j], envp[i][j] pointer */
	} stk;
	struct adata arg, env;
	int	chrsz, vecsz, stksz, wdmask, wdout, stkoff, stktop;
	int	stkenvp;
	register int i;

	/* Validate and evaluate size of args and envs */
	if (! excount (argp, & arg, wdin) || ! excount (envp, & env, wdin))
		return NULL;

	/* Calculate stack size and allocate it */
	chrsz = __ROUND_UP_TO_MULTIPLE (arg.nc + env.nc, sizeof (int));
	vecsz = (arg.np + 1 + env.np + 1) * sizeof (long);
	stksz = __ROUND_UP_TO_MULTIPLE (vecsz + chrsz + ISTSIZE, NBPC);

	if (stksz > MADSIZE || (sp = salloc (stksz, SFDOWN)) == NULL) {
		u.u_error = E2BIG;
		return NULL;
	}

	/* Set up target stack */
	stktop = xhp->segs [SISTACK].mbase;
	stk.cp = stktop - chrsz;
	stk.vp = stktop - chrsz - vecsz;
	stkoff = MAPIO (sp->s_vmem, stksz - stktop);
	u.u_argc = arg.np;
	u.u_argp = stk.vp;
	wdmask = -1;
	if (wdin == sizeof (short))
		wdmask = (unsigned short) wdmask;

	switch (stktop) {

	case ISP_386:
		wdout = sizeof (long);
		xhp->initsp = stk.vp - sizeof (long);
		dmaout (sizeof (long), xhp->initsp + stkoff, & arg.np);
		break;

	case ISP_286:
		wdout = sizeof (short);
		xhp->initsp = stk.vp - 3 * sizeof (short);
		stkenvp = stk.vp + (arg.np + 1) * sizeof (short);
		dmaout (sizeof (short), xhp->initsp + stkoff, & arg.np);
		dmaout (sizeof (short), xhp->initsp + sizeof (short) + stkoff,
			& stk.vp);
		dmaout (sizeof (short), xhp->initsp + 2 * sizeof (short) +
					stkoff,	& stkenvp);
		break;

	default:
		panic("exstack");

	}

	/* Arguments */
	for (i = 0 ; i < arg.np ; i ++, argp += wdin, stk.vp += wdout) {
		dmaout (wdout, stk.vp + stkoff, & stk.cp);
		stk.cp += exarg (stk.cp + stkoff, getupd (argp) & wdmask);
	}

	/* skip null word after arguments */
	stk.vp += wdout;

	/* Environments */
	for (i = 0; i < env.np ; i ++, envp += wdin, stk.vp += wdout) {
		dmaout (wdout, stk.vp + stkoff, & stk.cp);
		stk.cp += exarg (stk.cp + stkoff, getupd (envp) & wdmask);
	}

	return sp;
}

exarg(out, in)
caddr_t	in, out;
{
	char 	c;
	caddr_t	init_in;

	init_in = in;
	do {
		c = getubd (in ++);
		dmaout (sizeof (char), out ++, & c);
	} while (c);
	return in - init_in;
}

/*
 * Given a pointer to a list of arguments, a pointer to an argument count
 * and a pointer to a byte count, count the #characters/#strings
 * in the arguments
 */
excount(usrvp, adp, wdin)
register caddr_t usrvp;
struct adata *adp;
{
	register caddr_t	usrcp;
	register int c;
	register unsigned nb;
	register unsigned na;
	int	wdmask;

	wdmask = -1;
	if (wdin == sizeof (short))
		wdmask = (unsigned short) wdmask;
	na = nb = 0;
	if (usrvp != NULL) {
		for (;;) {
			usrcp = getupd (usrvp) & wdmask;
			usrvp += wdin;
			if (u.u_error)
				return 0;
			if (usrcp == NULL)
				break;
			na ++;
			for (;;) {
				c = getubd (usrcp ++);
				if (u.u_error)
					return 0;
				nb ++;
				if (c == '\0')
					break;
			}
		}
	}
	adp->np = na;
	adp->nc = nb;
	return 1;
}

/*
 * Round up a size to a paragraph
 * (mod 16) boundry.
 * This is really mod 512 to make swapping work
 */
off_t
exround(s)
off_t	s;
{
	return (s + 15) & ~ 0x0F;
}

pload(np)
char * np;
{
	return -1;

}
/*
 * Set up the first process, a small program which will exec
 * the init program.
 */
extern char aicodep[];

eveinit()
{
	SEG *sp;
	register PROC *pp;
	SELF = pp = eprocp;
/*	static struct xechdr xecinit[NUSEG+1] = { {0},{0},{0},{ISP_386}}; */ 

	/*
	 * Allocate, record, initialize code segment, make it executable.
	 */
	if ((sp = salloc (__ROUND_UP_TO_MULTIPLE (icodes, NBPC), 0)) == NULL)
		panic ("eveinit(code)");
	pp->p_segp [SIPDATA] = sp;

	/*
	 * Start process.
	 */
	u.u_argp = 0;
	if (sproto (0) == 0)
		panic ("eveinit()");
	segload ();
	setspace (SEG_386_UD | R_USR);
	kucopy (aicodep, 0, icodes);
}

/*
 * Given a major number, undo the previous function.
 */
puload(m)
int m;
{
	register CON *cp;
	register DRV *dp;

	dp = & drvl [m];

	if (m >= drvn || (cp = dp->d_conp) == NULL) {
		u.u_error = ENXIO;
		goto ret;
	}
	(* cp->c_uload) ();
	if (! u.u_error)
		dp->d_conp = NULL;
ret:
	return 0;
}

