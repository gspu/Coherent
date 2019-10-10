.ds TL Errata
.NH "READ THIS: Supplement to Release Notes"
.PP
The following describes errors in the release notes, and known
bugs in \*(CO 4.2.
.IP \(bu 0.3i
Due to a production error,
the character
.sp \n(pDu
	\*(RT
.sp \n(pDu
was rendered as:
.DM
	(\XA2)
.DE
If possible, please try to remember that the cent sign actually means
that you should press the \fB<Return>\fR key.
.IP \(bu
The section on third-party products implies the GNU C and GNU C++
are two products.
In fact, there is only one product, GNU C;
C++ is part of the package.
.IP \(bu
The price of GNU C is actually $79.95, not $49.95 as listed in the
release notes.
.IP \(bu
The price of COHware volumes IV and V is $49, not $40 as listed in the
release notes.
.IP \(bu
Udo Munk's e-mail address is \fBudo@umunk.gun.de\fR.
Axel Reinhold's e-mail address is incorrect and should not be used.
.IP \(bu
In the section on running third-party packages,
the description of selecting and de-selecting printers for WordPerfect
is no longer valid.
Printer selection now works as it should.
.IP \(bu
Command
.B fdformat
does not work with its \fB\-v\fR (verify) option if you are formatting
the block device.
It does work, however, if you use the character (raw) device.
For example, command
.DM
	fdformat -v /dev/rfva0
.DE
works, but command
.DM
	fdformat -v /dev/fva0
.DE
does
.I not
work.
This will be corrected in the near future, and a repaired version of
.B fdformat
will be posted to the MWC BBS and the Internet.
.IP \(bu
Some users who update will find that their systems retain two versions
of
.BR tar :
one in directory
.B /usr/bin
and the other in directory
.BR /bin .
Command
.B /bin/tar
will have been left over from your previous installation of \*(CO;
it has been superceded by command
.BR /usr/bin/tar ,
which is superior.
You should rename command
.B /bin/tar
to
.BR /bin/tar.old
to ensure that you use the improved
.B tar
command for your backups.
.SH "Before and After You Update"
.PP
The following gives helpful hints that you should consider before
you update to \*(CO 4.2.
.IP \(bu 0.3i
Before you update, be sure your file system is intact.
Run \fBfsck\fR on your root file system, at least, before you attempt
to update.
.IP \(bu
When you install third-party packages, we recommend that you run in
single-user mode.
This will considerably lessen the complications of installation.
.PP
After you install \*(CO, the following describes how to update your
system to enable new devices:
.IP \(bu 0.3i
To install or de-install a parallel printer:
.DM
	cd /etc/conf
	lp/mkdev
	/conf/mlpconfig
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
.IP \(bu
To install STREAMS or floating-point emulation:
.DM
	cd /etc/conf
	em87/mkdev
		\fIor\fP
	streams/mkdev
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
.IP \(bu
To install a driver for asynchronous ports, first edit
.B /etc/default/async
as described in the Lexicon; then:
.DM
	cd /etc/conf
	asy/mkdev
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
.IP \(bu
To disable a hard-drive controller:
.DM
	cd /etc/conf
	bin/idenable -d \fIdisk_driver\fP
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
where
.I disk_driver
is one of the following:
.DS
	\fBat\fR	AT disk driver
	\fBaha\fR	Old-style Adaptec driver
	\fBss\fR	Seagate/Future Domain driver
	\fBhai\fR	Host-independent Adaptec driver
.DE
.IP \(bu
To enable a hard drive:
.DM
	cd /etc/conf
	bin/idenable -e \fIdisk_driver\fP
	# if you are installing the hai driver:
	# hai/mkdev
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
where
.I disk_driver
is one of the following:
.DS
	\fBat\fR	AT disk driver
	\fBaha\fR	Old-style Adaptec driver
	\fBss\fR	Seagate/Future Domain driver
	\fBhai\fR	Host-independent Adaptec driver
.DE
.IP \(bu
To change from a loadable versus a non-loadable keyboard driver,
to change the video adapter, or to add or remove virtual consoles:
.DM
	cd /etc/conf
	console/mkdev
	bin/idmkcoh -o /\fIkernel_name\fR
.DE
