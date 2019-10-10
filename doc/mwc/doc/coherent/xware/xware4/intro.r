.NH "Xware 4:  Utilities"
.PP
Congratulations on purchasing Xware 4.
This package contains a selection of X utilities.
We believe that you will find most of them to be fun to work with, and
all of them useful.
.PP
This package also holds the library
.BR libXpm.a ,
whose functions manipulate pix-mapped images;
and a manual page for every program.
.SH "What is Xware?"
Xware is a series of packages from Mark Williams Company
that bundle some of the most popular and useful software
for the X Window System.
Each program in a Xware package comes with an executable binary;
source code;
instructions and scripts to help you recompile and install the program;
and a manual page for the program edited and formatted in the \*(CO
Lexicon format, which you can view with the \*(CO command
.BR man .
.PP
Other Xware packages are available or are being prepared.
These include packages of X window managers, games, graphics,
and development tools.
Xware makes it easy and convenient for you
to enlarge your supply of X software.
.SH "System Resources"
.PP
All of the programs in this package run in eight megabytes of RAM.
.PP
The amount of disk space depends upon how many programs you install,
and whether you leave the sources archives on your system or delete them.
As a rule of thumb,
you should assume that each de-archived program and its sources
consume between one-half and one megabyte of disk space.
Directions on how to install a program are given below.
.PP
The size of the screen required varies from program to program.
A few will run on a 640\(mu480 screen; most fit into 800\(mu600;
a few require a 1024\(mu768 screen.
Those that use the 1024\(mu768 screen can, in most instances,
be run under 800\(mu600, although the image being displayed may be cropped.
If you are using a virtual window manager (e.g.,
.B fvwm
or
.BR olwm ,
both of which are included with Xware volume 1),
you can cope by sliding the screen window around; patience is recommended.
.PP
Most programs recompile correctly with the \*(CO C compiler.
A few must be recompiled with the GCC compiler, which is
not included with this package.
Each archive contains a file named
.BR README.Coh ,
which describes how to install and recompile it; and each contains a
script called
.B Install.Coh
which installs the executable and its ancillary files (if any)
onto your system.
.PP
By default,
some of the utilities in this package use fonts that are included with the
Xware 1 window-managers package.
If you do not have this package, you can either revise the resource files
for these applications so that they use fonts that are included with X
Windows for \*(CO, or you can obtain the fonts from the Mark Williams
Bulletin Board.
.SH "Contents of This Package"
.PP
The individual programs are kept in the form of compressed archives.
When you install this package onto your system, all of the archives are
copied into directory
.BR /usr/X11/utilities ,
and the manual pages are installed into directory
.BR /usr/man/Xware4 .
.PP
The following describes the contents of this package:
.IP \fBgremlin\fR
A tool for drawing pictures and diagrams.
.IP \fBnotes\fR
Write, edit, and review notes to yourself.
Notes can be ``pasted'' to the screen as reminders.
.IP \fBsunclock\fR
Display the solar time around the globe.
This utility features excellent graphics.
.IP \fBtransfig\fR
Transform graphics files created with
.B xfig
into a variety of formats, including PostScript and TeX.
.IP \fBxabacus\fR
Display a standard Chinese abacus,
on which you can count and perform
arithmetic.
This utility also comes with a series of interactive tutorials to teach
you how to use the oldest computer of them all.
.IP \fBxcalendar\fR
Display a monthly calendar.
You can write reminders and notes for each day of the month.
.IP \fBxcuckoo\fR
This program lays its ``egg'' in the title bar of a window that is owned
by another program.
It's not exactly a utility, but it does demonstrate how to program
interprocess communication under X.
.IP \fBxdu\fR
This program reads the output of the \*(CO command
.BR du ,
and translates it into a histogram that shows graphically how much disk
space directories are consuming.
.IP \fBxfig\fR
This paint program lets you draw and paint pictures interactively.
With its companion program
.BR transfig ,
you can write
.BR xfig 's
the output into a variety of formats, including PostScript and TeX.
.IP \fBxfilemgr\fR
Manipulate files interactively, using a point-and-click environment.
The graphics are impressive.
.IP \fBxman\fR
Display manual pages interactively, by pointing and clicking.
.IP \fBxmore\fR
This resembles the \*(CO command
.BR more ,
which displays text files;
except that it uses a graphical X interface and has more features.
.IP \fBxpostit\fR
Create an electronic version of the popular
``Post-it'' notes.
With this program, you can create notes,
``stick'' them onto your root window, save them, and redisplay them.
The notes come in standard sizes and ``designer colors''.
.IP \fBxpt\fR
This program displays the periodic table of the elements.
Clicking on one cell of the table displays detailed
information about that element.
.IP \fBxspread\fR
A spreadsheet program that resembles some popular commercial packages.
Included are tools to perform matrix arithmetic and graphing.
.SH "Installing This Package"
.PP
To install this package onto your system, log in as the superuser
.B root
and type the installation command:
either
.DM
	/etc/install Xware4 /dev/fva0 4
.DE
if you have a 3.5-inch floppy-disk drive, or
.DM
	/etc/install Xware4 /dev/fha0 5
.DE
if your floppy-disk drive is 5.25 inches.
.B install
copies the programs into directory
.BR /usr/X11/utilities ,
installs the few extra tools you need to recompile these programs,
and automatically installs the manual pages for the programs.
.SH "Installing an Individual Program"
.PP
As described above, each program is kept in a compressed archive in directory
.BR /usr/X11/utilities .
Each archive contains a compiled version of the program, ready for running;
the sources for the program;
a file called
.BR README.Coh ,
which describes how to recompile the program, should you wish;
and a script named
.BR Install.Coh ,
which installs the program for you.
To install a program so you can use it, do the following:
.IP \(bu 0.3i
.B cd
to directory
.BR /usr/X11/utilities .
.IP \(bu
Use the command
.B gtar
to de-archive and uncompress the archive that holds the program you want.
For example, if you want to install program
.BR xfig ,
type the command:
.DM
	gtar -xvzf xfig.gtz
.DE
.IP \(bu
Change to the directory that holds the newly de-archived program.
For example, when you un-tar archive
.BR xfig.gtz ,
the program and its source files are copied into directory
.BR xfig .
.IP \(bu
Use the command
.B su
to become the superuser
.BR root ,
and execute script
.BR Install.Coh .
Each program has such a script; it handles all the details of
installation for you.
.IP \(bu
After you have installed the program, you may wish to remove the source
code and other files that you do not need.
To do so,
.B cd
back to the program's parent directory, and use the command
.B "rm \-rf"
to remove the directory and its contents.
For example, after you have installed
.BR xfig ,
you can remove this directory by typing the following commands:
.DM
	cd ..
	rm -rf xfig
.DE
Note that the archive from which you copied the program,
.BR xfig.gtz ,
is untouched by removing the directory
.BR xfig .
You can always de-archive the program again should you wish to work with
its program files or re-install it.
.PP
If you wish to recompile a program, follow the directions given in file
.B README.Coh
in that program's source directory.
.SH Recompiling
.PP
The file
.B README.Coh
in each package describes how to recompile it.
Before you can recompile some programs, you must first install the
X update kit.
This kit contains updated versions of some X header files, of the utility
.BR make ,
and of the \*(CO C compiler, as well as modified versions of other files.
This kit is available for free from the MWC BBS, or from the
.B ftp
site
.BR raven.alaska.edu .
If you cannot contact either site, write to MWC Technical Support as described
below, and a copy will be sent to you.
.SH "Modifying Resource Files"
The resource files that are installed by default with these applications
work nicely on a color monitor.
Most also work with a monochrome display; however, in some instances you
may wish to modify them.
.PP
Consider, for example, the following entries from the resource file
.BR /usr/X11/lib/app-defaults/XPostit ,
which is used by the program
.BR xpostit .
It contains the following entries that set colors:
.DM
.ta 0.5i 2.5i
	XPostit*Background:	PeachPuff
	XPostit*Foreground:	NavyBlue
	XPostit*Text*Background:	Yellow
	XPostit*Text*Foreground:	Black
	XPostit*Plaid*Background:	Red
	XPostit*Plaid*Foreground:	Black
.DE
.PP
A monochrome server maps both red and black (the colors used by the
.B Plaid
resource) to black; therefore, instead of seeing a cheerful plaid, you will
just see a black rectangle.
To fix this problem, edit the lines to read as follows:
.DM
.ta 0.5i 2.5i
	XPostit*Background:	PeachPuff
	XPostit*Foreground:	NavyBlue
	XPostit*Text*Background:	Yellow
	XPostit*Text*Foreground:	Black
	XPostit*Plaid*Background:	White
	XPostit*Plaid*Foreground:	Black
.DE
.PP
It's not always easy to predict which colors will be mapped to black and
which to white; you may have to depend upon trial and error.
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
We hope that you enjoy using these programs!
