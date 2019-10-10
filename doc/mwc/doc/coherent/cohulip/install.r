.ds TL "Installing cohulip"
.ds TI COHULIP
.NH "Installing cohulip"
.PP
.II cohulip^installation
This chapter describes how to install
.B cohulip
onto your system.
.PP
Installation of
.B cohulip
is a three-step process;
.IP \fB1.\fR 0.3i
Decide how you are going to tie your system into a network \(em either
by modem or by Ethernet, and gather some preliminary information.
.IP \fB2.\fR
Un\fBtar\fR the
.B cohulip
archive, and run the installation script to install
.B cohulip
onto your system.
.IP \fB3.\fR
Modify some configuration files by hand,
depending upon the type of network with which
.B cohulip
will be communicating.
.PP
None of these steps is particularly difficult; but each requires
that you pay attention to detail.
We urge you to read the following descriptions carefully
.I before
you begin to install
.BR cohulip ;
if you do not, you may waste time wrestling with a problem that has
already been solved.
.SH "Phase One \(em Preliminary Information"
.PP
Before you begin installation, you need to make some decisions and gather
some information.
.Sh "Install Hardware and Driver"
.PP
If you are going to use Ethernet, install your Ethernet card, and add the
.B ne
device driver to your \*(CO kernel.
Follow the directions included with your card and the driver.
.PP
If you are using SLIP, install your modem and configure your serial port.
For information how to do this, see the documentation that came with your
port; and check the entries for
.B asy
and
.B modem
in the \*(CO Lexicon.
.Sh "Choose Type of Network"
.PP
First, you must decide what type of
.B cohulip
will be communicating.
There are three possibilities:
.IP \fB1.\fR 0.3i
With an Internet provider via modem and SLIP.
.IP \fB2.\fR
With a local Ethernet network that has access to the Internet.
.IP \fB3.\fR
With a local mini-Ethernet network that does not have access to the Internet.
.PP
The rest of this chapter
assumes that you will be working with only one type of network.
It is possible to configure
.B cohulip
to work with more than one type of network simultaneously;
this is described in a later chapter.
.Sh "Obtain an IP Address"
.PP
.II "IP address"
An IP address is a 32-bit number that uniquely identifies a given machine
(or ``host'') that is running the suite of TCP/IP protocols.
As noted in earlier in the chapter on TCP/IP basics, sites send datagrams
to each other's ``IP address.''
Thus, before your machine can participate in a TCP/IP network,
it must have its IP number.
.PP
If you are using
.B cohulip
to hook your machine into an established Ethernet network, you must
talk with the person who administers the network and obtain an IP number
from her.
.PP
If you are creating a stand-alone mini-network, you can make up
a pseudo-IP number.
The only requirements are (1) that the number does not duplicate the IP
number on any other machine on your mini-network;
and (2) that it does not end in `0' or ``255''.
(These addresses have a special meaning to TCP/IP, and selecting one of them
for your pseudo-address will create all manner of problems.)
.PP
If you will be interfacing
.B cohulip
an Internet provider via SLIP, you must obtain the IP number from your
provider.
Internet providers assign IP numbers in either of two ways:
either they assign your system a permanent IP number, or they
assign you a different IP number every time you log in.
How the number is assigned affects how you configure and use
.BR cohulip ;
this is discussed in more detail below.
Note that if your provider offers you the choice of a permanent address
and a floating address,
.I always
take the permanent address.
.Sh "Obtain the Domain-Name Server IP Address"
.PP
Before TCP/IP can send a datagram to a remote site,
it must know that site's IP address.
People, however, usually prefer to remember sites by their names; therefore,
one of TCP's major tasks is to find the IP number that is linked to a given
site's name.
There are two ways to do this:
either by reading the contents of file
.BR $NETHOME/hosts.net ,
or by interrogating a domain-name server.
.PP
A
.I "domain-name server"
is a host that has a large data base of IP addresses and names, and
that you have permission to interrogate.
If you cannot find the address of the domain-name server, you will be limited
to accessing the systems whose names and IP addresses are listed in file
.BR hosts.net .
.PP
If you are hooking into an established Ethernet network, ask the system
administrator for the IP address of the domain-name server for that
network.
If you are accessing the Internet via SLIP, ask your Internet provider
for that address.
.PP
If you are using a local mini-network, you cannot use a domain-name server.
You must find the IP address of every other system on your mini-network,
and write it into file
.BR hosts.net .
The format of this file is discussed below.
Even if you have domain-name service, you may wish to write into this file
the addresses of systems that you contact frequently, to spare
.B cohulip
the trouble of interrogating the domain-name server.
.Sh "Obtain Another Computer"
.PP
If you are going to run
.B cohulip
over a local mini-network, you need another machine to talk to, if only
to see that your installation is working correctly.
You may be able to borrow such a system \(em these machines are very
common now.
If you are at work, you may be able to plug into a TCP/IP network there.
.Sh "Get the Dialing Sequence"
.PP
If you are going to access an Internet provider via SLIP, you must
find how to dial into your provider's modem.
You must obtain from the provider the telephone number to call;
the baud rate, parity, etc., for the modem; and your login identifier
and password.
Also note the port into which your modem is plugged, and the command
sequence with which you must initialize the modem.
.PP
You must also obtain the exact wording of each prompt that
.B cohulip
will encounter as it dials the provider, and what the proper response should
be.
The best way to obtain this information is to use the command
.B ckermit
to dial the remote provider by hand.
At every step (initializing the modem, entering your login identifier,
entering your password, etc.), write on a piece of paper what you will
send down the serial line, and what you see in response.
It is important that you write
.I exactly
what you see and what you enter.
You will use this information to write a ``dialing script'' for
.BR cohulip .
.SH "Phase Two \(em Installing cohulip"
.PP
Installing
.B cohulip
requires that you de-archive the
.B cohulip
package, then run an installation script.
.Sh "De-archiving the Files"
.PP
.B cohulip
comes in the form of a compress
.B tar
archive.
Move the archive into a directory where you store sources, e.g.,
.BR /usr/src ;
then type the following command:
.DM
	gtar -xvzf cohulip.tgz
.DE
De-archiving the files should take less than a minute, depending upon
the speed of your hard disk.
.PP
Dearchiving the files does not affect the archive
.B cohulp.tgz
itself.
You may wish to keep this archive intact as a backup copy, should you
accidentally damage or destroy some
.B cohulip
files.
.Sh "Contents of the cohulip Package"
.PP
When you un-\fBtar\fR the archive
.BR cohulip.tgz ,
its contents are written into directory
.BR cohulip .
This directory contains the following sub-directories and files:
.IP \fBInstall.sh\fR
This file holds a shell script that automates installation.
.IP \fBcohulip/README\fR
This file gives a general description of
.B cohulip 
and how to install it.
.IP \fBbin\fR
This sub-directory holds the executable program
.BR net ,
which is the executable program that actually performs networking tasks.
.IP \fBdoc\fR
This directory holds this manual, both in viewable and in
printable (PostScript) version.
It also holds the original K5JB documentation.
.IP \fBhelp.tar\fR
This archive holds help files read by
.BR net .
These files are de-archived when you install
.B cohulip
onto your system.
.IP \fBnet.tar\fR
This holds example configuration files that are used by the application
.BR net .
These files are described in a later chapter of this manual.
.IP \fBsrc\fR
This gives the sources for the application
.BR net .
.Sh "Invoking the Installation Script"
.PP
To install
.BR cohulip ,
log in as the superuser
.BR root ;
then enter the directory
.B cohulip
and invoke the script
.BR Install.sh .
This script performs the task of installation.
.PP
.B Install.sh
begins its work with the following message:
.B1
	                         WELCOME TO COHULIP
.sp \n(pDu
	This script installs the COHERENT user-level IP package (cohulip) onto your
	system.
.sp \n(pDu
	We strongly suggest that you read the document ./doc/install.p *before* you
	attempt to install cohulip onto your system.  If you wish to stop installation
	and review this chapter, answer 'n' to the following question.
.sp \n(pDu
	Do you wish to continue installation? [y]
.B2
.PP
Because you are reading this chapter you, you probably should answer `y' to
this question.
.B Install.sh
continues with this message:
.B1
	We assume that you have read the chapter on cohulip installation, and are
	continuing installation.  Please note that you can interrupt this program
	at any time without damaging your system in any way.  To interrupt
	installation, just type <ctrl-C>.  You may have to type it more than once.
	To resume installation, su to root and execute script Install.sh again.
.B2
.Sh "Get Home Directory for cohulip"
.PP
After the introductory messages, the first item of business is to name the
directory into which you want
.B cohulip
installed:
.B1
	The first step in installation is to select a home directory for cohulip.
	By default, cohulip is installed into directory $NETHOME.  To accept this
	default, simply press <return>.
.sp \n(pDu
	Home directory for cohulip: [/usr/net]
.B2
.PP
To accept the default directory
.BR /usr/net ,
simply press
.BR <return> .
If you want to install into another directory, type it at the prompt.
.B Install.sh
then asks you to confirm your choice:
.DM
	Install into directory /usr/net? [y]
.DE
To accept the default reply of `y', press
.BR <return> ;
if, however, you have changed your mind or if you see something wrong with
the directory's name, type `n' to cancel your choice.
.B Install.sh
will continue to ask you until you answer `Y' to some directory's name.
.PP
Once you have selected a directory for
.BR cohulip ,
.B Install.sh
then copies some files and archives into it, then processes the archives.
It also copies the executable
.B net
into directory
.BR /usr/bin .
.Sh "Get Local System Name"
.PP
The next step is to name your local system:
.B1
	The next step in installation is set your system's name.  Normally, this is
	the name set in the file /etc/uuname, although you can use another name if
	you wish.  Again, to accept the default, simply press <return>.
.sp \n(pDu
	Set the system name to \fIsystemname\^\fP? [y]
.B2
.PP
.I systemname
is the name of your system, as set in the file
.BR /etc/uuname .
If you wish to use another name, reply `n' to the above prompt.
.B Install.sh
displays the prompt
.DM
	Enter the name of your system:
.DE
and waits for you to type your system's name.
The cycle continues until you have entered and confirmed a name for your
system.
.Sh "Get Internet Address"
.PP
The next step is to enter your system's IP address:
.B1
	The next step is to enter your system's IP address.  As the manual states,
	some users are assigned an IP address when they log into their Internet
	provider's machine, and so may not know it at this time.
.sp \n(pDu
	Enter your IP address now ? [y]
.B2
.PP
If you do not yet know your IP address \(em or if you are accessing an
Internet provider that assigns your IP address only when you log in \(em
type `n'.
.B Install.sh
will then skip to the next item of business.
.PP
To accept the default response of `y', press
.BR <return> .
When you do so,
.B Install.sh
asks:
.DM
	Enter your IP address (e.g., 123.456.789.012):
.DE
Enter your system's IP address.
.B Install.sh
then asks you to confirm it:
.DM
	Is your IP address \fIaddress\^\fP?
.DE
where
.I address
is the address you typed.
Enter `y' if
.I address
is correct; if it is not, type `n' and
.B Install.sh
will try again.
.Sh "SLIP Information"
.PP
.B Install.sh
now asks you whether you intend to use SLIP:
.B1
	Use a serial port or modem for SLIP? [y]
.B2
.PP
Press `n' if you are
.I not
going to use SLIP.
If you press
.BR <return> ,
.B Install.sh
assumes that you are using SLIP, and proceeds to ask you some further
questions.
First, it asks you to name the serial port:
.DM
	Enter the serial port (e.g., com3fl):
.DE
Enter the port.
(This should be the local port, i.e., the port that ends with the
letter `l'.)
.B Install.sh
then asks you to confirm your entry:
.DM
	Serial port is \fIport\^\fP? [y]
.DE
If you enter `n',
.B Install.sh
tries again.
.PP
.B Install.sh
next asks you to enter the speed of the port:
.DM
	Enter the port's speed (e.g., 19200):
.DE
Do so.
.B Install.sh
then asks you to confirm what you typed:
.DM
	The serial port's speed is \fI\^\fP? [y]
.DE
If you type `n' in reply,
.B Install.sh
tries again.
.PP
This ends the information you must enter for SLIP.
.B Install.sh
displays the name it assigns to the SLIP interface:
.DM
	Your SLIP interface is named sl0
.DE
.Sh "Ethernet Information"
.PP
.B Install.sh
asks you whether you are going to use
.B cohulip
with an Ethernet connection:
.B1
	Use an NE1000 or NE2000 ethernet card? [y]
.B2
.PP
If you are
.I not
using Ethernet, answer `n';
.B Install.sh
skips to the next section.
If you press
.BR <return> ,
.B Install.sh
assumes that you will be using Ethernet and asks you to name the device
by which
.B cohulip
can access your Ethernet card:
.DM
	Enter the Ethernet card name (e.g., ne0):
.DE
Enter the name of the Ethernet device; do not prefix it with
.BR /dev/ .
.B Install.sh
asks you to confirm your entry:
.DM
	Your Ethernet card is named \fIcard\^\fP? [y]
.DE
If you enter `n',
.B Install.sh
tries again.
.PP
This ends the information you must enter for Ethernet
.B Install.sh
displays the name it assigns to the Ethernet interface:
.DM
	Your Ethernet interface is called 'ne0'
.DE
.Sh "Select the Default Route"
.PP
If you have indicated that you will be using both Ethernet and SLIP,
.B Install.sh
asks you to select which will be the default route:
.B1 
	Because you wish to use cohulip with both an Ethernet network and a SLIP
	network, you must select a default route.  This is the interface over which
	packets will be sent when a route cannot be located locally.  This is normally
	the interface that connects to the nearest gateway or router.
.sp \n(pDu
	Enter 'sl0' for SLIP or 'ne0' for Ethernet:
.B2
.PP
As the message states, you must select which network \(em the one you
access by Ethernet or the one you access via SLIP \(em should receive
data by default.
Enter ``sl0'' for SLIP, or ``ne0 for Ethernet,
depending upon which network you prefer.
.PP
.B Install.sh
asks you to confirm your choice:
.DM
	Use \fIroute\fP as default route?
.DE
where
.I route
is SLIP or Ethernet.
If you answer `n',
.B Install.sh
asks you to try again.
.PP
When you confirm your choice, or if you selected only SLIP or Ethernet,
.B Install.sh
states your system's default route:
either
.DM
	Your default route is \fIroute\fR
.DE
.Sh "Domain-Name Server"
.PP
.B Install.sh
now asks if you wish to use a domain-name server:
.B1
	As noted in the installation manual, you can enter the IP address or name
	of the domain-name server (DNS) that is available to your system.  Note that
	you should NOT enter this information if (1) you do not have it, or
	(2) your system does not have a permanently assigned IP address.
.sp \n(pDu
	Do you wish to enter the name or address of your domain-name server? [y]
.B2
.PP
As the message notes, you must take the following into mind when you decide
whether to enter the address of the domain-name server:
.IP \(bu 0.3i
If you do not know the address, answer `N'.
.IP \(bu
If you are using a local mini-network, and do not have access to a
domain-name server, answer `N'.
.IP \(bu
If you are using SLIP
.I and
your Internet provider assigns your IP address when you log in, answer `N'.
Do so even if you know the address of your Internet provider's domain-name
server.
The reason for this are explained below.
.IP \(bu
If you are using Ethernet on an established netowrk and you know the
address of that network's domain-name server, answer `Y'.
.IP \(bu
If you are using SLIP
.I and
your Internet provider has assigned you a permanent IP address
.I and
you know the address of your Internet provider's domain-name server,
answer `Y'.
.PP
If you answer `Y',
.B Install.sh
asks you to enter the name or address of the domain-name server:
.DM
	Please enter the correct name or dotted-decimal address of your DNS:
	e.g., '192.48.234.1' or 'myserver.com'.
	Enter name or address of your DNS:
.DE
Enter either the address of your domain-name server, or its name.
(Note that if you enter the name, you must put its name and address
into file
.BR $NETHOME/hosts.net ,
as described below.)
.B Install.sh
asks you confirm this:
.DM
	Your DNS has name or address \fIidentifier\^\fB? 
.DE
where
.I identifier
is the name or address you entered.
If you answer `n',
.B Install.sh
asks you to try again.
.Sh "Select Servers"
.PP
.B Install.sh
now asks you for the servers you wish to start up by default:
.B1
	The next step will be to ask you about the servers that you wish to turn on.
	Note that if you turn on a server, cohulip provides that service to people who
	are accessing into your system.  If you do not turn on a given server, you will
	still be able to use that service on other machines.
.B2
.PP
As the message states, a
.I server
provides service to people who are accessing your system;
it does not affect what you can do on other systems.
For example, if you decide to ``turn on'' the TELNET server, users on
other systems will be able to use the TELNET server to access your system.
If you decide not to do so, other users will not be able to do so, but you
will still be able to use the TELNET client to access other systems.
.PP
Note that ``turning on'' a server does not affect how your system performs:
a
.B cohulip
server does not occupy additional memory.
This is because all servers are already compiled into the application
.BR net ;
so invoking a server simply ``turns on'' one of
.BR net 's
feature.
.PP
.B Install.sh
goes through the following routine for four of its servers.
First, it asks you wish to turn on the server:
.DM
	Turn on the server for \fIserver\^\fP? [y]
.DE
where
.I server
is the server in question.
To accept the server, simply press
.BR <return> ;
.B Install.sh
displays the following to confirm your choice:
.DM
	Turning on server \fIserver\fP
.DE
If you do not want the server, answer `n'.
.PP
.B Install.sh
asks you about four servers:
.BR telnet ,
.BR echo ,
.BR finger ,
and
.BR ftp .
Note that the
.B echo
server services the command
.BR ping ;
if you select no other server, you should select this one, so that other
machines on your network can at least tell when you are on-line.
.Sh "Final Actions"
.PP
With those inquiries,
.B Install.sh
has finished asking you for information.
It displays the following messages:
.B1
	We have created a file called $STARTUP based upon
	the information you have enter.  For the application 'net' to use this
	information, you must invoke it with the following syntax:
.sp \n(pDu
		net startup.new
.sp \n(pDu
	We have also set the environmental variables NETHOME and NETSPOOL within the
	system-wide profile file '/etc/profile'.  However, this will not take effect
	until you next log in.  The network software needs these variables to locate
	its files.
.B2
.PP
.B Install.sh
then gives some information about configuration files:
.B1
	The file $NETHOME/hosts.net allows you assign aliases to IP addresses.
	Some examples are included in $NETHOME/hosts.net already.  The file
	$NETHOME/domain.txt is maintained by the domain-name client application
	if you have specified a name server.
	The file $NETHOME/hosts.net allows you assign aliases to IP addresses.
	Some examples are included in $NETHOME/hosts.net already.  The file
	$NETHOME/domain.txt is maintained by the domain-name client application
	if you have specified a name server.
.sp \n(pDu
	File $NETHOME/ftpusers controls access to ftp services.  There are examples
	in $NETHOME/ftpusers
.sp \n(pDu
	If you find that $NETHOME/startup.new is suitable for your system as
	we have created it, you can change its name to startup.net; it will then
	be automatically executed each time you start net.
.sp \n(pDu
	This concludes the installation of cohulip.
.B2
.PP
As the message states, that message concludes the work of
.BR Install.sh .
.Sh "After the Dust Settles"
.PP
The following describes what
.B Install.sh
has done to your system.
.PP
To begin,
.B Install.sh
created directory
.B $NETHOME
(default,
.BR /usr/net ).
It copied
.BR cohulip 's
configuration and help files into that directory.
.PP
Most importantly, it built script
.BR startup.new .
This script holds commands that reflect the information you entered when
you installed
.B cohulip
onto your system.
As you work with
.BR cohulip ,
you will find yourself copying and modifying this file rather frequently.
It is described in detail below.
.PP
.B Install.sh
also created sub-directory
.BR finger .
If you have enabled the
.B finger
server, you must write description files for your users and
copy them into that directory.
.PP
Finally,
.B Install.sh
copied the executable program
.B net
into directory
.BR /usr/bin .
.PP
.B cohulip
uses two environmental variables:
.IP \fBNETHOME\fR
This variable names the directory where
.BR net 's
configuration files are stored, e.g.,
the startup scripts,
.B finger
files, and
.B help
files.
.IP \fBNETSPOOL\fR
This names the file into which
.B net
writes files that it receives from other systems.
This is set to
.BR /usr/spool/uucppublic .
.PP
.B Install.sh
inserted these variables into file
.BR /etc/profile .
Note that you must log out and log in again before these
variables are added to your run-time environment.
.PP
.B net
also uses the environmental variable
.BR TZ .
This variable was set when you installed \*(CO onto your computer;
you do not need to worry about it.
.PP
.B net
requires that its files have the following permissions:
.DM
	-r-x--x--x   1 bin      bin     $NETHOME
	-rw-rw-rw-   1 bin      bin     domain.txt
	drwxrwxrwx   2 bin      bin     finger
	-rw-r--r--   1 bin      bin     ftpusers
	-rw-r--r--   1 bin      bin     hosts.net
	-rw-r--r--   1 bin      bin     startup.new
.DE
As you can see, all files should be owned by user and group
.BR bin .
Note, too that file
.B $NETHOME/domain.txt
should be writable by all users.
This file is updated automatically by the domain-name server, so it must
be open to every user who may use
.BR cohulip .
.PP
If the permissions on the
.B cohulip
files are not correct, you can set them with the following commands:
.DM
	chmog 511 bin bin /usr/bin/net
	chmog 644 bin bin $NETHOME/*
	chmog 666 bin bin $NETHOME/domain.txt
.DE
.SH "Phase Three \(em Post-Installation Configuration"
.PP
In some instances, you may have to perform some post-installation
configuration to get
.B cohulip
up and running.
.PP
The following discusses first Ethernet, then SLIP.
It walks you through the post-installation configuration you may have
to perform and simple tests to see if the system is working properly.
.Sh "Configuring cohulip for Ethernet"
.PP
Although Ethernet is a more sophisticated technology than a
serial line \(em or perhaps
.I because
it is more sophisticated \(em setting up
.B cohulip
on an Ethernet connection is much simpler than establishing it for SLIP.
The following describes the steps you must perform to configure
.B cohulip
for Ethernet.
It is adapted from mail by Bob Chalmers (earth@nanguo.cstpl.com.au).
.PP
First, copy
.B /usr/net/startup.new
to
.BR /usr/net/startup.net .
It should look like the following:
.DM
	# the name of your local host:
	hostname nanguo
	# the IP address of your local host:
	ip address 203.1.96.1
	# attach a TCP session to the Ethernet device:
	attach nx 0 0 arpa ne0 5 1500
.DE
.DM	
	# add the remote site's IP address to the routing
	# table for this session:
	route add 203.1.96.5 ne0
.DE
.DM
	# IP address of your network's domain-name server (DNS)
	# not present if entered no DNS address:
	domain addserver 123.456.789.012
.DE
.DM
	# these lines are boilerplate
	ip ttl 32
	tcp mss 1500
	tcp window 1500
	tcp irtt 2000
	eol unix
.DE
.DM
	# start the servers you selected; this shows them all
	start telnet
	start echo
	start finger
	start ftp
.DE
You may wish to replace line
.B "start telnet"
with the line
.BR "start telunix 23" .
.B telunix
is the server that manages logins; this command hangs it onto port 23,
which is the well-know port for TELNET.
It is described in the last chapter to this manual.
.PP
If you are using a mini-network, invoke TCP/IP on your other machine.
(A later section of this manual discusses how to bring up TCP/IP under
\*(MD.)
.PP
Now, try
.BR cohulip :
type
.B net
at your \*(CO prompt.
You will see some messages, then see the prompt
.BR net> .
.PP
To see if the networking is actually working, try
.BR ping ing
another machine on your network.
For example, if another system on your network is named
.BR lepanto ,
try the command:
.DM
	ping lepanto
.DE
Note that to access another machine by name, you must either have access
to a domain-name server or have entered its name and IP address into file
.BR hosts.net .
(This file is described in another section of this manual.)
Otherwise, you must use the other system's IP address with the
.B ping
command, instead of the system's name.
.PP
If the network is working correctly, you will see information from
.B ping
that describes how long it took to send a datagram to the remote system
and receive an acknowledgement.
Once this happens, you know that you can contact the remote system.
The next step is to invoke FTP and TELNET, and start to use your network.
For details on how to do this, see the chapter entitled \fIRunning net\fR,
which appears a little later in this manual.
.Sh "Connecting to the Internet via SLIP"
.PP
The following describes how to configure
.B cohulip
to connect to the Internet via an Internet provider.
Because of the complexity of this task, it requires a fair bit of
post-installation configuring.
Do not be discouraged if it does not work the first time:
after all, SLIP combines all of the complexity of a
network with that of a \*(UU link.
This will try your patience, but stick with it because the reward is great:
full access to the Internet and its wealth of information.
.PP
.II "Wright, Randy"
The following instructions, which are
adapted from mail by Randy Wright, should help make matters easier for you.
.Sh "Startup Script"
.PP
As with configuring for Ethernet,
the first step is to move file
.B $NETHOME/startup.new
to
.BR $NETHOME/startup.net .
.Sh "Modem Configuration"
.PP
Earlier in this chapter, we asked you to write exactly the steps
needed to initialize your modem and dial into your provider's system.
We will now use that information to write a script with which
.B net
can dial the provider's system.
If you are familiar with \*(UU and its ``chat scripts,'' then writing a
dialing script will be easy for you; if you are not, well,
you are about to learn how.
.PP
.II "script^dialing"
.II "dialing script"
A
.I "dialing script"
consists of an indefinite number of pairs of send/expect strings:
.B net
sends
.I send
half of the pair to the serial port; and it expects to see the
.I expect
string in response.
If, in response to a given send string,
it receives anything other than given the expect string,
.B net
assumes that something has gone wrong and aborts networking session.
.PP
A dialing script must perform the following tasks:
.IP \fB1\fR 0.3i
Initialize the modem to dial out.
.IP \fB2\fR
Dial the modem.
.IP \fB3\fR
Send your login identifier to the remote system.
.IP \fB4\fR
Send your password to the remote system.
.PP
You may also need to capture the IP address that your provider assigns you,
but that will be discussed in the following sub-section.
.PP
To initialize the modem, the first send/expect pair should read something
like the following:
.DM
	ATE1V1Q0S0=0\er OK
.DE
In standard Hayes notation, the send string turns on echoing,
turns on text-based messages, and turns off dial-in support.
The string ``\er'' tells
.B net
to send a carriage return at the end of the send string; this is needed to
flush the send the string out to the serial port.
The expect string states that you expect the modem to reply
.BR OK ,
which is the standard modem response.
For details on how you should initialize your modem, check the documentation
that came with it.
.PP
The next send/expect string dials the modem:
.DM
	ATDT1234567\er ogin:
.DE
The string
.B ATDT
is the standard Hayes command to dial a Touch-Tone telephone line.
``1234567'' stands for the telephone number to dial.
.B ogin:
is the
.I end
of the string with which your Internet provider's system prompts you to log in.
This prompt will, of course, change from system to system.
.PP
The next send/expect pair sends your login identifier to your provider's
system, and awaits the prompt for your password:
.DM
	mylogin\er ssword:
.DE
The send string
.I mylogin
stands for your actual login identifier.
As always, it ends with the string ``\er''.
The expect string
.B ssword:
is the
.I end
of the prompt for your password.
This prompt will, of course, change from system to system.
.PP
The final send/expect pair is actually just a send string:
.DM
	mypassword\er
.DE
where
.I mypassword
is your password.
Note that this is not followed by an expect string:
once
.B net
has processed the last send or expect string in the script,
it expects to start exchanging SLIP datagrams with your provider.
.PP
Thus, the entire script reads as follows:
.DM
	ATE1V1Q0S0=0\er OK ATDT1234567\er ogin: mylogin\er ssword: mypassword\er
.DE
To make the script available to
.BR net ,
edit file
.BR startup.net
and attach the script to the end of the command
.BR attach .
Note that all of
.B attach
command, including the dialing script, must appear on one line within the
.B startup
script.
If you are using MicroEMACS to edit
.BR startup.net ,
do not be alarmed if the
.B attach
command extends far past the right margin of your screen.
.PP
For more information on how to handle a modem or serial port, see the
entries
.B asy
and
.B modem
in the \*(CO Lexicon.
For a more elaborate of writing such a script, see the entry for
.B sys
in the Lexicon, or see the tutorial for \*(UU that is in the front portion
of your \*(CO manual.
For a detailed description of the
.B net
command
.BR attach ,
see the chapter \fIRunning net\fR, which appears later in this manual.
.Sh "Setting Your IP Address:  Permanent Address"
.PP
As we noted at the beginning of this chapter,
your Internet provider can give you an IP address in either of two ways:
.IP \(bu 0.3i
A ``permanent'' IP address, one that is assigned to you alone.
.IP \(bu
A ``floating'' IP address, which is assigned only when you dial in.
.PP
If you have a permanent IP address, you gave that address to
.B Install.sh
when you installed
.B cohulip
onto that system; and
.BR Install.sh ,
in turn, copied it into
.B startup.net
in a form of the command
.BR "ip address" .
For example, if your Internet provider has assigned your local host IP
address 123.456.789, then command
.DM
	ip address 123.456.789
.DE
should already be embedded within
.BR startup.net .
There is nothing more that you need to do.
.Sh "Setting Your IP Address:  Floating Address"
.PP
If, however, your Internet provider uses ``floating'' IP addresses,
the situation becomes much more complex.
.PP
When an Internet provider assigns an IP number, she flashes it on the screen
as the last item of text to be shown before entering SLIP mode.
You must somehow ``capture'' that address, so that you can type it into
.BR net .
To do so, you must add one final ``expect'' string to the end of the
dialing script that you added to the command
.BR attach ,
one that describes how the provider displays the IP address.
This forces
.B net
to display that address, so you can jot it down and then type it into
.BR net .
(Alas, there is as yet no automated way to capture this address.)
.PP
Unfortunately, there is no standard by which the provider displays this
information:
it varies from provider to provider.
For example, one provider ends her login messages with something like this:
.DM
	Your IP address 123.456.789 .....
.DE
To ``capture'' this address, add the string
.DM
	...
.DE
to the end of your dialing script.
This tells
.B net
to expect a string that ends with ``...'', and to display it.
When you see this, you can jot down the IP address.
.PP
Remember that the ``expect'' string must unique identify the
.I end
of the string that holds the IP address.
You may need to be creative to find such a string.
.PP
Once you have captured the IP address being assigned,
when you see the prompt
.B net>
you must type the command:
.DS
	\fBip address \fIip_address\fR
.DE
where
.I ip_address
is the IP address that you jotted down.
This tells
.B net
the address that has been assigned to you.
.Sh "Domain-Name Server Address"
.PP
Your Internet provider normally will have a name server that you can access.
This is needed so you can convert names of hosts into IP addresses (as
described in the previous chapter).
You should have obtained the address of this server as part of your
preparation for installing
.BR cohulip .
.PP
When you installed
.BR cohulip ,
the program
.B Install.sh
asked you for the address of your domain-name server.
At that time, we said that you should enter the address
.I only
if your Internet provider had given you a permanent IP address.
The reason is as follows:
When you address of the domain-name server,
.B net
initializes the domain-name software with the IP address for your
system that is current at the present moment.
If you have not yet set your IP address, the domain-name software
will be improperly set, and domain-name service will not work correctly.
.PP
If you have a permanent IP address, you entered the address of the
domain-name server when you installed
.BR cohulip .
The program
.B Install.sh
embedded this information in file
.B startup.net
in the form of the command
.DS
	\fBdomain addserver \fIip_address\fR
.DE
where
.I ip_address
is the address of your domain-name server.
This problem is solved, and you can move on to the next sub-section.
.PP
If, however, you use a floating IP address,
you must type the command
.DS
	\fBdomain addserver \fIip_address\fR
.DE
at the
.B net>
prompt to set the IP address of your name server.
Note that you must type this command
.I after
you have typed the command
.B "ip address"
to set your system's IP address, for the reason described above.
.PP
If you do not have access to any name server, you can still use SLIP,
but you cannot use the Internet's domain-name services.
Thus, you will be able to access by name only those sites that
are described in files
.B host.net
or
.BR domain.txt .
These files are described in detail in the next chapter.
.Sh "An Example Login Session"
.PP
At this point, you have finished your post-installation
configuration of
.BR cohulip ,
and you are ready to try it out.
.II "Chalmers, Robert"
The following example, which was supplied by Robert Chalmers,
walks through one such test session.
.PP
To begin, when you use
.B net
to dial into a remote system,
it displays on your screen your dialing script as it is executed.
.PP
To begin, this example uses the following version of
.BR startup.net :
.DM
	hostname nanguo
	ip address [203.1.96.1]
	attach asy 0 /dev/asy01l slip sl0 0 1500 9600 AT&F\er \e
		OK AT&Q6DT1234567\er ogin: MYLOGIN\er ord: PASSWORD\er
	route add default sl0
	domain addserver [192.160.127.125]
	ip ttl 32
	tcp mss 1500
	tcp window 1500
	tcp irtt 2000
	eol unix
	start telunix 23
	start echo
	start finger
	start ftp
.DE
Note that the
.B attach
command must appear on one line.
It is broken across two lines in the above display to make it legible.
.PP
As you can see, this user has a permanent IP address; so this script
automatically sets the IP addresses of both his local host and the
domain-name server.
.PP
When you invoke the command
.BR net ,
it logs its actions as follows:
.DM
	startup: [/usr/net/startnet.net]
	asy_init: tty name = /dev/asy01l
.DE
This shows that
.B net
is executing the start-up script
.BR startup.net ,
and that it is initializing device
.BR asy01l ,
as named in the
.B attach
command.
.PP
The next lines walk through the dialing script that is appended onto the
.B attach
command.
The first sequence initializes the modem:
.DM
	I'm sending  : 'AT&F\er'
	I'm expecting: 'OK'
	AT&F
	OK
.DE
The next lines dial the modem, and invoke the login prompt on the remote
system:
.DM
	I'm sending  : 'AT&Q6DT1234567\er'
	I'm expecting: 'ogin:'
	AT&Q6DT1234567
	login:
.DE
In the above sequence, ``1234567'' stands for the telephone number.
.PP
The next lines log you into the system:
.DM
	I'm sending : 'MYLOGIN\er'
	I'm expecting: 'ord:'
	MYLOGIN
	Password:
	I'm sending  : 'MYPASSWORD\er'
.DE
In the above, ``MYLOGIN'' represents the login identifier, and
``MYPASSWORD'' represents the password.
.PP
At this point, you are logged into the remote system,
and has begun to exchange SLIP datagrams with the remote system.
.PP
If the user had had a floating IP address, his dialing script would
have had one more string attached to it:
a string that matched the end of the string with which the provider
displayed his IP address.
He would have jotted it down; and when he saw the
.B net>
prompt, he would have typed the commands
.DS
	\fBip address \fIip_address\fR
	\fBdomain addserver \fIip_address\fR
.DE
in that order.
.PP
Perhaps the lesson here is:
if you can get a permanent IP address,
.IR "grab it!"
.Sh "Writing a Script to Invoke net"
.PP
If you are using
.B cohulip
to access one or more Internet providers via SLIP,
you may wish to wrap the command
.B net
within a shell script.
This is because the command
.B net
is very limited in how it can manipulate a serial port:
it can dial out, but it cannot enable or disable a port, or even hang
up the telephone after the
.B net
session has concluded.
You can handle such details in a script.
.PP
The following gives an example of a shell script called
.BR Net .
This script dials two different Internet providers, one called
.B MCS
and the other called
.BR SIS .
Each has its own startup script \(em called, respectively,
.B startup.mcs
and
.BR startup.sis .
.DM
	#!/bin/sh
	RMODEM=com3fr
	LMODEM=com3fl
.DE
.DM
	case $1 in
		mcs)	TARGET=mcs;;
		sis)	TARGET=sis;;
		*)	echo "Usage: Net mcs|sis";
			exit 1;;
	esac
.DE
.DM
	# disable the port
	echo Disabling port $RMODEM
	/etc/disable $RMODEM
.DE
.DM
	# invoke "net" for the target network
	net startup.$TARGET
.DE
.DM
	# reset the modem and port after a "net" session:
	echo Resetting the modem
.DE
.DM
	# get the modem's attention
	echo "+++" > /dev/$LMODEM
	sleep 2
	echo "+++" > /dev/$LMODEM
	sleep 2
.DE
.DM
	# hang up the phone
	echo "ath" > /dev/$LMODEM
	sleep 2
.DE
.DM
	# reload the standard modem settings
	echo "atz" > /dev/$LMODEM
	sleep 2
.DE
.DM
	# bump modem into high-speed mode; needed with 14.4 modems under COHERENT
	echo "at" > /dev/$LMODEM
	sleep 2
.DE
.DM
	# re-enable the port
	echo Re-enabling port $RMODEM
	/etc/enable $RMODEM
.DE
.DM
	# end of Net
.DE
Note that you must have permission to use the commands
.B enable
and
.B disable
before you can run this script successfully.
For details, see their entries in the Lexicon.
.PP
Please note that with some modems, simply echoing a string to the modem's
device does not work.
The following version of
.BR Net ,
which is taken from the \*(CO script
.BR /etc/modeminit ,
should work correctly with such modems:
.DM
	#!/bin/sh
	RMODEM=com3fr
	LMODEM=com3fl
.DE
.DM
	case $1 in
		mcs)	TARGET=mcs;;
		sis)	TARGET=sis;;
		*)	echo "Usage: Net mcs|sis";
			exit 1;;
	esac
.DE
.DM
	# disable the port
	echo Disabling port $RMODEM
	/etc/disable $RMODEM
.DE
.DM
	# invoke "net" for the target network
	net startup.$TARGET
.DE
.DM
	# reset the modem and port after a "net" session:
	echo Resetting the modem
.DE
.DM
	# get the modem's attention
	{
	  sleep 2
	  cat >&1 <<\!
	  +++
	  !
	} <$LMODEM
.DE
.DM
	# hang up the phone
	{ 
	  sleep 3
	  cat >&1 <<\!
	  ATH0
	  !
	} <$LMODEM
.DE
.DM
	# reload the standard modem settings
	{ stty 9600
	  sleep 2
	  cat >&1 <<\!
	  ATZ
	  !
	} <$LMODEM
.DE
.DM
	# re-enable the port
	echo Re-enabling port $RMODEM
	/etc/enable $RMODEM
.DE
.DM
	# end of Net
.DE
.SH "Where To Go From Here"
.PP
The next chapter in this manual describes
.BR cohulip 's
configuration files in some detail.
.PP
The chapter after that, \fIRunning net\fR, summarizes all of the commands
that the program
.B net
recognizes.
You may wish to use this chapter as a reference while you run
.BR cohulip .
.PP
Finally, the last chapter in this manual, \fIAdvanced cohulip\fR,
discusses advanced topics.
It discusses elaborate configuration, such as using
.B cohulip
as a firewall between two networks; and gives extended examples of
FTP session and debugging TCP datagrams.
