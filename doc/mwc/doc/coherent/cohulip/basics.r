.\" beginning is by fwb
.ds TL "TCP/IP Basics"
.ds TI COHULIP
.NH "TCP/IP History and Design"
.PP
Networking is one of the most exciting developments of computing in the 1990s.
Thanks to networking, each user on a personal computer can communicate
easily with other computers \(em either locally
(that is, computers in the same room or the same building)
or remotely
(that is, computers in other buildings, other cities, or other countries).
Networking places a world of information at your fingertips.
.PP
All of this power comes at a price, however:
networking is a complicated (and complex) topic.
It is possible to use a network without much study; but to set up networking
on your \*(CO machine and use it to its best advantage,
you must make the effort to learn how networking works.
.PP
This chapter introduces the basics of networking.
It is intended to give you a frame of reference, so that when you work with
.BR cohulip ,
you can use it to best advantage; and so that when a problem arises,
you will have a fighting chance of solving it yourself.
We hope that this chapter will cast some light on this subject for you,
and help you get the most out of
.BR cohulip .
.PP
.II "Hedrick, Charles L."
Please note that this chapter is based on the document
.I "Introduction to the Internet Protocols"
written by Charles L. Hedrick of Rutgers University.
The portions of that chapter taken from
his document are copyright \(co 1988 by Charles Hedrick,
and are used by permission.
.SH "What Is A Network?"
.PP
A major problem with discussing networking is that the term
.I network
has three different meanings:
.IP \fIHardware\fR 0.75i
The equipment that physically connects computers.
These can include Ethernet cards; serial cables (that is, a cable
strung directly from one computer's serial port to another's); and modems.
.IP \fIProtocols\fR
The rules by which computers communicate with each other.
These rules can either govern the behavior of hardware;
how systems package and unpackage data;
and how systems locate each other (also called \fIaddressing\fR).
.IP \fIServices\fR
The programs that actually perform networking tasks.
.PP
It is important to distinguish among these definitions of
.I network
when you think about networking.
.Sh "An Example of a Network"
.PP
For years, you have use a networked machine to exchange information
with persons at remote sites:
that is, you have used the telephone.
Consider what happens when make a telephone call:
.IP \(bu 0.3i
You initialize your system to transmit your data \(em i.e.,
you pick up the telephone receiver.
.IP \(bu
You wait for the network to notify you that it is ready to handle your
transmission \(em i.e., you listen for the dial tone.
.IP \(bu
You enter the seven-digit ``address'' of the remote system with which
you wish to exchange data \(em i.e., you dial the phone.
.IP \(bu
If you only know the other system's name, you must
translate that name into the remote system's address \(em that is, you must
look up your friend's name in your phone book and find his phone number.
.IP \(bu
If you do not have the number listed in your phone book, you must contact
a special remote system that will translate the name into the address \(em
that is, you telephone Directory Assistance.
.IP \(bu
The network interprets the seven-digit address and notifies the remote
system that a message is coming \(em i.e., it rings the other person's phone.
.IP \(bu
If the remote system is engaged with another site, the network notifies
you and you break off your transmission \(em i.e., you hear a busy signal
and hang up the phone.
.IP \(bu
Otherwise, the remote system engages the network to receive your
transmission \(em i.e., the person you are calling picks up the phone.
.IP \(bu
You and the remote system ``handshake'' to begin exchanging data \(em i.e.,
you and the person you are calling say ``Hello''.
.IP \(bu
You and the remote system identify each other \(em that is, the person you're
calling recognizes your voice, or you say who you are.
.IP \(bu
If the remote system does not recognize you
or it does not wish to receive your transmission
(perhaps you are selling aluminum siding or software),
it breaks off contact \(em i.e., the other person hangs up the phone.
.IP \(bu
If the conversation gets this far, data transmission commences \(em that is,
you begin to converse with other person.
.IP \(bu
When data transmission has ended, you disengage from the remote system \(em
i.e., when you're finished conversing, you say ``Bye''.
.IP \(bu
Finally, you disengage your system from the network \(em i.e., you hang up
the phone.
.PP
It may sound odd, but when two computers exchange data over a network,
they go through many of the same steps as you did when you made your
telephone call:
.IP \(bu 0.3i
Engage the physical device (either the Ethernet card or the modem).
.IP \(bu
Address the remote system.
.I Addressing
usually involved translating a name that you understand (e.g.,
.BR raven.alaska.edu )
into a number that the computer can understand (e.g.,
.BR 137.229.10.39 ).
.II "name server"
.II "domain-name server"
Your computer may be able to look up the number in one of its own files,
or it may be have to call a special remote system (called the
.IR "domain-name server" ),
which is the computer equivalent of Directory Assistance.
.IP \(bu
When the remote system is engaged, your system and the remote system
identify each other.
.IP \(bu
Data are exchanged.
.IP \(bu
When transmission concludes, the systems disengage.
.IP \(bu
Your system then either dials another system or disengaged from the network.
.PP
As with a computer network, you telephone conversation also involves
hardware, protcols, and services.
.IP \(bu 0.3i
The hardware is represented by the telephone itself and the switching
system that connected your two systems.
You need to know how to dial your telephone, but you do not need to worry
about how the switching system works; in the same way, you need to know
how to manipulate your modem or Ethernet card, but you need not worry about
all of the details of the intervening software.
.IP \(bu
Protocols affect many aspects of your conversion.
Protocol determines that a telephone's ``address'' is a seven-digit number.
Protocol also determines that the address is written with a hyphen between
the third and fourth digits.
Protocol also determines what you say to introduce yourself to a remote
system, how you introduce yourself, and what you say to disengage from the
remote system.
.IP \(bu
In this analogy, you take the place of the system, as you and
your knowledge control the telephone and the conversation.
However, for a computer to carry on a networking conversation, a program
must be written for each task that you perform \(em from looking up an
address, to dialing the telephone, to to conversing with the remote system.
.PP
The analogy between a telephone and computer network is not exact.
For example, your system can ``converse'' with multiple remote systems
simultaneously.
However, as you read this chapter,
you should remember that you have been using a network all of your life:
.I
Networking software simply instructs a computer to perform the same tasks
that you perform when you make a telephone call.
.R
In brief, if you can use a telephone, you should be able to grasp the
principles of networking; and if you can grasp those basics,
you should be able to grasp what
.B cohulip
does, and why.
.Sh "Protocols, the Internet, and TCP/IP"
.PP
Much has been written about the Internet and TCP/IP.
However,
what is not much appreciated is the fact that underlying these creations
are \fIprotocols\fR \(em documents, really \(em that describe how the Internet
and its services work.
These protocols are the result of years of work, much of it sponsored by
the federal government.
.PP
.II ARPA
.II "Advanced Research Projects Agency"
.II DARPA
.II "Defense Advanced Research Projects Agency"
.II "U.S. Department of Defense"
Research in networking was begun in the 1960s by the U.S. Department of
Defense's Advanced Research Projects Agency (ARPA) \(em later renamed
the Defense Advanced Research Projects Agency (DARPA).
The first network became operational in December 1969; it had four systems.
.II ARPAnet
This network, after development and expansion, became known as
.IR ARPAnet .
.PP
ARPA wrote, or commissioned, a set of protocols that describe how
ARPAnet works.
The most accurate name for the set of protocols we are describing is the
.IR "Internet protocol suite" ;
however, they now are almost universally called
.IR TCP/IP ,
after two of the most commonly used of the protocols:
.I TCP
(the ``Transmission Control Protocol''), and
.I IP
(the ``Internet Protocol'').
.PP
After it shook the kinks out of ARPAnet, the Defense Department created
other networks:
MILNET, to connect military installations in the United States; and
MINET, to connect installations in Europe.
.II NSF
.II "National Science Foundation"
.II CSNET
In 1980, the National Science Foundation (NSF) used the TCP/IP protocols
to create its Computer Science Network (CSNET), to link universities in the
United States; and later created NSFNET to link supercomputers.
Other networks include regional networks such as NYsernet,
local networks at a number of university and research institutions,
and a number of other military networks.
All of these networks, in turn, are connected to each other.
Thus, users can send messages from any of them to any other,
except where there are security or other policy restrictions on access.
.II Internet
Networks proliferate continually; and
this profusion of interlinked networks has grown into what we call the
.IR Internet .
.PP
.\" begin text by Charles Hedrick
.II "Defense Data Network"
.II "DDN^definition"
The set of Internet networks that are managed by the Department of Defense
is called the Defense Data Network (DDN).
This includes some research-oriented networks,
such as the ARPAnet, as well as more strictly military ones.
(Because much of the funding for Internet protocol developments is done 
via the DDN organization, the terms ``Internet'' and ``DDN''
can sometimes seem equivalent.)
.PP
The rest of this chapter discusses the TCP/IP protocols.
Other networking protocols have been written by private industry;
examples include AppleTalk and Novell networks.
However, TCP/IP is the ``native language'' of the Internet;
and these are the protocols that
.B cohulip
implements.
What follows
is not intended to be a complete description of these protocols;
but it can give you a reasonable idea of what they can do.
.PP
If you need to know the details of any TCP/IP protocols,
you will want to read the protocol document yourself.
Throughout this chapter, you will find references to the standards, in the
form of ``RFC'' or ``IEN'' numbers.
These are document numbers.
The final section of this chapter says how you can get copies
of these standards.
.PP
One last note:
from time to time, a protocol and a service will have the same name.
For example, the Internet's file-transfer protocol (FTP) describes how
to transfer files between machines; and the client program named
.B ftp
implements the FTP to transfer files.
We distinguish protocols from programs by printing the protocol's
acronym in capital letters and Roman type (e.g., ``FTP'') and printing
the program's name in lower-case letters and bold type (e.g.,
.BR ftp ).
.SH "Basic TCP/IP Protocols"
.PP
.II "Internet^protocol suite"
As mentioned above, what we call
.I TCP/IP
actually is a family of protocols.
A few provide low-level functions that are needed by many applications.
These include IP, TCP, and UDP.
(These will be described in a bit more detail later.)
Others are protocols for doing specific tasks, e.g., transferring files
between computers, sending mail, or finding out who is logged onto
another computer.
.PP
Initially, TCP/IP was used mostly between minicomputers or mainframes.
These machines had their own disks, and generally were self-contained.
Thus, the most important traditional TCP/IP services are
file transfer (FTP), remote login (TELNET), and computer mail (SMTP).
.Sh "FTP: File Transfer Protocol"
.PP
.II "FTP^definition"
.II "file transfer protocol"
.II "protocol^file transfer"
The file-transfer protocol (FTP) allows a user on any computer to exchange
files with another computer.
He must log into the remote computer by specifying
a user name and password.
FTP provides for transferring files between machines that have
different character sets, end-of-line conventions, etc.
.PP
.II "network file system"
.II NFS
.II NetBIOS
This is not quite the same thing as more recent network-file system (NFS)
or NetBIOS protocols.
Rather, you must invoke FTP any time you want to access a file
from another system; copy the file to your own system;
then work with your local copy.
.II "RFC 959"
See RFC 959 for specifications for FTP.
.Sh "TELNET: Remote Login"
.PP
.II "TELNET^definition"
.II "network terminal protocol"
.II "protocol^network terminal"
The network terminal protocol (TELNET) lets a user log onto another 
computer on the network.
You start a remote session by specifying a computer to connect to.
From that time until you finish the session, anything you type
is sent to the other computer.
.PP
Note that you are really still talking to your own computer;
but the
.B telnet
program effectively makes your computer invisible while it is running.
Every character you type is sent directly to the other system.
Generally, the connection to the remote computer behaves much like a
dialup connection;
that is, the remote system asks you to log in and give a password,
in whatever manner it would normally ask a user who had just dialed it up.
When you log off of the other computer,
.B telnet
exits, and once again what you type goes to your local computer.
.PP
.II "RFC 854"
.II "RFC 855"
.II Telenet
See RFCs 854 and 855 for specifications for TELNET.
By the way, do not confuse TELNET protocol with Telenet,
a vendor of commercial network services.
.Sh "SMTP: Simple Mail Transfer Protocol"
.PP
.II "SMTP^definition"
.II "simple mail transfer protocol"
.II "protocol^simple mail transfer"
.II mail
SMTP allows you to send messages to users on other computers.
Originally, people tended to use only one or two specific computers.
They would maintain ``mail files'' on those machines.
The computer mail system is simply a way for you to add a
message to another user's mail file.
.II "RFC 821"
.II "RFC 822"
.\" .II "RFC 937"
See RFCs 821 and 822 for specifications for computer mail.
.Sh "Client/Server Architecture"
.PP
The above services should be present in any implementation of TCP/IP,
and they still play a very important role in TCP/IP-based networks.
However, the way in which networks are used has been changing.
The older model of a number of large, self-sufficient computers is
beginning to change.
Now many installations have several kinds of computers,
including microcomputers, workstations, minicomputers, and mainframes.
A computer now is likely to be configured to perform a specialized task.
Although people are still likely to work with one specific computer,
that computer will call on other systems on the network for specialized
services.
.PP
.II "client/server model^definition"
.II server^definition
.II client^definition
This has led to the server/client model of network services.
A
.I server
is the program that provides a specific service to the rest of the network.
A
.I client
is the program that requests the service.
(Note that the server and client need not be on different computers.
They could be different programs running on the same computer;
for example, in \*(CO's implementation of the X Window System, the X server
and its clients both reside on the same machine.)
.PP
The following describes
some of the servers typically present in a modern computer setup.
Note that these computer services can all be provided
within the framework of TCP/IP:
.IP "\fINFS and NetBIOS\fR"
These permit a system to access files on another computer
in a fashion that is more closely integrated than FTP permits.
A network-file system provides the illusion that disks or other
devices on one system are directly connected to another.
You do not have to use a special network utility like FTP
to access a file on another system:
your computer simply thinks it has some extra disk drives.
These extra ``virtual'' drives refer to the other system's disks.
.IP
This capability is useful for several different purposes.
It lets you place large disks on a few computers,
but let all systems on the network access them.
Aside from the obvious economic benefits,
it makes system maintenance and backup easier, because you
do not need to worry about updating and backing up copies on many
different machines.
A number of vendors now offer high-performance diskless computers.
These computers have no disk drives at all.
.II "file server"
They depend entirely upon disks attached to a common ``file server.''
.IP
.II "RFC 1001"
.II "RFC 1002"
.II "NetBIOS^definition"
See RFCs 1001 and 1002 for a description of PC-oriented NetBIOS over TCP.
.II "NFS^definition"
In the workstation and minicomputer area, Sun's
Network File System (NFS) is more likely to be used.
Protocol specifications for it are available from Sun Microsystems.
.IP "\fIRemote Printing\fR"
This allows you to access printers on other
computers as if they were directly attached to yours.
.II "remote lineprinter protocol"
.II "protocol^remote lineprinter"
The most commonly used protocol is the remote line-printer protocol from
Berkeley \*(UN.
Unfortunately, there is no protocol document for this.
However, the C code is widely available, so implementations are common.
.IP "\fIRemote Execution\fR"
.II "remote execution"
This allows you to request that a particular
program be run on a different computer.
This is useful when you can
do most of your work on a small computer, but a few tasks require the
resources of a larger system.
.IP
There are a number of different kinds of remote execution.
Some operate on a command-by-command basis:
that is, you request that a specific command or set of commands
should run on some specific computer.
(More sophisticated versions will choose a system that happens to be free.)
.II "remote procedure calls"
However, there are also ``remote procedure call'' systems that allow a
program to call a subroutine that will run on another computer.
There are many protocols of this sort.
.II rsh
.II rexec
Berkeley \*(UN contains two servers to execute commands remotely:
.B rsh
and
.BR rexec .
.IP "\fIName Servers\fR"
.II "name server"
In large installations, there are a number of different collections
of names that have to be managed.
This includes users and their passwords, names and network addresses for
computers, and accounts.
It becomes very tedious to keep these data up to date on all of the computers.
Thus, the data bases are kept on a small number of systems.
Other systems access the data over the network.
.IP
.II "RFC 822"
.II "RFC 823"
.II "name-server protocol"
.II "protocol^name server"
RFC 822 and 823 describe the name-server protocol used to
keep track of host names and Internet addresses on the Internet.
This is now a required part of any TCP/IP implementation.
.II "IEN 116"
IEN 116
describes an older name-server protocol that is used by a few
terminal servers and other products to look up host names.
.IP
.II "Yellow Pages"
Sun's Yellow Pages system is designed as a general mechanism to handle user
names, file-sharing groups,
and other data bases commonly used by \*(UN systems.
It is widely available commercially.
Its protocol definition is available from Sun.
.IP "\fITerminal Servers\fR"
.II "terminal server"
Many installations no longer connect terminals directly to computers.
Instead, they connect them to terminal servers.
A terminal server is simply a small computer that knows only the
TELNET protocol (or some other protocol) to do remote login,
plus name service and a few other protocols.
.IP
A user who is connected to one of these simply types the name of a computer,
the server connects him to it.
Most servers permit you to have more than one connection active at a time;
they can switch between connections rapidly, and notify a user
when output is waiting on another connection.
.IP "\fINetwork-Oriented Windowing Systems\fR"
Until recently, high-performance graphics programs had to execute
on a computer that had a bit-mapped graphics screen directly attached to it.
Network windowing systems allow a program to use a display
on a different computer.
Full-scale network windowing systems provide an interface that lets
you distribute jobs to the systems that are best suited to handle them,
but still give you a single, graphically based user interface.
.IP
.II "X Window System"
The most widely-implemented windowing system is the X Window System.
.II "Project Athena"
.II "Massachusetts Institute of Technology"
.II MIT
A protocol description is available from the Massachusetts Institute of
Technology's Project Athena.
A reference implementation is publicly available from MIT.
.II NeWS
A number of vendors are also supporting NeWS,
a windowing system defined by Sun.
Both of these systems are designed to use TCP/IP.
.PP
Note that some of the protocols described above were designed by Berkeley, 
Sun, or other organizations.
Thus, they are not officially part of the Internet protocol suite.
However, they are implemented using TCP/IP, just as normal TCP/IP
application protocols are.
Because the protocol definitions are not considered proprietary,
and because commercially supported implementations are widely available,
it is reasonable to think of these protocols as being effectively part
of the Internet suite.
.PP
The above list gives services most commonly available through TCP/IP.
Most other protocols are specialized facilities for
retrieving information of various kinds, such as who is logged in on a
given system, the time of day, etc.
.II "RFC 1011"
If you need a facility that is not listed here, we encourage you
to look through the current edition of Internet Protocols
(currently RFC 1011), which lists all of the available protocols, and also
to look at some of the major TCP/IP implementations to see what various
vendors have added.
.SH "Structure of the TCP/IP Protocols"
.PP
TCP/IP is a layered set of protocols.
To understand what this means, it is useful to look at an example.
A typical situation is sending mail.
.IP \(bu 0.3i
First, there is a protocol for mail.
This defines a set of commands which one machine sends to another, e.g.,
commands to specify who the sender of the message is,
whom it is being sent to, and then the text of the message.
However, this protocol assumes that there is a way to
communicate reliably between the two computers \(em
like other application protocols,
it simply defines a set of commands and messages to be sent.
.IP \(bu
Mail hands its output to TCP.
TCP is responsible for making sure that data get through to the other end.
It keeps track of what data are sent, and retransmits anything
that did not get through.
If a message is too large to send in one burst, TCP breaks it into
pieces that it dispatches separately.
It also listens for confirming messages from the remote machine, to make
sure that all arrive data arrived correctly.
.IP
Because these functions are needed for
many applications, they are put together into a separate protocol, rather
than being part of the specifications for sending mail.
You can think of
TCP as forming a library of routines that applications can use when they
need reliable network communications with another computer.
.IP \(bu
TCP, in turn, calls on the services of IP.
Although the services that TCP supplies
are needed by many applications, there are still some kinds of
applications that don't need them.
However there are some services that every application needs.
So these services are put together into IP.
As with TCP, you can think of IP as a library of routines that TCP calls on,
but which is also available to applications that don't use TCP.
.IP \(bu
Finally, IP passes the data to the device that physically dispatches the
data to the remote system \(em either an Ethernet card or a serial port.
.PP
This strategy of building several levels of protocol is called
.IR layering .
We think of the applications programs such as mail, TCP, and IP, as each being
separate ``layers,'' each of which calls on the services of the layer below it.
.PP
Generally, TCP/IP applications use four layers:
.IP \(bu 0.3i
An application protocol such as
.BR mail .
.IP \(bu
A protocol, such as TCP, which packages data and monitors their transmission
to the remote site.
.IP \(bu
IP, which provides the basic service of getting datagrams to their destination.
.IP \(bu
The protocols needed to manage a specific physical medium, such as 
Ethernet or a serial line.
.PP
The system that receives the message reverses the process:
.IP \(bu 0.3i
The physical device receives the data.
.IP \(bu
The data are handed to IP, which confirms that they have arrived at the
correct destination.
.IP \(bu
IP hands the data to TCP, which confirms that all data have arrived,
send messages back to the sender to confirm the arrival of the data,
and assembles the datagrams back into one message.
.IP \(bu
TCP hands its reassembled message to the mail application, which handles
it \(em either pops it into a user's mailbox or forwards it to another
system, whichever is required.
.PP
.II "networking^ARPAnet model"
This four-layer model of networking is called the
.IR "ARPAnet model" ,
as it was developed by ARPA.
The terms ``TCP/IP model'' and ``ARPAnet model'' are used interchangeably,
although, as we noted above, TCP and IP are only two of the many
ARPAnet protocols.
.II ISO
.II "International Standards Organization"
.II OSI
.II "networking^OSI model"
.II "networking^ARPAnet model"
.II "Open Systems Interconnect model"
The International Standards Organization (ISO) has published a seven-layer
model of networking, called the Open Systems Interconnect (OSI) model.
.II "layer cake"
.II "cake, layer"
These models are sometimes called ``layer cakes,'' because their layers
are stacked upon each other.
The following diagram compares the OSI and ARPAnet models:
.ie p .PH 1 1 \\*(XD/osi.eps
.el \{\
.DM
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          Application          |                               |
   |                               |                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
   |          Presentation         |          Application          |
   |                               |          (e.g., ftp)          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
   |             Session           |-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                               |                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+          Host-to-Host         |
   |            Transport          |             (TCP)             |
   |                               |                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |             Network           |          Internet (IP)        |
   |                               |-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
   |            Data Link          |                               |
   |                               |          Network Access       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+         (Ethernet, SLIP)      |
   |             Physical          |                               |
   |                               |                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               OSI Model                      ARPAnet Model
.DE
\}
.PP
.II "TCP/IP stack"
The literature and advertisements speak of a ``TCP/IP stack.''
This term originates in the stacked design of the ARPAnet and OSI models.
.PP
Each ``layer'' in the TCP/IP ``layer cake'' usually is implemented as a
separate program.
.BR cohulip ,
however, combines the first three layers into one program, called
.BR net .
.PP
Although the OSI model is presented throughout the literature of networking,
we will not discuss it further here, as it does not directly apply to TCP/IP.
The above diagram lets you compare OSI's seven-layer stack with
ARPAnet's four layers.
.Sh "TCP and IP Tasks"
.PP
.II "catenet model"
.II "networking^catenet model"
TCP/IP is based on the ``catenet model.''
This model assumes that there are a large number of independent
networks connected by gateways.
The user should be able to access computers or other resources
on any of these networks.
.PP
Datagrams often pass through a dozen different networks before
getting to their final destination.
The routing needed to accomplish this should be completely
invisible to the user.
.II "Internet address^definition"
As far as the user is concerned,
all he needs to know in order to access another system is an
.IR "Internet address" .
This is a address that looks like
.BR 128.6.4.194 .
It is actually a 32-bit number.
However, it is normally written as four decimal numbers,
each representing eight bits of the address.
.II octet
(We will use the usual term ``byte'' for such eight-bit chunks.
Please note that the Internet documentation calls them ``octets,''
because TCP/IP is supported by some computers that have byte
sizes other than eight bits.)
.PP
Generally, the structure of the address gives you
some information about how to get to the system.
For example, 128.6 is a network number assigned
by a central authority to Rutgers University.
Rutgers uses the next byte to indicate which of the campus Ethernets is
involved.
128.6.4 happens to be an Ethernet used by the Computer Science department.
The last byte allows for up to 254 systems on each Ethernet.
(It is 254 instead of 256 because values 0 and 255 are not allowed,
for reasons that will be discussed later.)
Note that 128.6.4.194 and 128.6.5.194 would be different 
systems.
The structure of an Internet address is described in a bit more detail later.
.PP
Of course, people normally refer to systems by name,
rather than by Internet address.
When we specify a name, the network software looks it up in a data base,
and comes up with the corresponding Internet address.
Most of the network software deals strictly in terms of the address.
.II "RFC 882"
(RFC 882 describes the name-server technology used to handle this lookup.)
.PP
TCP/IP is built on
.I connectionless
technology.
Information is transferred as a sequence of datagrams.
.II datagram^definition
A
.I datagram
is a collection of data that is sent as a single message.
Each of these datagrams is sent through the network individually.
There are provisions to open connections
(i.e., to start a conversation that will continue for some time).
However, at some level, information from those connections is
broken up into datagrams, and each of those datagrams are treated
by the network as completely separate from all others.
.PP
For example, suppose you want to transfer a 15,000-byte file.
Most networks can't handle a 15,000-byte datagram.
So the protocols will break this up into something like 30 500-byte datagrams.
Each of these datagrams will be sent to the other end.
At that point, they will be reassembled into the 15000-byte file.
However, while those datagrams are in transit, the network doesn't know
that there is any connection between them.
It is perfectly possible that datagram 14 will
arrive before datagram 13.
It is also possible that somewhere in
the network, an error will occur and some datagram won't get through at all.
In that case, that datagram has to be sent again.
.PP
.II packet^definition
Note by the way that the terms
.I datagram
and
.I packet
often are used synonymously; however,
datagram is the correct word to use when discussing TCP/IP.
A datagram is a unit of data, which is what the protocols deal with.
A packet is a physical thing that appears on an Ethernet or some wire.
In most cases, a packet simply contains a datagram,
so there is very little practical difference between them.
However, they can differ.
When TCP/IP is used on top of X.25,
the X.25 interface breaks the datagrams up into 128-byte packets.
This is invisible to IP, because the packets are reassembled
into a single datagram at the other end before being processed by TCP/IP.
So in this case, one IP datagram would be carried by several packets.
However, most media take advantage of the fact that it is more efficient to
to send one datagram per packet, so the distinction tends to vanish.
.PP
Two separate protocols are involved in handling TCP/IP datagrams.
.II TCP^definition"
TCP is responsible for breaking up the
message into datagrams, reassembling them at the other end,
resending anything that gets lost, and putting things back in the right order.
.II IP^definition
IP is responsible for routing individual datagrams \(em that is, finding the
address of the remote system, and ensuring that the datagrams are dispatched
to it.
.PP
It may seem like TCP is doing all the work.
In a small network, this is true.
However, in the Internet, simply getting a datagram to its destination
can be a complex job.
A connection may require the datagram to go through several
networks at your local site, a fiber-optic line to the major center,
a couple of Ethernets there, then a series of 56-kilobaud telephone lines
to another NSFnet site, and more Ethernets elsewhere.
Keeping track of the routes to all of the destinations and handling
incompatibilities among different transport media turns out to be
a complex job.
.PP
The interface between TCP and IP is fairly simple:
TCP simply hands IP a datagram with a destination, and IP delivers it.
IP doesn't know, or care,
how this datagram relates to any datagram that precedes it or follows it.
.PP
It may have occurred to you that something is missing here.
We have talked about Internet addresses, but not about how you keep track of
multiple connections to a given system.
Clearly it isn't enough to get a datagram to the right destination.
TCP has to know which connection this datagram is part of.
.II demultiplexing^definition
This task is referred to as
.IR demultiplexing .
In fact, there are several levels of demultiplexing going on in TCP/IP.
.II "TCP/IP^headers
The information needed to do this demultiplexing is contained in a series of
.IR headers .
.PP
A header is simply a few extra bytes tacked onto the
beginning of a datagram by some protocol in order to keep track of it.
It's a lot like putting a letter into an envelope and putting an address
on the outside of the envelope;
except with modern networks it happens several times.
It's like you put the letter into a little envelope, your
secretary puts that into a somewhat bigger envelope, the Post Office
puts that envelope into a still bigger one, and so on.
.PP
The following is an overview of the headers that get stuck on a
message that passes through a typical TCP/IP network.
.Sh "The TCP Level"
.PP
First, we will discuss the TCP level.
We start with a single data stream, say, a file you are trying to send to
some other computer:
.DM
	......................................................
.DE
.PP
TCP breaks it up into manageable chunks.
To do this, TCP must know how large a datagram your network can handle.
Actually, the TCP at each end says how big a datagram it can handle;
then together they pick the smaller size:
.DM
	....  ....  ....  ....  ....  ....  ....  ....
.DE
.PP
TCP puts a header at the front of each datagram.
This header actually contains at least 20 bytes,
but the most important ones are a source and destination
.IR "port number"
and a
.IR "sequence number" .
.PP
.II "TCP^port numbers"
.II "ports"
TCP uses the port numbers to keep track of different conversations.
Suppose three different people are transferring files.
Your TCP might allocate port numbers 1000, 1001, and 1002 to these transfers.
When you are sending a datagram, this becomes the source port number,
because you are the source of the datagram.
Of course, the TCP at the other end has assigned a port number
of its own for the conversation.
Your TCP has to know the port number used by the other end as well.
(It finds out when the connection starts, as we will explain below.)
It puts this in the destination port field.
Of course, if the other end sends a datagram back to you, the source and
destination port numbers will be reversed, because then it will be the
source and you will be the destination.
.PP
.II "TCP^sequence number"
Each datagram has a sequence number.
This is used so that the other end can make sure that it gets the datagrams
in the right order, and that it hasn't missed any.
TCP doesn't number the datagrams, but the bytes.
So if there are 500 bytes of data in each datagram,
the first datagram might be numbered 0, the second 500,
the next 1,000, the next 1,500, etc.
.PP
.II TCP^checksum
TCP attaches a
.IR checksum .
This is a number that is computed by adding up
all the bytes in the datagram (more or less \(em see the TCP specification).
The result is put in the header.
TCP at the other end computes the checksum again.
If they disagree, something bad happened to the datagram in transmission,
and TCP throws it away.
The datagram now appears as follows:
.ie p .PH 1 1 \\*(XD/tcp_header.eps
.el \{\
.DM
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Source Port           |        Destination Port       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Sequence Number                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Acknowledgment Number                     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | Data  |           |U|A|P|R|S|F|                               |
   | Offset| Reserved  |R|C|S|S|Y|I|             Window            |
   |       |           |G|K|H|T|N|N|                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Checksum            |          Urgent Pointer       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    your data ...  next 500 bytes                              |
   | ......                                                        |
.DE
\}
.PP
The following describes each byte in the TCP header:
.IP \fB0-1\fR
The number of source port, binary format.
This usually is a number that the sender selects at random,
to ensure that each session moving between the two hosts is uniquely identified.
.IP \fB2-3\fR
Destination port, binary format.
This usually is a ``well-known port.''
The number of this port varies, depending upon the server on the remote
host for which this datagram is meant.
.IP \fB4-7\fR
Sequence number:
the sender's location in his data stream.
.IP \fB8-11\fR
Acknowledgement number:
the sender expects this byte next.
.IS \fB12-13\fR
.II "TCP^urgent field"
Header length, and code bits (bit fields).
The
.I urgent
field lets one end tell the other to skip ahead in its processing
to a particular byte.
This is often useful for handling asynchronous events \(em for example,
when you type a command to abort data transmission.
.IP \fB14-15\fR
.II "TCP^window field"
Window, which controls how much data can be in transit at any one time.
It is not practical to wait for each datagram to be acknowledged before sending 
the next one:
That would slow things down too much.
On the other hand, you can't just keep sending,
or a fast computer might overrun the capacity of a slow one to absorb data.
As the computer receives data, the amount of space left in its window decreases.
When it goes to zero, the sender has to stop.
.IP
As the receiver processes the data, it increases its window,
indicating that it is ready to accept more data.
Often the same datagram can be used
to acknowledge receipt of a set of data and to give permission for
additional new data (by an updated window).
.IP \fB16-17\fR
Checksum.
.IP \fB18-19\fR
Urgent pointer.
It points to out-of-band error messages.
This field usually is not used.
.PP
If we abbreviate the TCP header as
.BR T ,
the whole file now looks like this:
.DM
	T....  T....  T....  T....  T....  T....  T....
.DE
.PP
To make sure the datagram has arrived at its destination,
the recipient must send back an
.IR acknowledgment .
.II "TCP^acknowledgement number"
This is a datagram whose acknowledgment-number field is filled in.
For example, sending a packet with an acknowledgment of ``1500''
indicates that you have received all the data up to byte number 1500.
If the sender doesn't get an acknowledgment within a reasonable
amount of time, it sends the data again.
.PP
For a fuller discussion of the TCP header,
see the chapter \fIAdvanced cohulip\fR, later in this manual.
.Sh "The IP level"
.PP
TCP sends each of these datagrams to IP.
Of course it has to tell IP the Internet address of the computer
at the other end.
Note that this is all IP is concerned about.
It doesn't care about what is in the datagram, or even in the TCP header.
IP's job is simply to find a route for the datagram and get it
to the other end.
.PP
To allow gateways or other intermediate systems to forward the datagram,
IP adds its own header:
.ie p .PH 1 1 \\*(XD/ip_header.eps
.el \{\
.DM
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|            Total Length       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|     Fragment Offset     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Source Address                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Destination Address                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  TCP header, then your data ......                            |
   |                                                               |
.DE
\}
.PP
The following describes each byte in the IP header:
.IP "\fB0\fR"
Version and header length (bit fields).
Almost always 0x45.
.IP "\fB1\fR
Service type.
This usually is zero.
.IP "\fB2-3\fR"
Total length.
This varies, depending upon the size of the data field that the two systems
negotiated.
.IP "\fB4-5\fR
Datagram identifier.
.IP "\fB6-7\fR
Flags (bit fields) and fragment offset.
This and the previous field help to manage fragmentation of
datagrams \(em that is, when a site that lies between you and the remote
host must break
a datagram into pieces because the datagram is too large for it to handle.
This is discussed below.
.IP "\fB8\fR"
.II "IP^time to live"
.II "time to live"
Time to live.
This is a number that is decremented whenever the datagram
passes through a system.
When it goes to zero, the datagram is discarded.
This is to prevent a datagram from wandering endlessly through a network
should a loop develop.
This should be impossible, but well-designed networks are built
to cope with ``impossible'' conditions.
.IP "\fB9\fR"
.II "IP^protocol number"
Protocol number.
The protocol number tells IP at the other end to send the datagram to TCP.
Although most IP traffic uses TCP, there are other protocols that can use IP,
so you have to tell IP which protocol to send the datagram to.
Value 0x06 indicates that this is a TCP datagram, 0x01 indicates an
ICMP (\fBping\fR) datagram; and 0x11 indicates a UDP datagram.
.IP "\fB10-11\fR"
.II "IP^checksum"
Header checksum.
This allows IP at the other end to verify
that the header wasn't damaged in transit.
Note that TCP and IP have separate checksums.
IP needs to verify that the header wasn't damaged in transit,
or it could send a message to the wrong place.
It is both more efficient and safer to have TCP compute a separate
checksum for the TCP header and data.
.IP "\fB12-15\fR"
.II "IP^addresses"
The sender's IP address \(em that is,
the IP address of your machine \(em in binary.
This is necessary so the other end knows where the datagram came from.
Note that the binary number is in network byte-order.
.IP "\fB16-19\fR"
The recipient's IP address, in binary.
The destination Internet address is the address of the other machine.
This is necessary so any gateways in the middle know where you
want the datagram to go.
Note that the binary number is in network byte-order.
.PP
If we represent the IP header by an `I', your file now looks like this:
.DM
	IT....  IT....  IT....  IT....  IT....  IT....  IT....
.DE
.PP
Once IP has added its header to a datagram,
it's possible that no more headers are needed.
.\" This paragraph by fwb.
However, your machine uses a physical interface into the network.
.B cohulip
supports two such interfaces:
Ethernet, and the serial-line interface protocol (SLIP).
Each attaches its own header to a datagram.
We will discuss each in turn.
.Sh "The Ethernet Level"
.PP
.II "Ethernet^headers"
Ethernet is a
.IR "broadcast medium" .
That is, it is in effect like an old-fashioned party-line telephone:
when you send a packet out on the Ethernet,
every machine on the network sees the packet.
Something is needed to make sure that the right machine gets the packet.
As you might guess, this involves the Ethernet header.
.PP
Every Ethernet packet has a 14-byte header that includes the
source and destination Ethernet address, and a type code.
(This header is attached to each datagram by the Ethernet card itself:
.B cohulip
contains no code either to read or to write these headers.)
Each Ethernet card pays attention only to packets with its own Ethernet
address in the destination field.
.II "Ethernet^addresses"
Unfortunately, Ethernet has its own addresses, which are different
from those used by IP.
The people who designed Ethernet wanted to make sure that no
two machines would end up with the same Ethernet address.
Furthermore,
they didn't want the user to have to worry about assigning addresses,
so each Ethernet card comes from the factory with its address built in.
To make sure that they would never have to reuse addresses, the
Ethernet designers allocated 48 bits for the Ethernet address.
People who make Ethernet equipment have to register with a central
authority, to make sure that the numbers they assign don't overlap any
other manufacturer.
.PP
It is important to remember that there is no connection between
a machine's Ethernet address and its Internet address.
Each machine must have a table that maps a given Ethernet
address to its corresponding Internet address.
(We will describe how this table is constructed a bit later.)
.PP
.II "Ethernet^type code"
In addition to the addresses, the header contains a type code.
The type code is to allow for several different protocol families
to be used on the same network.
So you can use TCP/IP, DECnet, Xerox NS, etc., all at the same time.
Each of them will put a different value in the type field.
.PP
.II "Ethernet^checksum"
Finally, there is a checksum.
The Ethernet controller computes a checksum of the entire packet.
When the other end receives the packet,
it recomputes the checksum, and throws the packet away if the answer
disagrees with the original.
The checksum is put on the end of the 
packet, not in the header.
.PP
The final result is that your message looks like this:
.ie p .PH 1 1 \\*(XD/ethernet.eps
.el \{\
.DM
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |       Ethernet destination address (first 32 bits)            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | Ethernet dest (last 16 bits)  |Ethernet source (first 16 bits)|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |       Ethernet source address (last 32 bits)                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        Type code              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | IP header, then TCP header, then your data                    |
   |                                                               |
      ...
   |                                                               |
   |   end of your data                                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Ethernet Checksum                      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
.DE
\}
.PP
If we represent the Ethernet header with "E", and the Ethernet checksum 
with "C", your file now looks like this:
.DM
	EIT....C EIT....C EIT....C EIT....C EIT....C
.DE
.PP
When these packets are received by the other end, of course all the
headers are removed:
.IP \(bu 0.3i
The Ethernet interface removes the Ethernet header and the checksum.
It looks at the type code.
Because the type code is the one assigned to IP,
the Ethernet device driver passes the datagram to IP.
.IP \(bu
IP removes the IP header.
It looks at the IP protocol field.
Because the protocol type is TCP, it passes the datagram to TCP.
.IP \(bu
TCP now looks at the sequence number,
and uses it (and other information) to reassemble the file.
.PP
TCP then passes the reassembled file to the application for final disposal.
We will discuss later just
how TCP identifies the application to which it should pass the file,
and how it tells the application that it should process the file.
.\" This section is by fwb
.Sh "The SLIP Level"
.PP
.B cohulip
can use either of two physical channels to move datagrams from your
machine to another.
The first is Ethernet, which was discussed above.
The other is through a serial port, which accesses the other machine
either via a cable (i.e., ``direct link'') or via a modem.
To move datagrams through a serial port,
.B cohulip
uses SLIP (Serial-Line Internet Protocol).
.PP
SLIP is much simpler than Ethernet.
It does not add a header or checksum to a datagram.
.II "SLIP END"
Rather, after it receives the datagram from IP,
it merely writes the magic character SLIP END (0xC0) onto the
end of a datagram, to mark that the datagram has ended, and
sends it to the receiving machine.
.PP
Thus, a SLIP datagram looks like this:
.ie p .PH 1 1 \\*(XD/slip.eps
.el \{\
.DM
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        IP header                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        TCP header                             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                         your data                        |0xC0|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
.DE
\}
.PP
If the character SLIP END is part of the datagram itself, SLIP
prefaces it with the character SLIP ESC (0xDB),
causing the other machine to ignore that SLIP END character.
Likewise, if SLIP ESC is part of the datagram itself, SLIP follows it
with the character 0xDD, which tells SLIP on the
other machine to ignore that SLIP ESC character.
.PP
SLIP does not add its own addressing envelope,
because a serial line can pass only from one machine to another \(em
there's no way a message can go astray.
However, the lack of a checksum can create problems:
if a datagram is garbled in transmission, this fact must be discovered
by the IP or TCP level when it examines its own checksum, which is wasteful.
.PP
.\" resume text by Charled Hedrick
This ends our initial summary of TCP/IP.
There are still some crucial concepts we haven't gotten to,
so we'll now go back and add details in several areas.
.II "RFC 793"
.II "TCP^RFC"
.II "RFC^791"
.II "IP^RFC"
.II "RFC 894"
.II "RFC 826"
.II "Ethernet^RFCs"
For detailed descriptions of the items discussed here see, RFC 793 for TCP,
RFC 791 for IP, and RFCs 894 and 826 for sending IP over Ethernet.
.SH "Well-Known Ports and the Applications Layer"
.PP
So far, we have described how a stream of data is broken up into datagrams,
sent to another computer, and reassembled.
However, something more is needed before we can accomplish anything useful.
There must be a way for a human being to open a connection to a specified
computer, log into it, perform a task on that computer, receive its output,
and view it.
.II "applications protocols"
.II "protocols^applications"
This is done by
.IR "application protocols" .
.PP
The application protocols run ``on top'' of TCP/IP \(em that is,
when they want to send a message, they give the message to TCP.
TCP makes sure it gets delivered to the other end.
Because TCP and IP take care of all the networking details,
the applications protocols can treat a network
connection as if it were a simple byte stream,
like a terminal or telephone line.
.PP
Before going into more details about applications programs, we have to
describe how you find an application.
.PP
Suppose you want to send a file to
a computer whose Internet address is 128.6.4.7.
To start the process, you need more than just the Internet address:
you must connect to the FTP server at the other end.
In general, a networking program is written to perform a specific set of tasks.
Most systems have separate programs to handle file transfers,
remote terminal logins, mail, etc.
(\fBcohulip\fR is an exception to this rule.)
Thus when you connect to 128.6.4.7,
you must specify that you want to talk to the FTP server.
.II "ports^well known"
This is done by having a
.I "well-known port"
for each server.
.PP
Recall that TCP uses port numbers to track each conversation.
User programs normally are are assigned a port more or less at random.
However, the Internet assigns specific port numbers to the programs
that sit waiting for requests.
.PP
For example, if you want to send a file, you will start a program called
.BR ftp .
It opens a connection using some random number, say ``1234,''
for the port on its end.
However, it specifies port number 21 for remote host,
because 21 is the number of the well-known port for the FTP server.
.PP
As you can see, two different programs are involved.
You run
.B ftp
on your side.
This is a program designed to accept commands from your terminal
and pass them on to the other end.
Such a program is called a
.IR client .
The program that you talk to on the other machine accepts commands from
the client, executes them, and returns the output to the client.
Such a program is called a
.IR server .
The method of passing commands to a server via a client is the
.IR "client/server architecture"
about which so much is being written nowadays.
.PP
.\" resuming Charles Hedrick's text
.II "Assigned Numbers"
The official port numbers for each program are given in
.IR "Assigned Numbers" .
.\" fwb
The following gives the well-known ports that are used commonly:
.DS
	\fB7\fR	echo
	\fB9\fR	discard
	\fB20\fR	FTP data
	\fB21\fR	FTP control
	\fB23\fR	TELNET
	\fB25\fR	SMTP (mail)
	\fB79\fR	finger
	\fB87\fR	ttylink in NOS
	\fB540\fR	\*(UU
	\fB1234\fR	the UDP port used by remote
.DE
.\" Hedrick
Therefore, a connection is actually described by a set of four numbers:
the sender's Internet address and TCP port, and the recipient's
Internet address and TCP port.
Each datagram has all four numbers in it:
the Internet addresses are in the IP header,
and the TCP port numbers are in the TCP header.
.PP
No two connections can have the same set of numbers.
It is enough for any one number to be different.
For example, it is perfectly possible for two different users on a machine to
be sending files to the same other machine.
This could result in connections with the following parameters:
.DS
.ta 0.5i 1.5i 3.25i 5.0i
		\fIInternet addresses	TCP ports\fR
	\fIConnection 1\fB	128.6.4.194, 128.6.4.7	1234, 21\fR
	\fIConnection 2\fB	128.6.4.194, 128.6.4.7	1235, 21\fR
.DE
.PP
Because the same machines are involved, the Internet addresses are the same.
Because both sessions are performing file transfers, one end of the connection
involves the well-known port number for FTP.
The only thing that differs is the port number for the program
that the users are running; and that's enough of a difference.
.PP
Generally, at least one end of the connection asks the network software
to assign it a port number that is guaranteed to be unique.
Normally, it's the user's end because the server has to use a well-known number.
.PP
Now that we know how to open connections, let's get back to the
applications programs.
As mentioned earlier, once TCP has opened a
connection, we have something that might as well be a simple wire.
All the hard parts are handled by TCP and IP.
However, we still need to agree on what we send over this connection:
that is, what commands the application understands
and the format in which we send them.
Generally, what is sent is a combination of commands and data.
They use context to differentiate.
For example, the mail protocol works like this:
.IP \(bu 0.3i
Your mail program opens a connection to the mail server at the other end.
.IP \(bu
Your program gives it your machine's name, the sender of the message,
and the recipients you want it sent to.
.IP \(bu
It then sends a command saying that it is starting the message.
.IP \(bu
The other end stops treating what it sees as commands,
and starts accepting the message.
.IP \(bu
Your end then starts sending the text of the message.
.IP \(bu
At the end of the message, a special mark is sent (a dot in the first column).
After that, both ends understand that your program is again sending commands.
.PP
This is the simplest way to do things, and the one that most applications use.
.PP
File transfer is somewhat more complex.
The file-transfer protocol involves two different connections.
It starts out just like mail.
The user's program sends commands like ``log me in as this user,''
``here is my password,'' ``send me the file with this name.''
However, once the command to send data is sent, a second connection
is opened for the data itself.
It would certainly be possible to send the data on the same connection,
as mail does; but file transfers often take a long time.
The designers of the file-transfer protocol wanted to allow the user
to continue issuing commands while the transfer is going on.
For example, the user might make an inquiry, or he might abort the transfer.
Thus, the designers felt it was best to use a separate connection
for the data and leave the original command connection for commands.
(It is also possible to open command connections to two different computers,
and tell them to send a file from one to the other.
In that case, the data couldn't go over the command connection.)
.PP
Remote-terminal connections use yet another mechanism.
For remote logins, there is just one connection.
It normally sends data.
When it is necessary to send a command (e.g.,
to set the terminal type or to change some mode),
a special character is used to indicate that the next character is a command.
If the user happens to type that special character as data,
two of them are sent.
.PP
We are not going to describe the application protocols in detail in this
document.
It's better to read the RFCs yourself.
However, applications use a couple of common conventions
that will be described here.
First, the common network representation:
TCP/IP is intended to be usable on any computer.
Unfortunately, not all computers agree on how data are represented.
There are differences in character codes (ASCII vs. EBCDIC),
in end-of-line conventions (carriage return, line feed,
or a representation using counts), and in whether terminals expect characters
to be sent individually or a line at a time.
.PP
To allow computers of different kinds to communicate,
each application's protocol defines a standard representation.
Note that TCP and IP do not care about the representation.
TCP simply sends bytes.
However, the programs at both ends must agree on how the
bytes are to be interpreted.
The RFC for each application specifies the standard representation for that
application.
.II "net ASCII"
Normally, it is
.IR "net ASCII" .
This uses ASCII characters, with end of line denoted by a
carriage return followed by a line feed.
.PP
.II "standard terminal"
For remote login, there is also a definition of a
.IR "standard terminal" ,
which turns out to be a half-duplex terminal with echoing happening
on the local machine.
Most applications also make provisions for the two computers
to agree on other representations that they may find more convenient.
For example, PDP-10s have 36-bit words.
There is a way that two PDP-10s can agree to send a 36-bit binary file.
Likewise, two systems that prefer full-duplex terminal conversations
can agree on that.
However each application has a standard representation, which every 
machine must support.
.Sh "An Example Application: SMTP"
.PP
.II "simple mail transfer protocol"
.II "SMTP^definition"
To give a bit better idea what is involved in the application
protocols, we're going to show an example of the simple mail transfer
protocol (SMTP), which is the mail protocol.
We assume that a computer called
.B topaz.rutgers.edu
wants to send the following message:
.DM
	Date: Sat, 25 Jun 94 13:26:31 EDT
	From: hedrick@topaz.rutgers.edu
	To: levy@red.rutgers.edu
	Subject: meeting
.sp
	Let's get together Monday at 1pm.
.DE
.PP
.II "RFC 822"
First, note that the format of the message itself is described by an
Internet standard (RFC 822).
The standard specifies the fact that the message must be transmitted
as net ASCII
(i.e., it must be ASCII, with the end of each line of text marked by the
character pair CR/LF).
It also describes the general structure, as a group of header lines,
then a blank line, and then the body of the message.
Finally, it describes the syntax of the header lines in detail.
In general, they consist of a keyword and then a value.
.PP
Note that the addressee is indicated as
.BR levy@red.rutgers.edu .
Initially, addresses were simply ``person at machine.''
.II "mail server"
However, current standards make things more flexible, and
make provision for systems to handle other systems' mail.
This can allow automatic forwarding on behalf of
computers not connected to the Internet.
It can be used to direct mail for a number of systems to one central
mail server.
Indeed, there is no requirement that an actual computer by the name of
.B red.rutgers.edu
even exist:
the name servers could be set up so that you mail to department
names, and each department's mail is routed automatically to an 
appropriate computer.
.PP
It is also possible that the part before the `@' is
something other than a user name.
It is possible for programs to be set up to process mail.
There are also provisions to handle mailing lists, and generic names such as
.B postmaster
or
.BR operator .
.PP
.II "RFC 821"
.II "RFC 974"
The way the message is to be sent to another system is described
by RFCs 821 and 974.
The program that is going to be doing the sending asks the
name server several queries to determine where to route the message.
The first query is to find out which machines handle mail for the name 
.BR red.rutgers.edu .
In this case, the server replies that
.B red.rutgers.edu 
handles its own mail.
The program then asks for the address of 
.BR red.rutgers.edu ,
which is 128.6.4.2.
Then the mail program opens a TCP 
connection to port 25 on 128.6.4.2.
(Port 25 is the well-known port used for receiving mail.)
Once this connection is established, the mail program starts sending commands.
.PP
Here is a typical conversation.
Each line is labeled as to whether it is from
.B topaz
or
.BR red .
Note that
.B topaz
initiated the connection:
.DM
	RED 220 RED.RUTGERS.EDU SMTP Service at 27 Jun 94 05:17:18
	EDT
	TOPAZ HELO topaz.rutgers.edu
	RED 250 RED.RUTGERS.EDU - Hello, TOPAZ.RUTGERS.EDU
	TOPAZ MAIL From:<hedrick@topaz.rutgers.edu>
	RED 250 MAIL accepted
	TOPAZ RCPT To:<levy@red.rutgers.edu>
	RED 250 Recipient accepted
	TOPAZ DATA
	RED 354 Start mail input; end with <CRLF>.<CRLF>
	TOPAZ Date: Sat, 27 Jun 87 13:26:31 EDT
	TOPAZ From: hedrick@topaz.rutgers.edu
	TOPAZ To: levy@red.rutgers.edu
	TOPAZ Subject: meeting
	TOPAZ
	TOPAZ Let's get together Monday at 1pm.
	TOPAZ .
	RED 250 OK
	TOPAZ QUIT
	RED 221 RED.RUTGERS.EDU Service closing transmission channel
.DE
.PP
First, note that commands all use normal text.
This is typical of the Internet standards.
Many of the protocols use standard ASCII commands.
This makes it easy to watch what is going on and to diagnose problems.
For example, the mail program keeps a log of each conversation.
If something goes wrong, the log file can simply be mailed to the postmaster.
Since it is normal text, he can see what was going on.
It also allows a human to interact directly with the mail server, for testing.
(Some newer protocols are complex enough that this is not practical.
The commands would have to have a syntax that would require a
significant parser.
Thus, there is a tendency for newer protocols to use binary formats.
Usually they are structured like C or Pascal record structures.)
.PP
Second, note that the each response begins with a number.
This also is typical of Internet protocols.
The protocol defines the allowable responses; and the number that
prefixes a response lets the user program respond unambiguously.
The rest of the response is text, which is normally for use by any
human who may be watching the screen or reading a log;
it has no effect on the operation of the programs.
(However there is one point at which the protocol uses part of
the text of the response.)
The commands themselves simply allow the mail program on one end
to tell the mail server the information it needs to know in order to
deliver the message.
In this case, the mail server could get
the information by looking at the message itself.
But for more complex cases, that would not be safe.
.PP
Every session must begin with
.BR HELO ,
which gives the name of the system that initiated the connection.
Then the sender and recipients are specified.
(There can be more than one RCPT command, if there are several recipients.)
Finally, the data are sent.
Note that the text of the message is terminated by a line that
contains just a period.
(If such a line appears within the message, the period is doubled.)
.PP
After the message is accepted, the sender can send
another message, or terminate the session as in the example above.
.PP
There is a pattern to the response numbers, based their the initial digit:
.IP \fB2\fR 0.3i
Responses that begin with this digit indicate success.
.IP \fB3\fR
A further action is needed, as shown above.
.IP \fB4\fR
A temporary error, such as a disk filling.
The message should be saved, and tried again later.
.IP \fB5\fR
A permanent error, such as a non-existent recipient.
The message should be returned to the sender with an error message.
.PP
.II "RFC 814"
.II "RFC 821"
.II "RFC 822"
.II "RFC 959"
.II "RFC 854"
.II "RFC 855"
For more details about the protocols mentioned in this section, see RFCs
821/822 for mail, RFC 959 for file transfer, and RFCs 854/855 for remote  logins.
For the well-known port numbers, see the current edition of 
.IR "Assigned Numbers" ,
and possibly RFC 814.
.SH "Protocols other than TCP: UDP and ICMP"
.PP
So far, we have described only connections that use TCP.
However, many applications send messages that always fit in a single datagram,
for which TCP is not the best protocol.
.PP
For example, consider name lookup.
When a user attempts to connect to another system,
he usually specifies the system by name rather than Internet address.
His system must translate that name into an IP address before it can do
anything.
Most systems do not have a copy of the data base that translates system
names into IP addresses;
so the user's system must send a query to a system that does have it.
This query is very brief;
it fits in one datagram, as does the answer.
.PP
We don't need all the complexity of TCP to send a message that consists
of a single datagram.
For applications like this, there are alternatives to TCP.
.Sh "UDP"
.PP
.II UDP^definition
.II "user datagram protocol"
.II "protocol^user datagram"
The most common alternative is the user datagram protocol (UDP).
UDP is designed for applications where you don't need to put sequences of
datagrams together.
.PP
UDP fits into a network much like TCP does.
The network software writes a UDP header at the beginning of your data,
analogous to the TCP header.
Then UDP passes the data to IP, which adds the IP header;
the only difference on the IP level is that it writes UDP's protocol
number in the protocol field instead of TCP's.
.PP
UDP doesn't do as much as TCP does.
It provides port numbers, so that several programs can use it at once.
UDP ports are used just like TCP ports, and
there are well-known ports for servers that use UDP.
Thus, the UDP header is shorter than a TCP header:
it has source and destination port numbers, and a checksum,
but little else.
.PP
.II "IEN 116"
.II "RFC 882"
.II "RFC 883"
UDP is used by the protocols
that handle name lookups (see IEN 116, RFC 882, and RFC 883), and a number
of similar protocols.
.Sh "ICMP"
.PP
.II ICMP^definition
.II "Internet control message protocol"
.II "protocol^Internet control message"
Another alternative protocol is the Internet Control-Message Protocol (ICMP).
ICMP is used for error messages, and other messages intended
for the TCP/IP software itself, rather than a user program.
.PP
For example, if you attempt to connect to a host, your system may get back
an ICMP message saying ``host unreachable.''
ICMP can also be used to find out some information about the network.
.II "RFC 792"
See RFC 792 for details of ICMP.
.PP
ICMP is similar to UDP, in that it handles messages that fit in one datagram.
However it is even simpler than UDP.
It doesn't even have port numbers in its header.
Because not all ICMP messages are interpreted by the network software itself,
no port numbers are needed to say where a ICMP message is supposed to go.
.\"
.SH "TCP/IP Architecture:  A Summary"
.PP
The following diagram summarizes what we have presented on TCP/IP
architecture to this point:
.PH 1 1 \\*(XD/tcpip.eps
.PP
This diagram shows a human being on the remote host, who is using
two clients:
.B telnet
and
.BR mail .
.PP
The twisty dotted line traces the movement of mail from the human being on
the local host to the human being remote host, as follows:
.IP \(bu 0.3i
The human being on the local host invokes the mailer
.B elm
to send mail.
.IP \(bu
.BR elm ,
in turn, talks to
.BR smail ,
which manages mail.
.IP \(bu
.B smail
passes mail to TCP.
TCP breaks up the mail message into datagrams;
assigned the new SMTP session a port selected at random;
establishes a connection port 25 with the remote site
(the well-known port for mail);
and passes the datagrams to IP for transmission to the remote site.
.IP \(bu
IP looks up the address of the remote site, resolves that into an Ethernet
address, and launches the datagram down the Ethernet.
.IP \(bu
We do not know, or particularly care, which how many machines
handled the datagrams in between the local host and the remote host.
It may be few or it may be many.
.IP \(bu
The datagrams arrive at IP on the remote host, which confirms that the
datagrams belong at this site,
computes the checksums to confirm the integrity of the datagrams,
and passes them to TCP.
.IP \(bu
TCP computes its own checksums,
reassembles the datagrams into their original file.
Because this connection is bound to port 25, it then passes the
file to the mail server, which is the mail handler on this site.
.IP \(bu
The mail resides where the mail server
has placed it until the human being on the remote host invokes her mailer,
and disposes of the message.
.PP
The twisty dashed line shows how the human being on the local system
uses the
.B telnet
client to echo packets from the remote system:
.IP \(bu 0.3i
First, the user invokes the client
.B telnet, and tells it to access port 7 on the remote system.
Port 7 is the well-known port for the
.B echo
server.
.IP \(bu
The human being types something, then presses
.BR <return> .
What the human has typed is passed to the
.B telnet
client.
.IP \(bu
.B telnet
accesses TCP, which builds a message which contains what the human typed,
binds it to port 7,
and passes it to IP.
.IP \(bu
IP finds the address of the remote host (assuming that the human being
did not name it on
.BR telnet 's
command line), and launches the datagram over the Ethernet.
.IP \(bu
Once again, the datagram winds its way through the maze of machines that
comprise the Internet, and arrives at the IP server on the remote machine.
.IP \(bu
IP confirms that this datagram has arrived at the correct site,
computes the checksum to confirm the integrity of the datagram,
and passes
it to UDP.
.IP \(bu
UDP computes its own checksum.
Because the message is bound to port 7, it passes it to the
.B echo
server.
.IP \(bu
The
.B echo
server on the remote host echoes what it receives from remote host
and sends it back up the line.
.IP \(bu
The return packet makes its way back up the line, until it finally arrives
at the
.B telnet
client on the local host.
.BR telnet ,
in turn, redisplays on the human being's screen.
.IP \(bu
This cycle continues until the human being on the local host exits
from the
.B telnet
client.
.PP
Each of the step in the above is vastly more complicated than this
description indicates.
However, this description should give you an idea of how clients,
UDP, TCP, IP, and servers dovetail.
.\" Hedrick
.SH "Tracking Names and Information: The Domain System"
.PP
As we indicated earlier, the network software generally needs a 32-bit
Internet address to open a connection or send a datagram.
However, humans prefer to deal with computers by name rather than number.
Thus, there is a data base that allows the software to look up a
computer's name and find its corresponding number.
.PP
When the Internet was small, this was easy.
Each system had a file that listed every other system,
giving both its name and number.
Nowadays, there are too many computers for this approach to be practical.
.II "name server"
Thus, these files have been replaced by a set of
.I "name servers"
that keep track of host names and the corresponding Internet addresses.
(In fact these servers are somewhat more general than that.
This is just one kind of information stored in the domain system.) 
.PP
Note that a set of interlocking servers are used, rather than a single
central one.
There are now so many different institutions connected to the Internet
that it would be impractical for them to notify a central
authority whenever they installed or moved a computer.
Thus, naming authority is delegated to individual institutions.
The name servers form a tree, corresponding to institutional structure.
The names themselves follow a similar structure.
.PP
A typical example is the name
.BR borax.lcs.mit.edu.
This is a computer at the Laboratory for Computer Science (LCS) at MIT.
To find its Internet address, you might potentially
have to consult four different servers:
.IP \(bu 0.3i
First, you would ask a central server (called the
.IR "root server" )
where the EDU server is.
EDU is a server that keeps track of educational institutions.
The root server would give you the names and Internet addresses
of several servers for EDU.
(There are several servers at each level, to allow for the possibly that one
might be down.)
.IP \(bu
You would then ask EDU where the server for MIT is.
Again, it would give you names and Internet addresses of several servers.
Generally, not all of those servers would be at MIT,
to allow for the possibility of a general power failure at MIT.
.IP \(bu
Then you would ask MIT where the server for LCS is.
.IP \(bu
Finally, you would ask one of the LCS servers about
.BR borax .
.PP
.II domain^definition
The final result would be the Internet address for
.BR borax.lcs.mit.edu .
Each of these levels is called a
.IR domain .
The entire name,
.BR borax.lcs.mit.edu ,
is called a
.IR "domain name" .
(So are names of higher-level domains, such as
.BR lcs.mit.edu ,
.BR mit.edu ,
and
.BR edu .)  
.PP
Fortunately, you don't really have to go through all of this most of the time.
First of all, the root name-servers also happen to be the name
servers for the top-level domains, such as
.BR edu .
Thus, a single query to a root server will get you to MIT.
Second, software generally remembers answers that it received previously.
So, once we look up a name at
.BR lcs.mit.edu ,
our software remembers where to find servers for
.BR lcs.mit.edu ,
.BR mit.edu ,
and
.BR edu .
It also remembers the translation of
.BR borax.lcs.mit.edu .
.PP
Each of these pieces of information has a ``time to live'' associated with it.
Typically, this is a few days.
After that, system no longer regards the information as reliable,
and looks it up again.
This allows institutions to change things.
.PP
The domain system is not limited to finding out Internet addresses.
Each domain name is a node in a data base.
The node can have records that define a number of different properties.
Examples are Internet address, computer type, and a list of services
provided by a computer.
A program can ask for a specific piece of information,
or all information about a given name.
It is possible for a node in the data base to be marked as an
.I alias
(or nickname) for another node.
It is also possible to use the domain system to store information about users,
mailing lists, or other objects.
.PP
There is an Internet standard defining the operation of these data bases,
as well as the protocols used to make queries of them.
Every network utility has to be able to make such queries,
because this is now the official way to evaluate host names.
Generally utilities will talk to a server on their own system.
This server will take care of contacting the other servers for them.
This keeps down the amount of code that has to be in each application program.
.PP
The domain system is particularly important for handling computer mail.
There are entry types to define what computer handles mail for a given
name, to specify where an individual is to receive mail, and to define
mailing lists.
.PP
.II "RFC 882"
.II "RFC 883"
.II "RFC 973"
.II "RFC 974"
See RFCs 882, 883, and 973 for specifications of the domain system.
RFC 974 defines the use of the domain system in sending mail.
.SH Routing
.PP
Earlier, we said that stated that IP is responsible
for sending datagrams to their destination;
but little was said about how it would do this.
.II routing^definition
The task of finding how to get a datagram to its destination is referred to as
.IR "routing" .
.PP
Many of the details depend upon the implementation.
However some general things can be said.
.Sh "Gateways"
.PP
First, it is necessary to understand the model on which IP is based.
IP assumes that a system is attached to some local network.
We assume that the system can send datagrams to any other system on its
own network.
(In the case of Ethernet, it simply finds the Ethernet address of the
destination system, and puts the datagram out on the Ethernet.)
The problem comes when a system is asked to send a datagram to a system on a
different network.
This problem is handled by gateways.
.PP
.II gateway^definition
A
.I gateway
is a system that connects a network with one or more other networks.
Gateways are often normal computers that happen to have more than one network
interface.
For example, we have a machine that has two different Ethernet
interfaces \(em it is connected to networks 128.6.4 and 128.6.3.
Thus, this machine can act as a gateway between those two networks.
.PP
The software on a gateway must be set up so that it will forward
datagrams from one network to the other.
That is, if a machine on network 128.6.4 sends a datagram to the gateway,
and the datagram is addressed to a machine on network 128.6.3,
the gateway will forward the datagram to the destination.
Major communications centers often have gateways that
connect a number of different networks.
(In many cases, special-purpose gateway systems provide better performance
or reliability than general-purpose systems acting as gateways.
A number of vendors sell such systems.)
.Sh "Routing Through Gateways"
.PP
Routing in IP is based entirely upon the network number of the destination
address.
Each computer has a table of network numbers.
For each network number, a gateway is listed;
this is the gateway to be used to get to that network.
Note that the gateway doesn't have to connect directly to the network \(em
it just has to be the best place to go to get there.
.PP
For example at Rutgers, our interface to NSFnet is at the John von Neuman
Supercomputer Center (JvNC).
Our connection to JvNC is via a high-speed
serial line connected to a gateway whose address is 128.6.3.12.
Systems on net 128.6.3 will list 128.6.3.12 as the gateway
for many off-campus networks.
However, systems on net 128.6.4 will list 128.6.4.1 as the
gateway to those same off-campus networks.
Site 128.6.4.1 is the gateway between networks 128.6.4 and 128.6.3,
so it is the first step in getting to JvNC.
.PP
When a computer wants to send a datagram, it first checks to see if the
destination address is on the system's own local network.
If so, the datagram can be sent directly.
Otherwise, the system expects to find an entry for the network that
the destination address is on.
The datagram is sent to the gateway listed in that entry.
This table can get quite large.
.PP
For example, the Internet now includes thousands of individual networks.
Thus various strategies have been developed to reduce the size
of the routing table.
.II "default routes"
.II "routing^default routes"
One strategy is to depend upon
.IR "default routes" .
.PP
Often, there is only one gateway out of a network.
This gateway might connect a local Ethernet to a campus-wide backbone network.
In that case, we don't need to have a separate entry for every network
in the world.
.II "routing^default gateway"
.II "default gateway"
We simply define that gateway as a
.IR "default" :
when no specific route is found for a datagram, the datagram is sent
to the default gateway.
A default gateway can be used even when there are several gateways on a network.
Note that when your machine only has a SLIP connection to an Internet provider,
that provider is your default gateway by definition, and
all of your datagrams are routed to it automatically.
.Sh "Optimal Gatways"
.PP
When you attempt to route through a gateway, that gateway can reply
``I'm not the best gateway \(em use this one instead.''
.II ICMP
(The message is sent via ICMP.
.II "RFC 792
See RFC 792.)
Most network software is designed to use these
messages to add entries to their routing tables.
For example,
suppose network 128.6.4 has two gateways, 128.6.4.59 and 128.6.4.1.
Gateway 128.6.4.59 leads to several other internal Rutgers networks; and
128.6.4.1 leads indirectly to the NSFnet.
Suppose we set 128.6.4.59 as a default gateway, and have no other
routing-table entries.
Now, what happens when we need to send a datagram to MIT?
MIT is network 18.
Because we have no entry for network 18, the datagram
is sent to the default, 128.6.4.59.
As it happens, this gateway is the wrong one,
so it forwards the datagram to 128.6.4.1;
but it also returns error message to our system that says, in effect,
``To get to network 18, use 128.6.4.1.''
Our software then adds an entry to the routing table, and
then sends all future MIT datagrams directly to 128.6.4.1.
.II "ICMP"
(The error message is sent using the ICMP protocol.
The message type is called 
.IR "ICMP redirect" .)
.PP
Most IP experts recommend that an individual computer should not try to keep
track of the entire network.
Instead, it should start with default gateways,
and let the gateways tell them the routes, as just described.
.PP
However, this doesn't say how the gateways should find out about the routes.
The gateways can't depend upon this strategy.
They have to have fairly complete routing tables.
For this, some sort of routing protocol is needed.
.II "routing^protocol"
.II "protocol^routing"
A
.I "routing protocol"
is simply a technique for the gateways to find each other,
and keep up to date about the best way to get to every network.
.II "RFC 1009"
RFC 1009 reviews gateway design and routing.
.\" .II rip.doc
.\" However,
.\" .B rip.doc
.\" is probably a better introduction to the subject.
.\" It contains some tutorial material, and a detailed description of the most
.\" commonly used routing protocol.
.SH "Classes of Internet Addresses"
.PP
As indicated earlier, an Internet address is a 32-bit number, which normally
is written in decimal as four bytes, e.g., ``128.6.4.7''.
Actually, there are three different types of address.
The problem is that an address has to indicate both the network,
and the host within the network.
.PP
When Internet addressing was designed,
the designers believed that eventually there would be many networks.
Many of them would be small, but probably 24 bits would be needed
to represent all the IP networks.
It was also felt that some very big networks might need 24 bits
to represent all of their hosts.
This would seem to lead to 48-bit addresses.
However, the designers really wanted to use 32-bit addresses,
so they adopted a kludge.
Because the designers assumed that most networks would be small,
they set up three different ranges of address.
.PP
.II "network^class A"
A
.I class-A
address uses only the first byte for the network number;
these begins with numbers 1 to 126.
The other three bytes (24 bits) of the address
are available to identify a host within the network.
Class-A addresses are used for 126 large networks.
The ARPAnet is one, and there are a few large commercial networks;
but few normal organizations get a class-A address.
.PP
.II "network^class B"
Most large organizations use a
.I "class-B"
address.
Class-B addresses use the first two bytes (16 bits) for the network number.
Thus, class-B network comprise network numbers 128.1 through 191.254.
(We avoid 0 and 255, for reasons that we see below.
We also avoid addresses beginning with 127,
because that is used by some systems for special purposes.)
The last two bytes (16 bits) of the address are available for host addresses.
Thus, a class-B network to hold up to 64,516 hosts,
which should be enough for most organizations.
.PP
.II "network^class C"
Finally,
.IR class-C
addresses use three bytes, in the range 192.1.1 to 223.254.254.
Only the last byte (eight bits) are available to identify hosts within a
network; thus, a class-C network can contain no more than 254 hosts,
but there can be lots of these networks.
.PP
Addresses above 223 are reserved for future use,
as classes D and E (which are not yet defined).
.PP
Many large organizations find it convenient to divide their network number into
.IR subnets .
For example, Rutgers has been assigned a class B address, 128.6.
We find it convenient to use the third byte of the address to
indicate which Ethernet a host is on.
This division has no significance outside of Rutgers.
A computer at another institution would treat all
datagrams addressed to 128.6 the same way,
and would not look at the third byte of the address.
Thus, computers outside Rutgers would not have
different routes for 128.6.4 or 128.6.5;
but inside Rutgers, we treat 128.6.4 and 128.6.5 as separate networks.
In effect, gateways inside
Rutgers have separate entries for each Rutgers subnet,
whereas gateways outside Rutgers just have one entry for 128.6.
.PP
Note that we could do exactly the same thing by using a separate
class-C address for each Ethernet.
As far as Rutgers is concerned, it would be just as convenient
for us to have a number of class-C addresses.
However, using class-C addresses would make things inconvenient
for the rest of the world:
every institution that wanted to talk to us would have to have
a separate entry for each one of our networks.
If every institution did this, there would be far too many networks
for any reasonable gateway to keep track of.
By subdividing a class B network, we hide our internal structure from
everyone else, and save them trouble.
.II "RFC 950"
This subnet strategy requires special provisions in the network software.
It is described in RFC 950.
.Sh "Special Addresses"
.PP
Addresses 0 and 255 have special meanings.
.PP
.II address^0
Address 0 is reserved.
It is used in two special situations:
.IP \(bu 0.3i
Zero can be substituted for the host portion
of the IP address to yield a network address.
.IP \(bu
Address 0 can be used by machines that do not know their own addresses.
In certain circumstances, it is possible for a machine not to know
the number of the network it is on, or even its own host address.
For example, 0.0.0.23 would be a machine that knew it was host
number 23, but didn't know on what network.
.PP
.II address^255
.II broadcasting
Address 255 is used for broadcasting.
A
.I broadcast
is a message that you want every system on the network to see.
Broadcasts are used when a machine does not know whom to talk to.
For example, suppose you need to look up a host name and get its
Internet address.
If you don't know the address of the nearest name server,
you might send the request as a broadcast.
There are also cases where a number of systems are interested in information:
it is less expensive to send a single broadcast than to send a
datagram to each host that is interested in the information.
To send a broadcast, use a special address that is constructed from your network
address with ``255'' in the part of the address where the host number goes.
For example, if you are on network 128.6.4, you would use send a
broadcast to address 128.6.4.255.
.PP
How this is actually implemented depends upon the medium.
It is not possible to send broadcasts on the ARPAnet, or on a SLIP connection.
However, a broadcast is possible on an Ethernet:
if you use an Ethernet address with 255,
every machine on the Ethernet must look at that packet.
.PP
Although the official broadcast address for network 128.6.4 is now
128.6.4.255, there are some other addresses that may be treated as
broadcasts by certain implementations.
For convenience, the Standard also allows 255.255.255.255 to be used.
This refers to all hosts on the local network.
It is often simpler to use 255.255.255.255 instead of finding 
out the network number for the local network and forming a broadcast
address such as 128.6.4.255.
.PP
Because 0 and 255 are used for unknown and broadcast addresses,
a normal host should never be given an address that contains 0 or 255.
An address should never begin with 0, 127, or any number above 223.
.II Martians
.II "Central University of Mars"
Addresses violating these rules are sometimes referred to as ``Martians,''
because of rumors that the Central University of Mars uses network 255.
.SH "Datagram Fragmentation and Reassembly"
.PP
TCP/IP is designed for use with many different kinds of network.
.PP
Unfortunately, network designers do not agree about how big a packet can be.
Ethernet packets can be up to 1,500 bytes long.
ARPAnet packets have a maximum of about 1,000 bytes.
Some very fast networks have much larger packet sizes.
.PP
At first, you might think that IP should simply settle
on the smallest possible size.
Unfortunately, this causes serious performance problems:
when transferring large files, big packets are far more efficient
than small ones; therefore,
we want to use the largest packet possible.
But we also want to be able to handle networks with small limits.
TCP and IP have two ways to deal with this problem.
.PP
.II datagram^negotiation
First, TCP ``negotiates'' the size of the datagram.
When a TCP connection first opens,
each end ``bids'' maximum datagram size that it can handle.
The smaller of these ``bids'' is used for the rest of the connection.
This lets the sites use the largest datagram that both can handle.
.PP
However, this doesn't completely solve the problem, because
the two ends do not know about all of the machines that lie between them.
For example, when sending data between Rutgers and Berkeley,
it is likely that both computers will be on Ethernets.
Thus, each can handle 1,500-byte datagrams.
However, the connection will at some point end up going over the ARPAnet,
which cannot handle packets that large.
.PP
For this reason, IP can split a datagram into pieces.
.II fragmentation
This is called
.IR fragmentation .
The IP header contains fields that indicate that a datagram has been split,
and the information needed to reassemble the datagram from the fragments.
.PP
Thus, if a gateway connects an Ethernet to the ARPAnet,
it must be prepared to take 1,500-byte Ethernet packets and
split them into pieces that will fit onto the ARPAnet.
Further, every host implementation of TCP/IP must be prepared to
accept fragments and reassemble them.
.II reassembly
This is called
.IR reassembly .
.PP
Implementations of TCP/IP differ in how they select the size of a datagram.
Many implementations use 576-byte datagrams when they cannot verify
that the entire path can handle larger packets.
Because many implementations have bugs in the code that reassemble fragments
implementors often try to avoid having packets fragmented during transmission.
Different implementors on how they decide it is safe to use larger datagrams:
some use them only for the local network,
whereas others use them for any network on the same campus.
However, a size of 576 bytes is a ``safe'' size
that every implementation must support.
.SH "Ethernet Encapsulation: ARP"
.PP
Earlier,
there was a brief discussion about what IP datagrams look like on an Ethernet.
This discussion introduced the Ethernet header and checksum.
However it left one hole:
it didn't say how to figure out what Ethernet address
to use when you want to talk to a given Internet address.
.II "protocol^address resolution"
.II "address resolution protocol"
.II ARP^definition
In fact, there is a separate protocol for this, called
.I "address resolution protocol"
(ARP).
.PP
Please note that ARP is not an IP protocol:
that is, the ARP datagrams do not have IP headers.
.PP
Suppose you are on system 128.6.4.194 and you want to connect
to system 128.6.4.7.
Your system first verifies that 128.6.4.7 is on the same network,
so it can talk directly via Ethernet.
Then it looks up 128.6.4.7 in its ARP table,
to see if it already knows its Ethernet address.
If it does, it sticks on an Ethernet header and send the packet.
.PP
Suppose, however, that the target system is not the sender's ARP table.
The sender cannot send the packet, because it needs the Ethernet address.
In this situation, the sender uses the ARP protocol to send an ARP request.
Essentially, an ARP request says, ``I need the Ethernet address for 128.6.4.7.''
Every system listens to ARP requests.
When a system sees an ARP request for itself, it must respond.
.II "Ethernet^address notation"
.II "address^Ethernet notation"
Host 128.6.4.7 sees the request, and responds with an ARP reply that says,
in effect, ``IP host 128.6.4.7 has Ethernet address 8:0:20:1:56:34.''
(Recall that Ethernet addresses are 48 bits, or six bytes.)
Ethernet addresses are conventionally shown in
hexadecimal notation, with bytes separated by colons.)
.PP
The sender writes this information into its ARP table,
to spare it the trouble of sending another ARP for the same recipient;
then proceeds with its transmission.
Most systems treat the ARP table as a cache,
and erase entries from it if they have not been used
within a certain period of time.
.PP
Note, by the way, that an ARP request must be sent as broadcast;
after all, the whole reason for sending an ARP request is that you don't
know the Ethernet address.
To broadcast over an Ethernet network, a host sends a packet to Ethernet
address ``FF:FF:FF:FF:FF:FF'' (i.e., an Ethernet address of all ones).
By convention, every machine on the Ethernet must
pay attention to a packet with this address; therefore,
every system sees every ARP request.
Each system checks whether the request is for its IP address:
if so, it responds; if not, it ignores it.
(Some hosts use ARP requests to update their knowledge
about other hosts on the network, even if the request isn't for them.)
Note that packets whose IP address indicates broadcast (e.g.,
255.255.255.255 or 128.6.4.255) are also sent with an Ethernet address
that is all ones.
.SH "Getting More Information"
.PP
This concludes our introduction of the basics of networking.
If you want more information, you should look into the source documents
for the Internet.
The Internet comprises literally hundreds of documents;
the following describes the ones that we believe are the most important.
.PP
An Internet standard is called a
.IR "request for comment"
(RFC).
A proposed standard is initially issued as a proposal, and given an RFC number.
When it is finally accepted, it is added to Official Internet Protocols,
but it is still referred to by the RFC number.
.PP
We have also included two IENs.
(IENs used to be a separate classification for more informal documents.
This classification no longer exists \(em RFCs are now used
for all official Internet documents,
and a mailing list is used for more informal reports.)
.PP
The convention is that whenever an RFC is revised, the revised version gets a
new number.
This is fine for most purposes, but it causes problems with two documents:
Assigned Numbers and Official Internet Protocols.
These documents are being revised all the time, so their RFC numbers
keeps changing.
You will have to look in file
.B rfc-index.txt
to find the number of the latest edition.
.PP
.II "RFC 791"
To begin, anyone who is seriously interested in TCP/IP should read RFC 791,
which describes IP.
.II "RFC 1009"
RFC 1009 is also useful \(em it specifies gateways to be used by NSFnet.
As such, it contains an overview of much of the TCP/IP technology.
.PP
You should probably also read the description of at least one of the
application protocols, just to get a feel for the way things work.
.II "RFC 821"
.II "RFC 822"
RFCs 821 and 822, which describe mail, are a good place to begin.
.PP
.II "RFC 793"
RFC 793 defines TCP.
This is, of course, a very basic specification.
However, it is fairly complex, so you should only read this
when you have the time and patience to think about it carefully.
.II "Postel, Jon"
Fortunately, the author of the major RFCs (Jon Postel) writes very well,
so the RFC 793 is far easier to read than you would expect,
given the complexity of what it is describing.
.PP
You can look at the other RFCs as you become curious about
their subject matter.
Here is a list of the documents you are likely to want:
.IP "\fBrfc-index\fR" 1.0i
.II "rfc-index"
This lists all of the RFCs.
.IP "\fBRFC 1012\fR"
.II "RFC^listing"
.II "RFC 1012"
This gives a somewhat fuller list of all RFCs.
.IP "\fBRFC 1011\fR"
.II "RFC 1011"
.II "protocol^official list"
Official protocols.
It's useful to scan this to see what tasks protocols have been built for.
This defines which RFCs are actual standards,
as opposed to requests for comments.
.IP "\fBRFC 1010\fR"
.II "RFC 1010"
.II "assigned numbers"
Assigned numbers.
If you are working with TCP/IP,
you will want a copy of this as a reference.
It's not very exciting to read.
It lists all the officially defined well-known ports and lots of other things.
.IP "\fBRFC 1009\fR"
.II "RFC 1009"
.II NSFnet^protocol
NSFnet gateway specifications.
This gives a good overview of IP routing and gateway technology.
.IP "\fBRFC 1001, RFC 1002\fR"
.II "RFC 1001"
.II "RFC 1002"
.II netBIOS^protocol
netBIOS:
networking for PCs.
.IP "\fBRFC 973\fR"
.II "RFC 973"
.II domains^protocol
Update on domains.
.IP "\fBRFC 959\fR"
.II "RFC 959"
.II FTP^protocol
The file-transfer protocol (FTP).
.IP "\fBRFC 950\fR"
.II "RFC 950"
.II subnets^definition
Definition of subnets.
.IP "\fBRFC 937\fR"
.II "RFC 937"
.II POP2^protocol
Protocol for POP2, for reading mail on PCs.
.IP "\fBRFC 894\fR"
.II "RFC 894"
.II "RFC 825"
.II "IP^protocol for implementation"
How IP is to be put on Ethernet.
See also RFC 825.
.IP "\fBRFC 882, RFC 883\fR"
.II "RFC 882"
.II "RFC 883"
Definition of domains, i.e., the data base used to go from host names to
Internet address and back.
.II UUCP
This is also used to route \*(UU messages.
.II "RFC 973"
See also RFC 973.
.IP "\fBRFC 854, RFC 855\fR"
.II "RFC 854"
.II "RFC 855"
.II TELNET^protocol
Definition of the TELNET protocol, for remote logins.
.IP "\fBRFC 826\fR"
.II "RFC 826"
.II ARP^protocol
Protocol for ARP, for finding out Ethernet addresses.
.IP "\fBRFC 821\fR"
.IS "\fBRFC 822\fR"
.II "RFC 821"
.II "RFC 822"
.II mail^protocol
Definition for mail.
.IP "\fBRFC 814\fR"
.II "RFC 814"
.II "well-known ports^protocol"
Protocol for names and ports.
.II "well-known ports"
It gives the concepts that underlie
``well-known ports''.
.IP "\fBRFC 793\fR"
.II "RFC  793"
.II "TCP^protocol"
Protocol that defines TCP.
.IP "\fB792\fR
.II "RFC 792"
.II ICMP^protocol
Protocol that defines ICMP.
.IP "\fBRFC 791\fR"
.II "RFC 791"
.II IP^protocol
Protocol that defines IP.
.IP "\fBRFC 768\fR"
.II "RFC 768"
.II UDP^protocol
Protocol that defines UDP.
.IP \fBrip.doc\fR
.II rip.doc
Details of the most commonly used routing protocol.
.IP \fBIEN-116\fR
.II IEN-116
Protocol that defines the old name server
(still needed by several kinds of system).
.IP \fBIEN-48\fR
.II IEN-48
.II "Catnet modem"
The Catenet model, which describes the philosophy behind TCP/IP.
.PP
The following documents are somewhat more specialized:
.IP "\fBRFC 813\fR" 1.0i
.II "RFC 813"
Window and acknowledgment strategies in TCP.
.IP "\fBRFC 815\fR"
.II "RFC 815"
.II "datagram^reassembly"
Techniques for reassembling datagrams.
.IP "\fBRFC 816\fR"
.II "RFC 816"
Techniques to isolate and resolve faults.
.IP "\fBRFC 817\fR"
.II "RFC 817"
Modularity and efficiency in implementation
.IP "\fBRFC 879\fR"
.II "RFC 879"
The maximum-segment-size option in TCP.
.IP "\fBRFC 896\fR"
.II "RFC 896"
Congestion control
.IP "\fBRFC 827, RFC 888, RFC 904, RFC 975, RFC 985\fR"
.II "RFC 827"
.II "RFC 888"
.II "RFC 904"
.II "RFC 975"
.II "RFC 985"
.II EGP
EGP and related issues.
.Sh "How To Obtain Documents"
.PP
The most important RFCs have been collected into a three-volume
set, the DDN Protocol Handbook.
.II "DDN Protocol Handbook"
.II "DDN Network Information Center"
It is available from the DDN Network Information Center,
SRI International, 333 Ravenswood Avenue, Menlo Park, CA 94025
(telephone 800-235-3155).
You should be able to get them via anonymous FTP from
.BR sri-nic.arpa .
.PP
.B rip.doc
is available by anonymous FTP from
.BR topaz.rutgers.edu ,
as
.BR /pub/tcp-ip-docs/rip.doc .
.PP
The InfoMagic CD-ROM of standards
contains (among many other things) the ARPAnet RFCs.
This CD-ROM is available from many commercial sources; or you can telephone
InfoMagic at 1-800-800-6613.
.SH "Where To Go From Here"
.PP
The next chapter describes how to install and configure
.BR cohulip .
Please read it thoroughly before you attempt to install anything.
.PP
The subsequent chapters describe how run the application
.BR net ,
whic is the program that actually performs networking tasks;
and how to administer
.BR cohulip .
