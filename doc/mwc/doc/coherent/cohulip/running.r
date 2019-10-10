.ds TL "Running Networking"
.ds TI COHULIP
.NH "Running net"
.PP
The command
.B net
is the part of
.B cohulip
that actually performs the networking tasks.
.B net
executes all networking tasks that a true networking package would
implement as separate executable commands.
.PP
You should read this chapter carefully to see what options are available for
.BR net ,
and how to invoke its features.
You can also use this chapter as a reference while you are using
.BR net .
.PP
Please note that commands for
.B net
are case sensitive:
for example, command
.B ftp
works, but commands
.B Ftp
or
.B FTP
do not.
.SH "Starting Up"
.PP
The command
.B net
has the following syntax:
.DS
	\fBnet [ \fIstartup.file \fB]\fR
.DE
.I startup.file
holds a set of commands that initialize
.BR net .
As you will see, these commands are simply ordinary instruction to
.BR net ,
just as you could type interactively into
.BR net .
The rest of this chapter will show you what these commands mean, and how
you can modify them to suit the networking environment in which you are
working.
.PP
If you are going to use
.B cohulip
to perform a variety of networking tasks,
you may wish to have more than one startup file.
For example, you may need one startup file to SLIP into the Internet
via an Internet provider, and another to access a local network via Ethernet.
For details on how to modify your a startup file, see the chapter on installing
.BR cohulip .
.PP
If you do not name a
.I startup.file
on
.BR net 's
command line, it reads the file
.BR $NETHOME/startup.net .
If you do not name a
.I startup.file
on
.BR net 's
command line and
.B net
cannot find
.BR $NETHOME/startup.net ,
then you must give
.B net
its initialization commands by hand.
.SH "Modes of Operation"
.PP
.B net
works in either of two modes:
command mode and converse mode.
.PP
In
.I command
mode,
.B net
displays the prompt
.BR net> ,
and you can type any of the commands described below.
In
.I converse
mode, keyboard input is processed according to the
.IR "current session" ,
which may be either a TELNET or FTP session:
.IP \(bu 0.3i
In a TELNET session, keyboard input is sent to the remote system and
any output from the remote system is displayed on the console.
.IP \(bu
In an FTP session,
.B net
displays the prompt
.BR ftp> .
.B net
examines keyboard input to see if it is an FTP command.
if it is, then
.B net
executes it as such; if not, it passes it directly to the remote remote
host's FTP server.
For a list of FTP commands, see the description of the command
.BR ftp ,
below.
.PP
While you are conversing with a remote host via FTP or TELNET,
you can type a ``hot key'' to escape back into
.BR net 's
command mode.
After you have pressed the hot key,
.B net
displays the prompt
.BR net> .
You can then issue one or more commands to
.BR net .
To resume conversing with the remote host, type
.BR <return> .
You can use this to mechanism to close an interactive session,
or perform other useful tasks; these are described below.
By default,
.B net
sets the hot key to
.B <ctrl-]>
(0x1D).
.PP
The keyboard also has ``cooked'' and ``raw'' states.
.II "keyboard^cooked mode"
In cooked state,
.B net
processes input one line at a time.
Pressing
.B <return>
passes the complete line up to the application.
.II "keyboard^raw mode"
In raw mode,
.B net
passes each character to the application as you type it.
.PP
While the keyboard is in cooked mode,
you can use the following editing commands as you are typing input:
.IP \fB<ctrl-A>\fR 1.0i
Move one word to the left.
.IP \fB<backspace>\fR
Delete the previous character.
.IP \fB<ctrl-D>\fR
Move one character to the right.
.IP \fB<ctrl-E>\fR
Throw away the current line and redisplay the previous line.
.IP \fB<ctrl-F>\fR
Move one word to the right.
.IP \fB<ctrl-H>\fR
Delete the previous character.
.IP \fB<ctrl-R>\fR
Redisplay the current line.
.IP \fB<ctrl-S>\fR
Move one character to the left.
.IP \fB<ctrl-U>\fR
Delete the current line.
.IP \fB<ctrl-V>\fR
Do not interpret the following character.
Use this to pass an editing command through to the remote site.
.IP \fB<ctrl-W>\fR
Delete the previous word.
.IP \fB<ctrl-Y>\fR
Delete the current line.
.PP
.B net
is in the cooked state either when
.B net
is in command mode or when you are conversing with a remote host via FTP.
When you are conversing with a remote host via a TELNET session,
the keyboard's state depends upon whether the remote end has issued
(and the local end has accepted) the TELNET protocol's option
.BR "WILL ECHO" .
For details, see the description of the
.B net
command
.BR echo ,
below.
.SH "Host Names"
.PP
Many of the commands described below use the symbol
.I <hostid>
to represent a remote host or gateway.
You can specify the
.I <hostid>
either as a domain name (e.g., ``raven.alaska.edu''),
or as a numeric IP address written in dotted decimal notation and
enclosed by brackets (e.g., \fB[44.0.0.1]\fR).
.PP
You can use domain names in either of two ways.
The first way is to write the name and IP address of the domain into file
.BR $NETHOME/hosts.net .
(The format of this file is described in the previous chapter.)
The other way is to interrogate a remote domain-name server.
The command
.B domain
lets you set the IP address of a domain-name server that you have permission
to interrogate.
For details, see the command
.BR domain ,
described below.
.SH "Syntax of Commands"
.PP
The following describes each of the commands that
.B net
recognizes.
Square brackets enclose optional arguments.
The character `|' denotes an either-or selection.
Commands are case sensitive, and are all in lower case.
.Sh "<return> \(em Send a Command"
.PP
Return to converse mode with the current TELNET or FTP session.
If there is no current session,
.B net
remains in command mode.
.Sh "? \(em Print a Help Message"
.PP
Help:
print a list of all commands recognized.
.Sh "! \(em Send a Command to the Shell"
.PP
The command
.DS
	\fB! \fIcommand\fR
.DE
sends
.I command
to your \*(CO shell for execution.
If you give the command
.B !
without a
.IR command ,
.B net
invokes the shell.
.Sh "# \(em Ignore a Command"
.PP
Ignore the command that follows.
You can use this to write a comment into a
.I startup
file.
.\" .Sh "arp \(em Manipulate the ARP Table"
.\" .PP
.\" .II "commands^arp"
.\" .II "arp"
.\" .II ARP^definition
.\" .II "Address Resolution Protocol table"
.\" This command lets you manipulate the
.\" Address Resolution Protocol (ARP) table,
.\" which maps IP addresses to their subnet (link) addresses
.\" on subnetworks capable of broadcasting.
.\" This command takes the following form:
.\" .IP \fBarp\fR
.\" When invoked without an argument,
.\" .B arp
.\" displays the contents of the ARP.
.\" For each IP address entry, it displays the subnet type (e.g., Ethernet),
.\" subnet address, and time to expiration.
.\" If the link address is unknown,
.\" .B arp
.\" also shows the number of IP datagrams awaiting resolution.
.\" .IP "\fBarp add \fIhostid \fBether\fR"
.\" Add the entry for
.\" .I hostid
.\" into the ARP table.
.\" This is useful for ``hard-wiring'' digipeater paths,
.\" and for other paths that are not directly resolvable.
.\" Optional digipeater calls are added to the end of this line.
.\" .IP "\fBarp drop \fIhostid\fB ether\fR"
.\" Remove the entry for
.\" .I hostid
.\" from the ARP table.
.\" .IP "\fBarp publish \fIhostid\fB ether\fR"
.\" Respond to an ARP query for some other host.
.\" This is commonly referred to as
.\" .BR "proxy arp" ,
.\" and is considered a fairly dangerous tool.
.\" This way, the rest of the world does not know the second machine
.\" isn't really on the air.
.\" Use this command with caution.
.Sh "attach \(em Attach net To a Network"
.PP
.II "commands^attach"
.II attach
Command
.B attach
configures and attaches a hardware interface to your system.
It has the following syntax:
.DS
	\fBattach \fIhwtype IO_address device mode label bufsize mtu \fB[ \fIspeed \fB] \fB[ \fIscript \fB]\fR
.DE
The following details the arguments to this command:
.IP \fIhwtype\fR
the kind of I/O device that is being attached, as follows:
.RS
.IP \fBasy\fR 0.3i
An asynchronous (serial) port.
.IS \fBnx\fR
An Ethernet card, using the device driver
.B ne
to access an NEC NE2000 Ethernet board, or a clone thereof.
.RE
.IP \fIIO_address\fR
The base address of the control registers for the device.
Under
.BR cohulip ,
this is always zero.
.IP \fIdevice\fR
Name the device by which you are accessing the network.
If
.I hwtype
is
.BR asy ,
then this names the serial port you are using, e.g.,
.BR /dev/com3fr .
If
.I hwtype
is
.BR nx ,
then set this to zero.
.IP \fImode\fR
Set how IP datagrams are encapsulated.
It can be either of the following:
.RS
.IP \fBslip\fR
Enclose IP datagrams within SLIP frames, without a link header.
Use this option if
.I hwtype
is set to
.BR asy .
.IP \fBarpa\fR
Use the ARP to map an IP address to an Ethernet address.
Use this option if
.I hwtype
is set to
.BR nx .
.RE
.IP \fIlabel\fR
The name by which the interface will be known to the various commands,
such as
.BR connect ,
.BR route ,
and
.BR trace .
When you installed
.BR cohulip ,
this was set to
.B sl0
for a serial connection, or to
.B ne0
for an Ethernet connection.
.IP \fIbufsize\fR
This argument is ignored.
.IP \fImtu\fR
The size of the maximum transmission unit, in bytes.
If a datagram exceeds this limit,
.B cohulip
fragments it at the IP level.
.IP \fIspeed\fR
The speed of the serial port through which the network
is being accessed.
If the type of connection is Ethernet, this argument is ignored and is
not required.
.IP \fIscript\fR
A dialing script, through which
.B cohulip
can dial into a remote site.
You should use this option if you are using SLIP to access a remote system
via modem.
.PP
A
.I script
has the following format:
.DS
	\fB[ \- ] \fIsendstring expectstring \fB[ \fIsendstring expectstring ... \fB]\fR
.DE
If the first entry in the script is `\-',
.B cohulip
displays the send/expect strings on the screen as executes the script.
.I sendstring
and
.I expectstring
come in pairs, with the exception of the the last
.IR sendstring :
once the remote site goes into SLIP mode, no more strings are forthcoming, and
.B cohulip
waits silently for recognizable SLIP packets or a hangup.
.PP
.I sendstring
can include the following escape sequences:
.DS
	\fB\e\e\fR	Literal `\e'
	\fB\eE\fR	End of file
	\fB\eN\fR	NUL
	\fB\eb\fR	Backspace
	\fB\en\fR	Newline
	\fB\er\fR	Carriage return
	\fB\es\fR	Space character
	\fB\et\fR	Horizontal tab
.DE
You should end each
.I sendstring
with a `\er', to flush the string to the output device.
.PP
The send-expect function has a 30-second timeout.
Unlike a \*(UU chat script, a
.B cohulip
script does not support alternate-send phrases in the event of timeout.
The following gives an example
.IR script :
.DM
	ATE1V1Q0S0=0\er OK ATDP1234567\er ogin: mylogin\er ord: mypassword\er
.DE
The first pair, 
.DM
	ATE1V1Q0S0=0\er OK
.DE
sends an initialization string to the modem and
awaits the reply ``OK'' from the modem.
.PP
The second pair,
.DM
	ATDP1234567\er ogin:
.DE
dials the modem and awaits the login prompt from the remote system.
(The string ``1234567'' stands for the remote system's telephone number,
whatever it might happen to be.)
.PP
The third pair,
.BR "mylogin\er ord:" ,
sends the your login identifier and awaits the password prompt
from the remote system.
(Note that ``mylogin'' stands for your login identifier, whatever it
might happen to be.)
.PP
Finally,
.B net
sends the password string \(em in this example,
.BR mypassword\er .
.PP
Note that
.BR attach 's
scripting mechanism gives you only limited control over the port or modem.
If you are dialing out through a modem that is plugged into an enabled port,
you must disable the port by hand before you invoke
.BR net .
Also note that
.B net
has no mechanism for sending commands to the modem after a SLIP session has
concluded;
therefore, you must re-initialize the modem by hand after you
have finished working with the remote system.
Hints on how to write a script to do this appear below.
.PP
Finally, note that you can use a script to access a remote system only
if you have a permanently assigned IP number,
assigned either by an Internet authority or by your Internet provider.
Some providers assign IP addresses ``on the fly;'' in this case, you must
use a different method of dialing into the remote system.
For details, see the previous chapter, on configuring
.BR cohulip .
.Sh "cd \(em Change Directory\fR"
.PP
.II "commands^cd"
.II cd
This command changes the directory you are accessing on your local machine.
If you are running an FTP or TELNET session,
it does not change the directory that you are accessing on the remote host.
It works exactly the same as the
.B cd
command as described in the \*(CO Lexicon.
.Sh "close \(em Close a Session On Your End"
.PP
.II "commands^close"
.II "close"
Close an FTP or TELNET session.
This is an alternative to asking the remote
server to initiate a close (i.e., sending the command
.B quit
via FTP, or the command
.B logout
via TELNET.
When either FTP or TELNET sees the incoming half of a TCP connection close,
it automatically responds by closing the outgoing half of the connection.
.PP
.B close
recognizes the syntax
.DS
	\fBclose [ \fItype \fB[ \fIsession \fB] ]\fR
.DE
where
.I type
is the type of session to close (either TELNET or FTP), and
.I session
is the number of the session to close.
If you are running more than one
.I type
of session and do not specify which
.I session
to close,
.B net
closes the one that you opened most recently.
.PP
.B close
is more graceful than the command
.BR reset ,
in that it is less likely
to leave the remote TCP in a ``half-open'' state.
Note, however, that if you use
.BR net 's
.B close
command instead of closing from within a session, it be a few minutes before
the fact that your TCP session has closed percolates through the net to
your remote host and it closes the connection on its end;
so wait a few minutes before you attempt to log again into the remote host.
.Sh "dir \(em List Contents of Directory on Remote Host"
.PP
.II "commands^dir"
.II dir
The command
.B dir
resembles the command of the same name under \*(MD.
It displays on your screen the contents of a directory on the remote host;
the information displayed includes
the name of each file, its size, and the date and time it was last modified.
.PP
.B cd
recognizes the following syntax:
.DS
	\fBdir [ \fIdirectory \fB] [/w]
.DE
If no argument is given,
.B cd
displays the contents of directory you are accessing on the remote host.
The switch
.B /w
tells it to display only the name, grouped into five columns.
.PP
The command
.B ftp
also implements a version of the
.B dir
command.
It is described below.
.Sh "domain \(em Manipulate Information About Domains"
.PP
.II "commands^domain"
.II domain^command"
The command
.B domain
lets you manipulate information about domains.
Information about domains is kept in file
.BR $NETHOME/domains.txt .
For a discussion of the contents of this file, see the previous chapter
in this manual.
.PP
.B domain
takes the following forms:
.IP "\fBdomain addserver \fIip_address\fR"
Identify the site at
.I ip_address
as a domain-name server that you have permission to interrogate.
Hereafter, if
.B net
cannot find a domain in
.BR $NETHOME/domains.txt ,
it interrogates the system with
.IR ip_address .
When the domain-name server returns information about a domain,
.B net
records it in
.BR $NETHOME/domains.txt ,
to spare the trouble having to interrogate the domain-name server
repeated about the same domain.
.IP "\fBdomain dropserver \fIip_address\fR"
Drop the system at
.I ip_address
from your list of domain-name servers.
.IP "\fBdomain suffix .\fIabc\fR"
Set to \fB.\fIabc\fR the suffix used by default with a partial domain name.
For example, the command
.DM
	domain suffix .edu
.DE
sets the default suffix to
.BR .edu .
Hereafter, if you enter a partial domain name (e.g., \fBraven\fR or
\fBlepanto\fR) that
.B net
does not recognize, it will interrogate the domain-name server for that
name plus \fB.edu\fR (e.g., (\fBraven.edu\fR or \fBlepanto.edu\fR).
.IP "\fBdomain trace on|off\fR"
Turn domain-name tracing on or off.
If domain-name tracing is turned on,
.B net
displays information about its interactions with the domain-name server.
.Sh "echo \(em Change Response to telnet Echo Request"
.PP
.II "protocol^TELNET"
.II "TELNET^protocol"
.II "commands^echo"
.II echo
The TELNET protocol specifies that in the absence of a negotiated agreement
to the contrary, neither end echoes data received from the other.
In this mode, a client TELNET session echoes keyboard input locally
and performs other line-discipline tasks, but sends nothing until you type
.BR <return> .
.PP
When communicating from keyboard to keyboard,
.B net
uses the standard local-echo mode, so setting the echo parameter has no effect.
However, \*(CO sends a TELNET
.B "WILL ECHO"
instruction immediately upon receiving a request to connect from an incoming
TELNET session.
The command
.B echo
lets you set whether
.B net
accepts a request to echo.
It takes one argument:
either
.B accept
or
.BR refuse .
.B "echo accept"
tells
.B net
to return the instruction
.B "DO ECHO"
in response to a
.B "WILL ECHO"
request, and begin to echo; whereas
.B "echo refuse"
tells
.B net
to return
.B "DONT ECHO"
and refuse to echo.
.B refuse
is the normal setting, because echoing every character sent via
.B telnet
consumes a great deal of network resources.
Typing
.B echo
without an argument toggles the response.
.PP
Do not confuse the command
.B echo
with the server
.BR echo .
The latter responds to
.B ping
requests from remote hosts.
.Sh "eol \(em Display or Modify End-of-Line Behavior"
.PP
.II "commands^eol"
.II eol
The command
.B eol
manipulates the TELNET end-of-line behavior when the command
.B telnet
is in remote-echo mode.
It can take one argument:
either
.B unix
or
.BR standard .
.PP
In
.B standard
mode, each key is sent as\-is.
In
.B unix
mode,
.B net
translates carriage returns into line feeds.
This command is not necessary with all \*(UN systems;
use it only when you find that a given system
responds to line feeds but not to carriage returns.
.PP
When you installed
.BR cohulip ,
the command
.B "eol unix"
was written by default into file
.BR startup.net .
.PP
If you type
.B eol
without an argument, it displays the current end-of-line mode.
.Sh "escape \(em Display or Change the Escape Character"
.PP
.II "commands^escape"
.II escape
The command
.B escape
displays or changes the ``escape character'' \(em that is, the character
with which you can escape from conversation mode into command mode.
If you type
.B escape
without an argument, it displays the current escape character.
.PP
If you type it with a one-character argument,
.B net
makes that character the escape character.
It would not be a good idea to make a printable character the escape
character, lest you need to type that character while you are working.
If you type a control character, that character may not be echoed onto
the screen, but
.B escape
recognizes it anyway.
.PP
By default,
.B net
sets its escape character to \fB<ctrl-]>\fR (0x1D).
.Sh "etherstat \(em Display Ethernet Statistics"
.PP
.II "commands^etherstat"
.II etherstat
The command
.B etherstat
displays statistics for your NE2000 Ethernet controller,
if it is configured to keep statistics.
.Sh "exit \(em Exit From net\fR"
.PP
.II "commands^exit"
.II exit
The command
.B exit
exits you from
.B net
and returns you to the program from which you invoked it
(either the shell or a shell script).
It attempt to close all open sessions before it shuts itself down.
.Sh "finger \(em Display Information About Users"
.PP
.II "commands^finger"
.II finger
The command
.B finger
displays information about ``known users'' on a system.
To become a ``known user,'' a file named
.IB userid .txt
must reside in directory
.BR $NETHOME/finger .
For example, for user
.B fred
to become a known user, a file named
.B fred.txt
must be inserted into directory
.BR $NETHOME.finger .
The user can describe himself in this file;
the description can be serious or whimsical, as he desires.
.PP
.B finger
also recognizes the file
.BR $NETHOME/finger/all.txt .
This file usually is used to hold information about the entire system,
its users, and subordinate system; although it can, in fact,
hold anything you want.
.PP
The command
.B finger
lets you display a finger-file for a recognized user.
It takes the following forms:
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
Using host station \fBfarhost\fR as an example, typing the command
.DM
	finger @myhost
.DE
displays something like:
.DM
	Known users at myhost:
		alice
		joe
		all
.DE
Then typing the command
.DM
	finger all@myhost
.DE
retrieves the contents of file
.B all.txt
from the remote host's finger directory and displays it on the screen.
.PP
You can use
.B finger
to examine your own system, if you wish, to see if the connection is
working correctly.
.Sh "ftp \(em File Transfer Protocol"
.PP
.II "commands^ftp"
.II ftp
.II "RFC 959"
The command
.B ftp
implements the Internet's file-transfer protocol (FTP; RFC 959).
With it, you can log into a remote system and perform file transfers.
(The scope of what you can do and where you can work depends upon what
permissions the remote system grants you.)
.PP
When you use the command
.B ftp
to access a remote system,
.B net
established two channels to that remote host, both of which run
over one physical device.
One channel transfers data between the two hosts \(em usually
files, directory listings, or messages and prompts.
The other channel sends commands from your local host to the remote host,
and receives the remote host's reply.
Thus, you can send commands to the remote host even while the remote host
is transferring a file to your system \(em although
the variety of commands that you can send varies depending upon what the
remote host is doing at any given moment.
The commands and replies that can be sent over the command channel are
precisely defined by FTP's RFC; a number of these commands will be described
in the following paragraphs.
.PP
The
.B net
command's version of
.B ftp
implements many (though by no means all) of the commands found in a
full implementation of
.BR ftp .
Note, too, that because
.B ftp
is a sub-command of
.BR net ,
instead of as a process in its own right, you must escape from
.B ftp
back into
.BR net 's
command mode to perform some tasks; in particular, you must do so to
close an FTP session.
.PP
To open an FTP control channel to a remote host, type:
.DS
	\fBftp \fIremote.host\fR
.DE
.B net
opens the channel to the remote host.
Usually, you will see a banner, then a prompt for login and password.
Many sites permit user
.B anonymous
to log in; for information, check a published description of the site.
(For information on how to set up your machine so that others users can
log in via
.BR ftp ,
see the next chapter in this manual, which describes how to administer
.BR cohulip .)
.PP
After you have logged into the remote site,
.B net
displays the prompt
.BR ftp> ;
this indicates that you are in ``conversation mode'' with
.BR ftp .
When you type something,
.B net
checks whether it is one of the following
.B ftp
commands; if it is,
.B net
executes it; otherwise,
.B net
passes it intact to the remote site.
.B net
displays the remote server's response directly on your the screen.
Note that while
.B net
is transferring data via
.BR ftp ,
the only
.B ftp
command you can execute is
.BR abort ,
which aborts the file transfer.
.PP
If you fail to log into the remote site, or if something else goes wrong,
press the escape key to return to
.BR net 's
command mode, and from there type the command
.B close
to close the
.B ftp
session.
Then you can try again.
.PP
The following lists the commands that you can enter at the
.B ftp>
prompt:
.IP "\fBabort\fR"
.II "ftp^abort"
Aborts a transfer of data before it concludes.
The transfer of data can be initiated by the commands
.BR get ,
.BR put ,
or
.BR dir .
If invoked while
.B net
is receiving a file,
.B abort
simply resets the data connection;
the next incoming data packet generates the message TCP RST (reset)
in response, which clears the remote server.
If invoked while
.B net
is sending a file,
.B abort
sends a premature EOF (end of file) message.
In either case,
.B abort
leaves a partial copy of the file on the destination machine,
which must be removed by hand.
.IP "\fBbell\fR"
Tell
.B net
to ring your terminal's bell when it finishes transferring a file.
.IP "\fBcd \fIdirectory\fR"
.II "ftp^cd"
Change to
.I directory
on the remote host.
You must have permission to change to the target directory.
.IP "\fBcwd [ \fIdirectory \fB]\fR
.II "ftp^cwd"
This command changes the working directory \(em that is,
the directory you are using on the remote host.
It resembles the command
.BR cd ,
except that if you do not give it an argument it displays the name of the
current working directory.
.IP "\fBdele \fIfile\fR"
.II "ftp^dele"
Delete
.I file
from the host machine.
You can use this command only if the remote host has granted you ``destroy''
permission.
.IP "\fBdir [\fIremote_file \fB[ \fIlocal_file \fB] ]\fR"
.II "ftp^dir"
.B net
sets the command
.B LIST
on the control channel, to print the contents of the current working directory.
If you invoke
.B dir
with one argument, it prints information about
.BR file ,
which resides on the remote host.
Note that
.I file
can be a directory as well as an ordinary file; and that it can be either
a full path name or a relative path name.
If you invoke
.B dir
with two arguments,
.B dir
assumes that the second argument names a file on your local host,
and writes its output into that file.
.IP
You can use other options with
.BR dir ,
but which options are available vary from one remote host to another.
Many \*(UN-based hosts will let you a regular expression to
form the name of
.IR remotefile ;
for example, the command
.DM
	dir [A-M]*.gif
.DE
displays every file whose name that begins with the letters `A' through `M'
and ends with the string ``.gif''.
Likewise, many \*(UN-based remote hosts recognize the option
.BR \-t ,
which prints the files in the order in which they were last modified, from
youngest to oldest.
For details on the options that are potentially available to you, see the
entry for
.B ls
in the \*(CO Lexicon.
.IP "\fBget \fIremote_file \fB[ \fIlocal_file \fB]\fR"
.II "ftp^get"
.B net
sends the commands
.B PORT
and
.B RETR
on the control channel, to copy
.I remote_file
from the remote host into
.I local_file
on your local host.
If you do not name a
.IR local_file ,
.B net
copies
.I remote_file
into a file of the same name on your local system.
.IP \fBhash\fR
.II "ftp^hash"
Toggle hashing.
When hashing is on,
.B ftp
displays the ``hash'' character `#' for every one kilobyte of data it
processes during a file transfer.
By watching the hash marks appear on your screen, you can get a rough
idea of how well (or how poorly) the file transfer is proceeding.
If the file transfer appears to be hung, you can use the command
.B abort
to abort the file transfer, and try again some other time.
.IP \fBlist\fR
.II "ftp^list"
This is a synonym for the command
.BR dir .
.IP "\fBls \fIremote_file \fB[ \fIlocal file \fB]\fR"
.II "ftp^ls"
.B net
sends the command
.B NLST
over the control channel to print, in abbreviated form, the contents of
the current working directory on the remote host.
If you name a
.IR local_file ,
.B ls
writes its data into that file on your local host instead of to your screen.
Only the names of the files are shown, just as with the \*(CO command
.BR ls .
.IP "\fBmkdir \fIdirectory\fR"
.II "ftp^mkdir"
Create
.I directory
on the remote host.
The remote host must have granted you ``create'' permission before
you can use this command.
.IP "\fBpass \fIpassword\fR"
.II "ftp^pass"
.B net
automatically invokes this command while you are logging into the remote
system:
it requests you for your password, then passes what you type
to the remote host.
.IP
If you wish to change your identity after you have logged into a remote
host, first invoke the command
.BR user ,
then
.BR pass .
.IP "\fBput \fIlocal_file \fB[ \fIremote_file \fB]\fR"
.II "ftp^put"
.B net
sends the commands
.B PORT
and
.B STOR
over the control channel to copy
.I local_file
from your local into into
.I remote_file
on the remote host.
If you do not name a
.IR remote_file ,
.B net
writes
.I local_file
into an identically named file on the remote host.
The remote host must have granted you ``write'' permission before you can
use the command.
.IP \fBpwd\fR
.II "ftp^pwd"
Print working directory \(em that is, print the name of the directory
within which you are working on the remote host.
.IP \fBquit\fR
.II "ftp^quit"
Close the current
.B FTP
session.
.B net
notifies the remote host that you are quitting, so the connection is
closed gracefully.
This is equivalent to the command
.B bye
under most implementation of
.BR ftp .
.IP "\fBrmdir \fIdirectory\fR"
.II "ftp^rmdir"
Remove
.I directory
from the remote host.
Before you can use this command, the remote host must have granted you
``destroy'' permission, and
.I directory
must be empty.
.IP "\fBtype [a|i|l\fIbytesize\^\fB]\fR"
.II "ftp^type"
Send the command
.B TYPE
on the control channel to tell remote server the type of file to be transferred.
The following types are recognized:
.RS
.IP \fBa\fR 0.3i
ASCII mode:
the file consists of printable characters,
with the end of a line marked by a CR/LF sequence.
.IP \fBi\fR
Image mode:
send the file exactly as it appears on the remote host.
Set this mode before you download any binary data, e.g., executable
programs, object modules, images, or archives of compressed material.
.IP \fBl\fIbytesize\fR
The same as image mode, except that
.B net
notifies the remote system that the word length is
.I bytesize
(typically, eight).
Use this to download files from systems with an unusual word size
(e.g., DECSYSTEM-10s and 20s).
.RE
.IP "\fBuser \fIusername\fR"
.II "ftp^user"
This command prompts you for your user identifier, and passes it to the
remote host.
This command is invoked automatically when you are first logging into
the remote host.
.IP
You can also invoke
.B user
after you have logged in, to change you identity on the remote host.
If you do so, you must follow it with the command
.B pass
(described above).
.Sh "ip \(em Manipulate the IP Handler"
.II "commands^ip"
.PP
The following commands let you manipulate the IP handler \(em that is,
the portion of
.B net
that processes the Internet Protocol:
.IP "\fBip address [ \fImyaddress \fB]\fR"
.II "ip^address"
Set your local IP address to
.IR myaddress .
If invoked without an address, this command displays your
local host's IP address.
.IP "\fBip status [ \fIclear \fB]\fR"
.II "ip^status"
This command displays information about your local host's IP status.
The first line of output displays some SLIP information (should you be
using SLIP).
The second line displays IP statistics,
such as total packet counts and error counters of various types.
.IP
.II ICMP^definition
.II "Internet Control Message Protocol"
.B "ip status"
also displays statistics about the Internet Control Message Protocol (ICMP),
including the number of ICMP messages of each
type sent or received.
.IP "\fBip ttl [ \fIvalue \fB]\fR"
.II "ip^ttl"
Set the time-to-live of outgoing IP datagrams to
.I value
data hops.
Every time a remote system handles a datagram, it decrements that datagram's
time-to-live value; a datagram ``dies'' when its time-to-live reaches zero.
Should a datagram's address be unresolvable, the time-to-live mechanism
prevents it from circulating endlessly through the network.
If you set
.I value
to too low a value, a datagram will not live long enough to reach its
destination; if you set it too high, you could be wasting network resources.
.IP
If you invoke this command without a
.IR value ,
it displays the current value of the time-to-live parameter.
.Sh "help \(em Display a help file"
.II "commands^help"
.II help
.PP
The command
.B help
displays the contents of a help file.
It has the following syntax:
.DS
	help [ \fIsubject \fB]\fR
.DE
.B help
displays the contents of file \fB$NETHOME/\fIsubject\fB.hlp\fR,
which presumably contains information to help you with
.IR subject .
The file is displayed through a pager; to quit, type the escape character
.BR <ctrl-]> .
.PP
If you do not specify
.IR subject ,
.B help
displays the contents of file
.BR $NETHOME/net.hlp .
.B cohulip
comes with help files for some, but not all, of its commands.
You can prepare other help files if you wish.
.Sh "hostname \(em Set the Local Host's Name"
.II "commands^hostname"
.II "hostname"
.PP
.B net
uses a name to identify your local host to remote systems, when it uses
FTP or SMTP to exchange files with a remote system.
The command
.B hostname
lets you set the name of your local host.
.PP
.B hostname
recognizes the following syntax:
.DS
	\fBhostname [ \fIname \fB]\fR
.DE
.B hostname
sets the name of your local host to
.IR name .
Note that
.I name
must be a text string, not an IP address.
.PP
If invoked without an argument,
.B hostname
displays your local host's name.
.PP
You should set your local host's name to its \*(UU name,
which is recorded in file
.BR /etc/uucpname ,
although you are not required to do so.
.Sh "kick \(em Kick Out Pending Text"
.II "commands^kick"
.II "kick"
.PP
The command
.B kick
flushes the TCP buffer, and launches all datagrams that it is ``sitting on''.
You can use this if a TCP session is being ``balky'' for any reason.
.Sh "log \(em Log Server Activity"
.II "commands^log"
.II log 
.PP
The command
.B log
lets you record all activity on the servers that you launch on your local host.
It recognizes the following syntax:
.DS
	\fBlog [ \fIlogfile \fB| stop ]\fR
.DE
If invoked without an argument,
.B log
tells you whether
.B net
is logging server activity.
Logging entries appear as follows:
.DM
	Jul 28 22:02:45 198.162.16.89:1001 - open Finger
	Jul 28 22:02:57 198.162.16.89:1002 - open Telnet
	Jul 28 22:04:29 198.162.16.89:1003 - open FTP
	Jul 28 22:09:16 198.162.16.89:1004 - rw logged in, ID 
	Jul 28 22:10:14 198.162.16.89:1003 - close FTP
	Jul 28 22:11:37 198.162.16.89:1004 - RETR /projects/smail/smail3.1.28.tz
	Jul 28 22:32:29 198.162.16.89:1008 - open FTP
	Jul 28 22:32:35 198.162.16.89:1008 - rw logged in, ID 
	Jul 28 22:33:40 198.162.16.89:1004 - close FTP
.DE
.PP
If invoked with the argument
.IR logfile ,
.B log
turns on logging (if logging was not yet on), and appends the logging
information onto
.I logfile
on your local host.
You must have permission to write into
.IR logfile ,
or logging will not work.
Because entries are appended onto
.IR logfile ,
you can use the same file repeatedly, to keep a running log of your network
activity.
.PP
.II "log stop"
If invoked with the argument
.BR stop ,
.B log
stops logging.
.Sh "ping \(em Reach Out and Touch A Remote Host"
.II "commands^ping"
.II "ping"
.PP
The command
.B ping
sends a datagram to a remote host, then tells you how long it took the
remote host to respond.
This is an easy way to tell if the remote host is ``on the air,'' and, if so,
how quickly the connection between your machines is functioning.
.PP
.B ping
recognizes the following syntax:
.DS
	\fBping [ \fIremotehost \fB[ \fIinterval \fB] ] | clear\fR
.DE
The command \fBping \fIremotehost\fR ``pings''
.IR remotehost .
When it receives a reply,
.B net
displays an information reply something like the following:
.DM
	12.34.5.6: echo reply id 0 seq 124, 55 ms
.DE
This indicates the round trip time was 55 milliseconds.
(A round-trip time of zero usually indicates that the remote system was
inaccessible for some reason.)
.PP
The command \fBping \fIremotehost interval\fR
tells
.B net
to ``ping''
.I remotehost
every
.I interval
seconds.
You can use this to command to profile the connection time between your
local host and
.IR remotehost .
Also,
if your system is hooked into a network that drops a connection when activity
ceases for a given interval, you can use this command to keep your
system on the air.
.PP
To stop a continual ping, use the command
.BR "ping clear" .
.Sh "pwd \(em Print Working Directory"
.II "commands^pwd"
.II "pwd"
.PP
This command is just like the \*(CO command
.BR pwd :
it displays the directory you are accessing on your local host.
.Sh "record \(em Record All Activity"
.II "commands^record"
.II "record"
.PP
The command \fBrecord \fIlogfile\fR tells
.B net
to open
.I logfile
and append onto it all data it receives during the current session.
.B net
also appends all data it sends on the current session,
except for TELNET sessions in remote-echo mode.
.PP
The command
.B "record off" 
tells
.B net
to stop recording and close
.IR logfile .
.PP
Note that this command is not supported for FTP sessions.
.Sh "reset \(em Blow Away a Session"
.II "commands^reset"
.II "reset"
.II TCB^definition
.II "TCP Control Block"
.PP
The command
.B reset
deletes the TCP Control Block (TCB) that belongs to a given session.
This immediately terminates your half of a session \(em in effect, it
``hangs up the phone'' on a session.
.PP
If you invoke
.B reset
with a session's number as an argument,
.B net
deletes the TCB for that session; if you invoke it without an argument,
.B net
deletes the TCB of the current session, should there be one.
.PP
Use this command with caution, because it does not inform the remote host
that the connection no longer exists.
You should use it only to get rid of a lingering, half-open connection
after a remote host has crashed or otherwise gone ``off the air''.
.Sh "route \(em Manipulate the Routing Table"
.PP
.II "commands^route"
.II "route"
The command
.B route
manipulates the IP routing table \(em that is, the table that tells
.B net
how to get from here to there.
It comes in the following three basic forms:
.IP \fBroute\fR
Display the contents of the routing table.
.IP "\fBroute drop \fIremote.host\fR"
.II "route^drop"
Drop the entry for
.I remote.host
from the routing table.
If a packet arrives for the 
deleted address and a default route is in effect, the default will be used.
.IP "\fBroute add \fIremote.host\fB[/\fIbits\^\fB]|default \fIinterface \fB[ \fIgateway \fB[ \fImetric \fB] ]\fR"
.II "route^add"
Add a route to the routing table.
.I remote.host
identifies the remote system to which you wish to route datagrams.
If this system is described in file
.BR $NETHOME/hosts.net ,
you can identify it by name; otherwise, you must give its IP address.
.IP
.I interface
names the
.B net
interface you are linking to
.IR remote.host .
You must first have used the command
.B attach
to define
.I interface
before you can use the it in a
.B route
command.
.IP
If
.I remote.host
is not local and you are using Ethernet to access your network,
you must also name the
.I gateway
through which your local host accesses
.IR remote.host .
(This is not required for SLIP, because a serial connection goes
directly from your system to the remote system, with no chance for the
datagrams to go astray.)
If
.I gateway
is described in file
.BR $NETHOME/hosts.net ,
you can use its name; otherwise, you must give its IP address.
Again, if this system is described in file
.BR $NETHOME/hosts.net ,
you can identify it by name; otherwise, you must give its IP address.
.IP
The optional suffix
.BI / bits
to
.I remote.host
specifies how many bits of its IP address are to be considered significant
in routing comparisons.
If not specified,
.B net
assumes that all 32 bits are significant.
With this option, you can use a single routing-table entry
to refer to many hosts whose IP addresses begin with the same bit-string.
For example, ARPA classes A, B, and C networks would use suffixes of /8, /16,
and /24 respectively.
For example, the command
.DM
	route add [44]/8 sl0 [44.64.0.2]
.DE
tells
.B net
to route to [44.64.0.2] every IP address that begins with ``44''.
.IP
When an IP address matches more than one entry in the routing table,
.B net
selects the route that matches the IP address most tightly \(em that is,
the entry with largest
.I bits
parameter.
This lets you set a general rule for routing a large block of hosts,
then set exceptions.
.IP
The special destination
.B default
is used to route datagrams to addresses not in the routing table;
it is equivalent to specifying a
.BI / bits
suffix of 0 to any destination
.IR remote.host .
Be careful when you define default entries:
two nodes whose default entries point at each other will ``ping-pong''
datagrams until their time-to-live fields expire.
If the routing table has only one entry, that should be the default.
.IP
The following gives some more examples of the
.B route
command:
.DM
	# Station with IP address [44.0.0.10] on local channel
	# needing an explicit route for some reason
	route add [44.0.0.10] ax0
.DE
.DM
	# Route datagrams to IP address 44.0.0.3 to SLIP line #0.
	# No gateway is needed because SLIP is point-to point.
	route add [44.0.0.3] sl0
.DE
.DM
	# Route all default traffic to the gateway on the local Ethernet
	# with IP address [44.0.0.1]
	route add default ec0 [44.0.0.1]
.DE
.DM
	# The local Ethernet has an ARPA Class-C address assignment;
	# route all IP addresses beginning with 192.4.8 to it
	route add [192.4.8]/24 nx0
.DE
.Sh "session \(em Display Information About Sessions"
.PP
.II "commands^session"
.II session
The command
.B session
displays information every session that running on your local host.
The information displayed includes session number, remote TCP address,
and the address of the TCP control block.
.B net
prints an asterisk `*'  to the description of the current session;
to enter or return to conversation mode with that session, type
.BR <return> .
.PP
To enter conversation mode with a session other than the current session,
type
.B session
followed by that session's number.
.PP
If the TELNET server is enabled,
.B net
automatically notifies you when a remote user initiates a session upon your
system.
By typing
.B session
followed by the new session's number, you can enter conversation mode with
that incoming session.
.Sh "shell \(em Invoke the COHERENT Shell"
.PP
.II commands^shell
.II shell
The command
.B shell
invokes a \*(CO shell.
You can give commands directly to \*(CO;
.BR net ,
meanwhile, continues to ``mind the store.''
.PP
When you exit from the shell (either by typing
.B <ctrl-D>
or
.BR exit ),
\*(CO returns to working with
.BR net .
.PP
Note that you can also use \*(CO's virtual-console facility to give
commands to \*(CO while working with
.BR net .
This is in many ways much easier than invoking a subshell, then returning;
its main drawback is that it is quite easy to forget that you have a
network session running, and walk away and leave yourself logged into
a system thousands of miles away.
For details on virtual consoles, see the entry for
.B "virtual console"
in the \*(CO Lexicon.
.Sh "smtp \(em Simple Mail Transport Facility"
.PP
.II "smtp"
.II "commands^smtp"
The command
.B smtp
manipulates the SMTP server on your system.
It recognizes the following sub-commands:
.IP "\fBsmtp gateway [ \fIhostid \fB]\fR"
.II "smtp^gateway"
Display or sets the host to be used as a ``smart'' mail relay.
Any mail sent to a
.I hostid
not in the host table will instead be sent to the gateway for forwarding.
.IP "\fBsmtp kick\fR"
.II "smtp^kick"
Run through the outgoing-mail queue and attempt to deliver any pending mail.
This command is periodically invoked by a timer whenever
.B net
is running; this command allows the user to ``kick'' the mail system manually.
.IP
Note that if there was mail queued up and not sent when you exit
.BR net ,
there will be zero-length files with the suffix
.B .lck
extension in the directory
.BR mqueue .
These mail messages will not go when
.B net
is restarted; you must delete these files by hand.
.IP "\fBsmtp maxclients [ \fImaximum \fB]\fR
.II "smtp^maxclients"
Display or set to
.I maximum
the maximum number of simultaneous outgoing SMTP sessions
that
.B net
allows.
The default is ten; reduce it if network congestion is a problem.
.IP "\fBsmtp mode [queue|route]\fR
.II "smtp^mode"
Use
.B queue
to stage messages in the subdirectory
.B rqueue
so a mailing agent other than
.B net
can deal with it.
By default,
.B net
sets the mode to
.BR route .
.IP "\fBsmtp timer [ \fIseconds \fB]\fR"
.II smtp^timer
Display or set the interval, in seconds, between scans of the outbound
mail queue.
For example, the command
.DM
	smtp timer 600
.DE
tells
.B net
to check for outgoing mail every ten minutes and attempt to deliver anything
it finds, subject to the
.B maxclients
limit.
Setting
.I seconds
to zero disables queue-scanning altogether:
this is the default.
This value is recommended for stand-alone IP gateways that never handle mail.
.IP "\fBsmtp trace [ \fItrace_level \fB]\fR
.II "smtp^trace"
Display or set the trace level in the SMTP client.
This permits you to watch SMTP's conversations as it delivers mail.
.I trace_value
can be set to 0, 1, 5, or 7.
Zero disables tracing; this is the default.
Values 1, 5, and 7 give increasing levels of details.
.PP
Please note that this server works only with the mail-router
.BR bm .
This router works in a very non-standard way,
so you will have serious problems if you attempt to use it for your mail.
.\" .Sh "sokname \(em Toggle How Socket Names Are Displayed"
.\" .PP
.\" If compiled with the directive, SOKNAME, defined, sokname is available to 
.\" toggle between showing socket names, ports, and IP users as names or as 
.\" numbers.
.\" This option affects the tcp stat command as well as telnet, 
.\" finger, and other processes that show activity by process or user number 
.\" or name.
.\" Initially this information is shown as names.
.\" (IP addresses in 
.\" hosts.net must conform to the pattern, 44.78.0.2; not, 44.78.00.02, 
.\" because sokname does a string pattern match.)  Toggle off to reduce disk 
.\" activity.
.Sh "start \(em Invoke a Server"
.PP
.II commands^start
.II start
The command
.B start
starts a specific Internet server.
Each server services processes any requests that it receives from the
outside world.
.B start
commands usually are embedded in a
.B startup
file.
.PP
To use it, simply type
.B start
followed by the name of the server you wish to start up.
The variety of servers available depends upon how your version of
.B cohulip
was configured;
among the available servers are
.BR echo ,
.BR finger ,
.BR ftp ,
.BR smtp ,
.BR telnet ,
and
.BR telunix .
.PP
.II telunix
The command
.B "start telunix"
takes an optional third argument, which lets you set the number of the
socket to which the
.B telunix
server is attached.
By default, the
.B telunix
is attached to socket 513; however, the command
.DM
	start telunix 23
.DE
connects the
.B telunix
server to socket 23, which is the well-known port for TELNET.
Connecting
.B telunix
to port 23 lets users who access your system via TELNET log in as they
normally would, and so gain their normal suite of permissions.
For details on
.BR telunix ,
see the chapter on advanced
.B cohulip
that follows in this manual.
.Sh "stop \(em Shut Down a Server\fR"
.PP
.II "commands^stop"
.II stop
To shut down a server, type the command
.B stop
followed by the name of that server.
.PP
Note that the command \fBstop ping\fR
stops new connections from being made, but does not affect
sessions that are in progress.
To close an existing session, use the command
.BR close .
.Sh "tcp \(em Manipulate TCP"
.PP
.II "commands^tcp"
The command
.B tcp
lets you modify how
.B net
manages TCP.
It recognizes the following sub-commands:
.IP "\fBtcp irtt [ \fIseconds \fB]\fR
.II "tcp^irtt"
.II IRTT
.II "Initial Round Trip Time"
Display or set
.BR net 's
estimate initial round trip time (IRTT), in seconds.
The default is five seconds.
TCP uses this estimate for new TCP connections \(em that is, when TCP
establishes a new connection, it waits
.I seconds
before it retransmits a datagram.
As the TCP session progresses, TCP can measure how much time is needed for
a datagram round trip \(em i.e., the interval between TCP's dispatching of
a datagram, and its receipt of confirmation that the remote host received it.
.IP
If you are running a TCP session over a slow or busy channel, you should
increase the IRTT.
This helps to avoid the flurry of retransmissions that would otherwise
occur as the smoothed estimate settles down to the correct value.
.IP
You must issue this command before you start servers,
or it will not affect incoming connections.
(A typical IRTT for a
.B startup
script is 2000.)
.IP "\fBtcp kick \fItcb_address\fR"
.II TCB
.II "TCP Control Block"
.II "tcp^kick"
If there is data on the send queue of a given TCP control block (TCB),
this command forces an immediate retransmission.
.I tcb_address
gives the TCB's address; you can obtain this value by using the command
.BR "tcp status" .
.IP "\fBtcp mss [ \fIbytes \fB]\fR"
.II "tcp^mss"
.II MSS
.II "Maximum Segment Size"
When your system is establishing a TCP channel with a remote system, your
system and it negotiate the TCP Maximum Segment Size (MSS) \(em
that is, the size of the largest datagram to pass between them.
Each system offers its maximum, and the lesser of the two is selected.
.IP
The command
.B "tcp mss"
sets to
.I bytes
the MSS that your system will send to a remote system.
Changing the value of the MSS affects only future connections;
existing connections are unaffected.
This value normally is set in your
.B startup
script.
.IP
If you type
.B "tcp mss"
without a
.I bytes
argument,
.B net
displays the current size of the MSS.
.IP
By default,
.B cohulip
sets the MSS to 512 bytes.
When you installed
.B cohulip
onto your system, the command
.B "tcp mss 1500"
was written into your
.B startup
file; this is a more reasonable figure given the current state of
communications equipment.
If you find that a third-party networking scheme is handling fragmentation
incorrectly, set the MSS to 216; this allows 40 bytes for IP and TCP headers,
and so prevents fragmentation.
.IP "\fBtcp reset \fItcb_address\fR"
.II "tcp^reset"
.II "TCP Control Block"
.II TCB
Delete the TCB control block (TCB) that at
.IR tcb_address .
This is the same as issuing the
.B reset
command, and should be used just as cautiously.
.IP "\fBtcp rtt \fItcb_address milliseconds\fR"
.II "tcp^rtt"
Set the round-trip time (RTT) that is in the TCP control block (TCB)
.I tcb_address
to
.IR milliseconds .
This replaces whatever RTT that TCP computed for the session that this
TCB controls.
This command can help speed up recovery from a series of lost packets,
because it lets you manually bypass the normal retransmission-timing
mechanism.
.IP
If you set the RTT to a bogus value, you can help cause a major tie-up on
the Information Superhighway.
Therefore, use this command only if you know
.I exactly
what you are doing.
.IP "\fBtcp status [ \fItcb_address \fB]\fR
.II "tcp^status"
This command displays information about TCP sessions that are in progress.
.IP
If you enter this command without arguments, it displays a brief summary
of all sessions.
The summary includes several TCP-level statistics, plus a summary
of all existing TCP connections, including TCB address, send and receive
queue sizes, local and remote sockets, and connection state.
.IP
If you name a
.I tcb_address
on the command line,
.B net
displays a detailed dump of the TCP Control Block (TCB) at this address.
The dump includes send and receive sequence numbers, and timer information.
.IP "\fBtcp timertype \fB[ linear|exponential ] [ \fIminutes\fB ]\fR"
.II "tcp^timertype"
Display or set the method used to set the TCP retry/backoff timer
and the maximum value to which it it will back off.
.IP
When entered without an argument, the command
.B "tcp timertype"
displays the current type of timer, and the backoff time.
.IP
When entered with one argument,
.B net
attempts to set the timer's type to that argument.
The argument must be
.B linear
or
.BR exponential .
.IP
The optional argument
.I minutes
sets the maximum backoff time.
.PP
The default is exponential timing, with a maximum backoff of 30 minutes.
.IP "\fBtcp window [ \fIbytes \fB]\fR"
.II "tcp^window"
Display or set the size, in bytes, of the receive window that TCP uses
when it creates a new connection.
This command does not affect existing connections.
The default is 2,048 bytes.
If you encounter fragmentation problems with third party networking schemes,
set the TCP window to 1,080.
.Sh "telnet \(em Converse With Another System"
.PP
.II telnet
.II "commands^telnet"
The command
.B telnet
uses the TELNET protocol to connect with a remote host.
When the connection is made, you can converse with that host,
just as if your machine were a terminal plugged into it.
.PP
The command
.B telnet
takes the following syntax:
.DS
	\fBtelnet \fIremote.host \fB[ \fIport \fB]\fR
.DE
.I remote.host
names the host to contact; this can be its domain name, or an IP address.
.I port
names the port to connect to.
Some remote hosts establish special ports for special purposes; or you
may wish to contact a well-known port for another protocol (e.g., SMPT
or FTP) in order to debug a connection (or send forged mail).
If you do not name a
.IR port ,
.B telnet
connects to port 23, which is the well-known port for the TELNET protocol.
.Sh "trace \(em Trace the Handling of Datagrams"
.PP
.II trace
.II "commands^trace"
The command
.B trace
lets you establish and customize the tracking of datagrams.
You can use the information that a trace yields to help debug a connection.
.PP
.B trace
recognizes the following syntax:
.DS
	\fBtrace [ \fIinterface flags\fB [ allmode|cmdmode ] ]\fR
	\fBtrace to \fIoutputfile\fR
.DE
When invoked without arguments,
.B trace 
lists all defined interfaces and their tracing status.
.PP
The first argument,
.IR interface ,
names the interface that you wish to trace.
The interface first have been defined through the command
.B attach
before you can trace it.
.PP
The second command
.IR flags ,
is a three-digit number that controls some aspects of tracing.
The first (leftmost) digit controls the type of tracing, as follows:
.IP \fB0\fR 0.3i
Decode protocol headers, but do not display data.
.IS \fB1\fR
Decode protocol headers; display only the data.
Data are displayed as ASCII characters, 64 per line;
unprintable characters are displayed as periods.
.IS \fB2\fR
Decode protocol headers; display both data and headers.
Data and headers are displayed in hexadecimal and ASCII, 16 characters per line.
.PP
The second (middle) digit of
.I flags
controls the tracing of incoming datagrams:
one enabled tracing, whereas zero disables it.
.PP
The third (rightmost) digit of
.I flags
controls the tracing of output datagrams:
if one, tracing is enabled; if zero, it is disabled.
one enabled tracing, whereas zero disables it.
.PP
For example, setting
.I flags
to
.B 011
enables tracing of both incoming and outgoing datagrams, but displays
information only about the headers.
.PP
The optional third argument to
.B trace
lets you select whether traced packets are always displayed.
.B allmode
displays tracing all the time, regardless of whether you are in
command mode or conversation mode.
.B cmdmode
tells
.B net
to display tracing only when it is in command mode.
.PP
Finally, the command \fBtrace to \fIoutputfile\fR redirects the tracing
from the screen into
.IR outputfile .
This command lets you examining tracing information at your leisure.
.Sh "udp status \(em Display Status of UDP Receive Queues"
.PP
.II udp
.II "commands^udp"
The title says it all:
this command displays the status of every UDP receive queue.
.Sh "upload \(em Write File Into Current TELNET Session"
.PP
.II upload
.II "commands^upload"
This command opens a file and writes its contents into the current session,
just as if you had typed those contents from the keyboard.
When it has finished copying the file to the session,
.B net
displays the message,
.BR "Uploading off" .
.PP
This command is valid only with a TELNET session.
You can use it to execute on the remote host a script that resides on
your local host.
.SH "Where To Go From Here"
.PP
For some technical background of the commands used in this chapter,
see the chapter \fITCP/IP History and Design\fR, which appears earlier
in this manual.
