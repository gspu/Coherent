.TH tr "" "" "Device Driver"
.PC "Driver to read stored error messages"
.B /dev/trace
.PP
.II /dev/trace
.II "driver^kernel traceback"
.II "device driver^kernel traceback"
.II "kernel^traceback, driver"
The device driver
.B tr
is the ``traceback'' driver for the \*(CO kernel.
It manipulates an internal buffer that holds error messages from the
kernel or another device driver.
It has major number 6.
This driver is extremely useful to persons who are writing device drivers.
.PP
The DDI/DKI kernel routine
.B cmn_err()
can be invoked by drivers to write formatted messages.
By default, it writes the messages both onto the system's console and
into an internal buffer in memory that can hold up to
four megabytes of text.
Messages that begin with a caret, `^', go to the
console but not the internal buffer.
Messages that begin with an exclamation
point, `!', go to the buffer but not to the console.
.PP
The trace driver
.B tr
reads this internal buffer, and lets you copy its contents into a file
for later perusal.
This offers two major
advantages to persons developing and debugging device drivers:
.IP \(bu 0.3i
First, copious diagnostic output will no longer scroll off the screen and
be lost.
The trace buffer holds every error message written through
.B cmn_err()
until you read the buffer.
When you install
.BR tr ,
you can set the size of the buffer, up to four megabytes.
.IP \(bu
Second, if messages are written to the trace buffer only and not to the
console,
system timing is affected much less than if the messages were
written to the console.
This makes it easier to catch subtle problems in timing.
.PP
To add
.B tr
to your kernel, do the following:
.IP \(bu 0.3i
Log in as the superuser
.BR root .
.IP \(bu
.B cd
to directory
.BR /etc/conf .
.IP \(bu
Execute script
.BR tr/mkdev .
This will walk you through the process of configuring your kernel to use
this driver, and create the device
.BR /dev/trace .
.IP \(bu
Execute script
.BR bin/idmkcoh ,
to generate a new kernel.
.IP \(bu
Invoke the script
.B /etc/shutdown
to shutdown system, then boot the new kernel.
.PP
To read the contents of the trace buffer, simply use the command
.DM
	cp /dev/trace \fIfile\fP
.DE
.PP
where
.I file
is the file into which you wish to copy the contents of the trace buffer.
.SH "See Also"
.Xr "device drivers" device_dr
.br
\fICOHERENT Device Driver Kit:\fR
.Xr "cmn_err()" cmn_err
