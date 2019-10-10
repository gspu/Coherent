# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 1 (of 6)."
# Contents:  PATCHLEVEL README append.c config.h cpio.c limits.h mem.c
#   pass.c pathname.c paxdir.h port.c port.h regexp.h tar.1 wildmat.c
# Wrapped by mark@jhereg on Tue Dec 27 19:37:30 1988
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f PATCHLEVEL -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"PATCHLEVEL\"
else
echo shar: Extracting \"PATCHLEVEL\" \(64 characters\)
sed "s/^X//" >PATCHLEVEL <<'END_OF_PATCHLEVEL'
XPatchlevel 0
X$Id: PATCHLEVEL,v 1.1 88/12/23 18:02:43 mark Rel $
END_OF_PATCHLEVEL
if test 64 -ne `wc -c <PATCHLEVEL`; then
    echo shar: \"PATCHLEVEL\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f README -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"README\"
else
echo shar: Extracting \"README\" \(4822 characters\)
sed "s/^X//" >README <<'END_OF_README'
XPAX - Portable Archive Interchange
X
XCopyright (C) 1989 Mark H. Colburn
XAll Rights Reserved.
X
X
XIntroduction
X
X    This is version 1.1 of Pax, a public domain archiving utility.  
X    
X    Pax is an archiving utility that reads and writes tar and cpio formats, 
X    both the traditional ones and the extended formats specified in IEEE 
X    1003.1.  It handles multi-volume archives and automatically determines 
X    the format of an archive while reading it.  Three user interfaces are 
X    supported: tar, cpio, and pax.  The pax interface was designed by IEEE 
X    1003.2 as a compromise in the chronic controversy over which of tar or 
X    cpio is best.
X
X    The USENIX Association provided some support for this implementation 
X    project.  As a result, the Pax utility is being distributed free of 
X    charge and may be redistributed by others in either source or binary 
X    form.  (See the liscensing section for restrictions)
X
X    The source for Pax is being posted to comp.sources.unix on USENET and 
X    will also be available by anonymous FTP on the Internet from uunet.uu.net,
X    moon.honeywell.com and from one of the Berkeley machines.  The source
X    to Pax will be available via anonymous UUCP from jhereg.mn.org, the 
X    author's home machine and possibly other sites.
X
X    The source for Pax will continue to change as long as the definition of 
X    the utility is modified by the 1003.2 working group.  (For example, 
X    there are a number of changes in Draft 8 which will be incorporated as 
X    soon as Draft 8 is available).  Additional modifications will be made 
X    based on user input, such as request for support of additional archive 
X    formats, etc.  Patches and new releases will be made as new functionality 
X    is added or problems are diagnosed and fixed.
X
X
XInstallation
X
X    In order to install Pax, you must first edit the Makefile and the 
X    config.h file according to the directions in each of the files.  
X    These two files provide the configuration information for most 
X    commonly available machines.  Please be sure to read through all 
X    the directions in each of these files before attempting to compile
X    Pax.
X
X
XPortability
X
X    Pax is intended to run on as many systems as possible.  If you have
X    problems getting Pax to compile or run on your system, please let me 
X    know so that the source or the installation procedure can be modified.
X
X    Pax has been tested and appears to run correctly on the following 
X    machines:
X
X        Machine                 Operating System/Release
X	-------------------------------------------------------
X	Altos 586		System III (2.3)
X	AT&T UNIX PC		System V.2 (Release 3.51)
X	Cray 2			UNICOS
X	HP 9000			HP/UX 6.0.1
X	Mac II 			A/UX 1.0
X	NCR Tower		System V.2
X	Pyramid			AT&T and Berkeley universe
X	Sequent Symetry		Dynix 3.0
X	SGI Iris 4D/60G		UNIX 3.0
X	SGI Iris 4D/70G		UNIX 3.0
X	Sun 2			SunOS 3.4
X	Sun 2			SunOS 3.5
X	Sun 3			SunOS 3.4
X	Sun 3			SunOS 3.5
X	Sun 3			SunOS 4.0
X	Sun 4			SunOS 4.0
X	VAX 8750		BSD 4.3 (Mt. Xinu)
X	VAX 8650		BSD 4.3 (Mt. Xinu)
X	VAX 780			BSD 4.3 (Berkeley)
X	-------------------------------------------------------
X
X    In future releases, the source will be moving toward ANSI C and POSIX 
X    compatibility.  This should allow for portability over any system 
X    supporting both ANSI and POSIX.  In addition, POSIX/ANSI portability 
X    library routines will be developed which will allow the code to run on 
X    the standard machines available now.
X
X
XCredit Where Credit is Due
X
X    Parts of the code which makes up Pax were gleaned from a number of 
X    different sources: the directory access routines in paxdir.h are 
X    modified copies of Doug Gwyn's dirent library; the regular expression 
X    matching routines in regexp.c are from Henry Spencer, some of the tar 
X    archive routines were initially written by John Gilmore for his PDTAR; 
X    and finally afio, written by Mark Brukhartz at Lachman Associates, was 
X    the basis for the buffering schemes used in pax.
X
X
XLicensing
X
X    Copyright (c) 1989 Mark H. Colburn.  
X    All rights reserved.
X
X    Redistribution and use in source and binary forms are permitted
X    provided that the above copyright notice is duplicated in all such 
X    forms and that any documentation, advertising materials, and other 
X    materials related to such distribution and use acknowledge that the 
X    software was developed by Mark H. Colburn and sponsored by The 
X    USENIX Association. 
X
X    THE SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
X    IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
X    WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
X
XPlease report any bug or problems to:
X
XMark Colburn
XNAPS International
X117 Mackubin St., Suite 1
XSt. Paul MN   55102
X(612) 224-9108
Xmark@jhereg.MN.ORG
END_OF_README
if test 4822 -ne `wc -c <README`; then
    echo shar: \"README\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f append.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"append.c\"
else
echo shar: Extracting \"append.c\" \(2298 characters\)
sed "s/^X//" >append.c <<'END_OF_append.c'
X/* $Source: /u/mark/src/pax/RCS/append.c,v $
X *
X * $Revision: 1.1 $
X *
X * append.c - append to a tape archive. 
X *
X * DESCRIPTION
X *
X *	Routines to allow appending of archives
X *
X * AUTHORS
X *
X *     	Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
X *
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
X * $Log:	append.c,v $
X * Revision 1.1  88/12/23  18:02:00  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: append.c,v 1.1 88/12/23 18:02:00 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* append_archive - main loop for appending to a tar archive
X *
X * DESCRIPTION
X *
X *	Append_archive reads an archive until the end of the archive is
X *	reached once the archive is reached, the buffers are reset and the
X *	create_archive function is called to handle the actual writing of
X *	the appended archive data.  This is quite similar to the
X *	read_archive function, however, it does not do all the processing.
X */
X
X#ifdef __STDC__
X
Xvoid append_archive(void)
X
X#else
X
Xvoid append_archive()
X
X#endif
X{
X    Stat            sb;
X    char            name[PATH_MAX + 1];
X
X    name[0] = '\0';
X    while (get_header(name, &sb) == 0) {
X	if (((ar_format == TAR)
X	     ? buf_skip(ROUNDUP((OFFSET) sb.sb_size, BLOCKSIZE))
X	     : buf_skip((OFFSET) sb.sb_size)) < 0) {
X	    warn(name, "File data is corrupt");
X	}
X    }
X    /* we have now gotten to the end of the archive... */
X
X    /* reset the buffer now that we have read the entire archive */
X    bufend = bufidx = bufstart;
X    create_archive();
X}
END_OF_append.c
if test 2298 -ne `wc -c <append.c`; then
    echo shar: \"append.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f config.h -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"config.h\"
else
echo shar: Extracting \"config.h\" \(4505 characters\)
sed "s/^X//" >config.h <<'END_OF_config.h'
X/* $Source: /u/mark/src/pax/RCS/config.h,v $
X *
X * $Revision: 1.1 $
X *
X * config.h - configuration options for PAX
X *
X * DESCRIPTION
X *
X *	This file contains a number of configurable parameters for the
X *	PAX software.  This files should be edited prior to makeing the
X *	package.
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
X#ifndef _PAX_CONFIG_H
X#define _PAX_CONFIG_H
X
X/* Defines */
X
X/*
X * USG - USG (Unix System V) specific modifications
X *
X * Define USG if you are running Unix System V or some similar variant
X */
X#define USG 	/* Running on a USG System */
X
X/*
X * BSD - BSD (Berkely) specific modifications
X *
X * Define BSD if you are running some version of BSD Unix
X */
X#define BSD 	/* Running on a BSD System */
X
X/*
X * DEF_AR_FILE - tar only (required)
X *
X * DEF_AR_FILE should contain the full pathname of your favorite archive
X * device.  Normally this would be a tape drive, but it may be a disk drive
X * on those systems that don't have tape drives.
X */
X#define DEF_AR_FILE	"-"	/* The default archive on your system */
X
X/*
X * TTY - device which interactive queries should be directed to (required)
X *
X * This is the device to which interactive queries will be sent to and
X * received from.  On most unix systems, this should be /dev/tty, however, on
X * some systems, such as MS-DOS, it my need to be different (e.g. "con:").
X */
X#define	TTY	"/dev/tty"	/* for most versions of UNIX */
X/* #define	TTY	"con:"		/* For MS-DOS */
X
X/*
X * PAXDIR - if you do not have directory access routines
X *
X * Define PAXDIR if you do not have Doug Gwyn's dirent package installed
X * as a system library or you wish to use the version supplied with PAX.  
X *
X * NOTE: DO NOT DEFINE THIS IF YOU HAVE BERKELEY DIRECTORY ACCESS ROUTINES.
X */
X/* #define PAXDIR		/* use paxdir.h paxdir.c */
X
X/*
X * DIRENT - directory access routines (required)
X *
X * If you have Doug Gwyn's dirent package installed, either as a system
X * library, or are using the paxdir.c and paxdir.h routines which come with 
X * PAX, then define dirent. 
X *
X * NOTE: DO NOT DEFINE THIS IF YOU HAVE BERKELEY DIRECTORY ACCESS ROUTINES.
X */
X/* #define DIRENT		/* use POSIX compatible directory routines */
X
X/*
X * OFFSET - compiler dependent offset type
X * 
X * OFFSET is the type which is returned by lseek().  It is different on
X * some systems.  Most define it to be off_t, but some define it to be long.
X */
X#define OFFSET	off_t	/* for most BSD, USG and other systems */
X/* #define OFFSET	long	/* for most of the rest of them... */
X
X/*
X * VOID - compiler support for VOID types
X *
X * If your system does not support void, then this should be defined to
X * int, otherwise, it should be left undefined.
X *
X * For ANSI Systems this should always be blank.
X */
X#ifndef __STDC__
X/* #define void	int	/* for system which do support void */
X#endif
X
X/*
X * SIG_T - return type for the signal routine
X *
X * Some systems have signal defines to return an int *, other return a
X * void *.  Please choose the correct value for your system.
X */
X/* #define SIG_T	void	/* signal defined as "void (*signal)()" */
X#define SIG_T	int	/* signal defined as "int (*signal)()" */
X
X/*
X * STRCSPN - use the strcspn function included with pax
X *
X * Some systems do not have the strcspn() function in their C libraries.
X * For those system define STRCSPN and the one provided in regexp.c will 
X * be used.
X */
X/* #define STRCSPN	/* implementation does not have strcspn() */
X
X/*
X * END OF CONFIGURATION SECTION
X *
X * Nothing beyond this point should need to be changed
X */
X
X#ifdef BSD
X#ifdef USG
X#include "You must first edit config.h and Makefile to configure pax."
X#endif
X#endif
X/*
X * Do a little sanity checking
X */
X#ifdef PAXDIR
X#  ifndef DIRENT
X#    define DIRENT
X#  endif
X#endif
X
X#endif /* _PAX_CONFIG_H */
END_OF_config.h
if test 4505 -ne `wc -c <config.h`; then
    echo shar: \"config.h\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f cpio.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"cpio.c\"
else
echo shar: Extracting \"cpio.c\" \(4492 characters\)
sed "s/^X//" >cpio.c <<'END_OF_cpio.c'
X/* $Source: /u/mark/src/pax/RCS/cpio.c,v $
X *
X * $Revision: 1.1 $
X *
X * cpio.c - Cpio specific functions for archive handling
X *
X * DESCRIPTION
X *
X *	These function provide a cpio conformant interface to the pax
X *	program.
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
X * $Log:	cpio.c,v $
X * Revision 1.1  88/12/23  18:02:05  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: cpio.c,v 1.1 88/12/23 18:02:05 mark Rel $";
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
Xstatic void 	usage(void);
X#else /* !__STDC__ */
Xstatic void 	usage();
X#endif /* __STDC__ */
X
X
X/* do_cpio - handle cpio format archives
X *
X * DESCRIPTION
X *
X *	Do_cpio provides a standard CPIO interface to the PAX program.  All
X *	of the standard cpio flags are available, and the behavior of the
X *	program mimics traditonal cpio.
X *
X * PARAMETERS
X *
X *	int	argc	- command line argument count
X *	char	**argv	- pointer to command line arguments
X *
X * RETURNS
X *
X *	Nothing.
X */
X
X#ifdef __STDC__
X
Xint do_cpio(int argc, char **argv)
X
X#else
X
Xint do_cpio(argc, argv)
Xint             argc;
Xchar          **argv;
X
X#endif
X{
X    int             c;
X    char           *dirname;
X    Stat            st;
X
X    /* default input/output file for CPIO is STDIN/STDOUT */
X    ar_file = "-";
X    names_from_stdin = 1;
X
X    /* set up the flags to reflect the default CPIO inteface. */
X    blocksize = BLOCKSIZE;
X    ar_interface = CPIO;
X    ar_format = CPIO;
X    msgfile=stderr;
X
X    while ((c = getopt(argc, argv, "D:Bacdfilmoprtuv")) != EOF) {
X	switch (c) {
X	case 'i':
X	    f_extract = 1;
X	    break;
X	case 'o':
X	    f_create = 1;
X	    break;
X	case 'p':
X	    f_pass = 1;
X	    dirname = argv[--argc];
X
X	    /* check to make sure that the argument is a directory */
X	    if (LSTAT(dirname, &st) < 0) {
X		fatal(syserr());
X	    }
X	    if ((st.sb_mode & S_IFMT) != S_IFDIR) {
X		fatal("Not a directory");
X	    }
X	    break;
X	case 'B':
X	    blocksize = BLOCK;
X	    break;
X	case 'a':
X	    f_access_time = 1;
X	    break;
X	case 'c':
X	    break;
X	case 'D':
X	    ar_file = optarg;
X	    break;
X	case 'd':
X	    f_create_dirs = 1;
X	    break;
X	case 'f':
X	    f_reverse_match = 1;
X	    break;
X	case 'l':
X	    f_link = 1;
X	    break;
X	case 'm':
X	    f_modification_time = 1;
X	    break;
X	case 'r':
X	    f_interactive = 1;
X	    break;
X	case 't':
X	    f_list = 1;
X	    break;
X	case 'u':
X	    f_unconditional = 1;
X	    break;
X	case 'v':
X	    f_verbose = 1;
X	    break;
X	default:
X	    usage();
X	}
X    }
X
X    if (f_create + f_pass + f_extract != 1) {
X	usage();
X    }
X    if (!f_pass) {
X	buf_allocate((OFFSET) blocksize);
X    }
X    if (f_extract) {
X	open_archive(AR_READ);	/* Open for reading */
X	read_archive();
X    } else if (f_create) {
X	open_archive(AR_WRITE);
X	create_archive();
X    } else if (f_pass) {
X	pass(dirname);
X    }
X
X    /* print out the total block count transfered */
X    fprintf(stderr, "%d Blocks\n", ROUNDUP(total, BLOCKSIZE) / BLOCKSIZE);
X    
X    exit(0);
X    /* NOTREACHED */
X}
X
X
X/* usage - print a helpful message and exit
X *
X * DESCRIPTION
X *
X *	Usage prints out the usage message for the CPIO interface and then
X *	exits with a non-zero termination status.  This is used when a user
X *	has provided non-existant or incompatible command line arguments.
X *
X * RETURNS
X *
X *	Returns an exit status of 1 to the parent process.
X *
X */
X
X#ifdef __STDC__
X
Xstatic void usage(void)
X
X#else
X
Xstatic void usage()
X
X#endif
X{
X    fprintf(stderr, "\
XUsage: %s -o[Bacv]\n", myname);
X    fprintf(stderr, "\
X       %s -i[Bcdmrtuvf] [pattern...]\n", myname);
X    fprintf(stderr, "\
X       %s -p[adlmruv] directory\n", myname);
X    exit(1);
X}
END_OF_cpio.c
if test 4492 -ne `wc -c <cpio.c`; then
    echo shar: \"cpio.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f limits.h -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"limits.h\"
else
echo shar: Extracting \"limits.h\" \(2765 characters\)
sed "s/^X//" >limits.h <<'END_OF_limits.h'
X/* $Source: /u/mark/src/pax/RCS/limits.h,v $
X *
X * $Revision: 1.1 $
X *
X * 	limits.h - POSIX compatible defnitions for some of <limits.h>
X *
X * DESCRIPTION
X *
X * 	We need to include <limits.h> if this system is being compiled with an 
X * 	ANSI standard C compiler, or if we are running on a POSIX confomrming 
X * 	system.  If the manifest constant _POSIX_SOURCE is not defined when 
X * 	<limits.h> is included, then none of the POSIX constants are defined 
X *	and we need to define them here.  It's a bit wierd, but it works.
X *
X * 	These values where taken from the IEEE P1003.1 standard, draft 12.
X * 	All of the values below are the MINIMUM values allowed by the standard.
X * 	Not all values are used by the PAX program, but they are included for
X * 	completeness, and for support of future enhancements.  Please see
X * 	section 2.9 of the draft standard for more information on the following
X * 	constants.
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
X#ifndef _PAX_LIMITS_H
X#define _PAX_LIMITS_H
X
X/* Headers */
X
X#if defined(__STDC__) || defined(_POSIX_SOURCE)
X#   include <limits.h>
X#endif
X
X
X/* Defines */
X
X#ifndef _POSIX_SOURCE
X
X#define MAX_INPUT	256	/* Max numbef of bytes in terminal input */
X#define NGROUPS_MAX	1	/* Max number of suplemental group id's */
X#define PASS_MAX	8	/* Max number of bytes in a password */
X#define PID_MAX		30000	/* Max value for a process ID */
X#define UID_MAX		32000	/* Max value for a user or group ID */
X#define ARG_MAX		4096	/* Nax number of bytes passed to exec */
X#define CHILD_MAX	6	/* Max number of simultaneous processes */
X#define MAX_CANON	256	/* Max numbef of bytes in a cononical queue */
X#define OPEN_MAX	16	/* Nax number of open files per process */
X#define NAME_MAX	14	/* Max number of bytes in a file name */
X#define PATH_MAX	255	/* Max number of bytes in pathname */
X#define LINK_MAX	8	/* Max value of a file's link count */
X#define PIPE_BUF	512	/* Max number of bytes for pipe reads */
X
X#endif /* _POSIX_SOURCE */
X#endif /* _PAX_LIMITS_H */
END_OF_limits.h
if test 2765 -ne `wc -c <limits.h`; then
    echo shar: \"limits.h\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f mem.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"mem.c\"
else
echo shar: Extracting \"mem.c\" \(3204 characters\)
sed "s/^X//" >mem.c <<'END_OF_mem.c'
X/* $Source: /u/mark/src/pax/RCS/mem.c,v $
X *
X * $Revision: 1.1 $
X *
X * mem.c - memory allocation and manipulation functions
X *
X * DESCRIPTION
X *
X *	These routines are provided for higher level handling of the UNIX
X *	memory allocation functions.
X *
X * AUTHOR
X *
X *     Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
X *
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
X * $Log:	mem.c,v $
X * Revision 1.1  88/12/23  18:02:17  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: mem.c,v 1.1 88/12/23 18:02:17 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* mem_get - allocate memory
X *
X * DESCRIPTION
X *
X *	Mem_get attempts to allocate a block of memory using the malloc
X *	function call.  In the event that the memory is not available, 
X *	mem_get will display an "Out of memory" message for the user
X *	the first time it encounters the an out of memory situation.
X *	Subsequent calls to mem_get may fail, but no message will be
X *	printed.
X *
X * PARAMETERS
X *
X *	uint len	- The amount of memory to allocate
X *
X * RETURNS
X *
X *	Normally returns the pointer to the newly allocated memory.  If
X *	an error occurss, NULL is returned, and an error message is
X *	printed.
X *
X * ERRORS
X *
X *	ENOMEM	No memory is available 
X */
X
X#ifdef __STDC__
X
Xchar *mem_get(uint len)
X
X#else
X
Xchar *mem_get(len)
Xuint            len;		/* amount of memory to get */
X
X#endif
X{
X    char           *mem;
X    static short    outofmem = 0;
X
X    if ((mem = (char *)malloc(len)) == NULL && !outofmem) {
X	outofmem++;
X	warn("mem_get()", "Out of memory");
X    }
X    return (mem);
X}
X
X
X/* mem_str - duplicate a string into dynamic memory
X *
X * DESCRIPTION
X *
X *	Mem_str attempts to make a copy of string.  It allocates space for
X *	the string, and if the allocation was successfull, copies the old
X *	string into the newly allocated space.
X *
X * PARAMETERS
X *
X *	char *str 	- string to make a copy of 
X *
X * RETURNS
X *
X *	Normally returns a pointer to a new string at least as large
X *	as strlen(str) + 1, which contains a copy of the the data 
X *	passed in str, plus a null terminator.  Returns (char *)NULL 
X *	if enough memory to make a copy of str is not available.
X */
X
X#ifdef __STDC__
X
Xchar *mem_str(char *str)
X
X#else
X
Xchar *mem_str(str)
Xchar           *str;		/* string to make a copy of */
X
X#endif
X{
X    char           *mem;
X
X    if (mem = mem_get((uint) strlen(str) + 1)) {
X	strcpy(mem, str);
X    }
X    return (mem);
X}
END_OF_mem.c
if test 3204 -ne `wc -c <mem.c`; then
    echo shar: \"mem.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f pass.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"pass.c\"
else
echo shar: Extracting \"pass.c\" \(3492 characters\)
sed "s/^X//" >pass.c <<'END_OF_pass.c'
X/* $Source: /u/mark/src/pax/RCS/pass.c,v $
X *
X * $Revision: 1.1 $
X *
X * pass.c - handle the pass option of cpio
X *
X * DESCRIPTION
X *
X *	These functions implement the pass options in PAX.  The pass option
X *	copies files from one directory hierarchy to another.
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
X * $Log:	pass.c,v $
X * Revision 1.1  88/12/23  18:02:20  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: pass.c,v 1.1 88/12/23 18:02:20 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* pass - copy within the filesystem
X *
X * DESCRIPTION
X *
X *	Pass copies the named files from the current directory hierarchy to
X *	the directory pointed to by dirname.
X *
X * PARAMETERS
X *
X *	char	*dirname	- name of directory to copy named files to.
X *
X */
X
X#ifdef __STDC__
X    
Xint pass(char *dirname)
X
X#else
X    
Xint pass(dirname)
Xchar	*dirname;
X
X#endif
X{
X    char            name[PATH_MAX + 1];
X    int             fd;
X    Stat            sb;
X
X    while (name_next(name, &sb) >= 0 && (fd = openi(name, &sb)) >= 0) {
X
X	if (rplhead != NULL) {
X	    rpl_name(name);
X	}
X	if (get_disposition("pass", name) || get_newname(name, sizeof(name))) {
X	    /* skip file... */
X	    if (fd) {
X		close(fd);
X	    }
X	    continue;
X	} 
X
X	if (passitem(name, &sb, fd, dirname)) {
X	    close(fd);
X	}
X	if (f_verbose) {
X	    fprintf(stderr, "%s/%s\n", dirname, name);
X	}
X    }
X}
X
X
X/* passitem - copy one file
X *
X * DESCRIPTION
X *
X *	Passitem copies a specific file to the named directory
X *
X * PARAMETERS
X *
X *	char   *from	- the name of the file to open
X *	Stat   *asb	- the stat block associated with the file to copy
X *	int	ifd	- the input file descriptor for the file to copy
X *	char   *dir	- the directory to copy it to
X *
X * RETURNS
X *
X * 	Returns given input file descriptor or -1 if an error occurs.
X *
X * ERRORS
X */
X
X#ifdef __STDC__
X
Xint passitem(char *from, Stat *asb, int ifd, char *dir)
X
X#else
X    
Xint passitem(from, asb, ifd, dir)
Xchar           *from;
XStat           *asb;
Xint             ifd;
Xchar           *dir;
X
X#endif
X{
X    int             ofd;
X    time_t          tstamp[2];
X    char            to[PATH_MAX + 1];
X
X    if (nameopt(strcat(strcat(strcpy(to, dir), "/"), from)) < 0) {
X	return (-1);
X    }
X    if (asb->sb_nlink > 1) {
X	if (f_link && islink(from, asb) == (Link *) NULL) {
X	    linkto(from, asb);
X	}
X	linkto(to, asb);
X    }
X    if ((ofd = openo(to, asb, islink(to, asb), 1)) < 0) {
X	return (-1);
X    }
X    if (ofd > 0) {
X	passdata(from, ifd, to, ofd);
X    }
X    tstamp[0] = asb->sb_atime;
X    tstamp[1] = f_modification_time ? asb->sb_mtime : time((time_t *) 0);
X    utime(to, tstamp);
X    return (ifd);
X}
END_OF_pass.c
if test 3492 -ne `wc -c <pass.c`; then
    echo shar: \"pass.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f pathname.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"pathname.c\"
else
echo shar: Extracting \"pathname.c\" \(4973 characters\)
sed "s/^X//" >pathname.c <<'END_OF_pathname.c'
X/* $Source: /u/mark/src/pax/RCS/pathname.c,v $
X *
X * $Revision: 1.1 $
X *
X * pathname.c - directory/pathname support functions 
X *
X * DESCRIPTION
X *
X *	These functions provide directory/pathname support for PAX
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
X * $Log:	pathname.c,v $
X * Revision 1.1  88/12/23  18:02:21  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: pathname.c,v 1.1 88/12/23 18:02:21 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* dirneed  - checks for the existance of directories and possibly create
X *
X * DESCRIPTION
X *
X *	Dirneed checks to see if a directory of the name pointed to by name
X *	exists.  If the directory does exist, then dirneed returns 0.  If
X *	the directory does not exist and the f_create_dirs flag is set,
X *	then dirneed will create the needed directory, recursively creating
X *	any needed intermediate directory.
X *
X *	If f_create_dirs is not set, then no directories will be created
X *	and a value of -1 will be returned if the directory does not
X *	exist.
X *
X * PARAMETERS
X *
X *	name		- name of the directory to create
X *
X * RETURNS
X *
X *	Returns a 0 if the creation of the directory succeeded or if the
X *	directory already existed.  If the f_create_dirs flag was not set
X *	and the named directory does not exist, or the directory creation 
X *	failed, a -1 will be returned to the calling routine.
X */
X
X#ifdef __STDC__
X
Xint dirneed(char *name)
X
X#else
X
Xint dirneed(name)
Xchar           *name;
X
X#endif
X{
X    char           *cp;
X    char           *last;
X    int             ok;
X    static Stat     sb;
X
X    last = NULL;
X    for (cp = name; *cp;) {
X	if (*cp++ == '/') {
X	    last = cp;
X	}
X    }
X    if (last == NULL) {
X	return (STAT(".", &sb));
X    }
X    *--last = '\0';
X    ok = STAT(*name ? name : ".", &sb) == 0
X	? ((sb.sb_mode & S_IFMT) == S_IFDIR)
X	: (f_create_dirs && dirneed(name) == 0 && dirmake(name, &sb) == 0);
X    *last = '/';
X    return (ok ? 0 : -1);
X}
X
X
X/* nameopt - optimize a pathname
X *
X * DESCRIPTION
X *
X * 	Confused by "<symlink>/.." twistiness. Returns the number of final 
X * 	pathname elements (zero for "/" or ".") or -1 if unsuccessful. 
X *
X * PARAMETERS
X *
X *	char	*begin	- name of the path to optimize
X *
X * RETURNS
X *
X *	Returns 0 if successful, non-zero otherwise.
X *
X */
X
X#ifdef __STDC__
X
Xint nameopt(char *begin)
X
X#else
X
Xint nameopt(begin)
Xchar           *begin;
X
X#endif
X{
X    char           *name;
X    char           *item;
X    int             idx;
X    int             absolute;
X    char           *element[PATHELEM];
X
X    absolute = (*(name = begin) == '/');
X    idx = 0;
X    for (;;) {
X	if (idx == PATHELEM) {
X	    warn(begin, "Too many elements");
X	    return (-1);
X	}
X	while (*name == '/') {
X	    ++name;
X	}
X	if (*name == '\0') {
X	    break;
X	}
X	element[idx] = item = name;
X	while (*name && *name != '/') {
X	    ++name;
X	}
X	if (*name) {
X	    *name++ = '\0';
X	}
X	if (strcmp(item, "..") == 0) {
X	    if (idx == 0) {
X		if (!absolute) {
X		    ++idx;
X		}
X	    } else if (strcmp(element[idx - 1], "..") == 0) {
X		++idx;
X	    } else {
X		--idx;
X	    }
X	} else if (strcmp(item, ".") != 0) {
X	    ++idx;
X	}
X    }
X    if (idx == 0) {
X	element[idx++] = absolute ? "" : "."; 
X    }
X    element[idx] = NULL;
X    name = begin;
X    if (absolute) {
X	*name++ = '/';
X    }
X    for (idx = 0; item = element[idx]; ++idx, *name++ = '/') {
X	while (*item) {
X	    *name++ = *item++;
X	}
X    }
X    *--name = '\0';
X    return (idx);
X}
X
X
X/* dirmake - make a directory  
X *
X * DESCRIPTION
X *
X *	Dirmake makes a directory with the appropritate permissions.
X *
X * PARAMETERS
X *
X *	char 	*name	- Name of directory make
X *	Stat	*asb	- Stat structure of directory to make
X *
X * RETURNS
X *
X * 	Returns zero if successful, -1 otherwise. 
X *
X */
X
X#ifdef __STDC__
X
Xint dirmake(char *name, Stat *asb)
X
X#else
X
Xint dirmake(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    if (mkdir(name, (int) (asb->sb_mode & S_IPOPN)) < 0) {
X	return (-1);
X    }
X    if (asb->sb_mode & S_IPEXE) {
X	chmod(name, (int) (asb->sb_mode & S_IPERM));
X    }
X    if (f_owner) {
X	chown(name, (int) asb->sb_uid, (int) asb->sb_gid);
X    }
X    return (0);
X}
END_OF_pathname.c
if test 4973 -ne `wc -c <pathname.c`; then
    echo shar: \"pathname.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f paxdir.h -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"paxdir.h\"
else
echo shar: Extracting \"paxdir.h\" \(1562 characters\)
sed "s/^X//" >paxdir.h <<'END_OF_paxdir.h'
X/*
X	<dirent.h> -- definitions for SVR3 directory access routines
X
X	last edit:	25-Apr-1987	D A Gwyn
X
X	Prerequisite:	<sys/types.h>
X*/
X
X#ifndef _PAX_DIRENT_H
X#define _PAX_DIRENT_H
X
X#include "config.h"
X#ifdef USG
X#define UFS
X#else
X#ifdef BSD
X#define BFS
X#endif
X#endif
X
Xstruct dirent { 			/* data from getdents()/readdir() */
X    long		d_ino;		/* inode number of entry */
X    off_t		d_off;		/* offset of disk directory entry */
X    unsigned short	d_reclen;	/* length of this record */
X    char		d_name[1];	/* name of file (non-POSIX) */
X};
X
Xtypedef struct {
X    int		dd_fd;			/* file descriptor */
X    int		dd_loc;			/* offset in block */
X    int		dd_size;		/* amount of valid data */
X    char	*dd_buf;		/* -> directory block */
X} DIR;					/* stream data from opendir() */
X
X
X/* 
X * The following nonportable ugliness could have been avoided by defining
X * DIRENTSIZ and DIRENTBASESIZ to also have (struct dirent *) arguments. 
X */
X#define	DIRENTBASESIZ		(((struct dirent *)0)->d_name \
X				- (char *)&((struct dirent *)0)->d_ino)
X#define	DIRENTSIZ( namlen )	((DIRENTBASESIZ + sizeof(long) + (namlen)) \
X				/ sizeof(long) * sizeof(long))
X
X#define	MAXNAMLEN 512			/* maximum filename length */
X
X#ifndef NAME_MAX
X#define	NAME_MAX (MAXNAMLEN - 1)	/* DAG -- added for POSIX */
X#endif
X
X#define	DIRBUF	 8192			/* buffer size for fs-indep. dirs */
X		/* must in general be larger than the filesystem buffer size */
X
Xextern DIR		*opendir();
Xextern struct dirent	*readdir();
Xextern OFFSET		telldir();
Xextern void		seekdir();
Xextern int		closedir();
X
X#endif /* _PAX_DIRENT_H */
END_OF_paxdir.h
if test 1562 -ne `wc -c <paxdir.h`; then
    echo shar: \"paxdir.h\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f port.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"port.c\"
else
echo shar: Extracting \"port.c\" \(4782 characters\)
sed "s/^X//" >port.c <<'END_OF_port.c'
X/* $Source: /u/mark/src/pax/RCS/port.c,v $
X *
X * $Revision: 1.1 $
X *
X * port.c - These are routines not available in all environments. 
X *
X * DESCRIPTION
X *
X *	The routines contained in this file are provided for portability to
X *	other versions of UNIX or other operating systems (e.g. MSDOS).
X *	Not all systems have the same functions or the same semantics,
X *	these routines attempt to bridge the gap as much as possible.
X *
X * AUTHOR
X *
X *	Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
X *	John Gilmore (gnu@hoptoad)
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
X * $Log:	port.c,v $
X * Revision 1.1  88/12/23  18:02:29  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: port.c,v 1.1 88/12/23 18:02:29 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/*
X * Some computers are not so crass as to align themselves into the BSD or USG
X * camps.  If a system supplies all of the routines we fake here, add it to
X * the list in the #if !defined()'s below and it'll all be skipped. 
X */
X
X#if !defined(mc300) && !defined(mc500) && !defined(mc700) && \
X    !defined(BSD)
X
X/* mkdir - make a directory
X *
X * DESCRIPTION
X *
X * 	Mkdir will make a directory of the name "dpath" with a mode of
X *	"dmode".  This is consistent with the BSD mkdir() function and the
X *	P1003.1 definitions of MKDIR.
X *
X * PARAMETERS
X *
X *	dpath		- name of directory to create
X *	dmode		- mode of the directory
X *
X * RETURNS
X *
X *	Returns 0 if the directory was successfully created, otherwise a
X *	non-zero return value will be passed back to the calling function
X *	and the value of errno should reflect the error.
X */
X
X#ifdef __STDC__
X
Xint mkdir(char *dpath, int dmode)
X
X#else
X    
Xint mkdir(dpath, dmode)
Xchar           *dpath;
Xint             dmode;
X
X#endif
X{
X    int             cpid, status;
X    Stat            statbuf;
X    extern int      errno;
X
X    if (STAT(dpath, &statbuf) == 0) {
X	errno = EEXIST;		/* Stat worked, so it already exists */
X	return (-1);
X    }
X    /* If stat fails for a reason other than non-existence, return error */
X    if (errno != ENOENT)
X	return (-1);
X
X    switch (cpid = fork()) {
X
X    case -1:			/* Error in fork() */
X	return (-1);		/* Errno is set already */
X
X    case 0:			/* Child process */
X
X	status = umask(0);	/* Get current umask */
X	status = umask(status | (0777 & ~dmode));	/* Set for mkdir */
X	execl("/bin/mkdir", "mkdir", dpath, (char *) 0);
X	_exit(-1);		/* Can't exec /bin/mkdir */
X
X    default:			/* Parent process */
X	while (cpid != wait(&status)) {
X	    /* Wait for child to finish */
X	}
X    }
X
X    if (TERM_SIGNAL(status) != 0 || TERM_VALUE(status) != 0) {
X	errno = EIO;		/* We don't know why, but */
X	return (-1);		/* /bin/mkdir failed */
X    }
X    return (0);
X}
X
X
X/* rmdir - remove a directory
X *
X * DESCRIPTION
X *
X *	Rmdir will remove the directory specified by "dpath".  It is
X *	consistent with the BSD and POSIX rmdir functions.
X *
X * PARAMETERS
X *
X *	dpath		- name of directory to remove
X *
X * RETURNS
X *
X *	Returns 0 if the directory was successfully deleted, otherwise a
X *	non-zero return value will be passed back to the calling function
X *	and the value of errno should reflect the error.
X */
X
X#ifdef __STDC__
X
Xint rmdir(char *dpath)
X
X#else
X    
Xint rmdir(dpath)
Xchar           *dpath;
X
X#endif
X{
X    int             cpid, status;
X    Stat            statbuf;
X    extern int      errno;
X
X    /* check to see if it exists */
X    if (STAT(dpath, &statbuf) == -1) {
X	return (-1);
X    }
X    switch (cpid = fork()) {
X
X    case -1:			/* Error in fork() */
X	return (-1);		/* Errno is set already */
X
X    case 0:			/* Child process */
X	execl("/bin/rmdir", "rmdir", dpath, (char *) 0);
X	_exit(-1);		/* Can't exec /bin/rmdir */
X
X    default:			/* Parent process */
X	while (cpid != wait(&status)) {
X	    /* Wait for child to finish */
X	}
X    }
X
X    if (TERM_SIGNAL(status) != 0 || TERM_VALUE(status) != 0) {
X	errno = EIO;		/* We don't know why, but */
X	return (-1);		/* /bin/rmdir failed */
X    }
X    return (0);
X}
X
X#endif /* MASSCOMP, BSD */
END_OF_port.c
if test 4782 -ne `wc -c <port.c`; then
    echo shar: \"port.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f port.h -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"port.h\"
else
echo shar: Extracting \"port.h\" \(2833 characters\)
sed "s/^X//" >port.h <<'END_OF_port.h'
X/* $Source: /u/mark/src/pax/RCS/port.h,v $
X *
X * $Revision: 1.1 $
X *
X * port.h - defnitions for portability library
X *
X * DESCRIPTION
X *
X *	Header for maintaing portablilty across operating system and
X *	version boundries.  For the most part, this file contains
X *	definitions which map functions which have the same functionality
X *	but different names on different systems, to have the same name.
X *
X * AUTHORS
X *
X *	Mark H. Colburn, NAPS International (mark@jhereg.mn.org)
X *	John Gilmore (gnu@hoptoad)
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
X#ifndef _PAX_PORT_H
X#define _PAX_PORT_H
X
X/*
X * Everybody does wait() differently.  There seem to be no definitions for
X * this in V7 (e.g. you are supposed to shift and mask things out using
X * constant shifts and masks.)  In order to provide the functionality, here
X * are some non standard but portable macros.  Don't change to a "union wait" 
X * based approach -- the ordering of the elements of the struct depends on the 
X * byte-sex of the machine. 
X */
X
X#define	TERM_SIGNAL(status)	((status) & 0x7F)
X#define TERM_COREDUMP(status)	(((status) & 0x80) != 0)
X#define TERM_VALUE(status)	((status) >> 8)
X
X/*
X * String library emulation definitions for the different variants of UNIX
X */
X
X#if defined(USG) 
X
X#   include <string.h>
X#   include <memory.h>
X
X#else /* USG */
X
X/*
X * The following functions are defined here since func.h has no idea which
X * of the functions will actually be used.
X */
X#  ifdef __STDC__
Xextern char *rindex(char *, char);
Xextern char *index(char *, char);
Xextern char *bcopy(char *, char *, unsigned int);
Xextern char *bzero(char *, unsigned int);
Xextern char *strcat(char *, char *);
Xextern char *strcpy(char *, char *);
X#  else /* !__STDC__ */
Xextern char *rindex();
Xextern char *index();
Xextern char *bcopy();
Xextern char *bzero();
Xextern char *strcat();
Xextern char *strcpy();
X#  endif /* __STDC__ */
X
X/*
X * Map ANSI C compatible functions to V7 functions
X */
X
X#   define memcpy(a,b,n)	bcopy((b),(a),(n))
X#   define memset(a,b,n)	bzero((a),(n))
X#   define strrchr(s,c)		rindex(s,c)
X#   define strchr(s,c)		index(s,c)
X
X#endif /* USG */
X#endif /* _PAX_PORT_H */
END_OF_port.h
if test 2833 -ne `wc -c <port.h`; then
    echo shar: \"port.h\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f regexp.h -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"regexp.h\"
else
echo shar: Extracting \"regexp.h\" \(803 characters\)
sed "s/^X//" >regexp.h <<'END_OF_regexp.h'
X/*
X * Definitions etc. for regexp(3) routines.
X *
X * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
X * not the System V one.
X */
X
X#ifndef _PAX_REGEXP_H
X#define _PAX_REGEXP_H
X
X#define NSUBEXP  10
Xtypedef struct regexp {
X	char *startp[NSUBEXP];
X	char *endp[NSUBEXP];
X	char regstart;		/* Internal use only. */
X	char reganch;		/* Internal use only. */
X	char *regmust;		/* Internal use only. */
X	int regmlen;		/* Internal use only. */
X	char program[1];	/* Unwarranted chumminess with compiler. */
X} regexp;
X
X
X/*
X * The first byte of the regexp internal "program" is actually this magic
X * number; the start node begins in the second byte.
X */
X#define	MAGIC	0234
X
Xextern regexp *regcomp();
Xextern int regexec();
Xextern void regsub();
Xextern void regerror();
X
X#endif /* _PAX_REGEXP_H */
END_OF_regexp.h
if test 803 -ne `wc -c <regexp.h`; then
    echo shar: \"regexp.h\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f tar.1 -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"tar.1\"
else
echo shar: Extracting \"tar.1\" \(4671 characters\)
sed "s/^X//" >tar.1 <<'END_OF_tar.1'
X.\" $Id: tar.1,v 1.1 88/12/23 18:02:37 mark Rel $
X.TH TAR 1 "USENIX Association" ""
X.SH NAME
Xtar \- process tape archives
X.SH SYNOPSIS
X.B tar
X.BR \-c [ bfvw ]
X.I device
X.I block
X.I filename...
X.br
X.B tar
X.BR \-r [ bvw ]
X.I device
X.I block
X.RI [ filename... ]
X.br
X.B tar
X.BR \-t [ fv ]
X.I device
X.br
X.B tar
X.BR \-u [ bvw ]
X.I device
X.I block
X.br
X.B tar
X.BR \-x [ flmovw ]
X.I device
X.RI [ filename... ]
X.SH DESCRIPTION
X.I Tar
Xreads and writes archive files which conform to the
X.B "Archive/Interchange File Format"
Xspecified in
X.IR "IEEE Std. 1003.1-1988" .
X.SS Options
XThe following options are available:
X.TP 1i
X.B \-c
XCreates a new archive; writing begins at the beginning of the archive,
Xinstead of after the last file.
X.TP 1i
X.B \-r
XWrites names files to the end of the archive.
X.TP 1i
X.B \-t
XLists the names of all of the files in the archive.
X.TP 1i
X.B \-u
XCauses named files to be
Xadded to the archive if they are not already there, or have been
Xmodified since last written into the archive.
XThis implies the
X.B \-r
Xoption.
X.TP 1i
X.B \-x
XExtracts named files
Xfrom the archive.
XIf a named file matches a directory whose contents had been written onto
Xthe archive, that directory is recursively extracted.
XIf a named file in the archive does not exist on the system, the file is
Xcreate with the same mode as the one in the archive, except that the
Xset-user-id and get-group-id modes are not set unless the user has
Xappropriate privileges.
X.PP
XIf the files exist, their modes are not changed except as described above.
XThe owner, group and modification time are restored if possible.
XIf no
X.I filename
Xargument is given, the entire contents of the archive is extracted.
XNote that if several files with the same name are in the archive,
Xthe last one will overwrite all earlier ones.
X.TP 1i
X.B \-b
XCauses
X.I tar
Xto use the next argument on the command line as the blocking factor for
Xtape records.
XThe default is 1; the maximum is 20.
XThis option should only be used with raw magnetic tape archives.
XNormally, the block size is determined automatically when reading tapes.
X.TP 1i
X.B \-f
XCauses
X.I tar
Xto use the next argument on the command line as the name of the archive
Xinstead of the default, which is usually a tape drive.
XIf
X.B -
Xis specified as a filename
X.I tar
Xwrites to the standard output or reads from the standard input, whichever
Xis appropriate for the options given.
XThus,
X.I tar
Xcan be used as the head or tail of a pipeline.
X.TP 1i
X.B \-l
XTells
X.I tar
Xto report if it cannot resolve all of the links to the files being
Xarchived.
XIf
X.B \-l
Xis not specified, no error messages are written to the standard output.
XThis modifier is only valid with the
X.BR \-c ,
X.B \-r
Xand
X.BR \-u
Xoptions.
X.TP 1i
X.B \-m
XTells
X.I tar
Xnot to restore the modification times.
XThe modification time of the file will be the time of extraction.
XThis modifier is invalid with th
X.B \-t
Xoption.
X.TP 1i
X.B \-o
XCauses extracted files to take on the user and group identifier of the user
Xrunning the program rather than those on the archive.
XThis modifier is only valid with the
X.B \-x
Xoption.
X.TP 1i
X.B \-v
XCauses
X.I tar
Xto operate verbosely.  Usually,
X.I tar
Xdoes its work silently, but
Xthe
X.B v
Xmodifier causes it to print the name of each file it processes,
Xpreceded by the option letter.
XWith the
X.B \-t
Xoption,
X.B v
Xgives more information about the archive entries than just the name.
X.TP 1i
X.B \-w
XCauses
X.I tar
Xto print the action to be taken, followed by the name of the file, and then
Xwait for the user's confirmation.
XIf a word beginning with
X.B y
Xis given, the action is performed.
XAny other input means "no".
XThis modifier is invalid with the
X.B \-t
Xoption.
X.SH FILES
X.TP 1i
X/dev/tty
Xused to prompt the user for information when the
X.BR \-i " or " \-y
Xoptions are specified.
X.SH "SEE ALSO"
Xcpio(1), dd(1), find(1), pax(1), cpio(5), tar(5)
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
END_OF_tar.1
if test 4671 -ne `wc -c <tar.1`; then
    echo shar: \"tar.1\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f wildmat.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"wildmat.c\"
else
echo shar: Extracting \"wildmat.c\" \(4958 characters\)
sed "s/^X//" >wildmat.c <<'END_OF_wildmat.c'
X/* $Source: /u/mark/src/pax/RCS/wildmat.c,v $
X *
X * $Revision: 1.1 $
X *
X * wildmat.c - simple regular expression pattern matching routines 
X *
X * DESCRIPTION 
X *
X * 	These routines provide simple UNIX style regular expression matching.  
X *	They were originally written by Rich Salz, the comp.sources.unix 
X *	moderator for inclusion in some of his software.  These routines 
X *	were released into the public domain and used by John Gilmore in 
X *	USTAR. 
X *
X * AUTHORS 
X *
X * 	Mark H. Colburn, NAPS International (mark@jhereg.mn.org) 
X * 	John Gilmore (gnu@hoptoad) 
X * 	Rich Salz (rs@uunet.uu.net) 
X *
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
X * $Log:	wildmat.c,v $
X * Revision 1.1  88/12/23  18:02:41  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: wildmat.c,v 1.1 88/12/23 18:02:41 mark Rel $";
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
Xstatic int star(char *, char *);
X#else  /* !__STDC__ */
Xstatic int      star();
X#endif /* __STDC__ */
X
X
X/*
X * star - handle trailing * in a regular expression 
X *
X * DESCRIPTION
X *
X *	Star is used to match filename expansions containing a trailing
X *	asterisk ('*').  Star call wildmat() to determine if the substring
X *	passed to it is matches the regular expression.
X *
X * PARAMETERS 
X *
X * 	char *source 	- The source string which is to be compared to the 
X *			  regular expression pattern. 
X * 	char *pattern 	- The regular expression which we are supposed to 
X *			  match to. 
X *
X * RETURNS 
X *
X * 	Returns non-zero if the entire source string is completely matched by 
X *	the regular expression pattern, returns 0 otherwise. This is used to 
X *	see if *'s in a pattern matched the entire source string. 
X *
X */
X
X#ifdef __STDC__
X
Xstatic int star(char *source, char *pattern)
X
X#else
X
Xstatic int star(source, pattern)
Xchar           *source;		/* source operand */
Xchar           *pattern;	/* regular expression to match */
X
X#endif
X{
X    while (!wildmat(source, pattern)) {
X	if (*++source == '\0') {
X	    return (0);
X	}
X    }
X    return (1);
X}
X
X
X/*
X * wildmat - match a regular expression 
X *
X * DESCRIPTION
X *
X *	Wildmat attempts to match the string pointed to by source to the 
X *	regular expression pointed to by pattern.  The subset of regular 
X *	expression syntax which is supported is defined by POSIX P1003.2 
X *	FILENAME EXPANSION rules.
X *
X * PARAMETERS 
X *
X * 	char *source 	- The source string which is to be compared to the 
X *			  regular expression pattern. 
X * 	char *pattern 	- The regular expression which we are supposed to 
X *			  match to. 
X *
X * RETURNS 
X *
X * 	Returns non-zero if the source string matches the regular expression 
X *	pattern specified, returns 0 otherwise. 
X *
X */
X
X#ifdef __STDC__
X
Xint wildmat(char *pattern, char *source)
X
X#else
X
Xint wildmat(pattern, source)
Xchar           *pattern;	/* regular expression to match */
Xchar           *source;		/* source operand */
X
X#endif
X{
X    int             last;	/* last character matched */
X    int             matched;	/* !0 if a match occurred */
X    int             reverse;	/* !0 if sense of match is reversed */
X
X    for (; *pattern; source++, pattern++) {
X	switch (*pattern) {
X	case '\\':
X	    /* Literal match with following character */
X	    pattern++;
X	    /* FALLTHRU */
X	default:
X	    if (*source != *pattern) {
X		return (0);
X	    }
X	    continue;
X	case '?':
X	    /* Match anything. */
X	    if (*source == '\0') {
X		return (0);
X	    }
X	    continue;
X	case '*':
X	    /* Trailing star matches everything. */
X	    return (*++pattern ? star(source, pattern) : 1);
X	case '[':
X	    /* [^....] means inverse character class. */
X	    if (reverse = pattern[1] == '^') {
X		pattern++;
X	    }
X	    for (last = 0400, matched = 0;
X		 *++pattern && *pattern != ']'; last = *pattern) {
X		/* This next line requires a good C compiler. */
X		if (*pattern == '-'
X		    ? *source <= *++pattern && *source >= last
X		    : *source == *pattern) {
X		    matched = 1;
X		}
X	    }
X	    if (matched == reverse) {
X		return (0);
X	    }
X	    continue;
X	}
X    }
X
X    /*
X     * For "tar" use, matches that end at a slash also work. --hoptoad!gnu 
X     */
X    return (*source == '\0' || *source == '/');
X}
END_OF_wildmat.c
if test 4958 -ne `wc -c <wildmat.c`; then
    echo shar: \"wildmat.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
echo shar: End of archive 1 \(of 6\).
cp /dev/null ark1isdone
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

