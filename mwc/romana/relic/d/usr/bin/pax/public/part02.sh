# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 2 (of 6)."
# Contents:  cpio.1 create.c func.h link.c names.c replace.c ttyio.c
#   warn.c
# Wrapped by mark@jhereg on Tue Dec 27 19:37:38 1988
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f cpio.1 -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"cpio.1\"
else
echo shar: Extracting \"cpio.1\" \(6231 characters\)
sed "s/^X//" >cpio.1 <<'END_OF_cpio.1'
X.\" $Id: cpio.1,v 1.1 88/12/23 18:02:04 mark Rel $
X.TH CPIO 1 "USENIX Association" ""
X.SH NAME
Xcpio \- copy file archives in and out
X.SH SYNOPSIS
X.B cpio
X.BR \-o [ Bacv ]
X.br
X.B cpio
X.BR \-i [ Bcdfmrtuv ]
X.RI [ pattern... ]
X.br
X.B cpio
X.BR \-p [ adlmruv ]
X.I directory
X.SH DESCRIPTION
XThe
X.B cpio
Xutility produces and reads files in the format specified by the
X.B cpio
X.B "Archive/Interchange File Format"
Xspecified in
X.IR "IEEE Std. 1003.1-1988" .
X.PP
XThe
X.B "cpio -i"
X(copy in) utility extracts files from the standard input, which is
Xassumed to be the product of a previous
X.B "cpio -o" .
XOnly files with names that match
X.I patterns
Xare selected.
XMultiple
X.I patterns
Xmay be specified and if no
X.I patterns
Xare specified, the default for
X.I patterns
Xis \*, selecting all files.
XThe extracted files are conditionally created and copied into the
Xcurrent directory, and possibly any levels below, based upon the
Xoptions described below and the permissions of the files will be those
Xof the previous
X.B "cpio -o" .
XThe owner and group of the files will be that of the current user
Xunless the user has appropriate privileges, which causes
X.B cpio
Xto retains the owner and group of the files of the previous
X.B "cpio -o" .
X.PP
XThe 
X.B "cpio -p"
X(pass) utility reads the standard input to obtain a list of path names
Xof files that are conditionally created and copied into the
Xdestination
X.I directory 
Xbased upon the options described below.
X.PP
XIf an error is detected, the cause is reported and the
X.B cpio
Xutility will continue to copy other files.
X.B cpio
Xwill skip over any unrecognized files which it encounters in the archive.
X.PP
XThe following restrictions apply to the 
X.B cpio
Xutility:
X.IP 1 .25i
XPathnames are restricted to 256 characters.
X.IP 2 .25i
XAppropriate privileges are required to copy special files.
X.IP 3 .25i
XBlocks are reported in 512-byte quantities.
X.SS Options
XThe following options are available:
X.TP .5i
X.B \-B
XInput/output is to be blocked 5120 bytes to the record.
XCan only be used with
X.B "cpio -o" 
Xor
X.B "cpio -i"
Xfor data that is directed to or from character special files.
X.TP .5i
X.B \-a
XReset access times of input files after they have been copied.
XWhen the
X.B \-l
Xoption is also specified, the linked files do not have their access
Xtimes reset.
XCan only be used with
X.B "cpio -o"
Xor
X.B "cpio -i" .
X.TP .5i
X.B \-c
XWrite header information in ASCII character for for portability.
XCan only be used with
X.B "cpio -i"
Xor
X.B "cpio -o" .
XNote that this option should always be used to write portable files.
X.TP .5i
X.B \-d
XCreates directories as needed.
XCan only be used with 
X.B "cpio -i" 
Xor
X.B "cpio -p" .
X.TP .5i
X.B \-f
XCopy in all files except those in
X.I patterns .
XCan only be used with
X.B "cpio -i" .
X.TP .5i
X.B \-l
XWhenever possible, link files rather than copying them.
XCan only be used with 
X.B "cpio -p" .
X.TP .5i
X.B \-m
XRetain previous modification times.
XThis option is ineffective on directories that are being copied.
XCan only be used with
X.B "cpio -i" 
Xor
X.B "cpio -p" .
X.TP .5i
X.B \-r
XInteractively rename files.
XThe user is asked whether to rename
X.I pattern
Xeach invocation.
XRead and write permissions for
X.B "/dev/tty"
Xare required for this option.
XIf the user types a null line, the file is skipped.
XShould only be used with
X.B "cpio -i"
Xor
X.B "cpio -o" .
X.TP .5i
X.B \-t
XPrint a table of contents of the input.
XNo files are created.
XCan only be used with
X.B "cpio -i" .
X.TP .5i
X.B \-u
XCopy files unconditionally; usually an older file will not replace a
Xnew file with the same name.
XCan only be used with
X.B "cpio -i"
Xor
X.B "cpio -p" .
X.TP .5i
X.B \-v
XVerbose: cause the names of the affected files to be printed.
XCan only be used with
X.B "cpio -i" .
XProvides a detailed listing when used with the 
X.B \-t
Xoption.
X.SS Operands
XThe following operands are available:
X.TP 1i
X.I patterns
XSimple regular expressions given in the name-generating notation of the
Xshell.
X.TP 1i
X.I directory
XThe destination directory.
X.SS "Exit Status"
XThe
X.B cpio
Xutility exits with one of the following values:
X.TP .5i
X0
XAll input files were copied.
X.TP .5i
X2
XThe utility encountered errors in copying or accessing files or
Xdirectories.
XAn error will be reported for nonexistent files or directories, or
Xpermissions that do not allow the user to access the source or target
Xfiles.
X.SS
XIt is important to use the
X.B "-depth"
Xoption of the
X.B find
Xutility to generate pathnames for 
X.B cpio .
XThis eliminates problems
X.B cpio
Xcould have trying to create files under read-only directories.
X.PP
XThe following command:
X.RS
Xls | cpio -o > ../newfile
X.RE
Xcopies out the files listed by the 
X.B ls
Xutility and redirects them to the file
X.B newfile .
X.PP
XThe following command:
X.RS
Xcat newfile | cpio -id "memo/al" "memo/b*"
X.RE
Xuses the output file
X.B newfile
Xfrom the
X.B "cpio -o"
Xutility, takes those files that match the patterns
X.B "memo/al"
Xand
X.B "memo/b*" ,
Xcreates the directories below the current directory, and places the
Xfiles in the appropriate directories.
X.PP
XThe command
X.RS
Xfind . -depth -print | cpio -pdlmv newdir
X.RE
Xtakes the file names piped to it from the
X.B find
Xutility and copies or links those files to another directory
Xnamed
X.B newdir ,
Xwhile retaining the modification time.
X.SH FILES
X.TP 1i
X/dev/tty
Xused to prompt the user for information when the
X.B \-i 
Xor 
X.B \-r
Xoptions are specified.
X.SH "SEE ALSO"
Xfind(1), pax(1), tar(1), cpio(5), tar(5)
X.SH COPYRIGHT
XCopyright (c) 1989 Mark H. Colburn.  
X.br
XAll rights reserved.
X.PP
XRedistribution and use in source and binary forms are permitted
Xprovided that the above copyright notice is duplicated in all such 
Xforms and that any documentation, advertising materials, and other 
Xmaterials related to such distribution and use acknowledge that the 
Xsoftware was developed by Mark H. Colburn and sponsored by The 
XUSENIX Association. 
X.PP
XTHE SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
XIMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
XWARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
X.SH AUTHOR
XMark H. Colburn
X.br
XNAPS International
X.br
X117 Mackubin Street, Suite 1
X.br
XSt. Paul, MN 55102
X.br
Xmark@jhereg.MN.ORG
X.sp 2
XSponsored by
X.B "The USENIX Association"
Xfor public distribution.
END_OF_cpio.1
if test 6231 -ne `wc -c <cpio.1`; then
    echo shar: \"cpio.1\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f create.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"create.c\"
else
echo shar: Extracting \"create.c\" \(7830 characters\)
sed "s/^X//" >create.c <<'END_OF_create.c'
X/* $Source: /u/mark/src/pax/RCS/create.c,v $
X *
X * $Revision: 1.1 $
X *
X * create.c - Create a tape archive. 
X *
X * DESCRIPTION
X *
X *	These functions are used to create/write and archive from an set of
X *	named files.
X *
X * AUTHOR
X *
X *     	Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
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
X * $Log:	create.c,v $
X * Revision 1.1  88/12/23  18:02:06  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: create.c,v 1.1 88/12/23 18:02:06 mark Rel $";
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
Xstatic void writetar(char *, Stat *);
Xstatic void writecpio(char *, Stat *);
Xstatic char tartype(int);
X
X#else /* !__STDC__ */
X
Xstatic void writetar();
Xstatic void writecpio();
Xstatic char tartype();
X
X#endif /* __STDC__ */
X
X
X/* create_archive - create a tar archive.
X *
X * DESCRIPTION
X *
X *	Create_archive is used as an entry point to both create and append
X *	archives.  Create archive goes through the files specified by the
X *	user and writes each one to the archive if it can.  Create_archive
X *	knows how to write both cpio and tar headers and the padding which
X *	is needed for each type of archive.
X *
X * RETURNS
X *
X *	Always returns 0
X */
X
X#ifdef __STDC__
X
Xint create_archive(void)
X
X#else
X
Xint create_archive()
X
X#endif
X{
X    char            name[PATH_MAX + 1];
X    Stat            sb;
X    int             fd;
X
X    while (name_next(name, &sb) != -1) {
X	if ((fd = openi(name, &sb)) < 0) {
X	    /* FIXME: pax wants to exit here??? */
X	    continue;
X	}
X
X	if (rplhead != NULL) {
X	    rpl_name(name);
X	    if (strlen(name) == 0) {
X		continue;
X	    }
X	}
X	if (get_disposition("add", name) || get_newname(name, sizeof(name))) {
X	    /* skip file... */
X	    if (fd) {
X		close(fd);
X	    }
X	    continue;
X	} 
X
X	if (!f_link && sb.sb_nlink > 1) {
X	    if (islink(name, &sb)) {
X		sb.sb_size = 0;
X	    }
X	    linkto(name, &sb);
X	}
X	if (ar_format == TAR) {
X	    writetar(name, &sb);
X	} else {
X	    writecpio(name, &sb);
X	}
X	if (fd) {
X	    outdata(fd, name, sb.sb_size);
X	}
X	if (f_verbose) {
X	    print_entry(name, &sb);
X	}
X    }
X
X    write_eot();
X    close_archive();
X    return (0);
X}
X
X
X/* writetar - write a header block for a tar file
X *
X * DESCRIPTION
X *
X * 	Make a header block for the file name whose stat info is in st.  
X *	Return header pointer for success, NULL if the name is too long.
X *
X * 	The tar header block is structured as follows:
X *
X *		FIELD NAME	OFFSET		SIZE
X *      	-------------|---------------|------
X *		name		  0		100
X *		mode		100		  8
X *		uid		108		  8
X *		gid		116		  8
X *		size		124		 12
X *		mtime		136		 12
X *		chksum		148		  8
X *		typeflag	156		  1
X *		linkname	157		100
X *		magic		257		  6
X *		version		263		  2
X *		uname		265		 32
X *		gname		297		 32
X *		devmajor	329		  8
X *		devminor	337		  8
X *		prefix		345		155
X *
X * PARAMETERS
X *
X *	char	*name	- name of file to create a header block for
X *	Stat	*asb	- pointer to the stat structure for the named file
X *
X */
X
X#ifdef __STDC__
X
Xstatic void writetar(char *name, Stat *asb)
X
X#else
X    
Xstatic void writetar(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    char	   *p;
X    char           *prefix = NULL;
X    int             i;
X    int             sum;
X    char            hdr[BLOCKSIZE];
X    Link           *from;
X
X    memset(hdr, 0, BLOCKSIZE);
X    if (strlen(name) > 255) {
X	warn(name, "name too long");
X	return;
X    }
X
X    /* 
X     * If the pathname is longer than TNAMLEN, but less than 255, then
X     * we can split it up into the prefix and the filename.
X     */
X    if (strlen(name) > 100) {
X	prefix = name;
X	name += 155;
X	while (name > prefix && *name != '/') {
X	    name--;
X	}
X
X	/* no slash found....hmmm.... */
X	if (name == prefix) {
X	    warn(prefix, "Name too long");
X	    return;
X	}
X	*name++ = '\0';
X    }
X
X#ifdef S_IFLNK
X    if ((asb->sb_mode & S_IFMT) == S_IFLNK) {
X	strcpy(&hdr[157], asb->sb_link);
X	asb->sb_size = 0;
X    }
X#endif
X    strcpy(hdr, name);
X    sprintf(&hdr[100], "%06o \0", asb->sb_mode & ~S_IFMT);
X    sprintf(&hdr[108], "%06o \0", asb->sb_uid);
X    sprintf(&hdr[116], "%06o \0", asb->sb_gid);
X    sprintf(&hdr[124], "%011lo ", (long) asb->sb_size);
X    sprintf(&hdr[136], "%011lo ", (long) asb->sb_mtime);
X    strncpy(&hdr[148], "        ", 8);
X    hdr[156] = tartype(asb->sb_mode);
X    if (asb->sb_nlink > 1 && (from = linkfrom(name, asb)) != NULL) {
X	strcpy(&hdr[157], from->l_name);
X	hdr[156] = LNKTYPE;
X    }
X    strcpy(&hdr[257], TMAGIC);
X    strncpy(&hdr[263], TVERSION, 2);
X    strcpy(&hdr[265], finduname((int) asb->sb_uid));
X    strcpy(&hdr[297], findgname((int) asb->sb_gid));
X    sprintf(&hdr[329], "%06o \0", major(asb->sb_rdev));
X    sprintf(&hdr[337], "%06o \0", minor(asb->sb_rdev));
X    if (prefix != NULL) {
X	strncpy(&hdr[345], prefix, 155);
X    }
X
X    /* Calculate the checksum */
X
X    sum = 0;
X    p = hdr;
X    for (i = 0; i < 500; i++) {
X	sum += 0xFF & *p++;
X    }
X
X    /* Fill in the checksum field. */
X
X    sprintf(&hdr[148], "%06o \0", sum);
X
X    outwrite(hdr, BLOCKSIZE);
X}
X
X
X/* tartype - return tar file type from file mode
X *
X * DESCRIPTION
X *
X *	tartype returns the character which represents the type of file
X *	indicated by "mode". 
X *
X * PARAMETERS
X *
X *	int	mode	- file mode from a stat block
X *
X * RETURNS
X *
X *	The character which represents the particular file type in the 
X *	ustar standard headers.
X */
X
X#ifdef __STDC__
X
Xstatic char tartype(int mode)
X
X#else
X    
Xstatic char tartype(mode)
Xint	    mode;
X
X#endif
X{
X    switch (mode & S_IFMT) {
X
X#ifdef S_IFCTG
X    case S_IFCTG:
X	return(CONTTYPE);
X#endif
X
X    case S_IFDIR:
X	return (DIRTYPE);
X
X#ifdef S_IFLNK
X    case S_IFLNK:
X	return (SYMTYPE);
X#endif
X
X#ifdef S_IFFIFO
X    case S_IFIFO:
X	return (FIFOTYPE);
X#endif
X
X#ifdef S_IFCHR
X    case S_IFCHR:
X	return (CHRTYPE);
X#endif
X
X#ifdef S_IFBLK
X    case S_IFBLK:
X	return (BLKTYPE);
X#endif
X
X    default:
X	return (REGTYPE);
X    }
X}
X
X
X/* writecpio - write a cpio archive header
X *
X * DESCRIPTION
X *
X *	Writes a new CPIO style archive header for the file specified.
X *
X * PARAMETERS
X *
X *	char	*name	- name of file to create a header block for
X *	Stat	*asb	- pointer to the stat structure for the named file
X */
X
X#ifdef __STDC__
X
Xstatic void writecpio(char *name, Stat *asb)
X
X#else
X    
Xstatic void writecpio(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    uint            namelen;
X    char            header[M_STRLEN + H_STRLEN + 1];
X
X    namelen = (uint) strlen(name) + 1;
X    strcpy(header, M_ASCII);
X    sprintf(header + M_STRLEN, "%06o%06o%06o%06o%06o",
X	    USH(asb->sb_dev), USH(asb->sb_ino), USH(asb->sb_mode), 
X	    USH(asb->sb_uid), USH(asb->sb_gid));
X    sprintf(header + M_STRLEN + 30, "%06o%06o%011lo%06o%011lo",
X	    USH(asb->sb_nlink), USH(asb->sb_rdev),
X	    f_modification_time ? asb->sb_mtime : time((time_t *) 0),
X	    namelen, asb->sb_size);
X    outwrite(header, M_STRLEN + H_STRLEN);
X    outwrite(name, namelen);
X#ifdef	S_IFLNK
X    if ((asb->sb_mode & S_IFMT) == S_IFLNK) {
X	outwrite(asb->sb_link, (uint) asb->sb_size);
X    }
X#endif	/* S_IFLNK */
X}
END_OF_create.c
if test 7830 -ne `wc -c <create.c`; then
    echo shar: \"create.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f func.h -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"func.h\"
else
echo shar: Extracting \"func.h\" \(5047 characters\)
sed "s/^X//" >func.h <<'END_OF_func.h'
X/* $Source: /u/mark/src/pax/RCS/func.h,v $
X *
X * $Revision: 1.1 $
X *
X * func.h - function type and argument declarations
X *
X * DESCRIPTION
X *
X *	This file contains function delcarations in both ANSI style
X *	(function prototypes) and traditional style. 
X *
X * AUTHOR
X *
X *     Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
X *
X * Sponsored by The USENIX Association for public distribution. 
X *
X * Copyright (c) 1989 Mark H. Colburn.
X * All rights reserved.
X *
X * Redistribution and use in source and binary forms are permitted
X * provided that the above copyright notice and this paragraph are
X * duplicated in all such forms and that any documentation,
X * advertising materials, and other materials related to such
X * distribution and use acknowledge that the software was developed
X * by Mark H. Colburn and sponsored by The USENIX Association. 
X *
X * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
X * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
X * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
X */
X
X#ifndef _PAX_FUNC_H
X#define _PAX_FUNC_H
X
X/* Function Prototypes */
X
X#ifdef __STDC__
X
Xextern Link    	       *linkfrom(char *, Stat *);
Xextern Link    	       *linkto(char *, Stat *);
Xextern char    	       *mem_get(uint);
Xextern char    	       *mem_str(char *);
Xextern char    	       *syserr(void);
Xextern int      	ar_read(void);
Xextern int      	buf_read(char *, uint);
Xextern int      	buf_skip(OFFSET);
Xextern int      	create_archive(void);
Xextern int      	dirneed(char *);
Xextern int      	read_archive(void);
Xextern int      	inentry(char *, Stat *);
Xextern int      	lineget(FILE *, char *);
Xextern int      	name_match(char *);
Xextern int      	name_next(char *, Stat *);
Xextern int      	nameopt(char *);
Xextern int      	open_archive(int);
Xextern int      	open_tty(void);
Xextern int      	openi(char *, Stat *);
Xextern int      	openo(char *, Stat *, Link *, int);
Xextern int      	pass(char *);
Xextern int      	passitem(char *, Stat *, int, char *);
Xextern int      	read_header(char *, Stat *);
Xextern int      	wildmat(char *, char *);
Xextern void     	buf_allocate(OFFSET);
Xextern void     	close_archive(void);
Xextern void     	fatal(char *);
Xextern void     	name_gather(void);
Xextern void     	name_init(int, char **);
Xextern void     	names_notfound(void);
Xextern void     	next(int);
Xextern int      	nextask(char *, char *, int);
Xextern void     	outdata(int, char *, OFFSET);
Xextern void     	outwrite(char *, uint);
Xextern void     	passdata(char *, int, char *, int);
Xextern void     	print_entry(char *, Stat *);
Xextern void     	warn();
Xextern void		warnarch(char *, OFFSET);
Xextern void     	write_eot(void);
Xextern void		get_archive_type(void);
Xextern struct group    *getgrgid();
Xextern struct group    *getgrnam();
Xextern struct passwd   *getpwuid();
Xextern char    	       *getenv(char *);
Xextern SIG_T   	      (*signal())();
Xextern Link            *islink(char *, Stat *);
Xextern char            *finduname(int);
Xextern char            *findgname(int);
Xextern int		findgid(char *gname);
X
X#else /* !__STDC__ */
X
Xextern Link    	       *linkfrom();
Xextern Link    	       *linkto();
Xextern char    	       *mem_get();
Xextern char    	       *mem_str();
Xextern char    	       *syserr();
Xextern int      	ar_read();
Xextern int      	buf_read();
Xextern int      	buf_skip();
Xextern int      	create_archive();
Xextern int      	dirneed();
Xextern int      	read_archive();
Xextern int      	inentry();
Xextern int      	lineget();
Xextern int      	name_match();
Xextern int      	name_next();
Xextern int      	nameopt();
Xextern int      	open_archive();
Xextern int      	open_tty();
Xextern int      	openi();
Xextern int      	openo();
Xextern int      	pass();
Xextern int      	passitem();
Xextern int     	 	read_header();
Xextern int      	wildmat();
Xextern void     	buf_allocate();
Xextern void     	close_archive();
Xextern void     	fatal();
Xextern void     	name_gather();
Xextern void     	name_init();
Xextern void     	names_notfound();
Xextern void     	next();
Xextern int      	nextask();
Xextern void     	outdata();
Xextern void     	outwrite();
Xextern void     	passdata();
Xextern void     	print_entry();
Xextern void     	warn();
Xextern void     	warnarch();
Xextern void     	write_eot();
Xextern void		get_archive_type();
Xextern char    	       *getenv();
Xextern char    	       *malloc();
Xextern char    	       *strcat();
Xextern char    	       *strcpy();
Xextern char    	       *strncpy();
Xextern SIG_T   	      (*signal())();
Xextern OFFSET   	lseek();
Xextern struct group    *getgrgid();
Xextern struct group    *getgrnam();
Xextern struct passwd   *getpwuid();
Xextern struct tm       *localtime();
Xextern time_t          	time();
Xextern uint            	sleep();
Xextern void            	_exit();
Xextern void            	exit();
Xextern void            	free();
Xextern Link            *islink();
Xextern char            *finduname();
Xextern char            *findgname();
Xextern int		findgid();
X
X#endif /* __STDC__ */
X#endif /* _PAX_FUNC_H */
END_OF_func.h
if test 5047 -ne `wc -c <func.h`; then
    echo shar: \"func.h\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f link.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"link.c\"
else
echo shar: Extracting \"link.c\" \(7575 characters\)
sed "s/^X//" >link.c <<'END_OF_link.c'
X/* $Source: /u/mark/src/pax/RCS/link.c,v $
X *
X * $Revision: 1.1 $
X *
X * link.c - functions for handling multiple file links 
X *
X * DESCRIPTION
X *
X *	These function manage the link chains which are used to keep track
X *	of outstanding links during archive reading and writing.
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
X * $Log:	link.c,v $
X * Revision 1.1  88/12/23  18:02:12  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: link.c,v 1.1 88/12/23 18:02:12 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* Defines */
X
X/*
X * Address link information base. 
X */
X#define	LINKHASH(ino)	(linkbase + (ino) % NEL(linkbase))
X
X/*
X * Number of array elements. 
X */
X#define	NEL(a)		(sizeof(a) / sizeof(*(a)))
X
X
X
X/* Internal Identifiers */
X
Xstatic Link    *linkbase[256];	/* Unresolved link information */
X
X
X/* linkfrom - find a file to link from 
X *
X * DESCRIPTION
X *
X *	Linkfrom searches the link chain to see if there is a file in the
X *	link chain which has the same inode number as the file specified
X *	by the stat block pointed at by asb.  If a file is found, the
X *	name is returned to the caller, otherwise a NULL is returned.
X *
X * PARAMETERS
X *
X *	char    *name   - name of the file which we are attempting
X *                        to find a link for
X *	Stat	*asb	- stat structure of file to find a link to
X *
X * RETURNS
X *
X * 	Returns a pointer to a link structure, or NULL if unsuccessful. 
X *
X */
X
X#ifdef __STDC__
X
XLink *linkfrom(char *name, Stat *asb)
X
X#else
X    
XLink *linkfrom(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    Link           *linkp;
X    Link           *linknext;
X    Path           *path;
X    Path           *pathnext;
X    Link          **abase;
X
X    for (linkp = *(abase = LINKHASH(asb->sb_ino)); linkp; linkp = linknext) {
X	if (linkp->l_nlink == 0) {
X	    if (linkp->l_name) {
X		free((char *) linkp->l_name);
X	    }
X	    if (linknext = linkp->l_forw) {
X		linknext->l_back = linkp->l_back;
X	    }
X	    if (linkp->l_back) {
X		linkp->l_back->l_forw = linkp->l_forw;
X	    }
X	    free((char *) linkp);
X	    *abase = (Link *) NULL;
X	} else if (linkp->l_ino == asb->sb_ino && linkp->l_dev == asb->sb_dev) {
X	    /* 
X	     * check to see if a file with the name "name" exists in the 
X	     * chain of files which we have for this particular link
X	     */
X	    for (path = linkp->l_path; path; path = pathnext) {
X		if (strcmp(path->p_name, name) == 0) {
X		    --linkp->l_nlink;
X		    if (path->p_name) {
X			free(path->p_name);
X		    }
X		    if (pathnext = path->p_forw) {
X			pathnext->p_back = path->p_back;
X		    }
X		    if (path->p_back) {
X			path->p_back->p_forw = pathnext;
X		    }
X		    if (linkp->l_path == path) {
X			linkp->l_path = pathnext;
X		    }
X		    free(path);
X		    return (linkp);
X		}
X		pathnext = path->p_forw;
X	    }
X	    return((Link *)NULL);
X	} else {
X	    linknext = linkp->l_forw;
X	}
X    }
X    return ((Link *)NULL);
X}
X
X
X
X/* islink - determine whether a given file really a link
X *
X * DESCRIPTION
X *
X *	Islink searches the link chain to see if there is a file in the
X *	link chain which has the same inode number as the file specified
X *	by the stat block pointed at by asb.  If a file is found, a
X *	non-zero value is returned to the caller, otherwise a 0 is
X *	returned.
X *
X * PARAMETERS
X *
X *	char    *name   - name of file to check to see if it is link.
X *	Stat	*asb	- stat structure of file to find a link to
X *
X * RETURNS
X *
X * 	Returns a pointer to a link structure, or NULL if unsuccessful. 
X *
X */
X
X#ifdef __STDC__
X
XLink *islink(char *name, Stat *asb)
X
X#else
X    
XLink *islink(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    Link           *linkp;
X    Link           *linknext;
X
X    for (linkp = *(LINKHASH(asb->sb_ino)); linkp; linkp = linknext) {
X	if (linkp->l_ino == asb->sb_ino && linkp->l_dev == asb->sb_dev) {
X	    if (strcmp(name, linkp->l_name) == 0) {
X		return ((Link *) NULL);
X	    } 
X	    return (linkp);
X	} else {
X	    linknext = linkp->l_forw;
X	}
X    }
X    return ((Link *)NULL);
X}
X
X
X/* linkto  - remember a file with outstanding links 
X *
X * DESCRIPTION
X *
X *	Linkto adds the specified file to the link chain.  Any subsequent
X *	calls to linkfrom which have the same inode will match the file
X *	just entered.  If not enough space is available to make the link
X *	then the item is not added to the link chain, and a NULL is
X *	returned to the calling function.
X *
X * PARAMETERS
X *
X *	char	*name	- name of file to remember
X *	Stat	*asb	- pointer to stat structure of file to remember
X *
X * RETURNS
X *
X * 	Returns a pointer to the associated link structure, or NULL when 
X *	linking is not possible. 
X *
X */
X
X#ifdef __STDC__
X
XLink *linkto(char *name, Stat *asb)
X
X#else
X    
XLink *linkto(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    Link           *linkp;
X    Link           *linknext;
X    Path           *path;
X    Link          **abase;
X
X    for (linkp = *(LINKHASH(asb->sb_ino)); linkp; linkp = linknext) {
X	if (linkp->l_ino == asb->sb_ino && linkp->l_dev == asb->sb_dev) {
X	    if ((path = (Path *) mem_get(sizeof(Path))) == (Path *) NULL || 
X		(path->p_name = mem_str(name)) == (char *) NULL) {
X		return((Link *) NULL);
X	    }
X	    if (path->p_forw = linkp->l_path) {
X		if (linkp->l_path->p_forw) {
X		    linkp->l_path->p_forw->p_back = path;
X		}
X	    } else {
X		linkp->l_path = path;
X	    }
X	    path->p_back = (Path *) NULL;
X	    return(linkp);
X	} else {
X	    linknext = linkp->l_forw;
X	}
X    }
X    /*
X     * This is a brand new link, for which there is no other information
X     */
X
X    if ((asb->sb_mode & S_IFMT) == S_IFDIR
X	|| (linkp = (Link *) mem_get(sizeof(Link))) == NULL
X	|| (linkp->l_name = mem_str(name)) == NULL) {
X	return ((Link *) NULL);
X    }
X    linkp->l_dev = asb->sb_dev;
X    linkp->l_ino = asb->sb_ino;
X    linkp->l_nlink = asb->sb_nlink - 1;
X    linkp->l_size = asb->sb_size;
X    linkp->l_path = (Path *) NULL;
X    if (linkp->l_forw = *(abase = LINKHASH(asb->sb_ino))) {
X	linkp->l_forw->l_back = linkp;
X    } else {
X	*abase = linkp;
X    }
X    linkp->l_back = NULL;
X    return (linkp);
X}
X
X
X/* linkleft - complain about files with unseen links 
X *
X * DESCRIPTION
X *
X *	Linksleft scans through the link chain to see if there were any
X *	files which have outstanding links that were not processed by the
X *	archive.  For each file in the link chain for which there was not
X *	a file,  and error message is printed.
X */
X
X#ifdef __STDC__
X
Xvoid linkleft(void)
X
X#else
X    
Xvoid linkleft()
X
X#endif
X{
X    Link           *lp;
X    Link          **base;
X
X    for (base = linkbase; base < linkbase + NEL(linkbase); ++base) {
X	for (lp = *base; lp; lp = lp->l_forw) {
X	    if (lp->l_nlink) {
X		warn(lp->l_path->p_name, "Unseen link(s)");
X	    }
X	}
X    }
X}
END_OF_link.c
if test 7575 -ne `wc -c <link.c`; then
    echo shar: \"link.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f names.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"names.c\"
else
echo shar: Extracting \"names.c\" \(5005 characters\)
sed "s/^X//" >names.c <<'END_OF_names.c'
X/* $Source: /u/mark/src/pax/RCS/names.c,v $
X *
X * $Revision: 1.1 $
X *
X * names.c - Look up user and/or group names. 
X *
X * DESCRIPTION
X *
X *	These functions support UID and GID name lookup.  The results are
X *	cached to improve performance.
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
X * $Log:	names.c,v $
X * Revision 1.1  88/12/23  18:02:19  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: names.c,v 1.1 88/12/23 18:02:19 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* Defines */
X
X#define myuid	( my_uid < 0? (my_uid = getuid()): my_uid )
X#define	mygid	( my_gid < 0? (my_gid = getgid()): my_gid )
X
X
X/* Internal Identifiers */
X
Xstatic int      saveuid = -993;
Xstatic char     saveuname[TUNMLEN];
Xstatic int      my_uid = -993;
X
Xstatic int      savegid = -993;
Xstatic char     savegname[TGNMLEN];
Xstatic int      my_gid = -993;
X
X
X/* finduname - find a user or group name from a uid or gid
X *
X * DESCRIPTION
X *
X * 	Look up a user name from a uid/gid, maintaining a cache. 
X *
X * PARAMETERS
X *
X *	char	uname[]		- name (to be returned to user)
X *	int	uuid		- id of name to find
X *
X *
X * RETURNS
X *
X *	Returns a name which is associated with the user id given.  If there
X *	is not name which corresponds to the user-id given, then a pointer
X *	to a string of zero length is returned.
X *	
X * FIXME
X *
X * 	1. for now it's a one-entry cache. 
X *	2. The "-993" is to reduce the chance of a hit on the first lookup. 
X */
X
X#ifdef __STDC__
X
Xchar *finduname(int uuid)
X
X#else
X    
Xchar *finduname(uuid)
Xint             uuid;
X
X#endif
X{
X    struct passwd  *pw;
X
X    if (uuid != saveuid) {
X	saveuid = uuid;
X	saveuname[0] = '\0';
X	pw = getpwuid(uuid);
X	if (pw) {
X	    strncpy(saveuname, pw->pw_name, TUNMLEN);
X	}
X    }
X    return(saveuname);
X}
X
X
X/* finduid - get the uid for a given user name
X *
X * DESCRIPTION
X *
X *	This does just the opposit of finduname.  Given a user name it
X *	finds the corresponding UID for that name.
X *
X * PARAMETERS
X *
X *	char	uname[]		- username to find a UID for
X *
X * RETURNS
X *
X *	The UID which corresponds to the uname given, if any.  If no UID
X *	could be found, then the UID which corrsponds the user running the
X *	program is returned.
X *
X */
X
X#ifdef __STDC__
X
Xint finduid(char *uname)
X
X#else
X    
Xint finduid(uname)
Xchar            *uname;
X
X#endif
X{
X    struct passwd  *pw;
X    extern struct passwd *getpwnam();
X
X    if (uname[0] != saveuname[0]/* Quick test w/o proc call */
X	||0 != strncmp(uname, saveuname, TUNMLEN)) {
X	strncpy(saveuname, uname, TUNMLEN);
X	pw = getpwnam(uname);
X	if (pw) {
X	    saveuid = pw->pw_uid;
X	} else {
X	    saveuid = myuid;
X	}
X    }
X    return (saveuid);
X}
X
X
X/* findgname - look up a group name from a gid
X *
X * DESCRIPTION
X *
X * 	Look up a group name from a gid, maintaining a cache.
X *	
X *
X * PARAMETERS
X *
X *	int	ggid		- goupid of group to find
X *
X * RETURNS
X *
X *	A string which is associated with the group ID given.  If no name
X *	can be found, a string of zero length is returned.
X */
X
X#ifdef __STDC__
X
Xchar *findgname(int ggid)
X
X#else
X    
Xchar *findgname(ggid)
Xint             ggid;
X
X#endif
X{
X    struct group   *gr;
X
X    if (ggid != savegid) {
X	savegid = ggid;
X	savegname[0] = '\0';
X	setgrent();
X	gr = getgrgid(ggid);
X	if (gr) {
X	    strncpy(savegname, gr->gr_name, TGNMLEN);
X	}
X    }
X    return(savegname);
X}
X
X
X
X/* findgid - get the gid for a given group name
X *
X * DESCRIPTION
X *
X *	This does just the opposit of finduname.  Given a group name it
X *	finds the corresponding GID for that name.
X *
X * PARAMETERS
X *
X *	char	uname[]		- groupname to find a GID for
X *
X * RETURNS
X *
X *	The GID which corresponds to the uname given, if any.  If no GID
X *	could be found, then the GID which corrsponds the group running the
X *	program is returned.
X *
X */
X
X#ifdef __STDC__
X
Xint findgid(char *gname)
X
X#else
X    
Xint findgid(gname)
Xchar           *gname;
X
X#endif
X{
X    struct group   *gr;
X
X    /* Quick test w/o proc call */
X    if (gname[0] != savegname[0] || strncmp(gname, savegname, TUNMLEN) != 0) {
X	strncpy(savegname, gname, TUNMLEN);
X	gr = getgrnam(gname);
X	if (gr) {
X	    savegid = gr->gr_gid;
X	} else {
X	    savegid = mygid;
X	}
X    }
X    return (savegid);
X}
END_OF_names.c
if test 5005 -ne `wc -c <names.c`; then
    echo shar: \"names.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f replace.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"replace.c\"
else
echo shar: Extracting \"replace.c\" \(6493 characters\)
sed "s/^X//" >replace.c <<'END_OF_replace.c'
X/* $Source: /u/mark/src/pax/RCS/replace.c,v $
X *
X * $Revision: 1.1 $
X *
X * replace.c - regular expression pattern replacement functions
X *
X * DESCRIPTION
X *
X *	These routines provide for regular expression file name replacement
X *	as required by pax.
X *
X * AUTHORS
X *
X *	Mark H. Colburn, NAPS International
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
X * $Log:	replace.c,v $
X * Revision 1.1  88/12/23  18:02:36  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: replace.c,v 1.1 88/12/23 18:02:36 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* not lint */
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* add_replstr - add a replacement string to the replacement string list
X *
X * DESCRIPTION
X *
X *	Add_replstr adds a replacement string to the replacement string
X *	list which is applied each time a file is about to be processed.
X *
X * PARAMETERS
X *
X *	char	*pattern	- A regular expression which is to be parsed
X */
X
X#ifdef __STDC__
X
Xvoid add_replstr(char *pattern)
X
X#else
X
Xvoid add_replstr(pattern)
Xchar           *pattern;
X
X#endif
X{
X    char           *p;
X    char            sep;
X    Replstr        *rptr;
X    int             len;
X
X    if ((len = strlen(pattern)) < 4) {
X	warn("Replacement string not added",
X		 "Malformed substitution syntax");
X	return;
X    }
X    if ((rptr = (Replstr *) malloc(sizeof(Replstr))) == NULL) {
X	warn("Replacement string not added", "No space");
X	return;
X    }
X
X    /* First character is the delimeter... */
X    sep = *pattern;
X
X    /* Get trailing g and/or p */
X    p = pattern + len - 1;
X    while (*p != sep) {
X	if (*p == 'g') {
X            rptr->global = 1;
X	} else if (*p == 'p') {
X	    rptr->print = 1;
X	} else {
X	    warn(p, "Invalid RE modifier");
X	}
X	p--;
X    }
X
X    if (*p != sep) {
X	warn("Replacement string not added", "Bad delimeters");
X	free(rptr);
X	return;
X    }
X    /* strip off leading and trailing delimeter */
X    *p = '\0';
X    pattern++;
X
X    /* find the separating '/' in the pattern */
X    p = pattern;
X    while (*p) {
X	if (*p == sep) {
X	    break;
X	}
X	if (*p == '\\' && *(p + 1) != '\0') {
X	    p++;
X	}
X	p++;
X    }
X    if (*p != sep) {
X	warn("Replacement string not added", "Bad delimeters");
X	free(rptr);
X	return;
X    }
X    *p++ = '\0';
X
X    /*
X     * Now pattern points to 'old' and p points to 'new' and both are '\0'
X     * terminated 
X     */
X    if ((rptr->comp = regcomp(pattern)) == NULL) {
X	warn("Replacement string not added", "Invalid RE");
X	free(rptr);
X	return;
X    }
X    rptr->replace = p;
X    rptr->next = NULL;
X    if (rplhead == NULL) {
X	rplhead = rptr;
X	rpltail = rptr;
X    } else {
X	rpltail->next = rptr;
X	rpltail = rptr;
X    }
X}
X
X
X
X/* rpl_name - possibly replace a name with a regular expression
X *
X * DESCRIPTION
X *
X *	The string name is searched for in the list of regular expression
X *	substituions.  If the string matches any of the regular expressions
X *	then the string is modified as specified by the user.
X *
X * PARAMETERS
X *
X *	char	*name	- name to search for and possibly modify
X */
X
X#ifdef __STDC__
X
Xvoid rpl_name(char *name)
X
X#else
X
Xvoid rpl_name(name)
Xchar           *name;
X
X#endif
X{
X    int             found = 0;
X    int             ret;
X    Replstr        *rptr;
X    char            buff[PATH_MAX + 1];
X    char            buff1[PATH_MAX + 1];
X    char            buff2[PATH_MAX + 1];
X    char           *p;
X    char           *b;
X
X    strcpy(buff, name);
X    for (rptr = rplhead; !found && rptr != NULL; rptr = rptr->next) {
X	do {
X	    if ((ret = regexec(rptr->comp, buff)) != 0) {
X		p = buff;
X		b = buff1;
X		while (p < rptr->comp->startp[0]) {
X		    *b++ = *p++;
X		}
X		p = rptr->replace;
X		while (*p) {
X		    *b++ = *p++;
X		}
X		strcpy(b, rptr->comp->endp[0]);
X		found = 1;
X		regsub(rptr->comp, buff1, buff2);
X		strcpy(buff, buff2);
X	    }
X	} while (ret && rptr->global);
X	if (found) {
X	    if (rptr->print) {
X		fprintf(stderr, "%s >> %s\n", name, buff);
X	    }
X	    strcpy(name, buff);
X	}
X    }
X}
X
X
X/* get_disposition - get a file disposition
X *
X * DESCRIPTION
X *
X *	Get a file disposition from the user.  If the user enters 'y'
X *	the the file is processed, anything else and the file is ignored.
X *	If the user enters EOF, then the PAX exits with a non-zero return
X *	status.
X *
X * PARAMETERS
X *
X *	char	*mode	- string signifying the action to be taken on file
X *	char	*name	- the name of the file
X *
X * RETURNS
X *
X *	Returns 1 if the file should be processed, 0 if it should not.
X */
X
X#ifdef __STDC__
X
Xint get_disposition(char *mode, char *name)
X
X#else
X
Xint get_disposition(mode, name)
Xchar	*mode;
Xchar	*name;
X
X#endif
X{
X    char	ans[2];
X    char	buf[PATH_MAX + 10];
X
X    if (f_disposition) {
X	sprintf(buf, "%s %s? ", mode, name);
X	if (nextask(buf, ans, sizeof(ans)) == -1 || ans[0] == 'q') {
X	    exit(0);
X	}
X	if (strlen(ans) == 0 || ans[0] != 'y') {
X	    return(1);
X	} 
X    } 
X    return(0);
X}
X
X
X/* get_newname - prompt the user for a new filename
X *
X * DESCRIPTION
X *
X *	The user is prompted with the name of the file which is currently
X *	being processed.  The user may choose to rename the file by
X *	entering the new file name after the prompt; the user may press
X *	carriage-return/newline, which will skip the file or the user may
X *	type an 'EOF' character, which will cause the program to stop.
X *
X * PARAMETERS
X *
X *	char	*name		- filename, possibly modified by user
X *	int	size		- size of allowable new filename
X *
X * RETURNS
X *
X *	Returns 0 if successfull, or -1 if an error occurred.
X *
X */
X
X#ifdef __STDC__
X
Xint get_newname(char *name, int size)
X
X#else
X
Xint get_newname(name, size)
Xchar	*name;
Xint	size;
X
X#endif
X{
X    char	buf[PATH_MAX + 10];
X
X    if (f_interactive) {
X	sprintf(buf, "rename %s? ", name);
X	if (nextask(buf, name, size) == -1) {
X	    exit(0);
X	}
X	if (strlen(name) == 0) {
X	    return(1);
X	}
X    }
X    return(0);
X}
END_OF_replace.c
if test 6493 -ne `wc -c <replace.c`; then
    echo shar: \"replace.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f ttyio.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"ttyio.c\"
else
echo shar: Extracting \"ttyio.c\" \(6321 characters\)
sed "s/^X//" >ttyio.c <<'END_OF_ttyio.c'
X/* $Source: /u/mark/src/pax/RCS/ttyio.c,v $
X *
X * $Revision: 1.1 $
X *
X * ttyio.c - Terminal/Console I/O functions for all archive interfaces
X *
X * DESCRIPTION
X *
X *	These routines provide a consistent, general purpose interface to
X *	the user via the users terminal, if it is available to the
X *	process.
X *
X * AUTHOR
X *
X *     Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
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
X * $Log:	ttyio.c,v $
X * Revision 1.1  88/12/23  18:02:39  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: ttyio.c,v 1.1 88/12/23 18:02:39 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* open_tty - open the terminal for interactive queries
X *
X * DESCRIPTION
X *
X * 	Assumes that background processes ignore interrupts and that the
X *	open() or the isatty() will fail for processes which are not
X *	attached to terminals. Returns a file descriptor or -1 if
X *	unsuccessful. 
X *
X * RETURNS
X *
X *	Returns a file descriptor which can be used to read and write
X *	directly to the user's terminal, or -1 on failure.  
X *
X * ERRORS
X *
X *	If SIGINT cannot be ignored, or the open fails, or the newly opened
X *	terminal device is not a tty, then open_tty will return a -1 to the
X *	caller.
X */
X
X#ifdef __STDC__
X
Xint open_tty(void)
X
X#else
X
Xint open_tty()
X
X#endif
X{
X    int             fd;		/* file descriptor for terminal */
X    SIG_T         (*intr)();	/* used to restore interupts if signal fails */
X
X    if ((intr = signal(SIGINT, SIG_IGN)) == SIG_IGN) {
X	return (-1);
X    }
X    signal(SIGINT, intr);
X    if ((fd = open(TTY, O_RDWR)) < 0) {
X	return (-1);
X    }
X    if (isatty(fd)) {
X	return (fd);
X    }
X    close(fd);
X    return (-1);
X}
X
X
X/* nextask - ask a question and get a response
X *
X * DESCRIPTION
X *
X *	Give the user a prompt and wait for their response.  The prompt,
X *	located in "msg" is printed, then the user is allowed to type
X *	a response to the message.  The first "limit" characters of the
X *	user response is stored in "answer".
X *
X *	Nextask ignores spaces and tabs. 
X *
X * PARAMETERS
X *
X *	char *msg	- Message to display for user 
X *	char *answer	- Pointer to user's response to question 
X *	int limit	- Limit of length for user's response
X *
X * RETURNS
X *
X *	Returns the number of characters in the user response to the 
X *	calling function.  If an EOF was encountered, a -1 is returned to
X *	the calling function.  If an error occured which causes the read
X *	to return with a value of -1, then the function will return a
X *	non-zero return status to the calling process, and abort
X *	execution.
X */
X
X#ifdef __STDC__
X
Xint nextask(char *msg, char *answer, int limit)
X
X#else
X
Xint nextask(msg, answer, limit)
Xchar           *msg;		/* message to display for user */
Xchar           *answer;		/* pointer to user's response to question */
Xint             limit;		/* limit of length for user's response */
X
X#endif
X{
X    int             idx;	/* index into answer for character input */
X    int             got;	/* number of characters read */
X    char            c;		/* character read */
X
X    if (ttyf < 0) {
X	fatal("/dev/tty Unavailable");
X    }
X    write(ttyf, msg, (uint) strlen(msg));
X    idx = 0;
X    while ((got = read(ttyf, &c, 1)) == 1) {
X	if (c == '\n') {
X	    break;
X	} else if (c == ' ' || c == '\t') {
X	    continue;
X	} else if (idx < limit - 1) {
X	    answer[idx++] = c;
X	}
X    }
X    if (got == 0) {		/* got an EOF */
X        return(-1);
X    }
X    if (got < 0) {
X	fatal(syserr());
X    }
X    answer[idx] = '\0';
X    return(0);
X}
X
X
X/* lineget - get a line from a given stream
X *
X * DESCRIPTION
X * 
X *	Get a line of input for the stream named by "stream".  The data on
X *	the stream is put into the buffer "buf".
X *
X * PARAMETERS
X *
X *	FILE *stream		- Stream to get input from 
X *	char *buf		- Buffer to put input into
X *
X * RETURNS
X *
X * 	Returns 0 if successful, -1 at EOF. 
X */
X
X#ifdef __STDC__
X
Xint lineget(FILE *stream, char *buf)
X
X#else
X
Xint lineget(stream, buf)
XFILE           *stream;		/* stream to get input from */
Xchar           *buf;		/* buffer to put input into */
X
X#endif
X{
X    int             c;
X
X    for (;;) {
X	if ((c = getc(stream)) == EOF) {
X	    return (-1);
X	}
X	if (c == '\n') {
X	    break;
X	}
X	*buf++ = c;
X    }
X    *buf = '\0';
X    return (0);
X}
X
X
X/* next - Advance to the next archive volume. 
X *
X * DESCRIPTION
X *
X *	Prompts the user to replace the backup medium with a new volume
X *	when the old one is full.  There are some cases, such as when
X *	archiving to a file on a hard disk, that the message can be a
X *	little surprising.  Assumes that background processes ignore
X *	interrupts and that the open() or the isatty() will fail for
X *	processes which are not attached to terminals. Returns a file
X *	descriptor or -1 if unsuccessful. 
X *
X * PARAMETERS
X *
X *	int mode	- mode of archive (READ, WRITE, PASS) 
X */
X
X#ifdef __STDC__
X
Xvoid next(int mode)
X
X#else
X
Xvoid next(mode)
Xint             mode;		/* mode of archive (READ, WRITE, PASS) */
X
X#endif
X{
X    char            msg[200];	/* buffer for message display */ 
X    char            answer[20];	/* buffer for user's answer */
X    int             ret;
X
X    close_archive();
X
X    sprintf(msg, "\
X%s: Ready for volume %u\n\
X%s: Type \"go\" when ready to proceed (or \"quit\" to abort): \07",
X		   myname, arvolume + 1, myname);
X    for (;;) {
X	ret = nextask(msg, answer, sizeof(answer));
X	if (ret == -1 || strcmp(answer, "quit") == 0) {
X	    fatal("Aborted");
X	}
X	if (strcmp(answer, "go") == 0 && open_archive(mode) == 0) {
X	    break;
X	}
X    }
X    warnarch("Continuing", (OFFSET) 0);
X}
END_OF_ttyio.c
if test 6321 -ne `wc -c <ttyio.c`; then
    echo shar: \"ttyio.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f warn.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"warn.c\"
else
echo shar: Extracting \"warn.c\" \(5446 characters\)
sed "s/^X//" >warn.c <<'END_OF_warn.c'
X/* $Source: /u/mark/src/pax/RCS/warn.c,v $
X *
X * $Revision: 1.1 $
X *
X * warn.c - miscellaneous user warning routines 
X *
X * DESCRIPTION
X *
X *	These routines provide the user with various forms of warning
X *	and informational messages.
X *
X * AUTHOR
X *
X *     Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
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
X * $Log:	warn.c,v $
X * Revision 1.1  88/12/23  18:02:40  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: warn.c,v 1.1 88/12/23 18:02:40 mark Rel $";
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
Xstatic void prsize(FILE *, OFFSET);
X
X#else /* !__STDC__ */
X
Xstatic void prsize();
X
X#endif /* __STDC__ */
X
X
X/* warnarch - print an archive-related warning message and offset
X *
X * DESCRIPTION
X *
X *	Present the user with an error message and an archive offset at
X *	which the error occured.   This can be useful for diagnosing or
X *	fixing damaged archives.
X *
X * PARAMETERS
X *
X *	char 	*msg	- A message string to be printed for the user.
X *	OFFSET 	adjust	- An adjustment which is added to the current 
X *			  archive position to tell the user exactly where 
X *			  the error occurred.
X */
X
X#ifdef __STDC__
X
Xvoid warnarch(char *msg, OFFSET adjust)
X
X#else 
X
Xvoid warnarch(msg, adjust)
Xchar           *msg;
XOFFSET          adjust;
X
X#endif
X{
X    fprintf(stderr, "%s: [offset ", myname);
X    prsize(stderr, total - adjust);
X    fprintf(stderr, "]: %s\n", msg);
X}
X
X
X/* syserr - return pointer to appropriate system error message
X *
X * DESCRIPTION
X *
X *	Get an error message string which is appropriate for the setting
X *	of the errno variable.
X *
X * RETURNS
X *
X *	Returns a pointer to a string which has an appropriate error
X *	message for the present value of errno.  The error message
X *	strings are taken from sys_errlist[] where appropriate.  If an
X *	appropriate message is not available in sys_errlist, then a
X *	pointer to the string "Unknown error (errno <errvalue>)" is 
X *	returned instead.
X */
X
X#ifdef __STDC__
X
Xchar *syserr(void)
X
X#else
X
Xchar *syserr()
X
X#endif
X{
X    static char     msg[40];		/* used for "Unknown error" messages */
X
X    if (errno > 0 && errno < sys_nerr) {
X	return (sys_errlist[errno]);
X    }
X    sprintf(msg, "Unknown error (errno %d)", errno);
X    return (msg);
X}
X
X
X/* prsize - print a file offset on a file stream
X *
X * DESCRIPTION
X *
X *	Prints a file offset to a specific file stream.  The file offset is
X *	of the form "%dm+%dk+%d", where the number preceeding the "m" and
X *	the "k" stand for the number of Megabytes and the number of
X *	Kilobytes, respectivley, which have been processed so far.
X *
X * PARAMETERS
X *
X *	FILE  *stream	- Stream which is to be used for output 
X *	OFFSET size	- Current archive position to be printed on the output 
X *			  stream in the form: "%dm+%dk+%d".
X *
X */
X
X#ifdef __STDC__
X
Xstatic void prsize(FILE *stream, OFFSET size)
X
X#else
X
Xstatic void prsize(stream, size)
XFILE           *stream;		/* stream which is used for output */
XOFFSET          size;		/* current archive position to be printed */
X
X#endif
X
X{
X    OFFSET          n;
X
X    if (n = (size / (1024 * 1024))) {
X	fprintf(stream, "%ldm+", n);
X	size -= n * 1024 * 1024;
X    }
X    if (n = (size / 1024)) {
X	fprintf(stream, "%ldk+", n);
X	size -= n * 1024;
X    }
X    fprintf(stream, "%ld", size);
X}
X
X
X/* fatal - print fatal message and exit
X *
X * DESCRIPTION
X *
X *	Fatal prints the program's name along with an error message, then
X *	exits the program with a non-zero return code.
X *
X * PARAMETERS
X *
X *	char 	*why	- description of reason for termination 
X *		
X * RETURNS
X *
X *	Returns an exit code of 1 to the parent process.
X */
X
X#ifdef __STDC__
X
Xvoid fatal(char *why)
X
X#else
X
Xvoid fatal(why)
Xchar           *why;		/* description of reason for termination */
X
X#endif
X{
X    fprintf(stderr, "%s: %s\n", myname, why);
X    exit(1);
X}
X
X
X
X/* warn - print a warning message
X *
X * DESCRIPTION
X *
X *	Print an error message listing the program name, the actual error
X *	which occurred and an informational message as to why the error
X *	occurred on the standard error device.  The standard error is
X *	flushed after the error is printed to assure that the user gets
X *	the message in a timely fasion.
X *
X * PARAMETERS
X *
X *	char *what	- Pointer to string describing what failed.
X *	char *why	- Pointer to string describing why did it failed.
X */
X
X#ifdef __STDC__
X
Xvoid warn(char *what, char *why)
X
X#else
X
Xvoid warn(what, why)
Xchar           *what;		/* message as to what the error was */
Xchar           *why;		/* explanation why the error occurred */
X
X#endif
X{
X    fprintf(stderr, "%s: %s : %s\n", myname, what, why);
X    fflush(stderr);
X}
END_OF_warn.c
if test 5446 -ne `wc -c <warn.c`; then
    echo shar: \"warn.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
echo shar: End of archive 2 \(of 6\).
cp /dev/null ark2isdone
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

