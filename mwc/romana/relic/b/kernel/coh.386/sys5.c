/*
 * system calls introduced by the 386 port
 *
 * Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991
 */ 
#include <sys/coherent.h>
#include <sys/buf.h>
#include <errno.h>
#include <canon.h>
#include <sys/con.h>
#include <fcntl.h>
#include <sys/fd.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <sys/mount.h>
#include <ustat.h>
#include <sys/statfs.h>
#include <sys/sysi86.h>

utime(tp)
long *tp;
{
	return timer.t_time;
}

/*
 * Return an unique number.
 */
usysi86(f, arg1)
{
	register MOUNT *mp;
	register struct filsys *fsp;
	extern void (*ndpEmFn)();
	int fpval;
	extern short ndpType;

	switch (f) {
	case SYI86UNEEK:
		if ((mp=getment(rootdev, 1)) == NULL)
			return;
		fsp = &mp->m_super;
		fsp->s_fmod = 1;
		return (++fsp->s_unique);
	case SI86FPHW:
		/* 
		 * 2's bit: floating point ndp is present (80287/80387/80486dx)
		 * 1's bit (when 2's bit = 1): 80387/486dx is present
		 */
		if (!useracc(arg1, sizeof(int), 1)) {
			SET_U_ERROR(EFAULT, "sysi386:SI86FPHW");
			return;
		}
		if (ndpType <= 1) { /* no ndp */
			fpval = (ndpEmFn) ? FP_SW : FP_NO;
		} else {
			fpval = (ndpType > 2) ? FP_387 : FP_287;
		}
		putuwd(arg1, fpval);
		return 0;
		break;
	}
}

ushmsys(func, arg1, arg2, arg3)
int func, arg1, arg2, arg3;
{
	switch(func){
		case 0: return ushmat(arg1, arg2, arg3);
		case 1: return ushmctl(arg1, arg2, arg3);
		case 2: return ushmdt(arg1);
		case 3: return ushmget(arg1, arg2, arg3);
		default: u.u_error = EINVAL;
	}
}

usemsys(func, arg1, arg2, arg3, arg4)
int func, arg1, arg2, arg3, arg4;
{
	switch(func){
	case 0:	return usemctl(arg1, arg2, arg3, arg4);
	case 1:	return usemget(arg1, arg2, arg3);
	case 2:	return usemop(arg1, arg2, arg3);
	default: u.u_error = EINVAL;
	}
}

/*
 * uname and ustat system calls.
 *
 * int uname(struct utsname *name)
 * Before lcall instruction 4(%esp) contains name, 8(%esp) contains
 * an unspecified value, and 12(%esp) contains the value 0.
 *
 * int ustat(int dev, struct ustat *buf)
 * Before lcall instruction 4(%esp) contains buf (REVERSE order of argument)
 * 8(%esp) contains dev, and 12(%esp) contains the value 0.
 */
int
uutssys(arg1, arg2, func)
{
	switch(func) {	
	case 0: return uname(arg1);
	case 2: return u_ustat(arg2, arg1);
	default:u.u_error = EINVAL;
	}
}

/*
 * uname - get name of the current operating system.
 */
extern char	version[];	/* Defined in main.c */
extern char 	release[];	/* Defined in main.c */

/*
 * uname() should return a non-negative value on success
 * On failure, u.u_error is set.
 */
int
uname(name)
struct utsname	*name;
{
	register char	*rcp;			/**/
	register int	i;			/* Counter, loop index */
	register INODE	*ip;			/* /etc/uucpname inode */
	BUF		*bp;			/* Read buffer */
	char		namebuf[SYS_NMLN];	/* System name */
	int		fl;			/* File length*/

	/* Check if *name is an available user area */
	if (!useracc((char *) name, sizeof(struct utsname), 1)) {
		u.u_error = EFAULT;
		return;
	}

	/* Find the size of the version number */
	for (rcp = version, i = 0; *rcp != '\0' && i < SYS_NMLN; i++, rcp++)
			;

	/* Write version number to user area */
	if (!kucopy(version, name->version, i)) {
		u.u_error = EFAULT;
		return;
	}

	/* Find the size of the release number */
	for (rcp = release, i = 0; *rcp != '\0' && i < SYS_NMLN; i++, rcp++)
			;

	/* Write release number to user area */
	if (!kucopy(release, name->release, i)) {
		u.u_error = EFAULT;
		return;
	}

	/* Write "machine" to user area */
	if (!kucopy("i386", name->machine, 4)) {
		u.u_error = EFAULT;
		return;
	}

	/* We supposed that system name and nodename are in /etc/uucpname */
	if (ftoi("/etc/uucpname", 'r') != 0)
		return(sys_unknown(name));

	ip = u.u_cdiri;
	if ((fl = ip->i_size) == 0) {
		idetach(ip);
		return(sys_unknown(name));
	}

	/* iaccess() sets u_error if it fails. */
	if (iaccess(ip, IPR) == 0) {
		idetach(ip);
		return;
	}

	if ((bp = vread(ip, (daddr_t) 0)) == NULL) {
		brelease(bp);
		idetach(ip);
		u.u_error = EFAULT;
		return;
	}

	/* namebuf should be not more than SYS_NMLN - 1 characters long */
	fl = (fl > SYS_NMLN) ? SYS_NMLN : fl;
	kkcopy(bp->b_vaddr, namebuf, fl);
	brelease(bp);
	idetach(ip);

	if (fl == 1 && namebuf[0] == '\n')
		return(sys_unknown(name));

	for (rcp = namebuf, i = 0; i < fl; rcp++) {
		i++;
		if (*rcp == '\n') {
			*rcp = '\0';
			break;
		}
	}
	namebuf[i - 1] = '\0';

	/* Write system name to user area */
	if (!kucopy(namebuf, name->sysname, i)) {
		u.u_error = EFAULT;
		return;
	}

	/* Write system name to user area */
	if (!kucopy(namebuf, name->nodename, i)) {
		u.u_error = EFAULT;
		return;
	}

	return 0;
}

/*
 * sys_unknown - write name unknown to utsname struct
 * Return 0 on success.
 * Set u.u_error on failure.
 */
char	unknown[] = "UNKNOWN";

int
sys_unknown(name)
struct utsname	*name;
{
	if (!kucopy(unknown, name->sysname, sizeof(unknown))) {
		u.u_error = EFAULT;
		return;
	}

	if (!kucopy(unknown, name->nodename, sizeof(unknown))) {
		u.u_error = EFAULT;
		return;
	}

	return 0;
}

/* 
 * u_ustat - get file system statistics. (Name ustat in use for stat s.c.)
 */
u_ustat(dev, buf)
dev_t	dev;
struct ustat	*buf;
{
	register MOUNT	*mp;

	/* Check if buf is an available user area. */
	/* B_READ | B_WRITE is not implemented yet. */
	if (!useracc((char *) buf, sizeof(struct ustat), 1)) {
		u.u_error = EFAULT;
		return;
	}

	/* Take mount filesystem, check if dev is mounted device */
	for (mp = mountp; mp != NULL; mp = mp->m_next)
		if (mp->m_dev == dev)
			break;
	if (mp == NULL) {
		u.u_error = EINVAL;
		return;
	}

	/* Pickup information from superblock */
	/* Number of free blocks */
	if (!kucopy(&(mp->m_super.s_tfree), &(buf->f_tfree),  
						sizeof(mp->m_super.s_tfree)))
		return;
	/* Number of free inodes */
	if (!kucopy(&(mp->m_super.s_tinode), &(buf->f_tinode),  
						sizeof(mp->m_super.s_tinode)))
		return;
	/* File system name */
	if (!kucopy(mp->m_super.s_fname, buf->f_fname,  
						sizeof(mp->m_super.s_fname)))
		return;
	/* File system pack name */
	if (!kucopy(mp->m_super.s_fpack, buf->f_fpack,  
						sizeof(mp->m_super.s_fpack)))
		return;
}

umsgsys(func, arg1, arg2, arg3, arg4, arg5)
{
	switch (func) {
	case 0: return umsgget(arg1, arg2);
	case 1: return umsgctl(arg1, arg2, arg3);
	case 2: return umsgrcv(arg1, arg2, arg3, arg4, arg5);
	case 3:	return umsgsnd(arg1, arg2, arg3, arg4);
	default:u.u_error = EINVAL;
	}
}

/* Don't tell user process about last remaining 64k of RAM */
#define BRK_CUSHION	16

uulimit(cmd, newlimit)
{
	int freeClicks;

	switch (cmd) {
	case 1:	/* Get max # of 512-byte blocks per file. */
		return u.u_bpfmax;
		break;
	case 2: /* Set max # of 512-byte blocks per file. */
		/* (only superuser may increase this) */
		if (newlimit <= u.u_bpfmax || super()) {
			u.u_bpfmax = newlimit;
			return 0;
		}
		/* else super() will have set u.u_error to EPERM */
		break;
	case 3: /* Get max break value. */
		/* return (current brk value) + (amount of free space) */
		/* Don't report all free clicks - leave a cushion. */
		freeClicks = allocno() - BRK_CUSHION;
		if (freeClicks < 0)
			freeClicks = 0;
		return u.u_segl[SIPDATA].sr_base
		  + SELF->p_segp[SIPDATA]->s_size + NBPC * freeClicks;
		break;
	case 4: /* Return configured number of open files per process. */
		return NOFILE;
		break;
	default:
		u.u_error = EINVAL;
	}
}

/*
 *  Change the size of a file.
 */
uchsize(fd, size)
int fd;
register long size;
{
	FD *fdp;
	register INODE *ip;

	if ( size < 0 ) {
		u.u_error = EINVAL;
		return -1;
	}
	if ( ((fdp=fdget(fd))==NULL) || ((fdp->f_flag&IPW)==0) ) {
		u.u_error = EBADF;
		return -1;
	}
	ip = fdp->f_ip;
	switch ( ip->i_mode&IFMT ) {
	case IFREG:
		if ( size > (((long) u.u_bpfmax) * BSIZE) ) {
			u.u_error = EFBIG;
			return -1;
		}
		if ( size == ip->i_size )
			break;
		ilock(ip);
		if ( size < ip->i_size )
			blclear(ip, (ip->i_size+BSIZE-1)/BSIZE);
		ip->i_size = size;
		imod(ip);
		icrt(ip);
		iunlock(ip);
		break;
	case IFPIPE:
		if ( size > PIPSIZE ) {
			u.u_error = EFBIG;
			return -1;
		}
		ilock(ip);
		if ( !ip->i_par && !ip->i_psr ) {
			u.u_error = EPIPE;
			sendsig(SIGPIPE, SELF);
			iunlock(ip);
			return -1;
		}
		ip->i_pwx += (size - ip->i_pnc);
		if ( size > ip->i_pnc ) {
			if ( ip->i_pwx >= PIPSIZE )
				ip->i_pwx -= PIPSIZE;
		} else if ( size < ip->i_pnc ) {
			if ( ip->i_pwx < 0 )
				ip->i_pwx += PIPSIZE;
		}
		ip->i_pnc = size;
		imod(ip);
		icrt(ip);
		if ( size > 0 )
			pwake(ip, 2);	/* 2==IFWFW, see pipe.c	*/
		if ( size < PIPSIZE )
			pwake(ip, 1);	/* 1==IFWFR, see pipe.c	*/
		iunlock(ip);
		break;
	default:
		u.u_error = EBADF;
		return -1;
	}
	return 0;
}

/*
 * Remove a directory.
 */
urmdir(path)
char	*path;
{
	register INODE	*ip;
	int 		isdirempty();
	
	if (ftoi(path, 'r') != 0)
		return;
	ip = u.u_cdiri;

	/* Check if path is a directory */
	if ((ip->i_mode & IFMT) != IFDIR) {
		idetach(ip);
		SET_U_ERROR(ENOTDIR, "rmdir: no such file or directory");
		return;
	}
	/* We have to check if directory is empty */
	if (!isdirempty(ip)) {
		idetach(ip);
		SET_U_ERROR(EEXIST, "rmdir: directory is not empty");
		return;
	}
	idetach(ip);
	removedir(path);
	return (u.u_error);
}

/*
 * remove a directory entry.
 * path is a pointer to user area.
 */
removedir(path)
char	*path;
{
	char		buf[512];
	char		*cpbuf,		/* to internal file_name buffer */
			*cppath;	/* to user file_name buffer */

	/* Write path to a kernel buffer buf */
	cpbuf = buf;
	cppath = path;

	while ((*cpbuf = getubd(cppath)) != '\0') {
		cppath++;
		if (++cpbuf >= &buf[sizeof(buf) - 3]) {
			SET_U_ERROR(ENOENT, "rmdir: path too long");
			return;
		}
	}
	*cpbuf++ = '/';
	*cpbuf++ = '.';
	*cpbuf = '\0';
	u.u_io.io_seg = IOSYS;
	dunlink(buf);
	*cpbuf++ = '.';
	*cpbuf = '\0';
	dunlink(buf);
	u.u_io.io_seg = IOUSR;
	dunlink(path);
	return;
}

/*
 * Unlink the given directory.
 */
dunlink(np)
char *np;
{
	register INODE *ip;
	register dev_t dev;

	if (ftoi(np, 'u') != 0)
		return;
 
	ip = u.u_pdiri;
	if (iaccess(ip, IPW) == 0) {
		u.u_error = EACCES;
		goto err;
	}

	dev = ip->i_dev;
	if (diucheck(dev, u.u_cdirn) == 0)
		goto err;
	idirent(0);
	idetach(ip);

	if ((ip=iattach(dev, u.u_cdirn)) == NULL)
		return;

	if (ip->i_nlink > 0)
		--ip->i_nlink;
	icrt(ip);	/* unlink - ctime */

err:
	idetach(ip);
	return (0);
}

/*
 * This is a copy of iucheck. The only one difference is that that allows
 * to remove a directory to a regular user.
 */
diucheck(dev, ino)
register dev_t dev;
register ino_t ino;
{
	register INODE *ip;
	INODE inode;

	for (ip=&inodep[NINODE-1]; ip>=inodep; --ip) {
		if (ip->i_ino==ino && ip->i_dev==dev)
			break;
	}
	if (ip < inodep) {
		ip = &inode;
		ip->i_dev = dev;
		ip->i_ino = ino;
		if (icopydm(ip) == 0)
			return (0);
	}
	return (1);
}

/* 
 * Check if directory is empty.
 */
int isdirempty(ip)
register INODE	*ip;
{
	register char	*cp;
	int		count;
	BUF		*bp;

	for (count = 0; count < ip->i_size; count += 512) {
		if ((bp = vread(ip, count)) == NULL) 
			break;
		for (cp = (char *) bp->b_vaddr; 
				cp < (char *) bp->b_vaddr + BSIZE; cp += 16) {
			if (*cp == '\0' && *(cp + 1) == '\0') 
				continue;
			if (*(cp + 2) != '.')
				goto bad;
			if (*(cp + 3) == '\0')
				continue;
			if (*(cp + 3) != '.' || *(cp + 4) != '\0')
				goto bad;
		}
		brelease(bp);
	}
	return 1;
bad:
	brelease(bp);
	return 0;
}

/*
 * SysV compatible mkdir() system call.
 *
 *	Create a directory of the given "path" and "mode", if possible.
 *	Creating the directory is straight forward.  Trying to clean
 *	up in case we run out of inodes or freee blocks in the process 
 * 	is not trivial.
 *	This system call was implemented in very press time.
 * 	Vlad 6-04-92
 */
umkdir(path, mode)
char	*path;
int	mode;
{
	INODE 		*dmknod();		/* make directory node */
	register INODE 	*pip;			/* parent inode pointer */
	register char 	*cp_path, 
			*cpb_path, 
			*cp_dot, 
			*cp_dotdot,
			*cp_parent;
	char	 	bufpath[512],
			bufdot[512],		
			bufdotdot[512],
			bufparent[512];
	int		uid;
	int		error;
	/*
	 * Create a local copies of "path" which we can use to build up
	 * the required directory links:
	 *	path/. -- bufdot
	 *	path/..-- bufdotdot
	 * Verify that the given path is not too long.
	 */
	cp_path = path;
	cpb_path = bufpath;
	cp_dot = bufdot;
	cp_dotdot = bufdotdot;
	cp_parent = bufparent;

	while ((*cpb_path = getubd(cp_path)) != '\0') {
		*cp_dot++ = *cp_dotdot++ = *cp_parent++ = *cpb_path;
		++cp_path;
		if (++cpb_path >= &bufpath[sizeof(bufpath) - 3]) {
			SET_U_ERROR(ENOENT, "sys5: mkdir: path too long");
			return;
		}
	}
	while (--cp_parent >= bufparent) {
		if (*cp_parent == '/') {
			*++cp_parent = '\0';
			break;
		}
	}
	if (cp_parent < bufparent) {
		*++cp_parent = '.';
		*++cp_parent = '\0';
	}
	
	*cp_dotdot++ = *cp_dot++ = '/';
	*cp_dotdot++ = *cp_dot++ = '.';
	*cp_dotdot++ = '.';
	*cp_dotdot = *cp_dot = '\0';

	u.u_io.io_seg = IOSYS;

	u.u_io.io_seg = IOUSR;
	if ((pip = dmknod(path, mode)) == NULL) {
		return;
	}
	u.u_io.io_seg = IOSYS;
	/* Now we can switch our id to root. It allows to use existing
         * functions.
	 */
	uid = u.u_uid;
	u.u_uid = 0;
	ulink(bufpath, bufdot);
	if (u.u_error) {
		error = u.u_error;
		u.u_error = 0;
		uunlink(bufpath);
		u.u_error = 0;
		u.u_io.io_seg = IOUSR;
		u.u_uid = uid;
		u.u_error = error;
		return;
	}
	ulink(bufparent, bufdotdot);
	if (u.u_error) {
		error = u.u_error;
		u.u_error = 0;
		uunlink(bufdot);
		u.u_error = 0;
		uunlink(bufpath);
		u.u_uid = uid;
		u.u_io.io_seg = IOUSR;
		u.u_error = error;
		return;
	}
	u.u_io.io_seg = IOUSR;
	u.u_uid = uid;
	return 0;
}

/*
 * Create a directory.
 * We cannot use original ulink because it makes the directories only for
 * superuser.
 */
INODE *dmknod(np, mode)
char	*np;	/* Direcotory name */
int	mode;
{
	register INODE *ip, *pip;
	register int type;

	type = (mode & ~IFMT);
	type |= S_IFDIR;

	if (ftoi(np, 'c') != 0)
		return NULL;

	if ((ip=u.u_cdiri) != NULL) {
		SET_U_ERROR(EEXIST, "sys5: path already exist")
		idetach(ip);
		return NULL;
	}
	if ((ip=imake(type, (dev_t) 0)) != NULL)
		idetach(ip);
	pip = u.u_pdiri;	/* grab ptr to parent inode */
	return pip;
}

/*
 * Get directory entry in file system independent format.
 */
ugetdents(fd, bp, n)
int 		fd;	/* File descriptor to an open directory */
char 		*bp;	/* Buffer where entries should be read */
unsigned 	n;	/* Number of bytes to be read */
{
	struct direct	r_dir;	
	unsigned	bytes;	/* Number of bytes */
	struct dirent	sd;
	ino_t		inode;	/* Inode number */
	unsigned short	ofnm;	/* Offset to file name in dirent */
	char		*cw, 
			*cr;
	int		minbuf;	/* Minimum possible size of the bp */
	int		i, mod;
	int		entry;
	char		ends[3] = "";
	int		total = 0;

	cw = bp;

	ofnm = sizeof(sd.d_ino) + sizeof(sd.d_off) + sizeof(sd.d_reclen);

	/* Find minimum possible size of bp. It should be enough to contain the
	 * header of dirent, file name + '\0', and be on a sizeof(long)
	 * boundary.
	 */
	entry = ofnm + DIRSIZ + 1;
	mod = entry % sizeof(long);
	minbuf = entry + (mod ? sizeof(long) - mod : 0); 

	/* Is user buffer available? */
	if (!useracc(bp, n, 1) || n < minbuf) {
		u.u_error = EFAULT;
		return(0);
	}

	while(n - (cw - bp) >= minbuf) {
		/* Read next entry from the directory. 
		 * inode == 0 for rm(ed) entries 
		 */
		do {
			if ((bytes = dirio(fd, &r_dir, sizeof(struct direct), 
							&sd.d_off)) == 0) {
				return(total);
			}
			inode = r_dir.d_ino;
		} while (!inode);

		/* Find the size of file name */
		for (cr = r_dir.d_name, i = 0; *cr != '\0' && i < DIRSIZ; 
						i++, cr++)
			;

		/* Copy file name */
		if (!kucopy(r_dir.d_name, cw + ofnm, i))
			return(0);
		/* Write '\0' */
		putubd(cw + ofnm + i++, '\0');
		/* Round up to long boundary */
		if (mod = (ofnm + i) % sizeof(long))
			if (!kucopy(ends, cw + ofnm + i, sizeof(long) - mod))
				return(0);
		sd.d_ino = r_dir.d_ino;
		sd.d_reclen = ofnm + i;		/* Size of directory entry */
		if (mod)
			sd.d_reclen += sizeof(long) - mod;
		if (!kucopy(&sd, cw, ofnm))
			return(0);
		total += sd.d_reclen;
		cw += sd.d_reclen;
	}
	return(total);
}

/*
 * Read `n' bytes from the directory `fd' using the buffer `bp'.
 */
dirio(fd, bp, n, offset)
struct direct	*bp;
unsigned 	n;
off_t		*offset;
{
	register FD *fdp;
	register INODE *ip;

	/* Check file descriptor */
	if ((fdp = fdget(fd)) == NULL)
		return (0);
	if (((fdp->f_flag & IPR)) == 0) {
		u.u_error = EBADF;
		return (0);
	}
	ip = fdp->f_ip;
	if ((ip->i_mode & IFMT) != IFDIR) {
		u.u_error = EBADF;
		return(0);
	}

	ilock(ip);	/* We do not want file changes during the read */

	u.u_io.io_seek = fdp->f_seek;
	u.u_io.io.vbase = bp;
	u.u_io.io_ioc  = n;
	u.u_io.io_flag = (fdp->f_flag & IPNDLY) ? IONDLY : 0;

	iread(ip, &u.u_io);
	iacc(ip);		/* read - atime */

	n -= u.u_io.io_ioc;
	*offset = fdp->f_seek;
	fdp->f_seek += n;

	iunlock(ip);

	return (n);
}

/* 
 * Get file system information by file name.
 */
ustatfs(path, stfs, len, fstyp)
char		*path;	/* File name */
struct statfs	*stfs;	/* Pointer to a user structure */
int		len;	/* Size of the structure */
int		fstyp;	/* File system type */
{
	struct filsys	*statmount();	/* Get mp for mounted device */
	struct filsys	*statunmount();	/* Get mp for unmounted device */
	struct filsys	*sb;		/* Pointer to superblock */
	int		count = 0;	/* Number of copied bytes */
	short		systype = 1;	/* System type */
	long		bsize = BSIZE;	/* Block size */
	long		frsize = 0;	/* Fragment size */

	/* Check if stfs is an available user area. */
	if (!useracc((char *) stfs, len, 1)) {
		SET_U_ERROR(EFAULT, "ustatfs 0");
		return;
	}

	/* Filesystem type is 1 for 512 bytes blocks. */
	count += sizeof(systype);
	if (count > len) {
		SET_U_ERROR(EFAULT, "ustatfs 1");
		return;
	}
	if (!kucopy(&(systype), &(stfs->f_fstyp), sizeof(systype)))
		return;
	
	/* Block size */
	count += sizeof(bsize);
	if (count > len) {
		SET_U_ERROR(EFAULT, "ustatfs 2");
		return;
	}
	if (!kucopy(&(bsize), &(stfs->f_bsize), sizeof(bsize)))
		return;

	/* Fragment size. */
	count += sizeof(int);
	if (count > len) {
		SET_U_ERROR(EFAULT, "ustatfs 3");
		return;
	}
	if (!kucopy(&(frsize), &(stfs->f_frsize), sizeof(frsize)))
		return;

	if (!fstyp) {
		if ((sb = statmount(-1, path)) == NULL)
			return;
		devinfo(sb, stfs, len, &count);
	} else {
		if ((sb = statunmount(-1, path)) == NULL)
			return;
		devinfo(sb, stfs, len, &count);
		kfree(sb);
	}
	return 0;
}

/*
 * statmount - get superblock for mounted file system.
 * fd - file descriptor or -1, path -  file name or NULL.
 */
struct filsys *statmount(fd, path)
int	fd;
char	*path;
{	
	register INODE	*ip;		/* Structure inode */
	register FD	*fdp;
	register MOUNT	*mp;		/* Pointer to device */
	dev_t		device;		/* Mounted device */

	/* Find the device */
	if (path) {	/* Find ip by file name */
		if (ftoi(path, 'r')) {
			/* If ftoi returned nonzero, it also set u.u_error. */
			return NULL;
		}
		ip = u.u_cdiri;
		device = ip->i_dev;
		idetach(ip);
	} else {		/* Find ip by file descriptor */
		if ((fdp = fdget(fd)) == NULL)
			return NULL;
		if (((fdp->f_flag & IPR)) == 0) {
			SET_U_ERROR(EBADF, "statmount 1");
			return NULL;
		}
		ip = fdp->f_ip;
		device = ip->i_dev;
	}	
	/* Take mount filesystem, check if dev is mounted device */
	for (mp = mountp; mp != NULL; mp = mp->m_next)
		if (mp->m_dev == device)
			break;
	if (mp == NULL) {
		u.u_error = EINVAL;
		return NULL;
	}
	return &mp->m_super;
}

/*
 * devinfo() write system information to user area
 */
devinfo(sb, stfs, len, count)
struct filsys	*sb;	/* File name */
struct statfs	*stfs;	/* Pointer to a user structure */
int		len;	/* Size of the structure */
int		*count;
{
	long		inode;

	/* Total number of blocks */
	*count += sizeof(sb->s_fsize);
	if (*count > len)
		return;
	if (!kucopy(&(sb->s_fsize), &(stfs->f_blocks), 
					sizeof(sb->s_fsize)))
		return;

	/* Count of free blocks */
	*count += sizeof(sb->s_tfree);
	if (*count > len)
		return;
	if (!kucopy(&(sb->s_tfree), &(stfs->f_bfree),
					sizeof(sb->s_tfree)))
		return;

	/* Total number of file inodes */
	*count += sizeof(inode);
	if (*count > len)
		return;
	inode = (long) (sb->s_isize - INODEI) * INOPB;
	if (!kucopy(&inode, &(stfs->f_files), sizeof(inode)))
		return;

	/* Number of free inodes */
	*count += sizeof(inode);
	if (*count > len)
		return;
	inode = sb->s_tinode;
	if (!kucopy(&inode, &(stfs->f_ffree), sizeof(inode)))
		return;

	/* Volume name */
	*count += sizeof(sb->s_fname);
	if (*count > len)
		return;
	if (!kucopy(sb->s_fname, stfs->f_fname, 
					sizeof(sb->s_fpack)))
		return;

	/* Pack name */
	*count += sizeof(sb->s_fpack);
	if (*count > len)
		return;
	if (!kucopy(sb->s_fpack, stfs->f_fpack, 
					sizeof(sb->s_fpack)))
		return;
}

/*
 * statunmount - get superblock for unmounted file system.
 * fd - file descriptor or -1, path -  file name or NULL.
 */
struct filsys *statunmount(fd, path)
int	fd;	/* File descriptor */
char	*path;	/* File name */
{
	register INODE	*ip;
	register MOUNT	*mp;
	register dev_t	rdev;
	FD		*fdp;
	int		mode;
	BUF		*bp;
	struct filsys	*sb;

	/* Find the device */
	if (path) {	/* Find ip by file name */
		if (ftoi(path, 'r')) 
			return NULL;
		ip = u.u_cdiri;
		mode = ip->i_mode;
		rdev = ip->i_a.i_rdev;
		idetach(ip);
	} else {		/* Find ip by file descriptor */
		if ((fdp = fdget(fd)) == NULL)
			return NULL;
		if (((fdp->f_flag & IPR)) == 0) {
			u.u_error = EBADF;
			return NULL;
		}
		ip = fdp->f_ip;
		ilock(ip);
		mode = ip->i_mode;
		rdev = ip->i_a.i_rdev;
		iunlock(ip);
	}	

	/* Check for block special device */
	if ((mode & IFMT) != IFBLK) {
		u.u_error = ENOTBLK;
		return NULL;
	}

	/* Check if device is mounted device */
	for (mp = mountp; mp != NULL; mp = mp->m_next) {
		if (mp->m_dev == rdev) {
			u.u_error = EBUSY;
			return NULL;
		}
	}
	dopen(rdev, IPR, DFBLK);
	if (u.u_error) 
		return NULL;
	
	/*
 	 * NIGEL: Modified for new dclose ().
	 */

	bp = bread(rdev, (daddr_t) SUPERI, 1);
	dclose (rdev, IPR, DFBLK);

	if (bp == NULL)
		return NULL;

	if ((sb = kalloc(sizeof(struct filsys))) == NULL)
		return (NULL);

	kkcopy(bp->b_vaddr, sb, sizeof(struct filsys));
	brelease(bp);
	cansuper(sb);		/* canonicalize supperblock */
	if (tstf(sb) == 0) {	/* check for consistency */
		kfree(sb);
		u.u_error = EINVAL;
		return(NULL);
	}
	return(sb);
}

/* 
 * Get file system information by file descriptor
 */
ufstatfs(fildes, stfs, len, fstyp)
int		fildes;	/* File descriptor */
struct statfs	*stfs;	/* Pointer to a user structure */
int		len;	/* Size of the structure */
int		fstyp;	/* File system type */
{	
	struct filsys	*statmount();	/* Get mp for mounted device */
	struct filsys	*statunmount();	/* Get mp for unmounted device */
	struct filsys	*sb;		/* Pointer to superblock */
	int		count = 0;	/* Number of copied bytes */
	short		systype = 1;	/* System type */
	long		bsize = BSIZE;	/* Block size */
	long		frsize = 0;	/* Fragment size */

	/* Check if stfs is an available user area. */
	if (!useracc((char *) stfs, len)) {
		u.u_error = EFAULT;
		return;
	}

	/* Filesystem type is 1 for 512 bytes blocks. */
	count += sizeof(systype);
	if (count > len) {
		SET_U_ERROR(EFAULT, "ufstatfs 0");
		return;
	}
	if (!kucopy(&(systype), &(stfs->f_fstyp), sizeof(systype)))
		return;
	
	/* Block size */
	count += sizeof(bsize);
	if (count > len) {
		SET_U_ERROR(EFAULT, "ufstatfs 1");
		return;
	}
	if (!kucopy(&(bsize), &(stfs->f_bsize), sizeof(bsize)))
		return;

	/* Fragment size. */
	count += sizeof(int);
	if (count > len) {
		SET_U_ERROR(EFAULT, "ufstatfs 2");
		return;
	}
	if (!kucopy(&(frsize), &(stfs->f_frsize), sizeof(frsize)))
		return;

	if (!fstyp) {
		if ((sb = statmount(fildes, NULL)) == NULL)
			return;
		devinfo(sb, stfs, len, &count);
	} else {
		if ((sb = statunmount(fildes, NULL)) == NULL)
			return;
		devinfo(sb, stfs, len, &count);
		kfree(sb);
	}
	return 0;
}

/*
 * Check superblock for consistency.
 */
tstf(fp)
register struct filsys *fp;
{
	register daddr_t *dp;
	register ino_t *ip;
	register ino_t maxinode;

	maxinode = (fp->s_isize - INODEI) * INOPB + 1;
	if (fp->s_isize >= fp->s_fsize)
		return (0);
	if (fp->s_tfree < fp->s_nfree
	 || fp->s_tfree >= fp->s_fsize - fp->s_isize + 1)
		return (0);
	if (fp->s_tinode < fp->s_ninode
	 || fp->s_tinode >= maxinode-1)
		return (0);
	for (dp = &fp->s_free[0]; dp < &fp->s_free[fp->s_nfree]; dp += 1)
		if (*dp < fp->s_isize || *dp >= fp->s_fsize)
			return (0);
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[fp->s_ninode]; ip += 1)
		if (*ip < 1 || *ip > maxinode)
			return (0);
	return (1);
}

/* the following calls are not in the BCS */

uadmin()
{
	u.u_error = EINVAL;
}
