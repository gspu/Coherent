.ds TI ""
.ds TL ""
.NH "COHERENT 4.2.10"
.PP
\*(CO 4.2.10 is a maintenance release of \*(CO that is designed to
fix problems reported with the last release of \*(CO, which is
release 4.2.05.
You have received this release because you have reported a problem
with your \*(CO system.
.PP
The following briefly describes how to update your \*(CO system to
\*(CO 4.2.10.
It also describes how to install it onto a system that does not now
have \*(CO.
.SH "Updating to COHERENT 4.2.10"
.PP
To update \*(CO 4.2.05 to release 4.2.10, do the following:
.IP \(bu 0.3i
Log in as the superuser
.BR root .
.IP \(bu
Insert the boot disk (disk 1) from your 4.2.10 installation kit into
drive 0 (drive A:) of your system.
.IP \(bu
Type the command
.DM
	/etc/shutdown halt 0
.DE
.IP
to shut down your system.
At the prompt, press the reset button on your computer.
.IP \(bu
A miniature version of \*(CO will be booted from the floppy disk in drive 0.
At the `?' prompt, type the command
.BR update .
.IP \(bu
Follow the messages and prompts as they appear on your screen.
.PP
The updating process closely resembles that for release 4.2.05, as described
in the release notes that you received with that product.
There are some differences, but these lie mainly in the wording of some
the messages and prompts, and in the order in which some tasks are performed.
.SH "Installing COHERENT 4.2.10"
.PP
To install \*(CO release 4.2.10, do the following:
.IP \(bu 0.3i
Turn off your system.
.IP \(bu
Insert the boot floppy disk (disk 1) from your installation kit into drive
A of your system.
.IP \(bu
Reboot your computer.
A miniature version of \*(CO will be booted from the floppy disk in drive A.
At the `?' prompt, type the command
.BR install .
.IP \(bu
Follow the messages and prompts as they appear on your screen.
.PP
The installation process closely resembles that for release 4.2.05,
as described in the release notes that you received with that product.
There are some differences, but these lie mainly in the wording of some
the messages and prompts, and in the order in which some tasks are performed.
.SH "A Remaining Problem"
.PP
\*(CO 4.2.10 still has a problem when it builds a file system
on a very large partition.
.PP
When you attempt to build a file sytsem on a disk partition that is larger
than approximately 200 megabytes, \*(CO may create a file system that
does not work properly.
This is because the program (called \fBmkfs\fR) that builds the file system
uses some default settings for aspects of the file system, including the
number of files that the file system can hold.
It sets the number of files based on the size of the file system it is
building; but when the file system is larger than 200 megabytes,
.B mkfs
occasionally attempts to build a file system that can hold an impossibly large
number of files (that is, more than 65,000).
This occurs under the following circumstances:
.IP \(bu 0.3i
When you use the default settings of
.B mkfs
to build a file system on a partition that exceeds 200 megabytes.
.IP \(bu
When you use
.B mkfs
to build a file system that exceeds 200 megabytes, using a prototype file
built by the program
.BR badscan .
.IP \(bu
When you explicitly request an impossibly large number of files.
.PP
If you are updating and your \*(CO file systems now work correctly,
do not alter anything when you update.
Your file systems will continue to work correctly.
.PP
If you are installing, however, and you are going to build a \*(CO file
system onto a partition that exceeds 200 megabytes, we suggest that you
do either of the following:
.IP \fB1.\fR 0.3i
During installation, when the time comes to change (or confirm)
the configuration of your hard disk, reorganize your disk so that no
partition exceeds 200 megabytes.
The release notes that came with \*(CO 4.2.05 describe how to do this.
Please note, however, that if you want DOS and \*(CO to reside on the
same hard disk, doing so could destroy your DOS file system and
require that you rebuild it.
This is described in detail in the release notes; study them carefully.
.IP \fB2.\fR
If you do not wish to alter the configuration of your hard disk, install
\*(CO without changing any partition.
However, make your root partition a partition that is smaller than 200
megabytes.
Then, when you have installed \*(CO and it boots properly, invoke
the command
.B mkfs
by hand with its option \fB\-i\fR explicitly
to set the number of i-nodes (that is, the number of files)
to a number smaller than 65,000.
You can then use that file system without any trouble.
.PP
If you observe these rules, you should be able to install \*(CO on very
large partitions without any trouble whatsoever.
.SH Conclusion
.PP
We would appreciate it if you would immediately report any problems you
experience to Mark Williams Company.
Send electronic mail to \fBsupport@mwc.com\fR; or telephone or write to
MWC Technical Support as described in the release notes.
.PP
Thank you for your patience in this matter; and we hope you enjoy
using \*(CO 4.2.10.
