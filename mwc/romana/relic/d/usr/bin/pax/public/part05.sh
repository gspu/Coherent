# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 5 (of 6)."
# Contents:  buffer.c paxdir.c
# Wrapped by mark@jhereg on Tue Dec 27 19:37:59 1988
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f buffer.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"buffer.c\"
else
echo shar: Extracting \"buffer.c\" \(17360 characters\)
sed "s/^X//" >buffer.c <<'END_OF_buffer.c'
X/* $Source: /u/mark/src/pax/RCS/buffer.c,v $
X *
X * $Revision: 1.1 $
X *
X * buffer.c - Buffer management functions
X *
X * DESCRIPTION
X *
X *	These functions handle buffer manipulations for the archiving
X *	formats.  Functions are provided to get memory for buffers, 
X *	flush buffers, read and write buffers and de-allocate buffers.  
X *	Several housekeeping functions are provided as well to get some 
X *	information about how full buffers are, etc.
X *
X * AUTHOR
X *
X *	Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
X *
X * Sponsored by The USENIX Association for public distribution. 
X *
X * Copyright (c) 1989 Mark H. Colburn.
X * All rights reserved.
X *
X * Redistribution and use in source and binary forms are permitted
X * provided that the above copyright notice is duplicated in all such 
X * forms and that any documentation, advertising materials, and other 
X * materials related to such distribution and use acknowledge that the 
X * software was developed * by Mark H. Colburn and sponsored by The 
X * USENIX Association. 
X *
X * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
X * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
X * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
X *
X * $Log:	buffer.c,v $
X * Revision 1.1  88/12/23  18:02:01  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: buffer.c,v 1.1 88/12/23 18:02:01 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* Function Prototypes */
X
X#ifdef __STDC__
X
Xstatic int ar_write(int, char *, uint);
Xstatic void buf_pad(OFFSET);
Xstatic int indata(int, OFFSET, char *);
Xstatic void outflush(void);
Xstatic void buf_use(uint);
Xstatic int buf_in_avail(char **, uint *);
Xstatic uint buf_out_avail(char **);
X
X#else /* !__STDC__ */
X
Xstatic int ar_write();
Xstatic void buf_pad();
Xstatic int indata();
Xstatic void outflush();
Xstatic void buf_use();
Xstatic int buf_in_avail();
Xstatic uint buf_out_avail();
X
X#endif /* __STDC__ */
X
X
X/* inentry - install a single archive entry
X *
X * DESCRIPTION
X *
X *	Inentry reads an archive entry from the archive file and writes it
X *	out the the named file.  If we are in PASS mode during archive
X *	processing, the pass() function is called, otherwise we will
X *	extract from the archive file.
X *
X *	Inentry actaully calls indata to process the actual data to the
X *	file.
X *
X * PARAMETERS
X *
X *	char	*name	- name of the file to extract from the archive
X *	Stat	*asb	- stat block of the file to be extracted from the
X *			  archive.
X *
X * RETURNS
X *
X * 	Returns zero if successful, -1 otherwise. 
X */
X
X#ifdef __STDC__
X
Xint inentry(char *name, Stat *asb)
X
X#else
X
Xint inentry(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    Link           *linkp;
X    int             ifd;
X    int             ofd;
X    time_t          tstamp[2];
X
X    if ((ofd = openo(name, asb, linkp = linkfrom(name, asb), 0)) > 0) {
X	if (asb->sb_size || linkp == NULL || linkp->l_size == 0) {
X	    close(indata(ofd, asb->sb_size, name));
X	} else if ((ifd = open(linkp->l_path->p_name, O_RDONLY)) < 0) {
X	    warn(linkp->l_path->p_name, syserr());
X	} else {
X	    passdata(linkp->l_path->p_name, ifd, name, ofd);
X	    close(ifd);
X	    close(ofd);
X	}
X    } else {
X	return(buf_skip((OFFSET) asb->sb_size) >= 0);
X    }
X    tstamp[0] = (!f_pass && f_access_time) ? asb->sb_atime : time((time_t *) 0);
X    tstamp[1] = f_modification_time ? asb->sb_mtime : time((time_t *) 0);
X    utime(name, tstamp);
X    return (0);
X}
X
X
X/* outdata - write archive data
X *
X * DESCRIPTION
X *
X *	Outdata transfers data from the named file to the archive buffer.
X *	It knows about the file padding which is required by tar, but no
X *	by cpio.  Outdata continues after file read errors, padding with 
X *	null characters if neccessary.   Closes the input file descriptor 
X *	when finished.
X *
X * PARAMETERS
X *
X *	int	fd	- file descriptor of file to read data from
X *	char   *name	- name of file
X *	OFFSET	size	- size of the file
X *
X */
X
X#ifdef __STDC__
X
Xvoid outdata(int fd, char *name, OFFSET size)
X
X#else
X
Xvoid outdata(fd, name, size)
Xint             fd;
Xchar           *name;
XOFFSET          size;
X
X#endif
X{
X    uint            chunk;
X    int             got;
X    int             oops;
X    uint            avail;
X    int		    pad;
X    char           *buf;
X
X    oops = got = 0;
X    if (pad = (size % BLOCKSIZE)) {
X	pad = (BLOCKSIZE - pad);
X    }
X    while (size) {
X	avail = buf_out_avail(&buf);
X	size -= (chunk = size < avail ? (uint) size : avail);
X	if (oops == 0 && (got = read(fd, buf, (unsigned int) chunk)) < 0) {
X	    oops = -1;
X	    warn(name, syserr());
X	    got = 0;
X	}
X	if (got < chunk) {
X	    if (oops == NULL) {
X		oops = -1;
X	    }
X	    warn(name, "Early EOF");
X	    while (got < chunk) {
X		buf[got++] = '\0';
X	    }
X	}
X	buf_use(chunk);
X    }
X    close(fd);
X    if (ar_format == TAR) {
X	buf_pad((OFFSET) pad);
X    }
X}
X
X
X/* write_eot -  write the end of archive record(s)
X *
X * DESCRIPTION
X *
X *	Write out an End-Of-Tape record.  We actually zero at least one 
X *	record, through the end of the block.  Old tar writes garbage after 
X *	two zeroed records -- and PDtar used to.
X */
X
X#ifdef __STDC__
X
Xvoid write_eot(void)
X
X#else
X
Xvoid write_eot()
X
X#endif
X{
X    OFFSET           pad;
X    char            header[M_STRLEN + H_STRLEN + 1];
X
X    if (ar_format == TAR) {
X	/* write out two zero blocks for trailer */
X	pad = 2 * BLOCKSIZE;
X    } else {
X	if (pad = (total + M_STRLEN + H_STRLEN + TRAILZ) % BLOCKSIZE) {
X	    pad = BLOCKSIZE - pad;
X	}
X	strcpy(header, M_ASCII);
X	sprintf(header + M_STRLEN, H_PRINT, 0, 0,
X		       0, 0, 0, 1, 0, (time_t) 0, TRAILZ, pad);
X	outwrite(header, M_STRLEN + H_STRLEN);
X	outwrite(TRAILER, TRAILZ);
X    }
X    buf_pad((OFFSET) pad);
X    outflush();
X}
X
X 
X/* outwrite -  write archive data
X *
X * DESCRIPTION
X *
X *	Writes out data in the archive buffer to the archive file.  The
X *	buffer index and the total byte count are incremented by the number
X *	of data bytes written.
X *
X * PARAMETERS
X *	
X *	char   *idx	- pointer to data to write
X *	uint	len	- length of the data to write
X */
X
X#ifdef __STDC__
X
Xvoid outwrite(char *idx, uint len)
X
X#else
X
Xvoid outwrite(idx, len)
Xchar           *idx;	/* pointer to data to write */
Xuint            len;	/* length of data to write */
X
X#endif
X{
X    uint            have;
X    uint            want;
X    char           *endx;
X
X    endx = idx + len;
X    while (want = endx - idx) {
X	if (bufend - bufidx < 0) {
X	    fatal("Buffer overlow in out_write\n");
X	}
X	if ((have = bufend - bufidx) == 0) {
X	    outflush();
X	}
X	if (have > want) {
X	    have = want;
X	}
X	memcpy(bufidx, idx, (int) have);
X	bufidx += have;
X	idx += have;
X	total += have;
X    }
X}
X
X
X/* passdata - copy data to one file
X *
X * DESCRIPTION
X *
X *	Copies a file from one place to another.  Doesn't believe in input 
X *	file descriptor zero (see description of kludge in openi() comments). 
X *	Closes the provided output file descriptor. 
X *
X * PARAMETERS
X *
X *	char	*from	- input file name (old file)
X *	int	ifd	- input file descriptor
X *	char	*to	- output file name (new file)
X *	int	ofd	- output file descriptor
X */
X
X#ifdef __STDC__
X
Xvoid passdata(char *from, int ifd, char *to, int ofd)
X
X#else
X
Xvoid passdata(from, ifd, to, ofd)
Xchar           *from;
Xint             ifd;
Xchar           *to;
Xint             ofd;
X
X#endif
X{
X    int             got;
X    int             sparse;
X    char            block[BUFSIZ];
X
X    if (ifd) {
X	lseek(ifd, (OFFSET) 0, 0);
X	sparse = 0;
X	while ((got = read(ifd, block, sizeof(block))) > 0
X	       && (sparse = ar_write(ofd, block, (uint) got)) >= 0) {
X	    total += got;
X	}
X	if (got) {
X	    warn(got < 0 ? from : to, syserr());
X	} else if (sparse > 0
X		 && (lseek(ofd, (OFFSET)(-sparse), 1) < 0
X		     || write(ofd, block, (uint) sparse) != sparse)) {
X	    warn(to, syserr());
X	}
X    }
X    close(ofd);
X}
X
X
X/* buf_allocate - get space for the I/O buffer 
X *
X * DESCRIPTION
X *
X *	buf_allocate allocates an I/O buffer using malloc.  The resulting
X *	buffer is used for all data buffering throughout the program.
X *	Buf_allocate must be called prior to any use of the buffer or any
X *	of the buffering calls.
X *
X * PARAMETERS
X *
X *	int	size	- size of the I/O buffer to request
X *
X * ERRORS
X *
X *	If an invalid size is given for a buffer or if a buffer of the 
X *	required size cannot be allocated, then the function prints out an 
X *	error message and returns a non-zero exit status to the calling 
X *	process, terminating the program.
X *
X */
X
X#ifdef __STDC__
X
Xvoid buf_allocate(OFFSET size)
X
X#else
X
Xvoid buf_allocate(size)
XOFFSET            size;
X
X#endif
X{
X    extern char *malloc();
X    
X    if (size <= 0) {
X	fatal("invalid value for blocksize");
X    }
X    if ((bufstart = malloc((unsigned) size)) == NULL) {
X	fatal("Cannot allocate I/O buffer");
X    }
X    bufend = bufidx = bufstart;
X    bufend += size;
X}
X
X
X/* buf_skip - skip input archive data
X *
X * DESCRIPTION
X *
X *	Buf_skip skips past archive data.  It is used when the length of
X *	the archive data is known, and we do not wish to process the data.
X *
X * PARAMETERS
X *
X *	OFFSET	len	- number of bytes to skip
X *
X * RETURNS
X *
X * 	Returns zero under normal circumstances, -1 if unreadable data is 
X * 	encountered. 
X */
X
X#ifdef __STDC__
X
Xint buf_skip(OFFSET len)
X
X#else
X
Xint buf_skip(len)
XOFFSET           len;
X
X#endif
X{
X    uint            chunk;
X    int             corrupt = 0;
X
X    while (len) {
X	if (bufend - bufidx < 0) {
X	    fatal("Buffer overlow in buf_skip\n");
X	}
X	while ((chunk = bufend - bufidx) == 0) {
X	    corrupt |= ar_read();
X	}
X	if (chunk > len) {
X	    chunk = len;
X	}
X	bufidx += chunk;
X	len -= chunk;
X	total += chunk;
X    }
X    return (corrupt);
X}
X
X
X/* buf_read - read a given number of characters from the input archive
X *
X * DESCRIPTION
X *
X *	Reads len number of characters from the input archive and
X *	stores them in the buffer pointed at by dst.
X *
X * PARAMETERS
X *
X *	char   *dst	- pointer to buffer to store data into
X *	uint	len	- length of data to read
X *
X * RETURNS
X *
X * 	Returns zero with valid data, -1 if unreadable portions were 
X *	replaced by null characters. 
X */
X
X#ifdef __STDC__
X
Xint buf_read(char *dst, uint len)
X
X#else
X
Xint buf_read(dst, len)
Xchar           *dst;
Xuint            len;
X
X#endif
X{
X    int             have;
X    int             want;
X    int             corrupt = 0;
X    char           *endx = dst + len;
X
X    while (want = endx - dst) {
X	if (bufend - bufidx < 0) {
X	    fatal("Buffer overlow in buf_read\n");
X	}
X	while ((have = bufend - bufidx) == 0) {
X	    have = 0;
X	    corrupt |= ar_read();
X	}
X	if (have > want) {
X	    have = want;
X	}
X	memcpy(dst, bufidx, have);
X	bufidx += have;
X	dst += have;
X	total += have;
X    }
X    return (corrupt);
X}
X
X
X/* indata - install data from an archive
X *
X * DESCRIPTION
X *
X *	Indata writes size bytes of data from the archive buffer to the output 
X *	file specified by fd.  The filename which is being written, pointed
X *	to by name is provided only for diagnostics.
X *
X * PARAMETERS
X *
X *	int	fd	- output file descriptor
X *	OFFSET	size	- number of bytes to write to output file
X *	char	*name	- name of file which corresponds to fd
X *
X * RETURNS
X *
X * 	Returns given file descriptor. 
X */
X
X#ifdef __STDC__
X
Xstatic int indata(int fd, OFFSET size, char *name)
X
X#else
X
Xstatic int indata(fd, size, name)
Xint             fd;
XOFFSET          size;
Xchar           *name;
X
X#endif
X{
X    uint            chunk;
X    char           *oops;
X    int             sparse;
X    int             corrupt;
X    char           *buf;
X    uint            avail;
X
X    corrupt = sparse = 0;
X    oops = NULL;
X    while (size) {
X	corrupt |= buf_in_avail(&buf, &avail);
X	size -= (chunk = size < avail ? (uint) size : avail);
X	if (oops == NULL && (sparse = ar_write(fd, buf, chunk)) < 0) {
X	    oops = syserr();
X	}
X	buf_use(chunk);
X    }
X    if (corrupt) {
X	warn(name, "Corrupt archive data");
X    }
X    if (oops) {
X	warn(name, oops);
X    } else if (sparse > 0 && (lseek(fd, (OFFSET) - 1, 1) < 0
X			      || write(fd, "", 1) != 1)) {
X	warn(name, syserr());
X    }
X    return (fd);
X}
X
X
X/* outflush - flush the output buffer
X *
X * DESCRIPTION
X *
X *	The output buffer is written, if there is anything in it, to the
X *	archive file.
X */
X
X#ifdef __STDC__
X
Xstatic void outflush(void)
X
X#else
X
Xstatic void outflush()
X
X#endif
X{
X    char           *buf;
X    int             got;
X    uint            len;
X
X    /* if (bufidx - buf > 0) */
X	for (buf = bufstart; len = bufidx - buf;) {
X	    if ((got = write(archivefd, buf, MIN(len, blocksize))) > 0) {
X		buf += got;
X	    } else if (got < 0) {
X		next(AR_WRITE);
X	    }
X	}
X    bufend = (bufidx = bufstart) + blocksize;
X}
X
X
X/* ar_read - fill the archive buffer
X *
X * DESCRIPTION
X *
X * 	Remembers mid-buffer read failures and reports them the next time 
X *	through.  Replaces unreadable data with null characters.   Resets
X *	the buffer pointers as appropriate.
X *
X * RETURNS
X *
X *	Returns zero with valid data, -1 otherwise. 
X */
X
X#ifdef __STDC__
X
Xint ar_read(void)
X
X#else
X
Xint ar_read()
X
X#endif
X{
X    int             got;
X    static int      failed;
X
X    bufend = bufidx = bufstart;
X    if (!failed) {
X	if (areof) {
X	    if (total == 0) {
X		fatal("No input");
X	    } else {
X		next(AR_READ);
X	    }
X	}
X	while (!failed && !areof && bufstart + blocksize - bufend >= blocksize) {
X	    if ((got = read(archivefd, bufend, (unsigned int) blocksize)) > 0) {
X		bufend += got;
X	    } else if (got < 0) {
X		failed = -1;
X		warnarch(syserr(), (OFFSET) 0 - (bufend - bufidx));
X	    } else {
X		++areof;
X	    }
X	}
X    }
X    if (failed && bufend == bufstart) {
X	failed = 0;
X	for (got = 0; got < blocksize; ++got) {
X	    *bufend++ = '\0';
X	}
X	return (-1);
X    }
X    return (0);
X}
X
X
X/* ar_write - write a filesystem block
X *
X * DESCRIPTION
X *
X * 	Writes len bytes of data data from the specified buffer to the 
X *	specified file.   Seeks past sparse blocks. 
X *
X * PARAMETERS
X *
X *	int     fd	- file to write to
X *	char   *buf	- buffer to get data from
X *	uint	len	- number of bytes to transfer
X *
X * RETURNS
X *
X *	Returns 0 if the block was written, the given length for a sparse 
X *	block or -1 if unsuccessful. 
X */
X
X#ifdef __STDC__
X
Xstatic int ar_write(int fd, char *buf, uint len)
X
X#else
X
Xstatic int ar_write(fd, buf, len)
Xint             fd;
Xchar           *buf;
Xuint            len;
X
X#endif
X{
X    char           *bidx;
X    char           *bend;
X
X    bend = (bidx = buf) + len;
X    while (bidx < bend) {
X	if (*bidx++) {
X	    return (write(fd, buf, len) == len ? 0 : -1);
X	}
X    }
X    return (lseek(fd, (OFFSET) len, 1) < 0 ? -1 : len);
X}
X
X
X/* buf_pad - pad the archive buffer
X *
X * DESCRIPTION
X *
X *	Buf_pad writes len zero bytes to the archive buffer in order to 
X *	pad it.
X *
X * PARAMETERS
X *
X *	OFFSET	pad	- number of zero bytes to pad
X *
X */
X
X#ifdef __STDC__
X
Xstatic void buf_pad(OFFSET pad)
X
X#else
X
Xstatic void buf_pad(pad)
XOFFSET           pad;
X
X#endif
X{
X    int             idx;
X    int             have;
X
X    while (pad) {
X	if ((have = bufend - bufidx) > pad) {
X	    have = pad;
X	}
X	for (idx = 0; idx < have; ++idx) {
X	    *bufidx++ = '\0';
X	}
X	total += have;
X	pad -= have;
X	if (bufend - bufidx == 0) {
X	    outflush();
X	}
X    }
X}
X
X
X/* buf_use - allocate buffer space
X *
X * DESCRIPTION
X *
X *	Buf_use marks space in the buffer as being used; advancing both the
X *	buffer index (bufidx) and the total byte count (total).
X *
X * PARAMETERS
X *
X *	uint	len	- Amount of space to allocate in the buffer
X */
X
X#ifdef __STDC__
X
Xstatic void buf_use(uint len)
X
X#else
X
Xstatic void buf_use(len)
Xuint            len;
X
X#endif
X{
X    bufidx += len;
X    total += len;
X}
X
X
X/* buf_in_avail - index available input data within the buffer
X *
X * DESCRIPTION
X *
X *	Buf_in_avail fills the archive buffer, and points the bufp
X *	parameter at the start of the data.  The lenp parameter is
X *	modified to contain the number of bytes which were read.
X *
X * PARAMETERS
X *
X *	char   **bufp	- pointer to the buffer to read data into
X *	uint	*lenp	- pointer to the number of bytes which were read
X *			  (returned to the caller)
X *
X * RETURNS
X *
X * 	Stores a pointer to the data and its length in given locations. 
X *	Returns zero with valid data, -1 if unreadable portions were 
X *	replaced with nulls. 
X *
X * ERRORS
X *
X *	If an error occurs in ar_read, the error code is returned to the
X *	calling function.
X *
X */
X
X#ifdef __STDC__
X
Xstatic int buf_in_avail(char **bufp, uint *lenp)
X
X#else
X
Xstatic int buf_in_avail(bufp, lenp)
Xchar          **bufp;
Xuint           *lenp;
X
X#endif
X{
X    uint            have;
X    int             corrupt = 0;
X
X    while ((have = bufend - bufidx) == 0) {
X	corrupt |= ar_read();
X    }
X    *bufp = bufidx;
X    *lenp = have;
X    return (corrupt);
X}
X
X
X/* buf_out_avail  - index buffer space for archive output
X *
X * DESCRIPTION
X *
X * 	Stores a buffer pointer at a given location. Returns the number 
X *	of bytes available. 
X *
X * PARAMETERS
X *
X *	char	**bufp	- pointer to the buffer which is to be stored
X *
X * RETURNS
X *
X * 	The number of bytes which are available in the buffer.
X *
X */
X
X#ifdef __STDC__
X
Xstatic uint buf_out_avail(char **bufp)
X
X#else
X
Xstatic uint buf_out_avail(bufp)
Xchar          **bufp;
X
X#endif
X{
X    int             have;
X
X    if (bufend - bufidx < 0) {
X	fatal("Buffer overlow in buf_out_avail\n");
X    }
X    if ((have = bufend - bufidx) == 0) {
X	outflush();
X    }
X    *bufp = bufidx;
X    return (have);
X}
END_OF_buffer.c
if test 17360 -ne `wc -c <buffer.c`; then
    echo shar: \"buffer.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f paxdir.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"paxdir.c\"
else
echo shar: Extracting \"paxdir.c\" \(16936 characters\)
sed "s/^X//" >paxdir.c <<'END_OF_paxdir.c'
X/*
X	opendir -- open a directory stream
X  
X	last edit:	16-Jun-1987	D A Gwyn
X*/
X
X#include	<sys/errno.h>
X#include	<sys/types.h>
X#include	<sys/stat.h>
X#include	"paxdir.h"
X
X#ifdef BSD_SYSV
X/*
X	<sys/_dir.h> -- definitions for 4.2,4.3BSD directories
X  
X	last edit:	25-Apr-1987	D A Gwyn
X  
X	A directory consists of some number of blocks of DIRBLKSIZ bytes each,
X	where DIRBLKSIZ is chosen such that it can be transferred to disk in a
X	single atomic operation (e.g., 512 bytes on most machines).
X  
X	Each DIRBLKSIZ-byte block contains some number of directory entry
X	structures, which are of variable length.  Each directory entry has the
X	beginning of a (struct direct) at the front of it, containing its
X	filesystem-unique ident number, the length of the entry, and the length
X	of the name contained in the entry.  These are followed by the NUL-
X	terminated name padded to a (long) boundary with 0 bytes.  The maximum
X	length of a name in a directory is MAXNAMELEN.
X  
X	The macro DIRSIZ(dp) gives the amount of space required to represent a
X	directory entry.  Free space in a directory is represented by entries
X	that have dp->d_reclen > DIRSIZ(dp).  All DIRBLKSIZ bytes in a
X	directory block are claimed by the directory entries; this usually
X	results in the last entry in a directory having a large dp->d_reclen.
X	When entries are deleted from a directory, the space is returned to the
X	previous entry in the same directory block by increasing its
X	dp->d_reclen.  If the first entry of a directory block is free, then
X	its dp->d_fileno is set to 0; entries other than the first in a
X	directory do not normally have 	dp->d_fileno set to 0.
X  
X	prerequisite:	<sys/types.h>
X*/
X
X#if defined(accel) || defined(sun) || defined(vax)
X#define	DIRBLKSIZ	512	/* size of directory block */
X#else
X#ifdef alliant
X#define	DIRBLKSIZ	4096	/* size of directory block */
X#else
X#ifdef gould
X#define	DIRBLKSIZ	1024	/* size of directory block */
X#else
X#ifdef ns32000			/* Dynix System V */
X#define	DIRBLKSIZ	2600	/* size of directory block */
X#else				/* be conservative; multiple blocks are okay
X				 * but fractions are not */
X#define	DIRBLKSIZ	4096	/* size of directory block */
X#endif
X#endif
X#endif
X#endif
X
X#define	MAXNAMELEN	255	/* maximum filename length */
X/* NOTE:  not MAXNAMLEN, which has been preempted by SVR3 <dirent.h> */
X
Xstruct direct {			/* data from read()/_getdirentries() */
X    unsigned long   d_fileno;	/* unique ident of entry */
X    unsigned short  d_reclen;	/* length of this record */
X    unsigned short  d_namlen;	/* length of string in d_name */
X    char            d_name[MAXNAMELEN + 1];	/* NUL-terminated filename */
X};
X
X/*
X	The DIRSIZ macro gives the minimum record length which will hold the
X	directory entry.  This requires the amount of space in a (struct
X	direct) without the d_name field, plus enough space for the name with a
X	terminating NUL character, rounded up to a (long) boundary.
X  
X	(Note that Berkeley didn't properly compensate for struct padding,
X	but we nevertheless have to use the same size as the actual system.)
X*/
X
X#define	DIRSIZ( dp )	((sizeof(struct direct) - (MAXNAMELEN+1) \
X			+ sizeof(long) + (dp)->d_namlen) \
X			/ sizeof(long) * sizeof(long))
X
X#else
X#include	<sys/dir.h>
X#ifdef SYSV3
X#undef	MAXNAMLEN		/* avoid conflict with SVR3 */
X#endif
X /* Good thing we don't need to use the DIRSIZ() macro! */
X#ifdef d_ino			/* 4.3BSD/NFS using d_fileno */
X#undef	d_ino			/* (not absolutely necessary) */
X#else
X#define	d_fileno	d_ino	/* (struct direct) member */
X#endif
X#endif
X#ifdef UNK
X#ifndef UFS
X#include "***** ERROR ***** UNK applies only to UFS"
X/* One could do something similar for getdirentries(), but I didn't bother. */
X#endif
X#include	<signal.h>
X#endif
X
X#if defined(UFS) + defined(BFS) + defined(NFS) != 1	/* sanity check */
X#include "***** ERROR ***** exactly one of UFS, BFS, or NFS must be defined"
X#endif
X
X#ifdef UFS
X#define	RecLen( dp )	(sizeof(struct direct))	/* fixed-length entries */
X#else				/* BFS || NFS */
X#define	RecLen( dp )	((dp)->d_reclen)	/* variable-length entries */
X#endif
X
X#ifdef NFS
X#ifdef BSD_SYSV
X#define	getdirentries	_getdirentries	/* package hides this system call */
X#endif
Xextern int      getdirentries();
Xstatic long     dummy;		/* getdirentries() needs basep */
X#define	GetBlock( fd, buf, n )	getdirentries( fd, buf, (unsigned)n, &dummy )
X#else				/* UFS || BFS */
X#ifdef BSD_SYSV
X#define read	_read		/* avoid emulation overhead */
X#endif
Xextern int      read();
X#define	GetBlock( fd, buf, n )	read( fd, buf, (unsigned)n )
X#endif
X
X#ifdef UNK
Xextern int      _getdents();	/* actual system call */
X#endif
X
Xextern char    *strncpy();
Xextern int      fstat();
Xextern OFFSET   lseek();
X
Xextern int      errno;
X
X#ifndef DIRBLKSIZ
X#define	DIRBLKSIZ	4096	/* directory file read buffer size */
X#endif
X
X#ifndef NULL
X#define	NULL	0
X#endif
X
X#ifndef SEEK_CUR
X#define	SEEK_CUR	1
X#endif
X
X#ifndef S_ISDIR			/* macro to test for directory file */
X#define	S_ISDIR( mode )		(((mode) & S_IFMT) == S_IFDIR)
X#endif
X
X
X#ifndef SEEK_CUR
X#define	SEEK_CUR	1
X#endif
X
X#ifdef BSD_SYSV
X#define open	_open		/* avoid emulation overhead */
X#endif
X
Xextern int      getdents();	/* SVR3 system call, or emulation */
X
Xtypedef char   *pointer;	/* (void *) if you have it */
X
Xextern void     free();
Xextern pointer  malloc();
Xextern int
Xopen(), close(), fstat();
X
Xextern int      errno;
Xextern OFFSET   lseek();
X
X#ifndef SEEK_SET
X#define	SEEK_SET	0
X#endif
X
Xtypedef int     bool;		/* Boolean data type */
X#define	false	0
X#define	true	1
X
X
X#ifndef NULL
X#define	NULL	0
X#endif
X
X#ifndef O_RDONLY
X#define	O_RDONLY	0
X#endif
X
X#ifndef S_ISDIR			/* macro to test for directory file */
X#define	S_ISDIR( mode )		(((mode) & S_IFMT) == S_IFDIR)
X#endif
X
X#ifdef __STDC__
X
XDIR *opendir(char *dirname)
X
X#else
X    
XDIR *opendir(dirname)
Xchar           *dirname;	/* name of directory */
X
X#endif
X{
X    register DIR   *dirp;	/* -> malloc'ed storage */
X    register int    fd;		/* file descriptor for read */
X    struct stat     sbuf;	/* result of fstat() */
X
X    if ((fd = open(dirname, O_RDONLY)) < 0)
X	return NULL;		/* errno set by open() */
X
X    if (fstat(fd, &sbuf) != 0 || !S_ISDIR(sbuf.st_mode)) {
X	close(fd);
X	errno = ENOTDIR;
X	return NULL;		/* not a directory */
X    }
X    if ((dirp = (DIR *) malloc(sizeof(DIR))) == NULL
X	|| (dirp->dd_buf = (char *) malloc((unsigned) DIRBUF)) == NULL
X	) {
X	register int    serrno = errno;
X	/* errno set to ENOMEM by sbrk() */
X
X	if (dirp != NULL)
X	    free((pointer) dirp);
X
X	close(fd);
X	errno = serrno;
X	return NULL;		/* not enough memory */
X    }
X    dirp->dd_fd = fd;
X    dirp->dd_loc = dirp->dd_size = 0;	/* refill needed */
X
X    return dirp;
X}
X
X
X/*
X *	closedir -- close a directory stream
X *
X *	last edit:	11-Nov-1988	D A Gwyn
X */
X
X#ifdef __STDC__
X
Xint closedir(register DIR *dirp)
X
X#else
X    
Xint closedir(dirp)
Xregister DIR	*dirp;		/* stream from opendir() */
X
X#endif
X{
X    register int	fd;
X
X    if ( dirp == NULL || dirp->dd_buf == NULL ) {
X	errno = EFAULT;
X	return -1;			/* invalid pointer */
X    }
X
X    fd = dirp->dd_fd;			/* bug fix thanks to R. Salz */
X    free( (pointer)dirp->dd_buf );
X    free( (pointer)dirp );
X    return close( fd );
X}
X
X
X/*
X	readdir -- read next entry from a directory stream
X  
X	last edit:	25-Apr-1987	D A Gwyn
X*/
X
X#ifdef __STDC__
X
Xstruct dirent  *readdir(register DIR *dirp)
X
X#else
X    
Xstruct dirent  *readdir(dirp)
Xregister DIR   *dirp;		/* stream from opendir() */
X
X#endif
X{
X    register struct dirent *dp;	/* -> directory data */
X
X    if (dirp == NULL || dirp->dd_buf == NULL) {
X	errno = EFAULT;
X	return NULL;		/* invalid pointer */
X    }
X    do {
X	if (dirp->dd_loc >= dirp->dd_size)	/* empty or obsolete */
X	    dirp->dd_loc = dirp->dd_size = 0;
X
X	if (dirp->dd_size == 0	/* need to refill buffer */
X	    && (dirp->dd_size =
X		getdents(dirp->dd_fd, dirp->dd_buf, (unsigned) DIRBUF)
X		) <= 0
X	    )
X	    return NULL;	/* EOF or error */
X
X	dp = (struct dirent *) & dirp->dd_buf[dirp->dd_loc];
X	dirp->dd_loc += dp->d_reclen;
X    }
X    while (dp->d_ino == 0L);	/* don't rely on getdents() */
X
X    return dp;
X}
X
X
X/*
X	seekdir -- reposition a directory stream
X  
X	last edit:	24-May-1987	D A Gwyn
X  
X	An unsuccessful seekdir() will in general alter the current
X	directory position; beware.
X  
X	NOTE:	4.nBSD directory compaction makes seekdir() & telldir()
X		practically impossible to do right.  Avoid using them!
X*/
X
X#ifdef __STDC__
X
Xvoid seekdir(register DIR *dirp, register OFFSET loc)
X
X#else
X    
Xvoid seekdir(dirp, loc)
Xregister DIR   *dirp;		/* stream from opendir() */
Xregister OFFSET  loc;		/* position from telldir() */
X
X#endif
X{
X    register bool   rewind;	/* "start over when stymied" flag */
X
X    if (dirp == NULL || dirp->dd_buf == NULL) {
X	errno = EFAULT;
X	return;			/* invalid pointer */
X    }
X    /*
X     * A (struct dirent)'s d_off is an invented quantity on 4.nBSD
X     * NFS-supporting systems, so it is not safe to lseek() to it. 
X     */
X
X    /* Monotonicity of d_off is heavily exploited in the following. */
X
X    /*
X     * This algorithm is tuned for modest directory sizes.  For huge
X     * directories, it might be more efficient to read blocks until the first
X     * d_off is too large, then back up one block, or even to use binary
X     * search on the directory blocks.  I doubt that the extra code for that
X     * would be worthwhile. 
X     */
X
X    if (dirp->dd_loc >= dirp->dd_size	/* invalid index */
X	|| ((struct dirent *) & dirp->dd_buf[dirp->dd_loc])->d_off > loc
X    /* too far along in buffer */
X	)
X	dirp->dd_loc = 0;	/* reset to beginning of buffer */
X    /* else save time by starting at current dirp->dd_loc */
X
X    for (rewind = true;;) {
X	register struct dirent *dp;
X
X	/* See whether the matching entry is in the current buffer. */
X
X	if ((dirp->dd_loc < dirp->dd_size	/* valid index */
X	     || readdir(dirp) != NULL	/* next buffer read */
X	     && (dirp->dd_loc = 0, true)	/* beginning of buffer set */
X	     )
X	    && (dp = (struct dirent *) & dirp->dd_buf[dirp->dd_loc])->d_off
X	    <= loc		/* match possible in this buffer */
X	    ) {
X	    for ( /* dp initialized above */ ;
X		 (char *) dp < &dirp->dd_buf[dirp->dd_size];
X		 dp = (struct dirent *) ((char *) dp + dp->d_reclen)
X		)
X		if (dp->d_off == loc) {	/* found it! */
X		    dirp->dd_loc =
X			(char *) dp - dirp->dd_buf;
X		    return;
X		}
X	    rewind = false;	/* no point in backing up later */
X	    dirp->dd_loc = dirp->dd_size;	/* set end of buffer */
X	} else
X	 /* whole buffer past matching entry */ if (!rewind) {	/* no point in searching
X								 * further */
X	    errno = EINVAL;
X	    return;		/* no entry at specified loc */
X	} else {		/* rewind directory and start over */
X	    rewind = false;	/* but only once! */
X
X	    dirp->dd_loc = dirp->dd_size = 0;
X
X	    if (lseek(dirp->dd_fd, (OFFSET) 0, SEEK_SET)
X		!= 0
X		)
X		return;		/* errno already set (EBADF) */
X
X	    if (loc == 0)
X		return;		/* save time */
X	}
X    }
X}
X
X
X/* telldir - report directory stream position
X *
X * DESCRIPTION
X *
X *	Returns the offset of the next directory entry in the
X *	directory associated with dirp.
X *
X *	NOTE:	4.nBSD directory compaction makes seekdir() & telldir()
X *		practically impossible to do right.  Avoid using them!
X *
X * PARAMETERS
X *
X *	DIR	*dirp	- stream from opendir()
X *
X * RETURNS
X *
X * 	Return offset of next entry 
X */
X
X
X#ifdef __STDC__
X
XOFFSET telldir(DIR *dirp)
X
X#else
X    
XOFFSET telldir(dirp)			
XDIR            *dirp;		/* stream from opendir() */
X
X#endif
X{
X    if (dirp == NULL || dirp->dd_buf == NULL) {
X	errno = EFAULT;
X	return -1;		/* invalid pointer */
X    }
X    if (dirp->dd_loc < dirp->dd_size)	/* valid index */
X	return ((struct dirent *) & dirp->dd_buf[dirp->dd_loc])->d_off;
X    else			/* beginning of next directory block */
X	return lseek(dirp->dd_fd, (OFFSET) 0, SEEK_CUR);
X}
X
X
X#ifdef UFS
X
X/*
X	The following routine is necessary to handle DIRSIZ-long entry names.
X	Thanks to Richard Todd for pointing this out.
X*/
X
X
X/* return # chars in embedded name */
X
X#ifdef __STDC__
X
Xstatic int NameLen(char *name)
X
X#else
X    
Xstatic int NameLen(name)
Xchar            *name;		/* -> name embedded in struct direct */
X
X#endif
X{
X    register char  *s;		/* -> name[.] */
X    register char  *stop = &name[DIRSIZ];	/* -> past end of name field */
X
X    for (s = &name[1];		/* (empty names are impossible) */
X	 *s != '\0'		/* not NUL terminator */
X	 && ++s < stop;		/* < DIRSIZ characters scanned */
X	);
X
X    return s - name;		/* # valid characters in name */
X}
X
X#else				/* BFS || NFS */
X
Xextern int      strlen();
X
X#define	NameLen( name )	strlen( name )	/* names are always NUL-terminated */
X
X#endif
X
X#ifdef UNK
Xstatic enum {
X    maybe, no, yes
X} state = maybe;
X
X
X/* sig_catch - used to catch signals
X *
X * DESCRIPTION
X *
X *	Used to catch signals.
X */
X
X/*ARGSUSED*/
X
X#ifdef __STDC__
X
Xstatic void sig_catch(int sig)
X
X#else
X    
Xstatic void sig_catch(sig)
Xint             sig;		/* must be SIGSYS */
X
X#endif
X{
X    state = no;			/* attempted _getdents() faulted */
X}
X#endif
X
X
X/* getdents - get directory entries
X *
X * DESCRIPTION
X *
X *	Gets directory entries from the filesystem in an implemenation
X *	defined way.
X *
X * PARAMETERS
X *
X *	int             fildes	- directory file descriptor 
X *	char           *buf	- where to put the (struct dirent)s 
X *	unsigned	nbyte	- size of buf[] 
X *
X * RETURNS
X * 
X *	Returns number of bytes read; 0 on EOF, -1 on error 
X */
X
X#ifdef __STDC__
X
Xint getdents(int fildes, char *buf, unsigned nbyte)
X
X#else
X    
Xint getdents(fildes, buf, nbyte)	
Xint             fildes;		/* directory file descriptor */
Xchar           *buf;		/* where to put the (struct dirent)s */
Xunsigned        nbyte;		/* size of buf[] */
X
X#endif
X{
X    int             serrno;	/* entry errno */
X    OFFSET          offset;	/* initial directory file offset */
X    struct stat     statb;	/* fstat() info */
X    union {
X	/* directory file block buffer */
X#ifdef UFS
X	char		dblk[DIRBLKSIZ + 1];
X#else
X	char            dblk[DIRBLKSIZ];
X#endif
X	struct direct   dummy;	/* just for alignment */
X    } u;		/* (avoids having to malloc()) */
X    register struct direct *dp;	/* -> u.dblk[.] */
X    register struct dirent *bp;	/* -> buf[.] */
X
X#ifdef UNK
X    switch (state) {
X	SIG_T         (*shdlr)();	/* entry SIGSYS handler */
X	register int    retval;		/* return from _getdents() if any */
X
X    case yes:			/* _getdents() is known to work */
X	return _getdents(fildes, buf, nbyte);
X
X    case maybe:		/* first time only */
X	shdlr = signal(SIGSYS, sig_catch);
X	retval = _getdents(fildes, buf, nbyte);	/* try it */
X	signal(SIGSYS, shdlr);
X
X	if (state == maybe) {	/* SIGSYS did not occur */
X	    state = yes;	/* so _getdents() must have worked */
X	    return retval;
X	}
X	/* else fall through into emulation */
X
X/*	case no:	/* fall through into emulation */
X    }
X#endif
X
X    if (buf == NULL
X#ifdef ATT_SPEC
X	|| (unsigned long) buf % sizeof(long) != 0	/* ugh */
X#endif
X	) {
X	errno = EFAULT;		/* invalid pointer */
X	return -1;
X    }
X    if (fstat(fildes, &statb) != 0) {
X	return -1;		/* errno set by fstat() */
X    }
X
X    if (!S_ISDIR(statb.st_mode)) {
X	errno = ENOTDIR;	/* not a directory */
X	return -1;
X    }
X    if ((offset = lseek(fildes, (OFFSET) 0, SEEK_CUR)) < 0) {
X	return -1;		/* errno set by lseek() */
X    }
X
X#ifdef BFS			/* no telling what remote hosts do */
X    if ((unsigned long) offset % DIRBLKSIZ != 0) {
X	errno = ENOENT;		/* file pointer probably misaligned */
X	return -1;
X    }
X#endif
X
X    serrno = errno;		/* save entry errno */
X
X    for (bp = (struct dirent *) buf; bp == (struct dirent *) buf;) {	
X
X    	/* convert next directory block */
X	int             size;
X
X	do {
X	    size = GetBlock(fildes, u.dblk, DIRBLKSIZ);
X	} while (size == -1 && errno == EINTR);
X
X	if (size <= 0) {
X	    return size;	/* EOF or error (EBADF) */
X	}
X
X	for (dp = (struct direct *) u.dblk;
X	     (char *) dp < &u.dblk[size];
X	     dp = (struct direct *) ((char *) dp + RecLen(dp))
X	    ) {
X#ifndef UFS
X	    if (dp->d_reclen <= 0) {
X		errno = EIO;	/* corrupted directory */
X		return -1;
X	    }
X#endif
X
X	    if (dp->d_fileno != 0) {	/* non-empty; copy to user buffer */
X		register int    reclen =
X		DIRENTSIZ(NameLen(dp->d_name));
X
X		if ((char *) bp + reclen > &buf[nbyte]) {
X		    errno = EINVAL;
X		    return -1;	/* buf too small */
X		}
X		bp->d_ino = dp->d_fileno;
X		bp->d_off = offset + ((char *) dp - u.dblk);
X		bp->d_reclen = reclen;
X
X		{
X#ifdef UFS
X		    /* Is the following kludge ugly?  You bet. */
X
X		    register char   save = dp->d_name[DIRSIZ];
X		    /* save original data */
X
X		    dp->d_name[DIRSIZ] = '\0';
X		    /* ensure NUL termination */
X#endif
X		    /* adds NUL padding */
X		    strncpy(bp->d_name, dp->d_name, reclen - DIRENTBASESIZ);
X#ifdef UFS
X		    dp->d_name[DIRSIZ] = save;
X		    /* restore original data */
X#endif
X		}
X
X		bp = (struct dirent *) ((char *) bp + reclen);
X	    }
X	}
X
X#ifndef BFS			/* 4.2BSD screwed up; fixed in 4.3BSD */
X	if ((char *) dp > &u.dblk[size]) {
X	    errno = EIO;	/* corrupted directory */
X	    return -1;
X	}
X#endif
X    }
X
X    errno = serrno;		/* restore entry errno */
X    return (char *) bp - buf;	/* return # bytes read */
X}
END_OF_paxdir.c
if test 16936 -ne `wc -c <paxdir.c`; then
    echo shar: \"paxdir.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
echo shar: End of archive 5 \(of 6\).
cp /dev/null ark5isdone
MISSING=""
for I in 1 2 3 4 5 6 ; do
    if test ! -f ark${I}isdone ; then
	MISSING="${MISSING} ${I}"
    fi
done
if test "${MISSING}" = "" ; then
    echo You have unpacked all 6 archives.
    rm -f ark[1-9]isdone
else
    echo You still need to unpack the following archives:
    echo "        " ${MISSING}
fi
##  End of shell archive.
exit 0

