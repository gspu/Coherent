# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 4 (of 6)."
# Contents:  extract.c list.c pax.1
# Wrapped by mark@jhereg on Tue Dec 27 19:37:53 1988
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f extract.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"extract.c\"
else
echo shar: Extracting \"extract.c\" \(14327 characters\)
sed "s/^X//" >extract.c <<'END_OF_extract.c'
X/* $Source: /u/mark/src/pax/RCS/extract.c,v $
X *
X * $Revision: 1.1 $
X *
X * extract.c - Extract files from a tar archive. 
X *
X * DESCRIPTION
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
X * $Log:	extract.c,v $
X * Revision 1.1  88/12/23  18:02:07  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: extract.c,v 1.1 88/12/23 18:02:07 mark Rel $";
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
X * Swap bytes. 
X */
X#define	SWAB(n)	((((ushort)(n) >> 8) & 0xff) | (((ushort)(n) & 0xff00)) << 8)
X
X
X/* Function Prototypes */
X
X#ifdef __STDC__
X
Xstatic int inbinary(char *, char *, Stat *);
Xstatic int inascii(char *, char *, Stat *);
Xstatic int inswab(char *, char *, Stat *);
Xstatic int readtar(char *, Stat *);
Xstatic int readcpio(char *, Stat *);
X
X#else /* !__STDC__ */
X
Xstatic int inbinary();
Xstatic int inascii();
Xstatic int inswab();
Xstatic int readtar();
Xstatic int readcpio();
X
X#endif /* __STDC__ */
X
X
X/* read_archive - read in an archive
X *
X * DESCRIPTION
X *
X *	Read_archive is the central entry point for reading archives.
X *	Read_archive determines the proper archive functions to call
X *	based upon the archive type being processed.
X *
X * RETURNS
X *
X */
X
X#ifdef __STDC__
X
Xint read_archive(void)
X
X#else
X    
Xint read_archive()
X
X#endif
X{
X    Stat            sb;
X    char            name[PATH_MAX + 1];
X    int             match;
X    int		    pad;
X
X    name_gather();		/* get names from command line */
X    name[0] = '\0';
X    while (get_header(name, &sb) == 0) {
X	match = name_match(name) ^ f_reverse_match;
X	if (f_list) {		/* only wanted a table of contents */
X	    if (match) {
X		print_entry(name, &sb);
X	    }
X	    if (((ar_format == TAR) 
X		? buf_skip(ROUNDUP((OFFSET) sb.sb_size, BLOCKSIZE)) 
X		: buf_skip((OFFSET) sb.sb_size)) < 0) {
X		warn(name, "File data is corrupt");
X	    }
X	} else if (match) {
X	    if (rplhead != NULL) {
X		rpl_name(name);
X		if (strlen(name) == 0) {
X		    continue;
X		}
X	    }
X	    if (get_disposition("extract", name) || 
X                get_newname(name, sizeof(name))) {
X		/* skip file... */
X		if (((ar_format == TAR) 
X		    ? buf_skip(ROUNDUP((OFFSET) sb.sb_size, BLOCKSIZE)) 
X		    : buf_skip((OFFSET) sb.sb_size)) < 0) {
X		    warn(name, "File data is corrupt");
X		}
X		continue;
X	    } 
X	    if (inentry(name, &sb) < 0) {
X		warn(name, "File data is corrupt");
X	    }
X	    if (f_verbose) {
X		print_entry(name, &sb);
X	    }
X	    if (ar_format == TAR && sb.sb_nlink > 1) {
X		/*
X		 * This kludge makes sure that the link table is cleared
X		 * before attempting to process any other links.
X		 */
X		if (sb.sb_nlink > 1) {
X		    linkfrom(name, &sb);
X		}
X	    }
X	    if (ar_format == TAR && (pad = sb.sb_size % BLOCKSIZE) != 0) {
X		pad = BLOCKSIZE - pad;
X		buf_skip((OFFSET) pad);
X	    }
X	} else {
X	    if (((ar_format == TAR) 
X		? buf_skip(ROUNDUP((OFFSET) sb.sb_size, BLOCKSIZE)) 
X		: buf_skip((OFFSET) sb.sb_size)) < 0) {
X		warn(name, "File data is corrupt");
X	    }
X	}
X    }
X
X    close_archive();
X}
X
X
X
X/* get_header - figures which type of header needs to be read.
X *
X * DESCRIPTION
X *
X *	This is merely a single entry point for the two types of archive
X *	headers which are supported.  The correct header is selected
X *	depending on the archive type.
X *
X * PARAMETERS
X *
X *	char	*name	- name of the file (passed to header routine)
X *	Stat	*asb	- Stat block for the file (passed to header routine)
X *
X * RETURNS
X *
X *	Returns the value which was returned by the proper header
X *	function.
X */
X
X#ifdef __STDC__
X
Xint get_header(char *name, Stat *asb)
X
X#else
X    
Xint get_header(name, asb)
Xchar *name;
XStat *asb;
X
X#endif
X{
X    if (ar_format == TAR) {
X	return(readtar(name, asb));
X    } else {
X	return(readcpio(name, asb));
X    }
X}
X
X
X/* readtar - read a tar header
X *
X * DESCRIPTION
X *
X *	Tar_head read a tar format header from the archive.  The name
X *	and asb parameters are modified as appropriate for the file listed
X *	in the header.   Name is assumed to be a pointer to an array of
X *	at least PATH_MAX bytes.
X *
X * PARAMETERS
X *
X *	char	*name 	- name of the file for which the header is
X *			  for.  This is modified and passed back to
X *			  the caller.
X *	Stat	*asb	- Stat block for the file for which the header
X *			  is for.  The fields of the stat structure are
X *			  extracted from the archive header.  This is
X *			  also passed back to the caller.
X *
X * RETURNS
X *
X *	Returns 0 if a valid header was found, or -1 if EOF is
X *	encountered.
X */
X
X#ifdef __STDC__
X
Xstatic int readtar(char *name, Stat *asb)
X
X#else
X    
Xstatic int readtar(name, asb)
Xchar	*name;
XStat    *asb;
X
X#endif
X{
X    int             status = 3;	/* Initial status at start of archive */
X    static int      prev_status;
X
X    for (;;) {
X	prev_status = status;
X	status = read_header(name, asb);
X	switch (status) {
X	case 1:		/* Valid header */
X		return(0);
X	case 0:		/* Invalid header */
X	    switch (prev_status) {
X	    case 3:		/* Error on first record */
X		warn(ar_file, "This doesn't look like a tar archive");
X		/* FALLTHRU */
X	    case 2:		/* Error after record of zeroes */
X	    case 1:		/* Error after header rec */
X		warn(ar_file, "Skipping to next file...");
X		/* FALLTHRU */
X	    default:
X	    case 0:		/* Error after error */
X		break;
X	    }
X	    break;
X
X	case 2:			/* Record of zeroes */
X	case EOF:		/* End of archive */
X	default:
X	    return(-1);
X	}
X    }
X}
X
X
X/* readcpio - read a CPIO header 
X *
X * DESCRIPTION
X *
X *	Read in a cpio header.  Understands how to determine and read ASCII, 
X *	binary and byte-swapped binary headers.  Quietly translates 
X *	old-fashioned binary cpio headers (and arranges to skip the possible 
X *	alignment byte). Returns zero if successful, -1 upon archive trailer. 
X *
X * PARAMETERS
X *
X *	char	*name 	- name of the file for which the header is
X *			  for.  This is modified and passed back to
X *			  the caller.
X *	Stat	*asb	- Stat block for the file for which the header
X *			  is for.  The fields of the stat structure are
X *			  extracted from the archive header.  This is
X *			  also passed back to the caller.
X *
X * RETURNS
X *
X *	Returns 0 if a valid header was found, or -1 if EOF is
X *	encountered.
X */
X
X#ifdef __STDC__
X
Xstatic int readcpio(char *name, Stat *asb)
X
X#else
X    
Xstatic int readcpio(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    OFFSET          skipped;
X    char            magic[M_STRLEN];
X    static int      align;
X
X    if (align > 0) {
X	buf_skip((OFFSET) align);
X    }
X    align = 0;
X    for (;;) {
X	buf_read(magic, M_STRLEN);
X	skipped = 0;
X	while ((align = inascii(magic, name, asb)) < 0
X	       && (align = inbinary(magic, name, asb)) < 0
X	       && (align = inswab(magic, name, asb)) < 0) {
X	    if (++skipped == 1) {
X		if (total - sizeof(magic) == 0) {
X		    fatal("Unrecognizable archive");
X		}
X		warnarch("Bad magic number", (OFFSET) sizeof(magic));
X		if (name[0]) {
X		    warn(name, "May be corrupt");
X		}
X	    }
X	    memcpy(magic, magic + 1, sizeof(magic) - 1);
X	    buf_read(magic + sizeof(magic) - 1, 1);
X	}
X	if (skipped) {
X	    warnarch("Apparently resynchronized", (OFFSET) sizeof(magic));
X	    warn(name, "Continuing");
X	}
X	if (strcmp(name, TRAILER) == 0) {
X	    return (-1);
X	}
X	if (nameopt(name) >= 0) {
X	    break;
X	}
X	buf_skip((OFFSET) asb->sb_size + align);
X    }
X#ifdef	S_IFLNK
X    if ((asb->sb_mode & S_IFMT) == S_IFLNK) {
X	if (buf_read(asb->sb_link, (uint) asb->sb_size) < 0) {
X	    warn(name, "Corrupt symbolic link");
X	    return (readcpio(name, asb));
X	}
X	asb->sb_link[asb->sb_size] = '\0';
X	asb->sb_size = 0;
X    }
X#endif				/* S_IFLNK */
X
X    /* destroy absolute pathnames for security reasons */
X    if (name[0] == '/') {
X	if (name[1]) {
X	    while (name[0] = name[1]) {
X		++name;
X	    }
X	} else {
X	    name[0] = '.';
X	}
X    }
X    asb->sb_atime = asb->sb_ctime = asb->sb_mtime;
X    if (asb->sb_nlink > 1) {
X	linkto(name, asb);
X    }
X    return (0);
X}
X
X
X/* inswab - read a reversed by order binary header
X *
X * DESCRIPTIONS
X *
X *	Reads a byte-swapped CPIO binary archive header
X *
X * PARMAMETERS
X *
X *	char	*magic	- magic number to match
X *	char	*name	- name of the file which is stored in the header.
X *			  (modified and passed back to caller).
X *	Stat	*asb	- stat block for the file (modified and passed back
X *			  to the caller).
X *
X *
X * RETURNS
X *
X * 	Returns the number of trailing alignment bytes to skip; -1 if 
X *	unsuccessful. 
X *
X */
X
X#ifdef __STDC__
X
Xstatic int inswab(char *magic, char *name, Stat *asb)
X
X#else
X    
Xstatic int inswab(magic, name, asb)
Xchar           *magic;
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    ushort          namesize;
X    uint            namefull;
X    Binary          binary;
X
X    if (*((ushort *) magic) != SWAB(M_BINARY)) {
X	return (-1);
X    }
X    memcpy((char *) &binary,
X		  magic + sizeof(ushort),
X		  M_STRLEN - sizeof(ushort));
X    if (buf_read((char *) &binary + M_STRLEN - sizeof(ushort),
X		 sizeof(binary) - (M_STRLEN - sizeof(ushort))) < 0) {
X	warnarch("Corrupt swapped header",
X		 (OFFSET) sizeof(binary) - (M_STRLEN - sizeof(ushort)));
X	return (-1);
X    }
X    asb->sb_dev = (dev_t) SWAB(binary.b_dev);
X    asb->sb_ino = (ino_t) SWAB(binary.b_ino);
X    asb->sb_mode = SWAB(binary.b_mode);
X    asb->sb_uid = SWAB(binary.b_uid);
X    asb->sb_gid = SWAB(binary.b_gid);
X    asb->sb_nlink = SWAB(binary.b_nlink);
X    asb->sb_rdev = (dev_t) SWAB(binary.b_rdev);
X    asb->sb_mtime = SWAB(binary.b_mtime[0]) | SWAB(binary.b_mtime[1]); << 16
X    asb->sb_size = SWAB(binary.b_size[0]) | SWAB(binary.b_size[1]); << 16
X    if ((namesize = SWAB(binary.b_name)) == 0 || namesize >= PATH_MAX) {
X	warnarch("Bad swapped pathname length",
X		 (OFFSET) sizeof(binary) - (M_STRLEN - sizeof(ushort)));
X	return (-1);
X    }
X    if (buf_read(name, namefull = namesize + namesize % 2) < 0) {
X	warnarch("Corrupt swapped pathname", (OFFSET) namefull);
X	return (-1);
X    }
X    if (name[namesize - 1] != '\0') {
X	warnarch("Bad swapped pathname", (OFFSET) namefull);
X	return (-1);
X    }
X    return (asb->sb_size % 2);
X}
X
X
X/* inascii - read in an ASCII cpio header
X *
X * DESCRIPTION
X *
X *	Reads an ASCII format cpio header
X *
X * PARAMETERS
X *
X *	char	*magic	- magic number to match
X *	char	*name	- name of the file which is stored in the header.
X *			  (modified and passed back to caller).
X *	Stat	*asb	- stat block for the file (modified and passed back
X *			  to the caller).
X *
X * RETURNS
X *
X * 	Returns zero if successful; -1 otherwise. Assumes that  the entire 
X *	magic number has been read. 
X */
X
X#ifdef __STDC__
X
Xstatic int inascii(char *magic, char *name, Stat *asb)
X
X#else
X    
Xstatic int inascii(magic, name, asb)
Xchar           *magic;
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    uint            namelen;
X    char            header[H_STRLEN + 1];
X
X    if (strncmp(magic, M_ASCII, M_STRLEN) != 0) {
X	return (-1);
X    }
X    if (buf_read(header, H_STRLEN) < 0) {
X	warnarch("Corrupt ASCII header", (OFFSET) H_STRLEN);
X	return (-1);
X    }
X    header[H_STRLEN] = '\0';
X    if (sscanf(header, H_SCAN, &asb->sb_dev,
X	       &asb->sb_ino, &asb->sb_mode, &asb->sb_uid,
X	       &asb->sb_gid, &asb->sb_nlink, &asb->sb_rdev,
X	       &asb->sb_mtime, &namelen, &asb->sb_size) != H_COUNT) {
X	warnarch("Bad ASCII header", (OFFSET) H_STRLEN);
X	return (-1);
X    }
X    if (namelen == 0 || namelen >= PATH_MAX) {
X	warnarch("Bad ASCII pathname length", (OFFSET) H_STRLEN);
X	return (-1);
X    }
X    if (buf_read(name, namelen) < 0) {
X	warnarch("Corrupt ASCII pathname", (OFFSET) namelen);
X	return (-1);
X    }
X    if (name[namelen - 1] != '\0') {
X	warnarch("Bad ASCII pathname", (OFFSET) namelen);
X	return (-1);
X    }
X    return (0);
X}
X
X
X/* inbinary - read a binary header
X *
X * DESCRIPTION
X *
X *	Reads a CPIO format binary header.
X *
X * PARAMETERS
X *
X *	char	*magic	- magic number to match
X *	char	*name	- name of the file which is stored in the header.
X *			  (modified and passed back to caller).
X *	Stat	*asb	- stat block for the file (modified and passed back
X *			  to the caller).
X *
X * RETURNS
X *
X * 	Returns the number of trailing alignment bytes to skip; -1 if 
X *	unsuccessful. 
X */
X
X#ifdef __STDC__
X
Xstatic int inbinary(char *magic, char *name, Stat *asb)
X
X#else
X    
Xstatic int inbinary(magic, name, asb)
Xchar           *magic;
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    uint            namefull;
X    Binary          binary;
X
X    if (*((ushort *) magic) != M_BINARY) {
X	return (-1);
X    }
X    memcpy((char *) &binary,
X		  magic + sizeof(ushort),
X		  M_STRLEN - sizeof(ushort));
X    if (buf_read((char *) &binary + M_STRLEN - sizeof(ushort),
X		 sizeof(binary) - (M_STRLEN - sizeof(ushort))) < 0) {
X	warnarch("Corrupt binary header",
X		 (OFFSET) sizeof(binary) - (M_STRLEN - sizeof(ushort)));
X	return (-1);
X    }
X    asb->sb_dev = binary.b_dev;
X    asb->sb_ino = binary.b_ino;
X    asb->sb_mode = binary.b_mode;
X    asb->sb_uid = binary.b_uid;
X    asb->sb_gid = binary.b_gid;
X    asb->sb_nlink = binary.b_nlink;
X    asb->sb_rdev = binary.b_rdev;
X    asb->sb_mtime = binary.b_mtime[0] | binary.b_mtime[1]; << 16
X    asb->sb_size = binary.b_size[0] | binary.b_size[1]; << 16
X    if (binary.b_name == 0 || binary.b_name >= PATH_MAX) {
X	warnarch("Bad binary pathname length",
X		 (OFFSET) sizeof(binary) - (M_STRLEN - sizeof(ushort)));
X	return (-1);
X    }
X    if (buf_read(name, namefull = binary.b_name + binary.b_name % 2) < 0) {
X	warnarch("Corrupt binary pathname", (OFFSET) namefull);
X	return (-1);
X    }
X    if (name[binary.b_name - 1] != '\0') {
X	warnarch("Bad binary pathname", (OFFSET) namefull);
X	return (-1);
X    }
X    return (asb->sb_size % 2);
X}
END_OF_extract.c
if test 14327 -ne `wc -c <extract.c`; then
    echo shar: \"extract.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f list.c -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"list.c\"
else
echo shar: Extracting \"list.c\" \(16229 characters\)
sed "s/^X//" >list.c <<'END_OF_list.c'
X/* $Source: /u/mark/src/pax/RCS/list.c,v $
X *
X * $Revision: 1.1 $
X *
X * list.c - List all files on an archive
X *
X * DESCRIPTION
X *
X *	These function are needed to support archive table of contents and
X *	verbose mode during extraction and creation of achives.
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
X * $Log:	list.c,v $
X * Revision 1.1  88/12/23  18:02:14  mark
X * Initial revision
X * 
X */
X
X#ifndef lint
Xstatic char *ident = "$Id: list.c,v 1.1 88/12/23 18:02:14 mark Rel $";
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
X * isodigit returns non zero iff argument is an octal digit, zero otherwise
X */
X#define	ISODIGIT(c)	(((c) >= '0') && ((c) <= '7'))
X
X
X/* Function Prototypes */
X
X#ifdef __STDC__
X
Xstatic void cpio_entry(char *, Stat *);
Xstatic void tar_entry(char *, Stat *);
Xstatic void pax_entry(char *, Stat *);
Xstatic void print_mode(ushort);
Xstatic long from_oct(int digs, char *where);
X
X#else /* !__STDC__ */
X
Xstatic void cpio_entry();
Xstatic void tar_entry();
Xstatic void pax_entry();
Xstatic void print_mode();
Xstatic long from_oct();
X
X#endif /* __STDC__ */
X
X
X/* Internal Identifiers */
X
Xstatic char       *monnames[] = {
X    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
X    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
X};
X
X
X/* read_header - read a header record
X *
X * DESCRIPTION
X *
X * 	Read a record that's supposed to be a header record. Return its 
X *	address in "head", and if it is good, the file's size in 
X *	asb->sb_size.  Decode things from a file header record into a "Stat". 
X *	Also set "head_standard" to !=0 or ==0 depending whether header record 
X *	is "Unix Standard" tar format or regular old tar format. 
X *
X * PARAMETERS
X *
X *	char   *name		- pointer which will contain name of file
X *	Stat   *asb		- pointer which will contain stat info
X *
X * RETURNS
X *
X * 	Return 1 for success, 0 if the checksum is bad, EOF on eof, 2 for a 
X * 	record full of zeros (EOF marker). 
X */
X
X#ifdef __STDC__
X
Xint read_header(char *name, Stat *asb)
X
X#else
X    
Xint read_header(name, asb)
Xchar           *name;
XStat           *asb;
X
X#endif
X{
X    int             i;
X    long            sum;
X    long	    recsum;
X    Link           *link;
X    char           *p;
X    char            hdrbuf[BLOCKSIZE];
X
X    memset((char *)asb, 0, sizeof(Stat));
X    /* read the header from the buffer */
X    if (buf_read(hdrbuf, BLOCKSIZE) != 0) {
X	return (EOF);
X    }
X
X    strcpy(name, hdrbuf);
X
X    recsum = from_oct(8, &hdrbuf[148]);
X    sum = 0;
X    p = hdrbuf;
X    for (i = 0 ; i < 500; i++) {
X
X	/*
X	 * We can't use unsigned char here because of old compilers, e.g. V7. 
X	 */
X	sum += 0xFF & *p++;
X    }
X
X    /* Adjust checksum to count the "chksum" field as blanks. */
X    for (i = 0; i < 8; i++) {
X	sum -= 0xFF & hdrbuf[148 + i];
X    }
X    sum += ' ' * 8;
X
X    if (sum == 8 * ' ') {
X
X	/*
X	 * This is a zeroed record...whole record is 0's except for the 8
X	 * blanks we faked for the checksum field. 
X	 */
X	return (2);
X    }
X    if (sum == recsum) {
X	/*
X	 * Good record.  Decode file size and return. 
X	 */
X	if (hdrbuf[156] != LNKTYPE) {
X	    asb->sb_size = from_oct(1 + 12, &hdrbuf[124]);
X	}
X	asb->sb_mtime = from_oct(1 + 12, &hdrbuf[136]);
X	asb->sb_mode = from_oct(8, &hdrbuf[100]);
X
X	if (strcmp(&hdrbuf[257], TMAGIC) == 0) {
X	    /* Unix Standard tar archive */
X	    head_standard = 1;
X#ifdef NONAMES
X	    asb->sb_uid = from_oct(8, &hdrbuf[108]);
X	    asb->sb_gid = from_oct(8, &hdrbuf[116]);
X#else
X	    asb->sb_uid = finduid(&hdrbuf[265]);
X	    asb->sb_gid = findgid(&hdrbuf[297]);
X#endif
X	    switch (hdrbuf[156]) {
X	    case BLKTYPE:
X	    case CHRTYPE:
X		asb->sb_rdev = makedev(from_oct(8, &hdrbuf[329]),
X				      from_oct(8, &hdrbuf[337]));
X		break;
X	    default:
X		/* do nothing... */
X		break;
X	    }
X	} else {
X	    /* Old fashioned tar archive */
X	    head_standard = 0;
X	    asb->sb_uid = from_oct(8, &hdrbuf[108]);
X	    asb->sb_gid = from_oct(8, &hdrbuf[116]);
X	}
X
X	switch (hdrbuf[156]) {
X	case REGTYPE:
X	case AREGTYPE:
X	    /*
X	     * Berkeley tar stores directories as regular files with a
X	     * trailing /
X	     */
X	    if (name[strlen(name) - 1] == '/') {
X		name[strlen(name) - 1] = '\0';
X		asb->sb_mode |= S_IFDIR;
X	    } else {
X		asb->sb_mode |= S_IFREG;
X	    }
X	    break;
X	case LNKTYPE:
X	    asb->sb_nlink = 2;
X	    linkto(&hdrbuf[157], asb);
X	    linkto(name, asb);
X	    asb->sb_mode |= S_IFREG;
X	    break;
X	case BLKTYPE:
X	    asb->sb_mode |= S_IFBLK;
X	    break;
X	case CHRTYPE:
X	    asb->sb_mode |= S_IFCHR;
X	    break;
X	case DIRTYPE:
X	    asb->sb_mode |= S_IFDIR;
X	    break;
X#ifdef S_IFLNK
X	case SYMTYPE:
X	    asb->sb_mode |= S_IFLNK;
X	    strcpy(asb->sb_link, &hdrbuf[157]);
X	    break;
X#endif
X#ifdef S_IFIFO
X	case FIFOTYPE:
X	    asb->sb_mode |= S_IFIFO;
X	    break;
X#endif
X#ifdef S_IFCTG
X	case CONTTYPE:
X	    asb->sb_mode |= S_IFCTG;
X	    break;
X#endif
X	}
X	return (1);
X    }
X    return (0);
X}
X
X
X/* print_entry - print a single table-of-contents entry
X *
X * DESCRIPTION
X * 
X *	Print_entry prints a single line of file information.  The format
X *	of the line is the same as that used by the LS command.  For some
X *	archive formats, various fields may not make any sense, such as
X *	the link count on tar archives.  No error checking is done for bad
X *	or invalid data.
X *
X * PARAMETERS
X *
X *	char   *name		- pointer to name to print an entry for
X *	Stat   *asb		- pointer to the stat structure for the file
X */
X
X#ifdef __STDC__
X
Xvoid print_entry(char *name, Stat *asb)
X
X#else
X    
Xvoid print_entry(name, asb)
Xchar		*name;
XStat	        *asb;
X
X#endif
X{
X    switch (ar_interface) {
X    case TAR:
X	tar_entry(name, asb);
X	break;
X    case CPIO:
X	cpio_entry(name, asb);
X	break;
X    case PAX: pax_entry(name, asb);
X	break;
X    }
X}
X
X
X/* cpio_entry - print a verbose cpio-style entry
X *
X * DESCRIPTION
X *
X *	Print_entry prints a single line of file information.  The format
X *	of the line is the same as that used by the traditional cpio 
X *	command.  No error checking is done for bad or invalid data.
X *
X * PARAMETERS
X *
X *	char   *name		- pointer to name to print an entry for
X *	Stat   *asb		- pointer to the stat structure for the file
X */
X
X#ifdef __STDC__
X
Xstatic void cpio_entry(char *name, Stat *asb)
X
X#else
X    
Xstatic void cpio_entry(name, asb)
Xchar	       *name;
XStat	       *asb;
X
X#endif
X{
X    struct tm	       *atm;
X    Link	       *from;
X    struct passwd      *pwp;
X    struct group       *grp;
X
X    if (f_list && f_verbose) {
X	fprintf(msgfile, "%-7o", asb->sb_mode);
X	atm = localtime(&asb->sb_mtime);
X	if (pwp = getpwuid((int) USH(asb->sb_uid))) {
X	    fprintf(msgfile, "%-6s", pwp->pw_name);
X	} else {
X	    fprintf(msgfile, "%-6u", USH(asb->sb_uid));
X	}
X	fprintf(msgfile,"%7ld  %3s %2d %02d:%02d:%02d %4d  ",
X	               asb->sb_size, monnames[atm->tm_mon], 
X		       atm->tm_mday, atm->tm_hour, atm->tm_min, 
X		       atm->tm_sec, atm->tm_year + 1900);
X    }
X    fprintf(msgfile, "%s", name);
X    if ((asb->sb_nlink > 1) && (from = islink(name, asb))) {
X	fprintf(msgfile, " linked to %s", from->l_name);
X    }
X#ifdef	S_IFLNK
X    if ((asb->sb_mode & S_IFMT) == S_IFLNK) {
X	fprintf(msgfile, " symbolic link to %s", asb->sb_link);
X    }
X#endif	/* S_IFLNK */
X    putc('\n', msgfile);
X}
X
X
X/* tar_entry - print a tar verbose mode entry
X *
X * DESCRIPTION
X *
X *	Print_entry prints a single line of tar file information.  The format
X *	of the line is the same as that produced by the traditional tar 
X *	command.  No error checking is done for bad or invalid data.
X *
X * PARAMETERS
X *
X *	char   *name		- pointer to name to print an entry for
X *	Stat   *asb		- pointer to the stat structure for the file
X */
X
X#ifdef __STDC__
X
Xstatic void tar_entry(char *name, Stat *asb)
X
X#else
X    
Xstatic void tar_entry(name, asb)
Xchar		*name;
XStat            *asb;
X
X#endif
X{
X    struct tm  	       *atm;
X    int			i;
X    int			mode;
X    char               *symnam = "NULL";
X    Link               *link;
X
X    if ((mode = asb->sb_mode & S_IFMT) == S_IFDIR) {
X	return;			/* don't print directories */
X    }
X    if (f_extract) {
X	switch (mode) {
X#ifdef S_IFLNK
X	case S_IFLNK: 	/* This file is a symbolic link */
X	    i = readlink(name, symnam, PATH_MAX - 1);
X	    if (i < 0) {		/* Could not find symbolic link */
X		warn("can't read symbolic link", syserr());
X	    } else { 		/* Found symbolic link filename */
X		symnam[i] = '\0';
X		fprintf(msgfile, "x %s symbolic link to %s\n", name, symnam);
X	    }
X	    break;
X#endif
X	case S_IFREG: 	/* It is a link or a file */
X	    if ((asb->sb_nlink > 1) && (link = islink(name, asb))) {
X		fprintf(msgfile, "%s linked to %s\n", name, link->l_name); 
X	    } else {
X		fprintf(msgfile, "x %s, %d bytes, %d tape blocks\n", 
X			name, asb->sb_size, ROUNDUP(asb->sb_size, 
X			BLOCKSIZE) / BLOCKSIZE);
X	    }
X	}
X    } else if (f_append || f_create) {
X	switch (mode) {
X#ifdef S_IFLNK
X	case S_IFLNK: 	/* This file is a symbolic link */
X	    i = readlink(name, symnam, PATH_MAX - 1);
X	    if (i < 0) {		/* Could not find symbolic link */
X		warn("can't read symbolic link", syserr());
X	    } else { 		/* Found symbolic link filename */
X		symnam[i] = '\0';
X		fprintf(msgfile, "a %s symbolic link to %s\n", name, symnam);
X	    }
X	    break;
X#endif
X	case S_IFREG: 	/* It is a link or a file */
X	    fprintf(msgfile, "a %s ", name);
X	    if ((asb->sb_nlink > 1) && (link = islink(name, asb))) {
X		fprintf(msgfile, "link to %s\n", link->l_name); 
X	    } else {
X		fprintf(msgfile, "%d Blocks\n", 
X			ROUNDUP(asb->sb_size, BLOCKSIZE) / BLOCKSIZE);
X	    }
X	    break;
X	}
X    } else if (f_list) {
X	if (f_verbose) {
X	    atm = localtime(&asb->sb_mtime);
X	    print_mode(asb->sb_mode);
X	    fprintf(msgfile," %d/%d %6d %3s %2d %02d:%02d %4d %s",
X		    asb->sb_uid, asb->sb_gid, asb->sb_size,
X		    monnames[atm->tm_mon], atm->tm_mday, atm->tm_hour, 
X		    atm->tm_min, atm->tm_year + 1900, name);
X	} else {
X	    fprintf(msgfile, "%s", name);
X	}
X	switch (mode) {
X#ifdef S_IFLNK
X	case S_IFLNK: 	/* This file is a symbolic link */
X	    i = readlink(name, symnam, PATH_MAX - 1);
X	    if (i < 0) {		/* Could not find symbolic link */
X		warn("can't read symbolic link", syserr());
X	    } else { 		/* Found symbolic link filename */
X		symnam[i] = '\0';
X		fprintf(msgfile, " symbolic link to %s", name, symnam);
X	    }
X	    break;
X#endif
X	case S_IFREG: 	/* It is a link or a file */
X	    if ((asb->sb_nlink > 1) && (link = islink(name, asb))) {
X		fprintf(msgfile, " linked to %s", link->l_name);
X	    }
X	    break;		/* Do not print out directories */
X	}
X	fputc('\n', msgfile);
X    } else {
X	fprintf(msgfile, "? %s %d blocks\n", name,
X		ROUNDUP(asb->sb_size, BLOCKSIZE) / BLOCKSIZE);
X    }
X}
X
X
X/* pax_entry - print a verbose cpio-style entry
X *
X * DESCRIPTION
X *
X *	Print_entry prints a single line of file information.  The format
X *	of the line is the same as that used by the LS command.  
X *	No error checking is done for bad or invalid data.
X *
X * PARAMETERS
X *
X *	char   *name		- pointer to name to print an entry for
X *	Stat   *asb		- pointer to the stat structure for the file
X */
X
X#ifdef __STDC__
X
Xstatic void pax_entry(char *name, Stat *asb)
X
X#else
X    
Xstatic void pax_entry(name, asb)
Xchar	       *name;
XStat	       *asb;
X
X#endif
X{
X    struct tm	       *atm;
X    Link	       *from;
X    struct passwd      *pwp;
X    struct group       *grp;
X
X    if (f_list && f_verbose) {
X	print_mode(asb->sb_mode);
X	fprintf(msgfile, " %2d", asb->sb_nlink);
X	atm = localtime(&asb->sb_mtime);
X	if (pwp = getpwuid((int) USH(asb->sb_uid))) {
X	    fprintf(msgfile, " %-8s", pwp->pw_name);
X	} else {
X	    fprintf(msgfile, " %-8u", USH(asb->sb_uid));
X	}
X	if (grp = getgrgid((int) USH(asb->sb_gid))) {
X	    fprintf(msgfile, " %-8s", grp->gr_name);
X	} else {
X	    fprintf(msgfile, " %-8u", USH(asb->sb_gid));
X	}
X	switch (asb->sb_mode & S_IFMT) {
X	case S_IFBLK:
X	case S_IFCHR:
X	    fprintf(msgfile, "\t%3d, %3d",
X		           major(asb->sb_rdev), minor(asb->sb_rdev));
X	    break;
X	case S_IFREG:
X	    fprintf(msgfile, "\t%8ld", asb->sb_size);
X	    break;
X	default:
X	    fprintf(msgfile, "\t        ");
X	}
X	fprintf(msgfile," %3s %2d %02d:%02d ",
X	               monnames[atm->tm_mon], atm->tm_mday, 
X		       atm->tm_hour, atm->tm_min);
X    }
X    fprintf(msgfile, "%s", name);
X    if ((asb->sb_nlink > 1) && (from = islink(name, asb))) {
X	fprintf(msgfile, " == %s", from->l_name);
X    }
X#ifdef	S_IFLNK
X    if ((asb->sb_mode & S_IFMT) == S_IFLNK) {
X	fprintf(msgfile, " -> %s", asb->sb_link);
X    }
X#endif	/* S_IFLNK */
X    putc('\n', msgfile);
X}
X
X
X
X
X/* print_mode - fancy file mode display
X *
X * DESCRIPTION
X *
X *	Print_mode displays a numeric file mode in the standard unix
X *	representation, ala ls (-rwxrwxrwx).  No error checking is done
X *	for bad mode combinations.  FIFOS, sybmbolic links, sticky bits,
X *	block- and character-special devices are supported if supported
X *	by the hosting implementation.
X *
X * PARAMETERS
X *
X *	ushort	mode	- The integer representation of the mode to print.
X */
X
X#ifdef __STDC__
X
Xstatic void print_mode(ushort mode)
X
X#else
X    
Xstatic void print_mode(mode)
Xushort	mode;
X
X#endif
X{
X    /* Tar does not print the leading identifier... */
X    if (ar_interface != TAR) {
X	switch (mode & S_IFMT) {
X	case S_IFDIR: 
X	    putc('d', msgfile); 
X	    break;
X#ifdef	S_IFLNK
X	case S_IFLNK: 
X	    putc('l', msgfile); 
X	    break;
X#endif	/* S_IFLNK */
X	case S_IFBLK: 
X	    putc('b', msgfile); 
X	    break;
X	case S_IFCHR: 
X	    putc('c', msgfile); 
X	    break;
X#ifdef	S_IFIFO
X	case S_IFIFO: 
X	    putc('p', msgfile); 
X	    break; 
X#endif	/* S_IFIFO */ 
X	case S_IFREG: 
X	default:
X	    putc('-', msgfile); 
X	    break;
X	}
X    }
X    putc(mode & 0400 ? 'r' : '-', msgfile);
X    putc(mode & 0200 ? 'w' : '-', msgfile);
X    putc(mode & 0100
X	 ? mode & 04000 ? 's' : 'x'
X	 : mode & 04000 ? 'S' : '-', msgfile);
X    putc(mode & 0040 ? 'r' : '-', msgfile);
X    putc(mode & 0020 ? 'w' : '-', msgfile);
X    putc(mode & 0010
X	 ? mode & 02000 ? 's' : 'x'
X	 : mode & 02000 ? 'S' : '-', msgfile);
X    putc(mode & 0004 ? 'r' : '-', msgfile);
X    putc(mode & 0002 ? 'w' : '-', msgfile);
X    putc(mode & 0001
X	 ? mode & 01000 ? 't' : 'x'
X	 : mode & 01000 ? 'T' : '-', msgfile);
X}
X
X
X/* from_oct - quick and dirty octal conversion
X *
X * DESCRIPTION
X *
X *	From_oct will convert an ASCII representation of an octal number
X *	to the numeric representation.  The number of characters to convert
X *	is given by the parameter "digs".  If there are less numbers than
X *	specified by "digs", then the routine returns -1.
X *
X * PARAMETERS
X *
X *	int digs	- Number to of digits to convert 
X *	char *where	- Character representation of octal number
X *
X * RETURNS
X *
X *	The value of the octal number represented by the first digs
X *	characters of the string where.  Result is -1 if the field 
X *	is invalid (all blank, or nonoctal). 
X *
X * ERRORS
X *
X *	If the field is all blank, then the value returned is -1.
X *
X */
X
X#ifdef __STDC__
X
Xstatic long from_oct(int digs, char *where)
X
X#else
X
Xstatic long from_oct(digs, where)
Xint             digs;		/* number of characters to convert */
Xchar           *where;		/* character representation of octal number */
X
X#endif
X{
X    long            value;
X
X    while (isspace(*where)) {	/* Skip spaces */
X	where++;
X	if (--digs <= 0) {
X	    return(-1);		/* All blank field */
X	}
X    }
X    value = 0;
X    while (digs > 0 && ISODIGIT(*where)) {	/* Scan til nonoctal */
X	value = (value | (*where++ - '0'); << 3)
X	--digs;
X    }
X
X    if (digs > 0 && *where && !isspace(*where)) {
X	return(-1);		/* Ended on non-space/nul */
X    }
X    return(value);
X}
END_OF_list.c
if test 16229 -ne `wc -c <list.c`; then
    echo shar: \"list.c\" unpacked with wrong size!
fi
# end of overwriting check
fi
if test -f pax.1 -a "${1}" != "-c" ; then 
  echo shar: Will not over-write existing file \"pax.1\"
else
echo shar: Extracting \"pax.1\" \(13279 characters\)
sed "s/^X//" >pax.1 <<'END_OF_pax.1'
X.\" $Id: pax.1,v 1.1 88/12/23 18:02:22 mark Rel $
X.TH PAX 1 "USENIX Association" ""
X.SH NAME
Xpax \- portable archive exchange 
X.SH SYNOPSIS
X.TP \w'\fBpax\fR\ 'u
X.B pax
X.RB [ \-cimopuvy ]
X.RI "[\fB\-f\fR" " archive" ]
X.RI "[\fB\-s\fR" " replstr" ]
X.RI "[\fB\-t\fR" " device" ] 
X.RI [ pattern... ]
X.TP \w'\fBpax\ \-r\fR\ 'u
X.B "pax\ \-r"
X.RB [ \-cimnopuvy ]
X.RI "[\fB\-f\fR" " archive" ]
X.RI "[\fB\-s\fR" " replstr" ] 
X.RI "[\fB\-t\fR" " device" ] 
X.RI [ pattern... ]
X.TP \w'\fBpax\ \-w\fR\ 'u
X.B "pax\ \-w"
X.RB [ \-adimuvy ] 
X.RI "[\fB\-b\fR" " blocking" ]
X.RI "[\fB\-f\fR" " archive" ] 
X.RI "[\fB\-s\fR" " replstr" ] 
X.RI "[\fB\-t\fR" " device" ] 
X.RI "[\fB\-x\fR" " format" ]
X.RI [ pathname... ]
X.TP \w'\fBpax\ \-rw\fR\ 'u
X.B "pax\ \-rw"
X.RB [ \-ilmopuvy ] 
X.RI "[\fB\-s\fR" " replstr" ] 
X.RI [ pathname... ]
Xdirectory
X.SH DESCRIPTION
X.I Pax 
Xreads and writes archive files which conform to the 
X.B "Archive/Interchange File Format" 
Xspecified in 
X.IR "IEEE Std. 1003.1-1988" .
X.I Pax
Xcan also read, but not write, a number of other file formats
Xin addition to those specified in the 
X.B "Archive/Interchange File Format"
Xdescription.
XSupport for these traditional file formats, such as V7 
X.I "tar" 
Xand System V binary 
X.I "cpio" 
Xformat archives,
Xis provided for backward compatibility and to maximize portability.
X.PP
X.I Pax 
Xwill also support traditional 
X.I cpio 
Xand 
XSystem V
X.I tar
Xinterfaces if invoked with the name
X"cpio" or "tar" respectively.
XSee the 
X.I cpio(1) 
Xor 
X.I tar(1)
Xmanual pages for more details.
X.PP
XCombinations of the
X.B \-r 
Xand 
X.B \-w
Xcommand line arguments specify whether 
X.I pax 
Xwill read, write or list the contents of the specified archive,
Xor move the specified files to another directory.
X.PP
XThe command line arguments are:
X.TP .5i
X.B \-w
Xwrites the files and directories specified by 
X.I pathname
Xoperands to the standard output together with the pathname and status 
Xinformation prescribed by the archive format used.
XA directory
X.I pathname
Xoperand refers to the files and (recursively) subdirectories of that 
Xdirectory.
XIf no 
X.I pathname 
Xoperands are given, then the standard input is read to get a
Xlist of pathnames to copy, one pathname per line.
XIn this case, only those pathnames appearing on the standard input are
Xcopied.
X.TP .5i
X.B \-r
X.I Pax
Xreads an archive file from the standard input.
XOnly files with names that match any of the 
X.I pattern
Xoperands are selected for extraction.
XThe selected files are conditionally created and copied relative to the
Xcurrent directory tree, subject to the options described below.
XBy default, the owner and group of selected files will be that of the
Xinvoking process, and the permissions and modification times will be the
Xsames as those in the archive.
X.RS .5i
X.PP
XThe supported archive formats are automatically detected on input.
XThe default output format is 
X.IR ustar ,
Xbut may be overridden by the 
X.B \-x
X.I format
Xoption described below.
X.RE
X.TP .5i
X.B \-rw
X.I Pax 
Xreads the files and directories named in the
X.I pathname
Xoperands and copies them to the destination 
X.IR directory .
XA directory
X.I pathname 
Xoperand refers to the files and (recursively) subdirectories of that
Xdirectory.
XIf no 
X.I pathname
Xoperands are given, the standard input is read to get a list of pathnames
Xto copy, one pathname per line.
XIn this case, only those pathnames appearing on the standard input are
Xcopied.
XThe directory named by the 
X.I directory
Xoperand must exist and have the proper permissions before the copy can
Xoccur.
X.PP
XIf neither the
X.BR \-r " or " \-w 
Xoptions are given, then 
X.I pax
Xwill list the contents of the specified archive.
XIn this mode, 
X.I pax 
Xlists normal files one per line, hard link pathnames as
X.sp
X.RS 1i
X.IR pathname " == " linkname
X.RE
X.sp
Xand symbolic link pathnames (if supported by the implementation) as
X.sp
X.RS 1i
X.IR pathname " -> " linkname
X.RE
X.sp
Xwhere 
X.I pathname
Xis the name of the file being extracted, and
X.I linkname
Xis the name of a file which appeared earlier in the archive.
X.PP
XIf the 
X.B \-v
Xoption is specified, then 
X.I pax 
Xlist normal pathnames in the same format used by the 
X.I ls
Xutility with the
X.B \-l
Xoption.
XHard links are shown as
X.sp
X.RS 1i
X.IR "<ls -l listing>" " == " linkname
X.RE
X.sp
Xand symbolic links (if supported) are shown as
X.sp
X.RS 1i
X.IR "<ls -l listing>" " -> " linkname
X.RE
X.sp 
X.PP
X.I Pax 
Xis capable of reading and writing archives which span multiple physical 
Xvolumes.
XUpon detecting an end of medium on an archive which is not yet completed,
X.I pax 
Xwill prompt the user for the next volume of the archive and will allow the 
Xuser to specify the location of the next volume.
X.SS Options
XThe following options are available:
X.TP 1i
X.B \-a
XThe files specified by
X.I pathname
Xare appended to the specified archive.
X.TP 1i
X.BI \-b " blocking"
XBlock the output at
X.I blocking
Xbytes per write to the archive file.
XA 
X.B k
Xsuffix multiplies 
X.I blocking 
Xby 1024, a
X.B b 
Xsuffix multiplies 
X.I blocking 
Xby 512 and a 
X.B m
Xsuffix multiplies 
X.I blocking 
Xby 1048576 (1 megabyte).
XIf not specified,
X.I blocking 
Xis automatically determined on input and is ignored for 
X.B \-rw.
X.TP 1i
X.B \-c
XComplement the match sense of the the 
X.I pattern
Xoperands.
X.TP 1i
X.B \-d
XIntermediate directories not explicitly listed in the archive are not 
Xcreated.
XThis option is ignored unless
Xthe 
X.B \-r
Xoption is specified.
X.TP 1i
X.BI \-f " archive"
XThe
X.I archive
Xoption specifies the pathname of the input or output archive,
Xoverriding the default of standard input for 
X.B \-r
Xor standard output for
X.BR \-w .
X.TP 1i
X.B \-i
XInteractively rename files.
XSubstitutions specified by 
X.B \-s
Xoptions (described below)
Xare performed before requesting the new file name from the user.
XA file is skipped if an empty line is entered and
X.I pax
Xexits with an exit status of 0 if 
X.B EOF 
Xis encountered.
X.TP 1i
X.B \-l
XFiles are linked rather than copied when possible.
X.TP 1i
X.B \-m
XFile modification times are not retained.
X.TP 1i
X.B \-n
XWhen 
X.B \-r
Xis specified, but
X.B \-w
Xis not, the 
X.I pattern
Xarguments are treated as ordinary file names.
XOnly the first occurrence of each of these files in the input archive
Xis read.
XThe
X.B pax
Xutility exits with a zero exit status after all files in the list have been
Xread.
XIf one or more files in the list is not found,
X.B pax
Xwrites a diagnostic to standard error for each of the files and exits with
Xa non-zero exit status.
Xthe file names are compared before any of the 
X.BR \-i ,
X.BR \-s ,
Xor
X.B \-y
Xoptions are applied.
X.TP 1i
X.B \-o
XRestore file ownership as specified in the archive.
XThe invoking process must have appropriate privileges to accomplish this.
X.TP 1i
X.B \-p
XPreserve the access time of the input files after they have been copied.
X.TP 1i
X.BI \-s " replstr"
XFile names are modified according to the substitution expression using the 
Xsyntax of 
X.I "ed(1)"
Xas shown:
X.sp
X.RS 2i
X-s /\fIold\fR/\fInew\fR/\fB[\fRgp\fB]\fR
X.RE
X.RS 1i
X.PP
XAny non null character may be used as a delimiter (a / is used here as an
Xexample).
XMultiple 
X.B \-s
Xexpressions may be specified; the expressions are applied in the order
Xspecified terminating with the first successful substitution.
XThe optional trailing 
X.B p
Xcauses successful mappings to be listed on standard error.
XThe optional trailing
X.B g
Xcauses the 
X.I old 
Xexpression to be replaced each time it occurs in the source string.
XFiles that substitute to an empty string are ignored both on input and
Xoutput.
X.RE
X.TP 1i
X.BI \-t " device"
XThe
X.I device
Xoption argument is an implementation-defined identifier that names the input
Xor output archive device, overriding the default of standard input for
X.B \-r
Xand standard output for
X.BR \-w .
X.TP 1i
X.B \-u
XCopy each file only if it is newer than a pre-existing file with the same
Xname.
XThis implies 
X.BR \-a .
X.TP 1i
X.B \-v
XList file names as they are encountered.
XProduces a verbose table of contents listing on the standard output when both 
X.B \-r 
Xand
X.B \-w
Xare omitted,
Xotherwise the file names are printed to standard error as they are
Xencountered in the archive.
X.TP 1i
X.BI \-x " format"
XSpecifies the output archive
X.IR format .
XThe input format, which must be one of the following, is automatically
Xdetermined when the
X.B \-r 
Xoption is used.
XThe supported formats are:
X.TP 1i
X.I cpio
XThe extended 
X.I CPIO
Xinterchange format specified in
X.B "Extended CPIO Format" in 
X.I "IEEE Std. 1003.1-1988."
X.TP 1i
X.I ustar
XThe extended 
X.I TAR
Xinterchange format specified in
X.B "Extended TAR Format" in 
X.I "IEEE Std. 1003.1-1988."
XThis is the default archive format.
X.RE
X.TP 1i
X.B \-y
XInteractively prompt for the disposition of each file.
XSubstitutions specified by 
X.B \-s
Xoptions (described above)
Xare performed before prompting the user for disposition.
X.B EOF
Xor an input line starting with the character
X.B q
Xcaused
X.I pax
Xto exit.
XOtherwise, an input line starting with anything other than 
X.B y
Xcauses the file to be ignored.
XThis option cannot be used in conjunction with the 
X.B \-i 
Xoption.
X.PP
XOnly the last of multiple 
X.B \-f 
Xor
X.B \-t
Xoptions take effect.
X.PP
XWhen writing to an archive, the
Xstandard input is used as a list of pathnames if no
X.I pathname
Xoperands are specified.
XThe format is one pathname per line.
XOtherwise, the standard input is the archive file,
Xwhich is formatted according to one of the specifications in
X.B "Archive/Interchange File format"
Xin 
X.IR "IEEE Std. 1003.1-1988" ,
Xor some other implementation-defined format.
X.PP
XThe user ID and group ID of the process, together with the appropriate
Xprivileges, affect the ability of 
X.I pax
Xto restore ownership and permissions attributes of the archived files.
X(See 
X.I "format-reading utility"
Xin
X.B "Archive/Interchange File Format"
Xin 
X.IR "IEEE Std. 1003.1-1988" ".)"
X.PP
XThe options
X.BR \-a ,
X.BR \-c ,
X.BR \-d ,
X.BR \-i ,
X.BR \-l ,
X.BR \-p ,
X.BR \-t ,
X.BR \-u ,
Xand
X.BR \-y 
Xare provided for functional compatibility with the historical
X.I cpio
Xand
X.I tar
Xutilities.
XThe option defaults were chosen based on the most common usage of these
Xoptions, therefore, some of the options have meanings different than
Xthose of the historical commands.
X.SS Operands
XThe following operands are available:
X.TP 1i
X.I directory
XThe destination directory pathname for copies when both the
X.B \-r
Xand 
X.B \-w
Xoptions are specified.
XThe directory must exist and be writable before the copy or and error
Xresults.
X.TP 1i
X.I pathname
XA file whose contents are used instead of the files named on the standard
Xinput.
XWhen a directory is named, all of its files and (recursively) 
Xsubdirectories
Xare copied as well.
X.TP 1i
X.IR pattern 
XA
X.I pattern
Xis given in the standard shell pattern matching notation.
XThe default if no 
X.I pattern 
Xis  specified is
X.BR * \,
Xwhich selects all files.
X.SH EXAMPLES
XThe following command
X.sp
X.RS 1i
Xpax \-w \-f /dev/rmt0 \.
X.RE
X.sp
Xcopies the contents of the current directory to tape drive 0.
X.PP
XThe commands
X.sp
X.RS 1i
X.RI mkdir " newdir"
X.br
X.RI cd " olddir"
X.br
X.RI "pax -rw . " newdir
X.RE
X.sp
Xcopies the contents of 
X.I olddir 
Xto 
X.I newdir .
X.PP
XThe command
X.sp
X.RS 1i
Xpax \-r \-s ',//*usr//*,,' -f pax.out
X.RE
X.sp
Xreads the archive 
X.B pax.out
Xwith all files rooted in "/usr" in the archive extracted
Xrelative to the current directory.
X.SH FILES
X.TP 1i
X/dev/tty
Xused to prompt the user for information when the
X.BR \-i " or " \-y
Xoptions are specified. 
X.SH "SEE ALSO"
Xcpio(1), find(1), tar(1), cpio(5), tar(5)
X.SH DIAGNOSTICS
X.I Pax
Xwill terminate immediately, without processing any 
Xadditional files on the command line or in the archive.
X.SH "EXIT CODES"
X.I Pax 
Xwill exit with one of the following values:
X.IP 0 .5i
XAll files in the archive were processed successfully.
X.IP ">0" .5i
X.I Pax 
Xaborted due to errors encountered during operation.
X.SH BUGS
XSpecial permissions may be required to copy or extract special files.
X.PP
XDevice, user ID, and group ID numbers larger than 65535 cause additional
Xheader records to be output.
XThese records are ignored by some historical version of
X.I "cpio(1)"
Xand
X.IR "tar(1)" .
X.PP
XThe archive formats described in
X.B "Archive/Interchange File Format" 
Xhave certain restrictions that have
Xbeen carried over from historical usage.
XFor example, there are restrictions on the length of pathnames stored in
Xthe archive.
X.PP
XWhen getting an "ls -l" style listing on
X.I tar
Xformat archives, link counts are listed as zero since the 
X.I ustar
Xarchive format does not keep link count information.
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
END_OF_pax.1
if test 13279 -ne `wc -c <pax.1`; then
    echo shar: \"pax.1\" unpacked with wrong size!
fi
# end of overwriting check
fi
echo shar: End of archive 4 \(of 6\).
cp /dev/null ark4isdone
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

