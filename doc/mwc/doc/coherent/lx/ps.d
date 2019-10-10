.TH ps "" "" "Device Driver"
.PC "Driver to return information about processes"
.B /dev/ps
.PP
.II /dev/ps
.II "kernel^process table"
.II "process table"
.II "driver^process table"
.II "device driver^process table"
The file
.B /dev/ps
accesses the kernel's process table.
It is a part of the driver
.BR mem ,
which manages memory; thus, it has major number 0 and minor number 6.
.PP
.B /dev/ps
is a read-only device that exists only to support the command
.B ps
and its variants.
The command
.B ps
reads this device to display a ``snapshot'' of the
processes that the \*(CO kernel is executing.
.PP
Reading
.B /dev/ps
deposits an array of the structure
.B stMonitor
into the read buffer.
The number of bytes requested by the system call
.B read()
should be enough to accommodate the entire process table.
Header file
.B <sys/coh_ps.h>
defines
.BR stMonitor .
.SH "See Also"
.Xr "device drivers," device_dr
.Xr "ps [command]" ps
