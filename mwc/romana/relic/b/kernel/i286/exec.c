/* $Header: /kernel/kersrc/i286/RCS/exec.c,v 1.1 92/07/17 15:21:27 bin Exp Locker: bin $ */
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
 * This file contains a special version
 * of "sys exec" for the i8086. This version has
 * no driver load code in it (save space) and has
 * special load code so that the text of a shared
 * and separated image can be shared.
 * Loadable kernel processes are partially supported:
 * the process text and data must be ld'ed with the system
 * and the l.out executed must have no loadable or allocateable
 * segments.
 *
 * $Log:	exec.c,v $
 * Revision 1.1  92/07/17  15:21:27  bin
 * Initial revision
 * 
 * Revision 1.1	88/03/24  17:39:26	src
 * Initial revision
 * 
 * 88/01/21	Allan Cornish		/usr/src/sys/i8086/src/exec.c
 * Segments are now de-associated from processes before freeing the segment.
 *
 * 87/12/03	Allan Cornish		/usr/src/sys/i8086/src/exec.c
 * ld_start() now reverts to kernel mode [depth=0] from user mode [depth=1].
 *
 * 87/11/25	Allan Cornish		/usr/src/sys/i8086/src/exec.c
 * vaddr_t bp->b_vaddr --> faddr_t bp->b_faddr.
 *
 * 87/11/14	Allan Cornish		/usr/src/sys/i8086/src/exec.c
 * Init code+data now split into icodep/icodes and idatap/idatas.
 *
 * 87/11/05	Allan Cornish		/usr/src/sys/i8086/src/exec.c
 * New seg struct now used to allow extended addressing.
 *
 * 87/10/09	Allan Cornish		/usr/src/sys/i8086/src/exec.c
 * pload() now handles new format [separate code] loadable device drivers.
 *
 * 87/10/08	Allan Cornish		/usr/src/sys/i8086/src/exec.c
 * Exsread() initializes the (IO).io_flag field to 0.
 */
#include <sys/coherent.h>
#include <acct.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <l.out.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/i8086.h>

/*
 * Sizes.
 */
#define	sh	((fsize_t)sizeof(struct ldheader))
#define si	lssize[L_SHRI]
#define pi	lssize[L_PRVI]
#define bi	lssize[L_BSSI]
#define sd	lssize[L_SHRD]
#define pd	lssize[L_PRVD]
#define bd	lssize[L_BSSD]

/*
 * Segments.
 */
#define upsp	pp->p_segp[SIUSERP]
#define sssp	pp->p_segp[SISTACK]
#define	sisp	pp->p_segp[SISTEXT]
#define pisp	pp->p_segp[SIPTEXT]
#define pdsp	pp->p_segp[SIPDATA]

/*
 * Loadable driver initiation point.
 */
static
ld_start()
{
	register SEG * sp;
	register int ret;

	/*
	 * Kernel processes start by default at user level.
	 * Revert to kernel level.
	 */
	if ( depth == 1 )
		depth--;

	/*
	 * Initialize memory references.
	 */
	u.u_btime = timer.t_time;
	sproto();
	segload();


	/*
	 * Invoke the driver if it has a shared or private code segment.
	 */
	ret = 100;
	if ( (sp = SELF->p_segp[SISTEXT]) || (sp = SELF->p_segp[SIPTEXT]) ) {
		ret = ld_xcall( sp->s_faddr );
	}

	uexit( ret );
}

/*
 * Set up the first process, a small programme which will exec
 * the init programme.
 */
eveinit(sp)
SEG *sp;
{
	register PROC *pp;
	SELF = pp = eprocp;

	/*
	 * Record user area.
	 */
	pp->p_segp[SIUSERP] = sp;

	/*
	 * Allocate, record, initialize code segment, make it executable.
	 */
	if ((sp=salloc((fsize_t)icodes, 0)) == NULL)
		panic("eveinit(code)");
	pp->p_segp[SIPTEXT] = sp;
	kfcopy( icodep, sp->s_faddr, icodes );
	sp->s_flags |= SFTEXT;
	vremap(sp);

	/*
	 * Allocate, record, and initialize data segment.
	 */
	if ((sp=salloc((fsize_t)idatas, 0)) == NULL)
		panic("eveinit(data)");
	pp->p_segp[SIPDATA] = sp;
	kfcopy( idatap, sp->s_faddr, idatas );

	/*
	 * Allocate and record stack segment.
	 */
	if ((sp=salloc((fsize_t)UPASIZE, SFDOWN)) == NULL)
		panic("eveinit()");
	pp->p_segp[SISTACK] = sp;

	/*
	 * Start process.
	 */
	u.u_argp = 0;
	if (sproto() == 0)
		panic("eveinit()");
	segload();
}

/*
 * Load a driver which has already been linked into the system image.
 */
pload( np )
char * np;
{
	register INODE * ip;
	register PROC  * cpp;
	struct seg     * sp;
	fsize_t		lssize[NUSEG];		/* Segment sizes */
	int		lflag;			/* l_flags from l.out */
	vaddr_t		pc;			/* l_entry from l.out */
	int		r;			/* Flag for "exload" */
	int		s;
	extern char	end[];


	if (super() == 0) {
		return( -1 );
	}

	/*
	 * Coalesce memory BEFORE loading driver, since it can't be moved.
	 */
	krunch(10000);

	if ((ip=exlopen(np, lssize, &lflag, &pc)) == NULL) {
		return( -1 );
	}

	/*
	 * New format loadable drivers must have separate code/data.
	 * It must have executable code, but no initialized data.
	 * Uninitialized data must match the kernel data size.
	 */
	if ( ((lflag & (LF_KER|LF_SHR|LF_SEP)) != (LF_SEP|LF_KER))
	  || (si==0) || (sd!=0) || (pd!=0) || (bd != (int)end) ) {
		u.u_error = EBADFMT;
		idetach(ip);
		return( -1 );
	}

	/*
	 * Allocate and initialize driver code segment.
	 * NOTE: Must be system segment to prevent relocation.
	 */
	sp = ssalloc(&r, ip, SFTEXT|SFHIGH|SFNSWP|SFSYST, si+pi+bi, sh, si+pi);

	/*
	 * Release driver object file.
	 */
	idetach(ip);

	if ( r < 0 ) {
		u.u_error = ENOMEM;
		return( -1 );
	}

	/*
	 * Spawn kernel process to service driver.
	 */
	if ((cpp = process(ld_start)) == NULL ) {
		u.u_error = ENOMEM;
		sfree(sp);
		return( -1 );
	}

	/*
	 * Record the basename of the loaded driver.
	 */
	kscopy( u.u_direct.d_name, cpp->p_segp[SIUSERP],
		offset(uproc,u_comm[0]), sizeof(u.u_comm) );

	/*
	 * Record the driver code segment in the process's private code.
	 */
	cpp->p_segp[SIPTEXT] = sp;
	cpp->p_cval = CVCHILD;
	cpp->p_sval = SVCHILD;
	cpp->p_rval = RVCHILD;
	cpp->p_ppid = 1;

	/*
	 * Make the process executable.
	 */
	s = sphi();
	setrun( cpp );
	spl( s );

	/*
	 * Return driver process id.
	 */
	return( cpp->p_pid );
}

/*
 * Given a major number, undo the previous function.
 */
puload(m)
int m;
{
	register CON *cp;
	register DRV *dp;

	dp = &drvl[m];
	lock(dp->d_gate);
	if (m>=drvn || (cp=dp->d_conp)==NULL) {
		u.u_error = ENXIO;
		goto ret;
	}
	(*cp->c_uload)();
	if ( ! u.u_error)
		dp->d_conp = NULL;
ret:
	unlock(dp->d_gate);
	return (0);
}

/*
 * Pass control to an image in a file.
 * Make sure the format is acceptable. Release
 * the old segments. Read in the new ones. Some special
 * care is taken so that shared and (more important) shared
 * and separated images can be run on the 8086.
 */
pexece(np, argp, envp)
char	*np;
char	*argp[];
char	*envp[];
{
	register INODE	*ip;			/* Load file INODE */
	register PROC	*pp;			/* A cheap copy of SELF */
	register SEG	*ssp;			/* New stack segment */
 	register SEG    *segp;
	register fsize_t	ss;			/* Segment size temp. */
	register int	i;			/* For looping over segments */
	int		r;			/* Flag for "exload" */
	int		lflag;			/* l_flags from l.out */
	vaddr_t		pc;			/* l_entry from l.out */
	vaddr_t		sp;			/* Initial stack pointer */
	fsize_t		lssize[NUSEG];		/* Segment sizes */
	fsize_t		codsize;		/* Total if CS segment	*/
	fsize_t		datsize;		/* Total of DS segment	*/
	extern fsize_t	exround();		/* Paragraph rounder */

	pp = SELF;
	if ((ip=exlopen(np, lssize, &lflag, &pc)) == NULL) {
		return;
	}

	if ( (lflag & LF_SEP) == 0 ) {
		u.u_error = EBADFMT;
		idetach(ip);
		return;
	}

	/*
	 * Kernel processes are now supported through the sload() system call.
	 * 87/10/09	Allan Cornish.
	 */
	if ((lflag&LF_KER) != 0) {
		u.u_error = EBADFMT;
		idetach(ip);
		return;
	}

	/*
	 * If a shared and separated image
 	 * has stuff in segments that makes it impossible
	 * to share, give an error immediately so that we don't
	 * lose the parent.
	 */
	lflag &= LF_SHR|LF_SEP;

	if (lflag==(LF_SHR|LF_SEP) && (pi!=0 || bi!=0)) {
		u.u_error = EBADFMT;
		idetach(ip);
		return;
	}

	if ((ssp=exstack(&sp, argp, envp)) == NULL) {
		idetach(ip);
		return;
	}

	switch (lflag) {
	case LF_SEP:
		codsize = si+pi+bi;
		datsize = ssp->s_size+sd+pd+bd;
		break;
	case LF_SHR|LF_SEP:
		codsize = si;
		datsize = ssp->s_size+exround(sd)+pd+bd;
		break;
	}
	codsize = (codsize+(BSIZE-1)) & ~(BSIZE-1);
	datsize = (datsize+(BSIZE-1)) & ~(BSIZE-1);
	if ( (codsize >= MAXU) || (datsize >= MAXU) ) {
		u.u_error = E2BIG;
		idetach(ip);
		return;
	}

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
	for ( i = 1; i < NUSEG; ++i ) {
 		if ((segp = pp->p_segp[i]) != NULL) {
			pp->p_segp[i] = NULL;
			sfree(segp);
		}
	}

	/*
	 * Read in the loadable segments.
	 */
	sssp = ssp;
	switch (lflag) {
	case 0:
		ss = si+pi+sd+pd;
		pdsp = ssalloc(&r, ip, 0, ss+bi+bd, sh, ss);
		if (r < 0)
			goto out;
		break;

	case LF_SHR:
		ss = exround(si+sd);
		pdsp = ssalloc(&r, ip, 0, ss+pi+pd+bi+bd, sh, si);
		if (r < 0)
			goto out;
		if (exsread(pdsp, ip, sd, sh+si+pi, si) == NULL)
			goto out;
		if (exsread(pdsp, ip, pi, sh+si, ss) == NULL)
			goto out;
		if (exsread(pdsp, ip, pd, sh+si+pi+sd, ss+pi) == NULL)
			goto out;
		break;

	case LF_SEP:
		pisp = ssalloc(&r, ip, SFTEXT, si+pi+bi, sh, si+pi);
		if (r < 0)
			goto out;
		pdsp = ssalloc(&r, ip, 0, sd+pd+bd, sh+si+pi, sd+pd);
		if (r < 0)
			goto out;
		break;

	case LF_SHR|LF_SEP:
		/* pi=0, bi=0 */
		sisp = ssalloc(&r, ip, SFSHRX|SFTEXT, si, sh, si);
		if (r < 0)
			goto out;
		ss = exround(sd);
		pdsp = ssalloc(&r, ip, 0, ss+pd+bd, sh+si, sd);
		if (r<0 || exsread(pdsp, ip, pd, sh+si+sd, ss) == NULL)
			goto out;
	}
	if (sproto() == 0)
		 goto out;
#if 0
	if ( (datsize != pdsp->s_size) ||
	     ( (lflag==LF_SEP) && (codsize != pisp->s_size) ) ||
	     ( (lflag==(LF_SEP|LF_SHR)) && (codsize != sisp->s_size) ) ) {
		printf("\nExec ERROR:  codsize: 0x%X  datsize: 0x%X\n", 
					codsize, datsize);
		printf(
		"pdsp->s_size: 0x%X  pisp->s_size: 0x%X  sisp->s_size: 0x%X\n",
			pdsp->s_size, pisp->s_size, sisp->s_size);
	}
#endif
	/*
	 * The new image is read in
	 * and mapped. Perform the final grunge
	 * (set-uid stuff, accounting, loading up
	 * registers, etc).
	 */
	u.u_flag &= ~AFORK;
	kkcopy(u.u_direct.d_name, u.u_comm, sizeof(u.u_comm));
	if (iaccess(ip, IPR) == 0) {	/* Can't read ? no dump or trace */
		pp->p_flags |= PFNDMP;
		pp->p_flags &= ~PFTRAC;
	}
	if (iaccess(ip, IPW) == 0)	/* Can't write ? no trace */
		pp->p_flags &= ~PFTRAC;
	if ((ip->i_mode&ISUID) != 0) {	/* Set user id ? no trace */
		pp->p_uid = u.u_uid = ip->i_uid;
		pp->p_flags &= ~PFTRAC;
	}
	if ((ip->i_mode&ISGID) != 0) {	/* Set group id ? no trace */
		u.u_gid = ip->i_gid;
		pp->p_flags &= ~PFTRAC;
	}
	for (i=0; i<NSIG; ++i)
		if (u.u_sfunc[i] != SIG_IGN)
			u.u_sfunc[i] = SIG_DFL;
	if ((pp->p_flags&PFTRAC) != 0)	/* Being traced */
		sendsig(SIGTRAP, pp);
	idetach(ip);
	msetusr(pc, sp);
	segload();
	return (0);

	/*
	 * We did not make it.
	 * Release the INODE for the load
	 * file, and return through the "sys exit"
	 * code with a "SIGSYS", or with the signal actually received
	 * if we are aborting due to interrupted exec.
	 */
out:
	idetach(ip);
	if (u.u_error == EINTR)
		pexit(nondsig());
	pexit(SIGSYS);
}

/*
 * Open an l.out, make sure it is an l.out and executable and return the
 * appropriate information.
 */
INODE *
exlopen(np, ssizep, flagp, pcp)
char *np;
fsize_t *ssizep;
int *flagp;
vaddr_t *pcp;
{
	register INODE *ip;
	register struct ldheader *ldp;
	register int n;
	register BUF *bp;
	int m;

	/*
	 * Make sure the file is really an executable l.out and read the
	 * header in.
	 */
	if (ftoi(np, 'r') != 0)
		return (NULL);
	ip = u.u_cdiri;
	if (iaccess(ip, IPE) == 0) {
		idetach(ip);
		return (NULL);
	}
	if ((ip->i_mode&(IPE|IPE<<3|IPE<<6))==0 || (ip->i_mode&IFMT)!=IFREG) {
		u.u_error = EACCES;
		idetach(ip);
		return (NULL);
	}
	if ((bp=vread(ip, (daddr_t)0)) == NULL) {
		u.u_error = EBADFMT;
		idetach(ip);
		return (NULL);
	}

	/*
	 * Copy everything we need from the l.out header and check magic
	 * number and machine type.
	 */
	ldp = FP_OFF(bp->b_faddr);
	m = ldp->l_magic;
	canint(m);
	if (m != L_MAGIC) {
		u.u_error = ENOEXEC;
		brelease(bp);
		idetach(ip);
		return (NULL);
	}
	m = ldp->l_machine;
	canint(m);
	if (m != mactype) {
		u.u_error = EBADFMT;
		brelease(bp);
		idetach(ip);
		return (NULL);
	}
	kkcopy(ldp->l_ssize, ssizep, NXSEG*sizeof(fsize_t));
	for (n=0; n<NXSEG; n++)
		cansize(ssizep[n]);
	*flagp = ldp->l_flag;
	canint(*flagp);
	*pcp = ldp->l_entry;
	canvaddr(*pcp);
	brelease(bp);
	return (ip);
}

/*
 * Given a segment `sp', read `ss' bytes from the inode `ip' starting
 * at seek address `sa' into offset `so' in the segment.
 */
SEG *
exsread(sp, ip, ss, sa, so)
register SEG *sp;
INODE *ip;
fsize_t ss;
fsize_t sa;
fsize_t so;
{
	while (ss > 0) {
		u.u_io.io_seg = IOPHY;
		u.u_io.io_seek = sa;
		u.u_io.io_phys = sp->s_paddr + so;
		u.u_io.io_flag = 0;
		if (ss >= 4096) {
			u.u_io.io_ioc = 4096;
			ss -= 4096;
		} else {
			u.u_io.io_ioc = ss;
			ss = 0;
		}
		sp->s_lrefc++;
		iread(ip, &u.u_io);
		sp->s_lrefc--;
		if (nondsig()) {
			u.u_error = EINTR;
			break;
		}
		sa += 4096;
		so += 4096;
	}
	if (u.u_error == 0)
		return (sp);
	return (NULL);
}

/*
 * Given a pointer to a list of arguments and a pointer to a list of
 * environments, return a stack with the arguments and environments on it.
 */
SEG *
exstack(iusp, argp, envp)
char **iusp;		/* Back patch sp value */
char *argp[];		/* Arguments for new process */
char *envp[];		/* Environments for new process */
{
	SEG *sp;		/* Stack segment pointer */
	struct adata {		/* Storage for arg and env data */
		char	**up;		/* User vector pointer */
		int	np;		/* Number of pointers in vector */
		int	nc;		/* Number of characters in strings */
	} arg, env;
	struct sdata {		/* To keep segment pointers */
		vaddr_t	base;		/* Top of segment virtual */
		vaddr_t	ap;		/* Argc, argv, envp pointer */
		vaddr_t	vp;		/* Argv[i], envp[i] pointer */
		vaddr_t	cp;		/* Argv[i][j], envp[i][j] pointer */
	} aux, stk;
	aold_t aold;			/* Auxiliary map storage */
	register char **usrvp;		/* Vector pointer into user seg */
	register char *usrcp;		/* Character pointer into user seg */
	register int c;			/* Character fetched from user */
	register int chrsz;		/* Size of strings */
	register struct adata *adp;	/* Arg and env scanner */
	register int vecsz;		/* Size of vectors */
	register int stksz;		/* Size of stack argument region */

	/* Validate and evaluate size of args and envs */
	arg.up = argp;
	env.up = envp;
	chrsz = 0;
	vecsz = 0;
	for (adp = &arg; ; adp = &env) {
		adp->np = 0;
		adp->nc = 0;
		if (excount(adp->up, &adp->np, &adp->nc) == 0)
			return (NULL);
		chrsz += adp->nc * sizeof(char);
		vecsz += adp->np * sizeof(char *);
		if (adp == &env)
			break;
	}

	/* Calculate stack size and allocate it */
	chrsz = roundu(chrsz, sizeof(int));
	stksz = sizeof(int)		/* argc */
		+ sizeof(char **)	/* argv */
		+ sizeof(char **)	/* envp */
		+ vecsz			/* argv[i] and envp[i] */
		+ chrsz			/* *argv[i] and *envp[i] */
		+ sizeof(int)		/* Mystery zero word */
		+ sizeof(char *)	/* Splimit for z8000 */
		+ sizeof(int);		/* errno */
	stksz += ISTSIZE;
	if (stksz > MADSIZE) {
		u.u_error = E2BIG;
		return (NULL);
	}
	if ((sp=salloc((fsize_t)stksz, SFDOWN)) == NULL)
		return (NULL);
	stksz -= ISTSIZE;

	/*
	 * Initialize segment data.
	 */
	asave(aold);

	abase(FP_SEL(sp->s_faddr));
	aux.base = sp->s_size;
	aux.ap = aux.base - stksz;
	aux.vp = aux.ap + sizeof(int) + 2*sizeof(char **);
	aux.cp = aux.vp + vecsz;

	stk.base = ISTVIRT;
	stk.ap = stk.base - stksz;
	stk.vp = stk.ap + sizeof(int) + 2*sizeof(char **);
	stk.cp = stk.vp + vecsz;

	/*
	 * Write argc.
	 */
	aputi((int *)aux.ap, arg.np-1);
	aux.ap += sizeof(int);

	/*
	 * Arguments and environments.
	 */
	for (adp = &arg; ; adp = &env) {

		/* Write argv or envp */
		aputp((char ***)aux.ap, (char **)stk.vp);
		aux.ap += sizeof(char **);
		if ((usrvp = adp->up) != NULL) {

			/* Write argv[i] or envp[i] */
			while ((usrcp = getupd(usrvp++)) != NULL) {
				aputp((char **)aux.vp, (char *)stk.cp);
				aux.vp += sizeof(char *);
				stk.vp += sizeof(char *);

				/* Write argv[i][j] or envp[i][j] */
				do {
					c = getubd(usrcp++);
					aputc((char *)aux.cp, c);
					aux.cp += sizeof(char);
					stk.cp += sizeof(char);
				} while (c != '\0');
			}
		}

		/* Write argv[argc] or envp[envc] */
		aputp((char **)aux.vp, NULL);
		aux.vp += sizeof(char *);
		stk.vp += sizeof(char *);
		if (adp == &env)
			break;
	}

	/*
	 * Clear out the slop.
	 */
	aux.base -= sizeof(int);
	aputi((int *) aux.base, 0);		/* errno */
	aux.base -= sizeof(char *);
	aputp((char **) aux.base, (char *)stk.base - sp->s_size + SOVSIZE);
	aux.base -= sizeof(int);
	aputi((int *) aux.base, 0);		/* mystery word */

	arest(aold);

	/*
	 * Patch some values and return.
	 */
	*iusp = stk.ap;		/* Patch initial usp */
	u.u_argc = arg.np-1;
	u.u_argp = stk.vp;	/* Points after NULL of envs */
	return (sp);
}

/*
 * Given a pointer to a list of arguments, a pointer to an argument count
 * and a pointer to a byte count, update incrementally the argument count
 * and the byte count.
 */
excount(usrvp, nap, nbp)
register char **usrvp;
int *nap;
int *nbp;
{
	register char *usrcp;
	register int c;
	register unsigned nb;
	register unsigned na;

	na = 1;
	nb = 0;
	if (usrvp != NULL) {
		for (;;) {
			usrcp = getupd(usrvp++);
			if (u.u_error)
				return (0);
			if (usrcp == NULL)
				break;
			na++;
			for (;;) {
				c = getubd(usrcp++);
				if (u.u_error)
					return (0);
				nb++;
				if (c == '\0')
					break;
			}
		}
	}
	*nap += na;
	*nbp += nb;
	return (1);
}

/*
 * Round up a size to a paragraph
 * (mod 16) boundry.
 * This is really mod 512 to make swapping work
 */
fsize_t
exround(s)
fsize_t	s;
{
	return ((s+15)&~0x0F);
}
