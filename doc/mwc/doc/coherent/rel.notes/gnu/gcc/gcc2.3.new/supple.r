.ds TL Installation
.NH Installation
.PP
This describes how to install the kernel-enhancement kit for COHERENT.
Note that you must have 5,000 blocks (that is, 2.5 megabytes)
of space free on your root file system to install this kit.
.PP
To install the kernel-enhancement kit, type the following command:
.DM
	/etc/install CohUpd \fIdrive\fP 2
.DE
where
.I drive
is the floppy-disk drive from which you are installing the kit.
For example, if you are installing from drive 0 (or A) and that
drive is a high-density, 5.25-inch floppy disk, use the following command:
.DM
	/etc/install CohUpd /dev/fha0 2
.DE
For a list of the names of all recognized floppy-disk drives, see the
Lexicon entry
.BR "floppy disk" .
.PP
.B install
will prompt you to insert each floppy disk in turn into drive A.
It will then copy each disk's contents onto your hard disk.
.B install
installs all items of the kernel-update kit into their usual places
in your COHERENT system.
For example, it installs the new version of the command
.B cc
into directory
.BR /bin .
