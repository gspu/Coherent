.ds TL "Installation"
.ds TI "RELEASE NOTES"
.NH "Installing COHERENT"
.PP
The following describes how to install \*(CO onto your computer.
Installation of \*(CO is straightforward, and Mark Williams Company
has prepared a suite of programs that automate much of the work for you.
.PP
Tens of thousands of users have successfully installed \*(CO onto their
computers.
Most have found the process to be easy and quick.
An uncomplicated installation will take approximately 30 minutes.
.PP
A few users have experienced some problems with installation;
so if installation does not go as smoothly as you would like, the end of this
section describes the commoner problems and how you can fix them.
If all else fails, telephone Mark Williams Technical Support, as described
in the introduction to this booklet.
.SH "Installing Vs. Updating"
.PP
Before you continue any further, please note that this section is for
users who are not yet running \*(CO on their computers.
If you already are running \*(CO and are updating to \*(CO 4.2,
\fRskip\fI this section and instead read the following section,
.IR "Updating COHERENT" .
If you re-install \*(CO over an existing \*(CO system, all \*(CO
files you already have will be wiped out!
.PP
We do not mean to frighten you.
However, we do know that you may well have spent a great deal of customizing
your \*(CO system; and we would hate to see all that work lost due to a
simple mistake.
So, we'll say it once again:
\fIIf you already are running \*(CO, you want to \fBupdate\fI \*(CO \(em
not \fBinstall\fI it.
Skip this section and instead
read the following section, on updating \*(CO!\fR
.SH "Before You Install"
.PP
As you have read this far, we assume that you are \fInot\fR now running
\*(CO, and that you do wish to \fIinstall\fR \*(CO onto your system.
Before you begin installation, you must make a few decisions about how
you want to configure your computer; and you must collect some information
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
20 megabytes of space free on it.
More is recommended, but 20 megabytes is the minimum space required
to install \*(CO.
You need at least 45 megabytes if you also wish to install X Windows for \*(CO.
.IP
If your system does not have a free partition, you must either take one away
from \*(MD, or you must split an existing DOS partition into two and
give one to \*(CO.
In either case, all data on that partition will be destroyed.
How to do this is described below.
.IP \fB2.\fR
Your system must be based upon the Intel 80386 or 80486 processor,
or an exact clone.
.IP \fB3.\fR
Your system must have at least four megabytes of available RAM.
If you wish to run X Windows for \*(CO, you must have at least eight
megabytes; more is recommended.
.\".IP
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
If you are installing \*(CO onto a notebook or other
computer system that uses a modified 83-key layout rather than the
industry standard 101-key or 102-key layout, you may experience
key transpositions and other anomalies.
If you are using the keyboard driver that uses loadable tables,
read Lexicon article
.B vtnkb
for information on how to modify the keyboard-mapping tables for your system.
.IP \fB6.\fR
Your system must have at least one high-density, 3.5-inch or
5.25-inch floppy-disk drive installed as drive A.
The distribution disks for \*(CO cannot be read by a low-density
floppy-disk drive, nor can they be converted from one size of disk to
the other.
.IP
Due to limitations in the BIOS of the IBM PC and its clones,
\*(CO can be installed \fIonly\fR from drive A.
Thus, if your system has both sizes of floppy-disk drive, your release of
\*(CO must be for the disk drive that your system knows as drive A.
.PP
If your system meets these criteria, then you probably can install and
run \*(CO, as tens of thousands of other users have done.
Check the list of compatible and incompatible hardware in the second
section of this booklet to see if any of your hardware is listed.
If your system does not appear on either list, you probably can still install
and run \*(CO \(em but Mark Williams Company will be interested in your
experience.
.PP
The following sections discuss your preparations in more detail.
.Sh "Hard-Disk Partitioning"
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
up to two AT-style (IDE, RLL, MFM, or ESDI) disks, and up to eight SCSI disks.
You can have either one AT controller on your system, or one SCSI
controller, or one AT controller plus one SCSI controller.
.PP
The point of the installation procedure is to create
one or more partitions on a hard disk to hold \*(CO and its files.
When you (or your dealer) installed \*(MD on your computer, you (or he)
divided your computer's hard disk (or disks) into \fIpartitions\fR.
Each hard disk can has four partitions.
Partitions are numbered sequentially, beginning with partition 0:
If you have one hard disk, the partitions are numbered 0 through 3;
if you have two hard disks, the partitions on the first hard disk are numbered
0 through 3, the partitions on the second are numbered 4 through 7; and so on.
.PP
Although a hard disk has four partitions, you can modify the amount
of disk space that each partition holds.
Each disk has a written on it a
.IR "partition table" ,
which records where each partition begins and where it ends \(em and
thus, how much space each partition holds.
A partition can even be set to have zero space, thus making it unused.
Further, special rules govern how you must configure very large IDE
drives (that is, IDE drives that have more than 1,024 cylinders).
These rules are discussed below.
Both \*(CO and \*(MD come with a program named
.BR fdisk ,
which lets you edit a disk's partition table to reset the beginning and
end points of each disk's partitions.
Later in this chapter, we will discuss how you can use \*(CO's version of 
to change the dimensions of a partition on your disk, should you wish to.
.PP
Please note that not every partition must be
used \(em a hard disk can be divided into four partitions,
but have \*(MD file systems in only two of them,
with the third and fourth partitions being idle.
Note, too, that the four partitions do not necessarily have to
encompass the entire hard disk \(em a disk may have space that is outside
any partition and so remains unused.
.PP
The file systems for \*(CO and \*(MD are very different, so it is not
possible to have both systems use the same partition \(em each
must have one or more partitions completely to itself.
(Note, however, that \*(CO comes with tools that let you copy files
between an \*(MD file system \*(CO.)
.Sh "MS-DOS Parititions on the Hard Disk"
.PP
Another factor that installation must cope with is how many partitions
have you already given to \*(MD.
If you have one hard disk, you may have either one or two \*(MD partitions.
The first partition is the \*(MD primary partition, and
the second (if there is one) is the \*(MD extended partition.
\*(MD lets you sub-divide the extended partition into logical drives D, E, F,
and so on.
.PP
\*(CO
.I cannot
be installed onto a logical drive.
In this scenario, you have these options:
.IP \fB1.\fR 0.3i
Install \*(CO onto
.I all
of the \*(MD extended partition.
You will lose drives D, E, F, etc., and
.I all
data in them.
.IP \fB2.\fR
Use the \*(MD version of the command
.B fdisk
to delete an existing partition, then make it smaller
by however much disk space you wish to allocate to \*(CO.
Remember that \*(CO requires at least 20 megabytes;
while \*(CO plus X requires at least 45 megabytes.
This step frees disk space from \*(MD.
\*(CO will see this freed space and allow you to turn this into a \*(CO
partition (or partitions).
.IP \fB3.\fR
If your hard drive has space that lies outside of any existing partition,
\*(CO will let you turn it into a \*(CO partition.
.PP
These options apply even if you already have \*(MD plus any other operating
system (e.g., OS/2 or Windows NT) on your system.
.PP
It may well be that you do not know the configuration of your hard disk
(or disks).
\*(CO's installation program will figure this out automatically, then
displayed this information to you at the appropriate point in installation.
.Sh "Hard-Disk Controller"
.PP
Another variable is the type of hard-disk controller you have.
\*(CO works with AT-style (IDE, RLL, MFM, and most ESDI) hard-disk controllers.
It also works with many SCSI host adapters.
Please check the previous section in this booklet
for a list of supported hard\-disk controllers and host adapters.
.PP
If you intend to install \*(CO onto a system that uses one of the
supported SCSI host adapters, be sure that your host adapter is jumpered
to enable interrupts.
Make sure that you know the interrupt number and other configuration
information before you begin to install \*(CO.
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
If you have an Adaptec host adapter, whether your device's BIOS uses
a 64-head translation mode.
This is the default.
.IP \fB4.\fR
The IRQ the host adapter uses.
The default is 11 for Adaptec, five for Seagate or Future Domain devices.
.IP \fB5.\fR
The hexadecimal host adapter base-port address.
The default is 0x330 for Adaptec, 0xCA00 for Seagate or Future Domain devices.
.IP \fB6.\fR
The DMA channel the host adapter uses.
The default is five.
.IP \fB7.\fR
For each SCSI device you have, you must assign a SCSI ID, zero through seven.
If you have only one SCSI device, the default is zero.
.PP
If for some reason you cannot find out this information for your device,
don't panic.
The default value for each of these parameters is carefully chosen,
and in all probability will work with your device.
.Sh "Hard Disk Summary"
.PP
To summarize, when you install \*(CO, you must cope with the following
variables:
.IP \(bu 0.3i
The number of disk controllers on your system.
.IP \(bu
The type of each controller.
.IP \(bu
The number of disks plugged into each controller.
.IP \(bu
The size of each disk (with special rules for IDE drives that have more
than 1,024 cylinders).
.IP \(bu
The size of each partition on each partition.
.IP \(bu
The number partitions you have already given to \*(MD, and the size
of each.
.PP
Installation thus will follow any of a number of possible scenarios,
depending on how your system is organized, how much space you've given
to \*(MD, and how much disk space you wish to give to \*(CO.
However, \*(CO's installation program will walk you through these decisions,
to make this process as easy as possible.
.PP
On a side note,
we recommend that you keep a copy, on paper, of your computer's partitioning.
This information can be vital in helping you to recover from a catastrophic
error with your hard disk.
.PP
For an overview of how \*(CO handles hard disks, see the article entitled
.B hard disk"
in the \*(CO manual's Lexicon.
.Sh "Peripheral Devices"
.PP
You should note the port into which you have plugged each your printers and
modems.
If you have a SCSI tape device, note the SCSI identifier that it uses.
The installation program will ask you for this information so it can
configure your system correctly.
.PP
The installation program will also ask you if you wish to have one or more
of your serial ports in polled mode.
For information on what this means, see the Lexicon article on
.BR asy .
.PP
You will also be asked about what console devices you have on your system.
Currently, the \*(CO install procedure lets you install up to a total of
eight virtual-console sessions on each of two physical consoles.
Note the number of video cards plugged into your system, and whether the
cards are monochrome or color.
.Sh "System Name"
.PP
When you install \*(CO you will be asked to name your system.
You must do this to use electronic mail under \*(CO;
this will permit other systems to identify your system.
You should pick a name that is easy to remember, yet probably unique.
No name should be longer than seven characters.
.PP
See the Lexicon article
.B uucpname
for details on system names, and examples.
.Sh "Domain Name"
.PP
Finally, when you install \*(CO you will be asked to name your system's
electronic mail domain.
If you will be adding your system to a \*(UU domain, you should enter its
name; otherwise, you should select the default domain name \fBUUCP\fP.
If you join a \*(UU domain later, or change domains, you can name that
domain simply by editing the file \fB/etc/domain\fR.
.PP
See the Lexicon article
.B domain
for details on domains.
.Sh "Information Form"
.PP
The following page gives a form that summarizes the information you need
before you install \*(CO onto your system.
We suggest that you fill it out, then refer to it as you install \*(CO
onto your system.
This will help you ensure the accuracy and completeness of the information
you enter during installation, and help spare you from experiencing that
``sinking feeling'' one gets when one realizes half-way through installation
that you've forgetten something important.
.bp
.B1
.so install.form
.sp
.B2
.bp
.SH "Installing COHERENT"
.PP
Now that the preliminaries are over, we can begin to install \*(CO
onto your system.
This assumes that you have done the following:
.IP \(bu 0.3i
Filled out the form on the preceding page.
.IP \(bu
Backed up your disk.
.IP \(bu
If necessary, used \*(MD's version of
.B fdisk
to free up enough space for \*(CO.
.PP
You can abort the installation procedure at any time by typing
.BR <ctrl-C> .
However, aborting installation does \fInot\fR mean that
your hard disk will be returned
to the state it was in before installation was begun.
\fIWhen a disk is repartitioned, the files that were on any
modified partitions are gone forever!\fR
.PP
During the installation, \*(CO explains what it is doing.
It asks your permission before it executes any action
that could destroy data on your disk.
However, read the following directions carefully before you begin,
so you know
.I exactly
what is going on before you begin.
Please read them through before you begin.
.Sh "Beginning Installation"
.PP
To begin installation, turn your system off.
Take the \fBBoot\fR disk from your installation
kit (i.e., disk 1) and insert it into floppy-disk drive A.
(The disk
.I must
be placed into drive A.
Installation will not work if you put it into drive B.)
.PP
Now, turn your system on.
Your system reads the boot disk and boots a mini-version of \*(CO from it.
In a moment, you see a question mark `?', which is a prompt.
Type:
.DM
	begin
.DE
and press the \*(RT key.
This loads \fBbuild\fR, the \*(CO installation program, into memory and
begins execution.
As \*(CO loads
.BR build ,
it displays a spinning character for your amusement.
.PP
When
.B build
is loaded into memory and begins execution, it displays a copyright notice:
.B1
.sp 2
                              The COHERENT System
.sp
                Copyright (c) 1982, 1995 by Mark Williams Company
.sp
                     60 Revere Drive, Northbrook, IL  60062
.sp
                        708-291-6700, 708-291-6750 (FAX)
.sp 2
.B2
.PP
Press \*(RT.
You will see the following greeting:
.B1
.sp
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
.sp
.B2
.PP
As the instructions say, you can interrupt installation at any point by
typing \fB<ctrl-C>\fR.
.Sh "Enter the Serial Number"
.PP
.B begin
asks you to enter a nine-digit serial number:
.B1
.sp
	A card included with your distribution gives the serial number
	of your copy of COHERENT.
.sp \n(pDu
	Type in the serial number from the card:
.sp
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
We now enter the most important phase of installation:
describing and configuring your hard disks.
.PP
.B build
begins this phase by asking you how many hard-disk controllers you have,
and what their types are:
.B1
.sp
	Most PC compatible computer systems use MFM, RLL, IDE, or ESDI disk
	controllers and disk drives.  Others use SCSI disk drives.
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
.sp
.B2
.PP
Select the appropriate entry:
1, 2, or 3.
As the above message implies, you can have one AT-type controller, one
SCSI controller, or one of each type.
You cannot have more than one of either type of controller.
.Sh "Installing an AT Disk"
.PP
If your system has an AT-compatible hard-drive controller
(i.e., IDE, RLL, MFM, or ESDI controller),
.B build
will ask you about polling:
.B1
.sp
	Most AT-compatible controllers work with NORMAL polling.
.sp \n(pDu
	Some IDE hard drives require ALTERNATE polling.
.sp \n(pDu
	If you get "<Watchdog Timeout>" or "at0:TO" errors with normal polling,
	use alternate polling.
.sp \n(pDu
	Use NORMAL polling ? [y]
.sp
.B2
.PP
Throughout installation, the default response to a prompt appears in
square brackets after the prompt.
To select the default, simply press \*(RT.
In this, press \*(RT to select the default response, `Y' (for ``yes'').
To use alternate polling, press `N'; however, you should not
do so unless you are absolutely certain that normal polling does not work.
.PP
.B build
then asks for the number of AT-type hard-disk drives you have:
.B1
.sp
	How many IDE hard drives are on your system ? (1-2) [1]
.sp
.B2
.PP
To select the default \(em in this case, one hard drive \(em press \*(RT.
Press `2' if you have two AT-style hard-disk drives.
Note that you can have no more than two AT-type hard drives
plugged into your one AT-type controller.
.PP
Finally,
.B build
asks:
.B1
.sp
	Are you installing on an IBM PS1 or ValuePoint ? [n]
.sp
.B2
.PP
Press \*(RT to select the default, which is `N'.
Press `Y' if you are installing onto a PS/1 or ValuePoint machine.
Also answer `Y' if you are installing onto an IBM ThinkPad machine.
.B build
will ask you a few additional questions that apply only to those machines.
.PP
At this point,
.B build
writes some information to reflect what you have entered so far.
You may see some vaguely comprehensible text on your screen.
Do not be alarmed.
.Sh "Installing a SCSI Hard Disk"
.PP
If you indicated that your system has one or more SCSI hard disks,
.B build
at this point asks for information about them.
If you do not have a SCSI hard disk, but you do have a SCSI controller
with other peripherals (e.g., tape drive), you will be asked about
these at a later point in installation.
.PP
First, it asks what type of SCSI controller you have:
.B1
.sp
	COHERENT currently supports the following SCSI host adapters:
.sp \n(pDu
	(1) Adaptec AHA-154x series
	(2) Seagate ST01 or ST02
	(3) Future Domain TMC-845/850/860/875/885
	(4) Future Domain TMC-840/841/880/881
.sp \n(pDu
	Your choice ? (1-4)
.sp
.B2
.PP
.B build
then walks you through a dialogue to gather information.
If you do not know what your disk uses, or if you do not understand
what the question is asking, select the default \(em
it nearly always is correct.
.PP
A SCSI controller can have up to eight peripheral devices plugged into it.
Each peripheral device (hard disk, CD-ROM, or tape device) that is plugged
into a SCSI controller has its own unique SCSI ID number.
Some devices set you set this by setting a dial;
others require that you set a jumper.
At this point in installation,
.B build
asks you which SCSI identifiers you have assigned to the hard disk or disks
that are plugged into your SCSI controller:
.B1
.sp
	Hard disk drives are configures at the following SCSI IDs
	0
	Is this correct ? [y]
.sp
.B2
.PP
It is customary to assign SCSI ID hard disks beginning with ID zero;
however, you are not required to do so.
.B build
displays one default number for each SCSI hard disk.
If the default is correct, press \*(RT.
If it is not, press `N';
.B build
will then prompt you to enter the SCSI ID number (or numbers) for each
hard disk that is plugged into your SCSI controller.
.PP
The next four questions display information that is specific to the
host adapter.
The prompts will vary, depending upon which host adapter you are using.
.PP
The first of the host-adapter-specific questions
asks you to name the I/O port for your host adapter.
Each family of supported host adapters has its own range of ports
and defaults; for example, if you have an Adaptec AHA-154x controller,
the prompt will appear as follows:
.B1
.sp
	The AHA-154x can use IO port:
	0x130, 0x134, 0x230, 0x234, 0x330, 0x334
.sp \n(pDu
	Your choice: ? (0x100-0xfff) [0x330]
.sp
.B2
.PP
If you are not sure of the correct port for your controller,
select the default:
in most instances, it will be correct.
.PP
The next question asks you to enter the interrupt channel.
Again, each family of controllers will have its own channel; if you are
using an Adaptec AHA-154x controller, the prompt appears as follows:
.B1
.sp
	The AHA-154x can use Interrupt Channel: 9, 10, 11, 12, 14, 15.
.sp \n(pDu
	Your choice: ? (2-15) [11]
.sp
.B2
.PP
Again, if you do not know which channel to use, select the default; it
probably is correct.
.PP
The third question asks you to enter the host adapter's DMA channel.
If you have an Adaptec AHA-154x controller, the prompt appears as follows:
.B1
.sp
	The AHA-154x can use DMA channel: 0, 5, 6, 7.
.sp \n(pDu
	Your choice: ? (0-7) [5]
.sp
.B2
.PP
Again, select the default if you are not sure which channel to use.
.PP
The last host-adapter-question question asks you to enter the SCSI ID for the
host adapter itself.
If you have an Adaptec AHA-154x controller, the prompt appears as follows:
.B1
.sp
	The AHA-154x host adapter may have SCSI ID 0 through 7.
.sp \n(pDu
	Your choice: ? (0-7) [7]
.sp
.B2
.PP
As you would expect, you should select the default if you are not sure
which SCSI ID to use.
.PP
At this point,
.B build
records the information you have entered.
You will again see some barely comprehensible text on the screen.
This is normal; do not be alarmed.
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
.sp
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
.sp
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
.sp
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
	Do you want to use the COHERENT master boot [y or n] ?
.sp
.B2
.PP
As explained in the prompt text, a \fIbootstrap\fR is a program that
pulls an operating system into memory and sets it to running \(em the
name relates to the fact that the operating system \*(QLpulls itself up
by its boot straps\*(QR.
The \*(CO master bootstrap can boot \*(CO as well as many other
operating systems, including \*(MD, OS/2, and Windows NT.
.PP
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
.Sh "Partitioning the Hard Disk"
.PP
.B build
then moves into the next phase:
selecting one or more disk partitions for \*(CO.
As described above, partitioning can vary greatly from disk to disk;
how the disk is partitioned will determine how much space is allocated
to \*(CO and how much to other systems.
This is the trickiest part of installation, so be sure to read carefully.
.Sh "How a Disk Is Organized"
.PP
This phase begins by displaying the current layout of your hard disk:
the number of partitions, the size of each partition, and the operating
system that ``owns'' each partition.
Note that the partition table is displayed in \fIphysical\fP order (i.e., the
actual order in which the partition table entries occur).
Some operating systems display the table in \fIlogical\fP or sorted order.
.PP
The following gives the printout for a typical hard disk.
This hard disk, called disk \fB0\fR,
has approximately 33 megabytes on it organized into
two \*(MD partitions, as follows:
.B1
.sp
	Drive 0 Currently has the following logical partitions:
	              [In Cylinders] [  In Tracks  ]
	Number  Type  Start End Size Start  End Size Mbyte Blocks Name
	0 Boot MS-DOS    0  149  150     0  899  900  7.83  15300 /dev/at0a
	1     EXT-DOS  150  614  464   900 3684 2784 24.28  47430 /dev/at0b
	2      UNUSED    0    0    0     0    0    0     0      0 /dev/at0c
	3      UNUSED    0    0    0     0    0    0     0      0 /dev/at0d
.sp
.B2
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
hard-disk drive actually contains many hard disks, or \fIplatters\fR,
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
of the amount of data we can write into a partition;
but when you organize your disk,
it is much easier to think in terms of cylinders.
It is simple to translate cylinders into megabytes, which gives you
the best of both worlds; this will be discussed below.
.Sh "Partitioning the Disk"
.PP
Now that we have discussed partitioning in theory, here's how
.B build
walks you through this process.
.PP
.B build
begins by displaying the following message:
.B1
.sp
	This program will let you change partition information for each disk drive.
	A disk drive can be divided into one to four logical partitions.
	You can change the active partition (the partition which your
	system boots by default) or change the layout of logical partitions.
	Other programs which change hard disk partition information
	may list logical partitions in a different order.
	Hit <Esc><Enter> to return to the main menu at any time.
	Now hit <Enter>.
.sp
.B2
.PP
.B build
then displays a summary of information about the first hard-disk drive
on your system (what \*(CO calls drive 0).
The following gives an example of this summary:
.B1
.sp
	According to your computer system:
	Drive 0 has 615 cylinders, 6 heads, and 17 sectors per track.
	It contains:
		615 cylinders of 52224 bytes each,
		3690 tracks of 8704 bytes each,
		62730 sectors of 512 bytes each
	or a total of 32117760 bytes (32.12 megabytes).
	Do you think the above values are correct [y or n] ?
.sp
.B2
.PP
If you this this information is correct, press `Y'; otherwise press `N'.
Your confirmation shows that
.B build
is reading information from your system correctly.
.PP
.B build
then displays the configuration of the hard disk, followed by a menu of options:
.B1
.sp
	Drive 0 Currently has the following logical partitions:
	              [In Cylinders] [  In Tracks  ]
	Number  Type  Start End Size Start  End Size Mbyte Blocks Name
	0 Boot MS-DOS    0  149  150     0  899  900  7.83  15300 /dev/at0a
	1     EXT-DOS  150  614  464   900 3684 2784 24.28  47430 /dev/at0b
	2      UNUSED    0    0    0     0    0    0     0      0 /dev/at0c
	3      UNUSED    0    0    0     0    0    0     0      0 /dev/at0d
.sp \n(pDu
	Possible actions:
	   0 = Quit	
	   1 = Change active partition (or make no partition active)
	   2 = Change one logical partition
	   3 = Change all logical partitions
	   4 = Delete one logical partition
	   5 = Change drive characteristics
	   6 = Display drive information
	   7 = Proceed to next drive
.sp
.B2
.PP
Before we discuss what these options do, note that you must
.I not
use any of the above options to alter the size of an \*(MD partition.
You can either install \*(CO into
.I all
of an \*(MD partition, or into any part of the drive
that is not allocated to \*(MD.
If you use
.B build
to change the size of an \*(MD partition to make room for \*(CO,
will (1) invalidate the \*(MD partition table, (2) corrupt your \*(MD data,
and (3) make \*(MD unbootable from the hard drive.
If you wish to change the
size of an \*(MD partition, abort installation of \*(CO, back up all of your
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
The \fIactive partition\fR is the partition whose operating system
the \*(CO bootstrap program boots by default when you turn on your computer.
This option allows you to change which partition is the active partition.
.IP
You can also say that you system has
.I no
active partition.
If you do so, the computer will prompt you at boot time
for the partition to boot.
Note that if later you wish to change the active partition, you can use the
\*(CO command \fBfdisk\fR to do so.
See the Lexicon entry on \fBfdisk\fR for details.
.IP \fB2.\fR
This option lets you change one logical partition \(em in effect, it lets
you select a logical partition for \*(CO.
You should use this option if your hard disk has more than one partition
and you wish to install \*(CO onto only one of them.
The partition you select must hold at least 15 megabytes.
Note that the contents of the partition will be deleted.
.IP \fB3.\fR
This reconfigures the entire disk.
You can reset the number of partitions, and the size of each.
.IP \fB4.\fR
This options lets you delete an partition.
.IP \fB5.\fR
This option lets you to change the drive parameters
associated with each drive on your system.
.IP \fB6.\fR
Give summary information about the disk.
.IP \fB7.\fR
This option appears only if you have more than one hard disk drive.
Use this option to select the next hard disk drive.
.PP
If the owner of this hard disk wanted to organize her hard disk by
megabytes, all she would have to do is divide 1 million by 52,224 to find
that one megabyte is approximately equal to 20 cylinders; thus, to make
a 15-megabyte partition, she would assign it 300 cylinders.
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
If your disk already has more than one logical partition,
use option 2 to assign one to \*(CO
or use option 3 to assign more than one,
reserving the rest for your current operating system.
Of the partition(s) that you assign to \*(CO, one must hold at least
20 megabytes \(em you cannot use two ten-megabyte partitions;
thus, if no partition on your disk holds 20 megabytes, you must use option 3.
.PP
How much space should you give \*(CO?
\*(CO is a multi-user, multi-tasking operating system:
the more space you assign to it on your disk,
the more users and the more processes it can support.
\*(CO, via \*(UU and other communications programs, also gives you access to
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
An alternative strategy would be to boot from the \*(CO boot floppy disk
and then restore your root partition.
This assumes that the device you dump and restore from is different
than your boot floppy!
.PP
We will discuss options 2 and 3 in a moment.
First, however, we must discuss a special situation in partitioning
a disk \(em that is, partitioning an extended IDE disk.
.Sh "Large IDE Drives"
.PP
Earlier, we mentioned that special rules govern how \*(CO manages large
IDE drives.
.PP
The design of the IBM PC limits to 1,024 the number of cylinders that an
AT-style hard disk can have.
This puts practical limitations on how large a hard disk can be;
however, some IDE drives perform use some special routines so they
can address more than 1,024 cylinders.
.PP
Beginning with release 4.2.14, \*(CO supports this extended addressing on
an IDE drive; however, there is one restriction on this support:
\fIthe root partition for \*(CO must lie entirely within the disk's first
1,024 cylinders.\fR
If you have such an extended IDE hard disk, bear this rule in mind as you
partition your disk and assign a partition to \*(CO for its root file system.
If you do so, all will be well.
.PP
The following two sub-sections describe what happen when you invoke options
2 or 3 from the
.B fdisk
menu.
.Sh "Changing One Logical Partition"
.PP
You will first be asked which partition you wish to change.
Reply by entering the partition you want.
.B build
replies with the following text:
.B1
.sp
	Existing data on a partition will be lost if you change
	the base or the size of the partition.  Be sure you have
	backed up all data from any partition which you are going
	to change.
.sp \n(pDu
	You may specify partition bases in cylinders or in tracks.
	Do you want to specify bases in cylinders?
.sp
.B2
.PP
Reply \*(QlY\*(Qr, to use cylinders.
.B build
then asks:
.B1
.sp
	You may specify partition sizes in cylinders or in megabytes.
	Do you want to specify partition sizes in cylinders?
.sp
.B2
.PP
Reply \*(QlY\*(Qr, again to use cylinders.
Next, the system says whether the partition is initialized to another
operating system (that is, to Xenix, OS/2, or MS-DOS), or is unused.
It then asks you whether you wish to install \*(CO into the partition,
leave the partition unchanged, or mark the partition as unused.
For example:
.B1
.sp
	The current operating system type is MS-DOS.
	Do you want this to be a COHERENT partition?
.sp
.B2
.PP
Answer `Y' if you want this to be a \*(CO partition.
If you answer `N',
.B build
asks:
.B1
.sp
	Do you want the partition marked as unused?
.sp
.B2
.PP
Answer `Y' if you want it to be unused.
If you answer `N',
.B build
displays the following message:
.B1
.sp
	This program can mark a partition as a COHERENT partition
	or mark it as unused.  It CANNOT initialize a partition for
	use by any other operating system.  To do so, you must mark
	it as unused now and subsequently use the disk partitioning
	program provided by the other system to initialize it correctly.
	Do you still want to modify this partition?
.sp
.B2
.PP
As this message notes,
.B build
cannot install any operating system other than \*(CO into a partition.
You can, of course, leave the partition unchanged.
.PP
The final two questions ask you to enter the new base cylinder for the
partition and the size of the partition in cylinders.
Each question prompts you with the current value for the partition.
Simply pressing \*(RT leaves this current value unchanged.
You could make the partition smaller, but this serves no
practical purpose if you intend to install \*(CO into this partition.
.PP
If you have made a mistake during this process,
.B build
asks you to correct it.
Otherwise, you will proceed to the next phase of installation, which is
to scan for bad blocks (described below).
.PP
We've said it before, but just to make sure we'll say it once again:
\fINever\fR use the \*(CO version of \fBfdisk\fR to change
the size of an \*(MD partition!
Use it to change the size of an unused partition,
or of partitions that contain \*(CO.
If you wish to change the size of an \*(MD partition, back up all data
in that partition, then use the \*(MD version of \fBfdisk\fR.
.Sh "Changing All Logical Partitions"
.PP
This option is identical to the one that changes only one partition, except that
.B build
repeats it for every partition.
If you have four partitions and wish to eliminate one,
just set its size to zero.
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
Another common error is to request an impossibly large partition.
.PP
We strongly recommend that you
.I not
include the last cylinder of your hard disk in any partition.
This cylinder is often used by diagnostic programs, and, as such,
is not available for general use.
.PP
Once you have made your selection of partition strategy,
.B build
displays the altered partition table and asks you if you wish to proceed
with partitioning.
Answer `Y' if you do, `N' if you do not.
.Sh "Selecting the Root File System"
.PP
At this point, you have finished describing how your disk drive (or drives)
is partitioned.
.PP
If your newly partitioned disk has more than one partition assigned
to \*(CO,
.B build
displays information about the \*(CO partitions and asks which you want
to hold the root file system.
The following display is for a system with two hard-disk drives (one
IDE and one SCSI);
the IDE drive has two \*(CO partitions and the SCSI has four:
.B1
.sp
	Your system includes 6 COHERENT partitions:
	Drive  Partition    Device     Megabytes
	  0        0      /dev/at0a      31.01
	  0        1      /dev/at0b      75.20
	  1        4      /dev/sd0a      20.96
	  1        5      /dev/sd0b      62.91
	  1        6      /dev/sd0c      62.91
	  1        7      /dev/sd0d      60.82
.sp \n(pDu
	You must specify one COHERENT partition as the root filesystem.
	The root filesystem contains the files normally used by COHERENT.
	The root filesystem should be at least 20 megabytes.
.sp \n(pDu
	Which partition do you want to be the root filesystem?
.sp
.B2
.PP
Select the number of the partition you want to hold the root file system.
You must boot \*(CO from the partition that holds the root file system;
however, you are not obliged to make that partition the active partition.
If you do not make the root partition the active partition,
you must select that partition by hand when you wish to boot \*(CO.
This is easy to do, and in a moment we'll show you how to do it.
.Sh "Scanning for Bad Blocks"
.PP
Next, you must decide whether to scan each \*(CO partition for bad blocks.
.PP
\*(CO keeps a list of bad blocks for each partition, to ensure
that it does not write data into an unreliable area.
(NB, the terms \fIblock\fR and \fIsector\fR are often used interchangeably.)
Most hard disks have at least a few blocks in which the disk's surface
is flawed and therefore cannot be trusted to hold data reliably.
.PP
.B build
asks you the following question for each partition that you have set aside
for \*(CO:
.B1
.sp
	The next step in installation is to scan each COHERENT partition on the
	drives for bad blocks.  This will not write to the partition being scanned.
	Be patient.  This will take a few minutes.
.sp \n(pDu
	Do you wish to scan /dev/\fIdevice\fP for bad blocks [y/n] ?
.sp
.B2
.PP
Answer `Y' if you wish to examine the partition for bad blocks;
answer `N' if you do not.
Scanning is not usually performed for SCSI devices; however, some users
with removable SCSI media have encountered problems with bad blocks,
regardless of the promises of their devices' manufacturers.
.PP
Scanning a partition takes a few minutes, so patience is recommended.
.Sh "Creating COHERENT File Systems"
.PP
Once you have selected the root file system and (and, optionally,
scanned the selected partitions for bad blocks),
.B build
asks you if you wish to create a new \*(CO file system for each
partition that you have assigned to \*(CO.
.PP
.B build
asks you the following question for each partition that you have
assigned to \*(CO:
.B1
.sp
	Do you want to create a new COHERENT filesystem on partition \fIn\fP [y or n]?
.sp
.B2
.PP
Answer `Y' if you wish to build a new file system; otherwise, answer `N'.
Note that if, for some reason, you are re-installing \*(CO onto a system
that already has existing \*(CO partitions, answering `Y' for a given
partition will wipe out all files that may exist on that partition.
You are obliged to create a new file system only for the root partition.
You do not have to do so for other, non-root \*(CO partitions.
.PP
When it comes to the partition that you have marked as holding the root
file system,
.B build
displays the following message:
.B1
.sp
	WARNING!!!
.sp \n(pDu
	The installation process expects a NEW file system in the root partition.
	If you are trying to update an existing COHERENT partition, you must run
	the COHERENT update.  If you are trying to install again after a partial
	or failed installation, a new root filesystem must be created again now.
.sp
.B2
.PP
The message is clear:
you
.I must
create a new file system upon the root partition.
.PP
Creating a file system takes a few minutes.
Patience is recommended.
.Sh "Copying Files"
.PP
The next step is to copy some key files from the boot floppy disk to your
hard disk.
.B build
displays the following message:
.B1
.sp
	The next step is to copy some COHERENT files from the diskette to the
	root filesystem of your hard disk.  This will take a few minutes...
.sp
.B2
.PP
.B build
copies a set of files it needs to run the operating system.
As the message states, this takes a few minutes.
.Sh "Mounting File Systems"
.PP
After it copies key files into the root partition, \*(CO asks you about
each \*(CO partition in turn:
.B1
.sp
	Do you want /dev/\fIdev\fP mounted [y or n]?
.sp
.B2
.PP
Answer `Y' for each partition whose file system you want to mount
automatically when you boot your \*(CO system.
If you are a novice \*(CO user, you probably should mount all file systems.
.PP
Please note that even if you answer `N' to this question, you can change
your mind later on, and either mount a partition by hand (using the command
.BR mount ),
or change a configuration file so that the partition is mounted automatically.
For details on what it means to
.I mount
a file system, see the entry for
.B mount
in the \*(CO manual's Lexicon.
.Sh "Cohabitating with MS-DOS"
.PP
.B build
now asks if \*(CO will cohabitate with \*(MD on your system:
.B1
.sp
	Do you use both COHERENT and MS-DOS on your hard disk [y or n]?
.sp
.B2
.PP
If you answer `Y',
.B build
then guides you through
the process of ensuring that \*(CO and \*(MD cohabitate peacefully.
.Sh "Naming File Systems"
.PP
If you have chosen to mount more than one \*(CO partition,
.B build
now displays the following prompt:
.DM
	Your system includes \fIN\fP partition[s] in addition to the root partition.
	These partitions are usually mounted on directories in the COHERENT
	root filesystem when the system goes into multiuser mode.
	For example, one non-root partition might be mounted on
	directory "/u", another on "/v", and so on.
	You now may specify where you want each partition mounted.
.sp
.B2
.PP
.B build
is asking you to name each file system.
The name is actually the name of the directory upon which
\*(CO mounts that partition's the file system.
Historically, a file system's directory is named using a single
letter from the end of the alphabet, such as `v' or `u'.
There's no reason, however, not to give the directory a descriptive name,
such as ``work'' or ``data''.
Each name must be preceded by a slash `/'.
.Sh "Default MS-DOS Partition"
.PP
If you have an \*(MD partition on one of your hard disks,
.B build
asks if you intend to use both operating systems.
If you answer `Y',
.B build
then asks you to enter the number of the partition that holds the
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
.sp
	The kernel on your hard drive will now be patched to run on your system.
.sp
.B2
.PP
This step patches the kernel that
.B build
has copied onto your hard disk, so that the kernel will be able to read your
hard disk (or disks) when
.B build
reboots your system.
.PP
You will see a mass of obscure text on your screen.
Do not be alarmed.
.Sh "First Reboot"
.PP
Now that you have allocated a partition on your hard disk to \*(CO,
created a file system on it, and copied some key files onto it,
the next step requires that you boot \*(CO from the hard disk.
This is the first of two reboots that the installation of \*(CO requires;
the other will come after you have described to
.B build
your configuration of serial and parallel ports, and other peripheral devices.
.PP
.B build
prints the following message:
.B1
.sp
	You have installed the COHERENT operating system onto your hard disk.
	To install files from the remaining diskettes in the installation kit,
	you must boot the COHERENT system from the hard disk.  It will prompt
	you to install the remaining diskettes in the installation kit.
.sp \n(pDu
	After you have finished reading this information, remove the floppy disk.
.sp
.B2
.PP
As the prompt says, remove the boot disk from your floppy-disk drive.
.PP
If you have assigned \*(CO to a partition other than the boot partition \(em or
if you chose not to make any partition the boot partition \(em
.B build
displays the following:
.B1
.sp
	If you type a partition number (0-7) while
	the boot procedure is trying to read the floppy disk,
	your system will boot the operating system on that partition.
	If you type nothing, your system will boot active partition \fIN\fP (\fIsystem\fP).
.sp \n(pDu
	Now remove the floppy disk so your system does not boot from the floppy.
	You MUST type \fIN\fP when the system tried to read the floppy disk during the boot
	procedure to boot the partition containing the new COHERENT root filesystem.
	Preparing to reboot system.  Hit <Enter> ...
.sp
.B2
.PP
where
.I N
is the number of the partition that holds your root file system.
In this case, press
.I N
on the row of number of keys on the main set of keys (\fInot\fR on the keypad)
when the floppy disk's light comes on while the system is booting.
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
It prompts you to insert the remaining disks that hold
the \*(CO files, one after the other.
.PP
This process will take a few minutes.
Patience is recommended.
.Sh "Confirming Configuration of AT Controller"
.PP
Now that you have booted \*(CO from your hard disk and copied the rest of
the release disks onto your system,
.B build
resumes the job of configuring your system.
.PP
.B build
begins this phase of installation by asking you to confirm how you
have described your system's hard-disk controller (or controllers).
.PP
If your system has an AT-style hard-disk controller,
.B build
displays the following message:
.B1
.sp
	AT-type hard drive (IDE, RLL, MFM, or compatible).
.sp \n(pDu
	Device IS enabled.
	Do you want this device enabled ? [y]
.sp
.B2
.PP
To leave the AT-style hard disk enabled, which is the default,
simply press \*(RT.
If, however, you want to disable the hard disk (for whatever reason),
press `N'.
.PP
.B build
then asks:
.B1
.sp
	Change AT-type hard disk controller configuration ? [n]
.sp
.B2
.PP
Press \*(RT if the controller is configured correctly; this leaves the
current configuration alone.
If you have reason to believe that the configuration is wrong, press `Y'.
.B build
will walk you through the process of configuring your controller.
Do this only if you have strong evidence that the controller
is formatted improperly.
.Sh "Confirm and Complete SCSI Configuration"
.PP
If you earlier indicated that you have a SCSI hard-disk controller,
.B build
now asks you to confirm that that controller's configuration is correct.
For example, if you indicated that you have an Adaptec controller,
and you selected the first (main) driver for that controller,
.B build
displays the following message:
.B1
.sp
	HAI: Host adapter independent SCSI driver
.sp \n(pDu
	Device IS enabled.
	Do you want this device enabled ? [y]
.sp
.B2
.PP
Press \*(RT to accept this default, which is to leave the device enabled.
Answer `Y' if you wish to change the status \(em in this case, if you
wish to disable the SCSI controller.
.PP
.B build
now asks you to confirm the configuration of your controller:
.B1
.sp
	Change configuration of host adapter or target devices ? [n]
.sp
.B2
.PP
Press \*(RT to accept the default \(em that is, to leave the controller's
configuration alone.
However, you should press `Y' either if you suspect that the controller's
configuration is wrong,
.I or
if you wish to use a SCSI tape device on your system.
In either case,
.B build
again asks you for information on your SCSI peripheral devices.
.PP
If you answered `Y',
.B build
first asks you to confirm the SCSI identifiers you entered for the hard disks:
.B1
.sp
	Hard drives are configured at the following SCSI id numbers:
	\fInumbers\fP
	Is this correct ? [y]
.sp
.B2
.PP
If the SCSI identifiers are assigned correctly to your SCSI hard disk
(or disks), press \*(RT.
Otherwise, press `N'.
.B build
will again walk you through the process of assigning SCSI IDs to your
SCSI hard disks.
.PP
.B build
now asks you about SCSI tape:
.B1
.sp
	No SCSI tape drives are configured.
	Is this correct ? [y]
.sp
.B2
.PP
If you do
.I not
have a SCSI tape device, press \*(RT.
If, however, you have a SCSI tape device plugged into your SCSI controller,
press `N';
.B build
in reply will ask you to assign SCSI identifiers to your tape device or
devices:
.B1
.sp
	Enter all tape drive SCSI ID's separated by spaces, or <Enter> if none
.sp
.B2
.PP
Enter one SCSI identifier for each tape device that you have.
The SCSI identifier that you give is set by the hardware.
Some tape drives require that you set jumpers; others let you set
switch or dial to select the SCSI ID.
Note that a SCSI tape drive usually is given SCSI identifier 2;
however, the only requirement is that you not give it an identifier
already assigned to another SCSI device \(em either a hard disk or another
tape device.
.B build
will complain if you enter a SCSI ID that you have already assigned to
another device.
.PP
For details on how to use your tape device, use the command
.B man
to view the Lexicon entry
.BR tape .
.Sh "Configuring Serial Ports"
.PP
At this point,
you have confirmed the configuration of your controller (or controllers),
.B build
configures your suite of serial ports.
.B build
asks whether you wish to enable your serial ports:
.B1
.sp
	Configuring serial ports Com1-Com4.
.sp \n(pDu
	See "asy" in the COHERENT Lexicon for configuring serial ports beyond
	Com1-Com4.
.sp \n(pDu
	If your computer system uses both COM1 and COM3, one of these must
	be run in polled mode rather than interrupt-drive. Similarly, if your
	computer system uses both ports COM2 and COM4, one must be run in polled
	mode rather rather than interrupt-drive.
.sp \n(pDu
	Your BIOS identifies the following serial ports:
	Com1: \fIaddress\fP  COM2: \fIaddress\fP  COM3: \fIaddress\fP  COM4: \fIaddress\fP
.sp \n(pDu
	Enable serial port driver ? [y]
.sp
.B2
.PP
.I address
in the message is the address of the serial port in question.
If
.B build
could not detect a given port, it displays ``NONE'' for that port's
address.
Press \*(RT if you wish to enable the ports; if you do not, press `N'.
.PP
.B build
now asks whether your default printer is plugged into a serial port:
.B1
.sp
	The default printer on COHERENT is /dev/lp.
	This device may be linked to any valid serial or parallel port.
.sp \n(pDu
	Link /dev/lp to one of COM1 through COM4 ? [n]
.sp
.B2
.PP
If your printer is plugged into a parallel port
(or if you do not have a printer), press \*(RT.
If, however, your printer is plugged into a serial port, press `Y';
.B build
will ask you for the number of that port.
.PP
.B build
now asks you about your default modem:
.B1
.sp
	The default modem on COHERENT is /dev/modem.
	This device may be linked to any valid serial port.
.sp \n(pDu
	Link /dev/modem to COM1 through COM4 ? [y]
.sp
.B2
.PP
If you wish to link
.B /dev/modem
to a serial port, press \*(RT; if you do not, press `N'.
If you chose to link
.B /dev/modem
to a serial port,
.B build
now asks you which one:
.B1
.sp
	Enter 1 to 4 for COM1 through COM4 for /dev/modem ?
.sp
.B2
.PP
Enter the number of the port into which you have plugged your modem.
.PP
Again, if you have stated that you have a modem plugged into a serial port,
.B build
asks you if you wish to run the port in polled mode:
.B1
.sp
	Do you want to run COM1 in polled mode ? [n]
.sp
.B2
.PP
Note that running a modem in polled mode is much less efficient than
having it run in interrupt mode, and it will probably slow down your
entire system.
If at all possible, you should
.I not
run your modem in polled mode.
Press `Y' if you wish to run your modem in polled mode.
If you do not wish to run it in polled mode, and prefer instead to run it
in interrupt mode, press \*(RT.
.Sh "Keyboard and Console: Use Virtual Consoles?"
.PP
Now that you have configured your serial port,
.B build
walks you through the process of configuring your keyboard and console.
This process involves (1) selecting a keyboard and keyboard driver;
(2) installing virtual consoles, should you want them;
and (3) selecting the appropriate video controller.
Because the video display and the keyboard work together intimately,
you must configure both at the same time.
.PP
To begin this process,
.B build
asks you whether you wish to use virtual consoles.
With virtual consoles, you can have more than one console session functioning
at the same time;
\*(CO will think that it is dealing with multiple separate console devices.
You can flip from one console session to another by pressing a control key;
or you can have two different physical consoles \(em one monochrome and one
color \(em and run multiple sessions on each.
.B build
displays the following information about virtual consoles:
.B1
.sp
	This release of COHERENT supports multiple simultaneous sessions on the
	system console.  This feature, known as virtual consoles, supports both
	monochrome and color video cards (text mode only) with multiple "login"
	sessions on each, depending upon your hardware configuration.  If you have
	both color and monochrome adapters on your system, you can run multiple
	sessions on each of them at the same time.
.sp \n(pDu
	You may choose a loadable keyboard table or a non-loadable keyboard table.
	Loadable keyboard tables allow you to remap keys and load new tables while
	your system is active. This requires keyboards that are 100% IBM AT compliant.
.sp \n(pDu
	If you chose a non-loadable keyboard, you will not have this flexibility.
.sp \n(pDu
	Not all keyboards are 100% IBM AT compliant and will not work with loadable
	keyboard tables.  If your keyboard is not 100% IBM AT compliant, it may result
	in COHERENT not booting properly after the installation process is completed,
	or you may find that your keyboard keys are not mapped as they are labelled and
	that some keys cease to function at all.  For this reason, the default selection
	is the non-loadable keyboard.
.sp \n(pDu
	Greek keyboard support is available only if virtual consoles are selected.
.sp \n(pDu
	Do you wish to include support for virtual consoles ? [y or n]
.sp
.B2
.PP
If you answer `Y',
.B build
prompts you later for more information on your video card.
.Sh "Keyboard and Console: Select Keyboard"
.PP
The next step is in the process of configuring the keyboard and console
is to select a keyboard.
.B build
asks you to select a keyboard table from the following list:
.B1
.sp
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
.sp
.B2
.PP
The menu entries marked
.B "(not loadable)"
are linked directly into the kernel.
These include U.S., German, and French keyboards.
These keyboard drivers work with all keyboards,
even the non-standard ``clones''.
If you choose one of the ``not loadable'' keyboards,
you can use only that keyboard layout; the only way to select another
keyboard layout is to use the commands
.B cohtune
and
.B idmkcoh
to modify the kernel configuration and build a new kernel.
This sounds complex, but actually is rather easy;
for details, see the Lexicon entries for these commands.
.PP
The other 16 or so menu entries represent loadable tables.
These allow you to customize the tables to your liking,
as well as load new tables at run time, without having
to build a new kernel.
Please note, however, that they work only with well-engineered
keyboards, such as those built by IBM, Cherry, MicroSwitch, 
NMB Technologies, or Keytronics;
when used with an non-standard ``clone'' keyboard, they may not work correctly.
.PP
When the menu appears, it highlights the line for the non-loadable U.S.
keyboard.
This table will satisfy the needs of all users in the United States, and
many users abroad.
To select this entry, simply press \*(RT:
\*(CO will do the rest.
.PP
If you want another keyboard, press the space bar
until the table you want is highlighted;
then press \*(RT to select the highlighted entry.
Note that the new keyboard layout takes effect as soon as \*(CO
reboots itself.
.PP
If your keyboard's layout does not correspond to one of the
entries from the menu, select a loadable keyboard table
whose layout resembles your keyboard.
After installation, you can modify that
entry to make it correspond to your keyboard.
See Lexicon article
.B vtnkb
for details on how to create a new keyboard layout, or modify an existing one.
.Sh "Keyboard and Console: Select Video Adapter"
.PP
The next step in configuring the keyboard and console is to select
the type of video adapter you have.
Note that you can have a monochrome (text) adapter, a color (graphics)
adapter, or both a monochrome and a color adapter.
You cannot have two monochrome or two color adapters.
.PP
.B build
displays the following prompt:
.B1
.sp
	Please indicate the type of video adapter you are using.
	Many portable computers now use VGA video adapters, even though the
	the display is monochrome. For these systems, select (1) below:
.sp \n(pDu
		1. Color or VGA monochrome.
		2. Monochrome, (non-VGA).
		3. Both (two video adapters, two displays).
.sp \n(pDu
	Your choice:
.sp
.B2
.PP
Select the option that matches the hardware on your system.
.Sh "Keyboard and Console: Select Video Adapter"
.PP
The last step in configuring your keyboard and console is to select the
number of virtual consoles you want for each display device.
.B build
skips this step if you indicated that you did not want to use virtual
consoles.
.PP
.B build
asks you in the following manner:
.B1
.sp
	You may select a total of up to 8 virtual console sessions.
.sp \n(pDu
	How many active virtual console sessions would you like on your
	\fItype\fP video display [1-8] ?
.sp
.B2
.PP
.I type
is the type of video display you have, either color or monochrome.
If you have two video displays, you will see this prompt twice.
Enter the appropriate number, from `1' through `8'.
As each virtual console consumes memory, you should request only as
many as you think you need; in most instances, four is sufficient.
.PP
This concludes the configuration of keyboard and console.
.Sh "Emulation of Hardware Floating-Point Processing"
.PP
If your system does not have a mathematics co-processor
(i.e., an 80387 or an 80486),
.B build
asks you if you want to add emulation of hardware floating-point arithmetic
to your system:
.B1
.sp
	System is not equipped for floating point support.
	Floating point operations are used by WordPerfect, 123, and gcc.
	Enable software emulation of floating point [y]?
.sp
.B2
.PP
Press \*(RT if you intend to run any software that requires that your system
have a mathematics co-processor or emulate one;
these include WordPerfect, Lotus 1-2-3, and the GNU C compiler.
If you are not going to run any of these packages, you do not need
hardware floating-point emulation.
If you do not have a floating-point processor on your system, we strongly
recommend that include this module in your kernel:
it makes the kernel a little larger,
but greatly increases the usefulness of your system.
.PP
.B build
asks you to confirm your choice:
.B1
.sp
	Floating point will be ENABLED.
	Is this correct ? [y]
.sp
.B2
.PP
If this is correct, press \*(RT.
If not, press `N';
.B build
will walk you through this process again.
.Sh "Configure Floppy Tape"
.PP
The next step in configuration of your system is to configure your
system's floppy-tape device, should you have one.
.B build
begins by asking you the following question:
.B1
.sp
	QIC-80/QIC-40 Tap Drive Attached to Diskette Controller (Floppy Tape)
.sp \n(pDu
	Device is NOT enabled.
	Do you want this device enabled ? [n] 
.sp
.B2
.PP
Press \*(RT if you do
.I not
have a floppy-tape drive on your system.
If you do have such a tape, however, type `Y';
.B build
will then ask you some questions about your floppy-tape device.
.PP
It first asks you if you wish to configure the floppy-tape drive:
.B1
.sp
	Configure floppy tape ? [n]
.sp
.B2
.PP
If you press \*(RT,
.B build
will use its default configuration for floppy tape.
To perform your configuration by hand, press `Y'.
.PP
If you do press `Y',
.B build
asks you some detailed questions about floppy tape.
First, it asks about tape buffers:
.B1
.sp
	Number of 32 Kbyte tape buffers ? [32]
.sp
.B2
.PP
Enter the number of tape buffers you wish to use.
By default, \*(CO uses 32 such buffers.
The minimum number of buffers you can use is two; the maximum is 127.
.PP
Next,
.B build
asks you whether you wish to configure the driver manually, or
to let the tape drive configure itself:
.B1
.sp
	0 - manual configuration (if you know unit number or soft select mode)
	1 - auto configuration of tape drive select
.sp \n(pDu
	Your choice ? [1]
.sp
.B2
.PP
The default is to let the tape drive configure itself.
If you select that,
.B build
replies:
.B1
.sp
	Tape drive select method will be automatically determined.
	Is this correct ? [y]
.sp
.B2
.PP
Press \*(RT to accept automatical configuration.
In most instances, this will give you accept results.
If, however, you wish to enter configuration information by hand,
enter `0' to the question about manual configuration.
.B build
then asks you two additional questions about your floppy-tape drive's
configuration.
The first question concerns mode of selection:
.B1
.sp
	0 - hard select
	1 - soft select, Archive/Conner/Mountain/Summit
	2 - soft select, Colorado Memory Systems
.sp \n(pDu
	Your choice ? [1]
.sp
.B2
Press \*(RT to select the default, which is Archive soft selection.
.PP
The second question asks the unit number of your drive:
.B1
.sp
	Which unit number (0..3) is the tape drive assigned to ?
.sp
.B2
.PP
Enter the number of the tape drive's unit number.
.Sh "Configure Parallel Ports"
.PP
.B build
now asks you about your parallel ports.
The process resembles that of configuring your serial ports, but is
simpler.
.PP
.B build
begins by displaying the following:
.B1
.sp
	Your BIOS identifies the following parallel printer ports:
	LPT1: \fIaddress\fP LPT2: \fIaddress\fP LPT3: \fIaddress\fP
.sp \n(pDu
	Device is enabled.
	Do you want this device enabled ? [y]
.sp
.B2
.PP
Press \*(RT if you want your \*(CO system to be able to use the parallel ports.
If you do not, press `N'.
.PP
If you did not earlier indicate that your printer is plugged into a serial
port,
.B build
now asks which port your printer is plugged into:
.B1
.sp
	Do you want /dev/lp to refer to LPT1?
.sp
.B2
.PP
.B build
asks you this for each parallel port, until either you answer `Y' or
.B build
runs out of ports about which to inquire.
.PP
If you are not sure which port you have your printer plugged into, answer
`Y' for LPT1.
The Lexicon entry for
.B printer
tells you how to find out the port into which your printer is plugged;
it also contains much useful information on how to configure your printer,
and the printer-related software that comes with \*(CO.
.PP
After you have entered your information about parallel ports,
.B build
asks you to confirm it:
.B1
.sp
	/dev/lp will be linked to /dev/lpt\fIN\fP.
	Parallel ports will be enabled.
	Is this correct ? [y]
.sp
.B2
.PP
.I N
is the number of the port into which your printer is plugged (if any).
If this information is correct, press \*(RT.
If it is not, press `N';
.B build
will walk you again through the process of describing your parallel ports.
.Sh "Advanced Configuration of Parallel Ports"
.PP
Some printers require that a parallel port be configured in a special manner.
If this is the case with your printer \(em and if you have the information
at hand \(em then you can use this information to refine the configuration
of your printer's port.
.PP
.B build
begins by asking you if you wish to perform advanced configuration:
.B1
.sp
	Perform parallel printer advanced configuration ? [n]
.sp
.B2
.PP
If you do not wish to perform advanced configuration (and in all
probability, you will not), press \*(RT and jump to the next section
in these notes.
If you later learn that you must reconfigure your printer's port, you can
do so easily.
For details, see the Lexicon entry for the device driver
.BR lp .
.PP
If you do choose to perform advanced configuration,
.B build
begins by asking you about timings:
.B1
.sp
	The driver uses a hybrid busy-wait/timeout discipline to support printers
	efficiently that have varying buffer sizes in a multi-tasking environment.
.sp \n(pDu
	Parameter LPWAIT sets the time for which the processor waits for the
	printer to accept the next character.  If the printer is not ready
	within the LPWAIT period, the processor then resumes normal processing
	for the number of ticks set by parameter LPTIME.  Thus, setting LPWAIT
	to an extremely large number (e.g., 1000) and LPTIME to a very small
	number (e.g., 1) results in a fast printer, but leaves very few cpu
	cycles available for anything else.  Conversely, setting LPWAIT to a
	small number (e.g., 50) and LPTIME to a large number (e.g., 5) results
	in efficient multi-tasking but also results in a slow printer unless the
	printer itself contains a buffer (as is normal with all but the least
	expensive printers).  By default, LPWAIT is set to 400 and LPTIME to 4.
.sp \n(pDu
	We recommend that you set LPWAIT to no less than 50 and no more than 1000
	and LPTIME to no less than 1.
.sp \n(pDu
	LPWAIT setting:
	LPTIME setting:
.sp
.B2
.PP
Enter the settings you want.
.PP
.B build
now asks about driver checks:
.B1
.sp
	Parameter LPTEST determines whether the device driver checks
	to see if the printer is on-line before it uses the device.  If your
	printer does not support this signal, you must set LPTEST to zero.
.sp \n(pDu
	LPTEST setting:
.sp
.B2
.PP
Enter the setting you want.
.PP
Finally,
.B build
asks about hard control signals:
.B1
.sp
	Finally, some ports do not assert hardware control signal LP0 reliably.
	If port LPT1 has this problem, set LPT1_LP0_BUG to 1, otherwise set it to 0.
	Set LPT2_LP0_BUG and LPT3_LP0_BUG similarly for ports LPT2 and LPT3.
.sp \n(pDu
	LPT1_LP0_BUG setting:
	LPT2_LP0_BUG setting:
	LPT3_LP0_BUG setting:
.sp
.B2
.PP
Enter the settings you want.
.PP
This concludes the configuration of parallel ports.
.Sh "Set Number of Pseudo-ttys"
.PP
A
.I tty
is the software with which \*(CO sessions on your computer.
When a user logs into your system either through a modem
or through a terminal plugged directly into a serial port,
\*(CO uses a tty to manage her interaction with the system as a whole.
.PP
\*(CO also uses a system of
.I pseudo-ttys
(or ``ptys'') to manage user sessions that do not involve a physical device.
For example, when you use the X Window System, you can display a number
of windows on your screen, each of which runs a separate login session.
Even though you are using one physical device for all of those sessions,
\*(CO must manage each separately; and it uses pseudo-ttys to do so.
.PP
The next step in installation is to tell
.B build
how many pseudo-tty sessions you want.
.B build
requests this information from you as follows:
.B1
.sp
	To use the X windowing system or the screens package
	you must have the pty driver installed.
.sp \n(pDu
	The more pty devices you have, the more windows
	you can open, but each pty device uses memory.
.sp \n(pDu
	You may specify from 0 to 128 pty devices.
	Your system is currently configured for 8 ptys.
	How many pty devices would you like? [8]
.sp
.B2
.PP
Enter the number you want:
the default, eight, is a good, workable number.
.PP
.B build
then asks you to confirm your choice:
.B1
.sp
	Pty driver will be enabled with \fIN\fP ptys.
	Is this correct ? [y]
.sp
.B2
.PP
where
.I N
is the number of ptys that you requested.
If this is correct, press \*(RT.
If it is not, press `N'; and
.B build
will again walk you through the process of setting the number of
pseudo-ttys.
.Sh "Enable STREAMS?"
.PP
The next step in configuring your system is to decide whether you
wish to \*(ST to your kernel.
\*(ST is a system for managing peripheral devices that was introduced
with \*(UN System V, release 4.
At present, no \*(CO drivers require \*(ST; however, this will change
in the near future, with the release of networking and networked X
for \*(CO.
.PP
.B build
asks you about \*(ST as follows:
.B1
.sp
	Enable STREAMS ? [n]
.sp
.B2
.PP
Press \*(RT if you do not want \*(ST included in your kernel.
Press `Y' if you do want it included.
Note that if you do not include \*(ST in your kernel, you can easily
build a kernel at a later time that does include \*(ST.
For details on how to do so, see the Lexicon entry for
.BR STREAMS .
.PP
.B build
asks you to confirm your choice:
.B1
.sp
	STREAMS will be DISABLED.
	Is this correct ? [y]
.sp
.B2
.PP
If you had chosen to include \*(ST in your kernel,
the word ``DISABLED'' in the above message would have been
replaced by ``ENABLED''.
If this is incorrect, press `N';
.B build
will again ask you whether you want to include \*(ST in your kernel.
If, however, everything is to your satisfaction, press \*(RT.
.Sh "Trace Driver"
.PP
The \*(CO trace driver buffers kernel error messages within the kernel.
You can then dump these messages into a file, for later review.
Most users will not need this driver; however, if you are developing
device drivers or other programs that inhabit the kernel, you will
find this driver to be an invaluable debugging tool.
.PP
.B build
asks you:
.B1
.sp
	Device is NOT enabled.
	Do you want this device enabled ? [n]
.sp
.B2
.PP
As always, press \*(RT to accept the default, which in this instance is
.I not
to have the trace driver in your kernel.
To include the trace driver in your kernel, type `Y'.
.PP
If you answer `Y', to include the trace driver in your kernel,
.B build
asks you one additional question:
.B1
.sp
	Number of bytes in trace ring buffer [65536]
.sp
.B2
.PP
The ring buffer is the memory set aside to hold error messages.
The default is a good, workable amount of memory.
To accept the default, press \*(RT; to change the amount of space,
type the number of bytes that you want to set aside for the buffer.
After you enter this figure,
.B build
will ask you to confirm it:
.B1
.sp
	Is this correct ? [y]
.sp
.B2
.PP
If the amount of memory shown is correct, press \*(RT; otherwise, press `N',
and
.B build
will again ask you how much memory you wish to reserve for the trace buffer.
.Sh "Rebuilding the Kernel"
.PP
Up to this point,
.B build
has been asking you questions about your system's hardware, and about
how you wish to configure \*(CO's kernel.
.B build
now has all the information it needs;
so it now builds a new kernel that incorporates this information.
.PP
You will see much incomprehensible stuff on your screen during this phase.
Do not worry about it \(em if something goes serious wrong,
.B build
will tell you about it explicitly.
.PP
Once the new kernel is built, you will be able to boot \*(CO on your
system.
The rest of the installation process involves questions about where you
are in the world, and who will be using your system.
.Sh "Setting the Date and Time"
.PP
.B build
next sets the date and time for \*(CO.
Setting the date and time is vital to the correct operation of
\*(CO; however, \*(CO records the date and time quite differently
from the way \*(MD does.
.PP
Time under \*(CO is recorded as the number of seconds since January 1, 1970,
at midnight.
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
.sp
	You can run COHERENT with or without conversion for daylight savings time
	(summer time).  You should normally run with daylight savings time
	conversion.  However, if you are going to use both COHERENT and MS-DOS
	and you choose to run with daylight savings time conversion,
	your time will be wrong (by one hour) during daylight savings time
	while you are running under MS-DOS.
.sp \n(pDu
	Do you want COHERENT to use daylight savings time conversion [y or n]?
.sp
.B2
.PP
Answer `Y' if you wish to use daylight-savings time, or `N' if you do not.
If you answer `Y',
.B build
then asks you about when daylight-savings time begins and ends:
.B1
.sp
	By default, COHERENT assumes daylight savings time begins on the
	first Sunday in April and ends on the last Sunday in October.
	If you want to change the defaults, edit the file "/etc/timezone"
	after you finish installing COHERENT.
.sp \n(pDu
	Is daylight savings time currently in effect?
.sp
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
.sp
	According to your system clock, your local date and time are:
	\fI[date and time]\fP
	Is this correct?
.sp
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
.sp
	The command which sets the internal real-time clock of your system is
	failing repeatedly.  Either you are entering the date and time incorrectly
	or your clock hardware is not completely AT-compatible.  If your clock
	hardware is incompatible, you can continue with the installation without
	setting the clock correctly.  However, if you do so, subsequent clock
	references (including file access and modification time information) will be
	incorrect and some commands (such as "date") will not function correctly.
.sp \n(pDu
	Do you want to proceed without setting the clock correctly?
.sp
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
.sp
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
.sp
.B2
.PP
If you select 0 through 11,
.B build
sets your local time automatically.
If you select ``Other'',
.B build
displays the following:
.B1
.sp
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
.sp
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
.sp
	In order to use COHERENT's electronic mail facility and UUCP subsystem,
	you must choose a "site name" for your computer system.  In general, a site
	name consists of lower case letters or digits and should be at most seven
	characters in length.  The name you choose should be unique if you intend
	to access any other computer systems.  Some of the more well known site
	names include "mwc", "uunet", "clout", "decwrl", "hp", "kgbvax", "prep",
	"seismo", and "ucbvax".
.sp \n(pDu
	Please enter the site name for this system:
.sp
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
.sp
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
.sp
.B2
.PP
If you have arranged to have your system join a domain, enter its name.
If you have not, press \*(RT to select the default domain
.BR UUCP .
.PP
For more information on domains and electronic mail, see the tutorial for
UUCP, and the Lexicon entry for \fBmail\fR.
.Sh "Install the Dictionary"
.PP
Next,
.B build
asks if you want to uncompress the dictionary used by the command
.BR spell :
.B1
.sp
	The COHERENT dictionary in /usr/dict is shipped in compressed form.
	The compressed dictionary uses about 350 blocks (.175 MB) of disk space.
	The uncompressed dictionary uses about 1700 blocks (.85 MB).
	You can either:
		1) Remove the dictionary,
		2) Leave the dictionary compressed, or
		3) Uncompress the dictionary.
	The "spell" command will work only if you uncompress the dictionary (option 3).
.sp
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
.sp
	COHERENT includes print spooling software which allows you to use
	multiple printers.  Earlier we asked you about your printer to determine
	which port it used.  Now we will complete printer spooler configuration.
.sp
.B2
.PP
.B build
then asks:
.B1
.sp
	Is your printer a laser printer [y/n]?
.sp
.B2
.PP
Answer `Y' if it is, and `N' if it is not.
.PP
.B build
then asks:
.B1
.sp
	Is your printer a PostScript printer [y/n]?
.sp
.B2
.PP
Again, answer `Y' if it is, and `N' if it is not.
.PP
That's all there is to it.
.B build
will configure the spooler correctly, so you can begin to print as soon
as you have completed installation.
.PP
Note that the above procedure supports only one printer on one port.
You can configure MLP to support an indefinite number of printers, plugged
into serial ports, parallel ports, or even into the auxiliary ports of
dumb terminals.
For details, see the Lexicon entry for
.BR printer .
.Sh "Adding Additional Users"
.PP
The final step in installation is to tell
.B build
about the other people who will be using your system.
.PP
.B build
begins this process with the following message:
.B1
.sp
	Your COHERENT system initially allows logins by users "root" (superuser) and
	"bin" (system administrator).  In addition, the password file contains special
	entries for "daemon" (the spooler), "sys" (to access system information),
	and "uucp" (for communication with other COHERENT systems).
.sp \n(pDu
	If your system has multiple users or allows remote logins, you should require
	a password for all accounts.
	Do you want to require accounts to have passwords ? [n]
.sp
.B2
.PP
Press \*(RT if you do not want every account to have a password.
If you do want every account to have a password, press `Y'.
Regardless of what you press
.B build
now displays the following message:
.B1
.sp
	If you enter a password for an account, you will be required to enter it twice
	in order to be sure that you typed it correctly, since passwords are never
	printed to the screen when they are being entered.
.sp \n(pDu
	If you change your mind, you can change the setting of a password later with
	the 'passwd' utility. Note that if you set a password for the super-user
	'root' (and it is generally a good idea to do so), then you must take great
	care NEVER to lose or forget the password, otherwise you will be locked out
	from your own computer.
.sp
.B2
.PP
.B build
now asks you the following:
.B1
.sp
	Do you want to assign a password for user "root" ? [n]
	Do you want to assign a password for user "bin" ? [n]
	Do you want to assign a password for user "uucp" ? [n]
.sp
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
displays the following message about users who may dial into your system:
.B1
.sp
	You should consider setting up a password for remote users dialing into your
	system. In order to do this, you should consult the documentation for "login"
	to see how to create "/etc/d_passwd" and /etc/dialup" files.
.sp
.B2
.PP
The last in this process (and in installation) is to add new users to your
system.
.B build
begins by asking for the name \(em that is, the login identifier \(em of
a user:
.B1
.sp
	If you want, you can create additional logins on your system now, or you
	can simply press <enter> to indicate that you don't wish to create any
	additional user accounts.
.sp \n(pDu
	User name to add, or a blank line to finish adding users ? 
.sp
.B2
.PP
The login identifier is a brief name, usually all in lower-case letters.
Often, it's the user's first name, or first initial and last name;
e.g., user John Smith would receive the login identifier
``john'' or ``jsmith''.
Each user's login identifier can be simple;
but it must be unique on your system.
.PP
After you enter the login identifier,
.B build
asks you for the user's full name:
.B1
.sp
	Full name of user, or !! to abandon ?
.sp
.B2
.PP
Enter the user's full name, e.g., ``John Smith''.
.PP
Now, enter the file system that will hold your user's home directory:
.B1
.sp
	Parent directory of user's home directory, or !! to abandon ? [/home]
.sp
.B2
.PP
Press \*(RT to select the default.
.PP
Now, you must select a shell for your new user:
.B1
.sp
	There are two shells available for use as login shells; choose one of
	  1) the Bourne shell, /bin/sh
	  2) the Korn shell, /usr/bin/ksh.
	Choose a shell for user \fIname\fP, or !! to abandon ?
.sp
.B2
.PP
Press `1' to give your user the Bourne shell, or `2' to give him the
Korn shell.
If you are not sure which to use, select the Bourne shell.
For information on these shell, see the Lexicon entries for
.B sh
and
.BR ksh .
You can also change a user's shell at a later time; for details on how
to do so, see the Lexicon entry for file
.BR passwd .
.PP
.B build
asks you if you wish to set a password for your new user:
.B1
.sp
	Do you want set a password for user \fIname\fP  [n]
.sp
.B2
.PP
If you do wish to set a password, press `Y'; otherwise, press \*(RT.
If you choose to set a password for your user, you will see the prompts:
.B1
.sp
	New password:
	Verification:
.sp
.B2
.PP
Enter the password at each prompt.
.PP
That concludes the information you must enter for a user.
.B build
again asks you if you wish to add a new user.
When you finished entering new users, press \*(RT.
.Sh "Second Reboot and Conclusion"
.PP
.B build
has now completed its work:
it has copied \*(CO onto your hard disk, built file systems,
used information you entered to configure a kernel for you,
added users, and performed all of the other miscellaneous tasks
needed to establish a multi-user,
multi-tasking operating system on your machine.
.PP
The final task that
.B build
performs is to reboot your system a second time.
This invoke the kernel it built after the first time it rebooted your system.
After the system has rebooted, you can log in and begin to use your new system.
.PP
Before
.B build
reboots, it displays the following message:
.B1
.sp
	For a list of changes to the Lexicon since the manual was last printed,
	type
		man Lexicon
.sp \n(pDu
	System configuration done, will take effect after reboot.
.sp \n(pDu
	Installation complete.
	Don't forget to remove the last diskette from the disk drive.
.sp \n(pDu
	You must reboot your system in order to allow the preceding
	configuration steps to take effect.
	Preparing to halt system.  Hit <Enter> ...
.sp
.B2
.PP
If your system does not immediately reboot after you see this message,
press the reset button.
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
If you are experienced with \*(CO or \*(UN,
you may wish simply to see what comprises \*(CO.
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
answer `Y'.
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
the introduction to this booklet.
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
.Sh "System hangs on the second reboot"
.PP
Under some rare circumstances,
first reboot of \*(CO goes well, but the final reboot
(to boot the whole system for the first time)
hangs just before or after the screen clears.
To fix this problem, do the following:
.IP \fB1.\fR 0.3i
Press the space bar when you see the message: 
.DM
	COHERENT Tertiary boot Version 1.2.7
	Press <SPACE> to abort boot.
.DE
.IP \fB2.\fR
Type
.BR coherent.min .
.IP \fB3.\fR
Type
.B <ctrl-C>
when you see the message:
.DM
	Checking filesystems...
.DE
This drops you into single-user mode.
.IP \fB4.\fR
Type the command:
.DM
	/conf/patch -p /coherent.min rootdev:s pipedev:s
.DE
Record the values that are displayed.
(They will resemble ``0x0D00'' or ``0x0B00''.)
.IP \fB5.\fR
Type the command
.DM
	/conf/patch -v /coherent rootdev=\fIXXXX\fP:s pipedev=\fIXXXX\fP:s
.DE
where
.I XXXX
stands for the values you recorded previously.
.IP \fB6.\fR
Finally, type the command
.BR sync ,
then reboot your system and let it boot normally.
.Sh "Printing does not work"
.PP
In some cases, attempts to print via the parallel ports generates
messages of the form
.DM
	Cannot open /dev/lpt1
.DE
even though that device exists and has its permissions set correctly.
This probably is due to the fact that in a few rare instances,
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
.BR idmkcoh .
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
The waiting helps ensure that all buffered data truly ``vanish'' from your
system.
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
.Sh "Does your system have six megabytes of RAM?"
.PP
On some AMI 386SX/25 motherboards, \*(CO requires that your system's amount
of RAM be a power of two \(em that is, two, four, eight, or 16 megabytes.
If your system has this motherboard and has six megabytes, booting may fail.
The solution is either to remove two megabytes of memory,
or add two megabytes.
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
