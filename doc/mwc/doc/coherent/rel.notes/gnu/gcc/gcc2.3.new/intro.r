.ds TL Introduction
.NH "Introduction"
.PP
.B gcc
for \*(CO
is a port of the popular GNU C/C++ compiler and
related tools to the \*(CO 386 operating system.
It offers a powerful and flexible compiler with many features that are
not found in most C compilers.
.\" Too damn many, if you ask me.
It is also the basis for many other products available from the
Free Software Foundation, including
.B groff
and
.BR perl .
.PP
This pamphlet introduces
.B gcc
for \*(CO.
It describes the contents of the package, tells you how to install it,
and introduces the documentation that is included on disk.
.SH "Contents of the Package"
.PP
The
.B gcc
distribution holds the GNU C compiler,
GNU C++ preprocessor, the GNU debugger, and related tools.
It consists of three disks.
.PP
Note that this package contains no source code.
It contains only headers, libraries, executables, and documentation.
The source code for this release of GNU C is available
through Mark Williams Company as a separate package.
.SH "Requirements"
.PP
.B gcc
for \*(CO will run only on release 4.2 of \*(CO or any subsequent release.
It will not run on any release of \*(CO prior to release 4.2.
If you have any release of \*(CO prior to release 4.2, you must update
to release 4.2 before you can run
.B gcc
on your system.
.PP
In addition to running \*(CO 386, your system must have the following:
.IP \(bu 0.3i
At least four megabytes of RAM; eight megabytes or more is preferred.
.IP \(bu
At least 20,000 blocks (ten megabytes) of free space on your hard disk
partition which contains directory
.BR /usr .
If you do not have a file system with that much free space, you must
create free space or you will not be able to install the release.
.SH "Warranties and Support"
.PP
Mark Williams Company cannot offer support for the portions of this
product that come from the Free Software Foundation.
These include the
.B gcc
compiler itself, its preprocessor, and derived products.
These are supplied on an \fIas-is\fR basis only.
.\" We suggest you call the MacArthur Foundation
.\" and ask for Richard Stallman's phone number.
.PP
With these caveats in mind, we hope that you enjoy working with
.B gcc
for \*(CO!
.SH "Kudos"
.PP
We at Mark Williams Company warmly thank
.I "Kent Galbraith"
for his invaluable help with this product.
.SH Installation
.PP
The following describes how to install
.B gcc
for \*(CO 4.2 onto your system.
.PP
Note that unlike
.B gcc
for \*(CO 4.0, you do not need to install a supplemental update kit.
The
.B gcc
package works with \*(CO 4.2 just as they come out of the box.
To install the
.B gcc
package, you must have roughly 17,000 blocks (8.5 megabytes) free on
your root file system if you intend to uncompress the documentation,
or 15,000 blocks (7.5 megabytes) if you leave the documentation compressed.
.PP
To install the package from 5.25-inch floppy disks in floppy-disk drive 0
(i.e., drive A), use the following command:
.DM
	/etc/install COHgcc /dev/fha0 3
.DE
To install the package from 3.5-inch floppy disks in floppy-disk drive 0,
use the following command:
.DM
	/etc/install COHgcc /dev/fva0 3
.DE
.B install
will prompt you to insert each floppy disk in turn into drive A.
It will then copy each disk's contents onto your hard disk.
Finally, it will de-compress and un-archive all files that are shipped
in compressed or archived form.
.PP
.B install
will copy the files into directories
.BR /usr/bin ,
.BR /usr/local/lib ,
and
.BR /usr/local/docs .
The first two directories hold executable and support files; the last holds
GNU documentation.
The installation script also installs into
.B /usr/bin
the
.B gdb
debugger and stubs for
.B gcc
and
.BR g++ ,
so you do not have to alter your search
.B PATH
to use
.BR gcc .
.SH "Contents"
.PP
The
.B gcc
for \*(CO release consists of file installed into the following directories:
.IP "\fB/usr/bin\fP" 0.5i
This directory contains the following tools:
.BR c++ ,
.BR g++ ,
.BR gcc ,
.BR gdb ,
.BR gmake ,
.BR protoize ,
and 
.BR unprotoize .
.IP "\fB/usr/local/docs\fP"
This directory contains GNU documentation for the
.B gcc
and
.BR g++ ,
the debugger
.BR gdb ,
and the library
.BR libg++ .
.IP "\fB/usr/local/lib/gcc-lib/i386-coh/2.\fI*\fR"
This directory contains the actual compiler phases, run-time startup
modules, and the C and C++ libraries.
.IP "\fB/usr/local/lib/gcc-lib/i386-coh/2.\fI*\fB/g++-include\fR"
This directory, along with any subdirectories, contains all header
files needed by the
.B g++
compiler or the
.B libg++
library.
