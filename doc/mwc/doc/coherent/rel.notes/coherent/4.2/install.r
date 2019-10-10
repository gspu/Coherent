.ds TL "Installation"
.NH "Installing COHERENT"
.PP
The following describes how to install \*(CO onto your computer.
Installation of \*(CO is straightforward, and Mark Williams Company
has prepared a suite of programs that automate much of the work for you.
.PP
Tens of thousands of users have successfully installed \*(CO onto their
computers.
Most have found the process to be easy and quick.
An uncomplicated installation will take less than an hour.
.PP
A few users have experienced some problems with installation;
so if installation does not go as smoothly as you would like, the end of this
section describes the commoner problems and how you can fix them.
If all else fails, telephone Mark Williams Technical Support, as described
in the introduction to this pamphlet.
.SH "Installing Vs. Updating"
.PP
Before you continue any further, please note that this section is for
users who are not yet running \*(CO on their computers.
\fIIf you already are running \*(CO and are updating to \*(CO 4.2,
\fRskip\fI this section and instead read the following section, \fRUpdating
\*(CO.\fI
If you re-install \*(CO over an existing \*(CO system, any \*(CO
files you already have will be wiped out!\fR
.PP
We do not mean to frighten you.
However, we do know how much time you can spend customizing your \*(CO
system; and we would hate to see all that work lost due to a few
moments' carelessness.
So, we'll say it once again:
\fIIf you already are running \*(CO, you want to \fRupdate\fI \*(CO
rather than \fRinstall\fI it.
\fRSkip\fI this section and instead
read the following section, on updating \*(CO!\fR
.SH "Before You Install"
.PP
As you have read this far, we assume that you are \fInot\fR now running
\*(CO, and that you do wish to \fIinstall\fR \*(CO onto your system.
Before you begin installation, you need to make a few decisions about how
you want to configure your computer;
and you need to collect some information
about your system.
We suggest that you make a checklist and fill it in before you begin
installation.
.PP
If you wish to have \*(CO reside on the same hard disk as \*(MD,
OS/2, or Windows NT,
be sure to read the section below entitled \fIInstalling COHERENT
With Other Operating Systems\fR.
.Sh "Caveats"
.PP
Although \*(CO is a flexible system that can run on a wide variety
of machines, it cannot run on everything.
The following conditions must apply before you can install
\*(CO on your system:
.IP \fB1.\fR 0.3i
Your system hard disk must have a free partition with at least
15 megabytes of space free on it.
More is recommended, but 15 megabytes is the minimum space required
to install \*(CO.
You need at least 40 megabytes if you wish to install X Windows for \*(CO
also.
.sp \n(pDu
If your system does not have a free partition, you must either take one away
from \*(MD, or you must split an existing DOS partition into two and
give one (or both) to \*(CO.
In either case, all data on that partition will be destroyed.
This is detailed below.
.IP \fB2.\fR
Your system must be based upon the Intel 80386 or 80486 processor,
or an exact clone.
.IP \fB3.\fR
Your system must have at least two megabytes of available RAM.
If you wish to run X Windows for \*(CO, you must have at least four
megabytes, at least eight is recommended.
.\".sp \n(pDu
.\"Please note that some clone motherboards do not
.\"``map'' the region of RAM which lies between 640K (0xA0000) and one
.\"megabyte (0x100000).
.\"If your system uses one of these motherboards and contains only
.\"one megabyte of RAM chips,
.\"you effectively have a 640K system and will not be able to install \*(CO.
.IP \fB4.\fR
Your system components must be 100% IBM compatible in order to run \*(CO.
The system bus can be based upon either the Industry Standard Architecture (ISA)
or the Extended Industry Standard Architecture (EISA).
MicroChannel (MCA) machines are not yet supported.
.IP \fB5.\fR
If you are installing \*(CO on a notebook or other
computer system that uses a modified 83-key layout rather than the
industry standard 101-key or 102-key layout, you may experience
key transpositions and other anomalies.
If you are using the
.B nkb
loadable keyboard driver, read Lexicon articles
.B "keyboard tables"
and
.B nkb
for information on how to modify the keyboard-mapping tables for your system.
.IP \fB6.\fR
Your system must have at least one high-density, 3.5-inch or
5.25-inch floppy-disk drive installed as drive A.
The distribution disks for \*(CO cannot be read by a low-density
floppy-disk drive, nor can they be converted from one size of disk to
the other.
.sp \n(pDu
Due to limitations in the BIOS of the IBM PC and its clones, \*(CO
can be installed \fIonly\fR from drive A.
Thus, if your system has both sizes of floppy-disk drive, your release of
\*(CO must be for the disk drive that your system knows as drive A.
.PP
If your system meets these criteria, then you probably can install and
run \*(CO, as tens of thousands of other users have done.
Check the list of compatible and incompatible hardware in the second
section of this pamphlet to see if any of your hardware is listed.
If your system does not appear on either list, you probably can still install
and run \*(CO \(em but Mark Williams Company will be interested in your
experience.
.PP
The following sections discuss your preparations in more detail.
.Sh "Hard Disk"
.PP
The hard disk and its controller is one of the most important elements
with which \*(CO must work.
\*(CO continually works with the hard disk:
it loads programs, creates temporary files, updates existing data files,
uses information stored on disk to access peripheral devices, and so on.
Thus, how you configure your hard disk will make a great difference to
your system.
.PP
A \*(CO system can have several hard disks:
up to two IDE (AT-style) disks, and an indefinite number of SCSI disks.
You can have up to two controllers on your system; if you have more than
one controller, one must be IDE and the other SCSI.
.PP
The point of the installation procedure is to create one or more partitions on
a hard disk to contain \*(CO and its files.
When you (or your dealer) installed \*(MD on your computer, you (or he)
divided your computer's hard disk (or disks) into \fIpartitions\fR.
Each hard disk can have from one to four partitions.
Partitions are numbered sequentially:
If you have one hard disk, the partitions are numbered 0 through 3;
if you have two hard disks, the partitions on one hard disk are numbered
0 through 3 and on the other are numbered 4 through 7; and so on.
.PP
Not every partition has to be used \(em a hard disk can be divided
into four partitions, but have \*(MD file systems in only two of them,
with the third and fourth partitions being idle.
Note, too, that the four partitions do not necessarily have to
encompass the entire hard disk \(em a disk may have space that is outside
any partition and so just remains unused.
.PP
The file systems for \*(CO and \*(MD are very different, so it is not
possible to have both systems use the same partition \(em each
must have one or more partitions completely to itself.
(Note, however, that \*(CO comes with tools that let you copy files
from an \*(MD file system to \*(CO and back again.)
.PP
As you can see, installation must cope with a number of variables:
the number of hard disks on your system,
the type of controller or controllers on your system,
the size of each disk,
the number of partitions into which each disk is divided,
the size of each partition,
and the number of partitions that are in use.
Installation thus will follow any of a number of possible scenarios,
depending on how your system is organized and how much disk space you wish
to give to \*(CO.
The installation process will walk you through these decisions, to make
them as painless as possible.
.PP
If you have one hard disk, you may have either one or two \*(MD partitions.
The first partition is the \*(MD primary partition, and
the second is the \*(MD extended partition.
.PP
\*(MD lets you sub-divide the extended partition into logical drives D, E, F,
and so on.
\*(CO
.I cannot
be installed onto a logical drive.
In this scenario, you have these options:
.IP \fB1.\fR
Install \*(CO onto
.I all
of the \*(MD extended partition.
You will lose drives D, E, F, etc., and
.I all
data in them.
.IP \fB2\fR
Use the \*(MD version of the command
.B fdisk
to delete an existing partition, then re-create it
while making it smaller by however much disk space you wish to allocate
to \*(CO.
Remember that \*(CO requires at least 15 megabytes; while \*(CO plus X
requires at least 40 megabytes.
.IP
This step frees disk space from \*(MD.
.B build
will see this freed space and allow you to turn this into a \*(CO
partition (or partitions).
.IP \fB3.\fR
If your hard drive has space that lies outside of any existing partition,
.B build
will allow you to turn it into a \*(CO partition.
.PP
These options apply even if you already have \*(MD plus any other operating
system (e.g., OS/2 or Windows NT) on your system.
.PP
Why is this neccessary?
Because \*(CO, like \*(UN, is a
stand-alone operating system that requires its own bootable partition.
.PP
It may be that you do not know the configuration of your hard disk (or disks).
\*(CO can figure this out, and the
information will be displayed for you at the appropriate point in installation.
We recommend that you keep a copy, on paper, of your
computer's partitioning.
This information can be vital in helping you to recover from a catastrophic
error with your hard disk.
.PP
For an overview of how \*(CO handles hard disks, see the Lexicon article
\fBhard disk\fR.
.Sh "Hard-Disk Controller"
.PP
Another variable is the type of hard-disk controller you have.
\*(CO works with RLL, MFM, IDE, and most ESDI hard-disk controllers.
It also works with some SCSI host adapters.
Please check the previous section for a list of supported hard disk
controllers and host adapters.
.PP
If you intend to install \*(CO on a system that uses one of the
supported SCSI host adapters, be sure that your host adapter is jumpered
to enable interrupts and that the interrupt number and other configuration
information is available prior to beginning the installation of \*(CO.
.PP
If you have a SCSI drive, \*(CO will ask you during installation for
the following information about your SCSI drive.
You should jot down the requested information before you begin installation:
.IP \fB1.\fR 0.3i
The number of SCSI drives you have.
.IP \fB2.\fR
Manufacturer of host adapter, and parameters:
Adaptec, Seagate, or Future Domain.
.IP \fB3.\fR
If you have an Adaptec host adapter, does your device's BIOS uses
a 64-head translation mode?
This is the default.
.IP \fB4.\fR
Which IRQ does the host adapter use?
The default is 11 for Adaptec, 5 for Seagate or Future Domain devices.
.IP \fB5.\fR
What is the hexadecimal host adapter base-port address?
The default is 0x330 for Adaptec, 0xCA00 for Seagate or Future Domain devices.
.IP \fB6.\fR
Which DMA channel does the host adapter use?
The default is 5.
.IP \fB7.\fR
For each SCSI device you have, you must assign a SCSI ID, zero through seven.
If you have only one SCSI device, the default is zero.
.Sh "Peripheral Devices"
.PP
You should note which ports you have plugged
your printers, modems, and other devices.
The installation program will ask you for this information so it can
correctly configure your system.
.PP
The installation program will also ask you if you wish to have one or more
of your serial ports in polled mode.
For information on what this means, see the Lexicon article on
.BR asy .
.PP
You will also be asked about what console devices you have on your system.
Currently, the \*(CO install procedure lets you install up to a total of
eight virtual-console sessions between two physical consoles.
For example, if you have two consoles, you could have six virtual consoles
on the color device and two on the monochrome; whereas if you have only a color
video card, you could have all eight console sessions on that console.
Note the number of video cards plugged into your system, and whether the
cards are monochrome or color.
.Sh "System Name"
.PP
When you install \*(CO you will be asked what the name of your
system is.
You must do this to use electronic mail under \*(CO; this will permit
other systems to identify your system.
You should pick a name that is easy to remember, yet probably unique;
and no name should be longer than eight characters.
.PP
See the Lexicon article
.B uucpname
for details on system names, and examples.
.Sh "Domain Name"
.PP
Finally, when you install \*(CO you will be asked to name your system's
electronic mail domain.
If you will be adding your system to a \*(UU domain, you should enter its
name; otherwise, you should select the default domain name \fIUUCP\fP.
If you join a \*(UU domain later, or change domains, you can name that
domain simply by editing the file \fB/etc/domain\fR.
.PP
See the Lexicon article
.B domain
for details on domains.
.SH "Installing COHERENT"
.PP
Once you have jotted down the information about your system and,
if necessary, backed up your data and used \*(MD \fBfdisk\fR
to free up a partition for \*(CO, you are ready to begin installation.
.PP
You can abort the installation procedure at any time by typing
\fB<ctrl-C>\fR.
However,
aborting installation does \fInot\fR mean that your hard disk will be returned
to the state it was in before installation was begun.
\fIWhen a disk is repartitioned, the files that were on any
modified partitions are gone forever!\fR
.PP
The following sections describe the installation process in some detail.
Once again, read them through before you begin.
.PP
During the installation, \*(CO explains what it is doing.
It asks your permission to execute any action
that could destroy data on your disk.
.Sh "Beginning Installation"
.PP
To begin installation, turn your system off.
Take the \fBBoot\fR disk from your installation
kit and insert it into floppy-disk drive A.
(The disk
.I must
be placed into drive A.
Installation will not work if you put it into drive B.)
.PP
Now, turn your system on.
Your system will boot a mini-version of \*(CO from the boot disk;
in a moment, you see a question mark \*(Ql?\*(Qr, which is a prompt.
Type:
.DM
	begin
.DE
and press the \*(RT key.
This loads \fBbuild\fR, the \*(CO installation program, into memory and
begins execution.
As \*(CO loads
.BR build ,
it entertains you with a spinning character,
with an occasional ``subliminal'' message that urges you to purchase COHware.
.PP
When
.B build
is loaded into memory and begins execution, it displays a copyright notice.
Press \*(RT.
You will see the following greeting:
.B1
	Welcome to the COHERENT operating system!
.sp \n(pDu
	Your computer is now running COHERENT 386 from the floppy disk.
	This program will install COHERENT onto your hard disk.
.sp \n(pDu
	If you are already running COHERENT on your hard disk, you must perform an
	update rather than a full installation.  To do so, please REBOOT NOW and
	follow the detailed update instructions in the COHERENT release notes supplied
	with this release.
.sp \n(pDu
	You can interrupt installation at any time by typing <Ctrl-C>;
	then reboot and start the installation procedure again.
	Please be patient and read the instructions on the screen carefully.
.B2
.PP
As the instructions say, you can interrupt installation at any point by
typing \fB<ctrl-C>\fR.
.Sh "Entering the Serial Number"
.PP
The next screen will ask you to enter a nine-digit serial number:
.B1
	A card included with your distribution gives the serial number
	of your copy of COHERENT.
.sp \n(pDu
	Type in the serial number from the card:
.B2
.PP
This number is included on the registration card
supplied with your copy of \*(CO.
The installation process cannot continue until you enter this number
correctly.
If you type the wrong number three times, installation will abort;
so type carefully.
.Sh "Configuration of Hard Disks"
.PP
At the next step,
.B install
asks you for the number and type of hard disks you have:
.B1
	Most PC compatible computer systems use MFM, RLL, IDE, or ESDI disk
	controllers and disk drives.  A few percent use SCSI disk drives.
	Please indicate the type(s) of disk drive(s) used in your computer system.
	If you are uncertain of the type, please select choice 1.
.sp \n(pDu
	Are you using:
.sp \n(pDu
	1.  AT-compatible hard drive controller (IDE/RLL/MFM/ESDI).
	2.  SCSI hard drive controller.
	3.  Both.
.sp \n(pDu
	Enter your choice:
.B2
.PP
Select the appropriate entry:
1, 2, or 3.
.Sh "Installing an AT Disk"
.PP
If your system has an AT-compatible hard-drive controller
(i.e., IDE, RLL, MFM, or ESDI controller),
.B build
will ask you about polling:
.B1
	Most AT-compatible controllers work with NORMAL polling.
	Perstor controllers and some IDE hard drives require ALTERNATE polling.
	If you get "<Watchdog Timeout>" or "at0:TO" errors with normal polling,
	use alternate polling.
.sp \n(pDu
	Use NORMAL polling?
.B2
.PP
In nearly every case, you should answer ``Yes''.
.B build
then asks:
.B1
	Enter number of non-SCSI hard drives:
.B2
.PP
Please do so.
If you have another non-SCSI hard drive,
.B build
repeats the above routine for that second drive.
.PP
Finally,
.B build
asks:
.B1
	Are you installing on an IBM PS1 or ValuePoint?
.B2
.PP
If you are, answer `Y'.
.B build
will ask you a few additional questions that apply only to those machines.
.Sh "Installing a SCSI Disk"
.PP
If you indicated that
you are installing \*(CO onto a SCSI disk drive, at this point
.B build
asks for information about your SCSI device.
(If you do not have a SCSI device, skip to the next section.)
.PP
First, it asks what type of device you are using:
.B1
	COHERENT currently supports the following SCSI host adapters:
.sp \n(pDu
	(1) Adaptec AHA-154x series, with or without tape support
.sp \n(pDu
	(2) Adaptec AHA-154x series, no tape support (alternate driver)
	(3) Seagate ST01 or ST02
	(4) Future Domain TMC-845/850/860/875/885
	(5) Future Domain TMC-840/841/880/881
.sp \n(pDu
	Enter a number from the above list or 0 to exit:
.B2
.PP
If you intend to install \*(CO on a system which uses one of these
supported SCSI host adapters,
be sure that your host adapter is jumpered to enable interrupts
and that the interrupt number and other configuration
information is available.
.PP
.B build
then walks you through a dialogue to gather the information described
in the earlier section entitled \fIHard Disk\fR.
If you do not know what your disk uses, or if you do not understand
what the question is asking, you should select the default.
.PP
.B build
first requests the IRQ for your host adapter:
.DM
	Which IRQ does the host adapter use? 
.DE
.PP
.B build
then asks for the base port's address:
.DM
	What is the hexadecimal host adapter base port address? 
.DE
.PP
Use the default if you do not know the address.
.PP
.B build
then asks about the DMA channel:
.DM
	Which DMA channel does the host adapter use? 
.DE
.PP
Again, the default is your best bet if you are not sure.
.PP
If you are using an Adaptec disk,
.B build
then asks:
.DM
	How many heads will be used for translation mode?
.DE
.PP
If you're not sure, press \*(RT to select the default.
.B build
then asks about the number of sectors per track:
.DM
	How many sectors per track will be used for translation mode?
.DE
.PP
Unless you're certain of a different value, press \*(RT to select the default.
.PP
.\".B build
.\"then asks about the IRQ number:
.\".DM
.\"	Which IRQ number does the host adapter use [\fIN\fP]? 
.\".DE
.\".PP
.\"Press \*(RT to accept the default.
.\".PP
.\".B build
.\"then asks for base-segment address:
.\".DM
.\"	Your host adapter is configured for a base segment address.  Possible
.\"	values are: C800, CA00, CC00, CE00, DC00, and DE00.
.\"	What is your 4-digit hexadecimal base address [\fIN\fP]? 
.\".DE
.\".PP
.\"If (like most people) you do not know what the base-segment address is,
.\"press \*(RT to select the default.
.\"In all probability, this will work fine.
.\".PP
.\"Finally,
.B build
asks you to name a SCSI ID for each device that you have plugged into your
SCSI board.
This ID, which is any integer from zero through seven,
represents the device's address on the SCSI bus.
You do not need these addresses to assign a SCSI ID.
Most host-adapter BIOSes work best if SCSI hard drives are given IDs that begin
at zero and go up by increments of one.
A system might not boot properly otherwise.
SCSI tape is often at ID 2.
.PP
If you just have a hard disk plugged into your SCSI controller, you see:
.B1
	You must specify a SCSI-ID (0 through 7) for each SCSI hard disk device.
	Each SCSI hard disk device can contain up to four partitions.
.sp \n(pDu
	1) Enter SCSI ID for SCSI hard disk (fixed or removable media)
	2) End entering SCSI ID values.
.sp \n(pDu
	Your choice:
.B2
.PP
You must enter a unique SCSI ID number for each SCSI hard disk
(\fInot\fR to each partition).
If you have a tape device, you must enter a SCSI ID number for it as well.
We suggest that you assign SCSI ID numbers beginning with zero.
Start with your hard disk or disks, and then assign an ID to each tape drive.
.Sh "Back Up the Hard Disk"
.PP
After you have entered information about time and your disk,
installation moves on to its next phase,
partitioning the hard disk.
Before you become seriously involved in partitioning, however, you have
one last chance to back up your hard disk.
As you enter the partition phase of installation, you will see the following
text:
.B1
	This installation procedure allows you to create one or more partitions
	on your hard disk to contain the COHERENT system and its files.
	Each disk drive may contain no more than four logical partitions.
	If all four partitions on your disk are already in use, you will
	have to overwrite at least one of them to install COHERENT.
	If your disk uses fewer than four partitions and has enough unused space
	for COHERENT (\fIN\fP megabytes), you can install COHERENT into the unused space.
	If you intend to install MS-DOS after installing COHERENT,
	you must leave the first physical partition free for MS-DOS.
.sp \n(pDu
	The next part of the installation procedure will let you change the
	partitions on your hard disk.  Data on unchanged hard disk partitions
	will not be changed.  However, data already on your hard disk may be
	destroyed if you change the base or the size of a logical partition,
	or if you change the order of the partition table entries.
	If you need to back up existing data from the hard disk,
	type <Ctrl-C> now to interrupt COHERENT installation; then reboot your
	system and back up your hard disk data onto diskettes.
.B2
.PP
If you need to back up your hard disk and have not yet done so, please abort
.B build
and do so now.
.Sh "Use the COHERENT Bootstrap?"
.PP
If you have already backed up your hard disk, continue to the next phase,
which is to decide whether to use the \*(CO master bootstrap.
When you press \*(RT, you will see the following text:
.B1
	COHERENT initialization normally writes a new master bootstrap program onto
	your hard disk.  The COHERENT master boot allows you to boot the operating
	system on one selected disk partition (the active partition) automatically;
	it also allows you to boot the operating system on any disk partition by
	typing a key when you reboot.  Mark Williams strongly recommends that you
	use the COHERENT master boot.  However, the COHERENT master boot may not
	work with some operating systems (for example, Xenix) if you make the
	COHERENT partition active; instead, leave the other partition (e.g. Xenix)
	active and boot COHERENT by typing a key.  If you do not use the COHERENT
	bootstrap, you must understand how to boot the COHERENT partition using your
	existing bootstrap program.
.sp \n(pDu
	Do you want to use the COHERENT master boot?
.B2
.PP
As explained in the prompt text, a \fIbootstrap\fR is a program that
pulls an operating system into memory and sets it to running \(em the
name relates to the fact that the operating system \*(QLpulls itself up
by its boot straps\*(QR.
The \*(CO master bootstrap can boot \*(CO as well as many other
operating systems, including \*(MD (at least, the many versions of \*(MD
that have been tested).
If you choose not to use the \*(CO master bootstrap, you must consult
the documentation that came with your system to see how you can use your
operating system's current bootstrap routine to boot another operating system.
If, however, you choose to use the \*(CO master bootstrap and find that
it has trouble booting your current operating system, you should be able to
boot your current operating system by using a boot floppy disk; you will not
be able to boot it off of the hard disk, but at least it will be available
to you.
For these reasons, we strongly suggest that you use the \*(CO master
bootstrap routine.
.SH "Partitioning the Hard Disk"
.PP
.B build
then moves into the next phase:
selecting one or more disk partitions for \*(CO.
As described above, partitioning can vary greatly from disk to disk;
how the disk is partitioned will determine how much space is allocated
to \*(CO and how much to \*(MD.
This is the trickiest part of installation, so be sure to read carefully.
.Sh "How a Disk Is Organized"
.PP
This phase begins by displaying the current layout of your hard disk:
the number of partitions, the size of each partition, and the operating
system associated with each partition.
Note that the partition table is displayed in \fIphysical\fP order (i.e., the
actual order in which the partition table entries occur).
Some operating systems display the table in \fIlogical\fP or sorted order.
.PP
The following gives the printout for a typical hard disk.
This hard disk, called disk \fB0\fR,
has approximately 33 megabytes on it organized into
two \*(MD partitions, as follows:
.B1
	Drive 0 Currently has the following logical partitions:
	              [In Cylinders] [  In Tracks  ]
	Number  Type  Start End Size Start  End Size Mbyte Blocks Name
	0 Boot MS-DOS    0  149  150     0  899  900  7.83  15300 /dev/at0a
	1     EXT-DOS  150  614  464   900 3684 2784 24.28  47430 /dev/at0b
	2      UNUSED    0    0    0     0    0    0     0      0 /dev/at0c
	3      UNUSED    0    0    0     0    0    0     0      0 /dev/at0d
.B2
.PP
If you have more than one hard disk on your machine, then
you will see the table for the second disk after you have finished
partitioning the first.
If you do not wish to change the partition table on the first drive and
you wish to install \*(CO onto the second, selection option 7,
.IR "Proceed to next drive" .
Note that you
.I must
respond
.B yes
to the question ``Do you wish to update the partition table,'' or
.B build
will not continue to the next drive.
.PP
Note that \*(MD names each ``drive'' by a letter of the alphabet,
beginning with C.
(This nomenclature is a hold-over from the days when \*(MD only ran on
floppy disks, with `A' and `B' reserved for the two floppy-disk drives.)
Because \*(MD allows you to have only two DOS partitions on any
given hard disk, then if an \*(MD system has more than two ``drives'' on
one hard disk, the additional drives are \fIlogical\fR drives and are a
division of the EXT-DOS partition.
\*(CO cannot be installed onto a logical drive.
.PP
As mentioned above, we suggest that you copy down this table before
continuing; if an error were to occur, this information will help you
recover the data on your disk.
.PP
As you can see, this disk has four partitions, numbered 0 through 3.
Partition 0 is marked as the boot partition; what this means is
explained below.
\*(CO has given each partition a name, \fB/dev/at0a\fR through
\fB/dev/at0d\fR; you will not be working with these, however, so you can
safely ignore them for now.
.PP
Note that the middle columns of the table give
the size of each partition in three ways:
in cylinders, in tracks, and in megabytes.
How do these differ?
Megabytes is the easiest to understand:
that is the number of bytes that can be written into the partition.
Cylinders and tracks, however, relate to the way a hard disk is built.
A moment spent here on background can make what is to come much easier
to understand.
.Sh "Cylinders and Tracks"
.PP
Consider a high-density floppy disk.
Its surface is organized into 80 concentric rings, or \fItracks\fR,
numbered 0 through 79.
Each track holds a fixed amount of data; the amount held
depends upon the density of the disk.
When the disk is in your disk drive, a \fIhead\fR
moves back and forth, reading tracks as directed.
Unlike a phonograph cartridge, however,
the head jumps from track to track discretely \(em it does not spiral in.
Thus, you can measure space on a disk simply by counting the tracks.
Note, too, that
the term \*(QLhead\*(QR is often used to describe one surface of a
multi-sided disk.
.PP
As you've probably noticed, a floppy disk has two surfaces:
the top and the bottom.
The top is usually refered to as side 0, the bottom as side 1.
Each surface has its own system of tracks, each numbered 0 through 79,
giving a floppy disk a total of 160 tracks.
Also, to read the disk a floppy disk drive actually has two heads,
one for each surface.
.PP
A \fIcylinder\fR is the set of identically numbered tracks from both
surfaces of the disk.
A floppy disk organizes its 160 tracks into 80 cylinders:
side 0 track 0 plus side 1 track 0 form cylinder 0; side 0 track 1
plus side 1 track 1 form cylinder 1, etc.
If you think of a track as being a ring on the disk, then origin of
the term \*(QLcylinder\*(QR should be obvious.
.PP
Now, consider a hard disk.
The term \*(QLhard disk\*(QR is somewhat incorrect, because one
hard disk actually contains many hard disks, or \fIplatters\fR,
inside itself.
The platters are stacked on a spindle, much like a set of 45-rpm
records stacked on a record changer \(em except that heads move
between the platters, one head for each platter surface (or two per platter).
The number of platters and the number of tracks on each platter determine
both the number of cylinders and the amount of data that can be written to
the disk.
Consider the disk described in the above table, which is a fairly
typical device.
It has three platters (six heads).
Each head has 615 tracks, each of which holds 8,704 bytes.
Thus, the device has a total of 3,690 tracks (6 times 615), organized into
615 cylinders, with each cylinder holding 52,224 bytes (6 times 8,704).
.PP
Different operating systems organize disk partitions in different ways.
\*(MD, for historical reasons, organizes partitions along track boundaries;
under this scheme, the tracks of a cylinder can
be divided between two partitions.
\*(UN, \*(CO, and similar operating systems prefer to organize partitions
along cylinder boundaries:
all of the tracks of a cylinder belong to only one partition.
This lessens movement of the heads, which in turn speeds up reading of the disk.
Note that, strictly speaking, ``megabytes'' has no meaning for the task of
disk partitioning:
partitioning must be done either in tracks, or in cylinders.
Humans tend to think of partitions in terms of megabytes, that is, in terms
of the amount of data we can write into a partition, but when organizing your
disk it is much easier to think in terms of cylinders.
However, it is simple to translate cylinders into megabytes, which gives you
the best of both worlds; this will be discussed in the following sub-section.
.Sh "Partitioning the Disk"
.PP
Now that we have discussed partitioning in theory, here's now
.B build
walks you through the process.
.PP
.B build
begins by displaying a summary of information about disk 0 on your system.
.B1
	Drive 0 has 615 cylinders, 6 heads, and 17 sectors per track.
	It contains:
		615 cylinders of 52224 bytes each,
		3690 tracks of 8704 bytes each,
		62730 sectors of 512 bytes each
	or a total of 32117760 bytes (32.12 megabytes).
.B2
.PP
It then asks you to confirm if this is correct.
Your confirmation shows that
.B build
is reading information from your system correctly.
.PP
When you enter the partitioning phase of installation, the installation
programs displays the following message:
.B1
	This program will let you change partition information for each disk drive.
	A disk drive can be divided into one to four logical partitions.
	You can change the active partition (the partition which your
	system boots by default) or change the layout of logical partitions.
	Other programs which change hard disk partition information
	may list logical partitions in a different order.
	Hit <Esc><Enter> to return to the main menu at any time.
.sp \n(pDu
	Now hit <Enter>.
.B2
.PP
It then displays the configuration of your hard disk for you, using
a table like the one shown above, and the following menu:
.B1
	Possible actions:
	   0 = Quit	
	   1 = Change active partition (or make no partition active)
	   2 = Change one logical partition
	   3 = Change all logical partitions
	   4 = Delete one logical partition
	   5 = Change drive characteristics
	   6 = Display drive information
	   7 = Proceed to next drive
.B2
.PP
Before we discuss what these options do, note that you must
.I not
alter the
.I size
of an \*(MD partition using any of the above options.
You must install \*(CO into
.I all
of an \*(MD partition,
or into any part of the drive that has space unallocated to \*(MD.
Changing the
.I size
of an \*(MD partition at this point, to make room for \*(CO,
will invalidate the \*(MD partition table, may corrupt \*(MD data, and
may make it unbootable from the hard drive.
If you do wish to change the
.I size
of an \*(MD partition, abort installation of \*(CO, back up all of your
\*(MD data, use the \*(MD version of
.B fdisk
to change the layout of your hard disk, restore your \*(MD data, and only
then install \*(CO.
.PP
The following describes each option in detail.
.RS
.IP \fB0.\fR 0.3i
Quit this phase of installation.
This option does not alter the disk's partition table in any way;
and it moves into the next phase of installation.
Use this option if you do not wish to alter your disk's partitioning
in any way.
.IP \fB1.\fR
The \fIactive partition\fR is the partition that the bootstrap
program reads by default.
When a partition is made the active partition, the operating system mounted
on that partition is booted automatically when you turn on your computer.
This option allows you to change the active partition,
or to designate \fIno\fP active partition,
in which case the computer will prompt you at boot time
for the partition number to boot.
You will need this option only if your hard disk has more than one logical
partition, and the partitions contain different operating systems.
Note that if later you wish to change the active partition, you can use the
\*(CO command \fBfdisk\fR to do so.
See the Lexicon entry on \fBfdisk\fR for details.
.IP \fB2.\fR
This option lets you change one logical partition \(em in effect, it lets
you select a logical partition for \*(CO.
You should use this option if your hard disk has more than one partition
and you wish to install \*(CO on only one of them.
The partition you select must hold at least ten megabytes.
Note that the contents of the partition will be deleted.
.IP \fB3.\fR
This reconfigures the entire disk.
You can reset the number of partitions, and the size of each.
.IP \fB4.\fR
Option 4 is self-explanatory.
.IP \fB5.\fR
This option allows you to change the drive parameters
associated with each drive on your system.
.IP \fB6.\fR
Give summary information about the disk.
.IP \fB7.\fR
This option will only appear if you have more than one hard disk drive.
Use this option to select the next hard disk drive.
.PP
If the owner of this hard disk wanted to organize her hard disk by
megabytes, all she would have to do is divide 1 million by 52,224 to find
that one megabyte is approximately equal to 20 cylinders; thus, to make
a ten-megabyte partition, he would assign it 200 cylinders.
The size of a cylinder may be different on your system, but the principle
is the same.
.PP
The next step depends on two factors:
the current organization of your hard disk, and the amount of space you
wish to give to \*(CO.
The limit for a \*(CO partition is 500 megabytes,
but up to several hundred megabytes is more realistic.
If your disk has only one logical partition, you must use option 3
to create at least one new logical partition.
If your disk already has more than one logical partition, you can
use option 2 to assign one to \*(CO
or use option 3 to assign more than one,
reserving the rest for your current operating system.
Of the partition(s) that you assign to \*(CO, one must hold at least
20 megabytes \(em you cannot use two ten-megabyte partitions;
thus, if no partition on your disk holds ten megabytes, you must use option 3.
.PP
How much space should you give \*(CO?
\*(CO is a multi-user, multi-tasking operating system; the more space
you assign to it on your disk, the more users and the more processes it
can support.
\*(CO, via UUCP and other communications programs, also gives you access to
information on other \*(CO and \*(UN systems throughout the world; you will
want to exchange mail with other users and possibly download news and
information.
All of this takes up space.
You must have one 20-megabyte partition to hold \*(CO's root file system
(that is, the file system that holds the files that make \*(CO go), and
you would be well advised to assign at least one more partition of at least
half that size to hold users' accounts and their files.
.PP
If you have a large disk drive that is organized into one partition
that you wish to assign to \*(CO, you are well advised to divide it
into two smaller partitions.
For example, if you wish to allocate 500 megabytes to \*(CO, you should
create two 250-megabyte partitions rather than one 500-megabyte partition.
In addition, if you anticipate wanting to perform a full restore
of a dumped root partition, you are well advised to have a spare
\*(CO partition in addition to the root partition.
An alternative strategy would be to boot from the \*(CO Boot floppy disk
and then restore your root partition.
This assumes that the device you dump and restore from is different
than your boot floppy!
.PP
The following two sub-sections describe what happen when you invoke options
2 or 3.
.Sh "Changing One Logical Partition"
.PP
You will first be asked which partition you wish to change.
Reply by entering the partition you want.
The system replies with the following text:
.B1
	Existing data on a partition will be lost if you change
	the base or the size of the partition.  Be sure you have
	backed up all data from any partition which you are going
	to change.
.sp \n(pDu
	You may specify partition bases in cylinders or in tracks.
	Do you want to specify bases in cylinders?
.B2
.PP
Reply \*(QlY\*(Qr, to use cylinders.
The system then asks:
.DM
	You may specify partition sizes in cylinders or in megabytes.
	Do you want to specify partition sizes in cylinders?
.DE
Reply \*(QlY\*(Qr, again to use cylinders.
Next, the system says whether the partition is initialized to another
operating system (that is, to Xenix, OS/2, or MS-DOS), or is unused.
It then asks you whether you wish to install \*(CO into the partition,
leave the partition unchanged, or mark the partition as unused.
For example:
.DM
	The current operating system type is MS-DOS.
	Do you want this to be a COHERENT partition?
.DE
.PP
Answer `Y' if you want this to be a \*(CO partition.
If you answer `N',
.B build
asks:
.DM
	Do you want the partition marked as unused?
.DE
.PP
Answer `Y' if you want it to be unused.
If you answer `N',
.B build
displays the following message:
.B1
	This program can mark a partition as a COHERENT partition
	or mark it as unused.  It CANNOT initialize a partition for
	use by any other operating system.  To do so, you must mark
	it as unused now and subsequently use the disk partitioning
	program provided by the other system to initialize it correctly.
	Do you still want to modify this partition?
.B2
.PP
As this message notes,
.B build
cannot install any operating system other than \*(CO into a partition.
You can, of course, leave the partition unchanged.
.PP
The final two questions ask you to enter the new base cylinder for the
partition and the size of the partition in cylinders.
Each question will prompt you with the current value for the partition.
Simply pressing \*(RT leaves this current value unchanged.
You could make the partition smaller, but this serves no
practical purpose if you intend to install \*(CO into this partition.
.PP
If you have made a mistake during this process, the system will prompt
you and ask you to correct it.
Otherwise, you will proceed to the next phase of installation, which is
to scan for bad blocks (described below).
.PP
We've said it before, but just to make sure we'll say it once again:
\fINever\fR use the \*(CO version of \fBfdisk\fR to change
the size of an \*(MD partition!
Use it to change the size of an unused partition, or of a partition that
contains an operating system other than \*(CO.
If you wish to change the size of an \*(MD partition, back up all data
in that partition, then use the \*(MD version of \fBfdisk\fR.
.Sh "Changing All Logical Partitions"
.PP
This process mirrors what occurs when only one partition is changed,
except that it is iterated for every existing partition.
If you have four partitions and wish to eliminate one, simply set its
size to zero.
If you have fewer than four partitions, you will be asked if you wish
to create any additional ones.
.PP
Note one additional feature:
the table that displays the layout of partitions (an example of which is
shown above) is redisplayed after every partition, showing the changes
you have made (if any).
By looking at the table, you will find it easy to keep straight just what
you have done \(em when you work with this table, you will see the value of
working in cylinders.
.PP
If you make a mistake, the system will prompt you to correct it.
A common error is requesting overlapping partitions \(em that is,
setting the base cylinder of a partition within an area already
allocated to another partition.
Another error would be to request an impossibly large partition.
.PP
It is strongly recommended that you \fInot\fP include the last
cylinder of your hard disk in any partition.
This cylinder is often used by diagnostic programs, and, as such,
is not available for general use.
.PP
Once you have made your selection of partition strategy,
.B build
displays the altered partition table and asks you if you wish to proceed
with partitioning.
Answer `Y' if you do, `N' if you do not.
.Sh "Scanning for Bad Blocks"
.PP
When a partition on an MFM, RLL, or IDE disk drive
is assigned to \*(CO, it must be scanned for
bad blocks.
(The terms \fIblock\fR and \fIsector\fR are often used
interchangeably.)
Most hard disks have at least a few blocks in which the disk's surface
is flawed and therefore cannot be trusted to hold data reliably.
.PP
\*(CO keeps a list of bad blocks for each partition, to ensure
that it does not write data into an unreliable area.
This checking is performed automatically, but takes a few minutes.
Patience is recommended.
.PP
Note that scanning is usually not required on SCSI interface devices.
However, some users have reported problems with bad blocks on SCSI drives
that use removeable media.
We recommend that you scan your SCSI devices, just to be sure, especially
if they use removeable media; but this is not required.
.Sh "Selecting the Root File System"
.PP
If your newly partitioned disk has more than one partition assigned
to \*(CO,
.B build
displays information about the \*(CO partitions and asks you to make
one the root file system.
The following displays is for a system with two hard-disk drives (one
IDE and one SCSI):
the IDE drive has two \*(CO partitions and the SCSI has four:
.B1
	Your sytem includes 6 COHERENT partitions:
	Drive  Partition    Device     Megabytes
	  0        0      /dev/at0a      31.01
	  0        1      /dev/at0b      75.20
	  1        4      /dev/sd0a      20.96
	  1        5      /dev/sd0b      62.91
	  1        6      /dev/sd0c      62.91
	  1        7      /dev/sd0d      60.82
.sp \n(pDu
	You must specify one COHERENT partition as the root filesystem.
	The root filesystem contains the fies normally used by COHERENT.
	The root filesystem should be at least 10 megabytes.
	COHERENT partition 1 is marked as active in the partition table.
	If you choose it as the root, you can boot COHERENT automatically.
.sp \n(pDu
	Which partition do you want to be the root filesystem?
.B2
.PP
Select the partition number that you want to make the root file system.
Note that you are not obliged to make the boot partition the root file system;
this means that when you wish to boot \*(CO, you must select the partition
to boot by hand.
In a moment, you will be shown how to do this.
.Sh "Creating COHERENT File Systems"
.PP
Once you have selected the root file system,
.B build
displays the following message:
.B1
	You must create an empty COHERENT filesystem on each COHERENT partition
	before you can use it.  Creating an empty filesystem will destroy all
	previously existing data on the partition.
.B2
.PP
You must create
file system for each of the partitions that you have assigned to \*(CO.
You are not obliged to create a new file system for each \*(CO partition.
For example, some users who already have \*(CO may prefer to install the
new version of \*(CO onto an empty partition, rather than use \*(CO's
.B update
utility.
If this is the case with you, be careful not to build a new file system
onto a partition whose files you wish to save; for if you do so, all
existing files will be wiped out.
.PP
If you have more than one \*(CO partition,
.B build
asks you about each in turn.
When it comes to the partition that you have marked as holding the root
file system,
.B build
displays the following:
.B1
	WARNING!!!
.sp \n(pDu
	The installation process expects a NEW file system in the root partition.
	If you are trying to update an existing COHERENT partition, you must run
	the COHERENT update.  If you are trying to install again after a partial
	or failed installation, a new root filesystem must be created again now.
.B2
.PP
The message is clear:
build a new file system onto the root partition.
.Sh "Copying Files"
.PP
The next step is to copy some key files from the boot floppy disk to your
hard disk.
.B build
displays the following message:
.B1
	The next step is to copy some COHERENT files from the diskette to the
	root filesystem of your hard disk.  This will take a few minutes...
.B2
.PP
.B build
will copy a set of files for running the operating system.
It will also copy a set of configuration files and devices, to match
the information that you have entered so far.
As the message states, this does take a few minutes.
.Sh "Mounting File Systems"
.PP
After it copies key files into the root partition, \*(CO asks you about
each \*(CO partition in turn:
.B1
	Do you want /dev/\fIdev\fP mounted [y or n]?
.B2
.PP
Answer `y' for each partition whose file system you want to mount
automatically when you boot your \*(CO system.
If you are a novice \*(CO user, you probably should mount all file systems.
For details on what it means to
.I mount
a file system, see the entry for
.B mount
in the Lexicon.
.Sh "Cohabitating with MS-DOS"
.PP
.B build
now asks if \*(CO will cohabitate with \*(MD on your system:
.B1
	Do you use both COHERENT and MS-DOS on your hard disk?
.B2
.PP
If you answer `y',
.B build
then guides the process of ensuring that \*(CO and \*(MD cohabitate
peacefully.
.Sh "Naming File Systems"
.PP
If you have assigned more than one partition to \*(CO,
.B build
will display the following prompt:
.DM
	Your system includes \fIN\fP partition[s] in addition to the root partition.
	These partitions are usually mounted on directories in the COHERENT
	root filesystem when the system goes into multiuser mode.
	For example, one non-root partition might be mounted on
	directory "/u", another on "/v", and so on.
	You now may specify where you want each partition mounted.
.B2
.PP
.B build
is asking you to name each file system.
The name is actually the name of the directory upon which
\*(CO mounts that partiton's the file system.
Historically, a file system's directory has been named using a single
letter from the end of the alphabet, such as \*(Qlv\*(Qr
or \*(Qlu\*(Qr.
There's no reason, however,
not to give the directory a descriptive name, such as
``work'' or ``data''.
Each name must be preceded by a slash \*(Ql/\*(Qr.
.PP
When you name a file system,
.B build
asks if you wish to mount that file system automatically
when \*(CO enters multi-user mode.
There is no reason not to mount a file system at that time, so you
should answer `Y'.
.PP
Finally, if you have an \*(MD partition on one of your hard disks,
.B build
will ask if you intend to use both operating systems.
If you answer `Y',
.B build
will then you to enter the number of the partition that holds the
\*(MD file system \(em from 0 through 3 (if you have one hard disk),
or from 0 through 7 (if you have two hard disks).
.B build
creates a device called
.BR /dev/dos .
You can use this device to read files from the
\*(MD partition, or write files onto it.
.Sh "Patch the Kernel"
.PP
.B build
now displays the message:
.B1
	The kernel on your hard drive will now be patched to run on your system.
.B2
.PP
This step patches the kernel that
.B build
has copied onto your hard disk, so that the kernel will be able to read your
hard disk (or disks) when
.B build
reboots your system.
.Sh "First Reboot"
.PP
Now that the keyboard has been selected,
partitions have been allocated, and file systems have been created,
the next step requires that \*(CO be booted from the hard disk.
This is the first of two reboots that the installation of \*(CO requires;
the other will come after you have described to
.B build
your configuration of serial and parallel ports, and other peripheral
devices.
.PP
If you have elected to use the \*(CO bootstrap, and if you have
the \*(CO boot partition the active partition, all you have to do is
remove the \fBBoot\fR disk from the floppy-disk drive when
prompted, and then reset your computer.
.PP
.B build
prints the following message:
.B1
	You have installed the COHERENT operating system onto your hard disk.
	To install files from the remaining diskettes in the installation kit,
	you must boot the COHERENT system from the hard disk.  It will prompt
	you to install the remaining diskettes in the installation kit.
.sp \n(pDu
	After you have finished reading this information, remove the floppy disk.
.B2
.PP
If you have assigned \*(CO to a partition other than the boot partition (that
is, the partition that boots automatically when you power up your system),
.B build
displays the following:
.B1
	If you type a partition number (0-7) while
	the boot procedure is trying to read the floppy disk,
	your system will boot the operating system on that partition.
	If you type nothing, your system will boot active partition \fIN\fP (\fIsystem\fP).
.sp \n(pDu
	Now remove the floppy disk so your system does nto boot from the floppy.
	You MUST type \fIN\fP when the system tried to read the floppy disk during the boot
	procedure to boot the partition containing the new COHERENT root filesystem.
	Preparing to halt system.  Hit <Enter> ...
.B2
.PP
In this case, press
.I N
on the row of number of keys on the main set of keys (\fInot\fR on the keypad)
when the floppy disk's light comes on while the system is booting.
This tells \*(CO's master boot routine to boot the operating system on
partition
.I N
instead of the operating system
.I OS
that is on the partition booted automatically.
After you install \*(CO, you can at any time change the partition that
the master bootstrap boots automatically.
For more information on how to do this, see the Lexicon entries for
.B booting
and
.BR fdisk .
.PP
Note that if you have made \*(CO the operating system you boot normally,
you can use this procedure to boot \*(MD or any other operating system
that's on your machine.
The section at the end of this chapter describes how to make \*(CO
cohabitate on your machine with \*(MD, OS/2, or Windows NT.
.Sh "Copying Files"
.PP
Rebooting will take a few moments.
.PP
Once \*(CO has rebooted itself from your hard disk,
.B build
copies the \*(CO files onto your hard disk.
The system will prompt you to insert the remaining disks that hold
the \*(CO files, one after the other.
If you are installing from 5.25-inch floppy disks, you will be prompted for
five additional disks; whereas
if you are installing from 3.5-inch floppy disks, you will prompted for
four addition disks.
The discrepancy is due to the fact that each 3.5-inch floppy disk can hold
more data, so fewer of them are needed.
.PP
The system will ask you whether you want the full set of manual pages
on line in uncompressed form, and whether you want the dictionaries used
by the \*(CO spelling checker also in uncompressed form.
These files must be uncompressed before they can be used, but take up
much more room on the disk.
.PP
We urge you to install the manual pages if at all possible; these contain
the latest version of all Lexicon, including all entries written or revised
after the \*(CO manual was printed.
However,
the dictionary is probably not of much use to you unless you intend to use
the commands
.B spell
and
.BR typo .
.Sh "Installation Continues"
.PP
After your system reboots, \*(CO invokes a script that continues the
installation process.
After it installing the remaining set of disks,
.B build
asks if you wish to add additional users to your system:
.B1
	Your COHERENT system initially allows logins by users "root" (superuser)
	and "bin" (system administrator).  In addition, the password file contains
	special entries for "remacc" (to control remote access, e.g. via modem),
	"daemon" (the spooler), "sys" (to access system information), and
	"uucp" (for communication with other COHERENT systems).
.sp \n(pDu
	If your system has multiple users or allows remote logins, you should assign
	a password to each user.
.sp \n(pDu
	Do you want to assign passwords to users?
.B2
.PP
You should assign a password to each of these special users.
Be sure you remember the password you give to the superuser
.BR root ,
because that user controls the entire system.
If you answer `Y',
.B build
asks you the following:
.B1
	You must enter each password twice.
	Do you want to assign a password for user "root"?
	Do you want to assign a remote access password?
	Do you want to assign a password for user "bin"?
	Do you want to assign a password for user "uucp"?
.B2
.PP
For each of the above users for whom you answered `Y',
.B build
prompts you for a password.
As we noted before, don't forget those passwords.
In particular, do not forget the password for the superuser
.BR root .
.PP
Next,
.B build
ask you to create logins for every other person on your system.
You should create a login for every person who will be using your system,
including yourself:
.B1
	You should create a login for each additional user of your system.
.sp \n(pDu
	Do you want to create another login?
.B2
.PP
If you indicate that you want to create one or more logins,
.B build
displays the following:
.B1
	You must specify a login name, a full name and a shell for each user.
	Joe Smith might have login name "joe" and full name "Joseph H. Smith."
	His home directory would be in "/usr" by default, namely "/usr/joe".
	Do not type quotation marks around the names you enter.
.sp \n(pDu
	Do you want home directories in "/usr"?
.B2
.PP
The first question asks you for the name of the directory system that will
hold your users' home directories.
Traditionally, these go into
.BR /usr ,
although you can put them into any file system that you created earlier.
.PP
Now, for each user,
.B build
asks the following questions:
.B1
	Login name:
.sp \n(pDu
	Full name:
.B2
.PP
These questions are self-explanatory.
.PP
.B build
then asks you for the shell that you want the user to use.
It explains the situation as follows:
.B1
	COHERENT includes three different command line interpreters, or shells.
	A command line interpreter is a program which reads and executes each
	command which the user types.  The available command line interpreters
	are the Bourne shell (/bin/sh), the Korn shell (/usr/bin/ksh) and the
	visual (full screen) shell (/usr/bin/vsh).
.sp \n(pDu
	Use the visual shell if you prefer a menu-driven interface.  Use the
	Bourne shell if you are not sure which shell to use.
.sp \n(pDu
	After you have finished installing COHERENT, you can change the shell
	for any user by editing the password file /etc/passwd.
.sp \n(pDu
	Do you want to user \fIuser\fP to use the Bourne shell (/bin/sh)?
.B2
.PP
.B build
then asks you in turn whether you wish to assign
.BR sh ,
.BR vsh ,
or
.BR ksh .
As the message says, if you do not know which shell to use, select
.BR sh .
.PP
Finally,
for each user
.B build
asks you if you wish to assign a password to that user.
If you say yes, it prompts you for the password.
.Sh "Describe Serial Ports"
.PP
.B build
now asks you about your serial ports and modems:
.DM
	Does your computer system have a modem?
.DE
.PP
If you answer `Y',
.B build
asks:
.B1
	You must specify which asychronous serial line your modem will use.
	See the article "asy" in the COHERENT documentation for details.
.sp \n(pDu
	Enter 1 to 4 for COM1 through COM4:
.B2
.PP
Enter the serial port into which you have plugged your modem, as shown above.
.PP
The next question concerns whether you wish to run a serial port in polled
or interrupt mode.
Under the architecture of the IBM PC, serial ports are linked in pairs:
port 1 is linked to port 3, and port 2 to port 4.
If you use both ports in a pair, only one can interrupt-driven; the other must
be polled.
Polling a port consumes much more in the way of system resources than having
the port interrupt driven.
The lesson is that if you have two devices plugged into serial ports on your
system, you should plug one into an even-numbered port and the other into an
odd-numbered one; in this way, both can interrupt-driven.
.PP
If you use both even-numbered or odd-numbered ports,
.B build
asks you which should be interrupt-driven, as follows:
.B1
	If your computer system uses both ports COM\fIx\fP and COM\fIy\fP,
	one must be run in polled mode rather than interrupt-driven
.sp \n(pDu
	Do you want to run COM\fIx\fP in polled mode?
.B2
.PP
where
.I x
and
.I y
are either 1 and 3, respectively, or 2 and 4.
Respond appropriate to the prompts.
When you have finished,
.B build
links the file
.B /dev/modem
to the port that has your modem plugged into it.
The command
.B ckermit
will use this by default.
For information on how to plug more than one modem into your system, see
the Lexicon article
.BR modem .
.PP
.B build
then asks if you wish to ``turn on'' your serial ports:
.B1
	Enable use of serial ports?
.B2
.PP
Unless you have a serious reason not to do so, you should answer `y'.
.PP
Finally,
.B build
asks you whether your printer is plugged into a serial port:
.B1
	The default printer on COHERENT is /dev/lp.
	This device may be linked to any valid parallel or serial port.
.sp \n(pDu
	Do you want /dev/lp to refer to COM1?
.B2
.PP
It asks you this question for each serial port until you either answer
`y' or until it runs out of ports.
If your printer is plugged into a parallel port, answer `n' to each prompt.
.B build
will configure parallel ports in a moment.
.Sh "Virtual Consoles, Part 1"
.PP
As noted earlier, \*(CO supports virtual consoles.
This means that you can have more than one console functioning at the same
time; the system will think that it is dealing with multiple separate console
devices.
You can flip from one console session to another by pressing a \fIhot key\fP;
or you can have two different physical devices \(em one monochrome and one
color, if you wish \(em and run multiple sessions on each.
This is useful if, for example, you wish to debug a program
by running it from one console session and sending the output to another.
You should decide how many virtual consoles you want:
from one to four per physical device.
.PP
At this point in installation, you will see the following prompt:
.B1
	This release of COHERENT supports multiple simultaneous sessions on the
	system console.  This feature, known as virtual consoles, supports both
	monochrome and color video cards (text mode only) with multiple "login"
	sessions on each, depending upon your hardware configuration.  If you have
	both color and monochrome adapters on your system, you can run multiple
	sessions on each of them at the same time.
.sp \n(pDu
	Virtual console support only works with systems which have at least one
	megabyte of usable system memory.  You may choose a loadable keyboard table
	or a non-loadable keyboard table.  Loadable keyboard tables will give you the
	flexibility of remapping your keys and to load new tables at any time while
	your system is active.  This requires keyboards that are 100% IBM AT compatible.
	If you chose a non-loadable keyboard you will not have this flexibility.
.sp \n(pDu
	Not all keyboards are 100% IBM AT compliant and will not work with loadable
	keyboard tables.  If your keyboard is not 100% IBM AT compliant, it may result
	in COHERENT not booting properly after the installation process is completed,
	or you may find that your keyboard keys are not mapped as they are labelled and
	that some keys cease to function at all.  For this reason, the default selection
	is the non-loadable keyboard.
.sp \n(pDu
	Do you wish to include support for virtual consoles?
.B2
.PP
If you answer `y',
.B build
will prompt you later for more information on your video card.
In either case, the next step is to select your keyboard.
.Sh "Selecting a Keyboard"
.PP
The next menu asks you to select a keyboard table from the following list:
.nf
.sp \n(pDu
	Belgian 102-key AT keyboard table
	Canadian French 102-key AT keyboard table
	Danish 102-key AT keyboard table
	Dutch 102-key AT keyboard table
	French 101-key standard AT keyboard (not loadable)
	French 102-key AT keyboard table
	German 102-key standard AT keyboard (not loadable)
	German 102-key AT keyboard table
	Italian 102-key AT keyboard table
	Latin American 102-key AT keyboard table
	Norwegian 102-key AT keyboard table
	Portuguese 102-key AT keyboard table
	Spanish 102-key AT keyboard table
	Swedish/Finnish 102-key AT keyboard table
	Swiss French 102-key AT keyboard table
	Swiss German 102-key AT keyboard table
	U.K. 102-key AT keyboard table
	U.S. 101-key standard AT keyboard (not loadable)
	U.S. 101-key AT keyboard table
.fi
.PP
The menu entries marked
.B "(not loadable)"
are the traditional \*(CO keyboard drivers, which are linked
directly into the kernel.
These include the traditional US, German, and French keyboards.
These traditional keyboard drivers work with all keyboards,
even the non-standard ones sold with some ``clones''.
These drivers cannot be used with virtual consoles,
and so will not appear on the list if you are using these consoles.
.PP
The other 16 or so menu entries represent loadable tables.
These use the
.B nkb
driver and thus allow you to customize the tables to your liking,
as well as load new tables at run time.
Please note, however, that the
.B nkb
driver (and loadable tables) only work with well-engineered
keyboards, such as those built by IBM, Cherry, MicroSwitch, 
NMB Technologies, or Keytronic;
when used with an non-standard ``clone'' keyboard, it may not work correctly.
.PP
When the menu appears, it highlights the line for the non-loadable U.S.
keyboard.
This table will satisfy the needs of all users in the United States, and
many users abroad.
To select this entry, simply press \*(RT:
\*(CO will do the rest.
.PP
If you want another keyboard, press the space bar
until the table you want is highlighted; then press \*(RT
to select the highlighted entry.
Note that the new keyboard layout takes effect as soon as \*(CO reboots
itself to read the rest of the installation disks.
.PP
If you choose one of the entries marked
.BR "not loadable" ,
you can use only that keyboard layout; the only way to select another
keyboard layout is to use the commands
.B cohtune
and
.B idmkcoh
to modify the kernel configuration and build a new kernel.
This sounds complex, but actually is rather easy;
for details, see the Lexicon entries for these commands.
.PP
If your keyboard's layout does not correspond to one of the
entries from the menu, select a loadable keyboard table
layout that resembles your keyboard.
After installation, you will be able to modify that
entry to make it correspond to your keyboard.
See Lexicon articles
.B "keyboard tables"
and
.B nkb
for further details on creating a new keyboard layout,
or modifying an existing one.
In addition,
the Mark Williams Company BBS system contains additional keyboard tables.
See the following sections of this manual for details on accessing
the MWC BBS system and on contacting satellite \*(CO BBS systems
around the world.
.Sh "Virtual Consoles, Part 2"
.PP
If you earlier decided to install virtual consoles,
.B build
asks you for information about the number and type of video cards that
you have in your system.
.PP
First, you will be asked:
.B1
	Does this system have a monochrome (i.e., MDA) video adapter
.B2
.PP
Answer `y' or `n'.
If you answer `y', you will then be asked:
.B1
	How many active virtual console sessions would you like on your
	monochrome video adapter card [1-8]?
.B2
.PP
Enter the number of sessions.
.PP
Next,
.B build
will ask:
.B1
	Does this system have a color (i.e., CGA/EGA/VGA) video adapter?
.B2
.PP
Answer `y' or `n'.
If you answer `y',
.B build
then asks:
.DM
	How many active virtual console sessions would you like on your
	color video adapter card [1-8]?
.DE
Enter the number of sessions you would like to have on this device.
.Sh "Installing Floating Point"
.PP
The next issue that
.B build
addresses is that of floating point:
.B1
	System is not equipped for floating point support.
	Floating point operations are used by WordPerfect, 123, and other programs.
	Enable software emulation of floating point?
.B2
.PP
.B build
asks this if your system does not have a mathematics co-processor.
(If your system does have a coprocessor, you will not see this prompt;
and \*(CO will use hardware floating point by default.)
Answer `Y' if you intend to run any software that requires that your system
have a mathematics co-processor or emulate one; these packages include
WordPerfect, Lotus 1-2-3, and also the GNU C compiler.
If you are not going to run any of these packages, you do not need floating
point emulation.
If you are not sure, you probably are better off to have it; it makes the
kernel somewhat larger, but otherwise does not affect the operation of
your system.
.Sh "Identifying the SCSI Devices"
.PP
If you have a SCSI controller, you earlier were asked to give a SCSI
identifier to each SCSI hard disk that you have plugged into that controller.
At this point,
.B build
asks you to confirm this:
.B1
	Hard drives are configured at the follow SCSI id numbers:
	\fInumbers\fP
	Is this correct?
.B2
.PP
If this is not correct, press `n'.
.B build
will walk you through entering the correct numbers.
.PP
If earlier you selected option 1 from the menu of possible SCSI devices
(that is, the Adaptec controller with one or more hard disks and tape),
.B build
now asks you about the SCSI identifier for the tape device:
.B1
	Tape drives are configured at the following SCSI id number:
	2
	Is this correct?
.B2
.PP
Practically every SCSI tape device is jumpered by default for SCSI identifier
2.
It `2' is the correct identifier, press `y';
.B build
will move on to the next topic.
However, if `2' is not the correct idenifier, press `n';
.B build
will then walk you through entering the correct number.
.Sh "Describe Parallel Ports"
.PP
Unlike the serial ports,
you do not have to tell the \*(CO kernel how it should read your system's
parallel ports.
However, as with the serial ports,
.B build
asks you if you have your printer plugged into a parallel port:
.B1
	The default printer on COHERENT is /dev/lp.
	This device may be linked to any valid parallel or serial port.
.sp \n(pDu
	Do you want /dev/lp to refer to LPT1?
.B2
.PP
.B build
asks you this for each parallel port, until either you answer `y' or
.B build
runs out of ports about which to inquire.
.PP
If you are not sure which port you have your printer plugged into, answer
`y' for LPT1.
The Lexicon entry for
.B printer
tells you how to find out the port into which your printer is plugged;
it also contains much useful information on how to configure your printer,
and the printer-related software that comes with \*(CO.
.PP
.B build
then displays the following information:
.B1
	Your BIOS identifies the following parallel printer ports:
	LPT1: \fIaddress\fP LPT2: \fIaddress\fP LPT3: \fIaddress\fP
	Enable use of parallel ports?
.B2
.PP
Press `y' if you want your \*(CO system to be able to use the parallel ports.
Press `n' if you do not, or if these addresses are bogus.
Note that if something is wrong (such as a wrong address being used),
you should be able to modify the kernel
after installation to correct the problem.
.Sh "Rebuilding the Kernel"
.PP
After
.B build
has asked you about your parallel ports, it builds a new kernel on your system.
This new kernel reflects the information you entered about your serial and
parallel ports, and about floating-point emulation.
.PP
You may see much incomprehensive stuff on your screen during this phase.
Do not worry about it \(em if
something goes serious wrong,
.B build
will tell you about it explicitly.
.Sh "Setting the Date and Time"
.PP
.B build
next sets the date and time for \*(CO.
Setting the date and time is vital to the correct operation of
\*(CO; however, \*(CO records the date and time quite differently
from the way \*(MD does.
.PP
Time under \*(CO is recorded as the number of seconds since January 1, 1970,
at exactly midnight.
Internally, \*(CO always stores time as Greenwich Mean Time.
GMT is used
to make it easy for \*(CO systems around the globe to coordinate time with
each other.
When \*(CO
time-stamps a file or displays the time, it converts Greenwich Mean Time to
your local time, depending on what time zone you live in and whether
Daylight Savings Time is in effect.
(For a detailed discussion of this topic, see the Lexicon's entry for
.BR TIMEZONE .)
.PP
.B build
first asks you if you wish to convert system time to daylight-savings time:
.B1
	You can run COHERENT with or without conversion for daylight savings time
	(summer time).  You should normally run with daylight savings time
	conversion.  However, if you are going to use both COHERENT and MS-DOS
	and you choose to run with daylight savings time conversion,
	your time will be wrong (by one hour) during daylight savings time
	while you are running under MS-DOS.
.sp \n(pDu
	Do you want COHERENT to use daylight savings time conversion?
.B2
.PP
Answer `Y' if you wish to use daylight-savings time, or `N' if you do not.
If you answer `Y',
.B build
then asks you about when daylight-savings time begins and ends:
.B1
	By default, COHERENT assumes daylight savings time begins on the
	first Sunday in April and ends on the last Sunday in October.
	If you want to change the defaults, edit the file "/etc/timezone"
	after you finish installing COHERENT.
.sp \n(pDu
	Is daylight savings time currently in effect?
.B2
.PP
The default settings are those enacted by law for the United States.
For information on how to reset the file \fB/etc/timezone\fR, see the
entry for
.B TIMEZONE
in the Lexicon.
.PP
.B build
then asks you to confirm your system's date and time:
.B1
	According to your system clock, your local date and time are:
	\fI[date and time]\fP
	Is this correct?
.B2
.PP
Answer `Y' if the date and time are correct, `N' if they are not.
If the date and time are not correct,
.B build
asks you to enter the current date and time, in the form \fIYYMMDDHHMM\fR.
.PP
If
.B build
cannot read or alter your system's clock (for whatever reason), it displays the
following message:
.B1
	The command which sets the internal real-time clock of your system is
	failing repeatedly.  Either you are entering the date and time incorrectly
	or your clock hardware is not completely AT-compatible.  If your clock
	hardware is incompatible, you can continue with the installation without
	setting the clock correctly.  However, if you do so, subsequent clock
	references (including file access and modification time information) will be
	incorrect and some commands (such as "date") will not function correctly.
.sp \n(pDu
	Do you want to proceed without setting the clock correctly?
.B2
.PP
If you answer `N',
.B build
aborts entering date and time information.
You should carefully examine your system's hardware, to ensure that the clock
is working correctly (that its battery, for example, is still charged).
Many of \*(CO's features do not work if it cannot read the system's clock.
.PP
Next,
.B build
displays the following menu and asks you what time zone you live in:
.B1
	Please choose one of the following timezones:
		0	Central European
		1	Greenwich
		2	Newfoundland
		3	Atlantic
		4	Eastern
		5	Central
		6	Mountain
		7	Pacific
		8	Yukon
		9	Alaska
		10	Bering
		11	Hawaii
		12	Other
.B2
.PP
If you select 0 through 11,
.B build
sets your local time automatically.
If you select ``Other'',
.B build
displays the following:
.B1
	You need to specify an abbreviation for your timezone,
	whether you are east or west of Greenwich, England,
	and the difference in minutes between your timezone
	and Greenwich Time (called UTC or GMT).  For example,
	Germany is 60 minutes of time east of Greenwich.
.sp \n(pDu
	Abbreviation for your timezone:
.sp \n(pDu
	Is your timezone east of Greenwich?
.sp \n(pDu
	Difference in minutes from GMT:
.sp \n(pDu
	Abbreviation for your daylight savings timezone:
.B2
.PP
If you are unclear on these concepts, consult the Lexicon article on
.BR TIMEZONE .
If you are unsure about how your local time relates to Greenwich time,
consult an atlas, check with your local library, or telephone a
local radio station.
.PP
.B build
then displays the corrected local time and asks if it is correct.
If not, you can repeat the process until the time is correct.
.Sh "System and Domain Names"
.PP
.B build
now asks you for the name of your system, and of your domain.
You must give your system a name if you wish to use electronic mail
on your system.
.PP
.B build
begins by displaying the following message and asking you for the name of
your system:
.B1
	In order to use COHERENT's electronic mail facility and UUCP subsystem,
	you must choose a "site name" for your computer system.  In general, a site
	name consists of lower case letters or digits and should be at most seven
	characters in length.  The name you choose should be unique if you intend
	to access any other computer systems.  Some of the more well known site
	names include "mwc", "uunet", "clout", "decwrl", "hp", "kgbvax", "prep",
	"seismo", and "ucbvax".
.sp \n(pDu
	Please enter the site name for this system:
.B2
.PP
See the Lexicon article
.B uucpname
for details on what sort of name you should give your system.
.PP
A
.I domain
is a group of systems that can be addressed collectively by other
computer systems.
Most domains are registered with the groups that administer e-mail collectives
nationally and have their addresses recorded.
Thus, by enrolling in a domain, computer users from around the
world will be able to exchange electronic mail with you.
For more information, see the Lexicon entry
.BR domain .
.PP
.B build
now asks you to enter your system's domain:
.B1
	The COHERENT mail subsystem supports "domain addressing" in addition to
	traditional "bang paths".  Until your system becomes part of a registered
	domain, you may use the UUCP pseudo-domain.  Domain names consist of groups
	of letters and digits separated by periods (dots).  Some of the more well
	known domains include "com", "edu", "gov", "org", "net", as well as domains
	covering a geographical area, such as the Chicago area "chi.il.us" domain.
	If you are not registered in a domain, or if you are uncertain about this
	question, simply press the <Enter> key to default to the UUCP pseudo-domain.
.sp \n(pDu
	Please enter the domain name for this system:
.B2
.PP
If you have arranged to have your system join a domain, enter its name.
If you have not, press \*(RT to select the default domain
.BR UUCP .
.PP
For more information on domains and electronic mail, see the tutorial for
UUCP, and the Lexicon entry for \fBmail\fR.
.Sh "Installing Taylor UUCP Files"
.PP
At this point,
.B build
installs the files used by the Taylor UUCP system.
This system performs remote, automated communication with other \*(CO and
\*(UN systems.
When this is finished, press \*(RT to continue with installation.
.\".Sh "Uncompress Manual Pages"
.\".PP
.\"The next issue that
.\".B build
.\"addresses is the installation of on-line manual pages:
.\".B1
.\"	The online COHERENT manual pages in /usr/man are shipped in compressed form.
.\"	The compressed manual pages use about 2000 blocks (1 MB) of disk space.
.\"	The uncompressed manual pages use about 5000 blocks (2.5 MB).
.\".B2
.\".PP
.\"If your system has less than 5,000 blocks (2.5 megabytes) free,
.\".B build
.\"will show you the following warning:
.\".B1
.\"	Your system has insufficient free space on the root filesystem to store
.\"	the uncompressed manual pages.  You may uncompress and un-tar the manual
.\"	pages at a later date if you free up at least 5000 blocks on your root
.\"	filesystem.
.\".B2
.\".PP
.\"If you do have sufficient space,
.\".B build
.\"gives you the following choices:
.\".B1
.\"	You can either:
.\"		1) Remove the manual pages,
.\"		2) Leave the manual pages compressed, or
.\"		3) Uncompress the manual pages.
.\"	The "man" command will work only if you uncompress the manual pages (option 3).
.\".B2
.\".PP
.\"If you have enough space to spare, you should install the manual pages.
.\"These give the most up-to-date version of the \*(CO Lexicon pages.
.Sh "Install the Dictionary"
.PP
Next,
.B build
asks if you want to uncompress the dictionary used by the command
.BR spell :
.B1
	The COHERENT dictionary in /usr/dict is shipped in compressed form.
	The compressed dictionary uses about 350 blocks (.175 MB) of disk space.
	The uncompressed dictionary uses about 1700 blocks (.85 MB).
	You can either:
		1) Remove the dictionary,
		2) Leave the dictionary compressed, or
		3) Uncompress the dictionary.
	The "spell" command will work only if you uncompress the dictionary (option 3).
.B2
.PP
You should uncompress the dictionary only if you wish to use
.B spell
to check your text documents.
.Sh "Install the Print Spooler"
.PP
\*(CO version 4.2 includes the MLP print spooler.
This spooler implements a version of the \fBlp\fR spooler used by \*(UN
System V.
To set up MLP correctly, you need to enter some information about
what printers you have plugged into your system, and the ports into which
they are plugged.
.B build
begins with the following message:
.B1
	COHERENT includes print spooling software which allows you to use
	multiple printers.  Earlier we asked you about your printer to determine
	which port it used.  Now we will complete printer spooler configuration.
.B2
.PP
.B build then asks:
.B1
	Is your printer a laser printer?
.B2
.PP
Answer `y' if it is, and `n' if it is not.
.PP
.B build
then asks:
.B1
	Is your printer a PostScript printer?
.B2
.PP
Again, answer `y' if it is, and `n' if it is not.
.PP
That's all there is to it.
.B build
will configure the spooler correctly, so you can begin to print as soon
as you have completed installation.
.PP
Note that
the above procedure supports only one printer on one port.
You can configure MLP to support an indefinite number of printers, plugged
into serial ports, parallel ports, or even into the auxiliary ports of
dumb terminals.
For details, see the Lexicon entries for
.B printer
and
.BR controls .
.Sh "Second Reboot"
.PP
At this point,
.B build
has completed its work.
It has copied \*(CO onto your hard disk, built file systems,
used information you entered to configure a kernel for you,
added users, and performed all of the other miscellaneous tasks
needed to establish a multi-user, multi-tasking operating system on
your machine.
.PP
The final task that
.B build
performs is to reboot your system a second time.
This invoke the kernel it built after the first time
it rebooted your system.
After the system has rebooted, you can log in and begin to use your new
system.
.PP
Before
.B build
reboots, it displays the following message:
.B1
	System configuration done, will take effect after reboot.
.sp \n(pDu
	You have completed the installation procedure successfully.
	Don't forget to remove the last diskette from the disk drive.
.sp \n(pDu
	You must reboot your system in order to allow the preceding
	configuration steps to take effect.
	Preparing to halt system.  Hit <Enter> ...
.B2
.PP
If your system does not immediately reboot after you see this message, press
the reset button.
.PP
Your newly installed \*(CO system will use the utility
.B fsck
to examine and (possibly) correct any minor flaws in your system's file systems.
It will then go into multi-user mode.
You can log in and begin to use \*(CO.
.PP
If you are a novice to \*(CO, we urge you to turn to the tutorial
.IR "Using the COHERENT System" ,
which is located near the beginning of your \*(CO manual.
This will help you become acquainted with \*(CO.
.PP
If you are experienced with \*(CO or \*(UN, you may wish simply to
see what comprises \*(CO
Invoke the command
.BR /etc/coh_intro .
This gives you a ``guided tour'' of \*(CO, so you can see what is in it.
The Lexicon entry
.B commands
also summarizes all commands that are in the \*(CO system.
.PP
This concludes the installation of your \*(CO system.
The next sections of this chapter describe how you can have \*(CO
co-habitate with other operating systems on your machine.
The final section describes some problems that can arise with installation
and suggests how you can deal with them.
.SH "Installing COHERENT With Other Operating Systems"
.PP
Many users wish to run \*(CO but keep \*(MD, OS/2, or Windows NT
on their machines as well.
The following describes how to do this.
.Sh "MS-DOS in One Partition; OS/2 in Another; Boot Manager Installed"
.PP
IBM's OS/2 Boot Manager is a program that resides in its own partition on
the hard drive.
If the OS/2 Boot Manager is installed, then that probably means
that \*(MD is the first partition, and that OS/2 was installed as a
.I logical
or
.I extended
partition, which can only be booted from Boot Manager.
With the first three partitions taken up by \*(MD, Boot Manager, and OS/2,
\*(CO can be installed only into the fourth partition.
To install \*(CO:
.IP \fB1.\fR 0.3i
If the whole drive is taken up, you must back up the \*(MD
partition, delete it, and re-create it
(minus the amount of space you wish to reserve for \*(CO).
.IP \fB2.\fR
Install \*(CO normally:
Place the boot disk into drive A and reboot.
.IP \fB3.\fR
When you see the question
.DM
	Would you like to install the Coherent Master Bootstrap?
.DE
answer `y'.
.IP \fB4.\fR
A little later,
.B build
invokes \*(CO's
.B fdisk
utility.
Choose the partition into which you will install \*(CO.
Choose as you would normally; however, before you leave
.BR fdisk ,
\fImake the \*(CO partition active.\fR
This bypasses the OS/2 Boot Manager program and boots \*(CO automatically.
\fIThis is essential to completing installation!\fR
.PP
To boot \*(MD or \*(CO, use the \*(CO Master Bootstrap to boot the
partitions normally:
upon turning on the computer,
press the number key that corresponds to the partition whose operating
system you wish to boot.
.PP
To start OS/2, boot the Boot Manager partition by pressing the number key
that corresponds to its partition;
and when Boot Manager starts, select OS/2.
.Sh "MS-DOS and OS/2 in the Same Partition"
.PP
OS/2 users can install OS/2 into the same partition as \*(MD.
To boot OS/2, users type a command that swaps the
\*(MD boot files with the OS/2 boot files.
Due to the location, placement, and
attributes of these boot files, only the \*(MD files and
directory structures can be backed up; therefore, your options are
limited to the following:
.IP \fB1.\fR 0.3i
Back up the \*(MD\(emOS/2 partition.
This is to preserve your data and directory structure.
.IP \fB2.\fR
Boot \*(MD and run the \*(MD version of
.BR fdisk .
.IP \fB3.\fR
Delete the \*(MD\(emOS/2 partition, then re-create it minus the amount of
space you wish to reserve for \*(CO.
After rebooting out of the \*(MD version of
.BR fdisk ,
reinstall \*(MD.
.IP \fB4.\fR
Install \*(CO normally.
.IP \fB5.\fR
After you have installed \*(CO,
reinstall OS/2 back onto the \*(MD partition.
To help restore the previous OS/2 setup, run OS/2's
.B migrate
program.
.PP
To boot \*(CO when you boot your computer,
press the number key that corresponds to the number of
its partition.
To boot either \*(MD or OS/2,
press the number key that corresponds to the number of their partition \(em
usually `0'.
.Sh "Windows NT and MS-DOS with COHERENT"
.PP
The following describes how to install \*(CO, \*(MD 6.0, and Windows NT onto
the same system.
.IP \fB1.\fR
First install \*(MD 6.0 into its own partition.
.IP \fB2.\fR
Install Windows NT into its own partition.
Use the Dual-Boot facility in
Windows NT to select either Windows NT or \*(MD to boot when powering up.
.IP \fB3.\fR
Install \*(CO into its own partition.
This can either be on the same drive as \*(MD and Windows NT, or, if your
system has two hard disks, onto its own disk.
Use the \*(CO Master Bootstrap to be the boot manager.
In this way, you can boot any partition on your system, and so easily
boot any of the three operating systems.
.PP
Because you will be working with three operating systems, you may wish to
use the \*(CO
.B fdisk
command to configure the master bootstrap, so that it does not boot any
partition by default.
In that way, you can select whatever partition \(em and therefore,
whatever operating system \(em you want at boot time.
If you want to boot either \*(MD or Windows NT, select the partition for
Windows NT; its Dualboot will take over and boot either Windows NT or
\*(MD, whichever you choose.
.Sh "COHERENT and MS-DOS 6.0"
.PP
Release 6.0 of \*(MD offers a feature of dynamic file compression that
creates some difficulties for machines that have both \*(CO and \*(MD on
their systems.
.PP
To begin, \*(MD 6.0 assumes that it is the only operating system on your
computer.
When you install \*(MD 6.0, by default it overwrites the COHERENT master boot
block.
If at all possible, you should install \*(MD 6.0
.IR first ,
and then install \*(CO to ensure that the \*(CO Master Bootstrap is in
control of system, so that you can boot either operating system.
.PP
Second, \*(MD 6.0 offers a compression utility called
.BR dblspace ,
which compresses \*(MD file systems on the fly.
\*(CO's
.B dos
commands do not understand compressed \*(MD file systems created by the
\*(MD 6.0 utility
.B dblspace ,
or by such programs as
.BR Stacker .
If you are running \*(MD 6.0 with file compression,
you must copy files to an uncompressed file system
(for example, to an uncompressed floppy disk or to the uncompressed
host for a compressed file system) to make them accessible to the \*(CO
.B dos
commands.
.SH "Problem With Installation"
.PP
As we mentioned earlier, tens of thousands of users have
successfully installed \*(CO onto their computers.
Most encountered no difficulty whatsoever during installation;
chances are you too will not encounter any.
.PP
If problems do arise, the Mark Williams support staff will gladly
help you.
Before you call, however, be sure to read the following sub-sections
and follow their directions.
Each describes a commonly encountered problem,
and gives advice on how to overcome it.
.PP
If your problem is not covered in this chapter \(em or if you have tried
what it suggests and it does not work \(em call Mark Williams.
Information about contacting Mark Williams Technical Support is given in
the introduction to this pamphlet.
.Sh "Did the system go multi-user prematurely?"
.PP
With some configurations of hardware,
.B build
does not initiate the second rebooting of the system; rather, it attempts
to go multiuser immediately, and displays a cascade of errors on the system
console.
.PP
This problem is due to the fact that kernel being used at that stage of
installation does not yet support the full suite of peripheral devices.
You need to reboot your system to invoke the kernel that was built during
the final stage of installation.
Just press the reset button on your system; this will reboot your computer
and invoke the newly built kernel.
Thereafter, all should be well.
.Sh "Printing does not work"
.PP
In some cases, attempts to print via the parallel ports would generate
messages of the form
.DM
	Cannot open /dev/lpt1
.DE
even though that device exists and has its permissions set correctly.
This is probably due to the fact that in a few rare instances,
.B build
program did not include the driver
.B lp
in the kernel that it built during the final stage of installation.
.PP
To get around this problem, use the command
.B idenable
to enable the driver
.BR lp ;
then use the command
.B idmkcoh
to build a new kernel.
Once a new kernel is built, reboot your system and all should be well.
For details, see the Lexicon entries for
.B idenable
and
.B idmkcoh
in these notes.
.Sh "Does your system hang instead of reboot?"
.PP
In rare instances, the \*(CO command
.B reboot
does not reboot the system; rather, it stops current processing but does
not initiate rebooting.
If this happens, just press the reset button on your system to reboot;
doing so will not harm your system in any way.
.Sh "Are you using incorrect chipset parameters?"
.PP
If your system ``hangs'' when it attempts to boot from the \*(CO
boot disk, your motherboard's CMOS setup parameters may be incorrect.
Many systems use chipsets that support user-configurable bus speeds
and timing.
On these systems, if you attempt to run the bus clock at too high a rate,
certain peripherals will not function correctly.
For example,
on systems using the OPTI chipset and the AMI BIOS, you should verify
that the system's ``bus speed'' is set to
.B CLKIN/4
rather than
.B CLKIN/3
or
.BR CLKIN/2 .
In addition, if parameter ``AT STRETCH'' is available, it should be
set to
.BR ENABLED .
If the aforementioned parameters are not set correctly,
you will experience incorrect system behavior.
.Sh "Are you booting COHERENT Disk 1 via <ctrl-alt-del> (warm boot)?"
.PP
It may help to cold boot your system:
that is, turn power off, wait at least 30 seconds, then turn power back on.
Rebooting via cold boot is more resistant to
getting a virus from something you have already loaded, and avoids
the family of bugs in the AMI BIOS, \*(MD, and Windows that relate
to warm boots.
.Sh "Do you have a defective RAM chip?"
.PP
We have seen cases in which computers go through normal startup and
even run \*(MD without difficulty, but give panic errors or stack
overflow errors randomly when running in protected mode.
Weak memory chips and some defective or poorly designed
motherboards can be found by
running a thorough, protected-mode diagnostic program on memory,
then replacing all defective parts that it discovers.
.Sh "Are you running in Turbo mode?"
.PP
Sometimes customers, especially those experiencing panic traps, can
install when they change processor speed.
Although the improvement, when it occurs, usually happens when the CPU
is slowed down, we have seen one case where it helped to speed up the processor.
In general, panics that vanish or significantly change their pattern of
appearance when CPU speed is changed, strongly suggest that the problem
is due to a poorly designed or incompatible motherboard (see below).
.Sh "Will it help to change the master boot procedure?"
.PP
If you are having trouble selecting the partition from which
your computer boots,
it may help to modify the master boot sequence, as follows.
(1) Use the command
.B /etc/fdisk
to select \fIno\fR active partition, or,
during startup, hold down the key corresponding to the root \*(CO
partition (over the alpha keyboard, \fInot\fR on the numeric keypad) until
startup halts with a keyboard error.
(2) Press \fB<F1>\fR to continue.
Boot should progress to \*(CO.
.Sh "Is CMOS configured correctly for drive A?"
.PP
Some customers who ordered the wrong size of floppy disks
switched floppy-disk drives A and B,
but failed to update the setup for the computer.
This often produces an error message of the form
.DM
	fsminit:  no root dev (4,14)
.DE
The solution is to configure the CMOS correctly.
See your computer's technical documentation for details on how to do this.
.Sh "Do you have an incompatible hard-drive controller?"
.PP
If you have a Western Digital controller model WD1006V-SR2 and it is a
Feature 3 board, you may need to replace it.
Some of these controllers have defects that cause them to latch up.
.PP
If you have a Western Digital controller model WD1006V-SR2 and it is a
Feature 5 board (Feature 5 boards have all the
jumper headers installed), install
the ``non-latched'' mode jumper, which is the first one (J1).
.PP
In any case, please contact Mark Williams Technical Support
with the make and model of the controller.
It may lead us to a new pattern that we can accommodate.
Some customers have Perstor, IDE, ESDI, or SCSI controllers
and did not know it until they checked the controller type;
there are known compatibility issues
with some controllers of all the above types.
.Sh "Do you have incompatible hard-drive parameters?"
.PP
If you were
.I unable
to install \*(CO,
check the contents of \fBCONFIG.SYS\fR from the \*(MD
root directory on drive C.
If you find one of the following \fBDEVICE=\fR entries there
that configures the hard drive,
it may be an indication that your hard disk layout is non-standard.
However, many users of the following device drivers have successfully
installed \*(CO, so please be sure that you have attempted installation
\fIprior\fR to contacting Mark Williams Technical Support.
.DS
	DMDRVR.BIN
	EDVR.SYS
	SSTOR.SYS
	HARDRIVE.SYS
	FIXT_DRV.SYS
.DE
Before you call,
note your hard-drive's make, model number and
parameters (i.e., number of heads, cylinders,
and sectors per track) from the documentation supplied with the hard drive.
Most drives are configured with 17 sectors per track when used with
MFM encoding, 26 sectors per track with RLL encoding, and 31 sectors per
track with Perstor controllers.
Check these parameters against what is in the CMOS setup:
CMOS specifies parameters by storing the drive type number, which
specifies an entry in the BIOS disk parameter table.
If there is a setup program in the BIOS, it may give drive parameters
explicitly;
or, it may be necessary to look up the entry in the manual for the BIOS.
.PP
If the number of cylinders is greater than 1,024, you may have an incompatible
drive:
the format of the standard PC partition table does not allow more than
1,024 cylinders.
However, some disk controllers and most IDE drives
support a ``translation mode'' that allows them to
operate with BIOS parameters that do not match the actual parameters of
the disk drive (i.e., physical drive geometry).
For example, the following table lists the physical and translation mode
parameters for the Western Digital WD93044-A IDE drive:
.DS
.ta 0.5i 1.5i 2.5i
		\fIPhysical	Translation\fR
	Cylinders:	782	977
	Heads:	4	5
	Sectors:	27	17
	Landing Zone:	862	977
.DE
The Adaptec 2372 and Western Digital 1006V-SR2 series controllers
simulate the presence of valid drive parameters when the drive type is set
to \fB1\fR in the system setup.
With these controllers, it is possible to install \*(CO when the drive
parameters do not correspond to any drive type in the BIOS, but you must
be sure to set the drive type to \*(QL1\*(QR in the CMOS setup.
.Sh "Do you have an incompatible video board?"
.PP
If you are using a Zenith Z449 series CGA/VGA video adapter and are
experiencing panic traps, there is a known defect in this board.
Contact Zenith Data Systems, or try a different video board.
.PP
Some (unlabeled) monochrome boards do not work in protected mode.
Symptoms include immediate panic or loss of video when booting from disk 1.
If possible, try exchanging video boards.
.Sh "Do you have an incompatible keyboard?"
.PP
If you are experiencing problems with respect to key mappings,
and you installed one of the loadable keyboard mapping tables
from the keyboard selection menu, you may have an incompatible keyboard.
Please note that the \*(CO
.B nkb
driver (and loadable tables) only work with standard
keyboards, such as those built by IBM, Cherry, MicroSwitch,
NMB Technologies, or Keytronics;
when used with a poorly engineered ``clone'' keyboard,
it may not work correctly.
.PP
The preferred action is to replace the non-standard
keyboard with one made by one of the above-named manufacturers.
If, however, you wish to use a non-standard keyboard with \*(CO,
your best bet is to re-install \*(CO and select the
.B kb
(non-loadable) driver instead of
.BR nkb .
.B kb
is not loadable and supports only the U.S., German, and French
keyboard layouts.
.PP
If you do not wish to replace your keyboard or re-install \*(CO,
then the following
.I may
help you cope with your keyboard's problems.
.IP \fB1.\fR 0.3i
If the shift or control key seems to ``stick'' in the
.B on
or
.B down
position, you can try to fix the keyboard mapping table that
corresponds to your country.
For example, if you selected the U.S. keyboard mapping table
during installation but find that the right shift
or control key seems to stay down after you press it for the first time,
edit file
.BR /conf/kbd/us.c .
To fix the right shift key, change the line that reads:
.DM
	{K_57,rshift,rshift,rshift,rshift,rshift,rshift,rshift,rshift,rshift, S|MB },
.DE
to
.DM
	{K_57,none, none, none, none, none, none, none, none, none, O|MB },
.DE
To fix the right control key, change the line that reads
.DM
	{K_64, rctrl, rctrl, rctrl, rctrl, rctrl, rctrl, rctrl, rctrl, rctrl, S|MB },
.DE
to
.DM
	{K_64,none, none, none, none, none, none, none, none, none, O|MB },
.DE
This makes these keys inoperative.
This, of course, is less than desirable,
but it will at least prevent you from accidently
forcing your keyboard into a shift or control state from which you cannot exit.
.IP
Once you have modified the keyboard table, rebuild the keyboard executable that
corresponds to the modified table, as described in Lexicon articles for
.B nkb
and
.BR "keyboard tables" .
Thereafter, you can enter the name of the 
newly created executable and it will load the modified keyboard table into
the keyboard driver.
This also happens automatically at boot time.
.IP \fB2.\fR
If one or two keys are transposed, have no output, or output the incorrect
value, you must (a) figure out which key location is incorrect,
(b) decide what the correct value should be for all key states, (c) edit
the file in directory
.B /conf/kbd
that corresponds to your selection,
(d) rebuild the keyboard table executable (as described in the Lexicon articles
.B nkb
and
.BR "keyboard tables" )
and (e) reload the keyboard table by invoking the newly created executable.
.PP
As noted above, the easiest solution is to replace the keyboard.
.Sh "Do you have a system incompatible with the MWC master boot program?"
.PP
The Mark Williams master boot program is known \fInot\fR to work with
certain Zenith 241- and 248-series computers using Zenith's BIOS.
It should be possible to install and run \*(CO as long as the master boot
program is \fInot\fR used.
.Sh "Do you have an incompatible clock device?"
.PP
If the install process fails when trying to set the system time and date
(\fBATclock\fR failure), please contact Mark Williams Technical Support.
.PP
It is common to see an
.B ATclock
failure when a computer has less than one megabyte of RAM.
If your system has less than one megabyte of RAM, you must upgrade your memory.
.Sh "Is your system extremely slow?"
.PP
If you are running \*(CO on a SCSI disk drive and your system
is running very slowly,
odds are that your host adapter is not correctly configured.
Some non-\*(CO systems ``busy wait'' the host adapter
when performing SCSI I/O.
Because \*(CO is a multi-tasking, multi-user system,
it uses interrupts to signal when the host adapter
has completed an I/O operation and is ready for the next request.
If your host adapter is not correctly configured, it may not
be asserting the correct interrupt, or it may not be asserting any interrupt.
Please check the jumpers on your host adapter to verify that
the correct interrupt is being asserted.
If you have further questions, contact the hardware manufacturer.
.Sh "Is the setup of your system incorrect?"
.PP
.B build
will occasionally warm boot during initialization
if your system's BIOS has an incorrect notion of your system's configuration
of hardware.
One symptom of this problem is if your system prompts you to run a setup
program when you cold-boot your system.
.PP
To fix this problem, run the setup program that comes with your system.
Make sure that your system's clock setting and configuration of hardware
is correct.
\fIBe extremely careful not to change your hard disk's partitioning!\fR
.Sh "Are you running Extradrive?"
.PP
The disk-doubling utility Extradrive
writes material into sector 0 on your hard disk, where \*(CO writes its
master boot program.
If you have trouble booting \*(CO, try removing Extradrive, then
re-install \*(CO.
.PP
Note that for \*(MD, Extradrive, and \*(CO to work together on the same
machine, you must first install \*(MD, then configure Extradrive, and
finally install Extradrive.
If you do not follow this order, \*(MD or Extradrive will overwrite the
\*(CO master boot because each assumes that \*(MD is the only operating
system residing on the computer.
.Sh "Is your video card incompatible?"
.PP
Some makes of video card are incompatible with \*(CO's virtual consoles.
In this instance, the console may exhibit bizarre behavior, such as moving
the cursor around the screen but not printing any text.
This problem can also arise if you told \fBbuild\fR that you have a
monochrome card when in fact you have a color card, or vice versa.
.PP
Check the make of your video card and make sure you described it correctly to
.BR build .
Daredevil \*(CO hackers may wish to log in via a serial port and try to
change the configuration of the console driver; the Lexicon article
\fBconsole\fR gives information on this driver.
For most users, however, the only solution is to re-install \*(CO and
not install virtual consoles.
.Sh "Does your system use wait states?"
.PP
One user succeeded in installing \*(CO on his system by changing memory
to one wait state.
Normally, \*(CO should work correctly with zero wait states, but introducing
wait states may be necessary on some systems.
If all else fails, you may wish to try this.
.Sh "Do you have the Opti chip set?"
.PP
Some users have received panic dumps on clone PC built around
the Opti chip set.
One helpful tactic is to
use the setup routine of the AMI BIOS to disable the keyboard reset control.
.Sh "Do you have a notebook computer?"
.PP
During installation,
some notebook computers display a message like the following:
.DM
	Panic:  Parity error:  cs=FFC00018  ip=FFC0038E
.DE
This problem rests in the computer's power management.
For example, the Siemens notebook computer
has three features to increase the lifetime of the battery:
.IP \fB1.\fR 0.3i
It switches the hard disk off after a predetermined time has passed with no
disk access.
This technique works with \*(CO, but in multiuser mode the disk very seldom
shuts down because of disk accesses from sync and other running daemons.
It might happen that the time to spin the disk on to normal speed is too
long and \*(CO reports a bad track.
The timeout can be patched in the kernel to avoid this problem.
.IP \fB2.\fR
It switches off the backlight for the LCD after a predetermine time has
some time without keyboard input.
This technique works without complication under \*(CO.
.IP \fB3.\fR
It decreases clock frequency for the CPU and the refresh rate for the memory
after a predetermine time of inactivity.
This strategy, unfortunately, creates problems with \*(CO.
The power management hardware generates a non-maskable interrupt and
requests service from the BIOS.
\*(CO, however, switches the CPU into protected mode and the BIOS
no longer is available for such a service.
The only know reason for COHERENT to receive a non-maskable interrupt is a
memory parity error.
.PP
Fortunately this feature can be switched off in the BIOS setup of the
Siemens notebook computer.
If the BIOS of your notebook allows you to switch off this feature, do so.
If it does not, try setting the inactivity time in the BIOS to an extremely
Note that this problem should not arise with \*(CO 4.2.
.Sh "Do you have an incompatible system?"
.PP
If you have problems that cannot be helped by any of the above, 
then your computer system may not be capable of running \*(CO.
Note that it is possible for a system that cannot run \*(CO to be
made up entirely of components known to work on other systems that
run \*(CO.
The greatest number of these cases is seen with customers running
an AMI BIOS, IDE hard drives, or any kind of DTK motherboard.
Please record as much information as possible about make and model of
motherboard, BIOS, hard drive controller, hard drive, and all other equipment
installed; then contact Mark Williams Technical Support.
