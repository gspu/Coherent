.SH "Installing \*(PN"
.PP
.II installation
To install \*(PN on your system, following the directions in
the appropriate sub-section:
either for the VAX, or for the Intel development system.
.Sh "Installing \*(PN on the VAX"
.PP
To install \*(PN on the VAX, do the following:
.nr l1 0
\*i
Create a directory in which the compiler and its associated support libraries
can reside.
\*i
Copy the distribution tape into the directory via the VMS \fBBACKUP\fR
utility.
\*i
.II INSTAL86C.COM
Invoke the \fBINSTAL86C.COM\fR command file to build the compiler,
including your system run-time library.
\*i
Invoke the \fBSETUP86C.COM\fR command file to make the logical assignments
used by the compiler.  As shipped, each user must invoke the command file.
The compiler can also be made available for system-wide use.
.PP
The following lists the contents of
\fBINSTAL86.COM\fR:
.DM
.ta 4.0i
$
$ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ !	!
$ !   MWC86 Version 2.5.2	!
$ !   Copyright 1982 - 1986 by Mark Williams Company, Chicago.	!
$ !   All rights reserved may not be copied or disclosed	!
$ !   without permission.	!
$ !	!
$ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$ say ::= write sys$output
$ say "Linking the cross compiler"
$
$
.ta 2.0i
$ link	CCSCAN.obj,-
	cc86.olb/include=dope,-
	cc86.olb/include=macros,-
	cc86.olb/include=paternf,-
	cc86.olb/include=paternb,-
	cc86.olb/include=table0,-
	cc86.olb/include=table1,-
	cc86.olb/include=optab,-
	cc86.olb/include=regnam,-
	cc86.olb/lib
$
$ rename	CCSCAN.exe cc86.exe
$
.DE
The following gives the contents of
.BR SETUP86.COM :
.DM
$! File:  setup86c.com
$! Last edit:  31-JAN-1986
$!+
.ta 1.5i
$!	MWC86 VERSION 2.5.2
$!	COPYRIGHT (C) 1982 - 86
$!	BY MARK WILLIAMS COMPANY, CHICAGO.
$!	ALL RIGHTS RESERVED.
$!	MAY NOT BE COPIED OR DISCLOSED WITHOUT PERMISSION.
$!-
$!
$! This is the command file to set up the Mark Williams Co.
$! iAPX 86 C (VAX/VMS native mode) cross-compiler to run on VMS.
$! It is invoked by including the command
$!
$!	$ @mwc86c:setup86c
$!
$! in one's login.com file.  Here, "mwc" must be
$! the logical name
$! of the device containing this command file.
$! Note that this command file
$! assumes that the executable image of the compiler will reside on
$! DQA0: in directory [mwc86c].  This assignment should be
$! changed to reflect your installation realities.
$!
$ assign dqa0: mwc
$ assign mwc:[mwc86c.include] cc86$include
$ assign mwc:[mwc86c] mwc86c
$ cc86 :== $mwc86c:cc86
$ size :== $mwc86c:size
$!
$! Invoking the compiler:
$!
$! The command line is identical to that on the Intel Series III
$! development system.  You say:
$! 	     cc86 file [options]
$! 
$! The output file is in the same directory and has the
$! same name as the input file; the file type is changed
$! to "o86" (for object) or "a86" (for pseudo-assembly code). 
.DE
.Sh "Installing on the Intel development system"
.PP
To install \*(PN onto the Intel development system, simply
copy all of the files from the distribution disks onto your
hard disk.
Because its operating system does not use directories,
no special organization of the files is necessary.
