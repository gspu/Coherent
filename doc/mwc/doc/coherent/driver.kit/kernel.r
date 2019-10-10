.ds TL "Kernel Internals"
.NH "The COHERENT Kernel"
.PP
.II "kernel^description"
This section sketches the \*(CO kernel.
It also discusses defensive programming practices,
how to rebuild the kernel to include a newly written driver, and how to
interpret a dump from a failed driver.
.PP
The kernel is the program that permanently resides in memory
to control the moment-to-moment operation of \*(CO.
It works by controlling \fIprocesses\fR and \fIdevices\fR.
The following sub-sections introduce each.
.Sh "Processes and Scheduling"
.PP
.II process^definition
A
.I process
is a program that is being executed.
The kernel ``slices'' the processor's time amongst all processes,
which creates the illusion that \*(CO is running many programs simultaneously.
.PP
As you can see, managing processes \(em that is, seeing that each process
receives its share of the processor's time \(em is one of the kernel's main
tasks.
To do this, the kernel creates two queues whose entries describe every
process that the kernel has been asked to execute.
.II "queue^ready"
.II "ready queue"
One, the
.I ready
queue, describes every process that is ready to be processed further by
the microprocessor.
.II "suspended queue"
.II "queue^suspended"
The other, the
.I suspended
queue, describes every process that is waiting for something to happen;
for example, a word-processing program that is awaiting a keystroke
is placed on the suspended queue.
.PP
The kernel selects a process from the ready queue, then executes it
either until it has reached a stopping
point or until it has exhausted the slice of time allotted to it.
If the process has exhausted its slice of time,
the kernel moves it to the end of the ready queue.
.II sleep^definition
If, however, the process is awaiting an event,
the kernel moves it to the suspended queue.
A process on the suspended queue is said to be
.IR sleeping .
In either case,
the kernel saves the current state of the process,
then selects the next process from the ready queue and executes it.
.PP
.II "external events"
When an external event occurs (e.g., the user presses a key), the kernel
searches the suspended queue for a process that is awaiting that event.
If it finds one, the kernel moves it to the ready queue, where it waits
its turn to be executed further.
This continues until all processes have run to completion.
.PP
Before release 4.2.12 of \*(CO, the kernel selected processes from the
ready queue sequentially \(em that is, it executed each process in turn,
regardless of that process's demands upon the system.
With this method of scheduling processes, running a computation-intensive
program under \*(CO (e.g., a compiler) would degrade the system's response
to user input from the keyboard to the point where the system was nearly
useless.
.PP
Beginning with release 4.2.12 of \*(CO, the scheduler has been redesigned
to maximize the number of processes executed.
The scheduler now gives processes that make light demands on the system
(e.g., a user's input from a keyboard)
a higher priority than it gives processes that
make heavy demands on the system (e.g., image processing or compiling).
The new scheduler now permits a user to compile a program in the background
or run a tape backup, yet continue to type input from the console or from
a terminal with no noticeable degradation of response to the keyboard.
The compile will take slightly longer than it would have otherwise, but
the overall usefulness of the system is greatly increased.
.PP
.II "timeout functions"
.II "deferred functions"
In addition to managing processes, the kernel gives timeout functions,
deferred functions, and device drivers sequential, uninterrupted access
to the CPU.
The kernel intersperses
the slices of time it assigns to user processes amongst
the demands of these lower-level routines.
Any time the kernel is about to give the CPU to a user process,
such as after completing a system call, it first checks if
any timeouts or deferred functions are waiting to be called;
if there are, it executes them first.
.SH "Devices"
.PP
.II "device^definition"
A
.I device
is a piece of hardware with which a process must communicate, e.g.,
physical memory, a hard-disk drive, a floppy-disk drive, or a serial port.
Note that, unlike under \*(MD, \*(CO does not permit a program
to access hardware directly:
the \*(CO kernel manages all transfers of data between a process and a device.
.PP
.II "device^block special"
.II "device^character special"
.II "character-special device"
.II "block-special device"
Devices come in two types:
.I character-special
and
.IR block-special .
A
.I "character-special device"
is one with which \*(CO exchanges data one character at a time.
This class of devices includes serial and parallel ports and the console.
.II block
A
.I "block-special device"
is one with which \*(CO exchanges data one block at a time.
\*(CO defines a
.I block
as being 512 bytes (one-half kilobyte).
This class of devices includes the hard disk and the floppy disk.
The size of a block is defined by constant
.B BSIZE
in header
.BR <sys/buf.h> .
.PP
Note that the \*(CO system, unlike most editions of \*(UN,
allows a device driver
to be accessed in either block-special or character-special mode.
For example, under \*(CO both hard disks and floppy disks can be accessed
in either character or block mode.
.II "raw device"
.II "cooked device"
.II "device^raw"
.II "device^cooked"
If a device can be accessed in either mode, its character mode is sometimes
called the
.I raw
device (shown by the fact that its name begins or ends with the letter `r'),
whereas its block mode is sometimes called the
.I cooked
device.
This will be detailed below.
.PP
The kernel uses the structure
.B IO
to manage communication with a device.
It is defined in the header file
.BR <sys/io.h> ,
and includes the following fields:
.IP \fBio_seek\fR 1.0i
Number of bytes from the beginning of the file or device where the driver
should begin to read.
This is, of course, is meaningless for devices for devices like serial ports.
.II BSIZE
In the case of disk drives, this number must indicate the block to be read,
i.e., the number must be evenly divisible by constant
.BR BSIZE ,
which gives the size of a \*(CO block.
If this is not true, an error has occurred.
.IP \fBio_ioc\fR
Number of bytes to read or write.
When the data movement is completed, this should be set to the number
of bytes that remain to be read or written.
If it is not reset to zero, an error has occurred.
.IP \fBio_base\fR
Offset of data to be transferred in the user memory space.
This is converted to a physical or virtual memory address before
performing the read.
.IP \fBio_flag\fR
Flags.
.II io.h
See header file
.B <sys/io.h>
for the flags recognized by \*(CO.
.B IONDLY
indicates that the request is non-blocking.
.Sh "Buffer Cache"
.PP
.II "buffer cache"
A buffer cache is an area of RAM that holds data being written to
or read from a block-special device.
The kernel gives each block-special device its own buffer cache.
The kernel, in turn, assigns to each buffer cache a
.B BUF
structure, which the kernel uses to manipulate that buffer cache.
It is defined in the header file
.BR <sys/buf.h> ,
and includes the following fields:
.IP \fBb_dev\fR 1.0i
A
.B dev_t
structure that describes the device being buffered.
.II major()
.II minor()
The internal kernel macros
.B major()
and
.B minor()
translate this structure into the device's major and minor numbers.
.IP \fBb_req\fR
Type of I/O request, either
.B BREAD
or
.BR BWRITE .
.IP \fBb_bno\fR
The number of the starting block.
.IP \fBb_paddr\fR
The system global (DMA) address for the data.
.IP \fBb_count\fR
The number of bytes to read or write.
.IP \fBb_resid\fR
The number of bytes remaining to be transferred.
A value of zero indicates that all data transferred correctly, i.e., that
an error did not occur.
.SH "Interrupts"
.PP
.II interrupt^definition
Most devices gain the attention of the kernel by sending an
.IR interrupt ,
which is a hardware signal by which the device indicates
that it needs attention.
.PP
.II "interrupt vector"
The kernel assigns a unique pointer, or
.IR "interrupt vector" ,
to each device that uses interrupts.
.II "interrupt handler"
A device's interrupt vector points to a function, or
.IR "interrupt handler" ,
that is designed to service the device.
The kernel stores its table of interrupt vectors at the beginning
of main memory.
.PP
When \*(CO receives the interrupt from a device,
it saves the state of the process that is being executed.
It then reads the table of interrupt vectors and finds the vector for the
interrupt it has just received; then jumps to the handler to which the
interrupt vector points, and executes it.
Executing the interrupt handler may require awakening some sleeping processes.
.PP
When the interrupt handler has finished its work, the kernel
reloads the saved state of the interrupted process into the CPU,
and resumes processing it as if nothing had happened.
.SH "Device Drivers"
.PP
.II "device driver"
A
.I "device driver"
is the software that the kernel uses to communicate with a device.
Each device must have its own driver.
.PP
A driver stands between the kernel and the physical hardware:
.B1
	    Device:
	   I/O ports         \(<-   device   \(<-      COHERENT
	memory mapped I/O    \(->   driver   \(->       kernel
	      DMA
.B2
.PP
As the above
.\" crude
diagram shows, the driver
receives input from the hardware and writes output to it;
and it receives input from the kernel and writes output to it.
.Sh "Communication With an Application"
.PP
.II "device file"
.II mknod
.II "special file"
A user-level application communicates with a driver
via a special file called a
.I "device file"
or a
.IR "special file" .
A driver can have any number of special files.
Its suite of special files can access different devices of the same type
(for example, different floppy-disk drives or different partitions on a
hard disk); or can access the same device in different modes
(for example, a tape
device can be accessed in either rewind or non-rewind mode).
A special file is created with the command
.BR mknod .
.II /dev
Most device files are kept in directory
.BR /dev ;
if you execute the command
.B "ls -l"
on
.BR /dev ,
you will see a set of listings that appear something like the following:
.DM
.ta 1.2i 1.6i 2.1i 2.6i 3.1i 3.4i 4.6i 5.5i
\fI   1	2	3	4	5	6	     7	   8	   9\fP
brw-------	1	sys	sys	11	0	Fri Apr 27	16:56	at0a
brw-------	2	sys	sys	11	1	Fri Apr 27	16:56	at0b
brw-------	1	sys	sys	11	2	Fri Apr 27	16:56	at0c
brw-------	2	sys	sys	11	3	Fri Apr 27	16:56	at0d
brw-------	1	sys	sys	11	4	Fri Apr 27	16:56	at1a
brw-------	1	sys	sys	11	5	Fri Apr 27	16:56	at1b
brw-------	1	sys	sys	11	6	Fri Apr 27	16:56	at1c
brw-------	1	sys	sys	11	7	Fri Apr 27	16:56	at1d
crw-rw-rw-	1	bin	bin	 5	0	Fri Apr 27	16:56	com1r
crw-rw-rw-	3	bin	bin	 6	8	Sat Aug 18	12:57	com2
crw-rw-rw-	3	bin	bin	 6	8	Sat Aug 18	12:57	com2l
crw-rw-rw-	1	bin	bin	 6	0	Fri Apr 27	16:56	com2r
.DE
The listing consists of nine fields, as follows:
.IP \fB1.\fR 0.3i
Permissions.
The first character in the permissions field indicates what type of
device this is:
.B b
indicates a block-special (cooked) device, and
.B c
indicates a character-special (raw) device.
.IP \fB2\fR
Number of links to the file.
.IP \fB3\fR
The login identifier of the user who owns the file.
.IP \fB4\fR
The group identifier of the user who owns the file.
.IP \fB5\fR
.II "major-device number^definition"
Major-device number.
This is a unique number that identifies the device driver to the kernel.
The kernel can handle up to 32 devices at any given time,
numbered zero through 31.
The Lexicon article
.B "device drivers"
gives a table of all device drivers current recogized by the
\*(CO system, and the major-device number of each.
.IP \fB6\fR
Minor-device number.
.II "minor-device number^definition"
In addition to a type and a major-device number, each device file has a
minor-device number.
This allows \*(CO to distinguish among a number of devices of the same type.
For example, this table shows that major number 11 indicates
the AT hard disk.
The above listing shows ten device files with this major-device number 11,
five for device
.B at0
(which supports drive 0), and five for
.B at1
(which supports drive 1).
Files ending in
.B a
through
.B d
each support one partition on the drive;
the file ending in
.B x
supports that drive's partition table.
Each of these device files has a unique minor device number, to allow
the kernel to tell them apart.
.IP \fB7\fR
Date last modified.
.IP \fB8\fR
Time last modified.
.IP \fB9\fR
Name of file.
.Sh "Communication With the Kernel"
.PP
The kernel communicates with the driver through
.I "entry points"
within the driver.
If a user application invokes a system call for a given device (e.g.,
.B open()
or
.BR close() ,
the kernel in turn invokes the appropriate entry point within the driver.
.PP
The method by which entry points are defined within a driver varies,
depending upon whether the driver uses the DDI/DKI interface, or the
internal-kernel interface.
For details, see the entry for
.B "entry points"
within this manual's Lexicon.
.SH "Rebuilding the Kernel"
.PP
.II "device driver^add a new one"
The following walks you through the processing of adding a new driver.
We will add the driver \fBfoo\fR, which drives the popular ``widget'' device.
.IP \fB1.\fR 0.3i
To begin, log in as the superuser
.BR root .
.IP \fB2.\fR
Create a directory to hold the driver's sources and object.
Every driver must have its own directory under directory
.BR /etc/conf ;
and the sources must be held in directory
.B src
in that driver's directory.
In this case, create directory
.BR /etc/conf/foo ;
then create directory
.BR /etc/conf/foo/src .
.IP \fB3.\fR
Copy the sources for the driver into its source directory; in this case,
copy them into
.BR /etc/conf/foo/src .
.IP \fB4.\fR
Compile the driver.
This should create one object file that has the suffix \fB.o\fR.
Copy this file in the driver's home directory, and name it
.BR Driver.o .
In this case, the object for the
driver should be in file
.BR /etc/conf/foo/Driver.o .
.IP
In some rare cases, a driver compile into more than one object.
Use the command
.B ar
to store the objects into one archive called
.B Driver.a ,
and store the archive in the driver's home directory.
The \*(CO commands that build the new kernel know how to handle archives
correctly.
.IP
You may wish to add a
.B Makefile
for your driver, so that it will be recompiled, as needed,
whenever the command
.B idmkcoh
is run.
For a sample
.BR Makefile ,
see the various
.B src
subdirectories under
.BR /etc/conf .
.IP \fB5.\fR
Add an entry to file
.B /etc/conf/mdevice
for the new driver.
This file is a little more complex than
.BR sdevice ;
in particular, it distinguishes between \*(ST-style drivers and
``old-style'' \*(CO drivers.
In most cases, you can simply copy an entry for an existing driver of the
same type, and modify it slightly.
In this case, the entry for
.B foo
should read as follows:
.DM
	# full	func	misc	code	block	char	minor	minor	dma	cpu
	# name	flags	flags	prefix	major	major	min	max	chan	id
	foo	-	CGo	foo	15	15	0	255	-1	-1
.DE
.IP
In almost every case, the full name and the code prefix are identical.
The code prefix also names the directory that holds the driver's object.
Function flags are always always a hyphen, and miscellaneous flags almost
always
.BR CGo .
The block- and character-major numbers again are almost always identical.
The major number is usually assigned by the creator of the device driver.
In future releases of the kernel, these will be assigned dynamically by the
kernel itself; poorly written drivers that depend upon the driver having
a magic major-device number will no longer work.
Finally, the last four columns for non\-\*(ST drivers are almost always
0, 255, \-1, and \-1, respectively.
.IP \fB6.\fR
Add an entry to file
.B /etc/conf/sdevice
for this driver.
.BR sdevice ,
names the drivers to be included in the kernel.
The entries for practically every entry are identical; you need to note only
that the second column marks whether to include the driver in the kernel.
In this case, the entry for the driver
.B foo
should read as follows:
.DM
	foo	Y	0	0	0	0	0x0	0x0	0x0	0x0
.DE
.IP
For details on what each column means, read the comments in file
.BR /etc/conf/sdevice .
.IP \fB7.\fR
If the driver has tunable variables, set them in file
.BR Space.c ,
which should be stored in the driver's home directory.
As it happens,
.B foo
does not need a
.B Space.c
file.
For examples of such files, look in the various sub-directories of
.BR /etc/conf .
If the driver has tunable variables, you must a line for each
to file
.B /etc/conf/mtune
and (optionally) to
.BR /etc/conf/stune .
.IP
For example, suppose that the driver
.B foo
uses an array of
.B foo_info
structures, and that the number of these used is tunable.
Then
.B Space.c
might contain these lines:
.DM
	#include "conf.h"
	#include <sys/foo.h> /* defines struct foo_info */
		...
	extern int num_foo_info = NUM_FOO_INFO;
	extern struct foo_info foo_table[NUM_FOO_INFO];
.DE
.IP
.B /etc/conf/mtune
sets the range of legal values and the default value for
.BR NUM_FOO_INFO .
If you want to set
.B NUM_FOO_INFO
to some value other than the default, then pop an
entry for that variable into
.BR /etc/conf/stune .
.IP
In
.B mtune
and
.BR stune ,
the name given is not that of the tunable variable, but of an enumeration
constant used to initialize that variable.
.B idmkcoh
automatically generates
the header that defines this constant as needed.
.IP \fB8.\fR
Type the command
.B /etc/conf/bin/idmkcoh
to build a new kernel.
If necessary, move the new kernel into the root directory.
.IP \fB9.\fR
Save the old kernel and link the newly build kernel to
.BR /autoboot .
You want save the old kernel, just in case the new one doesn't work.
For directions on how to boot a kernel other than
.BR /autoboot ,
see entry for
.B booting
in the \*(CO Lexicon.
.IP \fB10.\fR
Back up your files!
With a new driver in your kernel, it's best to play it safe.
.IP \fB11.\fR
Reboot your system to invoke the new kernel.
If all goes well, you will now be enjoying the services of the new device
driver.
.PP
Because of space restrictions in the boot procedure,
the newly generated kernel object (e.g.,
.BR /testcoh )
does not contain a complete symbol table:
values for some symbols instead are stored in the ASCII
.B .sym
file (e.g.,
.BR /testcoh.sym ).
The debugger
.B db
can load symbols from a
.B .sym
file if you use its command-line option
.BR \-a .
.II keeplist
.II /etc/conf/install_conf/keeplist
To force a specific symbol to be included within the kernel object
(notably, to make the symbol patchable within the generated kernel),
add the symbol's name to a
.B \-I
option that is output by the file
.BR /etc/conf/install_conf/keeplist .
.SH "Debugging Program Crashes"
.PP
The following describes the \*(CO system's run-time environment.
If your program dies and generates a register dump of the following
form on the system console, this information will help you interpret
what appears on the console.
.PP
When a program dies an untimely death, it dumps information about
the contents of registers and memory onto the console device.
The registers shown are the 386/486 family registers.
All registers that begin with `e' and that are three characters long are
32-bit registers.
Refer to any 386/486 hardware/programming documents for
further details on register specifics.
.PP
Register
.B eip
is the instruction pointer.
This contains the user program's address at the time that
the fault or violation occurred.
Register
.B uesp
is the user-stack pointer.
.B cmd
specifies the command that faulted.
.PP
The following gives a sample dump of registers:
.DM
eax=32  ebx=408080      ecx=40541E      edx=0                 
esi=0   edi=8BF84589    ebp=7FFFF694    esp=FFFFFFE4
cs=B    ds=13           es=13           ss=13
err #14 eip=1C34F       uesp=7FFFF664   cmd=my_cmd
efl=13282       cr2=8BF84589    sig=11  trap_ip=FFC0079F
trapcode=4      User Trap                                     
segmentation violation -- core dumped                                           
.DE
Note that the format differs a little from that shown above.
The dump also gives a traceback readout of addresses in the kernel;
the addresses shown in the backtrace can be looked up file
\fIkernel\fB.sym\fR, where
.I kernel
names the kernel that you were running when the dump occurred.
This is discussed below in more detail.
.PP
The following gives the memory map for iBCS2 under \*(CO:
.DM
       0	-	  3fffff	program text	(fixed size)
  400000	-	 13fffff	program data	(grows upward)
7FC00000	-	7fffffff	program stack	(grows downward)
FFC00000	-	ffffffff	kernel
.DE
Examination of this dump uncovers the following clues about what killed
your program:
.IP \(bu 0.3i
Error 14 indicates a page fault.
The address that generated the fault is in register
.BR cr2 .
.IP \(bu
From the fact that
.B cr2
is an invalid address that matches
.BR edi ,
you can surmise that the program attempted to move a block
or string to an illegal address.
.IP \(bu
The base pointer
.B ebp
points into user stack, as expected.
.IP \(bu
.BR esp ,
the stack pointer,
points into the kernel stack (the top four kilobytes), also as expected.
.IP \(bu
The lower two bits for all the segment registers (\fBcs-ss\fR) are set, so the
program was in user mode.
If all had had their lower bits set,
the dump probably would indicate a panic.
.IP
Header file
.B <sys/reg.h>
defines the error numbers as follows:
.DS
	\fBSIDIV\fR	0	Divide overflow
	\fBSISST\fR	1	Single step
	\fBSINMI\fR	2	NMI (parity)
	\fBSIBPT\fR	3	Breakpoint
	\fBSIOVF\fR	4	Overflow
	\fBSIBND\fR	5	Bound
	\fBSIOP\fR	6	Invalid opcode
	\fBSIXNP\fR	7	Processor extension not available
	\fBSIDBL\fR	8	Double exception
	\fBSIXS\fR	9	Processor extension segment overrun
	\fBSITS\fR	10	Invalid task state segment
	\fBSINP\fR	11	Segment not present
	\fBSISS\fR	12	Stack segment overrun/not present
	\fBSIGP\fR	13	General protection
	\fBSIPF\fR	14	Page fault
	\fBSISYS\fR	32	System call
	\fBSIRAN\fR	33	Random interrupt
	\fBSIOSYS\fR	34	System call
	\fBSIDEV\fR	64	Device interrupt
.DE
Register
.B eip
points into user text space.
To see what the kernel was doing at the time of the crash, you must
check the
.B eip
address and any backtrace addresses against the known kernel text addresses.
Most symbols for any kernel are in its associated
.B .sym
file.
You should sort this before you look up addresses; then you can look up
the address with
.B grep
or a text editor.
Some symbols remain within the kernel executable.
You can use the command
.B "nm -n"
to read these.
The entry points within the
.B CON
structure usually are defined as static, but may be located using the command
.BR /etc/conf/bin/drvldump .
.IP \(bu
Register
.B uesp
is a valid user-stack value.
Note that it is below
.BR ebp .
This is a normal consequence of iBCS2 calling conventions.
.IP \(bu
.B cmd
matches the program you are debugging, indicating that it is
indeed that command and not something it calls.
.IP \(bu
You can pick apart the flag register
.B efl
with your i386 reference manual.
.IP \(bu
The signal number
.B sig
tells you what signal, if any, killed the process.
Header file
.B signal.h
defines signal numbers as follows:
.DS
.ta 0.5i 1.5i 2.0i
	\fBSIGHUP\fR	1	Hangup
	\fBSIGINT\fR	2	Interrupt
	\fBSIGQUIT\fR	3	Quit
	\fBSIGILL\fR	4	Illegal instruction
	\fBSIGTRAP\fR	5	Trace trap
	\fBSIGIOT\fR	6	IOT instruction
	\fBSIGEMT\fR	7	Emulator trap
	\fBSIGFPE\fR	8	floating point exception
	\fBSIGKILL\fR	9	Kill
	\fBSIGBUS\fR	10	Bus error
	\fBSIGSEGV\fR	11	Segmentation violation
	\fBSIGSYS\fR	12	Bad argument to system call
	\fBSIGPIPE\fR	13	Write to pipe with no readers
	\fBSIGALRM\fR	14	Alarm
	\fBSIGTERM\fR	15	Software termination signal
	\fBSIGUSR1\fR	16	User signal 1
	\fBSIGUSR2\fR	17	User signal 2
	\fBSIGCLD\fR	18	Death of a child
	\fBSIGCHLD\fR	18	Death of a child
	\fBSIGPWR\fR	19	Restart
	\fBSIGWINCH\fR	20	Window change
	\fBSIGPOLL\fR	22	Polled event in stream
.DE
.IP \(bu
.B trap_ip
is in kernel text space, as expected.
This will not tell you much unless you are debugging the kernel.
.IP \(bu
The trap code can safely be ignored.
.SH "Where To Go From Here"
.PP
The next section describes in more detail the structure of a device
driver.
It emphasizes the internal-kernel interface, which is that used by
practically all of the drivers shipped with \*(CO 4.2.
The following two sections give examples of drivers for, respectively,
a block device and character device.
.PP
See the entry
.B "device driver"
in this manual's Lexicon for a description of what device drivers are
available with the device-driver kit, and of the articles available in the
Lexicon.
