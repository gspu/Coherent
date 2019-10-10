.TH uname "" "" Command
.PC "Print information about \*(CO"
.B "uname [ \-amnrsv ]"
\fBuname [ \-S \fIsystemname\fB ]\fR
.PP
The command
.B uname
prints information about the current implementation of \*(CO.
It recognizes the following options:
.IP \fB\-a\fR 0.3i
Print all information.
.IP \fB\-m\fR
Print the machine on which this implementation of \*(CO is running.
This always defaults to the Intel 80386.
.IP \fB\-n\fR
Print the name of your system, as set in the file
.BR /etc/uucpname .
.IP \fB\-r\fR
Print the release of your copy of \*(CO.
.IP \fB\-s\fR
Print the system name.
.IP \fB\-S\fR
Change the system name.
.I systemname
is restricted to eight characters.
.IP \fB\-v\fR
Print the version of \*(CO.
.SH Example
The following script uses
.B uname
to implement a version of the Sun OS command
.BR hostname .
.II "Schubert, Cy"
It is by Cy Schubert (cschuber@bcsc02.gov.bc.ca):
.DM
	#!/bin/sh -
	# hostname - display or change the name of the host system
	case $# in
	    0)    uname -n;;
	    1)    uname -S $1;;
	    *)    echo Usage: hostname [new_hostname]
	          exit 1;;
	esac
.DE
.SH "See Also"
.Xr "commands" commands
