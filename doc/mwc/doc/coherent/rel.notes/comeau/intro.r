.NH "COHERENT Update for Comeau C++"
.PP
Thank you for purchasing the Comeau C++ package for \*(CO.
If you are running \*(CO with any release prior to 4.2.14, you
.I must
first install the supplemental-update disk for \*(CO, which
is included with this package.
The \*(CO update kit for Comeau C++ provides updated versions
of several \*(CO utilities (notably,
.B cc
and
.BR ld )
for use with Comeau C++.
.PP
If you are running \*(CO release 4.2.10 or later, do
.I not
install the supplemental update disk \(em it is unnecessary.
If you are running a version of \*(CO that predates release 4.2.05,
you must update to \*(CO 4.2.05 or later before you can run Comeau C++;
for details, contact Mark Williams Company.
.PP
To install the supplemental-update disk, do the following:
.IP \(bu 0.3i
Log in as the superuser
.BR root .
.IP \(bu
Insert the supplemental-update disk into a floppy-disk drive on your
system.
.IP \(bu
Type the following command:
.DM
	/etc/install ComoCOHUpd /dev/\fIdevice\fP 1
.DE
.I device
represents the name of the floppy-disk device into which you have inserted
the disk.
For example, if you inserted the disk into a 3.5-inch floppy-drive that is
drive 0 (i.e., drive A) on your system, you would type the following command:
.DM
	/etc/install ComeauCOHUpd /dev/fva0 1
.DE
For a complete listing of floppy-disk devices available under \*(CO, see
the entry for
.B "floppy disk"
in the \*(CO manual's Lexicon.
.PP
.B install
prompts you to insert the floppy disk into drive A.
It will then copy the floppy disk's contents onto your hard disk.
.B install
will install all items of the update kit into their usual
places on your \*(CO system.
For example, it installs the new
version of the command
.B cc
into directory
.BR /bin .
.PP
For details on how to install and test Comeau C++, see the full
Comeau C++ \*(CO User's Guide.
