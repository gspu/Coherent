.ds TI "Floppy Tape for COHERENT"
.ds TL "Release Notes"
.NH "COHERENT Update for Floppy Tape"
.PP
These notes describe how to install the new \*(CO driver
for QIC-40/QIC-80 (``floppy tape'') devices.
This driver is in a kit that is on the single floppy disk included with
these notes.
.SH "Before You Begin"
.PP
Before you begin installation of this kit,
please note that you must have \*(CO release 4.2.10 or later to use the
\*(CO floppy-tape driver.
If you do not yet have \*(CO 4.2.10, send e-mail to
.BR updates@mwc.com .
You will be sent by a copy without charge.
If you do not have e-mail, write to
.B Support
at the address given in the \*(CO manual.
.PP
Note, too, that installing this disk replaces the contents of directory
.B /etc/conf
and its sub-directories.
You should use the command
.B gtar
or
.B cpio
to back up this directory before you install this kit.
The installation script backs up this directory,
but it never hurt to be doubly sure.
.PP
Finally, copy the kernel that you boot by default into a backup file.
This will let you back out from this kit should something go wrong.
.SH "Installing the Kit"
.PP
To install the kit, log in as the superuser
.BR root ,
or use the command
.B "shutdown single 0"
to bring the system down to single-user mode.
Then run one of the following versions of the command
.BR install .
Which version you run depends upon whether you are installing from
floppy-disk drive A or B, and whether that drive is 3.5 or 5.25 inches,
as follows:
.IP "3.5-inch Drive A"
If you are installing from a 3.5-inch drive 0 (drive A),
use this command:
.DM
	/etc/install -c ft_1 /dev/fva0 1
.DE
.IP "3.5-inch Drive B"
If you are installing from a 3.5-inch drive 1 (drive B),
use this command:
.DM
	/etc/install -c ft_1 /dev/fva1 1
.DE
.IP "5.25-inch Drive A"
If you are installing from a 5.25-inch drive 0 (drive A),
use this command:
.DM
	/etc/install -c ft_1 /dev/fha0 1
.DE
.IP "5.25-inch Drive B"
If you are installing from a 5.25-inch drive 1 (drive B),
use this command:
.DM
	/etc/install -c ft_1 /dev/fha1 1
.DE
.PP
After it copies the bits from the floppy disk,
the installation script invokes the command
.B /etc/conf/bin/idbld
to link a new \*(CO kernel, called
.BR /coh.ft .
It then links this kernel to
.B /autoboot
so that it is loaded by default when you boot your system.
If you want to save your old default kernel,
copy it into a backup file.
For example, if your default kernel is named
.BR /coherent ,
use the command:
.DM
	cp /coherent /coherent.bak
.DE
Please note that must be the superuser
.B root
to execute this command.
.PP
Command
.B idbld
asks about how your system is configured.
Because installing this update disk replaces the directory
.B /etc/conf
and all of its sub-directories,
you must enable the hard-disk device driver (or drivers) for your system,
and repeat the system configuration you specified when you last installed
or updated \*(CO.
A good test to see whether the new system is
configured the same as before is to look at files
.B /etc/conf/sdevice
and
.B /etc/conf/stune
both before and after the update, and make sure any
old entries have not changed inadvertently.
.PP
After you have installed, we suggest that you test the tape drive, as follows:
.IP \(bu 0.3i
Try making several archives of varying size on tape.
The utilities
.B gtar
and
.B cpio
are optimally suited for this.
.IP \(bu
Try concatenating several archives onto the same tape.
The manual page for
.BR backups ,
which is included with these notes, describes how to do this.
.IP \(bu
Use the
.B \-d
option with
.B gtar
to verify the contents of the archive.
.PP
For further information, see the attached note and the revised manual pages
provided on the floppy disk.
.Sh "Documentation"
.PP
The following pages give printed versions of updated manual pages for
the commands
.B tape
and
.BR ftbad ,
for technical information on tape and backup procedures, and for the driver
.BR ft .
