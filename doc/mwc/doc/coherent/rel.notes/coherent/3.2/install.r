.ds TL "Installation"
.NH "Installing COHERENT" 1
.PP
This section describes how to install COHERENT onto your computer.
Installation of COHERENT is straightforward, and Mark Williams Company
has prepared a suite of programs that automate much of the work for you.
Tens of thousands of users have successfully installed COHERENT onto their
computers.
Most have found the process to be easy and quick.
A few have experienced problems with installation; so if
installation does not go as smoothly as you would like, the end of this
section describes the more common problems and how you can fix them.
If all else fails, telephone Mark Williams Technical Support, as described
in the introduction to this pamphlet.
.PP
Installation requires that you make a few decisions regarding
how you want your system to be configured.
We strongly urge you to read this section through at least once before you
begin, so you can decide correctly whenever an installation program
asks you to make a decision.
.PP
Before you begin, please note the following caveats:
.PP
First, the following conditions must be met if COHERENT is to work
on your system:
.RS
.IP 1. 0.3i
COHERENT is designed for use on the IBM AT, or on computers that
are totally compatible with the IBM AT.
These include IBM-compatible systems built around the Intel 80286, 80386,
or 80486 microprocessors.
It does not work on any MicroChannel computer
(although a MicroChannel version of COHERENT is currently under development);
nor does it work on any
computer that is not 100% compatible with the IBM AT.
.IP 2.
Your system must have at least 640K of memory,
and at least one high-density, 3.5-inch or
5.25-inch floppy-disk drive installed as drive A.
The distribution disks for COHERENT cannot be read by a low-density
floppy-disk drive, nor can they be converted from 3.5-inch to 5.25-inch,
or vice versa.
.IP 3.
Your system must have a hard disk, and the hard disk must have at least
ten megabytes of space free on it.
More is recommended, but ten megabytes is the minimum space required
by COHERENT.
If you do not have enough space on your hard disk, you will have to clear
space by removing or compressing existing files.
.IP 4.
COHERENT can only be installed onto all of a DOS partition, or into any
part of a non-DOS partition on your hard disk.
If you claim all of a DOS partition for COHERENT, COHERENT will format
that partition and turn it into a COHERENT partition \(em and in the
process, erase all MS-DOS data on that partition.
.sp \n(pDU
.I
If you do not wish to convert an MS-DOS partition to COHERENT and you do
not have a non-DOS partition of at least ten megabytes, then you must
back up your data, use the MS-DOS version of 
\fBfdisk\fP and make your MS-DOS partition smaller by at least ten
megabytes or however much space you wish to allocate to COHERENT.
.R
.IP 5.
COHERENT works with RLL, MFM, IDE and most ESDI hard-disk controllers.
It also works with some SCSI host adapters.
Please check the previous section for a list of supported hard disk
controllers and host adapters.
.IP 6.
If you intend to install COHERENT on a system which uses one of the
supported SCSI host adapters,
be sure that your host adapter is jumpered to enable interrupts
and that the interrupt number and other configuration
information is available prior to beginning the installation of COHERENT.
See Lexicon articles
.B aha154x
and
.B ss
for further details.
.PP
If you are unsure whether your system meets any or all of these conditions,
check the documentation that came with your system, or contact the dealer
from whom you purchased your system.
Section 2 of this pamphlet lists the hardware that is known to
be compatible, as well as known incompatibilities.
Check there before you begin installation.
If you believe that your computer cannot run COHERENT, please contact Mark
Williams Company.
.PP
Second, Mark Williams Company has made every effort to ensure that the
installation process will not destroy data on your hard disk.
Note, however, that the installation process requires that you assign
at least one partition of your hard disk to COHERENT.
.I
If you have any files on that partition that you wish to save, you
must back them up or they will be lost.
.ft R
It is also recommended that you keep a copy, on paper, of your
computer's partition table.
If you do not know how to obtain a copy of the partition table, one will
be displayed for you during installation.
We recommend that you jot it down at that time; if something should go
wrong, this information will help to recover the data on your disk.
.PP
Note, too, that installation may require that the entire disk be
repartitioned; in this case, you must back up all of your hard disk, or
your data will be lost.
The installation program will walk you through this process, so you do not
have to decide ahead of time what partitions, if any, need to be backed up.
.PP
With these caveats in mind, please continue \(em and we hope you
enjoy working with COHERENT!
.SH "What Does Installation Do?"
.PP
The point of the installation procedure is to create one or more partitions on
your hard disk to contain COHERENT and its files.
.PP
When you (or your dealer) installed MS-DOS on your computer, you (or he)
divided your computer's hard disk into \fIpartitions\fR.
A hard disk on the IBM AT can have anywhere from one to four
partitions.
Not every partition has to be used \(em your hard disk may be divided
into four partitions, but have MS-DOS file systems in only two of them,
with the third and fourth partitions being idle.
Note, too, that the four partitions do not necessarily have to
encompass the entire hard disk \(em a disk may have space that is outside
any partition and so just sits there unused.
.PP
The file systems for COHERENT and MS-DOS are very different, so it is not
possible to have both systems use the same partition \(em each
must have one or more partitions completely to itself.
.PP
As you can see, installation must cope with a number of variables:
the size of your disk,
the number of partitions into which it is divided,
the size of each partition,
and the number of partitions that are in use.
Installation thus will follow any of a number of possible scenarios,
depending on how your disk is organized and how much space you wish
to give over to COHERENT.
The installation process will walk you through these decisions, to make
them as painless as possible.
.PP
It may well be that you do not know the configuration of your hard disk.
COHERENT can figure this out, and the
information will be displayed for you at the appropriate point in installation.
.PP
You can abort the installation procedure at any time by typing
\fB<ctrl-C>\fR.
Note, however, that
aborting installation does \fInot\fR mean that your hard disk will be returned
to the state it was in before installation was begun.
.I
When a disk is repartitioned, the files that were on any
modified partitions are gone forever!
.ft R
.PP
The following sections describe the installation process in some detail.
Be sure to read them through before you begin.
.PP
During the installation, COHERENT will give you easily understandable prompts,
and it will notify you about all actions that could destroy data on your disk
before they are run.
You will always be asked to confirm that you wish to run an action that
destroys data before COHERENT executes that action.
.\".SH "BIOS Disk Parameters"
.\".PP
.\"Some disk controller and drive combinations use disk
.\"parameters other than the physical drive parameters.
.\"These parameters are called BIOS parameters or ``translation mode''
.\"parameters.
.\"If you do not know the BIOS parameters for your hard disk drives,
.\"you will need to invoke command
.\".BR dpb ,
.\"as described in the following section,
.\"prior to starting the COHERENT installation.
.\"Be sure to copy down all displayed parameters, as you may need
.\"to enter them during installation.
.SH "Getting Started"
.PP
To begin installation, reboot the system by pressing the reset button,
or cold-boot your system by turning it off, waiting 30 seconds, and
turning it on again.
Insert the \fBBoot\fR disk from your installation
kit into drive A on your system.
In a moment, you will be prompted with a question mark \*(Ql?\*(Qr.
Type:
.\".DM
.\"	dpb
.\".DE
.\"and press the
.\".B <enter>
.\"key.
.\"Copy down all the displayed information and keep it with your
.\"COHERENT distribution disks.
.\".PP
.\"After you finish copying down the disk information, enter
.DM
	begin
.DE
followed by the \fB<enter>\fR key.
The installation program will clear the screen and display some copyright
information.
After you press \fB<enter>\fR, you will see the following greeting:
.DM
	Welcome to the COHERENT operating system!
.sp \n(pDu
	Your computer is now running COHERENT from the floppy disk.
	This program will install COHERENT onto your hard disk.
	You can interrupt installation at any time by typing <Ctrl-C>;
	then reboot to begin the installation procedure again.
	Please be patient and read the instructions on the screen
	carefully.
.DE
As the instructions say, you can interrupt installation at any point by
typing \fB<ctrl-C>\fR.
Be sure, as well, to read the instructions carefully.
.SH "Selecting a Keyboard"
.PP
The next menu asks you to select a keyboard table from the list shown.
There are approximately 19 choices of keyboard tables.
.PP
The menu entries marked
.B "(not loadable)"
are the traditional COHERENT keyboard drivers, which are linked
directly into the kernel.
These include the traditional US, German, and French keyboards.
These traditional keyboard drivers work with all keyboards,
even the inferior ones sold with some ``clones''.
.PP
The other 16 or so menu entries represent loadable tables.
These use the
.B nkb
driver and thus allow you to customize the tables to your liking,
as well as load new tables at run time.
Please note, however, that the
.B nkb
driver (and loadable tables) only work with well-engineered
keyboards, such as those built by IBM, Cherry, MicroSwitch, or Keytronic;
when used with an inferior ``clone'' keyboard, it may not work correctly.
.PP
When the menu appears, the line for the non-loadable
US keyboard table is highlighted.
This table will satisfy the needs of all users in the United States, and
many users abroad.
To select this entry, simply press
.BR <enter> ;
COHERENT will do the rest.
.PP
If you live outside the United States and so need
a specialized keyboard table,
press the
.B <space>
bar or the arrow keys
until the table you want is highlighted; then press
.B <enter>
to select the highlighted entry.
.I
The new keyboard layout takes effect as soon as COHERENT reboots to read the
rest of the installation disks.
.R
.PP
Note that if you choose one of the entries marked
.BR "not loadable" ,
you can use only that keyboard layout; the only way to select another
keyboard layout is to re-install COHERENT.
Please note, too, that loadable keyboard tables will not work with
inferior ``clone'' keyboards; these keyboards work only with the menu entries
marked
.BR "not loadable" .
Loadable keyboard tables are known to work correctly with keyboards
manufactured by IBM, Cherry, MicroSwitch and Keytronic.
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
the MWC BBS system and on contacting satellite COHERENT BBS systems
around the world.
.SH "Entering the Serial Number"
.PP
The next screen will ask you to enter a nine-digit serial number.
This number is included on a paper supplied with your copy of the
COHERENT system.
The installation process cannot continue until you enter this number
correctly.
.SH "Setting the Date and Time"
.PP
The next screen asks you to set the date and time for COHERENT.
Setting the date and time is vital to the correct operation of
COHERENT; however, COHERENT records the date and time quite differently
from the way MS-DOS does.
.PP
Time under COHERENT is recorded as the number of seconds since January 1, 1970,
at exactly midnight.
Internally, COHERENT always stores time as Greenwich Mean Time.
GMT is used
to make it easy for COHERENT systems around the globe to coordinate time with
each other.
When COHERENT
time-stamps a file or displays the time, it converts Greenwich Mean Time to
your local time, depending on what time zone you live in and whether
Daylight Savings Time is in effect.
(For a detailed discussion of this topic, see the Lexicon's entry for
.BR TIMEZONE .)
.PP
The installation program will display the following text:
.DM
	It is important for the COHERENT system to know the
	correct date and time.  You must provide information
	about your timezone and daylight savings time.
.sp \n(pDu
	According to your computer system clock, your current
	local date and time are:
	\fIdate and time\fR
.DE
You will be asked if this is correct.
If it is not correct, the installation program will prompt you for the
correct date and time.
.PP
You will then be asked about daylight savings time:
.DM
	You can run COHERENT with or without daylight savings
	time conversion.  You should normally run with daylight
	savings time conversion. However, if you are going to use
	both COHERENT and MS-DOS and you choose to run with daylight
	savings time conversion, your time will be wrong (by one
	hour) during daylight savings time while you are running
	under MS-DOS.
.DE
You will be asked if you want to run in daylight-savings mode.
You should answer yes unless you have an overwhelming reason not to.
.PP
The installation program then describes the default daylight-savings
settings:
.DM
	By default, COHERENT assumes daylight savings time begins
	on the first Sunday in April and ends on the last Sunday
	in October.  If you want to change the defaults, edit the
	file "/etc/timezone" after you finish installing COHERENT.
.DE
The default settings are those enacted by law for the United States.
COHERENT will then
display a timezone menu similar to the following
and then ask you what time zone you live in:
.DM
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
.DE
If you select 0 through 11, COHERENT will set your local time automatically.
If you select \*(QLOther\*(QR, you will be asked how many minutes of time
you live east or west of Greenwich, and then asked to name your time zone.
If you are unclear on these concepts, consult the Lexicon article on
.BR TIMEZONE .
If you are unsure about how your local time relates to Greenwich time,
consult an atlas, check with your local library, or telephone a
local radio station.
.PP
COHERENT will then display the corrected local time and ask if it is correct.
If not, you can repeat the process until the time is correct.
.SH "Installing Onto a SCSI Hard Disk"
.PP
If you are installing COHERENT onto a SCSI disk drive,
you will will be prompted with a menu resembling:
.DM
	COHERENT currently supports the following SCSI host adapters:
.sp
	(1) Adaptec AHA-154x series
	(2) Seagate ST01 or ST02
	(3) Future Domain TMC-845/850/860/875/885
	(4) Future Domain TMC-840/841/880/881
.sp
	Enter a number from the above list or 0 to exit:
.DE
If you intend to install COHERENT on a system which uses one of these
supported SCSI host adapters,
be sure that your host adapter is jumpered to enable interrupts
and that the interrupt number and other configuration
information is available.
See Lexicon articles
.B aha154x
and
.B ss
for further details.
.SH "Back Up the Hard Disk"
.PP
After the time is set, installation moves on to its next phase,
partitioning the hard disk.
Before you become seriously involved in partitioning, however, you have
one last chance to back up your hard disk.
As you enter the partition phase of installation, you will see the following
text:
.DM
	This installation procedure allows you to create one
	or more partitions on your hard disk to contain the
	COHERENT system and its files.  Each disk drive may
	contain no more than four logical partitions.  If all
	four partitions on your disk are already in use, you will
	have to overwrite at least one of them to install COHERENT.
	If your disk uses fewer than four partitions and has enough
	unused space for COHERENT (ten megabytes), you can install
	COHERENT into the unused space.  If you intend to install
	MS-DOS, you should install it \fBbefore\fP you install COHERENT.
.sp \n(pDu
	The next part of the installation procedure will let you
	change the partitions on your hard disk.  Data on unchanged
	hard disk partitions will not be changed.  However, data
	already on your hard disk may be destroyed if you change
	the base or the size of a logical partition, or if you
	change the order of the partition table entries.  If you
	need to back up existing data from the hard disk, type
	<Ctrl-C> now to interrupt COHERENT installation; then
	reboot your system and back up your hard disk data onto
	diskettes.
.DE
If you need to back up your hard disk and have not yet done so, please
do so now.
.SH "Use the COHERENT Bootstrap?"
.PP
If you have already backed up your hard disk, continue to the next phase,
which is to decide whether to use the COHERENT master bootstrap.
When you press \fB<return>\fR, you will see the following text:
.DM
	COHERENT initialization normally writes a new master
	bootstrap program onto your hard disk.  The COHERENT
	master boot allows you to boot the operating system on
	one selected disk partition automatically; it also allows
	you to boot the operating system on any disk partition by
	typing a key when you reboot.  However, the COHERENT master
	boot may not work with all operating systems.  If you do
	not use the COHERENT boot, you must understand how to
	boot the COHERENT partition using your existing bootstrap
	program.
.DE
As explained in the prompt text, a \fIbootstrap\fR is a program that
pulls an operating system into memory and sets it to running \(em the
name relates to the fact that the operating system \*(QLpulls itself up
by its boot straps\*(QR.
The COHERENT master bootstrap can boot COHERENT as well as many other
operating systems, including MS-DOS (at least, the many versions of MS-DOS
that have been tested).
If you choose not to use the COHERENT master bootstrap, you must consult
the documentation that came with your system to see how you can use your
operating system's current bootstrap routine to boot another operating system.
If, however, you choose to use the COHERENT master bootstrap and find that
it has trouble booting your current operating system, you should be able to
boot your current operating system by using a boot floppy disk; you will not
be able to boot it off of the hard disk, but at least it will be available
to you.
For these reasons, we strongly suggest that you use the COHERENT master
bootstrap routine.
.PP
Prior to overwriting the bootstrap, the COHERENT installation procedure
will ask:
.DM
	Do you want to save the original boot block?
.DE
If you wish to save a copy of your existing bootstrap, you will need
to have a \fIhigh density formatted floppy\fP available.
The floppy disk may have been formatted using MS-DOS.
The boot block is not saved as a file \(em it is merely written into the
first sector on the floppy disk.
You will need to save your original boot block should you ever want to
remove COHERENT from your system.
While we cannot imagine why anyone would want to do such a thing,
you may want to save your original boot block, just in case.
.PP
.B install
then asks:
.DM
	Do you want to restore a previously saved boot block?
.DE
You should answer yes only if you are removing COHERENT from your
system and need to restore a previously saved boot block.
.PP
After you answer this prompt, you will move into the next phase,
partitioning the disk.
.SH "Partitioning the Hard Disk"
.PP
Installation then moves into the next phase:
selecting a disk partition for COHERENT.
As described above, partitioning can vary greatly from disk to disk;
how the disk is partitioned will determine how much space is allocated
to COHERENT and how much to MS-DOS.
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
two MS-DOS partitions, as follows:
.DM
Drive 0 Current has the following configuration:
              [In Cylinders] [  In Tracks  ]   
Number  Type  Start End Size Start  End Size Mbyte Blocks Name
0 Boot MS-DOS    0  149  150     0  899  900  7.83  15300 /dev/at0a
1     EXT-DOS  150  614  464   900 3684 2784 24.28  47430 /dev/at0b
2      UNUSED    0    0    0     0    0    0     0      0 /dev/at0c
3      UNUSED    0    0    0     0    0    0     0      0 /dev/at0d
.DE
If you have more than one hard disk on your machine, then
you will see the table for the second disk after you have finished
partitioning the first.
.PP
Note that MS-DOS names each ``drive'' by a letter of the alphabet,
beginning with C.
(This nomenclature is a hold-over from the days when MS-DOS only ran on
floppy disks, with `A' and `B' reserved for the two floppy-disk drives.)
Because MS-DOS allows you to have only two DOS partitions on any
given hard disk, then if an MS-DOS system has more than two ``drives'' on
one hard disk, the additional drives are \fIlogical\fR drives and are a
division of the EXT-DOS partition.
COHERENT cannot be installed onto a logical drive.
.PP
As mentioned above, we suggest that you copy down this table before
continuing; if an error were to occur, this information will help you
recover the data on your disk.
.PP
As you can see, this disk has four partitions, numbered 0 through 3.
Partition 0 is marked as the boot partition; what this means is
explained below.
COHERENT has given each partition a name, \fB/dev/at0a\fR through
\fB/dev/at0d\fR; you will not be working with these, however, so you can
safely ignore them for now.
.PP
Note that the middle columns of the table give
the size of each partition in three ways:
in cylinders, in tracks, and in megabytes.
How do these differ?
Megabytes is the easiest to understand:
that the number of bytes that can be written into the partition.
Cylinders and tracks, however relate to the way a hard disk is built.
A moment spent here on background can make what is to come much easier
to understand.
.Sh "Cylinders and Tracks"
.PP
Consider a high-density floppy disk.
Its surface is organized into 80 concentric rings, or \fItracks\fR,
numbered 0 through 79.
Each track holds a fixed amount of data, with the amount
depending upon the density of the disk.
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
between the platters, one head for each platter surface (or two per
platter).
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
MS-DOS, for historical reasons, organizes partitions along track boundaries;
under this scheme, the tracks of a cylinder can
be divided between two partitions.
UNIX, COHERENT, and similar operating systems prefer to organize partitions
along cylinder boundaries:
all of the tracks of a cylinder belong to only one partition.
This lessens movement of the heads, which in turn speeds up reading of the
disk.
Note that, strictly speaking, \*(QLmegabytes\*(QR has no meaning when
thinking about disk partitioning:
partitioning must be done either in tracks, or in cylinders.
Humans tend to think of partitions in terms of megabytes, that is, in terms
of the amount of data we can write into a partition, but when organizing your
disk it is much easier to think in terms of cylinders.
However, it is simple to translate cylinders into megabytes, which gives
you the best of both worlds; this will be discussed in the following
sub-section.
.Sh "Partitioning the Disk"
.PP
When you enter the partitioning phase of installation, the installation
programs will display the configuration of your hard disk for you, using
a table like the one shown above.
It then displays the following menu:
.DM
	Possible actions:
	   0 = Quit	
	   1 = Change active partition (or make no partition active)
	   2 = Change one logical partition
	   3 = Change all logical partitions
	   4 = Delete one logical partition
	   5 = Change drive characteristics
	   6 = Display drive information
	   7 = Proceed to next drive
.DE
Before we discuss what these options do, note that you must
.I not
alter the
.I size
of an MS-DOS partition using any of the above options.
You must install COHERENT into
.I all
of an MS-DOS partition,
or into any part of the drive that has space unallocated to MS-DOS.
Changing the
.I size
of an MS-DOS partition at this point, to make room for COHERENT,
will invalidate the MS-DOS partition table, may corrupt MS-DOS data, and
may make it unbootable from the hard drive.
If you do wish to change the
.I size
of an MS-DOS partition, abort installation of COHERENT, back up all of your
MS-DOS data, use the MS-DOS version of
.B fdisk
to change the layout of your hard disk, restore your MS-DOS data, and only
then install COHERENT.
.PP
The following describes each option in detail.
.RS
.IP 0. 0.3i
Option 0 is self-explanatory.
.IP 1.
The \fIactive partition\fR is the partition that the bootstrap
program reads by default.
When a partition is made the active partition, the operating system mounted
on that partition is booted automatically when your turn on your computer.
This option allows you to change the active partition,
or to designate \fBno\fP active partition,
in which case the computer will prompt you at boot time
for the partition number to boot.
You will need this option only if your hard disk has more than one logical
partition, and the partitions contain different operating systems.
Note that if later you wish to change the active partition, you can use the
COHERENT command \fBfdisk\fR to do so.
See the Lexicon entry on \fBfdisk\fR for details.
.IP 2.
This option lets you change one logical partition \(em in effect, it lets
you select a logical partition for COHERENT.
You should use this option if your hard disk has more than one partition
and you wish to install COHERENT on only one of them.
The partition you select must hold at least ten megabytes.
Note that the contents of the partition will be deleted.
.IP 3.
This reconfigures the entire disk.
You can reset the number of partitions, and the size of each.
.IP 4.
Option 4 is self-explanatory.
.IP 5.
This option allows you to change the drive parameters
associated with each drive on your system.
.IP 6.
Give summary information about the disk.
.IP 7.
This option will only appear if you have more than one hard disk drive.
Use this option to select the next hard disk drive.
.PP
Begin by entering option 6, to receive more information about your disk.
The following display gives the display for the hard disk described in
the above table:
.DM
	Drive 0 has 615 cylinders, 6 heads, and 17 sectors per track.
	It contains:
		615 cylinders of 52224 bytes each,
		3690 tracks of 8704 bytes each,
		62730 sectors of 512 bytes each
	or a total of 32117760 bytes (32.12 megabytes).
.DE
If the owner of this hard disk wanted to organize his hard disk by
megabytes, all he would have to do is divide 1 million by 52,224 to find
that one megabyte is approximately equal to 20 cylinders; thus, to make
a ten-megabyte partition, he would assign it 200 cylinders.
The size of a cylinder may be different on your system, but the principle
is the same.
.PP
The next step depends on two factors:
the current organization of your hard disk, and the amount of space you
wish to give to COHERENT.
The current limit for a COHERENT partition is 95 megabytes,
but 70 to 80 megabytes is more realistic.
If your disk has only one logical partition, you must use option 3
to create at least one new logical partition.
If your disk already has more than one logical partition, you can
use option 2 to assign one to COHERENT
or use option 3 to assign more than one,
reserving the rest for your current operating system.
Of the partition(s) that you assign to COHERENT, one must hold at least
ten megabytes \(em you cannot use two five-megabyte partitions;
thus, if no partition on your disk holds ten megabytes, you must use option 3.
.PP
How much space should you give COHERENT?
COHERENT is a multi-user, multi-tasking operating system; the more space
you assign to it on your disk, the more users and the more processes it
can support.
COHERENT, via UUCP and other communications programs, also gives you access to
information on other COHERENT and UNIX systems throughout the world; you will
want to exchange mail with other users and possibly download news and
information.
All of this takes up space.
You must have one ten-megabyte partition to hold COHERENT's root file system
(that is, the file system that holds the files that make COHERENT go), and
you would be well advised to assign at least one more partition of at least
half the size to hold users' accounts and
their files.
.PP
If you have a large disk drive that is organized into one partition
that you wish to assign to COHERENT, you are well advised to divide it
into two smaller partitions.
For example,
if you wish to allocate 40 megabytes to COHERENT, you should
create two 20-megabyte partitions rather than one 40-megabyte partition.
In addition, if you anticipate wanting to perform a full restore
of a dumped root partition, you are well advised to have a spare
COHERENT partition in addition to the root partition.
An alternative strategy would be to boot from the COHERENT Boot diskette
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
.DM
	Existing data on a partition will be lost if you change
	the base or the size of the partition.  Be sure you have
	backed up all data from any partition which you are going
	to change.
.sp \n(pDu
	You may specify partition bases in cylinders or in tracks.
.DE
Reply \*(Qly\*(Qr, to use cylinders.
The system then asks:
.DM
	You may specify partition sizes in cylinders or
	in megabytes.
.DE
Reply \*(Qly\*(Qr, again to use cylinders.
Next, the system says whether the partition is initialized to MS-DOS
or is unused.
It then asks you whether you wish to install COHERENT into the partition,
leave the partition unchanged, or mark the partition as unused.
You must select one of these possibilities:
\fBinstall\fR cannot install any operating system other than COHERENT
into a partition.
To install COHERENT into this partition, reply \*(Qly\*(Qr when asked if you
want this to be a COHERENT partition.
.PP
The final two questions ask you to enter the new base cylinder for the
partition and the size of the partition in cylinders.
Each question will prompt you with the current value for the partition.
Simply pressing \fB<return>\fR would leave this current value unchanged.
It is possible to make the partition smaller, but this serves no
practical purpose if you simply intend to install COHERENT into this
partition.
.PP
If you have made a mistake during this process, the system will prompt
you and ask you to correct it.
Otherwise, you will proceed to the next phase of installation, which is
to scan for bad blocks (described below).
.PP
One last word of warning:
you should \fInever\fR use the COHERENT version of \fBfdisk\fR to change
the size of an MS-DOS partition.
Use it to change the size of an unused partition, or of a partition that
contains an operating system other than MS-DOS; these include COHERENT
itself.
If you wish to change the size of an MS-DOS partition, back up all data
in that partition, then use the MS-DOS version of \fBfdisk\fR.
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
This concludes the discussion of partitioning the disk.
The system will then move to the next phase of installation.
.SH "Scanning for Bad Blocks"
.PP
When a partition on an MFM, RLL or IDE interface disk drive
is assigned to COHERENT, it must be scanned for
bad blocks.
(The terms \fIblock\fR and \fIsector\fR are often used
interchangeably.)
Most hard disks have at least a few blocks in which the disk's surface
is flawed and therefore cannot be trusted to hold data reliably.
Note that scanning is not required on SCSI interface devices.
.PP
COHERENT keeps a list of bad blocks for each partition, to ensure
that it does not write data into an unreliable area.
This checking is performed automatically, but takes a few minutes.
Patience is recommended.
.SH "Creating COHERENT File Systems"
.PP
Once COHERENT has created a list of bad blocks, it can generate a
file system for each of the partitions that you have assigned to it.
One partition must be assigned the root file system; the root file
system is the one that holds the files owned by COHERENT itself,
the files that make the system go.
If you are
assigning more than one partition to COHERENT, you will be asked
which you want to hold the root file system.
.SH "Mounting File Systems"
.PP
The next step is to mount the file system assigned to the physical
partition.
You are not required to mount any file system except the root file
system, although for most purposes there is no reason not to mount
a file system that you have created.
The system will ask you to assign a name to each file system.
For historical reasons, a file system is usually given the name of a single
letter from the end of the alphabet, such as \*(Qlv\*(Qr
or \*(Qlx\*(Qr, although there's no reason not to name a file system
\*(Qlwork\*(Qr or \*(Qlusr\*(Qr.
Each name must be preceded by a slash \*(Ql/\*(Qr.
.SH "Rebooting"
.PP
Now that partitions have been allocated and file systems have
been created and mounted,
the next step requires that COHERENT be booted from the hard disk.
If you have elected to use the COHERENT bootstrap, and if you have
the COHERENT boot partition the active partition, all you have to do is
remove the \fBBoot\fR disk from the floppy-disk drive when
prompted, and then reset your computer.
.PP
If you have made an MS-DOS partition the active partition, you must
perform one additional step:
type the number of the partition that holds the COHERENT root file
system as the system is attempting to access the floppy disk for the
bootstrap program.
The number must be typed from the numeric keys at the top of the keyboard,
\fInot\fR from the keypad.
Before it begins the rebooting process, the system
will tell you which number to press.
.PP
You can reconfigure the bootstrap at any time after installation.
For more information on booting, see the Lexicon article
.BR boot .
.SH "Copying Files"
.PP
If rebooting occurs correctly,
you will then be running COHERENT off of the hard disk.
Now comes the event for which all of this preparation has occurred:
the system copies the COHERENT files onto your hard disk.
The system will prompt you to insert the three remaining disks that hold
the COHERENT files, one after the other.
.PP
The system will ask you whether you want the full set of manual pages
on line in uncompressed form, and whether you want the dictionaries used
by the COHERENT spelling checker also in uncompressed form.
These files must be uncompressed before they can be used, but take up
much more room on the disk.
You must decide whether the extra convenience of having on-line manual
pages and a spelling checker is worth the extra space they require.
.SH "Touring the COHERENT File System"
.PP
Finally, for the last step in installation the system will ask you if
you wish to take a tour of the COHERENT file system.
We suggest you answer yes, for this is the best way to become familiar
with the layout of your newly installed COHERENT system.
.PP
And with that, the installation of COHERENT is finished!
.PP
If at some later time you wish to review the tour, simply run
\fB/etc/coh_intro\fP.
.SH "In Case of Difficulty"
.PP
As we mentioned earlier, tens of thousands of users have
successfully installed COHERENT onto their computers.
Most encountered no difficulties whatsoever during installation;
chances are you will not encounter any either.
If problems do arise, however, the Mark Williams support staff will gladly
help you.
Before you call, however, be sure to check the following sub-sections.
They describe many of the more commonly encountered problems,
and gives advice on how to overcome them.
If your problem is not covered in this chapter \(em or if you've tried
what it suggests and it does not work \(em call Mark Williams.
Information about contacting Mark Williams Technical Support is given in
the introduction to this pamphlet.
.Sh "Are you booting COHERENT Disk 1 via <ctrl-alt-del> (warm boot)?"
.PP
It may help to cold boot your system;
turn power off, wait at least 30 seconds, then turn power back on.
Rebooting via cold boot is more resistant to
getting a virus from something you have already loaded, and avoids
a family of bugs in the AMI BIOS, MS-DOS, and Windows that relate
to warm boots.
.Sh "Do you have a defective RAM chip?"
.PP
We have seen cases in which computers go through normal startup and
even run MS-DOS without difficulty, but give panic errors or stack
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
If you are having trouble selecting the partition your computer boots from,
it may help to modify the master boot sequence, as follows.
(1) Use the command
.B /etc/fdisk
to select \fIno\fR active partition, or,
during startup, hold down the key corresponding to the root COHERENT
partition (over the alpha keyboard, \fInot\fR on the numeric keypad) until
startup halts with a keyboard error.
(2) Press \fB<F1>\fR to continue.
Boot should progress to COHERENT.
.Sh "Is CMOS configured correctly for drive A?"
.PP
Some customers who ordered the wrong size of diskettes
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
If you have an old machine such as an IBM XT-286, you may have an eight-bit
XT controller rather than a 16-bit AT controller.
The eight-bit XT controllers are not compatible with COHERENT.
.PP
The following Western Digital controller types are eight-bit XT devices,
and are therefore incompatible with COHERENT:
.DS
	1004-27X
	1004-WX1
	1002-anything
	XTGEN, XTGEN+, XTGEN-2, XTGEN-R
.DE
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
to install COHERENT,
check the contents of \fBCONFIG.SYS\fR from the MS-DOS
root directory on drive C.
If you find one of the following \fBDEVICE=\fR entries there
that configures the hard drive,
it may be an indication that your hard disk layout is non-standard.
However, many users of the following device drivers have successfully
installed COHERENT, so please be sure that you have attempted installation
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
the standard PC partition table format does not allow cylinder numbers
greater than 1,024.
However, some disk controllers and most IDE drives
support a ``translation mode'' that allows them to
operate with BIOS parameters that do not match the actual parameters of
the disk drive (i.e., physical drive geometry).
For example, the following table lists the physical and translation mode
parameters for the Western Digital WD93044-A IDE drive:
.DS
.ta 0.5i 1.5i 2.5i
.I
		Physical	Translation
.ft R
	Cylinders:	782	977
	Heads:	4	5
	Sectors:	27	17
	Landing Zone:	862	977
.DE
The Adaptec 2372 and Western Digital 1006V-SR2 series controllers
simulate the presence of valid drive parameters when the drive type is set
to \*(Ql1\*(Qr in the system setup.
With these controllers, it is possible to install COHERENT when the drive
parameters do not correspond to any drive type in the BIOS, but you must
be sure to set the drive type to \*(QL1\*(QR in the CMOS setup.
.PP
Some instances of incompatible hard drives do not permit installation of
older releases of COHERENT, but do allow version \*(VN to be installed.
In version \*(VN, drive parameters can be entered manually from the keyboard
during installation.
.Sh "Do you have an incompatible keyboard?"
.PP
If you are experiencing problems with respect to key mappings,
and you installed one of the loadable keyboard mapping tables
from the keyboard selection menu,
you may have an incompatible keyboard.
Please note that the COHERENT
.B nkb
driver (and loadable tables) only work with well-engineered
keyboards, such as those built by IBM, Cherry, MicroSwitch, or Keytronic;
when used with an inferior ``clone'' keyboard, it may not work correctly.
If you wish to use an incompatible keyboard with COHERENT,
you will need to re-install COHERENT and specify one
of the keyboard entries marked
.B "not loadable"
from the keyboard menu.
.PP
Most of the incompatible keyboards exhibit one or more of the
following symptoms:
.PP
	\(bu SHIFT, CONTROL, or CAPS LOCK keys are transposed
.br
	\(bu CAPS LOCK, NUM LOCK, or SCROLL LOCK keys do not function
.br
	\(bu the system hangs when trying to load the keyboard mapping table
.PP
If your keyboard works correctly except for one key, such as the key
containing the `\fB|\fP' or the '\fB\e\fP', you only need update
the keyboard mapping table for your particular keyboard layout.
See Lexicon articles
.B nkb
and
.B "keyboard tables"
for further details.
.Sh "Do you have an incompatible video board?"
.PP
If you are using a Zenith Z449 series CGA/VGA video adapter and are
experiencing panic traps, there is a known defect in this board.
Contact Zenith Data Systems, or try a different video board.
.PP
Some (unlabeled) monochrome boards do not work in protected mode.
Symptoms include immediate panic or loss of video when booting from disk 1.
If possible, try exchanging video boards.
.Sh "Do you have a system incompatible with the MWC master boot program?"
.PP
The Mark Williams master boot program is known \fInot\fR to work with
certain Zenith 241- and 248-series computers using Zenith's BIOS.
It should be possible to install and run COHERENT as long as the master boot
program is \fInot\fR used.
.Sh "Do you have an incompatible clock device?"
.PP
If the install process fails when trying to set the system time and date
(\fBATclock\fR failure), please contact Mark Williams Technical Support.
.PP
It is common to see an
.B ATclock
failure when a computer has less than megabytes of RAM.
If you have less than 640 megabytes of RAM, you should upgrade your memory.
Not only will the clock problem be solved, but your system's performance
will improve as well.
.Sh "Is your system extremely slow?"
.PP
If you are running COHERENT on a SCSI disk drive and your system
is running very slowly,
odds are that your host adapter is not correctly configured.
Some non-COHERENT systems ``busy wait'' the host adapter
when performing SCSI I/O.
Since COHERENT is a multi-tasking, multi-user system, it requires
the use of interrupts to signal when the host adapter
has completed an I/O operation and is ready for the next request.
If your host adapter is not correctly configured, it may not
be asserting the correct interrupt, or it may not be asserting
any interrupt.
Please check the jumpers on your host adapter to verify that
the correct interrupt is being asserted.
If you have further questions, see Lexicon articles
.B aha154x
and
.B ss
for further details, or contact the hardware manufacturer.
.Sh "Do you have an incompatible system?"
.PP
If you have problems that cannot be helped by any of the above, 
then your computer system may not be capable of running COHERENT.
Note that it is possible for a system that cannot run COHERENT to be
made up entirely of components known to work on other systems running COHERENT.
The greatest number of these cases is seen with customers running
an AMI BIOS, IDE hard drives, or any kind of DTK motherboard.
Please record as much information as possible about make and model of
motherboard, BIOS, hard drive controller, hard drive, and all other equipment
installed; then contact Mark Williams Technical Support.
