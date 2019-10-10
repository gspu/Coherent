.ds TL Installation
.NH "Installing GNU Sources"
.PP
This kit contains sources for the tools in the GNU Software Development System.
With these sources, you can recompile these GNU tools on your \*(CO system
and modify them to suit your preferences.
.Sh "Requirements"
.PP
The following is required before you can install and recompile these
sources successfully:
.IP \(bu 0.3i
Your system must be running \*(CO release 4.2.10 or later.
Persons who have \*(CO 4.2.05 are eligible for a free upgrade to 4.2.10.
If you have \*(CO 4.2.05, contact Mark Williams Technical Support to
arrange to have a upgrade sent to you.
.IP \(bu
You must have a binary of GNU C release 2.5.6 or later.
This is available from Mark Williams Company, or from various public-domain
sources.
.IP \(bu
You should have GNU versions of the following tools:
.BR awk ,
.BR sed ,
.BR make ,
and
.BR bash .
Most of these sources will not rebuild correctly with the \*(CO versions
of these tools.
.IP 
You should install GNU
.B bash
as
.BR /bin/sh .
You should then make
.B /bin/sh
your login shell, so that GNU
.B bash
is used throughout the compilation process, instead of a native \*(CO shell.
.IP \(bu
As explained below, the sources in this package are organized into modules.
The disks in each module are separately labeled and numbered; this lets you
install the sources for one tool without having to fill your disk with the
sources for them all.
.IP
Every module is installed into directory
.BR /u1/gnu/src .
The disk requirements for each module are given below.
To install all of the modules onto your system, the file system that holds
directory
.B /u1/gnu/src
must have 60 megabytes of free space.
To recompile all of the packages, the file system that holds directory
.B /u1/gnu/src
must have an additional 40 megabytes of free space.
Thus, to install and recompile all of the sources, the file system
that holds directory
.B /u1/gnu/src
must have a total of 100 megabytes of free space.
.Sh "Documentation"
.PP
Rebuilding a major GNU tool, such a compiler or debugger, is not a trivial
task.
Before you begin, you should read all available documentation on the tool
you wish to rebuild.
.PP
Each module contains the documentation for its tool, in binary form.
You can read this documentation using a text editor; or you can use
the \*(CO commands
.B pr
or
.BR prps
to print them.
.PP
Directory
.B /u1/gnu/man
contains manual pages for many of the GNU tools whose source code
is in this kit.
Note that this directory is simply for your convenience:
it is redundant, and can be removed should you wish to save the
disk space.
.PP
Printed manuals for these tools are
available from the Free Software Foundation.
For information, contact the Free Software
Foundation, 675 Mass Avenue, Cambridge, MA 02139.
The manuals are reasonably priced and well written; and by
purchasing the you can help support the Free Software Foundation in its
work of maintaining and extending these products.
.Sh "Copyright Information"
.PP
Please note that the GNU sources are covered by the Free Software
Foundation's ``copyleft.''
A copy is included in the file
.BR COPYING ,
which is included with each module of sources.
The Free Software Foundation permits you to give copies of the GNU sources
to other users; however, it demands that certain conditions be met before
you do so.
Please read the GNU documentation carefully before you give away
copies of the GNU sources.
.bp
.SH "Installing and Recompiling GNU Sources"
.PP
As mentioned above, this package is organized into modules.
Each module holds the sources, documentation, and ancillary files for
one of the following GNU tools:
GNU C;
the GNU assembler
.BR gas ;
the GNU
.B bin
utilities;
the GNU C-source debugger
.BR gdb ;
and
the GNU C++ run-time library.
.PP
We organized the sources in this manner so that you can install just the
sources that interest you, without having to install the entire package.
The disks for each module are separated labeled and numbered.
When you install a module, be careful to use the correct set of disks.
.PP
The following sub-sections describe how to install and recompile each module.
.Sh "Installing and Recompiling GNU C"
.PP
GNU C sources are installed into directory
.BR /u1/gnu/src/gcc-2.5.6 .
Installing the sources requires 23 megabytes of
free disk space on the file system that holds this directory.
Compiling all of the stages of GNU C requires an additional 27 megabytes
of disk space on this file system, for a total of 50 megabytes.
Please note that it is not necessary to build the stage-3 compiler,
as described in the file
.BR README :
the compilers from the GNU Development Tools binary distribution
already are tested stage-3 compilers.
.PP
To install GNU C, use the command:
.DM
	/etc/install -c COHgcc /dev/fva\fIN\fP 5
.DE
.I N
is the number of the 3.5-inch floppy-disk drive from which you are installing
the sources, either `0' or `1'.
.PP
To compile the C compiler, use the command:
.DM
	make CC=gcc CFLAGS=-O2
.DE
To compile the Objective-C run-time library, use the command:
.DM
	make objc-runtime CC=gcc CFLAGS=-O
.DE
Note that you must
.I not
compile Objective-C programs with the compiler option
.BR -O2 :
the optimizer for Objective C has a known bug, and the programs will core dump.
.PP
To install the binaries, libraries, and support files, use the command:
.DM
	make install
.DE
To configure the C compilers for Coherent 4.2, use the command:
.DM
	configure --target=i386-coh --with-stabs --prefix=/u1/gnu
.DE
The configure procedure builds modified header files in the directory
.BR /usr/include .
Note that with the exception of the header files installed into directory
.BR /usr/include/objc ,
these header files are
.I not
required because the \*(CO header files are compatible with GNU C.
We strongly recommend that you remove these header files before
you build the compiler, so that the \*(CO header files are used.
.Sh "Installing and Recompiling the GNU Assembler"
.PP
The sources for the GNU assembler
.B gas
are installed into directory
.BR /u1/gnu/src/gas-2.2 .
Installing the sources requires eight megabytes of free disk space
on the file system that holds this directory.
Compiling
.B gas
requires an additional three megabytes of disk space on this file system,
for a total of 11 megabytes of disk space.
.PP
To install
.BR gas ,
use the command:
.DM
	/etc/install -c COHgas /dev/fva\fIN\fP 2
.DE
.I N
is the number of the 3.5-inch floppy-disk drive from which you are installing
the sources, either `0' or `1'.
.PP
To compile
.BR gas ,
use the command:
.DM
	make CC=gcc CFLAGS=-O2
.DE
To install
.BR gas ,
use the command:
.DM
	make install
.DE
Finally, to configure
.B gas
for \*(CO release 4.2, use the command:
.DM
	configure --target=i386-coh --prefix=/u1/gnu
.DE
.Sh "Installing and Compiling GNU Binary Utilities"
.PP
Sources for the GNU bin utilities are installed into directory
.BR /u1/gnu/src/binutils-2.4 .
Installing the sources requires eight megabytes of disk space
on the file system that holds this directory.
Compiling these utilities requires an additional four megabytes of disk space
on this file system,
for a total of 12 megabytes of disk space.
.PP
To install the GNU bin utilities, use the command:
.DM
	/etc/install -c COHbin /dev/fva\fIN\fP 2
.DE
.I N
is the number of the 3.5-inch floppy-disk drive from which you are installing
the sources, either `0' or `1'.
.PP
To compile the GNU utilities, use the command:
.DM
	make CC=gcc CFLAGS=-O2
.DE
To install the utilities, use the command:
.DM
	make install
.DE
This command installs the binaries into directory
.BR /u1/gnu/i386-coh/bin .
It does so because it assumes that you wish to use the tools for
cross-development.
To make the tools available for use on your system, copy the tools that
interest you into directory
.BR /u1/gnu/bin .
Once you have verified that a tool has been copied from
.B /u1/gnu/i386-coh/bin
into
.BR /u1/gnu/bin ,
you should remove the tool from
.B /u1/gnu/i386-coh/bin
to save disk space.
You do not have to copy every tool; in particular, you should
.I not
copy the tools
.B strip
and
.B granlib
because they do not work correctly under \*(CO.
.PP
If you wish to use the GNU binary utilities by default (instead of their
\*(CO counterparts),
you must name directory
.B /u1/gnu/bin
at the
.I beginning
of the environmental variable
.BR PATH .
.PP
Finally, to configure the bin utilities for \*(CO 4.2, use the command:
.DM
	configure --target=i386-coh --with-stabs --prefix=/u1/gnu
.DE
.Sh "Installing and Compiling the GNU Debugger"
.PP
Sources for
.BR gdb ,
the GNU C source debugger, are installed into directory
.BR /u1/gnu/src/gdb-4.9 .
Installing the sources requires 12 megabytes of disk space
on the file system that holds this directory.
Compiling these utilities requires an additional three megabytes of disk space
on this file system,
for a total of 15 megabytes of disk space.
.PP
To install
.BR gdb ,
use the command:
.DM
	/etc/install -c COHgdb /dev/fva\fIN\fP 3
.DE
.I N
is the number of the 3.5-inch floppy-disk drive from which you are installing
the sources, either `0' or `1'.
.PP
To compile
.BR gdb ,
use the command:
.DM
	make CC=gcc CFLAGS=-O2
.DE
To install
.BR gdb ,
use the command:
.DM
	make install
.DE
Finally, to configure
.B gdb
for \*(CO 4.2, use the command:
.DM
	configure --target=i386-coh --prefix=/u1/gnu
.DE
.Sh "Installing and Compiling C++ Run-time Library"
.PP
Sources for the GNU C++ run-time library are installed into directory
.BR /u1/gnu/src/libg++-2.5.3 .
Installing the sources requires six megabytes of disk space
on the file system that holds this directory.
Compiling these utilities requires an additional three megabytes of disk space
on this file system,
for a total of nine megabytes of disk space.
.PP
To install this library, use the command:
.DM
	/etc/install -c COHlibg /dev/fva\fIN\fP 2
.DE
.I N
is the number of the 3.5-inch floppy-disk drive from which you are installing
the sources, either `0' or `1'.
.PP
To compile the library, use the command:
.DM
	make CC=gcc CFLAGS=-O2
.DE
To install the library, use the command:
.DM
	make install
.DE
Finally, to configure the GNU C++ library for \*(CO 4.2, use the command:
.DM
	configure --target=i386-coh --prefix=/u1/gnu
.DE
.SH Conclusion
.PP
Technical support is available to help you install these programs, and
recompile them as they are shipped, ``out of the box''.
However, due to the highly technical nature of these programs,
Mark Williams Company cannot provide technical support for any GNU program
if you have made any modification to it whatsoever.
.PP
We hope that you enjoy working with these sources!
