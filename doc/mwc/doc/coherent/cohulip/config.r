.ds TL "Advanced cohulip"
.ds TI COHULIP
.NH "cohulip Configuration Files"
.PP
This chapter describes the configuration files that
.B cohulip
uses to hold networking information.
.SH "startup.net"
.PP
When
.B net
first starts up, it attempts to read a startup script.
You can name a startup script as an argument to the command
.BR net .
For example, to invoke
.B net
with the startup script
.BR startup.foo ,
use the command:
.DM
	net startup.foo
.DE
If you do not name a startup script on
.BR net 's
command line,
.B net
attempts to read the file
.BR $NETHOME/startup.net .
.PP
If it finds a startup script (either one you name on the
.B net
command line, or
.BR startup.net ),
.B net
executes all of the commands that file contains,
just as if you had typed them from the keyboard.
Thus, you can use the startup script to load key information into
.BR net ,
such as your system's IP address and the address of your system name-service
provider; and you can use it to start up the suite of servers that you wish
to support during this session.
.PP
When you installed
.B cohulip
onto your system, the installation program wrote a startup script named
.BR $NETHOME/startup.new .
After you have read this chapter and the chapter \fIRunning cohulip\fR, which
appears later in this manual, you should check
.BR startup.new ;
and if it appears to be correct, you should rename it
.BR startup.net .
.Sh "Order of Commands"
.PP
The order of the commands is important within a startup script.
Some commands depend upon information supplied by other commands.
If you modify a startup script, be sure that the commands appear in the
following order.
.PP
The following gives the usual order of commands within a startup script:
.DS
	# set your machine's name
	\fBhostname \fIname\fR
	# set your machine's IP address
	\fBip address \fIaddress\fR
.DE
.DS
	# attach a session to a physical device; can have multiple sessions
	\fBattach \fIarguments\fR
		.\^.\^.
	# commands to tune IP:
	\fBip ttl \fItime-to-live\fR
.DE
.DS
	# commands to tune TCP:
	\fBtcp mss \fIbytes\fR
	\fBtcp window \fIbytes\fR
	\fBtcp irtt \fIbytes\fR
	# set the IP address of domain-name server(s):
	\fBdomain addserver \fIip_address\fR
		.\^.\^.
.DE
.DS
	# add one or more routes to the routing table:
	\fBroute add \fIarguments\fR
		.\^.\^.
	# fire up servers to use during this networking session:
	\fBstart\fR \fIserver\fR
		.\^.\^.
	# miscellaneous commands:
	\fBlog $NETHOME/net.log\fR
.DE
Each of these commands is explained in detail in the chapter
\fIRunning net\fR, which appears later in this manual.
The following summarizes each briefly:
.IP "\fBhostname \fIname\fR"
This command sets your machine's name, as it will be known to the other
machines on the network.
You should set this to your system's \*(UU name, which is set in file
.BR /etc/uuname ;
however, this is not required.
.IP "\fBip address \fIaddress\fR"
Set your system's IP address.
By custom, this is set at the beginning of a startup script;
however, this is not required.
If you are using SLIP to access an Internet provider that assigns an IP
addresses when you log in, you will have to type in this command by hand
after the startup script has executed and you log in.
.IP "\fBattach \fIarguments\fR"
Attach a networking session to a physical device, either a serial port
(for SLIP) or your Ethernet card.
You can attach more than one session simultaneously; but
a physical device can have only one session attached to it at any given time.
For example, if your system has an Ethernet card, you can attach one session
to the Ethernet card and another to a serial port.
If you are attaching to a serial port that has a modem plugged into it, the
you can add a script to the
.B attach
command that dials the modem and logs you into the remote system.
.IP "\fBip ttl\fR"
.II "IP^time-to-live"
.II "time to live"
This command sets the time-to-live of IP datagrams \(em that is, it sets the
number of systems that can handle this datagram before it ``perishes.''
The command
.B ip
has a number of other sub-commands that let you tune the behavior of IP under
.BR net .
.IP "\fBtcp mss\fR"
.IS "\fBtcp window\fR"
.IS "\fBtcp irtt\fR"
These commands tune TCP.
They set, respectively, TCP's maximum segment size, its ``window'' (or maximum
number of bytes it can receive at any one time), and its initial round-trip
time (or time it will wait to receive the remote host's acknowledgement of
receipt of a datagram).
The command
.B tcp
has numerous other options as well; for details, see the chapter \fIRunning
net\fR, which follows in this manual.
.IP "\fBdomain addserver\fR"
Set the IP address of a domain-name server \(em that is, a remote system that
you can interrogate to translate systems' names into their IP addresses.
You can set more than one domain-name server at once.
.IP "\fBroute add\fR"
.II "routing table"
Set the IP address of the system to which a given session should route its
datagrams \(em that is, add its address to
.BR net 's
.IR "routing table" .
Each session must be linked to at least one remote system; and it may be
linked to more than one.
If a session is linked to more than one remote system,
.B net
uses an elaborate set of internal rules to decide which system to which
it should send the datagrams.
.IP \fBstart\fR
This command invokes one of the servers that is built into
.BR net .
.II server^definition
A
.I server
handles requests from a remote system for a given service.
The servers built into
.B net
include
.BR ftp ,
.BR telnet ,
.BR ping ,
.BR echo ,
and
.BR smtp .
.IP \fBlog\fR
At the end of the startup script, you add any number of miscellaneous
commands.
The command
.B log
logs activity that is generated when remote systems access your system.
It appends its output onto the file that you name on its command line, so
you can use this command to keep a running log of activity on your system.
.PP
For a full discussion of
.BR net 's
commands and their options, see the chapter \fIRunning net\fR, which follows
in this manual.
.SH "ftpusers"
.PP
When a user accesses your system via FTP, she bypasses \*(CO's normal login
routines.
However, you need to control who can access your system and what each person
can do; therefore,
.B net
provides the file 
.BR ftpusers .
This file names every user who can access your system via FTP,
sets the directories that that user can access, and the permissions
that she has in each.
.PP
.B ftpusers
consists of an indefinite number of rows.
Each row names one user who can access your system, and sets her password
and permissions.
It has the following format:
.DS
	\fIuser password directory permissions \fB[ \fIdirectory permissions ... \fB]\fR
.DE
There must be exactly one space character between each field.
If a line begins with a pound-sign character `#',
.B net
ignores it; you can use such lines to hold comments.
.PP
.I user
and
.I password
gives the user's name and password.
When a user attempts to access your system via FTP,
.BR net 's
FTP server asks her to identify herself, and to enter her password.
If the user enters a name that does not appear in
.BR ftpusers ,
or does not enter the correct password, then the FTP server refuses that user
access to your system.
.PP
A password of `*' tells the FTP server to accept any password.
Note that the passwords in this file are
.I not
encrypted.
Therefore, this file should be readable only by user
.BR bin ,
and user
.B bin
on your system should have a password.
.PP
.I directory
names a directory that the user can access.
When you name a directory, the user has access to that directory and all
of its subdirectories.
.I permissions
gives the permissions that
.I user
has in this directory.
This is a single-digit integer that sums the following permission values:
.DS
	\fB1\fR	Permission to read files within \fIdirectory\fR
	\fB2\fR	Permission to write new files within \fIdirectory\fR
	\fB4\fR	Permission to overwrite or delete files within \fIdirectory\fR
.DE
For example, a
.I permission
of `3' sums one and two, and thus grants
.I user
permission to read and write files within
.IR directory ,
but does not grant her permission to overwrite or remove files.
.PP
Each
.I directory
argument must be followed by a
.I permission
argument.
You can name up to eight
\fIdirectory\-permission\fR pairs for any given
.I user
within
.BR ftpusers .
.PP
File
.B ftpusers
is always kept in directory
.BR $NETHOME .
If
.B ftpusers
does not exist, no user will be granted access to your system via FTP
(not even yourself), and therefore
.BR net 's
FTP server will be useless.
.SH "hosts.net"
.PP
File
.B hosts.net
maps an IP address to the name or names of a host.
.B net
reads this file to transform a host name into that host's IP address.
.PP
The entries within
.B hosts.net
have the following format:
.DS
	\fIip_address\fR \fIhostname \fB[ \fIalias_hostname ... \fB]\fR
.DE
.I ip_address
gives the host's four-byte IP address.
The same IP address can appear in more than entry within
.BR hosts.net .
.PP
.I hostname
names the host.
Note that
.I hostname
does
.I not
have to give the host's full domain name; however,
.I hostname
must be unique within
.BR hosts.net .
.PP
.I hostname
can be followed by one or more
.IR alias_hostname s.
These are alternative names for the host, or abbreviations.
Note that each
.I alias_hostname
must be unique within
.BR hosts.net ,
or trouble will result.
.PP
The following gives an example entry from
.BR hosts.net :
.DM
	137.229.10.39	raven raven.alaska raven.alaska.edu
.DE
This entry links the IP address 137.229.10.39 to three host names:
.BR raven ,
.BR raven.alaska ,
and
.BR raven.alaska.edu .
All three are aliases for the same site.
You can also break these three host names into three separate entries within
.BR hosts.net ,
as follows:
.DM
	137.229.10.39	raven
	137.229.10.39	raven.alaska
	137.229.10.39	raven.alaska.edu
.DE
Some users find the latter format to be more legible;
however, if a site's IP address changes, you must be careful to update each
of its entries within
.BR hosts.net ,
or trouble will result.
.SH "Help Files"
.PP
.BR net 's
command \fBhelp \fIsubject\fR displays the contents of file
\fB$NETHOME/\fIsubject\fB.hlp\fR.
Normally,
.I subject
is one of
.BR net 's
commands or configuration files; but it can be anything you like,
so long as it fits into a file.
.PP
.II net.hlp
If you do not specify a
.IR subject ,
.B net
displays the contents of
.BR net.hlp .
.PP
.B cohulip
comes with a help file for each of
.BR net 's
commands.
You can modify or add to them if you wish.
.SH "Finger Files"
.PP
The command
.B finger
lets you read information about a user who is on a remote system.
It takes three forms:
.IP "\fBfinger \fIuser\fR\fR
Display information about
.IR user ,
who is a recognized user on your system.
.IP "\fBfinger @\fIhost\fR"
Display the name of each recongnized user on system
.IR host ,
which can be a remote host or your local host.
.IP "\fBfinger \fIuser\^\fB@\fIhost\fR"
Display the contents of the finger-file for
.I user
who is on system
.IR host .
.PP
.B cohulip
also supports a server for
.BR finger ,
so that other users can find information about the users on your system.
To enable
.B finger
on your system, do the following.
.PP
First, prepare a file in directory
.B $NETHOME/finger
for each user about whom you wish to release information.
Name the file after the user's login identifier plus the suffix
.BR .txt .
For example,
to release information about user
.BR fred ,
create file
.BR $NETHOME/finger/fred.txt .
You can put into that file anything that you like; presumably
it should describe that user, and not be slanderous.
.PP
Second, create file
\fB$NETHOME/finger/\fIhostname\fB.txt\fR, where
.I hostname
is the name of your local host.
This file should name all users on your system.
When a remote host tried to find the known users on your system,
it will be handed the contents of this file.
(You can also create a named pipe of this name,
and pump the output of the command
.BR who
through it.
For details on how to do this, see the entry for
.B "named pipe"
in the \*(CO Lexicon.)
.PP
Finally, invoke the
.B net
command
.BR "start finger" .
You can embed this in your startup script.
.PP
That's all there is to it.
Now, when someone accesses into your system via TCP/IP, she'll be able
to see information about selected users.
