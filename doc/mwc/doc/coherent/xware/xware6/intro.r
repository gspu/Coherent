.NH "Xware 6: Ghostscript"
.PP
Congratulations on purchasing Xware 6.
This package contains Ghostscript, a language that closely resembles
PostScript;
.BR gs ,
a Ghostscript interpreter;
.BR ghostview ,
a Ghostscript viewer that lets you display and manipulate images
interactively;
a set of drivers for a variety of input and output devices;
and a variety of example PostScript files.
.SH "What Is Xware?"
Xware is a series of packages from Mark Williams Company
that bundle some of the most popular and useful software
for the X Window System.
Each program in a Xware package comes with an executable binary;
instructions and scripts to help you recompile and install the program;
and a manual page for the program edited and formatted in the \*(CO
Lexicon format, which you can view with the \*(CO command
.BR man .
Many also include source code.
.PP
Other Xware packages are available or are being prepared.
These include packages of X window managers, games, graphics programs,
utilities, and development tools.
Xware makes it easy and convenient for you
to enlarge your supply of X software.
.SH "System Resources"
.PP
Ghostscript and its tools require 16 megabytes of RAM.
They take up approximately nine megabytes of disk space.
.PP
The size of screen required depends upon the image being displayed:
most Ghostscript images fit onto a 800\(mu600 display, although some
very large ones require a 1024\(mu768 display.
.SH "Installing This Package"
.PP
To install this package onto your system, log in as the superuser
.B root
and type the installation command:
either
.DM
	/etc/install Xware6 /dev/fva0 2
.DE
if you have a 3.5-inch floppy-disk drive, or
.DM
	/etc/install Xware6 /dev/fha0 3
.DE
if your floppy-disk drive is 5.25 inches.
.B install
copies the programs into sub-directories of
.BR /u1/gnu :
.IP \fB/u1/gnu/bin\fR
This holds the executable programs
.BR gs ,
which is the Ghostscript interpreter, and
.BR ghostview ,
which is the viewer for displaying and manipulating images.
.IP \fB/u1/gnu/lib/ghostscript\fR
This holds initialization files for Ghostscript.
.IP \fB/u1/gnu/lib/ghostscript/doc\fR
This holds documentation files for Ghostscript and its drivers.
.IP \fB/u1/gnu/lib/ghostscript/fonts\fR
This holds the Ghostscript fonts.
.IP \fB/u1/gnu/lib/ghostscript/examples\fR
This holds example PostScript files.
File
.B gs_man.ps
holds the manual page for
.BR gs ,
formatted into PostScript under the \*(CO implementation of
.BR troff .
.PP
After archives have been copied onto your system into the default
directories, the installation script asks you if you wish to continue
with the installation.
You should answer ``no'' if you do not wish to install Ghostscript into
the default directories, or if you do not have enough disk space to hold
the de-archived files (i.e., about six megabytes).
Otherwise, you should answer ``yes''.
.PP
The installation program will ask you for permission to add the following
line to the end of file
.BR /etc/profile :
.DM
	export GS_LIB=/u1/gnu/lib/ghostscript
.DE
.PP
This environmental variable tells
.B ghostview
where the Ghostscript interpreter and configuration files are stored.
If you do not give the installation program permission to do this,
you must insert this variable by hand into
.BR /etc/profile .
.PP
After installation has completed,
you must make the following modifications to file
.BR /etc/profile :
.IP \(bu 0.3i
Add the directory
.B /u1/gnu/bin
to the definition of the environmental variable
.BR PATH .
.PP
Once you have done this, log in again.
Then you can demonstrate Ghostscript:
invoke X in the usual manner, then type the command
.DM
	ghostview /u1/gnu/lib/ghostscript/examples/tiger.ps
.DE
This displays a picture of a rather fierce tiger on your screen.
The interpreter will then wait for further commands; to exit from it, type
.BR quit .
.\".SH "Changing Defaults and Recompiling"
.\".PP
.\"By default, Ghostscript is configured to run under directory
.\".BR /u1/gnu .
.\"This directory was chosen because Ghostscript is protected by the
.\"Free Software Foundation's ``copyleft'' and
.\".B /u1
.\"is the directory into which all FSF products are installed by default under
.\"\*(CO 4.2.
.\"(These include GNU C and GNU Tools, which are also available through Mark
.\"Williams Company.)
.\"If you wish to install Ghostscript under another directory, you must
.\"recompile it.
.\".PP
.\"Note before you begin that you must recompile with the \*(CO C compiler
.\".BR cc .
.\"You
.\".I cannot
.\"recompile with GNU C, because of inconsistencies with floating-point
.\"arithmetic.
.\"Note, further, you
.\".I must
.\"use the version of the \*(CO C compiler that is included with the
.\"X repair kit; you
.\".I cannot
.\"use the stock compiler included with \*(CO release 4.2.05.
.\"The compiler in the X repair kit fixes a bug that prevented the compiler
.\"from building a proper executable for Ghostscript.
.\"If you do not have the X repair kit, you can download a copy either from
.\"the Mark Williams bulletin board, or (if you have \fBftp\fR access to the
.\"Internet) from site
.\".BR raven.alaska.edu .
.\"If you cannot download the kit electronically, contact Mark Williams Company's
.\"technical support group and a copy will be sent to you, at no charge.
.\".PP
.\"The following describes how to recompile Ghostscript:
.\".IP \(bu 0.3i
.\"First, select the directory under which you wish to install Ghostscript.
.\".IP \(bu
.\"Log in as the superuser
.\".BR root .
.\".IP \(bu
.\"De-archive the Ghostscript sources:
.\"first
.\".B cd
.\"to directory
.\".B /u1/gnu/lib/ghostscript/src
.\"and type the command:
.\".DM
.\"	gtar -xvzf gs.gtz
.\".DE
.\".IP \(bu
.\".B cd
.\"to directory
.\".BR gs256 .
.\".IP \(bu
.\"The next step is the trickiest:
.\"modify the
.\".B Makefile
.\"to reflect the change you wish to make.
.\"Change macro
.\".B prefix
.\"from
.\".B /u1/gnu
.\"to the directory you want.
.\"If you want all of Ghostscript to reside under the same directory tree,
.\"changing
.\".B prefix 
.\"is sufficient.
.\"If you wish to divide it up among different directories, then you must also
.\"modify the macros
.\".BR exec_prefix ,
.\".BR bindir ,
.\".BR datadir ,
.\"and
.\".BR gsdatadir .
.\".IP \(bu
.\"Modify macro
.\".B GS_LIB_DEFAULT
.\"to the directory (or directories) that hold the configuration files and
.\"fonts.
.\"Normally, this is set to the same value as
.\".BR gsdatadir ,
.\"but this is not required.
.\".IP \(bu
.\"Type
.\".DM
.\"	make install
.\".DE
.\".IP
.\"to recompile and install the Ghostscript binaries and configuration
.\"files.
.\".IP \(bu
.\"Next, copy the Ghostscript fonts from directory
.\".B /u1/gnu/lib/ghostscript/fonts
.\"into the directory (or a directory) that you named in the macro
.\".B GS_LIB_DEFAULT
.\"in your modified
.\".BR Makefile .
.\"(Note that we did not include these fonts in the sources because they
.\"are so large.)
.\".IP \(bu
.\".B cd
.\"to the directory into which you installed the fonts, and decompress them
.\"by typing the command:
.\".DM
.\"	gunzip *.gz
.\".DE
.\".PP
.\"Once you have tried Ghostscript and are convinced that it is
.\"installed properly,
.\"you can remove the original installation of Ghostscript.
.\"First, copy the archive
.\".B /u1/gnu/lib/ghostscript/src/gs.gtz
.\"into a place where you wish to store your source archives.
.\"Then remove directory
.\".B /u1/gnu/lib/ghostscript
.\"and its contents, and remove file
.\".BR /u1/gnu/bin/gs.gz .
.\".PP
.\"Should you change your mind, you can still use the original installation.
.\"To do so, log in as the superuser
.\".BR root ,
.\".B cd
.\"to directory
.\".BR /conf ,
.\"and execute the script
.\".BR Xware6.post .
.\".PP
.\"Finally, be sure to name in your
.\".B PATH
.\"the directory into which you put the executable file
.\".BR gs .
.SH "Default Configuration"
.PP
As shipped in Xware 6, Ghostscript is configured to work only with X.
The executable
includes drivers for some output devices.
Each driver is describes in its own
.B .doc
file, in directory
.BR /u1/gnu/lib/ghostscript/doc .
.SH "Copyrights and Credits"
.PP
Ghostscript was written primarily by L. Peter Deutsch, who is president
of Aladdin Enterprises (ghost@aladdin.com).
He was assisted by numerous other persons, who are named in the documentation
files included with Ghostscript.
Ghostscript, its sources, documentation, and ancillary files, are
copyright \(cu 1989, 1992, 1993 Aladdin Enterprises.
All rights reserved.
.PP
Ghostscript is protected by the Free Software Foundation's ``copyleft''.
For details of the rights this document grants you, and the obligations
it imposes, see the file
.B COPYING
which is included with the source files for Ghostscript.
.SH "Technical Support"
.PP
The sources in this package have been run successfully under
\*(CO.
If you have a problem bringing up one of the programs, our
technical support team will attempt to help you.
However, because of the highly technical nature of the source
code in these packages, Mark Williams Company
.I cannot
give you technical assistance should you modify any of the source code
included in this package in any manner whatsoever.
.SH "A Final Word"
.PP
All of the source code for these games, and for all programs shipped in our
Xware packages, is available for free on the Mark Williams bulletin board
system, and on other publicly accessible archives.
The tutorial for
.B UUCP
in the \*(CO manual describes how to contact the BBS and make its services
available to you.
