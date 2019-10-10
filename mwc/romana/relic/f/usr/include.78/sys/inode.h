/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/inode.h
 * Memory resident inodes.
 */

#ifndef	__SYS_INODE_H__
#define	__SYS_INODE_H__

#include <sys/types.h>
#include <sys/ksynch.h>
#include <sys/_time.h>
#include <poll.h>


/*
 * Size of pipe buffer.
 */
#define	PIPSIZE	(ND*BSIZE)

/*
 * File inode.
 */
typedef struct inode {
	dev_t	i_dev;			/* Device */
	ino_t	i_ino;			/* Inode index */
	int	i_refc;			/* Reference count */
	unsigned i_lrt;			/* Last reference time */
	GATE	i_gate;			/* Gate */
	int	i_flag;			/* Flags */
	short	i_mode;			/* Mode and type */
	short	i_nlink;		/* Number of links */
	short	i_uid;			/* Owner's user id */
	short	i_gid;			/* Owner's group id */
	fsize_t	i_size;			/* Size of file in bytes */
	union ia_u {
		daddr_t	i_addr[13];	/* Disk addresses */
		dev_t	i_rdev;		/* Real device */
		struct ip_s {		/* Pipes */
			daddr_t ip_pipe[10];
			short	ip_pnc;	/* Number Characters */
			short	ip_prx;	/* Reader Offset */
			short	ip_pwx;	/* Writer Offset */
			short	ip_par; /* Number Awake Readers */
			short	ip_paw; /* Number Awake Writers */
			short	ip_psr; /* Number Sleeping Readers */
			short	ip_psw; /* Number Sleeping Writers */
			event_t ip_iev;	/* Polling Input Event */
			event_t ip_oev;	/* Polling Output Event */
		}	i_p;
	}	i_a;			/* Addresses */
	time_t	i_atime;		/* Last access time */
	time_t	i_mtime;		/* Last modify time */
	time_t	i_ctime;		/* Creation time */
#ifdef _I386
	struct	rlock	*i_rl;		/* List of record locks */
#endif
} INODE;

/*
 * Compatibility.
 */
#define i_pipe	i_a.i_p.ip_pipe
#define i_pnc	i_a.i_p.ip_pnc
#define i_prx	i_a.i_p.ip_prx
#define i_pwx	i_a.i_p.ip_pwx
#define i_par	i_a.i_p.ip_par
#define i_paw	i_a.i_p.ip_paw
#define i_psr	i_a.i_p.ip_psr
#define i_psw	i_a.i_p.ip_psw
#define i_iev	i_a.i_p.ip_iev
#define i_oev	i_a.i_p.ip_oev

/*
 * Flags.
 */
#define	IFACC	0x1			/* File has been accessed */
#define	IFMOD	0x2			/* File has been modified */
#define	IFCRT	0x4			/* File has been created */
#define IFMNT	0x8			/* Contains mounted file system */
#ifdef _I386
#define	IFEXCL	0x80			/* Exclusive open */
#endif /* _I386 */

/*
 * Permission bits.
 */
#define IPE	0x01			/* Execute */
#define IPW	0x02			/* Write */
#define IPR	0x04			/* Read */

/*
 * Modifier bits for fdopen().
 */
#define	IPNDLY	 0x08
#define	IPAPPEND 0x10
#ifdef _I386
#define IPSYNC	 0x20
#define IPEXCL	 0x40
#define IPNOCTTY 0x80
#endif /* _I386 */

/*
 * Bit for dup system call.
 */
#define DUP2	0x40

#ifdef KERNEL
/*
 * Macro functions.
 */
#define ilock(ip)	lock(ip->i_gate)
#define iunlock(ip)	unlock(ip->i_gate)
#define ilocked(ip)	locked(ip->i_gate)

/*
 * Functions to set modify time.
 */
#define iacc(ip) {							\
	ip->i_flag |= IFACC;						\
	ip->i_atime = timer.t_time;					\
}

#define imod(ip) {							\
	ip->i_flag |= IFMOD;						\
	ip->i_mtime = timer.t_time;					\
}

#define icrt(ip) {							\
	ip->i_flag |= IFCRT;						\
	ip->i_ctime = timer.t_time;					\
}

#define iamc(ip) {							\
	ip->i_flag |= IFACC|IFMOD|IFCRT;				\
	ip->i_atime = timer.t_time;					\
	ip->i_mtime = timer.t_time;					\
	ip->i_ctime = timer.t_time;					\
}

/*
 * Functions.
 */
extern	INODE	*exlopen();		/* exec.c */
extern	INODE	*ftoim();		/* fs1.c */
extern	INODE	*imake();		/* fs1.c */
extern	INODE	*iattach();		/* fs1.c */
extern	INODE	*ialloc();		/* fs2.c */
extern	daddr_t	balloc();		/* fs2.c */
extern	INODE	*pmake();		/* pipe.c */

/*
 * Global variables.
 */
extern	int	ronflag;		/* Root is read only */
extern	INODE	*inodep;		/* Pointer to in core inodes */
extern	INODE	*acctip;		/* Accounting file pointer */

#endif

#endif

/* end of sys/inode.h */
