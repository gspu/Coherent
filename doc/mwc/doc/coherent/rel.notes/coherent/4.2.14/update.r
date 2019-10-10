.ds RN "COHERENT"
.ds TL "Updating"
.ds TI "RELEASE NOTES"
.NH "Updating COHERENT"
.PP
If you are already running the \*(CO system and are not yet running release
4.2 of \*(CO, you need to
.I update
your system rather than
.I install
\*(CO.
Updating \*(CO basically involves copying a new image of the \*(CO
system into your system's root partition.
This process is fast and easy, but please read this section in its
entirety before you begin, in order to spare yourself unpleasant surprises.
.PP
The following description assumes that, as an experienced \*(CO user,
you understand what a \fIpartition\fR and a \fIfile system\fR is.
If you are unsure of what these are, see the sub-section
\fIHow a Disk Is Organized\fR, in the installation portion of these notes.
.SH "Before You Begin"
.PP
Before you begin to update your \*(CO system, you must make a few
decisions and check a few facts.
The following will help you prepare for updating.
.Sh "Ignore Your Serial Number"
.PP
Because you are updating \*(CO rather than installing it, your copy of
\*(CO 4.2 did
.I not
include a card with a serial number on it.
Don't panic!
.I
During updating, you will not be asked for your system's serial number.
.R
Therefore, you do not need the card with a serial number on it.
.Sh "Select a Partition"
.PP
Your first task is to select a partition into which you want to install \*(CO.
.PP
First, you must see if you have a partition that is large enough to hold
\*(CO 4.2.
.B su
to the superuser \fBroot\fR.
Then, if you have an ordinary AT-style disk, type the command:
.DM
	/etc/fdisk -r /dev/at0x
.DE
If you have a SCSI device, type the command:
.DM
	/etc/fdisk -r /dev/sd0x
.DE
This prints information about how your hard disk is partitioned.
The following shows the layout of a typical SCSI hard disk:
.DM
	Drive 0 currently has the following logical partitions:
	                 [ In Cylinders ]  [    In Tracks    ]
	Number     Type  Start  End  Size  Start    End   Size Mbytes  Blocks Name
	0       Coherent     0   19    20      1   1279   1279  20.96   40928 /dev/sd0a
	1       Coherent    20   79    60   1280   5119   3840  62.91  122880 /dev/sd0b
	2       Coherent    80  139    60   5120   8959   3840  62.91  122880 /dev/sd0c
	3 Boot  Coherent   140  197    58   8960  12671   3712  60.82  118784 /dev/sd0d
.DE
As you can see, this disk drive has four partitions,
each of which holds a \*(CO file system.
Device
.B /dev/sd0d
holds the root file system, as shown by the fact that it is marked as being
the ``Boot'' partition.
The size of the partition is in the next-to-last column, in blocks.
.PP
Now, you must select which partition you wish to use to hold the new
root file system.
The root partition must hold at least 20 megabytes (about 40,000 blocks),
of which at least 17,000 blocks must be free.
You can use a \*(CO partition other than the one that holds the current
\*(CO root file system,
although this will complicate the installation process.
.PP
If the device you wish to use as the root partition does not have at least
17,000 free blocks, you must free up some space by deleting or
compressing files.
.PP
Many \*(CO users choose to install \*(CO 4.2 onto a partition other than
their current root partition, rather than update their current root
partition.
This lets them run \*(CO 4.2 side-by-side with \*(CO 4.0, and gradually
configure \*(CO 4.2 to suit their preferences.
If you wish to do this, you should first back up all
files on that partition that were not part of the \*(CO 4.0 release,
as well as any files that you created.
This will ensure that your important files are not accidentally destroyed
during the installation process.
You must then perform a \fIfull installation\fP, as detailed in the first
section of these notes.
Only after you have successfully installed \*(CO 4.2 can you restore
your backups.
\fIDo not attempt to update a partition that does not hold
a \*(CO root file system!\fR
.PP
If you do not have a \*(CO root partition that is 20 megabytes or greater,
you must \fIinstall\fR \*(CO rather than \fIupdate\fR it.
The installation process will let you reconfigure your hard disk
to create a partition large enough to run \*(CO 4.2.
Before you begin installation, however, read the sub-section below entitled
\fISave System Files\fR, and follow its directions.
Then turn to the previous section in these notes, on installation,
and follow the directions there.
.PP
The update program will ask you for the device name
and partition number of your current \*(CO root partition.
Write down the device name and partition number.
.Sh "System Hardware"
.PP
During the update process, you will be asked a number of questions
about the hardware that your system has installed.
Make note of the following:
.IP \(bu 0.3i
Every monitor you have plugged into the system, and whether it is
plugged into a color or monochrome display card (i.e., video adapter).
\*(CO lets you have up to two monitors on your system, with
one plugged into a monochrome (e.g., MDA) display card and the other
into a color (CGA, EGA, or VGA) display card.
.IP \(bu
The number of printers you have,
the type of each printer (e.g., PostScript, LaserJet compatible),
and the port into which each is plugged.
.IP \(bu
The number of serial ports you have.
Note also which ones have local devices (e.g.,
terminals) plugged into them, and which have remote devices (e.g., modems).
Note whether you want the ports to be polled or interrupt-driven.
Write down your hardware configuration.
.Sh "System and Domain Name"
.PP
To use the mail and UUCP system under \*(CO, you must set the machine
name of your system.
If you have not yet named your system, select a name now.
The name should not exceed eight characters,
must consist of letters and numbers, and should begin with a letter.
.PP
If you have already named your system, you do not need to do so again.
For more information on the naming
conventions of machine and domain names,
along with suggestions on how to select a name for your system,
refer to sections \fISystem Name\fP and \fIDomain Name\fP in the
Installation chapter of these notes, as well as Lexicon articles
.B uucpname
and
.BR domain .
.PP
Write down the system and domain name.
.Sh "Turn Off Shadow Passwords"
.PP
Some users have installed shadow passwords on their system.
This stores users passwords in
.B /etc/shadow
rather
.BR /etc/passwd .
The
.B login
program included \*(CO 4.2 assumes that passwords are in
.BR /etc/passwd .
Therefore, if you are running shadow passwords, you should restore
.B /etc/passwd
before you attempt to update \*(CO.
.Sh "Save System Files"
.PP
The update program will make its best attempt to save all system files that
you have customized for yourself.
These files are saved by the update program into a
directory tree contained in archive
.B old_coh.taz
in your existing root file system.
The update program creates this archive prior to backing up \(em
\fIdo not create it yourself!\fP
.PP
If you had previously updated \*(CO (e.g., from release 3.2 to release 4.0),
you will already have a copy of this archive on your system.
The update procedure will do its best to keep this from being overwritten;
however, for safety's sake you should copy it to a floppy disk, or into
another directory, out of harm's way.
You should also rename it, so you do not confuse it with the archive that
the update program will create this time around.
.PP
The files that \*(CO will save during updating are named in file
.BR /conf/upd_save ,
which names the following files:
.DM
.ta 0.25i 2.25i 4.25i
	/.profile	/etc/.profile	/etc/brc
	/etc/checklist	/etc/domain	/etc/drvld.all
	/etc/modemcap	/etc/mount.all	/etc/passwd
	/etc/profile	/etc/serialno	/etc/termcap
	/etc/timezone	/etc/ttys	/etc/umount.all
	/etc/uucpname	/usr/lib/mail/aliases	/usr/lib/mail/paths
	/usr/lib/uucp/L-devices	/usr/lib/uucp/L.sys	/usr/lib/uucp/Permissions
	/usr/pub/phonebook
.DE
If it it does not name all the system-configuration files
that interest you, we suggest that you
edit it to name those files that are missing.
If you are updating from a release of \*(CO prior to release 4.2,
note that files
.BR L-devices ,
.BR L.sys ,
and
.B Permissions
are replaced by
.BR port ,
.BR sys ,
and
.B dial
as \*(UU is converted to Taylor \*(UU.
.PP
Once you are satisfied that
.B /conf/upd_save
names all of the system files that interest you, use it to make your own,
private copy of those files.
If you are updating from \*(CO 3.\fI?\fR or 4.0, use the command:
.DM
	tar -vzf $HOME/myarchive `cat /conf/upd_save`
.DE
.PP
If you are updating from \*(CO 4.2.05, use the command:
.DM
	gtar -xvzf $HOME/myarchive `cat /conf/upd_save`
.DE
.PP
.B update
will also attempt to save your old \*(CO 4.0 kernel by copying it into file
.BR /coh.B4.420 .
Note, however, that if updating should be interrupted and then restarted,
.B update
may overwrite your saved 4.0 kernel with the interim kernel it created
during the first, failed attempt at updating.
Therefore, you are well advised to save your \*(CO 4.0 kernel into
a file of your old naming (e.g.,
.BR /coherent.bak ),
before you begin to update your system.
.PP
.B update
overwrites all system files that (1) are in your root partition,
(2) come with the \*(CO system, and that
(3) are not named in
.BR /conf/upd_save .
You should save them, or their contents will be lost or modified.
For example, you may have modified the file
\fB/usr/games/lib/fortunes\fR.
Back it up, or your added words of wisdom will be lost.
If, however, you added a new fortune file to \fB/usr/games/lib\fR,
it will not be overwritten because it was not
originally included with your \*(CO distribution.
.PP
The point is, if you are doubtful about the status of any file
in your root partition \(em back it up!
If you find you do not need to reinstall it later,
you have a backup for future use.
.Sh "Manual Pages"
.PP
.B update
removes all files from directory
.BR /usr/man/COHERENT .
It also replaces the file
.BR /usr/man/man.index .
If you have installed into
.BR /usr/man/COHERENT
any non-\*(CO manual pages,
.B update
erases them.
You should save them into a directory of their own, e.g.,
.BR /usr/man/LOCAL ,
or they will be lost.
.PP
Note, too, that you must add to the file
index entries for those manual pages
.BR /usr/man/man.index ,
or the command
.B man
cannot find them.
The format of
.B man.index
has changed from release 4.0.
For details, see the Lexicon entry for the command
.BR man .
.Sh "C Code and Private Programs"
.PP
Almost all programs that run under \*(CO 4.0 will run
under \*(CO 4.2 without modification.
.PP
\*(CO 4.2 uses a new version of the header file
.B <stdio.h>
and of the standard I/O library.
This was done to achieve greater compatibility
with iBCS and \fBgcc\fR/\fBg++\fR libraries.
As a result of this, if you wish to modify a program, you must rebuild
.I all
existing
.B \&.o
and
.B \&.a
files, not just the modules that you modify.
If you do not, you may experience unresolved references from
the linker or programs may link but not work as expected.
However, linked binaries (that is, executable files)
are fine because they do not use the new headers or libraries
and thus do not need to be rebuilt.
.PP
In addition,
some programs that compile correctly under \*(CO 4.0 may need to be
modified to be compilable under \*(CO 4.2.
For example, the suite of header files changed under \*(CO 4.2, and your program
may need to include different files.
.PP
See the Lexicon entry \fBC language\fR for a summary of how the C language
is implemented under \*(CO 4.2.
The entry \fBheader files\fR names all header files, and notes which is
included with which implementation of \*(CO.
In addition, refer to section \fINotes and Errata\fP in these notes.
.SH "Updating COHERENT"
.PP
Now that you've finished your preparation, you're ready to update.
Log in as the superuser
.BR root .
If you are updating from \*(CO prior to release 4.2, type the command:
.DM
	/etc/shutdown
.DE
If you are updating \*(CO release 4.2.05, type the command:
.DM
	/etc/shutdown single 0
.DE
This will bring the system into single-user mode.
Then enter the command:
.DM
	sync
.DE
Before attempting to update your system, be sure to verify that enough
free space exists on your existing \*(CO root file system.
To do so, enter the command:
.DM
	df /dev/root
.DE
Command
.B df
display a line similar to:
.DM
	/dev/root   26794/ 75198 = 34.7%
.DE
The number before the slash indicates the number of free disk
blocks on the root file system.
The number after the slash indicates the maximum number of
data blocks on the file system.
In the above example, there are 26,794 free
blocks (about eight megabytes) on the root partition.
Because the update requires that the root file system have at least
20,000 free blocks, the example root file system has sufficient
free space to contain the \*(CO 4.2 update.
.PP
Next, enter the command:
.DM
	fsck
.DE
This will ensure that your file systems are in good shape.
Installing an update of \*(CO into a damaged file system could cause serious
problems, so be sure that your file systems are intact before you continue.
.PP
Next, take the \fBBoot\fR disk from your \*(CO installation kit, and
insert it into drive A on your computer.
Then press the reset button.
The screen will clear, and you will see the following prompt:
.DM
	COHERENT Tertiary boot Version 1.2.7
	If installing COHERENT, please type "begin".
	?
.DE
Type the following:
.DM
	update
.DE
This invokes the \*(CO update program, which begins the updating process.
\fIDo not type ``begin'':
that invokes the installation program!\fR
.PP
Your system will then boot a miniature version of \*(CO 386
from the floppy disk.
This miniature \*(CO executes a script that controls the updating process.
.PP
The rest of the updating process is straightforward, and consists mainly
of answering the few questions described above.
The previous section in these notes, on installation, describes the
questions that
.B update
asks, and gives the text of most every prompt and message.
We suggest that you read it for a preview of updating.
.PP
Many users have updated their \*(CO systems with few problems.
We expect that your experience will be the same.
Should a problem arise, however, review the previous section's final
part, which discusses the commoner problems and what you can do to
cope with them.
If your system cannot boot \*(CO 4.2 from the floppy disk, chances
are that you cannot run it on your computer.
Refer to the Copyright page of these notes for information on contacting
the Mark Williams Technical Support department.
.SH "Restarting Updating"
.PP
Some users have, for one reason or another, been forced to interrupt
updating their \*(CO systems midway throught he updating process.
These users will need to restart the updating process.
.PP
To restart
.BR update ,
do the following:
.IP \fB1.\fR
First boot your \*(CO 4.0 kernel.
.B update
will have saved it in file
.BR /coh.B4.420 .
.IP \fB2.\fR
Bring up your system in single-user mode.
You can do this by pressing
.B <ctrl-C>
while the command
.B fsck
is checking your file systems, before your system has entered multi-user
mode.
.IP \fB3.\fR
If you had completed copying all of the disks of your \*(CO 4.2 release
onto your system, cd to directory
.B /conf
and invoke the script
.BR Coh_420.post .
This will walk you through the process of configuring your system for
\*(CO 4.2.
.IP \fB4.\fR
If you had
.I not
completed copying all of the disks of your \*(CO 4.2 release onto your
system, type the command:
.DM
	rm /Coh_420.?
.DE
Then insert the boot disk from your installation kit into drive A and
reboot your system.
.PP
Note that if you are having trouble updating your \*(CO system for whatever
reason, you can
.I install
\*(CO 4.2 onto a partition other than your current root partition.
Then, you can run \*(CO 4.0 and \*(CO 4.2 side by side, and gradually migrate
from one system to the other.

