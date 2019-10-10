# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 3 (of 6)."
# Contents:  fileio.c namelist.c pax.c pax.h tar.c
# Wrapped by mark@jhereg on Tue Dec 27 19:37:45 1988
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f fileio.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"fileio.c\"
else
echo shar: Extracting \"fileio.c\" \(10596 characters\)
sed "s/^X//" >fileio.c <<'END_OF_fileio.c'
X/* $Source: /u/mark/src/pax/RCS/fileio.c,v $
X *
X * $Revision: 1.1 $
X *
X * fileio.c - file I/O functions for all archive interfaces
X *
X * DESCRIPTION
X *
X *	These function all do I/O of some form or another.  They are
X *	grouped here mainly for convienence.
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
X * $Log:	fileio.c,v $
X * Revision 1.1  88/12/23  18:02:09  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: fileio.c,v 1.1 88/12/23 18:02:09 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* open_archive -  open an archive file.  
X *
X * DESCRIPTION
X *
X *	Open_archive will open an archive file for reading or writing,
X *	setting the proper file mode, depending on the "mode" passed to
X *	it.  All buffer pointers are reset according to the mode
X *	specified.
X *
X * PARAMETERS
X *
X * 	int	mode 	- specifies whether we are reading or writing.   
X *
X * RETURNS
X *
X *	Returns a zero if successfull, or -1 if an error occured during 
X *	the open.
X */
X
X#ifdef __STDC__
X    
Xint open_archive(int mode)
X
X#else
X    
Xint open_archive(mode)
Xint             mode;
X
X#endif
X{
X    if (ar_file[0] == '-' && ar_file[1] == '\0') {
X	if (mode == AR_READ) {
X	    archivefd = STDIN;
X	    bufend = bufidx = bufstart;
X	} else {
X	    archivefd = STDOUT;
X	}
X    } else if (mode == AR_READ) {
X	archivefd = open(ar_file, O_RDONLY | O_BINARY);
X	bufend = bufidx = bufstart;	/* set up for initial read */
X    } else if (mode == AR_WRITE) {
X	archivefd = open(ar_file, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0666);
X    } else if (mode == AR_APPEND) {
X	archivefd = open(ar_file, O_RDWR | O_BINARY, 0666);
X	bufend = bufidx = bufstart;	/* set up for initial read */
X    }
X
X    if (archivefd < 0) {
X	warnarch(syserr(), (OFFSET) 0);
X	return (-1);
X    }
X    ++arvolume;
X    return (0);
X}
X
X
X/* close_archive - close the archive file
X *
X * DESCRIPTION
X *
X *	Closes the current archive and resets the archive end of file
X *	marker.
X */
X
X#ifdef __STDC__
X
Xvoid close_archive(void)
X
X#else
X    
Xvoid close_archive()
X
X#endif
X{
X    if (archivefd != STDIN && archivefd != STDOUT) {
X	close(archivefd);
X    }
X    areof = 0;
X}
X
X
X/* openo - open an output file
X *
X * DESCRIPTION
X *
X *	Openo opens the named file for output.  The file mode and type are
X *	set based on the values stored in the stat structure for the file.
X *	If the file is a special file, then no data will be written, the
X *	file/directory/Fifo, etc., will just be created.  Appropriate
X *	permission may be required to create special files.
X *
X * PARAMETERS
X *
X *	char 	*name		- The name of the file to create
X *	Stat	*asb		- Stat structure for the file
X *	Link	*linkp;		- pointer to link chain for this file
X *	int	 ispass		- true if we are operating in "pass" mode
X *
X * RETURNS
X *
X * 	Returns the output file descriptor, 0 if no data is required or -1 
X *	if unsuccessful. Note that UNIX open() will never return 0 because 
X *	the standard input is in use. 
X */
X
X#ifdef __STDC__
X
Xint openo(char *name, Stat *asb, Link *linkp, int ispass)
X
X#else
X    
Xint openo(name, asb, linkp, ispass)
Xchar           *name;
XStat           *asb;
XLink           *linkp;
Xint             ispass;
X
X#endif
X{
X    int             exists;
X    int             fd;
X    ushort          perm;
X    ushort          operm = 0;
X    Stat            osb;
X#ifdef	S_IFLNK
X    int             ssize;
X    char            sname[PATH_MAX + 1];
X#endif	/* S_IFLNK */
X
X    if (exists = (LSTAT(name, &osb) == 0)) {
X	if (ispass && osb.sb_ino == asb->sb_ino && osb.sb_dev == asb->sb_dev) {
X	    warn(name, "Same file");
X	    return (-1);
X	} else if ((osb.sb_mode & S_IFMT) == (asb->sb_mode & S_IFMT)) {
X	    operm = osb.sb_mode & S_IPERM;
X	} else if (REMOVE(name, &osb) < 0) {
X	    warn(name, syserr());
X	    return (-1);
X	} else {
X	    exists = 0;
X	}
X    }
X    if (linkp) {
X	if (exists) {
X	    if (asb->sb_ino == osb.sb_ino && asb->sb_dev == osb.sb_dev) {
X		return (0);
X	    } else if (unlink(name) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    } else {
X		exists = 0;
X	    }
X	}
X	if (link(linkp->l_name, name) != 0) {
X	    if (errno == ENOENT) {
X		if (f_create_dirs) {
X		    if (dirneed(name) != 0 ||
X			    link(linkp->l_name, name) != 0) {
X			    warn(name, syserr());
X			return (-1);
X		    }
X		} else {
X		    warn(name, 
X			     "Directories are not being created (-d option)");
X		}
X		return(0);
X	    } else if (errno != EXDEV) {
X		warn(name, syserr());
X		return (-1);
X	    }
X	} else {
X	    return(0);
X	} 
X    }
X    perm = asb->sb_mode & S_IPERM;
X    switch (asb->sb_mode & S_IFMT) {
X    case S_IFBLK:
X    case S_IFCHR:
X	fd = 0;
X	if (exists) {
X	    if (asb->sb_rdev == osb.sb_rdev) {
X		if (perm != operm && chmod(name, (int) perm) < 0) {
X		    warn(name, syserr());
X		    return (-1);
X		} else {
X		    break;
X		}
X	    } else if (REMOVE(name, &osb) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    } else {
X		exists = 0;
X	    }
X	}
X	if (mknod(name, (int) asb->sb_mode, (int) asb->sb_rdev) < 0) {
X	    if (errno == ENOENT) {
X		if (f_create_dirs) {
X		    if (dirneed(name) < 0 || mknod(name, (int) asb->sb_mode, 
X			   (int) asb->sb_rdev) < 0) {
X			warn(name, syserr());
X			return (-1);
X		    }
X		} else {
X		    warn(name, "Directories are not being created (-d option)");
X		}
X	    } else {
X		warn(name, syserr());
X		return (-1);
X	    }
X	}
X	return(0);
X	break;
X    case S_IFDIR:
X	if (exists) {
X	    if (perm != operm && chmod(name, (int) perm) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    }
X	} else if (f_create_dirs) {
X	    if (dirmake(name, asb) < 0 || dirneed(name) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    }
X	} else {
X	    warn(name, "Directories are not being created (-d option)");
X	}
X	return (0);
X#ifdef	S_IFIFO
X    case S_IFIFO:
X	fd = 0;
X	if (exists) {
X	    if (perm != operm && chmod(name, (int) perm) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    }
X	} else if (mknod(name, (int) asb->sb_mode, 0) < 0) {
X	    if (errno == ENOENT) {
X		if (f_create_dirs) {
X		    if (dirneed(name) < 0
X		       || mknod(name, (int) asb->sb_mode, 0) < 0) {
X			warn(name, syserr());
X			return (-1);
X		    }
X		} else {
X		    warn(name, "Directories are not being created (-d option)");
X		}
X	    } else {
X		warn(name, syserr());
X		return (-1);
X	    }
X	}
X	return(0);
X	break;
X#endif				/* S_IFIFO */
X#ifdef	S_IFLNK
X    case S_IFLNK:
X	if (exists) {
X	    if ((ssize = readlink(name, sname, sizeof(sname))) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    } else if (strncmp(sname, asb->sb_link, ssize) == 0) {
X		return (0);
X	    } else if (REMOVE(name, &osb) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    } else {
X		exists = 0;
X	    }
X	}
X	if (symlink(asb->sb_link, name) < 0) {
X	    if (errno == ENOENT) {
X		if (f_create_dirs) {
X		    if (dirneed(name) < 0 || symlink(asb->sb_link, name) < 0) {
X			warn(name, syserr());
X			return (-1);
X		    }
X		} else {
X		    warn(name, "Directories are not being created (-d option)");
X		}
X	    } else {
X		warn(name, syserr());
X		return (-1);
X	    }
X	}
X	return (0);		/* Can't chown()/chmod() a symbolic link */
X#endif				/* S_IFLNK */
X    case S_IFREG:
X	if (exists) {
X	    if (!f_unconditional && osb.sb_mtime > asb->sb_mtime) {
X		warn(name, "Newer file exists");
X		return (-1);
X	    } else if (unlink(name) < 0) {
X		warn(name, syserr());
X		return (-1);
X	    } else {
X		exists = 0;
X	    }
X	}
X	if ((fd = creat(name, (int) perm)) < 0) {
X	    if (errno == ENOENT) {
X		if (f_create_dirs) {
X		    if (dirneed(name) < 0 || 
X			    (fd = creat(name, (int) perm)) < 0) {
X			warn(name, syserr());
X			return (-1);
X		    }
X		} else {
X		    /* 
X		     * the file requires a directory which does not exist
X		     * and which the user does not want created, so skip
X		     * the file...
X		     */
X		    warn(name, "Directories are not being created (-d option)");
X		    return(0);
X		}
X	    } else {
X		warn(name, syserr());
X		return (-1);
X	    }
X	}
X	break;
X    default:
X	warn(name, "Unknown filetype");
X	return (-1);
X    }
X    if (f_owner) {
X	if (!exists || asb->sb_uid != osb.sb_uid || asb->sb_gid != osb.sb_gid) {
X	    chown(name, (int) asb->sb_uid, (int) asb->sb_gid);
X	}
X    }
X    return (fd);
X}
X
X
X/* openi - open the next input file
X *
X * DESCRIPTION
X *
X *	Openi will attempt to open the next file for input.  If the file is
X *	a special file, such as a directory, FIFO, link, character- or
X *	block-special file, then the file size field of the stat structure
X *	is zeroed to make sure that no data is written out for the file.
X *	If the file is a special file, then a file descriptor of 0 is
X *	returned to the caller, which is handled specially.  If the file
X *	is a regular file, then the file is opened and a file descriptor
X *	to the open file is returned to the caller.
X *
X * PARAMETERS
X *
X *	char   *name	- pointer to the name of the file to open
X *	Stat   *asb	- pointer to the stat block for the file to open
X *
X * RETURNS
X *
X * 	Returns a file descriptor, 0 if no data exists, or -1 at EOF. This 
X *	kludge works because standard input is in use, preventing open() from 
X *	returning zero. 
X */
X
X#ifdef __STDC__
X
Xint openi(char *name, Stat *asb)
X
X#else
X    
Xint openi(name, asb)
Xchar           *name;		/* name of file to open */
XStat           *asb;		/* pointer to stat structure for file */
X
X#endif
X{
X    int             fd;
X
X    switch (asb->sb_mode & S_IFMT) {
X    case S_IFDIR:
X	asb->sb_nlink = 1;
X	asb->sb_size = 0;
X	return (0);
X#ifdef	S_IFLNK
X    case S_IFLNK:
X	if ((asb->sb_size = readlink(name,
X			     asb->sb_link, sizeof(asb->sb_link) - 1)) < 0) {
X	    warn(name, syserr());
X	    return(0);
X	}
X	asb->sb_link[asb->sb_size] = '\0';
X	return (0);
X#endif				/* S_IFLNK */
X    case S_IFREG:
X	if (asb->sb_size == 0) {
X	    return (0);
X	}
X	if ((fd = open(name, O_RDONLY | O_BINARY)) < 0) {
X	    warn(name, syserr());
X	}
X	return (fd);
X    default:
X	asb->sb_size = 0;
X	return (0);
X    }
X}
END_OF_fileio.c
if test 10596 -ne `wc -c <fileio.c`; then
    echo shar: \"fileio.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f namelist.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"namelist.c\"
else
echo shar: Extracting \"namelist.c\" \(11056 characters\)
sed "s/^X//" >namelist.c <<'END_OF_namelist.c'
X/* $Source: /u/mark/src/pax/RCS/namelist.c,v $
X *
X * $Revision: 1.1 $
X *
X * namelist.c - track filenames given as arguments to tar/cpio/pax
X *
X * DESCRIPTION
X *
X *	Arguments may be regular expressions, therefore all agurments will
X *	be treated as if they were regular expressions, even if they are
X *	not.
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
X * $Log:	namelist.c,v $
X * Revision 1.1  88/12/23  18:02:17  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: namelist.c,v 1.1 88/12/23 18:02:17 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* Type Definitions */
X
X/*
X * Structure for keeping track of filenames and lists thereof. 
X */
Xstruct nm_list {
X    struct nm_list *next;
X    short           length;	/* cached strlen(name) */
X    char            found;	/* A matching file has been found */
X    char            firstch;	/* First char is literally matched */
X    char            regexp;	/* regexp pattern for item */
X    char            name[1];	/* name of file or rexexp */
X};
X
Xstruct dirinfo {
X    char            dirname[PATH_MAX + 1];	/* name of directory */
X    OFFSET	    where;	/* current location in directory */
X    struct dirinfo *next;
X};
X
X
X/* Function Prototypes */
X
X#ifndef __STDC__
X
Xstatic void pushdir();
Xstatic struct dirinfo *popdir();
X
X#else
X
Xstatic void pushdir(struct dirinfo *info);
Xstatic struct dirinfo *popdir(void);
X
X#endif
X
X
X/* Internal Identifiers */
X
Xstatic struct nm_list *namelast;	/* Points to last name in list */
Xstatic struct nm_list *namelist;	/* Points to first name in list */
X
X
X/* addname -  add a name to the namelist. 
X *
X * DESCRIPTION
X *
X *	Addname adds the name given to the name list.  Memory for the
X *	namelist structure is dynamically allocated.  If the space for 
X *	the structure cannot be allocated, then the program will exit
X *	the an out of memory error message and a non-zero return code
X *	will be returned to the caller.
X *
X * PARAMETERS
X *
X *	char *name	- A pointer to the name to add to the list
X */
X
X#ifdef __STDC__
X
Xvoid add_name(char *name)
X
X#else
X    
Xvoid add_name(name)
Xchar           *name;		/* pointer to name */
X
X#endif
X{
X    int             i;		/* Length of string */
X    struct nm_list *p;		/* Current struct pointer */
X
X    i = strlen(name);
X    p = (struct nm_list *) malloc((unsigned) (i + sizeof(struct nm_list)));
X    if (!p) {
X	fatal("cannot allocate memory for namelist entry\n");
X    }
X    p->next = (struct nm_list *) NULL;
X    p->length = i;
X    strncpy(p->name, name, i);
X    p->name[i] = '\0';		/* Null term */
X    p->found = 0;
X    p->firstch = isalpha(name[0]);
X    if (strchr(name, '*') || strchr(name, '[') || strchr(name, '?')) {
X        p->regexp = 1;
X    }
X    if (namelast) {
X	namelast->next = p;
X    }
X    namelast = p;
X    if (!namelist) {
X	namelist = p;
X    }
X}
X
X
X/* name_match - match a name from an archive with a name from the namelist 
X *
X * DESCRIPTION
X *
X *	Name_match attempts to find a name pointed at by p in the namelist.
X *	If no namelist is available, then all filenames passed in are
X *	assumed to match the filename criteria.  Name_match knows how to
X *	match names with regular expressions, etc.
X *
X * PARAMETERS
X *
X *	char	*p	- the name to match
X *
X * RETURNS
X *
X *	Returns 1 if the name is in the namelist, or no name list is
X *	available, otherwise returns 0
X *
X */
X
X#ifdef __STDC__
X
Xint name_match(char *p)
X
X#else
X    
Xint name_match(p)
Xchar           *p;
X
X#endif
X{
X    struct nm_list *nlp;
X    int             len;
X
X    if ((nlp = namelist) == 0) {/* Empty namelist is easy */
X	return (1);
X    }
X    len = strlen(p);
X    for (; nlp != 0; nlp = nlp->next) {
X	/* If first chars don't match, quick skip */
X	if (nlp->firstch && nlp->name[0] != p[0]) {
X	    continue;
X	}
X	/* Regular expressions */
X	if (nlp->regexp) {
X	    if (wildmat(nlp->name, p)) {
X		nlp->found = 1;	/* Remember it matched */
X		return (1);	/* We got a match */
X	    }
X	    continue;
X	}
X	/* Plain Old Strings */
X	if (nlp->length <= len	/* Archive len >= specified */
X	    && (p[nlp->length] == '\0' || p[nlp->length] == '/')
X	    && strncmp(p, nlp->name, nlp->length) == 0) {
X	    /* Name compare */
X	    nlp->found = 1;	/* Remember it matched */
X	    return (1);		/* We got a match */
X	}
X    }
X    return (0);
X}
X
X
X/* names_notfound - print names of files in namelist that were not found 
X *
X * DESCRIPTION
X *
X *	Names_notfound scans through the namelist for any files which were
X *	named, but for which a matching file was not processed by the
X *	archive.  Each of the files is listed on the standard error.
X *
X */
X
X#ifdef __STDC__
X
Xvoid names_notfound(void)
X
X#else
X    
Xvoid names_notfound()
X
X#endif
X{
X    struct nm_list *nlp;
X
X    for (nlp = namelist; nlp != 0; nlp = nlp->next) {
X	if (!nlp->found) {
X	    fprintf(stderr, "%s: %s not found in archive\n",
X			   myname, nlp->name);
X	}
X	free(nlp);
X    }
X    namelist = (struct nm_list *) NULL;
X    namelast = (struct nm_list *) NULL;
X}
X
X
X/* name_init - set up to gather file names 
X *
X * DESCRIPTION
X *
X *	Name_init sets up the namelist pointers so that we may access the
X *	command line arguments.  At least the first item of the command
X *	line (argv[0]) is assumed to be stripped off, prior to the
X *	name_init call.
X *
X * PARAMETERS
X *
X *	int	argc	- number of items in argc
X *	char	**argv	- pointer to the command line arguments
X */
X
X#ifdef __STDC__
X
Xvoid name_init(int argc, char **argv)
X
X#else
X    
Xvoid name_init(argc, argv)
Xint             argc;
Xchar          **argv;
X
X#endif
X{
X    /* Get file names from argv, after options. */
X    n_argc = argc;
X    n_argv = argv;
X}
X
X
X/* name_next - get the next name from argv or the name file. 
X *
X * DESCRIPTION
X *
X *	Name next finds the next name which is to be processed in the
X *	archive.  If the named file is a directory, then the directory
X *	is recursively traversed for additional file names.  Directory
X *	names and locations within the directory are kept track of by
X *	using a directory stack.  See the pushdir/popdir function for
X *	more details.
X *
X * 	The names come from argv, after options or from the standard input.  
X *
X * PARAMETERS
X *
X *	name - a pointer to a buffer of at least MAX_PATH + 1 bytes long;
X *	statbuf - a pointer to a stat structure
X *
X * RETURNS
X *
X *	Returns -1 if there are no names left, (e.g. EOF), otherwise returns 
X *	0 
X */
X
X#ifdef __STDC__
X
Xint name_next(char *name, Stat *statbuf)
X
X#else
X    
Xint name_next(name, statbuf)
Xchar           *name;
XStat           *statbuf;
X
X#endif
X{
X    int             err = -1;
X    static int      in_subdir = 0;
X    static DIR     *dirp;
X    struct dirent  *d;
X    static struct dirinfo *curr_dir;
X    int			len;
X
X    do {
X	if (names_from_stdin) {
X	    if (lineget(stdin, name) < 0) {
X		return (-1);
X	    }
X	    if (nameopt(name) < 0) {
X		continue;
X	    }
X	} else {
X	    if (in_subdir) {
X		if ((d = readdir(dirp)) != NULL) {
X		    /* Skip . and .. */
X		    if (strcmp(d->d_name, ".") == 0 ||
X		        strcmp(d->d_name, "..") == 0) {
X			    continue;
X		    }
X		    if (strlen(d->d_name) + 
X			strlen(curr_dir->dirname) >= PATH_MAX) {
X			warn("name too long", d->d_name);
X			continue;
X		    }
X		    strcpy(name, curr_dir->dirname);
X		    strcat(name, d->d_name);
X		} else {
X		    closedir(dirp);
X		    in_subdir--;
X		    curr_dir = popdir();
X		    if (in_subdir) {
X			errno = 0;
X			if ((dirp = opendir(curr_dir->dirname)) == NULL) {
X			    warn(curr_dir->dirname, "error opening directory (1)");
X			    in_subdir--;
X			}
X			seekdir(dirp, curr_dir->where);
X		    }
X		    continue;
X		}
X	    } else if (optind >= n_argc) {
X		return (-1);
X	    } else {
X		strcpy(name, n_argv[optind++]);
X	    }
X	}
X	if ((err = LSTAT(name, statbuf)) < 0) {
X	    warn(name, syserr());
X	    continue;
X	}
X	if (!names_from_stdin && (statbuf->sb_mode & S_IFMT) == S_IFDIR) {
X	    if (in_subdir) {
X		curr_dir->where = telldir(dirp);
X		pushdir(curr_dir);
X		close(dirp);
X	    } 
X	    in_subdir++;
X
X	    /* Build new prototype name */
X	    if ((curr_dir = (struct dirinfo *) 
X		    mem_get(sizeof(struct dirinfo))) == NULL) {
X		exit(2);
X	    }
X	    strcpy(curr_dir->dirname, name);
X	    len = strlen(curr_dir->dirname);
X	    while (len >= 1 && curr_dir->dirname[len - 1] == '/') {
X		len--;		/* Delete trailing slashes */
X	    }
X	    curr_dir->dirname[len++] = '/';	/* Now add exactly one back */
X	    curr_dir->dirname[len] = '\0';/* Make sure null-terminated */
X
X	    errno = 0;
X	    if ((dirp = opendir(curr_dir->dirname)) == NULL) {
X		warn(curr_dir->dirname, "error opening directory (2)");
X	    }
X	}
X    } while (err < 0);
X    return (0);
X}
X
X
X/* name_gather - gather names in a list for scanning. 
X *
X * DESCRIPTION
X *
X *	Name_gather takes names from the command line and adds them to
X *	the name list.
X *
X * FIXME
X *
X * 	We could hash the names if we really care about speed here.
X */
X
X#ifdef __STDC__
X
Xvoid name_gather(void)
X
X#else
X    
Xvoid name_gather()
X
X#endif
X{
X     while (optind < n_argc) { 
X	 add_name(n_argv[optind++]); 
X     } 
X}
X
X
Xstatic struct dirinfo *stack_head = NULL;
X
X
X/* pushdir - pushes a directory name on the directory stack
X *
X * DESCRIPTION
X *
X *	The pushdir function puses the directory structure which is pointed
X *	to by "info" onto a stack for later processing.  The information
X *	may be retrieved later with a call to popdir().
X *
X * PARAMETERS
X *
X *	dirinfo	*info	- pointer to directory structure to save
X */
X
X#ifdef __STDC__
X
Xstatic void pushdir(struct dirinfo *info)
X
X#else
X    
Xstatic void pushdir(info)
Xstruct dirinfo	*info;
X
X#endif
X{
X    if  (stack_head == NULL) {
X	stack_head = info;
X	stack_head->next = NULL;
X    } else {
X	info->next = stack_head;
X	stack_head = info;
X    } 
X}
X
X
X/* popdir - pop a directory structure off the directory stack.
X *
X * DESCRIPTION
X *
X *	The popdir function pops the most recently pushed directory
X *	structure off of the directory stack and returns it to the calling
X *	function.
X *
X * RETURNS
X *
X *	Returns a pointer to the most recently pushed directory structure
X *	or NULL if the stack is empty.
X */
X
X#ifdef __STDC__
X
Xstatic struct dirinfo *popdir(void)
X
X#else
X    
Xstatic struct dirinfo *popdir()
X
X#endif
X{
X    struct dirinfo	*tmp;
X
X    if (stack_head == NULL) {
X	return(NULL);
X    } else {
X	tmp = stack_head;
X	stack_head = stack_head->next;
X    }
X    return(tmp);
X}
END_OF_namelist.c
if test 11056 -ne `wc -c <namelist.c`; then
    echo shar: \"namelist.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f pax.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"pax.c\"
else
echo shar: Extracting \"pax.c\" \(13026 characters\)
sed "s/^X//" >pax.c <<'END_OF_pax.c'
X/* $Source: /u/mark/src/pax/RCS/pax.c,v $
X *
X * $Revision: 1.1 $
X *
X * DESCRIPTION
X *
X *	Pax is the archiver described in IEEE P1003.2.  It is an archiver
X *	which understands both tar and cpio archives and has a new interface.
X *
X * SYNOPSIS
X *
X *	pax -[cimopuvy] [-f archive] [-s replstr] [-t device] [pattern...]
X *	pax -r [-cimopuvy] [-f archive] [-s replstr] [-t device] [pattern...]
X *	pax -w [-adimuvy] [-b blocking] [-f archive] [-s replstr]...]
X *	       [-t device][-x format][pathname...]
X *	pax -r -w [-ilmopuvy][-s replstr][pathname...] directory
X *
X * DESCRIPTION
X *
X * 	PAX - POSIX conforming tar and cpio archive handler.  This
X *	program implements POSIX conformant versions of tar, cpio and pax
X *	archive handlers for UNIX.  These handlers have defined befined
X *	by the IEEE P1003.2 commitee.
X *
X * COMPILATION
X *
X *	A number of different compile time configuration options are
X *	available, please see the Makefile and config.h for more details.
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
X * $Log:	pax.c,v $
X * Revision 1.1  88/12/23  18:02:23  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: pax.c,v 1.1 88/12/23 18:02:23 mark Rel $";
Xstatic char *copyright = "Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.\n";
X#endif /* ! lint */
X
X
X/* Headers */
X
X#define NO_EXTERN
X#include "pax.h"
X
X
X/* Globally Available Identifiers */
X
Xchar           *ar_file;		/* File containing name of archive */
Xchar           *bufend;			/* End of data within archive buffer */
Xchar           *bufstart;		/* Archive buffer */
Xchar           *bufidx;			/* Archive buffer index */
Xchar           *myname;			/* name of executable (argv[0]) */
Xchar          **n_argv;			/* Argv used by name routines */
Xint             n_argc;			/* Argc used by name routines */
Xint             archivefd;		/* Archive file descriptor */
Xint             blocking;		/* Size of each block, in records */
Xint             gid;			/* Group ID */
Xint             head_standard;		/* true if archive is POSIX format */
Xint             ar_interface;		/* defines interface we are using */
Xint             ar_format;		/* defines current archve format */
Xint             mask;			/* File creation mask */
Xint             ttyf;			/* For interactive queries */
Xint             uid;			/* User ID */
Xint		names_from_stdin;	/* names for files are from stdin */
XOFFSET          total;			/* Total number of bytes transferred */
Xshort           f_access_time;		/* Reset access times of input files */
Xshort           areof;			/* End of input volume reached */
Xshort           f_create_dirs;		/* Create missing directories */
Xshort           f_append;		/* Add named files to end of archive */
Xshort           f_create;		/* create a new archive */
Xshort           f_extract;		/* Extract named files from archive */
Xshort           f_follow_links;		/* follow symbolic links */
Xshort           f_interactive;		/* Interactivly extract files */
Xshort           f_linksleft;		/* Report on unresolved links */
Xshort           f_list;			/* List files on the archive */
Xshort           f_modified;		/* Don't restore modification times */
Xshort           f_verbose;		/* Turn on verbose mode */
Xshort		f_link;			/* link files where possible */
Xshort		f_owner;		/* extract files as the user */
Xshort		f_pass;			/* pass files between directories */
Xshort           f_newer;		/* append files to archive if newer */
Xshort		f_disposition;		/* ask for file disposition */
Xshort           f_reverse_match;	/* Reverse sense of pattern match */
Xshort           f_modification_time;	/* Retain file modification time */
Xshort           f_unconditional;	/* Copy unconditionally */
Xtime_t          now = 0;		/* Current time */
Xuint            arvolume;		/* Volume number */
Xuint            blocksize = BLOCKSIZE;	/* Archive block size */
XFILE	       *msgfile;		/* message outpu file stdout/stderr */
XReplstr        *rplhead = NULL;		/* pointer to head of replstr list */
XReplstr        *rpltail;		/* pointer to tail of replstr list */
X
X
X/* Function Prototypes */
X
X#ifdef __STDC__
X
Xstatic void 	usage(void);
Xstatic OFFSET   pax_optsize(char *);
X
X#else /* !__STDC__ */
X
Xstatic void 	usage();
Xstatic OFFSET   pax_optsize();
X
X#endif /* __STDC__ */
X
X
X/* main - main routine for handling all archive formats.
X *
X * DESCRIPTION
X *
X * 	Set up globals and call the proper interface as specified by the user.
X *
X * PARAMETERS
X *
X *	int argc	- count of user supplied arguments
X *	char **argv	- user supplied arguments 
X *
X * RETURNS
X *
X *	Returns an exit code of 0 to the parent process.
X */
X
X#ifdef __STDC__
X
Xint main(int argc, char **argv)
X
X#else
X
Xint main(argc, argv)
Xint             argc;
Xchar          **argv;
X
X#endif
X{
X    /* strip the pathname off of the name of the executable */
X    if ((myname = strrchr(argv[0], '/')) != (char *)NULL) {
X	myname++;
X    } else {
X	myname = argv[0];
X    }
X
X    /* set upt for collecting other command line arguments */
X    name_init(argc, argv);
X
X    /* get all our necessary information */
X    mask = umask(0);
X    uid = getuid();
X    gid = getgid();
X    now = time((time_t *) 0);
X
X    /* open terminal for interactive queries */
X    ttyf = open_tty();
X
X    if (strcmp(myname, "tar")==0) {
X	do_tar(argc, argv);
X    } else if (strcmp(myname, "cpio")==0) {
X	do_cpio(argc, argv);
X    } else {
X	do_pax(argc, argv);
X    }
X    exit(0);
X    /* NOTREACHED */
X}
X
X
X/* do_pax - provide a PAX conformant user interface for archive handling
X *
X * DESCRIPTION
X *
X *	Process the command line parameters given, doing some minimal sanity
X *	checking, and then launch the specified archiving functions.
X *
X * PARAMETERS
X *
X *    int ac		- A count of arguments in av.  Should be passed argc 
X *			  from main
X *    char **av		- A pointer to an argument list.  Should be passed 
X *			  argv from main
X *
X * RETURNS
X *
X *    Normally returns 0.  If an error occurs, -1 is returned 
X *    and state is set to reflect the error.
X *
X */
X
X#ifdef __STDC__
X
Xint do_pax(int ac, char **av)
X
X#else
X
Xint do_pax(ac, av)
Xint             ac;		/* argument counter */
Xchar          **av;		/* arguments */
X
X#endif
X{
X    int             c;
X    char	   *dirname;
X    Stat	    st;
X
X    /* default input/output file for PAX is STDIN/STDOUT */
X    ar_file = "-";
X
X    /*
X     * set up the flags to reflect the default pax inteface.  Unfortunately
X     * the pax interface has several options which are completely opposite
X     * of the tar and/or cpio interfaces...
X     */
X    f_unconditional = 1;
X    f_modification_time = 1;
X    f_create_dirs = 1;
X    f_list = 1;
X    blocksize = 0;
X    blocking = 0;
X    ar_interface = PAX;
X    ar_format = TAR;	/* default interface if none given for -w */
X    msgfile=stdout;
X
X    while ((c = getopt(ac, av, "ab:cdf:ilmoprs:t:uvwx:y")) != EOF) {
X	switch (c) {
X	case 'a':
X	    f_append = 1;
X	    f_list = 0;
X	    break;
X	case 'b':
X	    if ((blocksize = pax_optsize(optarg)) == 0) {
X		fatal("Bad block size");
X	    }
X	    break;
X	case 'c':
X	    f_reverse_match = 1;
X	    break;
X	case 'd':
X	    f_create_dirs = 0;
X	    break;
X	case 'f':
X	    if (blocksize == 0) {
X		blocking = 1;
X		blocksize = 1 * BLOCKSIZE;
X	    }
X	    ar_file = optarg;
X	    break;
X	case 'i':
X	    f_interactive = 1;
X	    break;
X	case 'l':
X	    f_link = 1;
X	    break;
X	case 'm':
X	    f_modification_time = 0;
X	    break;
X	case 'o':
X	    f_owner = 1;
X	    break;
X	case 'p':
X	    f_access_time = 1;
X	    break;
X	case 'r':
X	    if (f_create) {
X		f_create = 0;
X		f_pass = 1;
X	    } else {
X		f_list = 0;
X		f_extract = 1;
X	    } 
X	    msgfile=stderr;
X	    break;
X	case 's':
X	    add_replstr(optarg);
X	    break;
X	case 't':
X	    if (blocksize == 0) {
X		blocking = 1;
X		blocksize = 10 * BLOCKSIZE;
X	    }
X	    ar_file = optarg;
X	    break;
X	case 'u':
X	    f_unconditional = 1;
X	    break;
X	case 'v':
X	    f_verbose = 1;
X	    break;
X	case 'w':
X	    if (f_extract) {
X		f_extract = 0;
X		f_pass = 1;
X	    } else {
X		f_list = 0;
X		f_create = 1;
X	    } 
X	    msgfile=stderr;
X	    break;
X	case 'x':
X	    if (strcmp(optarg, "ustar") == 0) {
X		ar_format = TAR;
X	    } else if (strcmp(optarg, "cpio") == 0) {
X		ar_format = CPIO;
X	    } else {
X		usage();
X	    }
X	    break;
X	case 'y':
X	    f_disposition = 1;
X	    break;
X	default:
X	    usage();
X	}
X    }
X
X    if (blocksize == 0) {
X	blocking = 1;
X	blocksize = blocking * BLOCKSIZE;
X    }
X    buf_allocate((OFFSET) blocksize);
X
X    if (f_extract || f_list) {
X	open_archive(AR_READ);
X	get_archive_type();
X	read_archive();
X    } else if (f_create) {
X	if (optind >= n_argc) {
X	    names_from_stdin++;		/* args from stdin */
X	}
X	open_archive(AR_WRITE);
X	create_archive();
X    } else if (f_append) {
X	open_archive(AR_APPEND);
X	get_archive_type();
X	append_archive();
X    } else if (f_pass && optind < n_argc) {
X	dirname = n_argv[--n_argc];
X	if (LSTAT(dirname, &st) < 0) {
X	    fatal(syserr());
X	}
X	if ((st.sb_mode & S_IFMT) != S_IFDIR) {
X	    fatal("Not a directory");
X	}
X	if (optind >= n_argc) {
X	    names_from_stdin++;		/* args from stdin */
X	}
X	pass(dirname);
X    } else {
X	usage();
X    }
X
X    return (0);
X}
X
X
X/* get_archive_type - determine input archive type from archive header
X *
X * DESCRIPTION
X *
X * 	reads the first block of the archive and determines the archive 
X *	type from the data.  If the archive type cannot be determined, 
X *	processing stops, and a 1 is returned to the caller.  If verbose
X *	mode is on, then the archive type will be printed on the standard
X *	error device as it is determined.
X *
X * FIXME 
X *
X *	be able to understand TAR and CPIO magic numbers
X */
X
X#ifdef __STDC__
X
Xvoid get_archive_type(void)
X
X#else
X
Xvoid get_archive_type()
X
X#endif
X{
X    if (ar_read() != 0) {
X	fatal("Unable to determine archive type.");
X    }
X    if (strncmp(bufstart, "070707", 6) == 0) {
X	ar_format = CPIO;
X	if (f_verbose) {
X	    fputs("CPIO format archive\n", stderr);
X	}
X    } else if (strncmp(&bufstart[257], "ustar", 5) == 0) {
X	ar_format = TAR;
X	if (f_verbose) {
X	    fputs("USTAR format archive\n", stderr);
X	}
X    } else {
X	ar_format = TAR;
X    }
X}
X
X
X/* pax_optsize - interpret a size argument
X *
X * DESCRIPTION
X *
X * 	Recognizes suffixes for blocks (512-bytes), k-bytes and megabytes.  
X * 	Also handles simple expressions containing '+' for addition.
X *
X * PARAMETERS
X *
X *    char 	*str	- A pointer to the string to interpret
X *
X * RETURNS
X *
X *    Normally returns the value represented by the expression in the 
X *    the string.
X *
X * ERRORS
X *
X *	If the string cannot be interpretted, the program will fail, since
X *	the buffering will be incorrect.
X *
X */
X
X#ifdef __STDC__
X
Xstatic OFFSET pax_optsize(char *str)
X
X#else
X
Xstatic OFFSET pax_optsize(str)
Xchar           *str;		/* pointer to string to interpret */
X
X#endif
X{
X    char           *idx;
X    OFFSET          number;	/* temporary storage for current number */
X    OFFSET          result;	/* cumulative total to be returned to caller */
X
X    result = 0;
X    idx = str;
X    for (;;) {
X	number = 0;
X	while (*idx >= '0' && *idx <= '9')
X	    number = number * 10 + *idx++ - '0';
X	switch (*idx++) {
X	case 'b':
X	    result += number * 512;
X	    continue;
X	case 'k':
X	    result += number * 1024;
X	    continue;
X	case 'm':
X	    result += number * 1024 * 1024;
X	    continue;
X	case '+':
X	    result += number;
X	    continue;
X	case '\0':
X	    result += number;
X	    break;
X	default:
X	    break;
X	}
X	break;
X    }
X    if (*--idx) {
X	fatal("Unrecognizable value");
X    }
X    return (result);
X}
X
X
X/* usage - print a helpful message and exit
X *
X * DESCRIPTION
X *
X *	Usage prints out the usage message for the PAX interface and then
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
XUsage: %s -[cimopuvy] [-f archive] [-s replstr] [-t device] [pattern...]\n",
X	myname);
X    fprintf(stderr, "\
X       %s -r [-cimopuvy] [-f archive] [-s replstr] [-t device] [pattern...]\n",
X	myname);
X    fprintf(stderr, "\
X       %s -w [-adimuvy] [-b blocking] [-f archive] [-s replstr]\n\
X	   [-t device] [-x format] [pathname...]\n",
X	myname);
X    fprintf(stderr, "\
X       %s -r -w [-ilmopuvy] [-s replstr] [pathname...] directory\n",
X	myname);
X    exit(1);
X}
END_OF_pax.c
if test 13026 -ne `wc -c <pax.c`; then
    echo shar: \"pax.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f pax.h -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"pax.h\"
else
echo shar: Extracting \"pax.h\" \(9317 characters\)
sed "s/^X//" >pax.h <<'END_OF_pax.h'
X/* $Source: /u/mark/src/pax/RCS/pax.h,v $
X *
X * $Revision: 1.1 $
X *
X * pax.h - defnitions for entire program
X *
X * DESCRIPTION
X *
X *	This file contains most all of the definitions required by the PAX
X *	software.  This header is included in every source file.
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
X#ifndef _PAX_H
X#define _PAX_H
X
X/* Headers */
X
X#include "config.h"
X#include "limits.h"
X#include <stdio.h>
X#include <errno.h>
X#include <signal.h>
X#include <ctype.h>
X#include <sys/types.h>
X#include <sys/ioctl.h>
X#include <sys/stat.h>
X#include "regexp.h"
X
X#if defined(DIRENT) || defined(_POSIX_SOURCE)
X# ifdef PAXDIR
X#  include "paxdir.h"
X# else
X#  include <dirent.h>
X# endif
X#else
X# include <sys/dir.h>
X# define dirent direct
X#endif
X
X#ifndef	major
X#   include <sys/sysmacros.h>
X#endif				/* major */
X
X#ifdef	SYSTIME
X#   include <sys/time.h>
X#else				/* SYSTIME */
X#   include <time.h>
X#endif				/* SYSTIME */
X
X#ifndef V7
X#   include <fcntl.h>
X#endif
X
X#ifdef XENIX
X#   include <sys/inode.h>
X#endif
X
X#include <pwd.h>
X#include <grp.h>
X#include <sys/file.h>
X
X/* Defines */
X
X#define	STDIN	0		/* Standard input  file descriptor */
X#define	STDOUT	1		/* Standard output file descriptor */
X
X/*
X * Open modes; there is no <fcntl.h> with v7 UNIX and other versions of
X * UNIX may not have all of these defined...
X */
X
X#ifndef O_RDONLY
X#   define	O_RDONLY	0
X#endif
X
X#ifndef O_WRONLY
X#   define	O_WRONLY	1
X#endif
X
X#ifndef O_RDWR
X#   define	O_WRONLY	2
X#endif
X
X#ifndef	O_BINARY
X#   define	O_BINARY	0
X#endif
X
X#ifndef NULL
X#   define 	NULL 		0
X#endif
X
X#define TMAGIC		"ustar"		/* ustar and a null */
X#define TMAGLEN		6
X#define TVERSION	"00"		/* 00 and no null */
X#define TVERSLEN	2
X
X/* Values used in typeflag field */
X#define REGTYPE		'0'		/* Regular File */
X#define AREGTYPE	'\0'		/* Regular File */
X#define LNKTYPE		'1'		/* Link */
X#define SYMTYPE		'2'		/* Reserved */
X#define CHRTYPE		'3'		/* Character Special File */
X#define BLKTYPE		'4'		/* Block Special File */
X#define DIRTYPE		'5'		/* Directory */
X#define FIFOTYPE	'6'		/* FIFO */
X#define CONTTYPE	'7'		/* Reserved */
X
X#define BLOCKSIZE	512	/* all output is padded to 512 bytes */
X#define	uint	unsigned int	/* Not always in types.h */
X#define	ushort	unsigned short	/* Not always in types.h */
X#define	BLOCK	5120		/* Default archive block size */
X#define	H_COUNT	10		/* Number of items in ASCII header */
X#define	H_PRINT	"%06o%06o%06o%06o%06o%06o%06o%011lo%06o%011lo"
X#define	H_SCAN	"%6ho%6ho%6ho%6ho%6ho%6ho%6ho%11lo%6o%11lo"
X#define	H_STRLEN 70		/* ASCII header string length */
X#define	M_ASCII "070707"	/* ASCII magic number */
X#define	M_BINARY 070707		/* Binary magic number */
X#define	M_STRLEN 6		/* ASCII magic number length */
X#define	PATHELEM 256		/* Pathname element count limit */
X#define	S_IFSHF	12		/* File type shift (shb in stat.h) */
X#define	S_IPERM	07777		/* File permission bits (shb in stat.h) */
X#define	S_IPEXE	07000		/* Special execution bits (shb in stat.h) */
X#define	S_IPOPN	0777		/* Open access bits (shb in stat.h) */
X
X/*
X * Trailer pathnames. All must be of the same length. 
X */
X#define	TRAILER	"TRAILER!!!"	/* Archive trailer (cpio compatible) */
X#define	TRAILZ	11		/* Trailer pathname length (including null) */
X
X#include "port.h"
X
X
X#define	TAR		1
X#define	CPIO		2
X#define	PAX		3
X
X#define AR_READ 	0
X#define AR_WRITE 	1
X#define AR_EXTRACT	2
X#define AR_APPEND 	4
X
X/*
X * Header block on tape. 
X */
X#define	NAMSIZ		100
X#define	PFIXSIZ		155
X#define	TUNMLEN		32
X#define	TGNMLEN		32
X
X/* The checksum field is filled with this while the checksum is computed. */
X#define	CHKBLANKS	"        "	/* 8 blanks, no null */
X
X/*
X * Exit codes from the "tar" program 
X */
X#define	EX_SUCCESS	0	/* success! */
X#define	EX_ARGSBAD	1	/* invalid args */
X#define	EX_BADFILE	2	/* invalid filename */
X#define	EX_BADARCH	3	/* bad archive */
X#define	EX_SYSTEM	4	/* system gave unexpected error */
X
X#define	ROUNDUP(a,b) 	(((a) % (b)) == 0 ? (a) : ((a) + ((b) - ((a) % (b)))))
X
X/*
X * Mininum value. 
X */
X#define	MIN(a, b)	(((a) < (b)) ? (a) : (b))
X
X/*
X * Remove a file or directory. 
X */
X#define	REMOVE(name, asb) \
X	(((asb)->sb_mode & S_IFMT) == S_IFDIR ? rmdir(name) : unlink(name))
X
X/*
X * Cast and reduce to unsigned short. 
X */
X#define	USH(n)		(((ushort) (n)) & 0177777)
X
X
X/* Type Definitions */
X
X/*
X * Binary archive header (obsolete). 
X */
Xtypedef struct {
X    short           b_dev;	/* Device code */
X    ushort          b_ino;	/* Inode number */
X    ushort          b_mode;	/* Type and permissions */
X    ushort          b_uid;	/* Owner */
X    ushort          b_gid;	/* Group */
X    short           b_nlink;	/* Number of links */
X    short           b_rdev;	/* Real device */
X    ushort          b_mtime[2];	/* Modification time (hi/lo) */
X    ushort          b_name;	/* Length of pathname (with null) */
X    ushort          b_size[2];	/* Length of data */
X} Binary;
X
X/*
X * File status with symbolic links. Kludged to hold symbolic link pathname
X * within structure. 
X */
Xtypedef struct {
X    struct stat     sb_stat;
X    char            sb_link[PATH_MAX + 1];
X} Stat;
X
X#define	STAT(name, asb)		stat(name, &(asb)->sb_stat)
X#define	FSTAT(fd, asb)		fstat(fd, &(asb)->sb_stat)
X
X#define	sb_dev		sb_stat.st_dev
X#define	sb_ino		sb_stat.st_ino
X#define	sb_mode		sb_stat.st_mode
X#define	sb_nlink	sb_stat.st_nlink
X#define	sb_uid		sb_stat.st_uid
X#define	sb_gid		sb_stat.st_gid
X#define	sb_rdev		sb_stat.st_rdev
X#define	sb_size		sb_stat.st_size
X#define	sb_atime	sb_stat.st_atime
X#define	sb_mtime	sb_stat.st_mtime
X#define	sb_ctime	sb_stat.st_ctime
X
X#ifdef	S_IFLNK
X#	define	LSTAT(name, asb)	lstat(name, &(asb)->sb_stat)
X#	define	sb_blksize	sb_stat.st_blksize
X#	define	sb_blocks	sb_stat.st_blocks
X#else				/* S_IFLNK */
X/*
X * File status without symbolic links. 
X */
X#	define	LSTAT(name, asb)	stat(name, &(asb)->sb_stat)
X#endif				/* S_IFLNK */
X
X/*
X * Hard link sources. One or more are chained from each link structure. 
X */
Xtypedef struct name {
X    struct name    *p_forw;	/* Forward chain (terminated) */
X    struct name    *p_back;	/* Backward chain (circular) */
X    char           *p_name;	/* Pathname to link from */
X} Path;
X
X/*
X * File linking information. One entry exists for each unique file with with
X * outstanding hard links. 
X */
Xtypedef struct link {
X    struct link    *l_forw;	/* Forward chain (terminated) */
X    struct link    *l_back;	/* Backward chain (terminated) */
X    dev_t           l_dev;	/* Device */
X    ino_t           l_ino;	/* Inode */
X    ushort          l_nlink;	/* Unresolved link count */
X    OFFSET          l_size;	/* Length */
X    char	   *l_name;	/* pathname to link from */
X    Path           *l_path;	/* Pathname which link to l_name */
X} Link;
X
X/*
X * Structure for ed-style replacement strings (-s option).
X*/
Xtypedef struct replstr {
X    regexp	   *comp;	/* compiled regular expression */
X    char	   *replace;	/* replacement string */
X    char	    print;	/* >0 if we are to print replacement */
X    char	    global;	/* >0 if we are to replace globally */
X    struct replstr *next;	/* pointer to next record */
X} Replstr;
X
X
X/*
X * This has to be included here to insure that all of the type 
X * delcarations are declared for the prototypes.
X */
X#include "func.h"
X
X
X#ifndef NO_EXTERN
X/* Globally Available Identifiers */
X
Xextern char    *ar_file;
Xextern char    *bufend;
Xextern char    *bufstart;
Xextern char    *bufidx;
Xextern char    *myname;
Xextern int      archivefd;
Xextern int      blocking;
Xextern uint     blocksize;
Xextern int      gid;
Xextern int      head_standard;
Xextern int      ar_interface;
Xextern int      ar_format;
Xextern int      mask;
Xextern int      ttyf;
Xextern int      uid;
Xextern OFFSET    total;
Xextern short    areof;
Xextern short    f_append;
Xextern short    f_create;
Xextern short    f_extract;
Xextern short    f_follow_links;
Xextern short    f_interactive;
Xextern short    f_linksleft;
Xextern short    f_list;
Xextern short    f_modified;
Xextern short    f_verbose;
Xextern short	f_link;
Xextern short	f_owner;
Xextern short	f_access_time;
Xextern short	f_pass;
Xextern short	f_pass;
Xextern short	f_disposition;
Xextern short    f_reverse_match;
Xextern short    f_modification_time;
Xextern short    f_create_dirs;
Xextern short    f_unconditional;
Xextern short    f_newer;
Xextern time_t   now;
Xextern uint     arvolume;
Xextern int	names_from_stdin;
Xextern Replstr *rplhead;
Xextern Replstr *rpltail;
Xextern char   **n_argv;
Xextern int      n_argc;
Xextern FILE    *msgfile;
X#endif /* NO_EXTERN */
X
Xextern char    *optarg;
Xextern int      optind;
Xextern int      sys_nerr;
Xextern char    *sys_errlist[];
Xextern int      errno;
X
X#endif /* _PAX_H */
END_OF_pax.h
if test 9317 -ne `wc -c <pax.h`; then
    echo shar: \"pax.h\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f tar.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"tar.c\"
else
echo shar: Extracting \"tar.c\" \(7881 characters\)
sed "s/^X//" >tar.c <<'END_OF_tar.c'
X/* $Source: /u/mark/src/pax/RCS/tar.c,v $
X *
X * $Revision: 1.1 $
X *
X * tar.c - tar specific functions for archive handling
X *
X * DESCRIPTION
X *
X *	These routines provide a tar conforming interface to the pax
X *	program.
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
X * software was developed by Mark H. Colburn and sponsored by The 
X * USENIX Association. 
X *
X * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
X * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
X * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
X *
X * $Log:	tar.c,v $
X * Revision 1.1  88/12/23  18:02:38  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: tar.c,v 1.1 88/12/23 18:02:38 mark Rel $";
Xstatic char *copyright ="Copyright (c) 1989 Mark H. Colburn.\nAll rights reserved.";
X#endif /* not lint */
X
X/* Headers */
X
X#include "pax.h"
X
X
X/* Defines */
X
X#define DEF_BLOCKING	20	/* default blocking factor for extract */
X
X
X/* Function Prototypes */
X
X#ifdef __STDC__
X
Xstatic int taropt(int , char **, char *);
Xstatic void usage(void);
X
X#else /* !__STDC__ */
X
Xstatic int taropt();
Xstatic void usage();
X
X#endif /* __STDC__ */
X
X
X/* do_tar - main routine for tar. 
X *
X * DESCRIPTION
X *
X *	Provides a tar interface to the PAX program.  All tar standard
X *	command line options are supported.
X *
X * PARAMETERS
X *
X *	int argc	- argument count (argc from main) 
X *	char **argv	- argument list (argv from main) 
X *
X * RETURNS
X *
X *	zero
X */
X
X#ifdef __STDC__
X
Xint do_tar(int argc, char **argv)
X
X#else
X
Xint do_tar(argc, argv)
Xint             argc;		/* argument count (argc from main) */
Xchar          **argv;		/* argument list (argv from main) */
X
X#endif
X{
X    int             c;		/* Option letter */
X
X    /* Set default option values */
X    names_from_stdin = 0;
X    ar_file = getenv("TAPE");	/* From environment, or */
X    if (ar_file == 0) {
X	ar_file = DEF_AR_FILE;	/* From Makefile */
X    }
X
X    /*
X     * set up the flags to reflect the default pax inteface.  Unfortunately
X     * the pax interface has several options which are completely opposite
X     * of the tar and/or cpio interfaces...
X     */
X    f_unconditional = 1;
X    f_modification_time = 1;
X    f_create_dirs = 1;
X    blocking = 0;
X    ar_interface = TAR;
X    ar_format = TAR;
X    msgfile=stdout;
X
X    /* Parse options */
X    while ((c = taropt(argc, argv, "b:cf:hlmortuvwx")) != EOF) {
X	switch (c) {
X	case 'b':		/* specify blocking factor */
X	    /* 
X	     * FIXME - we should use a conversion routine that does
X	     * some kind of reasonable error checking, but...
X	     */
X	    blocking = atoi(optarg);
X	    break;
X	case 'c':		/* create a new archive */
X	    f_create = 1;
X	    break;
X	case 'f':		/* specify input/output file */
X	    ar_file = optarg;
X	    break;
X	case 'h':
X	    f_follow_links = 1;	/* follow symbolic links */
X	    break;
X	case 'l':		/* report unresolved links */
X	    f_linksleft = 1;
X	    break;
X	case 'm':		/* don't restore modification times */
X	    f_modified = 1;
X	    break;
X	case 'o':		/* take on user's group rather than
X				 * archives */
X	    break;
X	case 'r':		/* named files are appended to archive */
X	    f_append = 1;
X	    break;
X	case 't':
X	    f_list = 1;		/* list files in archive */
X	    break;
X	case 'u':		/* named files are added to archive */
X	    f_newer = 1;
X	    break;
X	case 'v':		/* verbose mode */
X	    f_verbose = 1;
X	    break;
X	case 'w':		/* user interactive mode */
X	    f_disposition = 1;
X	    break;
X	case 'x':		/* named files are extracted from archive */
X	    f_extract = 1;
X	    break;
X	case '?':
X	    usage();
X	    exit(EX_ARGSBAD);
X	}
X    }
X
X    /* check command line argument sanity */
X    if (f_create + f_extract + f_list + f_append + f_newer != 1) {
X	(void) fprintf(stderr,
X	   "%s: you must specify exactly one of the c, t, r, u or x options\n",
X		       myname);
X	usage();
X	exit(EX_ARGSBAD);
X    }
X
X    /* set the blocking factor, if not set by the user */
X    if (blocking == 0) {
X#ifdef USG
X	if (f_extract || f_list) {
X	    blocking = DEF_BLOCKING;
X	    fprintf(stderr, "Tar: blocksize = %d\n", blocking);
X	} else {
X	    blocking = 1;
X	}
X#else /* !USG */
X	blocking = 20;
X#endif /* USG */
X    }
X    blocksize = blocking * BLOCKSIZE;
X    buf_allocate((OFFSET) blocksize);
X
X    if (f_create) {
X	open_archive(AR_WRITE);
X	create_archive();	/* create the archive */
X    } else if (f_extract) {
X	open_archive(AR_READ);
X	read_archive();		/* extract files from archive */
X    } else if (f_list) {
X	open_archive(AR_READ);
X	read_archive();		/* read and list contents of archive */
X    } else if (f_append) {
X	open_archive(AR_APPEND);
X	append_archive();	/* append files to archive */
X    }
X    
X    if (f_linksleft) {		
X	linkleft(); 		/* report any unresolved links */ 
X    }
X    
X    return (0);
X}
X
X
X/* taropt -  tar specific getopt
X *
X * DESCRIPTION
X *
X * 	Plug-compatible replacement for getopt() for parsing tar-like
X * 	arguments.  If the first argument begins with "-", it uses getopt;
X * 	otherwise, it uses the old rules used by tar, dump, and ps.
X *
X * PARAMETERS
X *
X *	int argc	- argument count (argc from main) 
X *	char **argv	- argument list (argv from main) 
X *	char *optstring	- sring which describes allowable options
X *
X * RETURNS
X *
X *	Returns the next option character in the option string(s).  If the
X *	option requires an argument and an argument was given, the argument
X *	is pointed to by "optarg".  If no option character was found,
X *	returns an EOF.
X *
X */
X
X#ifdef __STDC__
X
Xstatic int taropt(int argc, char **argv, char *optstring)
X
X#else
X
Xstatic int taropt(argc, argv, optstring)
Xint             argc;
Xchar          **argv;
Xchar           *optstring;
X
X#endif
X{
X    extern char    *optarg;	/* Points to next arg */
X    extern int      optind;	/* Global argv index */
X    static char    *key;	/* Points to next keyletter */
X    static char     use_getopt;	/* !=0 if argv[1][0] was '-' */
X    char            c;
X    char           *place;
X
X    optarg = NULL;
X
X    if (key == NULL) {		/* First time */
X	if (argc < 2)
X	    return EOF;
X	key = argv[1];
X	if (*key == '-')
X	    use_getopt++;
X	else
X	    optind = 2;
X    }
X    if (use_getopt) {
X	return getopt(argc, argv, optstring);
X    }
X
X    c = *key++;
X    if (c == '\0') {
X	key--;
X	return EOF;
X    }
X    place = strchr(optstring, c);
X
X    if (place == NULL || c == ':') {
X	fprintf(stderr, "%s: unknown option %c\n", argv[0], c);
X	return ('?');
X    }
X    place++;
X    if (*place == ':') {
X	if (optind < argc) {
X	    optarg = argv[optind];
X	    optind++;
X	} else {
X	    fprintf(stderr, "%s: %c argument missing\n",
X		    argv[0], c);
X	    return ('?');
X	}
X    }
X    return (c);
X}
X
X
X/* usage - print a helpful message and exit
X *
X * DESCRIPTION
X *
X *	Usage prints out the usage message for the TAR interface and then
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
XUsage: %s -c[bfvw] device block filename..\n", myname);
X    fprintf(stderr, "\
X       %s -r[bvw] device block [filename...]\n", myname);
X    fprintf(stderr, "\
X       %s -t[vf] device\n", myname);
X    fprintf(stderr, "\
X       %s -u[bvw] device block [filename...]\n", myname);
X    fprintf(stderr, "\
X       %s -x[flmovw] device [filename...]\n", myname);
X    exit(1);
X}
END_OF_tar.c
if test 7881 -ne `wc -c <tar.c`; then
    echo shar: \"tar.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
echo shar: End of archive 3 \(of 6\).
cp /dev/null ark3isdone
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

