.ds TL Errata
.NH "Notes and Errata"
.PP
The following notes limitations that have been discovered with \*(CO.
It also notes errors that have been found in the \*(CO manual, and
describes extensions to the \*(CO Lexicon that have been since the manual
was printed (but are available in the on-line Lexicon).
Try as we might, no system the size and complexity of \*(CO will ever
be entirely free of errors.
We hope that you will note these problems in your manual, and that you will
accept our apologies for any inconvenience they may have caused you.
.SH "Before and After You Update"
.PP
The following gives helpful hints that you should consider before
you update to \*(CO 4.2.
.IP \(bu 0.3i
Before you update, be sure your file system is intact.
Run \fBfsck\fR on your root file system, at least, before you attempt
to update.
.IP \(bu
When you install third-party packages, we recommend that you run in
single-user mode.
This will considerably lessen the complications of installation.
.PP
After you install \*(CO, the following describes how to update your
system to enable new devices:
.IP \(bu 0.3i
To install or de-install a parallel printer:
.DM
	cd /etc/conf
	lp/mkdev
	/conf/mlpconfig
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
.IP \(bu
To install STREAMS or floating-point emulation:
.DM
	cd /etc/conf
	em87/mkdev
		\fIor\fP
	streams/mkdev
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
.IP \(bu
To install a driver for asynchronous ports, first edit
.B /etc/default/async
as described in the Lexicon; then:
.DM
	cd /etc/conf
	asy/mkdev
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
.IP \(bu
To disable a hard-drive controller:
.DM
	cd /etc/conf
	bin/idenable -d \fIdisk_driver\fP
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
where
.I disk_driver
is one of the following:
.DS
	\fBat\fR	AT disk driver
	\fBaha\fR	Old-style Adaptec driver
	\fBss\fR	Seagate/Future Domain driver
	\fBhai\fR	Host-independent Adaptec driver
.DE
.IP \(bu
To enable a hard drive:
.DM
	cd /etc/conf
	bin/idenable -e \fIdisk_driver\fP
	# if you are installing the hai driver:
	# hai/mkdev
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
where
.I disk_driver
is one of the following:
.DS
	\fBat\fR	AT disk driver
	\fBaha\fR	Old-style Adaptec driver
	\fBss\fR	Seagate/Future Domain driver
	\fBhai\fR	Host-independent Adaptec driver
.DE
.IP \(bu
To change from a loadable versus a non-loadable keyboard driver,
to change the video adapter, or to add or remove virtual consoles:
.DM
	cd /etc/conf
	console/mkdev
	bin/idmkcoh -o /\fIkernel_name\fR
.DE
.SH "Known Limitations"
.PP
\*(CO 4.2 contains the following limitations:
.IP \(bu 0.3i
The program that converts old \*(UU files into the Taylor format makes
a number of assumptions about the form and content of the old files.
If those assumptions are not met, the program may write something
non-functional into one of the new \*(UU files.
Please note the following in particular:
.RS
.IP \(bu 0.3i
On occasion, times are converted without a day of week into the
.B sys
file.
.IP \(bu
Some commands that the old
.B Permissions
file allowed do not always make it into
.BR sys .
.IP
Be sure to read the generated files
.BR sys ,
.BR dial ,
and
.B port
to make sure that all is correct.
For details, check the Lexicon entries for these files.
.RE
.IP \(bu
The command
.B uumvlog
manages the log files that are kept in directory /usr/spool/uucp/.Log.
However, it does not touch the files kept in directory
/usr/spool/uucp/.Admin.
These files log all UUCP connections to your system, and can grow quite large.
You must manage the files by hand.
.SH "Notes"
.PP
The following should be noted prior to using \*(CO.
.IP \(bu 0.3i
This release of \*(CO does not include the floppy tape driver.
It is under development, but was not ready in time to be included in this
release.
It will be available shortly and will be posted on the
Mark Williams Company Bulletin Board System.
If you require the driver on floppy disk
please contact our Sales Department at 1-800-636-6700 or 708-291-6700.
We apologize for this delay and thank you for your patience and understanding.
.IP \(bu
The archiving utility
.B pax
has been removed from \*(CO.
This command (which was linked to the commands
.BR cpio ,
.BR tar ,
and
.BR ustar )
has been replaced by the commands
.B gnucpio
and
.BR gtar .
The former is linked to the command
.B tar
and the latter to
.BR cpio .
Both are robust utilities that are supplied by the Free Software Foundation
under its ``copyleft''.
For information on these utilities, see their manual pages in these notes,
or through the command
.BR man .
.SH "Documentation Revisions"
.PP
The following notes errors in revision 10 of the \*(CO manual.
The on-line Lexicon contains corrected entries, which you can view with the
command
.BR man .
.\".IP \(bu 0.3i
.\"The second paragraph of section `LICENSE' in the \*(CO Software License
.\"Agreement printed on the back cover of revision 10 of the COHERENT manual
.\"contains the following sentence:
.\".QS
.\"You shall, however, include in each
.\"Composite Program, and on the exterior label of every diskette, a copyright
.\"notice in this form: ``Portions of this program, copyright 1982, 1993, Mark
.\"Williams Company.''.
.\".QE
.\".IP
.\"The phrase ``and on the exterior label of every diskette''
.\"was included in the Software License
.\"Agreement in error, and should be regarded as void.
.IP \(bu 0.3i
The Lexicon entry for
.B signame[\^]
states that the signal numbers are offset by one within the array.
This is incorrect:
the signal numbers are not offset at all.
.IP \(bu
The Lexicon entry for
.B fopen()
states that the maximum number of
files structures that can be opened at any one time is 20.
Actually, maximum number of files is set by the manifest constant
.B FOPEN_MAX ,
which is defined in header file
.BR stdio.h .
For release 4.2, this is set to 60.
.IP \(bu
The default entry separator
for the command
.B help
is now the at-sign `@' instead of the pound sign `#'.
This permits us to include information on library routines and system
calls in the help file; using the `#' as a separator would cause
.B help
to abort when it read the string
.BR #include .
.IP \(bu
The command
.B man
recognizes index entries in either the new, 4.2 style, or in the old 4.0 style.
The former uses three fields, which are separated by tab characters;
the latter has two fields which are separated by one or more space characters.
.IP \(bu
The command
.B uuinstall
does not always work correctly:
on some terminals, the arrow keys do not move the cursor.
In this case, use
.BR vi -style
cursor-movement keys:
.DS
	\fBH\fR	Move the cursor left
	\fBK\fR	Move the cursor up
	\fBL\fR	Move the cursor right 
	\fBJ\fR	Move the cursor down
.DE
.IP \(bu
The Lexicon entry for the function
.B setgrent()
is wrong:
it actually returns nothing.
.PP
The following notes changes and revisions to the \*(CO documentation
that have occurred since revision 10 was printed.
To read them, use the
.B man
command to view the Lexicon entries in question:
.IP \(bu
As noted above, \*(CO release 4.2 includes System V-style tools for modifying
and rebuilding the kernel.
For information on how to use these tools,
see the articles
.BR cohtune ,
.BR idenable ,
.BR idmkcoh ,
and
.BR idtune .
.IP \(bu
The \*(CO console driver has been revised and extended.
It now supports something like the \*(PX's command set for consoles.
.IP \(bu
\*(CO now includes the tools
.B gnucpio
and
.BR gtar .
The on-line Lexicon entries for them give details on how to use them.
.IP \(bu
The Lexicon entry for
.B ioctl()
has been rewritten to list all commands that this system call recognizes.
.IP \(bu
.B make
has been drastically extended and revised.
The on-line Lexicon entry for
.B make
summarizes these changes.
.IP \(bu
The Korn shell
.B ksh
now supports
.BR vi -style
command-line editing.
The on-line Lexicon entry for
.B ksh
describes how to use this feature.
.IP \(bu
\*(CO now includes release 1.7 of the editor
.BR elvis .
The on-line Lexicon entry for
.B elvis
summarizes the changes from release 1.6.
.IP \(bu
The on-line Lexicon entry for
.B terminfo
describes several hundred instructions that were added since the manual
was printed.
.IP \(bu
The on-line entries for
.B tape
summarize \*(CO's tape facilities and describe the
.B tape
command, which lets you manipulate tape devices.
.IP \(bu
Where applicable, the on-line Lexicon's entries
for library routines, header files,
and system calls now include cross-references to the \*(PX.
.IP \(bu
The command
.B shutdown
now uses the script written by Udo Munk, which offers many more features
to users.
The on-line Lexicon entry for
.B shutdown
describes them.
.IP \(bu
The on-line Lexicon entry for the visual shell
.B vsh
now describes number of recently added features.
.IP \(bu
The Bourne shell
.B sh
now supports functions.
Its entry in the on-line Lexicon gives details and examples.
.IP \(bu
The Lexicon also contains many new articles in addition to the ones
mentioned above.
Some describe new library functions and system calls; others give
technical information or describe new commands.
New articles are being added constantly; for a canonical list, use
.B man
to read the latest version of the article
.BR Lexicon .
