.ds TL "Writing a Device Driver"
.NH "Writing a Device Driver"
.PP
This section discusses how to go about writing a device driver.
.SH "Types of Device Driver"
.PP
Beginning with release 4.2,
\*(CO offers two methods for interfacing a driver with the kernel:
the
.I "internal-kernel (IK) interface"
and the
.IR "DDI/DKI interface" .
Each consists of functions that the driver can call, predefined constants,
types, etc.
Under \*(CO 4.2, the structures that support the calling functions
can be either from the IK interface or the DDI/DKI.
This represents a transition from the IK method toward the DDI/DKI and
\*(ST, which is the direction of future development.
.PP
The following describes the differences between these interfaces.
.IP "\fBInternal Kernel Interface\fR"
This type of driver uses the routines internal to the \*(CO kernel.
Practically all of the drivers included in the Driver Kit use this interface.
.IP "\fBDDI/DKI Interface\fR"
.II "DDI/DKI^definition"
The source code for drivers
.B loop
and
.B dump
use this interface.
All new \*(ST drivers must use this interface.
This is the preferred interface for new character drivers.
.PP
New block drivers must use the basic features of this interface; however,
the DDI/DKI interface is preferred for new character drivers.
.PP
When you begin to write a driver for \*(CO, you should pick carefully
between these strategies:
.IP \(bu 0.3i
The internal-kernel interface is proven and works; however, note that
this does not conform to published \*(UN definitions,
and a driver written in this interface
is less portable to another operating system.
.IP \(bu
The DDI/DKI interface attempts compatibility with \*(UN System V Release 4;
however, the \*(CO implementation lacks some features present in true \*(UN.
We expect that the degree of compatibility between \*(CO's DDI/DKi interface
and System V Release 4 will increase with ongoing development of \*(CO
internals, including the memory manager and the file system.
.PP
\*(ST is a flexible and powerful method of programming character drivers.
The \*(CO implementation of \*(ST was developed simultaneously with its
implementation of the DDI/DKI, and therefore the two methods of writing
drivers show a high degree of overlap.
As shipped with \*(CO 4.2, \*(ST does not yet have line discipline
or TLI modules available.
All \*(ST development should be done with the DDI/DKI interface.
.PP
Sets of routines from the DDI/DKI can be combined with those from the
internal-kernel interface.
In some cases, the DDI/DKI offers the better method of performing a given
task; in others, the internal-kernel interface offers the better (or, more
likely, the only) method to perform a task.
If you are importing a driver from \*(UN System V Release 4, then you should
use the DDI/DKI routines primarily.
Likewise, you should use them primarily if you are writing a driver that you
wish to export to \*(UN.
Note, too, that as \*(CO evolves toward the standard of System V Release 4,
the DDI/DKI interface will grow in importance.
.PP
The sources included with release 4.2.05 of the device-driver kit are in
the internal-kernel format rather than DDI/DKI.
It was simply not practical to recast these drivers in the DDI/DKI mold
at the present time;
however, we are supplying information regarding DDI/DKI interfaces
to inform developers of the future direction of \*(CO.
In the development of new drivers, DDI/DKI facilities should be used
wherever possible for greatest compatibility, e.g., with future releases
of \*(CO.
.PP
To summarize,
all else being equal, the DDI/DKI is preferred over the internal-kernel
interface.
The Lexicon entries themselves will alert you of the alternate ways of
performing a given task, to help you decide which to use.
.PP
The best way to judge which interface you should use is to read the sources
included with the \*(CO Device Driver Kit:
.IP "\fBasy\fR (\fB/etc/conf/asy/src\^\fR)"
This driver manipulates non-intelligent serial ports.
It is an example of a non-\*(ST character driver.
.IP "\fBat\fR (\fB/etc/conf/at/src\fR)"
This driver manipulates the AT hard disk.
It gives the best demonstration of writing a block driver, with
regard to compatibility with \*(UN System V Release 4.
.IP "\fBhai154x\fR (\fB/etc/conf/hai/src\fR)"
This driver manipulates SCSI devices.
It demonstrates how to use first-party DMA.
.IP "\fBss\fR (\fB/etc/conf/ss/src\fR)"
This driver manipulates the Seagate SCSI disk.
It demonstrates how to use memory-mapped I/O.
.IP "\fBfd\fR (\fB/etc/conf/fd/src\fR)"
This driver manipulates the floppy disks.
It demonstrates how to perform DMA via the Intel controllers.
.PP
Beyond this, you must use your best judgement as you gain experience in
working with \*(CO.
.SH "Planning the Device Driver"
.PP
This section discusses how to plan a device driver.
We strongly urge you to read this section carefully:
it will help you avoid many of the pitfalls that plague developers of
device drivers.
.Sh "Defensive Programming"
.II "defensive programming"
.PP
To begin, you must assume that you will damage your file systems
.I "at least once"
during development of your driver.
To avoid damaging irreplacable files, we suggest that you do the following.
.PP
First, perform a full backup of your system before you begin to test and
debug your driver.
See the Lexicon entry for
.B backups
describes how to back up to floppy disk; the entry for
.B tape
describes how to back up to tape.
.PP
Second, you should create a \*(CO system that can be run from a floppy disk.
One attractive feature of the \*(CO system is that a stripped down
version is small enough to be run from a high-density floppy disk drive.
You can then incorporate your device driver into the kernel that is
run from your floppy-disk version of \*(CO; if something goes wrong,
the files on your hard disk should be protected from damage.
See the Lexicon entry for
.B booting
for directions on how to build a bootable floppy disk.
Also, you can run the script
.B /etc/conf/bin/Floppy
(which is included with the Driver Kit) to build a bootable floppy disk.
.PP
Finally, as you plan your driver, you must bear in mind the fact
that the driver stands between the kernel and the device.
It receives input from and writes output to the kernel; it receives
input from and writes output to the device.
Because the driver
is linked into the kernel, it in effect is part of the kernel.
Therefore, when you write a driver you must think like a kernel programmer:
you must
.I always
bear in mind the context in which a driver operates.
Always ask yourself:
.I
Is this function or structure available to me in this context?
If it is not, how can I make it available?
.R
.Sh "Testing the Hardware"
.PP
Before you begin to write a driver, be sure to test the hardware.
This will involve writing a program at the user level that lets you
access the hardware via a device driver.
When this is done, you should take the user manual and, as thoroughly
as you have time and patience for, test
.I every
feature described in the manual and confirm that the hardware works as
documented.
Our experience in both writing and using technical documentation leads us
to conclude that, try as one might,
it is practically impossible to write an error-free manual.
.PP
You will save yourself much time and agony
in the debugging phase if you test the hardware ahead of time.
We also suggest that you alert the manufacturer to any errors you discover
in the manual:
this will earn you the gratitude of the manufacturer and of your fellow users.
.Sh "Major Device Number"
.PP
Once you have tested and confirmed that the hardware works as described
(or noted all the places where the hardware's behavior varies from the
documentation),
you can begin to write your driver.
.PP
The first step is to select a major device number for the device you will
be supporting.
The entry for
.I "device drivers"
in this manual's Lexicon lists the major device numbers that are currently
used under the \*(CO system.
In addition, header file
.B <sys/devices.h>
contains symbolic constants for all assigned major numbers.
Select one that is unused and assign it to your driver.
.Sh "Naming Conventions"
.II "naming conventions"
.PP
The next step is to devise some naming conventions for your driver.
The conventions will govern both how you structure your driver, and how
you name it to the \*(CO system.
It is common practice to use the first two letters of the name of the
configuration table to indicate the device.
You can, however, use a prefix of up to eight characters; it is best to be
brief but unambituous.
.PP
To create a device file for a file, append the minor device number to the
device name.
If a driver can support more than one device, they can be distinguished
by an alphabetic suffix.
For example, \*(CO's hard-disk driver is called
.BR at .
The \*(CO system supports two drives, so there are two minor numbers,
.B at0
and
.BR at1 .
Finally, each drive can have four partitions, each of which is accessed via
a different device file, plus one for the partition table.
Thus, each drive has five device files:
.BR at0a ,
.BR at0b ,
.BR at0c ,
.BR at0d ,
.BR at0x ,
.BR at1a ,
.BR at1b ,
.BR at1c ,
.BR at1d ,
and
.BR at1x .
.PP
To avoid inadvertent name-space collisions, prefix 
with the name of the device
the names of functions, variables, and arrays within your device driver,
and the constants, types, etc., defined within associated header files.
.Sh "Errors"
.PP
To report an error to the calling process, call function
.B set_user_error()
with the appropriate error code.
.II errno.h
For a list of legal error codes, see the entry for the header file
.B <errno.h>
in the \*(CO manual's Lexicon.
.SH "Coding Requirements"
The following summarizes the coding requirements for device drivers that
use the internal-kernel or DDI/DKI interfaces.
.PP
To begin, the coding requirements for the internal-kernel interface:
.IP \fB1.\fR 0.3i
Put `C' in the miscellaneous flags in the file
.BR /etc/conf/mdevice .
.IP \fB2.\fR
Do not define symbol
.B _DDI_DKI
in the driver's source file.
.IP \fB3.\fR
Place driver's entry points in a
.B CON
structure.
This structure is described below.
The functions themselves may be declared as
.BR static .
.IP \fB4.\fR
There is no distinction between internal and external major-
and minor-device numbers.
A device number (\fBdev_t\fR) is a 16-bit object.
Use internal-kernel routine
.BR minor() ,
\fIq.v.\^\fR, to obtain the minor-device number.
.IP \fB5.\fR
Either include
.BR <sys/coherent.h> ,
or explicitly define symbol
.B _KERNEL
to be one, before any other #include directives in the driver source.
.PP
The coding requirements for the DDI/DKI interface are as follows:
.IP \fB1.\fR 0.3i
Do not put a `C' into the miscellaneous-flags field in file
.BR /etc/conf/mdevice
(\fIq.v.\^\fR).
.IP \fB2.\fR
Insert the directive
.DM
	#define _DDI_DKI 1
.DE
.IP
in the driver's source file, before any
.B #include
directives.
.IP \fB3.\fR
Put an entry into the function-flags field in
.B /etc/conf/mdevice
for each of the driver's entry points;
do not put them into a
.B CON
structure.
.IP \fB4.\fR
A device number (\fBdev_t\fR) is a 32-bit object.
There is some
discussion in the literature of internal \fIvs.\fR external numbering for
device numbers and for the major and minor parts of the device number as well.
As of \*(CO 4.2.05, only external numbers are of interest
to the writer of device drivers.
Thus, when a
.B dev_t
is passed to a driver's entry point, it is an external device number.
When major numbers are entered into file
.BR /etc/conf/mdevice ,
they are external major numbers.
Unit numbers and device features are decoded from the
external minor number, which is obtained from the external device
number by calling the DDI/DKI routine
.BR geteminor() .
.IP \fB5.\fR
Define symbol
.B _KERNEL
to be one in the driver source, before any
.B #include
directives.
.PP
The rest of this section emphasizes the internal-kernel interface to
the \*(CO, as this interface is used by practically every driver that
is now shipped with \*(CO 4.2.
The bibliography given at the beginning of this manual lists several
excellent books that describe the DDI/DKI and \*(ST, and give extended
examples of how to code uses these interfaces.
Note that these interfaces will become increasingly important as \*(CO
evolves toward the standard set by \*(UN System V Release 4.
.SH "The Internal-Kernel Interface"
.PP
This section describes the internal-kernel interface between a device
driver and the \*(CO kernel.
.PP
.II kernel^functions
The \*(CO kernel contains numerous functions that perform the basic
work of driving a device.
Some are ordinary kernel system calls, which a driver can call just
like any user-level application.
Others are internal to the kernel and can be called only by drivers and
other internal kernel processes.
The following introduces some of the more frequently used calls and routines.
Each is described in more detail either in this manual's Lexicon, or
in the Lexicon that comes in the manual for the \*(CO system.
.Sh "Interface to System Calls"
.PP
.II open()
.II close()
.II read()
.II write()
.II ioctl()
.II poll()
.II CON
Each driver that uses the internal-kernel interface contains a
.B CON
structure.
This structure contains pointers to the functions that the kernel is to
execute when an application invokes the system calls
.BR open() ,
.BR close() ,
.BR read() ,
.BR write() ,
.BR ioctl() ,
or
.BR poll() .
.B CON
includes the following fields:
.IP \fBc_flag\fR
This field's bits give the ways
in which this device can be accessed, as follows:
.DS
.II poll()
.ta 0.5i 1.5i
	\fBDFBLK\fR	Block-special device
	\fBDFCHR\fR	Character-special device
	\fBDFTAP\fR	Tape device
	\fBDFPOL\fR	Accessible via \*(CO system call \fBpoll()\fR
.DE
.IP "\fBc_mind\fR"
.II devices.h
This field gives the device's major-device number.
This number is an index to the driver's place in the kernel's table of
drivers.
This number must be in the range of zero through 31,
and must be a symbolic constant found in file
.BR <sys/devices.h> .
.IP "\fBc_open\fR"
This field points to the routine within the device driver
that is executed whenever \*(CO opens the device.
This function is always called with two arguments:
the first is a
.B dev_t
that indicates the device being accessed, and the second is an integer
that indicates the mode in which it is being opened.
The mode can be
.B IPW
(write mode),
.B IPR
(read mode),
or
.BR "IRW | IRP" .
If an error occurs during execution of this function, it should set field
.B u_error
within the process's
.B UPROC
structure to an appropriate value.
.IP "\fBc_close\fR"
This field points to the routine that is executed whenever \*(CO closes
the device.
This function takes the same arguments as the
.B open
function.
.IP "\fBc_block\fR"
This field
points to the routine within the device driver that is executed
when the kernel reads a file in block mode.
.II BUF
This function is called with a pointer to the structure
.BR BUF ,
which contains the following fields:
.RS
.IP \fBb_dev\fR 1.0i
This field is of type
.BR dev_t ,
which is a structure that describes the device being buffered.
.II major()
.II minor()
The kernel macros
.B major()
and
.B minor()
translate this structure into the device's major and minor numbers.
.IS \fBb_req\fR
Type of I/O request, either
.B BREAD
or
.BR BWRITE .
.IS \fBb_bno\fR
This givesn number of the starting block.
.IS \fBb_paddr\fR
This field gives the system-global (DMA) address for the data.
.IS \fBb_count\fR
This field gives number of bytes to read or write.
.IS \fBb_resid\fR
This field gives the number of bytes that remain to be transferred.
A value of zero indicates that all data transferred correctly, i.e., that
an error did not occur.
.RE
.IP
.II bdone()
The driver function that performs block transfers of data should first
perform the I/O transfer, then set field
.B b_resid
to the appropriate number and call kernel function
.B bdone()
to clean up after itself.
.IP
Note that the function that performs block transfer must
.I never
sleep or access a process's
.B uproc
structure.
This is because this function is asynchronous and therefore not pegged to
a particular process.
.IP "\fBc_read\fR"
.II "read a device"
This field
points to the driver's routine that is called when the kernel wishes to
read data from that driver's device.
.II dev_t
.II IO
It takes two arguments:
the first argument is a
.B dev_t
that indicates the device to read;
the second points to the
.B IO
structure for that device.
This structure contains the following fields:
.RS
.IP \fBio_seek\fR 1.0i
This field gives
the number of bytes from the beginning of the file or device
whence reading should begin.
This is, of course, is meaningless for devices for devices like serial ports.
.II BSIZE
In the case of disk drives, this number must indicate the block to be read,
i.e., the number must be evenly divisible by constant
.BR BSIZE ,
which gives the size of a \*(CO block.
If this is not true, an error has occurred.
.IS \fBio_ioc\fR
The number of bytes to read or write.
When the read is completed, this should be set to the number of bytes
that remain to be read or written.
If it is not reset to zero, then an error has occurred.
.IS \fBio_base\fR
The offset of data to be transferred in the user memory space.
This is converted to a physical or virtual memory address before
performing the read.
.IS \fBio_flag\fR
Flags.
.II io.h
See header file
.B <sys/io.h>
for the flags that \*(CO recognizes.
.B IONDLY
indicates that the request is non-blocking.
.RE
.IP
Unlike a block transfer, the read function does not return until I/O is
complete.
.II x_sleep()
.II wakeup()
Your driver can use the kernel functions
.B x_sleep()
and
.B wakeup()
to surrender the processor to another process while the read is being performed.
.II ioputc()
It can also use the kernel function
.B ioputc()
to send characters to the user process and to update counter
.BR io_ioc .
.IP "\fBc_write\fR"
.II "write to a device"
This field points to the function that the kernel
invokes when it wishes to write to this device.
It behaves exactly the same as the function pointed to by field
.BR c_read ,
except that the direction of data transfer is reversed.
.II iogetc()
Your driver can use kernel function
.B iogetc()
is used to fetch characters from the user process and to update counter
.BR io_ioc .
.IP "\fBc_ioctl\fR"
.II ioctl
.II "I/O control"
This field
points to the function that the kernel executes when it wishes to exert
I/O control over a device.
This function is called to perform non-standard manipulations of a device,
e.g., format a disk, rewind a tape, or change the speed of a serial port.
.IP
The kernel always calls this function with three arguments:
the first argument is a
.B dev_t
that identifies the device to be manipulated;
the second is an integer that indicates the command to be executed;
the third points to a character array that can hold additional information,
if any, that the command may need.
.IP
This function, by its nature, uses a considerable amount of device-specific
information.
.II tty.h
.II mtioctl.h
.II lpioctl.h
The header files
.BR <sys/tty.h> ,
.BR <sys/mtioctl.h> ,
and
.B <sys/lpioctl.h>
define codes for, respectively, teletypewriter devices (i.e., terminals),
magnetic-tape devices, and line printers.
.IP "\fBc_power\fR"
.II "power-fail routine"
This field
points to the routine to be executed should power fail on the system.
This field is not yet used by \*(CO.
.II dpower()
.IP "\fBc_timer\fR"
This field
points to the routine that the kernel executes when a device driver
requests periodic scheduling.
To request that the timeout routine for device
.I dev
be called once per second, set
\fBdrvl[major(\fIdev\fB)\.d_time\fR to a nonzero value.
.II con.h
.II major()
.II stat.h
The external variable
.B drvl
is declared in header file
.BR con.h ;
macro
.B major()
is defined in header file
.BR stat.h .
.IP
The value in field
.B d_time
is not altered by the kernel's clock routines.
To stop invocations of the timeout routine, store zero in
\fBdrvl[major(\fIdev\fB)].d_time\fR.
.I dev
is a
.B dev_t
that indicates which device is being timed out.
.IP "\fBc_load\fR"
.II "loading a driver"
This field
points to the routine that is executed when this device driver is loaded.
This performs all tasks necessary to prepare the device and
the driver to exchange information.
If the driver is linked into the kernel, then this routine is executed
when \*(CO is booted.
.IP
Although \*(CO does not currently support loadable drivers, you
should still write a load routine for driver startup and driver detachment
from the kernel.
This will spare you the labor of modifying your driver once loadable
drivers are enabled.
.IP "\fBc_uload\fR"
.II "unloading a driver"
This field
points to the driver's function that the kernel invokes when the driver
is unloaded from memory.
Although \*(CO does not support loadable drivers, you
should still write an unload routine for driver startup and driver detachment
from the kernel.
This will spare you the labor of modifying your driver once loadable
drivers are enabled.
.IP "\fBc_poll\fR"
.II "polling the device"
This field
points to a function that can be accessed by commands or functions
that poll the device.
The driver's polling function is always called with three arguments.
The first argument is a
.B dev_t
that indicates the device to be polled.
The second is an integer whose bits flag which polling tasks are to be
performed, as follows:
.DS
.ta 0.5i 1.5i
	\fBPOLLIN\fR	Input data is available
	\fBPOLLPRI\fR	Priority message is available
	\fBPOLLOUT\fR	Output can be sent		
	\fBPOLLERR\fR	A fatal error has occurred
	\fBPOLLHUP\fR	A hangup condition exists
	\fBPOLLNVAL\fR	\fBfd\fR does not access an open stream
.DE
.II poll.h
These are defined in the header file
.BR <sys/poll.h> .
The third argument is an integer that gives the number of millseconds by
which the response should be delayed.
Note that the \*(CO clock timer runs at 100 Hz rather than the
approximately 18 Hz clock used by MS-DOS.
.IP
.II pollopen()
.II pollwake()
The kernel functions
.B pollopen()
and
.BR pollwake() ,
respectively, initiate and terminate a polling event.
For more information on these functions, see their entries in this
manual's Lexicon.
.Sh Timing
.PP
Sometimes a driver must delay for a period of time while doing
something in the kernel.
For example, you may want to wait within a device driver
for no more than given period of time; if that period of time elapses,
you assume that a timeout has occurred in the chain of desired events.
.II timeout()
If the delay is a given number of clock ticks, use kernel routine
.B timeout()
to call a given function after the specified number of ticks.
Interrupts must be enabled for this to work;
granularity is 0.01 sec (10 milliseconds) because
that is the current length of a clock tick.
.PP
.II read_t0()
Kernel function
.B read_t0()
helps you to compute intervals of less than one tick.
.II PIT
This function, which takes no arguments and returns an
.BR int ,
reads channel 0 (t0) of the programmable interval timer (\fBPIT\fR),
which drives the system clock.
A system clock tick is the time it takes timer t0 to decrease from 11,932
to zero.
You can read the timer any time, whether interrupts are masked or not,
and get a number between 11,932 and zero.
Each unit therefore represents a little less than a microsecond.
Overhead per call to
.B read_t0()
is about five to ten microseconds, depending on your CPU and clock.
.II busyWait()
.II busyWait2()
The kernel functions
.B busyWait()
and
.B busyWait2()
are useful for waiting briefly.
They can return early from a wait if an awaited condition has been specified
and is met.
Their timing is independent of CPU speed.
.PP
.II read_t0()
Because CPU speeds among supported equipment vary by at least an order of
magnitude, we
.I strongly
encourage all fine-timing loops to use
.B read_t0()
rather than simply counting down to zero using some empirically chosen
loop count.
.Sh "Sleeping and Waking"
.PP
.II x_sleep()
.II wakeup()
.II "sleep^caveats"
The driver will constantly call the kernel functions
.B x_sleep()
and
.B wakeup()
to synchronize your device driver with events in the operating system.
.B x_sleep()
moves the driver process to the suspended queue and sets a unique
condition under which the process will awaken;
.B wakeup()
wakes up the process associated with that event.
.PP
For example, when a driver attempts to read a floppy disk, it may take
several seconds for the floppy disk to begin to spin fast enough to be read.
This may be a relatively brief period in real time, but the machine may be
able to do much work during those few seconds.
Thus, the floppy disk driver's
.B read
routine will begin to spin up the disk, then sleep until the floppy-disk
drive signals that the disk is spinning fast enough to be read.
The process will then awaken and begin to read; in the meantime, the
\*(CO system will have been able to work productively.
When you write you driver, you should look out for such situations and use
.B x_sleep()
and
.B wakeup()
to exploit them.
.PP
.II "race condition"
Note, however, that calling
.B x_sleep()
at the wrong time will trigger a \*(QLrace condition\*(QR, which under the
wrong conditions could cause the device to hang.
The entries for
.B x_sleep()
and
.B "race condition"
in this manual's Lexicon discuss when you should use the sleep mechanism,
and when you should not.
In brief,
.B x_sleep()
is available only when the active process can be
associated with entry into the driver \(em that is, during
the functions that are invoked via system calls
.BR open() ,
.BR close() ,
.BR read() ,
.BR write() ,
.BR ioctl() ,
and
.BR poll() .
If a driver sleeps when it is not associated with a process, the kernel
will never be able to awaken it.
.II load()
.II unload()
.II block()
.II defer()
.II timeout()
Thus, a driver must not sleep 
sleep during the routines invoked by the kernel routines
.BR load() ,
.BR unload() ,
or
.BR block()
or during the functions invoked by kernel routines
.B defer()
or
.BR timeout() .
.Sh "Error Handling"
.PP
When the kernel needs to tell a process that an error condition
has occurred,
it calls function
.B set_user_error()
with the appropriate error code.
.PP
Please note that like sleeping and some other situation,
your driver can set the user error status only when
user control is valid.
A driver can sleep or call
.B set_user_error()
only from within driver functions invoked through the system calls
.BR open() ,
.BR close() ,	
.BR read() ,
.BR write() ,
.BR ioctl() ,
and
.BR poll() ,
as described above.
In particular, information specific to the calling process is
.I not
available to the
driver functions invoked via the kernel routines
.BR load() ,
.BR unload() ,
.BR block() ,
.BR defer() ,
or
.BR timeout() .
.Sh "Memory Management"
.PP
This subsection describes how the kernel manages memory, and
suggest how your driver should use memory-management to best advantage.
.PP
.II "memory^pools"
.II "kalloc() memory pool"
.II "sysmem memory pool"
\*(CO divides all of the memory that is available to the kernel
into several pools.
The two most commonly used of these are the
.B kalloc()
pool and the
.B sysmem
pool.
.PP
The
.B kalloc()
pool acts as the kernel's heap, from which the kernel allocates and
de-allocates memory for most small, temporary structures.
Use the functions
.B kalloc()
and
.B kfree()
to allocate and free memory from this pool.
These functions in the Lexicon to this manual.
.PP
The
.B sysmem
pool is a large collection of four-kilobyte ``clicks'' (memory pages).
When new processes are created, the memory they need comes from this pool.
Applications can use the functions
.B malloc()
and
.B free()
to obtain and free memory.
.PP
The kernel uses three modes of memory access:
.IR "physical memory" ,
.IR "virtual memory" ,
and
.IR "system-global memory" .
.PP
Physical-memory addresses are the addresses
set directly onto the address bus to access memory.
Some peripheral equipment, such as video boards,
communicate with the CPU through reads and writes within some specific
range of physical-memory addresses.
.II "I/O^memory mapped"
.II "memory-mapped I/O"
This arrangement is also called
.IR "memory-mapped I/O" .
.II DMA
.II "direct-memory access"
Devices that perform
.I "direct-memory access"
(DMA), including the usual AT-compatible floppy-disk controller,
also use physical-memory addresses to obtain information on the
source and destination of a DMA transfer.
.II paddr_t
A physical memory address occupies four bytes,
and is properly kept in an object of type
.BR paddr_t .
.PP
Virtual-memory addresses are those used by programs that run under \*(CO.
Paging hardware allows the operating system to remap each four-kilobyte page
of physical memory to any region of virtual memory
that is on a four-kilobyte boundary.
The kernel maintains tables that give the correspondences
between pages of physical and virtual memory.
When a context switch occurs (i.e., when one user process is shifted to the
ready queue and another is selected from the ready queue for execution),
the kernel updates these tables to unmap pages of memory for the old process,
and to map pages of memory into virtual addresses for the new process.
Virtual-memory addresses occupy four bytes, and are properly stored in type
.BR vaddr_t .
.PP
System global memory provides a third way of viewing memory resources.
.II "I/O^raw"
.II "raw I/O"
Consider a device driver that supports raw I/O to a block device.
The term
.I raw
implies that the user process supplies an I/O buffer of arbitrary length.
This introduces a problem:
when the device driver is ready to transfer data between the user buffer and
the I/O device, the user buffer may not be mapped into virtual memory.
A
.I "system global"
address is a 32-bit object that retains access to the buffer,
even when it is paged out of virtual memory.
.PP
Only a few operations are valid on system-global addresses:
.IP \(bu 0.3i
Add or subtract an offset.
.IP \(bu
.II P2P()
Apply macro
.B P2P()
to obtain the corresponding physical address.
.IP \(bu
.II dmain()
.II dmaout()
Appear as the second argument of kernel routine
.B dmain()
or
.BR dmaout() .
.IP \(bu
.II xcopy()
Appear as the second argument (destination) in a call to kernel routine
.BR xpcopy() 
.IP \(bu
.II pxcopy()
Appear as the first argument (source) in a call to kernel routine
.BR pxcopy() .
.PP
In the last two cases, the final argument in the
function call must be manifest constant
.BR "SEG_386_KD|SEG_VIRT" .
For example:
.DM
	char src[BSIZE];
	char dest[BSIZE];
.sp \n(pDu
	/* copy from kernel data to system global buffer */
	xpcopy(src, bp->b_paddr, BSIZE, SEG_386_KD|SEG_VIRT);
.sp \n(pDu
	/* copy from system global buffer to kernel data */
	pxcopy(bp->b_paddr, dest, BSIZE, SEG_386_KD|SEG_VIRT);
.DE
To get a system global address, use macro
.BR MAPIO() .
This function takes two arguments, the segment table address
(obtained from the segment-reference structure [SR]),
and the offset within the segment.
.PP
For objects in the kernel kalloc pool, use
.B allocp()
as the SR.
For objects in user space for the *current* process, the SR is
\fBu.u_segl[\fIxxx\^\fB]\fR, where
.I xxx
is the segment index, e.g.,
.B SIPDATA
or
.BR SISTACK .
The user stack segment grows down, so the lower limit of the
stack segment is computed differently than for the other user segments.
See the example, below.
.PP
If your buffer has virtual address
.IR v ,
do the following.
If
.I v
is in kalloc pool, use the following:
.DM
	sgAddr = MAPIO(allocp.sr_segp->s_vmem, v - allocp.sr_base)
.DE
If
.I v
is in user data, then:
.DM
	sgAddr = MAPIO(u.u_segl[SIPDATA].sr_segp->s_vmem,
		v - u.u_segl[SIPDATA].sr_base)
.DE
If
.I v
is in user stack:
.DM
	sgAddr = MAPIO(u.u_segl[SISTACK].sr_segp->s_vmem,
		v - u.u_segl[SISTACK].sr_base - u.u_segl[SISTACK].sr_size)
.DE
.PP
For devices that support block I/O,
a block routine takes a single argument, a pointer to a
.B BUF
structure.
.II b_paddr
The field
.B b_paddr
of this
.B BUF
structure is a system global address.
.PP
Finally, please note that drivers that use the
DDI/DDK interface will not use
.B kalloc()
or
.BR kfree() .
They will use statically allocated structures sized by tunable
parameters, or they will use the specialized resource-allocation routines
that the DDK provides.
.SH "Where To Go From Here"
.PP
The next two sections give, respectively, an example driver for a block
device and an example driver for a character device.
Each uses the internal-kernel interface.
We suggest that you study these examples carefully before you attempt to
write a driver on your own.
Finally, the Lexicon in this manual describes
the functions and macros introduced in this section.
