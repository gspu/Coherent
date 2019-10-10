
From mwc!fwb Mon Dec 12 14:44:06 1994
Return-Path: <mwc!fwb>
Received: from mwc by lepanto.mwc.com with uucp
	(Smail3.1.28.1 #3) id m0rHHbJ-0001JPC; Mon, 12 Dec 94 14:44 CST
Received: by mwc.com (smail2.5.3-coh) id AA18754; 12 Dec 94 14:39:09 CST (Mon)
Message-Id: <9412121439.AA18753.V1.5@mwc.com>
Date: 12 Dec 94 14:39:09
From: fwb@mwc.com (Fred Butzen)
To: fred
Status: RO

.ds TI "Xware 5 Installation"
.ds TL ""
.NH "Xware 5:  Development Tools"
.PP
Congratulations on purchasing the Xware 5.
This package contains X development tools
.BR tcl ,
.BR tk ,
and
.BR xf .
These classic tools make it easy even for a non-programmer to give
an application a complex, Motif-like interface.
.SH "What is Xware?"
Xware is a series of packages from Mark Williams Company
that bundle some of the most popular and useful software
for the X Window System.
Each program in a Xware package comes with an executable binary;
instructions and scripts to install the program;
and a manual page for the program edited and formatted in the \*(CO
Lexicon format, which you can view with the \*(CO command
.BR man .
Most also come with full source code plus formatted \fBmakefiles\fR,
should you wish to modify and recompile the programs on your own.
.PP
Other Xware packages are available.
These include packages of X window managers, games, graphics,
and utilities.
Xware makes it easy and convenient for you to enlarge your supply of X software.
.SH "System Resources"
.PP
The programs in this package require 16 megabytes of RAM to run.
.PP
This package requires approximately 20 megabytes of space
in the file system that holds directory
.BR /usr .
.PP
The programs will run comfortably on a 800\(mu600 screen.
.PP
Note that if you do not have release 1.1 of X Windows for \*(CO,
you should install the update to X Windows for \*(CO
that is available from Mark Williams Company.
If you have not yet obtained a copy, you can download it from the MWC BBS
or via the Internet from site \fBraven.alaska.edu\fR.
If neither of these sites is available to you, contact Mark Williams Company
and a copy will be sent to you.
.SH "Contents of This Package"
.PP
This package consists of three major packages:
.IP \fBtcl\fR 0.3i
This package, whose name is an acronym for ``tool command language,''
is a library of C routines that implement a scripting language.
You can link the routines into your C programs,
thus instantly giving your programs a powerful and flexible scripting language,
or you can invoke
.BR tcl 's
routines in the form of commands that are interpreted through a shell.
Although
.B tcl
normally is used with X, you can also use it with ordinary, character-based
C programs.
.IP \fBtk\fR
This toolkit package implements a set of
Motif-like widgets that you can call through the
.B tcl
scripting language.
The widgets consist of bitmaps and code that together implement commonly
used X objects, such as windows, buttons, sliders, menus, and
font-management tools.
The widgets resemble those used by the Motif window manager.
.B tk
also implements some extensions to the
.B tcl
scripting language; in particular, it implements a method by which
X processes can communicate with each other.
.IP
.B tk
widgets are implemented in the form of C routines that you can link into
C programs.
It is commoner, however, to invoke them through the windowing shell
.BR wish .
With
.BR wish ,
a brief script written in the
.B tcl/tk
scripting language can create a sophisticated, Motif-like X application.
.IP \fBxf\fR
This application builds X applications interactively.
It is written entirely in the
.B tcl/tk
scripting language \(em it contains no C code for you to compile.
.IP
When you invoke
.BR xf ,
it displays a control panel from which you can select, modify, and assemble
.B tk
widgets.
When you are finished,
.B xf
writes a
.B tcl/tk
script to implement the application you have built.
With
.BR xf ,
even a non-programmer can build a sophisticated X application that uses
X widgets and has an attractive, Motif-like appearance.
.PP
Xware 5 also includes
.BR gmake ,
the GNU implementation of the utility
.BR make ,
which you need to install
.BR xf ;
and formatted manual pages for all of the
.B tcl
and
.B tk
routines, which you can read with the \*(CO command
.BR man .
.SH "Installing Xware 5"
.PP
Because it is a large, complex package,
.B "Xware 5"
requires some attention to detail if you wish to install it correctly.
.PP
Before you install, do the following:
.IP \(bu 0.3i
If you need to do so, install the X update kit.
Directions on how to do so are included with the kit.
.IP \(bu
Make sure that there is at least 20 megabytes
of disk space available on the partition that holds
.BR /usr .
If you do not have 20 megabytes available on that partition,
remove or compress files until you do.
.PP
The first part of installation involves moving the files onto your system:
.IP \(bu 0.3i
Log in as the superuser
.BR root .
.IP \(bu
Mount disk 1 of the Xware 5 kit.
.IP \(bu
Copy file
.B gmake.gz
from the release disk into directory
.BR /usr/local/bin .
Type the command
.DM
	gunzip gmake.gz
.DE
.IP
to uncompress it.
.IP \(bu
Copy archive
.B xware5_man.gtz
into directory
.BR /usr/man .
Type the command
.DM
	gtar -xvzf xware5_man.gtz
.DE
.IP
to de-archive and uncompress the manual pages.
.IP \(bu
Use the command
.B mkdir
to create directory
.BR /usr/tools .
Copy archive
.B tcl.gtz
from the disk into that directory.
.IP \(bu
.B umount
disk 1 of the Xware release, and
.B mount
disk 2.
.IP \(bu
Copy archive
.B tk.gtz
from disk 2 into directory
.BR /usr/tools .
.IP \(bu
.B umount
disk 2 of the release.
.IP \(bu
Use the command
.B mkdir
to create directory
.BR /usr/X11/xf2.2 .
.IP \(bu
.B mount
disk 3.
.IP \(bu
Copy archive
.B xf.gtz
from disk 3 into directory
.BR /usr/X11/xf2.2 .
.IP \(bu
.B umount
disk 3 and put it aside.
.PP
The next stage of installation de-archives the applications, and builds them:
.IP \(bu
.B cd
to
.BR /usr/tools ,
and type the following commands to de-archive and decompress the
.B tcl
and
.B tk
archives:
.DM
	gtar -xvzf tcl.gtz
	gtar -xvzf tk.gtz
.DE
.IP
These archives are very large, so patience is suggested.
After you have de-archived the files, you may wish to type the command:
.DM
	rm *.gtz
.DE
.IP
This will remove the archives, which are no longer needed,
and will save about three megabytes.
However, do
.I not
do so unless you are sure the archives were copied correctly.
.IP \(bu 0.3i
.B cd
to directory
.B /usr/tools/tcl7.1
and type the command:
.DM
	gmake install -f Makefile.coh
.DE
.IP \(bu
.B cd
to directory
.B /usr/tools/tk3.4
and type the command:
.DM
	gmake install -f Makefile.coh
.DE
.IP \(bu 0.3i
The final step is to install
.BR xf .
You must have the X server running before you do can install
.BR xf ,
because the installation script for
.B xf
reads information from the server (e.g., where fonts are stored).
So, type
.B startx
to invoke X.
When your
.B xvt
or
.B xterm
window has opened,
.B cd
to directory
.B /usr/tools/xf2.2
and again type the command:
.DM
	gmake install -f Makefile.coh
.DE
.PP
With this command, the tools will have been installed on your system.
Each installation command takes quite some time to execute:
patience is recommended.
.PP
Please note that by default,
the shells
.BR tclsh ,
.BR wish ,
and
.B xf
are installed into directory
.BR /usr/bin .
If you wish to change these defaults, you must modify the file
.B Makefile.coh
for each of these tools.
.PP
Configuration files for
.B tcl
and
.B tk
are copied into subdirectories of
.BR /usr/lib .
However, the libraries
.B libtcl.a
and
.B libtk.a
are not copied anywhere, but are left in the directories where you installed
them.
Likewise, the header files
.B tcl.h
and
.B tk.h
remain in their respective installation directories.
If you wish to use
.B tcl
and
.B tk
with C programs, you should copy these libraries and headers into
standard directories
(e.g.,
.B /usr/include
and
.BR /usr/lib );
or make sure that the
.BR Makefile s
for applications that use these libraries use the options
.B \-I
and
.B \-L
to read the directories where you have stored these files.
.SH "Copyrights"
.PP
Please note that the software in this package comes from third-party sources.
It is offered ``as is'', with no guarantee or warranty stated or implied,
whatsoever.
The copyrights on the source code and any executables built from it
belong to the persons who wrote the original code; full copyright
statements are included with the source code to each program.
We ask that you read and respect these statements.
.SH "Technical Support"
.PP
The sources in this package have been compiled and run successfully under
\*(CO.
If you have a problem bringing up one of the programs or with
compiling the source code as it is configured in this package, our
technical support team will attempt to help you.
However, because of the highly technical nature of the source
code in these packages, Mark Williams Company
.I cannot
give you technical assistance should you modify any of the source code
included in this package in any manner whatsoever.
.SH "A Final Word"
.PP
Most of the source code for these programs, and for the program shipped in
our other Xware packages,
is available for free on the Mark Williams bulletin board system.
The tutorial for
.B UUCP
in the \*(CO manual describes how to contact the BBS and make its services
available to you.
.PP
If you are seeking information on these tools, we suggest that you read
the article ``Tickled Pink'' by Kevin Reichard and Eric F. Johnson, which
appeared in the March 1994 issue of \fIUNIX Review\fR.
It gives a brief but well-crafted introduction to
.B tcl
and
.BR tk .
The manual pages for
.BR tclsh ,
.BR wish ,
and
.BR xf
give brief introductions to these programs.
You can view them with the \*(CO command
.BR man .
The manual pages for all
.B tcl
and
.B tk
routines \(em both functions and commands \(em are kept in directories
.B /usr/man/TCL
and
.BR /usr/man/TK .
You may wish to read and print them.
.PP
Manual pages for
.B xf
are kept in directories
.B /usr/man/XF
and
.BR /usr/X11/xf2.2/xfhelp/* .
You may wish to read or print these files.
Please note as well that because
.B xf
is a script that is interpreted by the shell
.BR wish ,
its sources are a good example of how to program in
.B tcl
and
.BR tk .
.PP
The best way to learn about these tools is to work with them.
Directory
.B tk3.4/libraries/demos
contains a number of demonstration scripts that are written in the
.B tcl/tk
language.
Each does something useful and entertaining.
By reading each and studying the manual pages for the routines that each
invokes, you should quickly learn how to use
.B tcl
and
.BR tk .
.PP
We hope that you enjoy using these programs!


fred@lepanto.mwc.com


