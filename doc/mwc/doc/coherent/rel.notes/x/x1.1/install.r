.ds TI "RELEASE NOTES"
.ds TL "Installation"
.NH "Installing X Windows"
.PP
.II X^install
.II "install X"
This section describes how to install X onto your \*(CO system, and
how to configure X to work with your hardware.
.PP
Before you attempt to install X Windows for \*(CO,
\fIread this section completely.\fR
This will help you discover any ``snags'' that could complicate the
installation, and help you to think through the process before you
plunge into it.
.SH "Preliminary Tasks"
.PP
Before you install X Windows for \*(CO on your system,
there are some preliminary tasks that you must perform.
.Sh "Check Your Video Card"
.PP
X Windows for \*(CO comes with two X servers:
one works in monochrome, the other in color.
The monochrome server works ``out of the box'' with most varieties of
VGA graphics cards.
The color server works with many, but not all, VGA chip sets.
.PP
The X installation procedure itself attempts to sense your card's chip set,
and installs either the monochrome or color server,
based on the results of its test.
However, sometimes it is wrong; if you know what chip set your system uses,
you can correct this problem by altering the configuration file that the
install program writes.
.PP
The color X server works with video cards build around the following chip sets:
.DS
	ATI 18800
	ATI 28800
	Genoa GVGA
	Paradise VGA Professional
	Trident TVGA 8900C
	Trident TVGA 9000
	Tseng Labs ET3000
	Tseng Labs ET4000
	Western Digital WD90C00
	Western Digital WD90C10
	Western Digital WD90C11
.DE
.PP
The following gives the chip sets used by many popular cards.
Note that not all can be used by X color server:
.sp \n(pDu
.nf
.ta 3.0i
\fIVideo Card	Chip Set\fR
ARC V-16	PVGA1A
AT&T VDC 600	PVGA1A
AST VGA Plus	PVGA1A
ATI Prism Elite	TVGA8800CS
ATI Ultra	Mach 8
ATI Ultra Plus	Mach 32
ATI Ultra Pro	Mach 32
ATI Vantage	Mach 8
ATI Wonder	ATI18800
ATI Wonder PLUS	ATI28800
ATI Wonder XL	ATI28800
Boca SuperVGA	ET4000
Compuadd Hi-Rez card	ET4000
Diamond SpeedStar	ET4000
Diamond SpeedStar+	ET4000
Diamond SpeedStar 24X	WD90C31
Eizo MD-B07	ET3000
Eizo MD-B10	ET3000
Eizo Extra/EM	ET3000
Everex UltraGraphics II EV-236	TVGA8900
Everex EV-678	TVGA8900
Genoa 5300 SuperVGA	ET3000
Genoa 5400 SuperVGA	ET3000
Genoa 6100 SuperVGA	GVGA
Genoa 6200 SuperVGA	GVGA
Genoa 6300 SuperVGA	GVGA
Genoa 6400 SuperVGA	GVGA
Oak Technology OTI-067	OTI67 (not compatible)
Oak Technology OTI-077	OTI77 (not compatible)
Optima Mega/1024	ET4000
Orchid Designer VGA	ET3000
Orchid Designer 800	ET3000
Orchid ProDesigner	ET3000
Orchid ProDesigner Plus	ET3000
Orchid ProDesigner /e	ET3000
Orchid ProDesigner II/1024	ET4000
Orchid ProDesigner IIs	ET4000
Paradise VGA Plus	PVGA1A
Paradise VGA Professional	PVGA1A
Paradise VGA 1024	WD90C00
Sigma Legend	ET4000
STB EM-16 VGA	ET3000
STB EM-16+ VGA	ET3000
STB Extra-EM	ET3000
STB PowerGraph	ET4000
Swan SVGA with VCO chip	ET4000
Tecmar VGA AD	ET3000
TOPVGA EXPERT	ET4000
TRICOM Mega/1024	ET4000
TRUETECH VGA	TVGA8800CS
Trident TVGA 9000	TVGA9000
Trident TVGA 8900	TVGA8900C
Trident TVGA 8800	TVGA8800CS
Tseng Labs VGA	ET4000
Video7 VRAM VGA	Video7 (not compatible)
Video7 VRAM II VGA	Video7 (not compatible)
Video7 VGA 1024i	Video7 (not compatible)
Video7 VGA 16	Video7 (not compatible)
Western Digital VGA 16	PVGA1A
.PP
The monochrome server, as noted above, works with the above-named chip sets
as well as with most other VGA chip sets.
.PP
Please note that X Windows for \*(CO does
.I not
work at all with the following video graphics cards:
.IP \(bu 0.3i
All Diamond Viper series of cards.
.IP \(bu
All ATI 64-bit cards.
.IP \(bu
All Hercules monochrome adapters.
.PP
If you have any of the above video adapters, you must replace it
before you can use X Windows for \*(CO.
.Sh "Find the Resolution of Your Monitor"
.PP
Find the resolution of your monitor.
Resolution is in number of pixels per row, and number of rows on the screen.
For example, a 640\(mu480 monitor has 480 rows of 640 pixels each.
You can find your monitor's resolution from the documentation
that comes with it.
Some higher-resolution monitors can also be displayed in lower resolution.
X Windows for \*(CO lets you choose what resolution your tube is in.
Again, check the documentation that came with your monitor to see if your
monitor supports multiple resolutions.
.PP
If you wish to run the color X server with a 1024\(mu768 display, your
video card must have at least one megabyte of RAM on board.
If it does not, you must use a display no larger than 800\(mu600.
.Sh "Upgrade COHERENT"
.PP
Your system must be running \*(CO 4.2 or later.
If you have not upgraded your system to \*(CO 4.2, do so now.
If your system cannot run \*(CO 4.2, it cannot run X Windows for \*(CO.
.Sh "Check the Amount of RAM"
.PP
Your system must have at least four megabytes of RAM.
Eight megabytes is better, 16 megabytes is preferred.
If your system does not have sufficient RAM, you must upgrade it before you
can run X Windows.
If your system has only four megabytes of RAM, you must reconfigure
\*(CO to use minimal RAM.
This is described below.
.Sh "Check the Amount of Disk Space"
.PP
To install X Windows for \*(CO, you must have 25 megabytes
of space available on the partition of your hard disk that holds directory
.BR /usr/X11 .
.PP
To see how much space you have available, use the command
.B df
(which is described in the \*(CO Lexicon).
If you do not have enough space, you must remove or compress files
to make enough space.
As a last resort, you may have to repartition your hard disk.
For information on how to repartition your machine, see the release notes
that came with your \*(CO system or see the article
in the \*(CO Lexicon for the command
.BR fdisk .
.Sh "Purge Other X Systems"
.PP
Before you attempt to install X Windows for \*(CO on your system, you
must first purge any other implementation of X from the \*(CO partitions
on your system.
The reason is that every X Windows system uses the same file names, but
those files are not be compatible across implementations.
If you do not purge the ``foreign'' implementation of X, you run the
risk of invoking those files accidentally, and so confusing X hopelessly.
.PP
If you do not want to purge a current X Windows system from your machine,
you should consider installing \*(CO and X Windows for \*(CO onto
its own partition on your machine.
You can configure the \*(CO bootstrap to boot that partition automatically,
or to let you select a partition interactively.
For details, see the articles on
.B booting
and
.B fdisk
in the \*(CO Lexicon.
.Sh "Install Your Mouse"
.PP
X Windows for \*(CO works only with a serial mouse \(em that is,
a mouse that are plugged into a serial port.
It can work with a mouse manufactured by Logitech (MouseMan), Microsoft, or
MouseSystems; or with a mouse that emulates one of these manufacturers' mice.
.PP
X Windows for \*(CO does
.I not
work with a bus mouse.
If you only have a bus mouse, you must replace it with a serial mouse
before you can use this version of X.
.PP
If you have not yet plugged your mouse into a serial port, do so.
Note the port into which you have plugged your mouse.
If you are not sure, don't worry:
the installation program for X will help you to confirm the port.
.Sh "Running X in Four Megabytes"
.PP
If you want to run X Windows on a computer system
with only four megabytes of RAM, you must
perform the following steps to reconfigure your \*(CO
system to use minimal RAM.
Four megabytes is a very minimal configuration for X.
You will be able to run X applications, but barely.
.IP \fB1.\fR 0.3i
If your system's BIOS supports shadow RAM, disable it.
This allows \*(CO and X to use the 384 kilobytes of RAM that
would otherwise be used for shadow RAM.
.IP \fB2.\fR
In the file
.BR /etc/conf/install_conf/keeplist ,
change
.DM
	echo '-I SHMMNI:SEMMNI:NMSQID'
.DE
to
.DM
	echo '-I SHMMNI:SEMMNI:NMSQID:NBUF:NHASH'
.DE
.IP \fB3.\fR
In file
.BR /etc/conf/sdevice ,
place a `Y' in the second column, next to the entry for
the standard keyboard driver
.BR kb ,
and place an `N' in the second column next to the entries for the
following drivers:
.DS
.B
	em87
	streams
	sem
	shm
	msg
	nkb
	vtkb
	vtnkb
	rm
.DE
This will enable the standard keyboard driver
.BR kb ,
and disable the drivers listed above.
This will reduce the size of your kernel, and so make more space available
for X.
An example of a minimum configuration is given in file
.BR /etc/conf/sdevice.4MB .
Note, however,
that this sample configuration includes only the driver for an AT hard disk;
if your system uses a SCSI disk, you must alter it appropriately.
.IP \fB4.\fR
Run the script
.BR /etc/conf/console/mkdev ,
to update all system files to the new keyboard driver
(and the new configuration of virtual terminals).
.IP \fB5.\fR
After you make the above changes, type the following commands
to create and patch a new \*(CO kernel, called
.BR /testcoh :
.DM
	cd /etc/conf
	bin/idmkcoh -o /testcoh
	/conf/patch /testcoh NBUF=100 NHASH=97
.DE
.IP \fB6.\fR
Edit file
.B /usr/X11/lib/xinit/xinitrc
to comment out the lines:
.DM
	xsetroot -bitmap /usr/X11/include/X11/bitmaps/wide_weave
	xclock -geometry 135x141+15+26 -fg blue -chime -update 1 &
.DE
Change these lines to:
.DM
	# xsetroot -bitmap /usr/X11/include/X11/bitmaps/wide_weave
	# xclock -geometry 135x141+15+26 -fg blue -chime -update 1 &
.DE
This changes the appearance of the X screen you see when you type
.BR startx .
.IP \fB7.\fR
Shut down your system and boot the new kernel
.B /testcoh
before you try to run X.
To boot the kernel, press the space bar when the boot procedure prompts you
to do so to interrupt booting; when you see the `?' prompt, type
.BR testcoh .
If you configured
.B /testcoh
correctly, as described above, \*(CO will not include support for virtual
consoles, so you may need to edit
.B /etc/ttys
accordingly if you wish to go multiuser.
However, note that running multiuser consumes memory, which means fewer
X applications will run.
.\" .IP \fB8.\fR
.\" Now that you've gotten tired of this silliness, go out and buy four more
.\" megabytes, you cheapskate, or get a copy of
.\" .BR mgr .
.SH "Installing X Windows for COHERENT"
.PP
Now that you have finished the preliminary tasks and have made sure that your
system can run X Windows for \*(CO, you can install the system onto your
machine.
.PP
Installation has three parts.
The first two are easy, but the last requires that you type in
some information.
Please read this description through carefully, so you will know what to
expect.
.PP
Before you begin installation, please note that you can abort installation
at any time by typing
.BR <ctrl-C> .
In some instances you may have to do so more than once to abort installation
altogether.
Aborting installation will not harm your \*(CO system in any way, nor will
it affect your ability to install X again at a later time.
.Sh "Invoking Installation"
.PP
To begin, log into your system as the superuser
.BR root .
.PP
If you are installing from a 3.5-inch floppy-disk drive that is drive 0
(i.e., drive A:) on your system, type the command:
.DM
	/etc/install CohX /dev/fva0 2
.DE
If you are installing from a 3.5-inch floppy-disk drive that is drive 1
(i.e., drive B:) on your system, type the command:
.DM
	/etc/install CohX /dev/fva1 2
.DE
If you are installing from a 5.25-inch floppy-disk drive that is drive 0
(i.e., drive A:) on your system, type the command:
.DM
	/etc/install CohX /dev/fha0 3
.DE
Finally, if you are installing from a 5.25-inch floppy-disk drive that is
drive 1 (i.e., drive B:) on your system, type the command:
.DM
	/etc/install CohX /dev/fha1 3
.DE
The program
.B install
will prompt you to place a disk from your installation kit into the
appropriate floppy-disk drive.
.Sh "Copying Files"
.PP
For the first step of installation,
.B install
copies the X files from their floppy disks onto your hard disk.
Most go into directory
.BR /usr/X11 .
This takes only a few minutes.
.Sh "Continue Installation?"
.PP
After it copies the X files onto your hard disk,
.B install
asks you if you wish to continue installation:
.B1
	The X Windows archives have been copied onto your system.  The next stage
	of installation consists of (1) de-archiving and de-compressing these files;
	(2) linking objects with libraries to form executable applications; and
	(3) asking you some questions about your system, and using the information
	to build the file /usr/X11/lib/Xconfig.  This process will take some time;
	how much depends entirely upon how fast your system is.  If you do not wish
	to continue installation, please reply negatively to the following question.
	This will abort installation.
.sp \n(pdu
	Do you wish to continue installation?
	Enter Y or N:
.B2
.PP
As you can see, the post-processing of the X files may take a while.
If you are busy and do not wish to continue installation at this time,
type `N'.
To resume installation at a later time, log in as the superuser
.BR root ,
and execute script
.BR /conf/CohX.post .
Installation will resume as if it had never been interrupted.
.PP
If you wish to continue the installation now, type `Y'.
.Sh "Continuing Installation"
.PP
When you choose to continue installation (and we assume that you will do so,
sooner or later), you will see the following message:
.B1
	Continuing installation.  Note that you can interrupt installation at
	any time by typing <ctrl-C>.  You may have to type it more than once.
	To resume installation, su to root and execute script /conf/CohX.post.
.B2
.PP
As this message shows, you can interrupt installation at any point just by
typing
.BR <ctrl-C> .
You may have to type it more than once.
To resume installation at a later time, simply log in as the superuser
.BR root ,
and execute script
.BR /conf/CohX.post .
.Sh "De-Archiving Binaries"
.PP
The next step in installation is to uncompress and de-archive various files.
As this happens, you will see the following messages:
.B1
	Extracting servers and pre-built binaries. Please be patient ...
	Extracting unlinked objects for clients. Please be patient ...
	Extracting documentation files ...
	Extracting X header files ...
	Extracting X libraries and configuration files ...
	Uncompressing tools ...
.B2
.PP
If your system already has a file named
.B /usr/local/bin/cpp
or one named
.BR /usr/local/bin/findmouse ,
you will see a prompt of the form:
.B1
	gunzip: /usr/local/bin/cpp already exists; overwrite?
.B2
.PP
Always type `N', which preserves the current version of
.BR findmouse .
The reason is that this prompt only appears if your \*(CO already has
a copy of
.B findmouse
built into it, and if this is the case, then that version of
.B findmouse
supercedes the version that is included with the \*(CO release of X.
So, to repeat:
type `N' to save your copy of
.BR findmouse .
.PP
If you do not have an executable file named
.B csh
in any of the directories
.BR /bin ,
.BR /usr/bin ,
or
.BR /usr/local/bin ,
you will see the following message:
.B1
	Linking /bin/sh to /bin/csh, as expected by xvt ...
.B2
.PP
As the message states, the X application
.B xvt
expects to work with a shell named
.BR /bin/csh ,
so it must be created.
If you have a real version of
.B csh
in a directory other than those named above, or under a different name,
be sure to link it to
.B /bin/csh
to override this action of
.BR install .
.Sh "Link X Clients"
.PP
X Windows for \*(CO ships most of its X clients in the form of unlinked
binaries, rather than as linked executables,
Now that the X files have been uncompressed and de-archived,
.B install
will link the X applications with the X libraries to create executable
binaries.
.PP
.B install
informs you of this action with the following message:
.B1
	We will now build the X clients. This will take some time.  You can either sit
	back and watch your drive fill up, or take a break and come back later.
.B2
.PP
As each client is linked,
.B install
displays a message on your screen to announce that fact.
As the above message states, this probably is a good time to fetch a
cup of coffee or play with your cat.
.PP
Should linking fail for whatever reason, a diagnostic message will be
printed on your screen, and
.B install
will abort with the message
.B1
	Could not make the X executables.
.B2
.PP
Linking could fail for any of a number of reasons:
Your system may have run out of disk space;
or a file on your installation kit may have been corrupted;
or the version of
.B make
on your system could not cope with linking these binaries.
You should contact Mark Williams Technical Support for advice.
.PP
If all of the applications link correctly, you see the message
.B1
	The X clients have been built.
.B2
.PP
and installation continues.
.Sh "Install the Mouse"
.PP
The next step is to install your mouse.
To begin,
.B install
displays the message:
.B1
	This release of X Windows for COHERENT only works with serial mice.  Serial
	mice come in three varieties:  (1) Logitech (Mouseman) mice, (2) Microsoft,
	or (3) MouseSystems mice.  Most mice either are built by these manufacturers,
	or adhere to their standards.
.sp \n(pDu
	Please select the number of the serial mouse you will be using:
.sp \n(pDu
		1. Logitech/Mouseman or Logitech-compatible serial mouse
		2. Microsoft or Microsoft-compatible serial mouse
		3. MouseSystems or MouseSystems-compatible serial mouse
.sp \n(pDu
	Select 1, 2, or 3:
.B2
.PP
Enter 1, 2, or 3, depending on the type of mouse you have.
If you are unsure what type of mouse you have, press `2', for Microsoft,
which is the most popular type of mouse.
If you select the wrong type, don't panic \(em you
will be able to fix this later.
.PP
If you select `2', for the Microsoft mouse,
.B install
assumes that your mouse has two buttons.
If you select `1' or `3' (for Logitech or MouseSystems),
.B install
asks you how many buttons your mouse has:
.B1
	How many buttons does your mouse have?  Enter 2 or 3:
.B2
.PP
Type `2' or `3', depending on how many buttons your mouse has.
.PP
If you selected mouse types `2' or `3' (for Microsoft or MouseSystems mice),
.B install
asks you for the baud rate of your mouse:
.B1
	What baud rate does your mouse operate at?
.sp \n(pDu
		1. 1200 baud
		2. 2400 baud
.sp \n(pDu
	If you are uncertain, select 1.  Enter selection:
.B2
.PP
Enter `1' or `2'.
As the message says, enter `1' if you are unsure which baud rate to select.
Again, if you make a mistake, don't panic \(em you can correct it later.
.PP
Finally,
.B install
helps you to confirm the port into which you plugged your mouse.
It begins by displaying the following message:
.B1
	Please enter the COM port in which your mouse is plugged - 1, 2, 3,
	or 4.  We will then open the port and ask you to wiggle your mouse.  If
	you see stuff appear on your screen when you push your mouse, then you
	have the correct port.
.sp \n(pDu
	Enter the COM port into which the mouse is plugged:  1, 2, 3, or 4:
.B2
.PP
Type the number of port into which you plugged the mouse.
.B install
then invokes the program
.B findmouse
to test whether the mouse is plugged into your port.
.PP
If
.B findmouse
cannot open the port that you requested, it prints the message:
.B1
	Cannot open this device.
.B2
.PP
This will happen if the requested does not exist, or if it is enabled.
(If the problem is due to the port's being enabled, you should abort
installation, disable the port, and resume installation.)
If this happens,
.B install
again asks you to enter the port into which you have plugged your mouse,
so you can try another port.
.PP
If
.B findmouse
can open the port you requested, it displays the message:
.B1
	If you see stuff on the screen when you move the mouse, then you have
	found the mouse port.  Type <ctrl-C> to exit.
.sp \n(pDu
	Now wiggle your mouse:
.B2
.PP
Wiggle the mouse.
If you have requested the correct port, you will see a stream of messages
of the form:
.B1
	Reading /dev/\fIport\fP - type <ctrl-C> to exit: \fINN\fP
.B2
.PP
.I port
is the port being examined,
and where
.I NN
is a two-character code returned by the mouse.
When you are satisfied that you have (or have not)
identified the port into which the mouse is plugged, press
.B <ctrl-C>
to stop
.BR findmouse .
.PP
.B install
then asks you:
.B1
	Did you see screen output when you wiggled your mouse?
	Enter Y or N:
.B2
.PP
If you did not see anything when you wiggled the mouse, press `N';
.B install
again asks you to select a port.
However, if you did see output when you wigged the mouse, press `Y'.
.B install
then displays the messages
.B1
	Linking /dev/mouse to \fIport\fP
	Mouse serial port set to \fIport\fP.
.B2
.PP
where
.I port
is the COM port you selected.
.PP
The selection of the port concludes the configuration of the mouse.
.Sh "Define the Video Interface"
.PP
The next step in installation is to define the video interface.
.B install
begins this phase with the message:
.B1
	We will now install the video interface.
.sp \n(pDu
	We will now invoke the color X server to determine which chipset is used by
	your video card.  Your screen may clear and then return to normal video mode.
	When it does, it will display information about your video board.
	Type <Enter> to continue:
.B2
.PP
Press \*(RT.
The screen will clear briefly, as
.B install
interrogates the video card to find what variety of chip set it uses, and
how much video RAM it has.
If it recognizes the chip set,
.B install
prints a message of the form:
.B1
	Chipset: \fIname of chip set\fP
.B2
.PP
If it does not recognize the chip set,
.B install
prints the message:
.B1
	Chipset: Unknown chipset
.B2
.PP
If
.B install
recognizes the amount of video RAM on your card, it displays either
of the messages:
.B1
	Video RAM: 512k
	Video RAM: 1024k
.B2
.PP
depending upon how much video RAM your card has.
If it does not recognize the video RAM on your system, you will see no message.
.PP
If
.B install
recognizes the chip set on your video card,
.B install
configures X to use the color server, and displays the following message:
.B1
	Your chip set is recognized.  Using COLOR X server.
.B2
.PP
If it does not recognize the chip set on your video card,
.B install
configures X to use the monochrome server, and displays the following message:
.B1
	Your chip set is not recognized.  Using MONO X server.  If this is incorrect,
	see the release notes for directions on how to invoke the color server.
.B2
.PP
As the message states, if this is wrong, you can correct it.
Instructions on how to do so appear below.
.PP
The next step in video configuration is to select the video mode for your
board and monitor.
The
.I mode
is the the size of the display, in pixels.
The larger the mode, the more stuff your screen can display \(em but the
smaller (and coarser) that stuff will be.
.PP
.B install
asks you for your monitor's mode, as follows:
.B1
	Of the following video modes, which does your monitor AND video board support?
.sp \n(pDu
		1. 640x480 only
		2. 640x480 and 800x600
		3. 640x480 and 800x600 and 1024x768 (interlaced)
		4. 640x480 and 800x600 and 1024x768 (non-interlaced)
.sp \n(pDu
	Note that to select 1024x768, your video card must have at least one megabyte
	of RAM.  If you are unsure, select 2.  Please enter your selection:
.B2
.PP
Select the appropriate mode for your system.
As the message states, if you're not sure, select `2'.
.PP
This concludes the configuration of video.
.Sh "Install Manual Pages"
.PP
At this point,
.B install
writes the file
.BR /usr/X11/lib/Xconfig ,
and displays the following message:
.B1
	A generic /usr/X11/lib/Xconfig file has been created.  In most cases, this
	generic configuration will get you up and running.  For details on how to
	modify this file, see the manual that comes with this product.
.B2
.PP
Finally,
.B install
checks whether you have enough space on your disk to install the X manual
pages.
It begins by displaying the message:
.B1
	Finally, we will install the manual pages.
.sp \n(pDu
	The online COHERENT X manual pages in /usr/man are shipped in compressed form.
	The compressed manual pages use about 300 blocks (150 kilobytes) of disk space.
	The uncompressed manual pages use about 1,100 blocks (550 kilobytes).
.B2
.PP
If you do not have enough disk space,
.B install
displays the message
.B1
	Your system has insufficient free space on the root filesystem to store
	the uncompressed manual pages.  You may uncompress and un-tar the manual
	pages at a later date if you free up at least 1022 blocks on your root
	file system.
.B2
.PP
and aborts installation.
.PP
If you do have enough space,
.B install
displays the message:
.B1
	You can:
		1. Remove the manual pages,
		2. Leave the manual pages compressed, or
		3. Uncompress the manual pages.
.sp \n(pDu
	The "man" command will work only if you uncompress the manual pages (option 3).
	Please choose an option:
.B2
.PP
Select the option you want.
.Sh "Set a COHERENT File"
.PP
At this point,
.B install
has concluded its work, and exits.
However, one task remains before installation is complete.
.PP
The last step of installation is to edit the \*(CO system file
.BR /etc/profile .
The \*(CO shell reads
.B /etc/profile
when a user logs in, and sets some default environmental variables.
First, change the entry
.DM
	export PATH=:/bin:/usr/bin
.DE
to read:
.DM
	export PATH=:/bin:/usr/bin:/usr/X11/bin
.DE
This change will let a user find X executables regardless of what her
current directory is.
Second, add the entries:
.DM
	export DISPLAY=unix:0.0
	export IMAKECPP=/usr/local/bin/cpp
.DE
These set the environmental variable
.BR DISPLAY ,
which is read by many X programs, to point to your system;
and set the C preprocessor used by the command
.B imake
to the GNU version of
.BR cpp ,
which is installed as part of this package.
.PP
With this change, installation is complete.
.Sh "Try X"
.PP
Now that X is installed, try out your newly installed X system.
Just type the command
.BR startx .
In most instances, X will come up correctly \(em you will see a clock
on your screen, a terminal window, and cursor darts around the screen
as you shift the mouse.
.PP
.II "resolution^change"
.II "screen^change resolution"
.II "video mode^change"
If you specified more than one video mode, you can jump between modes by typing
.B <ctrl><alt>+
or
.BR <ctrl><alt>- .
You should try all modes;
it is possible for X to work well in one mode but appear ``messed up'' in
another.
.PP
If all is well, turn to the next chapter in this manual, which introduces
the X system and how to work with it.
.PP
If, however, you have encountered some problems \(em if the X system does
not ``appear correct'' \(em the rest of this chapter gives information for
troubleshooting your X system.
.SH "Troubleshooting X"
.PP
.II X^troubleshoot
.II Xconfig
In most cases, X will come up without difficulty.
Should a problem arise, however, the following should help you diagnose
and solve it.
.Sh "X Does Not Come Up"
.PP
With some problems, the X server will not come up at all:
after you type
.BR startx ,
the X server prints some messages on the screen and then exits
without doing anything.
If this happens, type the following command:
.DM
	nohup startx &
.DE
.PP
This invokes X and saves all of its output \(em including error
messages \(em into file
.BR nohup.out .
Read this file to see what the problem is.
Usually you will see a message of the form:
.DM
	Cannot open device /dev/mouse
.DE
.PP
This means that a file or a device is missing.
In most cases, the message is self-explanatory and easily solved.
.Sh "Switching the Server"
.PP
In some cases, you will have specified using the color server when you
should not have.
Either the color server does not work properly with your chip set, or you
have an incompatible monitor, or you have only four megabytes of RAM.
The following describes how to switch from one server to the other.
.PP
.II X386color
.II X386mono
X Windows for \*(CO comes with two versions of the X server:
.B X386color
and
.BR X386mono .
As their names imply, the former server is for color systems, and the latter
for monochrome.
To select a server, you must link the default server
.B X
to the server you want.
To link a server, become the superuser
.B root
and
.B cd
to directory
.BR /usr/X11/bin .
To use the monochrome server, type the command:
.DM
	ln -f X386mono X
.DE
To use the color server, type the command:
.DM
	ln -f X386color X
.DE
That's all there is to it.
.Sh "Configuring the Mouse"
.PP
Sometimes problems can arise with the mouse.
With some users, X comes up but dies as soon as the user moves the mouse.
This probably is due to having the mouse described incorrectly.
Some mice, if incorrectly described, output all sorts of codes when
moved \(em including
.BR <ctrl><alt><backspace> ,
which shuts down X.
Or, the mouse cursor jumps
across the screen when you first touch the mouse, then stay there.
The solution is to try different types of mice and different baud rates
in the server.
.PP
Usually, you can fix the situation by editing file
.BR /usr/X11/lib/Xconfig .
This file holds information about the hardware on your system, including
the mouse.
The X server reads it as it comes it, so that it can configure itself
appropriately.
To edit this file, become the superuser
.B root
and the invoke your favorite editor to edit
.BR /usr/X11/lib/Xconfig .
(Before you begin to modify this file, be sure to save a backup copy,
in case you make a mistake.)
.PP
If you told
.B install
that you had a
.B Logitech
(Mouseman)
mouse, then you will see:
.DM
.ta 0.5i 2.0i
	#Mouse configuration
	logitech "/dev/mouse"
.DE
If you said that you had Microsoft mouse you will see:
.DM
.ta 0.5i 2.0i
	#Mouse configuration
	microsoft "/dev/mouse"
	BaudRate 1200
	SampleRate 150
.DE
And if you said you have a Mouse Systems mouse, you will see:
.DM
.ta 0.5i 2.0i
	#Mouse configuration
	mousesystems "/dev/mouse"
	BaudRate 1200
	SampleRate 150
.DE
Lines that begin with a pound sign `#' are comments;
.B X
ignores them.
.PP
As you can see, the entries for Microsoft and Mouse Systems mice specify
the baud rate and sample rate, whereas the one for the Logitech (Mouseman)
device does not.
.PP
The commonest problem with a mouse is that X thinks you have one kind of
mouse, when in fact you have another.
This would happen if you selected the wrong type of mouse when you installed
X onto your system.
For example, you may have purchased a ``bargain-basement'' mouse whose
documentation does not state clearly what variety of mouse it is emulating;
and you had to guess what variety of mouse to use.
.PP
To solve this problem, comment out the line that names the type of mouse,
and enter another.
If you are switching from mouse of type
.B logitech
to one of types
.B microsoft
or
.BR mousesystems ,
remember to enter the lines for baud rate and sample rate, exactly as shown
above.
.PP
To change the device into which you plugged the mouse,
use the command
.B ln
to link the appropriate device to the device
.BR /dev/mouse .
For example, if you shift your mouse from COM1 to COM3,
use the command:
.DM
	ln -f /dev/com3pl /dev/mouse
.DE
As you can see from this example,
you must always use the
.I polled
and
.I local
versions of the serial port.
For details, see the Lexicon entries for
.B asy
and
.BR ln .
.PP
Another problem is that your mouse has only two buttons, but the emulation
of the third button doesn't seem to work correctly.
If you have only two buttons on your mouse, the description for your
mouse should contain the instruction:
.DM
	emulate3buttons
.DE
This tells the X server that pressing both buttons on a two-button mouse
is equivalent to pressing
the center button on a three-button mouse.
If your mouse has only two buttons and this instruction does not appear in
mouse section of
.BR Xconfig ,
you should add it.
.PP
If problems persist despite trying all three varieties of mouse, it may be
that the baud rate is incorrect.
Insert the instruction
.DM
	BaudRate 2400
.DE
immediately after the line that names your mouse.
If your
.B Xconfig
file already contains a
.B BaudRate
instruction, place a `#' in front of it; this comments it out; and
then insert the new
.B BaudRate
instruction.
Then again try all three varieties of mouse, until you find one that works.
.Sh "Problems With the Color Server"
.PP
The trickiest problems involve configuring the color X server.
If you are using the color server and the screen appears ``messed up'' or
out of focus, this probably is due to the X server's being misinformed about
your hardware.
You can solve this problem by editing
.BR Xconfig .
The following gives the applicable portion of that file:
.DM
.ta 0.5i 2.0i
vga256
  Virtual 800 600
  ViewPort 0 0
  Clocks 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 
  Modes "640x480" "800x600"
.DE
.DM
ModeDB
#           clock    horizontal timing      vertical timing
 "640x480"    4     640  672  768  800    480  490  492  525
             25     640  672  768  800    480  490  492  525
             28     640  672  768  800    480  490  492  525
.DE
.DM
 "800x600"    7     800  808  944 1056    600  608  610  633
              7     800  904 1064 1168    600  603  609  630
             33     800  808  944 1056    600  608  610  633
             36     800  824  896 1024    600  601  603  625
             39     800  864  896 1008    600  600  606  624
             40     800  840  968 1056    600  601  605  628
             50     800  856  976 1040    600  637  643  666
.DE
.DM
 "920x690"   62     920  952 1088 1168    690  688  718  724
.DE
.DM
 "1024x768i"  4    1024 1096 1100 1280    768  775  795  829  Interlace
             39    1024 1072 1200 1240    768  766  782  786  Interlace
.DE
.DM             
 "1024x768"   7    1024 1072 1200 1240    768  766  782  786
             62    1024 1096 1272 1328    768  776  778  808
             62    1024 1072 1176 1272    768  778  779  804
             62    1024 1072 1200 1240    768  766  782  786
.DE
The following describes these in detail:
.IP \fBvga256\fR
Name the display, in this case a 256-color VGA card.
.IP \fBVirtual\fR
.II "screen^virtual"
.II "virtual screen"
The size of the virtual screen, in raster lines; in this case, the screen
800 rasters wide by 600 high.
The installation program set the size of the virtual window the largest
mode your screen allows:
either 800\(mu600, or 1024\(mu768.
.II "screen^physical"
.II "physical screen"
Note that in some modes (described below), the ``physical screen'' (that is,
the image that is displayed on your tube at any given moment)
can be smaller than the virtual screen.
In this case, the physical screen acts as a kind of window onto the virtual
screen; menus and windows that fit onto the virtual screen may not fit onto
the physical screen.
By moving the mouse cursor, you can shift the physical screen to display a
different portion of the virtual screen.
.IP
If you have only four megabytes of RAM, you may wish to use the 800 by 600
setting, even if you have a full megabyte of video RAM.
This will save memory, and help your system run more efficiently.
.IP \fBViewPort\fR
.II "virtual screen"
The X and Y coordinates of the upper-left corner of the physical screen.
In this case, the physical screen is fixed in the upper-left corner of the
virtual screen.
As noted above, you can change the positioning of the physical screen upon
to the virtual screen simply by shifting the mouse cursor.
.IP \fBClocks\fR
A video card has a bank of either eight or 16 clocks.
These govern the video timing, and will be discussed below.
.IP \fBModes\fR
These give the modes of the display \(em that is, the size of the physical
screen \(em in raster lines.
This version of
.B Xconfig
includes default settings for five modes:
640\(mu480, 800\(mu600, 920\(mu690, and 1024\(mu768,
and 1024\(mu768i (interlaced).
You can use more than one mode, if you wish; the X server lets you
jump dynamically from one mode to another.
The rules for selecting a mode or modes are given below.
.IP \fBModeDB\fR
This is a data base of video timings for your card's clock timings and mode.
Most of the job of adjusting video timing involves modifying these settings.
.PP
Configuring your system requires that you select or modify these settings.
.PP
The first step in correcting video problems is to make sure that
.B X
is finding the correct chip set and video RAM on your video card.
To see what chip set the X server thinks your system is running, read file
.BR /usr/X11/lib/color_results .
.B install
created this file when you asked it to determine what chip set is on your
video card.
This file will contain something like this:
.DM
	chipset	et4000 (mem: 1024K numclocks 8)
	Clocks	25 28 33 39 22 24 33 39
.DE
Usually the X server is correct, but not always.
If the chip set shown in that file does not match the set described in your
video card's documentation,
you can override the server's guess by inserting a
.B chip set
instruction into
.BR Xconfig .
The following gives the chip sets that the color X server supports:
.DS
.ta 0.5i 1.75i
	\fBati\fR	ATI 18800 or ATI 28800
	\fBgvga\fR	Genoa GVGA
	\fBtvga9000\fR	Trident TVGA 9000
	\fBet3000\fR	Tseng Labs ET3000
	\fBet4000\fR	Tseng Labs ET4000
	\fBpvga1\fR	Paradise VGA Professional,
	\fBtvga8900c\fR	Trident TVGA 8900c
		Western Digital WD90C00,
		Western Digital WD90C10, or
		Western Digital WD90C11
.DE
Note that to use the Trident chip sets with the color X server,
a card must have at least one megabyte of video RAM.
.PP
If color server incorrectly identified your video board's chip set, you can
instruct the color server to use the chip set you specify.
Just insert the line
.DM
	chipset "\fIcode\^\fP"
.DE
into
.BR Xconfig ,
just below the line
.BR vga256 .
.I code
is the code for the chip set, as given above in boldface.
Note that
.I code
is surrounded by quotation marks \(em these are important!
.PP
For example, if the X server thinks that your video card's chip uses the
.B ET4000
chip set, as in the above example by the line
.DM
	chipset	et4000 (mem: 1024K numclocks 8)
.DE
but your video card actually uses the ET3000 chip set (as indicates by
the documentation that came with your system), then insert the instruction
.DM
	chipset "et3000"
.DE
into
.BR Xconfig ,
just below the line
.BR vga256 .
.PP
If your video card's chip set is not one of the ones shown above,
you must use monochrome driver.
.PP
The next step is to make sure that the X server thinks you have enough
video RAM.
The phrase
.B "mem: 1024K"
in
.B color_results
gives the amount of video RAM the X server thinks you have.
If this is incorrect, you can override the server's guess by using the
instruction
.BR videoRAM .
For example, if the server thinks you have 512 kilobytes of video RAM and
you really have a megabyte, insert the instruction
.DM
	videoRAM 1024
.DE
below the line
.BR vga256 .
Note the spelling of ``videoRAM'':
this is important!
.PP
In all probability, however, the problem resides in the
.B ModeDB
area.
This is a data base of video timings.
The server will use one set of timings for a clock speed that your
video card recognizes and a mode that you have requested.
The X server selects a set of video timings from this data base by
first finding the mode that you have requested, then reading the timing
available by clock speed, going from last setting to first.
.PP
For example,
.B ModeDB
contains the following video settings for 800\(mu600 mode:
.DM
 "800x600"    7     800  808  944 1056    600  608  610  633
              7     800  904 1064 1168    600  603  609  630
             33     800  808  944 1056    600  608  610  633
             36     800  824  896 1024    600  601  603  625
             39     800  864  896 1008    600  600  606  624
             40     800  840  968 1056    600  601  605  628
             50     800  856  976 1040    600  637  643  666
.DE
If you requested 800\(mu600, then the X server will read the entries
until it finds one that matches a clock speed on your system, beginning
from 50 megahertz and working back up toward seven megahertz.
.PP
Note that not every set of video timings will work on your equipment,
even if a clock speed matches one on your video card.
It may be that the X server has selected a set of timings that do not
work with your particular combination of video card and monitor.
One step you can take to correct this situation is to comment out
some sets of video timings, so that the X server cannot use them.
.PP
To do this, the first step is to find what clock speeds the server thinks
your video card is using.
Again check the file
.BR color_results .
The line that begins
.B Clocks
gives the clock speeds.
Usually, there are eight or 16 of them; for example:
.DM
	Clocks	25 28 33 39 22 24 33 39
.DE
In this set, there are actually only five unique clock speeds:
22, 25, 28, 33, and 39 megahertz.
.PP
Now, if you look back to the
.B ModeDB
settings for mode 800\(mu600,
you'll see that there are two sets of possible video timings:
one for 39 megahertz, and the other for 33.
The X server would have picked 39 by default, because that's the first
set it would have encountered that matched a clock speed on your card.
.PP
If the screen appeared ``messed up,'' try commenting out the entry for
39 megahertz:
.DM
 "800x600"    7     800  808  944 1056    600  608  610  633
              7     800  904 1064 1168    600  603  609  630
             33     800  808  944 1056    600  608  610  633
             36     800  824  896 1024    600  601  603  625
#            39     800  864  896 1008    600  600  606  624
             40     800  840  968 1056    600  601  605  628
             50     800  856  976 1040    600  637  643  666
.DE
That will force the server to use the entry for 33 megahertz (the next
lower clock speed used by your card).
This
.I may
work correctly.
You may have to try several sets of timings before you find one that works.
.PP
For some combinations of clock speed and mode,
.B ModeDB
has several sets of video timings.
If one set of video timings does not appear to work, try another.
Remember, the entries are read from the bottom up, so comment out the
lower entry first.
Make sure that the clock speed you are using matches one of
those in the
.B Clocks
variable listed in
.BR color_result .
.Sh "Inserting New Timings"
.PP
The
.B Xconfig
file included with X Windows for \*(CO simply may not contain a set of
timings that work with your combination of monitor and video card,
at the mode that you have requested.
.PP
.II "Wexelblat, David"
.II modes.db
David Wexelblat has compiled a data base of timings for a great variety of
monitors and card.
This data base is included in file
.BR /usr/X11/doc/modes.db .
Check this file and see if it contains a set of timings for your combination
of monitor and card.
If it does, patch the timings into
.B Xconfig
and see if they work.
.PP
.II video.timings
If all else fails, you may need to ``diddle'' with the video timings by hand.
For information on what video-timing settings mean, read the file
.BR /usr/X11/doc/video.timings .
Before you begin your ``diddling,'' however, please note that radically
changing video timings can damage some varieties of monitor.
Read the information on video timings
.I carefully
before you begin to modify anything!
.PP
If worse comes to worst and you cannot configure the color server correctly,
you can use the monochrome server.
This works as well as the color server, although it is quite not as pretty.
The monochrome server should run without requiring configuration on your part.
.SH "Where To Go From Here"
.PP
The next chapter introduces you to the
.B twm
window manager.
The chapter after that introduces the utilities and clients included
with X Windows for \*(CO.
If X is new to you, you probably find these helpful.
If you are experienced, turn to the Lexicon, look up your favorite application,
and go to it!
