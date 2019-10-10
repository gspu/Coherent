.ds TL "Network Administration"
.NH "Administering TCP/IP"
.PP
TCP/IP is a networking protocol \(em that is, a description of how
computers can communicate with each other.
The acronym
.I TCP/IP
stands for ``Transmission Control Protocol/Internet Protocol''.
It consists of protocol, plus a set of user-level programs that let
you and other users of your system communicate easily with every
other computer that is hooked into your network.
As the name implies, TCP/IP was devised for use on the Internet, which
is a global computer network built and maintained by the Department
of Defense's Advanced Research Projects Agency (DARPA).
With TCP/IP, it is possible to
.\"send mail among the computers on your network;
log into each computer easily; upload or download files;
and do other useful tasks.
And if your network is linked into the Internet, your network effectively
becomes every Internet machine throughout the world.
.PP
Your NP-series network-protocol package is a networking tool that
implements TCP/IP and a set of selected tools.
The NP-series package lets you communicate over an
Ethernet local-area network with other computers that run TCP/IP-based software.
.PP
This tutorial will help you to set up your TCP/IP network, and will help
you get started in savoring the pleasures (and coping with the frustrations)
of your network.
However, the subject of how to set up and run a TCP/IP network, and how to
work with the Internet, is far too broad to cover in one brief manual.
If you intend to manage a network on your own, we suggest you obtain the
following book:
.QS
Hunt, Craig: \fITCP/IP Network Administration.\fR
Sebastopol, Calif., O'Reilly and Associates Inc., 1990.
.QE
.PP
If you wish to explore the Internet at all (and we urge that you
look into this possibility), we strongly recommend the following book:
.QS
Krol, Ed: \fIThe Whole Internet:
User's Guide & Catalog.\fR
Sebastopol, Calif., O'Reilly and Associates Inc., 1992.
.QE
.PP
Another useful source of information is the column ``Net Worth,'' which is
written by Steven Baker and which appears monthly in \fIUNIX Review\fR.
The rest of this chapter will refer continually to these publications.
.SH "How the Package Works"
.PP
TCP/IP provides the underlying protocols for the set
of commands in the NP-series network-protocol package.
These commands perform such functions as letting you log into remote systems,
transfer files, and execute commands remotely on other systems.
The commands are as follows:
.IP \fBtelnet\fR
To log into another system on your network, as if you were
using a terminal directly connected to that system.
If your local network is attached directly to the Internet, you can use
.B telnet
to log into any system on the Internet, anywhere in the world,
that grants access to outsiders.
.IP \fBftp\fR
The name
.B ftp
stands for ``file transfer protocol''.
It lets you transfer files between your local system and any system on
your network that also supports
.BR ftp .
.IP \fBtftp\fR
This is a specialized file-transfer utility.
It is usually used by intelligent peripheral devices rather than by humans.
.IP \fBrlogin\fR
This lets you log into a remote system as if it were your own,
as defined under the Berkeley UNIX release 4.3 (4.3BSD).
.IP \fBrshl\fR
This lets you execute a command (or a command file)
on a remote host that supports the 4.3BSD conventions for remote execution.
.IP \fBrcpy\fR
This lets you perform a ``remote copy,'' that is,
let you copy files from another system as defined under 4.3BSD.
Unlike
.BR ftp ,
.B rcpy
can handle ``third-party transfers'' \(em that is, copy files from one remote
system to another.
Under other implementations of TCP/IP, this utility is called
.BR rcp .
.IP \fBruptime\fR
Display the status of every system on your network
that uses the 4.3BSD conventions for reporting such information.
.IP \fBrwho\fR
List every user on every computer on your network that
uses the 4.3BSD conventions to report such information.
.PP
All of the utilities are functionally similar to the
4.3BSD commands
.BR telnet ,
.BR ftp ,
.BR tftp ,
.BR rlogin ,
.BR rsh ,
.BR rcp ,
.BR ruptime ,
and
.BR rwho .
Note, however, that
.B rsh
and
.B rcp
have been renamed, respectively,
.B rshl
and
.BR rcpy .
.PP
Because they use the standard DARPA protocols, you can use
.BR telnet ,
.BR ftp ,
and
.B tftp
to communicate with the wide variety of systems that support these protocols;
these include most flavors of UNIX and some software packages that run under
MS-DOS.
.PP
The other commands are derived from 4.3BSD,
and are supported on other, similar operating systems, including COHERENT.
Because of their origin, these utilities make certain assumptions
about the hosts with which they communicate (for example,
.B rcpy
assumes that your local user name exists on the remote host
and assumes UNIX/COHERENT file-naming conventions)
and have special features, such as a provision for recognizing
user names shared by different hosts (thus simplifying the login process).
It is easier to use this group of utilities if you plan to communicate with
remote systems that support them.
.PP
Each of these commands is summarized in the Lexicon at the back of this
manual.
All are introduced in the following chapter, entitled
.IR "TCP/IP Commands" .
.Sh "Clients and Servers"
.PP
The network commands are built
on the concept of clients and servers.
A
.I client
is a program that requests service from another computer system;
a
.I server
is a program that processes the requests of a client.
Each computer system runs both clients and servers:
the clients to request information from remote systems, and servers to
process requests that remote systems make on it.
.PP
When you invoke a program on your system to communicate with a remote system,
you are invoking a client.
When that client communicates with a remote system, it actually talks
with its server counterpart on that system.
The client interprets your requests, initiates
communication with a server, and directs the requests to the server.
The server typically runs continually on a system,
and waits for requests from clients.
The client passes your request to the server,
which determines whether the request is acceptable;
if it is, the server executes the request and sends the results back to
the client, which in turn reports them to you.
.PP
As you can see, an appropriate server must be running on the remote host
before any of the client programs can work:
.IP \(bu 0.3i
The commands
.BR telnet ,
.BR ftp ,
and
.B tftp
require, respectively, the servers
.BR telnetd ,
.BR ftpd ,
and
.BR tftpd .
.IP \(bu
.B rlogin
requires that the remote system run the
.B rlogin
server
.BR rlogind .
.IP \(bu
Depending on how you invoke it,
.B rshl
tries to contact either an
.B rsh
server or an
.B rlogin
server.
.IP \(bu
.B rcpy
uses the
.B rsh
server.
.PP
The utilities
.B ruptime
and
.B rwho
work slightly differently.
They do not contact remote servers when invoked, but
instead read files containing information collected by a local
.B rwho
server from other
.B rwho
servers on the network.
.PP
Your NP package also includes servers to handle incoming requests from
other users on the network, so a remote user can access to your files.
.Sh Terminology
.PP
Before we proceed, please note the following terms.
.IP \(bu 0.3i
Any computer system on your network that runs one or more servers is called a
.IR host .
This term was chosen to indicate that the computer system can host inquiries
from other systems on the network.
.IP \(bu
Your computer system \(em that is, the one upon which you invoke a client to
dispatch inquiries to other systems on the network \(em is sometimes called the
.IR "local host" .
Please do not confuse this term with the term
.IR "local network" ,
which is set of hosts that are in the same physical locale and that
share the same physical network.
.IP \(bu
A
.I gateway
is a computer that is hooked into more than one network.
Hosts on one network can pass information or requests to hosts on another
network via a gateway that is hooked into both networks.
A request from one host to another may cross an indefinite number of
networks, and pass through one or more gateways along the way.
You (or your network's chief administrator) will have to tell your
network which systems are gateways to other networks, plus the addresses
of those networks.
.IP \(bu
A
.I packet
is a bundle of information that may be up to several kilobytes long.
Ethernet passes information among the hosts on the network in the form of
packets; the host that is broadcasting the information breaks up the
transmission into packets, and the host receiving the information reassembles
the packets.
Each packet contains the Ethernet address of the system that broadcast it
and the system that is its intended destination.
A packet can also be called a
.IR frame .
.Sh "Data bases and Files"
.PP
The TCP/IP clients and servers continually read a
number of files on your system.
These files hold such information as the addresses of remote hosts,
the names of users on remote hosts, and the permissions (if any) that
individuals on remote hosts have on your system.
The following lists the files that define how TCP/IP operates on your system:
.IP \fB/etc/ftpusers\fR
This file
names the users who are \fInot\fR allowed access to your system via
.BR ftp .
.IP \fB/etc/hosts\fR
This file lists the names and Internet
addresses of remote systems that your system knows.
.IP \fB/etc/hosts.equiv\fR
This file names ``equivalent hosts'' \(em that is, the hosts with which
your system shares account names.
.IP \fB/etc/networks\fR
This file names the remote networks with which you want to communicate.
.IP \fB/etc/protocols\fR
This file names the communication protocols used on your local network.
.IP \fB$HOME/.rhosts\fR
This file names the remote users who can log into the account of
the user whose home directory this is.
If
.B $HOME
is directory \fB/\fR, then
.B \&.rlogin
names the remote systems whose superusers
have superuser privileges on this system.
.IP \fB/etc/services\fR
This file lists services provided by your local network.
.PP
The following section describes how to set up and maintain these files.
.SH "Setting Up and Administering TCP/IP"
.PP
Before you use the TCP/IP utilities, you must meet the
following requirements:
.IP \(bu 0.3i
The NP TCP/IP package is installed and running on your computer.
.IP \(bu
Your host computer's location, users, and services
have been identified to other hosts on the network.
.IP \(bu
The remote hosts on your network are running the appropriate servers.
.IP \(bu
You have the required privileges on your local system.
.IP \(bu
You know the names by which the various hosts are known on your network,
and their Internet addresses.
.IP \(bu
You know the names of users and their passwords you must
supply to each remote host.
.PP
Before you can use your NP package,
you must establish a support environment for your NP-series software.
This section covers various topics concerning the use of your NP package
on a TCP/IP network.
These topics include:
.IP \(bu 0.3i
Setting up and maintaining network data-base files.
.IP \(bu
Setting up a public-access directory for use with
.BR ftp .
.IP \(bu
Managing the routing table used to communicate via gateways
with systems on other networks.
.IP \(bu
Dividing your network into sub-networks.
.IP \(bu
Configuring the number of sockets and the window size of the NP622 TCP image.
.Sh "Overview of the Network Data-base Files"
.PP
This section discusses the following files:
.DS
.ta 0.5i 0.7i
	\(bu	\fB/etc/ftpusers\fR
	\(bu	\fB/etc/hosts\fR
	\(bu	\fB/etc/hosts.equiv\fR
	\(bu	\fB/etc/networks\fR
	\(bu	\fB/etc/protocols\fR
	\(bu	\fB$HOME/.rhosts\fR
	\(bu	\fB/etc/services\fR
.DE
.PP
All of these files have the same layout.
The pound-sign character `#' introduces a comment:
both clients and servers ignore all text from a `#' to the end of the line.
Blank lines are ignored.
Each line of data consists of one or more items.
Each item, in turn, consists of a string of printable character demarcated
by white space;
thus, no data item used by a TCP/IP client or server can have white space
embedded within it.
.PP
You can edit these files by hand, or you can use some of the commands
included with NP package to edit these files interactively.
The following sub-sections discuss the data contained in each file,
and the proper strategy for populating and editing each.
.Sh "/etc/hosts: Set System Names and Addresses"
.PP
The file
.B /etc/hosts
gives the name and address of each host on your system
with which your system can communicate.
There are three ways to identify a host on a network:
by its name, by its Internet address (also called a ``network number''),
or by its Ethernet address.
The Ethernet address is burned into the Ethernet card that that host
uses to hook into the network;
you can assign host names and Internet addresses yourself.
.Sh "Host Names"
.PP
You can assign to any host a name up to 32 characters long.
The name can include letters, digits, and punctuation marks,
but the first character must be a letter.
You must include this name in
.BR /etc/hosts ,
as explained in the section on \fBhosts\fR, later in this section.
Because other users will use your system's host name (or an alias)
when they dispatch inquiries to a server on your computer, you should keep
your host's name brief and to the point.
.PP
You set the host name for your computer when you installed COHERENT.
To change the host name for your computer, edit the file
.BR /etc/uucpname ,
or use the COHERENT command
.B uname
with the option
.BR \-S .
For details, see the Lexicon entries
.B uuname
and
.B uname
in the COHERENT Lexicon.
.Sh "Internet Addresses"
.PP
The Internet address is a four-byte number that identifies your network
and each host on the network.
It uses dot notation.
In dot notation, bytes are separated by periods.
The value of a byte is usually given in decimal (although octal and
hexadecimal can be used as well).
Decimal is the default notation;
to indicate an octal number, prefix it with a a `0' (zero);
to indicate a hexadecimal number, prefix it with `0x'.
For example:
.DM
	120.0.0.1
	Ox85.0x0.0x0.0x1
.DE
.PP
There are three classes of Internet addresses, A, B, and C.
They differ in the number of bytes they use to identify the network
and the number they use to identify each host on the network.
The higher the class of network, more hosts it can identify.
A class is identified by the value of the high-order bits in its
first byte.
The following summarizes each class of network:
.IP \fBA\fR 0.3i
A class-A network uses the first byte of the Internet address
to identify the network, and the remaining three bytes to identify
each host on the network.
The address of each class-A network sets the high-order bit of its first
byte to zero;
thus, the first byte of a class A network must be less
than 128 (decimal).
In the following example, the network number is 57 and the
remaining three bytes give the host address:
.DM
	57.0x00.0x01.0x01
.DE
.IP \fBB\fR
A class-B network uses the first two bytes of the Internet address
to identify the network, and the remaining two bytes to identify
each host on the network.
The address of each class-B network sets the two high-order bits of its
first byte to `10';
thus, the first byte of a class-B network address must be
between 128 and 191 (decimal).
In the following example, the network number is 150.8
and the remaining two bytes give the host address:
.DM
	150.8.123.16
.DE
.IP \fBC\fR
A class-C network uses the first three bytes of the Internet address
to identify the network, and the remaining byte to identify
each host on the network.
The address of each class-C network sets the three high-order bits of its
first byte to `110';
thus, the first byte of a class-C network address must be
between 192 and 240 (decimal).
In the following example, the network number is 192.20.0
and the last byte gives the host address:
.DM
	192.20.0.1
.DE
.PP
Every host on a network must have the same network number \(em that is,
they must differ only in host portion of their addresses.
.PP
If the hosts on your network are going to communicate with other networks on
the Internet, you must obtain a network number from the
University of Southern California Information Sciences Institute at
the following address:
.DS
	USC-Information Sciences Institute
	4676 Admiralty Way
	Marina del Rey, CA 90291
	(213) 822-1511
.DE
Otherwise, you can assign your own network number.
.PP
Regardless of whether you assign your own network number, or receive one
from DARPA, you (or your network's chief administrator)
must assign the Internet address to each host on your network.
Each host on your network must have a unique host address.
.PP
For a more detailed discussion of Internet addressing, see
``Defense to the Rescue,'' by Steven Baker (\fIUNIX Review\fR, March 1992,
pp. 13-22).
.PP
To assign a host address to your host computer, use the command
.BR ifconfig .
For details on this command, see the its entry in the Lexicon at the end
of this manual.
.Sh "Ethernet Addresses"
.PP
The Ethernet address is a unique, six-byte number that identifies
a host on the Ethernet.
It is the physical address of the host that is actually used
in transmitting packets over the Ethernet.
It is permanently stored in the Ethernet board by its manufacturer.
Each manufacturer ensures the uniqueness of the number of each of its boards
\(em not just among the boards it manufactures, but in the world.
Your NP-series board stores its Ethernet address in its ROM.
The NP software translates between
a host's physical Ethernet address and its logical Internet address.
.Sh "The Structure of /etc/hosts"
.PP
Each TCP/IP host on your network must have a file called
.BR /etc/hosts .
This file gives the name and address of every other host on the network.
Your NP distribution kit contains a sample
.BR /etc/hosts .
To add entries to this file, use the command
.BR mkhosts .
This command is summarized in the Lexicon at the back of this manual.
.PP
If your system already contained a copy of
.BR /etc/hosts ,
it was not overwritten when you installed your NP package.
You should, however, run
.B mkhosts
to ensure that your local-host name is correct.
.PP
Each line in
.B /etc/hosts
contains an entry for a host on the network.
The line begins with the Internet address.
The host name comes next, followed by any aliases \(em that
is, alternative names that are also to be recognized as referring
to that host.
For example:
.DM
.ta 0.5i 1.5i 2.5i 3.5i
	# Hosts Data base
	#
	193.16.16.27	accounting	acct	beancounters
	193.16.16.2	janet	jcr
	193.16.3.5	engineering	eng
.DE
.PP
As you can see, a given host can have more than one alias.
Aliases need not be terse; however, you should not use an alias name that
you would not want the users of that host to see.
.PP
Each copy of
.B /etc/hosts
must include the following standard entries:
.DM
.ta 0.5i 1.5i
	127.1	localhost
	127.0.0.1	loopback
.DE
.PP
When you specify only two parts of an address, the
second part represents the final three bytes of the address.
Thus, the addresses
.B 127.1
and
.B 127.0.0.1
are the same address in dot notation.
.PP
The address
.B 127.1
by convention names the local host.
Packets sent to this address are returned to the local host:
they do not go onto the Ethernet.
This loopback address is useful in debugging software.
The host names
.B hostname
and
.B loopback
are also conventional names for the local host.
.PP
Do not change these entries in
.BR /etc/hosts .
You must place a separate entry into
.B /etc/hosts
for your local host's Internet address and host name.
.PP
If you have relatively few hosts on the network, you can
maintain a complete
.B /etc/hosts
file for every host by following these steps:
.IP \fB1.\fR 0.3i
Select one device on the network to be the
.IR "master host" .
This should be the host used by the chief system administrator for your
network.
The superuser on the master host should have root privileges on every
other system on the network.
.IP \fB2.\fR 0.3i
Create a complete
.B /etc/hosts
file for the master host on the network.
(Remember to include an entry for the master host itself.)
.IP \fB3.\fR
Put an entry for the master host into the
.B /etc/hosts
on every other host.
.IP \fB4.\fR
The superuser on the master host then uses the command
.B rcpy
to copy the completed
.B /etc/hosts
from the master host to every other host.
.IP \fB5.\fR
Whenever a host is added to or deleted from the network,
the chief system administrator updates only the
.B /etc/hosts
on the master host, and then redistributes it to every other host on the
network.
.Sh "/etc/services"
.PP
The file
.B /etc/services
lists the services provided by TCP/IP and related protocols.
This is a standard file, and you should not have to change the one provided
as part of your NP-series network protocol package distribution kit.
.Sh "/etc/hosts.equiv"
.PP
The file
.B /etc/hosts.equiv
establishes equivalent accounts among the hosts on the network.
If your local host names a host in
.BR /etc/hosts.equiv ,
then the users on that host can use the commands
.B rshl
and
.B rcpy
to access your host without a password.
.PP
To permit users access to some or all of the hosts on the network,
each host should contain a file named
.B /etc/hosts.equiv
that lists the host names of other hosts whose users are to be
allowed access without a password.
.PP
Your NP package contains a sample copy of
.BR /etc/hosts.equiv .
If your host computer already has an
.BR /etc/hosts.equiv ,
it was not overwritten when you installed your NP package,
and you do not need to change it in any way.
.PP
To add entries to
.BR /etc/hosts.equiv ,
use the command
.BR mkhosts .
.Sh "$HOME/.rhosts"
.PP
Each user on each host can have a file named
.B \&.rhosts
file in his login directory.
This file names the users on other systems who can log into his account.
This mechanism is quite useful if one user has a login account
on a number of different hosts on your network,
and wishes to access any of them without the bother of logging in.
.PP
For example, the following gives the
.B \&.rhosts
file that user
.B fred
on system
.B lepanto
has in his home directory:
.DM
	siss81	fb
	mwc	fwb
.DE
.PP
This file lets user
.B fb
on system
.B siss81
and user
.B fwb
on system
.B mwc
log into
.BR fred 's
account.
.PP
The file
.B /.rhosts
names the hosts whose superusers can log into the local host.
For example, consder that file
.B /.rhosts
on system
.B lepanto
reads as follows:
.DM
	siss81	root
	mwc	root
.DE
Here, the superusers on systems
.B mwc
and
.B siss81
can log into system
.B lepanto
and act as the superuser on that system.
.PP
Each host's copy of
.B /.rhosts
should name the master host on your network.
.PP
On some implementations of the UNIX operating system,
a superuser cannot log in to a remote host because
.B root
is only allowed to log in on the system console.
Under COHERENT, the superuser can log in from any terminal.
.PP
To create
.BR /.rhosts ,
use the command
.BR mkhosts .
For more information, see the entries for
.B rlogin
and
.B rshl
in the Lexicon at the end of this manual.
.Sh "/etc/networks"
.PP
The file
.B /etc/networks
gives the names and network numbers of your network and
other networks with which you can communicate.
If you want to communicate on the Internet,
you must create this file by obtaining the official
network data base maintained by the Network Information Control Center (NIC).
(The address of the NIC is given above.)
To this, add information about other networks not listed by NIC,
with which you may want to communicate.
.PP
If you are not going to use the Internet, you can create your own
.B /etc/networks
file.
Each entry should include the following information:
.IP \(bu 0.3i
Official network name.
A network name can contain any printable character other than white space,
newline, or the comment character `#'.
.IP \(bu
Internet network number, as described above in the section on
.BR /etc/hosts .
.IP \(bu
Any aliases for the network name.
.Sh "/etc/protocols"
.PP
The file
.B /etc/protocols
contains information about the protocols used on the Internet.
For each protocol you use,
this file should contain one line with the following information:
.IP \(bu 0.3i
Official protocol name.
.IP \(bu
Protocol number.
.IP \(bu
Any aliases for the protocol name.
.PP
The following example is of a sample version of
.BR /etc/protocols :
.DM
.ta 0.5i 1.25i 2.15i 3.0i
	#
	# Internet (IP) protocols
	#
	ip	0	IP	# internet protocol, pseudo-protocol
	icmp	1	ICMP	# internet control message protocol
	ggp	3	GGP	# gateway-gateway protocol
	tcp	6	TCP	# transmission control protocol
	egp	8	EGP	# exterior gateway protocol
	pup	12	PUP	# PARC universal packet protocol
	udp	17	UDP	# user datagram protocol
	hmp	20	HMP	# host monitoring protocol
	xns-idp	22	XNS-IDP	# Xerox NS IDP
	rdp	27	RDP	# reliable datagram protocol
.DE
.Sh "/etc/ftpusers"
.PP
To restrict access to files on the local host, write into file
.B /etc/ftpusers
the names of all users who are
.I not
allowed access to the local host via the command
.BR ftp .
You should, for example, put
.B uucp
into this file, so that users cannot log in as
.B uucp
and use
.BR uucp 's
privileges to obtain files that contain sensitive information
(e.g., UUCP configuration files that contain the names and passwords of
other remote systems).
.SH "Setting Up Anonymous ftp"
.PP
You can set up on your system an account into which any user can log
anonymously via
.BR ftp ,
with no password required.
The home directory for this account can contain files that you wish to make
accessible to any user on any remote host.
System administrators occasionally do this to make information
available to the public at large, e.g., files of information on how to
obtain an account on the local host or of the services the local host offers.
By convention, such an account is given the login name
.B anonymous
or
.BR ftp ;
thus, such an account usually is called an
.I "anonymous ftp"
account.
.PP
Set up this account carefully, to ensure
that your system's security cannot be breached.
.PP
To set up the directories and file permissions that you need to run anonymous
.BR ftp ,
first create a login account for user
.BR ftp .
Use the following guidelines when you set up its home directory:
.IP \(bu 0.3i
Give user
.B ftp
the home directory
.BR /ftp .
It should have mode 500 \(em that is, it should
be readable and executable only by its owner (that is, by user \fBftp\fR),
and unwritable by anyone.
(For more information on what modes mean, see the Lexicon entry for
.BR ls ).
.IP \(bu
Create the directory
.BR /ftp/bin .
It must be owned by the superuser and have mode 444 (that is, readable by
anyone and writable by no one).
Modify the file
.B /ftp/.profile
to define the environmental variable
.B PATH
to point only to the directory
.BR /ftp/bin .
.IP \(bu
Copy into
.B /ftp/bin
the files
.B /bin/ls
and
.BR /bin/sh .
Give them mode 111.
This ensures that the user who logs in as an anonymous user will be able
to run a shell and to see what files are in
.B /ftp
and its subdirectories, but do nothing else.
For security reasons, you may wish to edit
.B /ftp/etc/passwd
to include only the user
.BR ftp .
.IP \(bu
Create the directory
.BR /ftp/etc .
Make it owned by the superuser and unwritable by anyone.
Copy into it the files
.B /etc/passwd
and
.B /etc/group
so that the
.B ls
command can work properly.
These files should have mode 444.
.IP \(bu
Create the directory
.BR /ftp/pub .
Make it owned by user
.B ftp
and give it mode 777 (that is, readable or writable by anyone).
You (or any user) can then write files into this directory that are to be
accessible via the anonymous account in this directory.
.PP
The following script performs the above steps;
note that it can only be run by the superuser
.BR root :
.B 
.DM
	cd /ftp
	mkdir etc bin dev public
	cp /etc/passwd /etc/group etc
	cp /bin/sh /bin/ls bin
	ls /dev/is* | cpio -pudvm .
	chmod 444 etc/*
	chmod 111 bin/*
	chmod 666 dev/is
	chmod 511 etc dev bin
	chmod 777 public
	exit
.DE
.SH "Routing"
.PP
Your NP package's routing feature lets your host computer
communicate via gateways with hosts on other networks
that are connected to your local network.
.PP
To communicate with a remote host or network, a user
or application name gives the name or Internet address
of the destination, as usual.
Your NP package recognizes that the destination address
requires routing, and forwards the packets to the appropriate gateway,
as defined in your NP package's routing tables.
Your NP package does not alter the destination Internet address of the packet;
rather, it simply routes the packet to the gateway, and lets the gateway
re-route the packet to its destination.
.PP
The routing table specifies the gateway to use for
a particular host on a remote network.
It is the responsibility of your network administrator to maintain
the routing table for your host.
You can use the command
.B route
to add, delete, or display entries in the routing table.
For details, see the entry for
.B route
in the Lexicon at the back of this manual.
.PP
For example, suppose there are two networks.
On network A, host
.B bill
is at address 120.0.0.1; and on network B, host
.B hillary
is at address 128.1.0.1.
Connecting the two networks is a gateway, whose address on
network A is 120.0.0.10.
.PP
.\"
.\" A diagram should go here.
.\"
Packets sent from
.B bill
to
.B hillary
must go through the gateway at 120.0.0.10.
To add this information to the routing table on
.BR bill ,
the administrator on
.B bill
must issue the command
.BR route ,
followed by the argument
.BR add ,
the name or address of the remote destination,
the name or address of the gateway,
and the number of hops (that is, gateways) a packet must traverse
to reach the destination \(em in this case:
.DM
	route add 128.1.0.1 120.0.0.10 1
.DE
.PP
A routing table must be set up every time your NP package is started.
You may find it convenient to combine all the
.B route
commands for your routing table entries in a shell script,
so that you can recreate the table without retyping all the commands each time.
.SH "Dividing Your Network into Subnets"
.PP
As discussed earlier, an Internet address is divided into a network address
and a host address.
All hosts on a network have the same network address; and
every host on a network has a unique host (IP) address.
.PP
In some cases, you may wish to split your
network into sub-networks, or
.IR subnets .
For example,
a large organization might want to put each department onto its own subnet.
This would allow you to control access to departmental resources,
while also keeping one overall network number for communication on the Internet.
.PP
Your NP software supports subnetting.
For communication between subnets, however,
you must have a gateway on the network; but
nodes that run NP software cannot function as a gateway.
.PP
In subnetting, bits from the host address identify the subnet.
The number of bits used in each host address indicates
how many subnets are available to that host.
Conversely, the more bits used in the subnet,
the smaller the number of hosts that can be on the subnet.
.PP
Use a bit mask to identify which bits of the
host address identify the subnet number;
typically, these bits are contiguous and
are the most significant bits of the host address.
When you assign host addresses, use the subnet bits of
the address to indicate which subnet the host is on.
.PP
Each host on the TCP/IP network must be made aware
that subnetting is in effect, and which bits represent the subnet number.
Once it knows this, it can interpret the address correctly.
.PP
You implement subnetting on a host running an
NP package via the netmask parameter of the command
.BR ifconfig .
For details on this command, see its entry in the Lexicon at the back of
this manual.
.PP
The netmask parameter uses a bit mask to specify
which bits of the Internet address are used to identify the subnet.
The mask includes the network address and the subnet address.
You can specify the mask in the following ways:
.IP \(bu 0.3i
As an Internet address in dot notation.
.IP \(bu
As a single hexadecimal number.
.IP \(bu
By using a pseudo-network name.
This name must be included in your copy of
.B /etc/networks
along with the Internet address that specifies the mask.
.PP
For example, if you have a class-B network, and you
want to use four bits of the Internet address for the subnet
address, the mask would be set as follows, using dot notation:
.DM
	ifconfig netmask 255.255.240.0
.DE
.PP
The four subnet bits are contiguous with the network address.
This is not a requirement, but it is recommended.
.PP
The corresponding hexadecimal number is
.DM
	0xFFFFF000
.DE
.PP
If you have the following entry in
.BR /etc/networks ,
you can use subnets as an argument with netmask to specify the four
bits used for the subnet address:
.DM
	255.255.240.0
.DE
.SH "Where To Go From Here
.PP
The next chapter in this manual introduces the commands that you can use
to interrogate other hosts on your network.
