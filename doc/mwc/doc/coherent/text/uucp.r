.II UUCP^tutorial
.ds TL "UUCP Remote Communication"
.NH "UUCP, Remote Communications Utility"
.PP
\*(UU is a set of programs that together let you
communicate in an unattended manner with remote \*(CO and \*(UN sites.
The term
.I UUCP
is an abbreviation for \*(QL\*(UN to \*(UN copy\*(QR; as its name
implies, \*(UU was developed under the \*(UN operating system.
.PP
\*(UU allows your \*(CO system to talk
to other computers that also run \*(CO or \*(UN.
It can transmit files and mail to other systems and receive material from them,
without needing you to guide it by hand every step of the way.
Moreover, you can instruct
\*(UU to telephone other computers at the same time each day;
this permits regular, orderly
exchange of mail, news, and files among computers, and allows you
to take advantage of lower telephone rates during off-peak periods.
In a similar fashion, \*(UU allows other systems to log into your system,
to exchange mail or other information, and otherwise perform useful tasks.
.PP
Numerous \*(UU systems have linked together to create an informal
network called the \fIUsenet\fR.
Many megabytes of source code, news, and technical information are
available across the Usenet.
Anyone who is connected to the Usenet can exchange mail with anyone
else who is also connected to the Usenet.
All that is required to hook into the Usenet is to obtain a \*(UU
connection to anyone else who is connected to the Usenet.
.PP
You can use \*(UU only if you have telephone access to another computer that
runs \*(UU, and if your system and the remote system with which you wish to
communicate have been described to each other.
\*(UU is standard with \*(CO and \*(UN, and can be purchased for \*(MD.
If you wish to copy files from another system, you
must arrange with the system administrator of
that system before you can begin to use \*(UU.
Likewise, if you want someone else to dial into your system to upload or
download files, you must first describe that system to your copy of \*(UU.
.Sh "Contents of This Tutorial"
.PP
This tutorial describes \*(UU and tells you how to set
up and run your \*(UU system.
It contains the following sections:
.IP \(bu 0.3i
An overview of \*(UU.
.IP \(bu
How to set up your modem to dial out.
.IP \(bu
How to set up \*(UU to contact
.BR mwcbbs ,
the Mark Williams bulletin board.
.IP \(bu
How to use the \*(CO utility
.B uuinstall
to set up \*(UU to contact
.BR mwcbbs .
.IP \(bu
How to set up your \*(UU system to accept calls from remote systems.
.IP \(bu
How to use the \*(UU utilities to exchange files and mail with remote
systems.
.IP \(bu
How to debug some common problems with \*(UU.
.IP \(bu
How to administer your \*(UU system.
.IP \(bu
A brief introduction to networks.
.PP
Try as we might, there is no way to present all of \*(UU in a brief tutorial.
If you wish to explore the heights and depths of \*(UU, we urge you to
acquire the following books:
.IP \(bu 0.3i
O'Reilly, T.:
\fI!%@:: A Directory of Electronic Mail Addressing and Networks.\fR
Sebastapol, Calif, O'Reilly & Associates Inc., 1989.
.IP \(bu
O'Reilly, T.; Todino, G.:
\fIManaging UUCP and Usenet\fR.
Sebastopol, Calif, O'Reilly & Associates Inc., 1987.
.IP \(bu
Krol, E.:
\fIThe Whole Internet:
User's Guide & Catalog.\fR
Sebastopol, Calif, O'Reilly & Associates Inc., 1992.
\fIHighly recommended.\fR
.IP \(bu
Seyer M.D.:
\fIRS-232 Made Easy:
Connecting Computers, Printers, Terminals, and Modems\fR.
Englewood Cliffs, NJ, Prentice-Hall Inc., 1984.
.SH "An Overview of UUCP"
.PP
\*(UU is a set of programs that exchange files with other computers that
run \*(UU.
You can set aside files or mail messages to be transferred
to another computer; \*(UU regularly checks to see if material
has been set aside to be transferred, dials the remote system, and
exchanges the files without requiring your assistance.
.PP
This appears to be a simple function, but it can be extremely useful to you.
Suppose, for example, that you run a real-estate office
that is a member of an organization with regional and national offices.
You can tell \*(UU to call your regional office each night,
to send a file of your new listings and to accept a file
of new listings in your district that had come from other local offices.
Likewise, your association's regional office can telephone the national office
each night to receive new listings in your region, which can then be passed on
automatically to the appropriate neighborhood offices.
All of this information can be transferred at night, when telephone
rates are lowest, and without needing you to be at the console.
When you come to work the next morning, you will have the latest
listings instantly available on your terminal.
.PP
In brief, what \*(UU offers is the ability to join a \fInetwork\fR
of computers, in which every user of every computer can exchange information
with every user on every other computer, automatically.
What computer networks can do is limited only by your need to exchange
information with other computer users, and by your imagination.
.Sh "Implementations of UUCP"
.PP
.II "Taylor, Ian"
This version of \*(CO implements \fBTaylor UUCP\fR,
a \*(UU package written be Ian Taylor (\fBian@airs.com\fR)
with numerous contributions from other people in the USENET community.
\*(CO does not implement the full suite of \*(UU
utilities provided with the Taylor \*(UU package; however, it does
implement the enough utilities for you to set up a robust \*(UU site
on your system.
.PP
The source code for the full Taylor distribution is available via
.B ftp
from various sources, and can be downloaded from
.BR mwcbbs .
For a description of the full package, see Ian Taylor's documentation
for his package, which is included with your \*(CO package.
.PP
This chapter presents examples to help you learn how to set up a simple
\*(UU configuration for a remote site.
It does not discuss the more exotic features of \*(UU;
however, the information given in this chapter is sufficient
for you to set up communications with most remote sites.
.PP
For more background information on the (sometimes arcane) subjects of serial
ports and modems, see the Lexicon entries for
.BR UUCP ,
.BR modem ,
.B asy
(the serial-port driver), and
.BR RS-232 .
These discuss aspects of serial communication, and point you to other
articles that you may find helpful.
.Sh "Programs"
.PP
The \*(UU system uses the following programs to do its work:
.IP \fBuucico\fR 1.0i
Call remote systems:
log into the remote system and transfer files.
.IP \fBuuconv\fR
Convert configuration files into Taylor format.
You will use this only if you are porting your system from another
implementation of \*(UU.
.IP \fBuucp\fR
\*(UN-to-\*(UN Copy:
copy files from one computer to another.
Be sure not to confuse the command
.B uucp
with the \*(UU system, despite their similar names.
.II "Soviet Union"
(Note that the name of this utility is retained for the sake of convenience,
and because the abbreviation of the phrase \*(CO-to\*(CO Copy would remind
users of the late Soviet Union.)
.IP \fBuuencode\fR
Translate binary files into printable ASCII characters for transmission
to another system.
.IP \fBuudecode\fR
Translate files encoded by \fBuuencode\fR back into object code.
.IP \fBuuinstall\fR
This program displays a template on your screen, and helps you describe
a system to \*(UU relatively painlessly.
.IP \fBuulog\fR
Read the \*(UU logs, which record what \*(UU does.
.IP \fBuumkdir\fR
Create a directory for a remote site.
This command is invoked by
.B uuinstall
and other programs.
.IP \fBuumvlog\fR
Copy the current \*(UU log files into backup files.
Throw away all log files older than a requested number of days.
\*(UU logs everything that it does; and since it does a lot, its log files
can grow very large very quickly.
.B uumvlog
ensures both that you have enough information
on your system to diagnose problems
with uucp, and that the \*(UU log files do not overwhelm your system.
.IP \fBuuname\fR
List the systems that your computer can reach.
.IP \fBuupick\fR
``Pick up'' files that have been uploaded to your system from a remote site.
.IP \fBuusched\fR
This is a script which invoke the command
.B uucico
to call all systems that have jobs waiting for them.
.IP \fBuuto\fR
This is a script that invokes the command
.B uucp
to copy files from your system to another system via \*(UU.
.IP \fBuutouch\fR
Create a file that triggers a call to a named remote system.
.IP \fBuutry\fR
Force a call to a remote system, for debugging purposes.
.IP \fBuux\fR
Execute a command on a remote system.
.IP \fBuuxqt\fR
Check directory \fB/usr/spool/uucp/\fIsitename\fR and execute all
files therein that have the prefix ``X.''
.PP
Two other programs, while not part of \*(UU \fIper se\fR, are used by it:
.IP \fBttystat\fR 1.0i
Check the status of your asynchronous ports.
If \*(UU is not receiving files from other systems or not sending
files to other systems, it may be because the appropriate ports
have not been enabled.
.IP \fBmail\fR
Send ``electronic mail'' to another person, either on your system
or on another system via \*(UU.
.SH "Files and Directories"
.PP
As mentioned earlier, your system can use \*(UU to contact many different
remote sites, and can have many different sites contact it.
Each site differs from all others in many respects:
by its name;
by the telephone number at which you call it;
by the permissions it may grant you and you may grant it;
by the day of the week and the time of day during which you may wish to
call it;
and by the procedure you must follow to log into it.
Remote sites may also differ with regard to the port by which you contact them;
the manner in which you contact it (direct connect or via modem);
the protocol with which you exchange files;
and the name by which your system identifies itself to the remote system.
.PP
As you can see, \*(UU needs a considerable amount of information before it can
communicate with a remote site.
\*(UU reads this information from data files.
The processing of setting up communicate with a remote system means that you
write the correct information about that site into each of the appropriate
\*(UU data files.
This process will be confusing at first \(em in part
because some of the notation is rather obscure, and in part because there's
simply a lot of it, and some of the information needed may touch on aspects
of your system about which you may not know very much.
.PP
Each implementation of \*(UU has its own suite of data files that you must
manipulate to set up a remote site.
\*(UU uses the following files and directories:
.IP \fB/etc/domain\fR
This file lists the \*(UU domain.
It is read by
.BR mail .
.IP \fB/etc/uucpname\fR
Holds the name of your system, as it is known to other \*(UU sites.
.IP \fB/usr/bin/uucp\fR
The \fBuucp\fR command.
Copy a file to another system that runs \*(UU.
.IP \fB/usr/bin/uulog\fR
The \fBuulog\fR command.
.IP \fB/usr/bin/uuname\fR
The \fBuuname\fR command.
.IP \fB/usr/bin/uudecode\fR
The \fBuudecode\fR command.
.IP \fB/usr/bin/uuencode\fR
The \fBuuencode\fR command.
.IP \fB/usr/bin/uupick\fR
The
.B uupick
command.
.IP \fB/usr/bin/uuto\fr
The script
.BR uuto .
.IP \fB/usr/lib/uucp\fR
Contains \*(UU commands and system data files.
.IP \fB/usr/lib/uucp/dial\fR
This file tells
.B uucico
how to dial the modems on your system.
.IP \fB/usr/lib/uucp/port\fR
This file describes the devices that
.B uucico
uses to call each remote \*(UU site.
.IP \fB/usr/lib/uucp/sys\fR
This file describes the remote \*(UU sites that your site can call,
or that can call your site.
The command
.B uucico
(the command that actually talks with remote systems)
reads the information in this file to to connect to remote systems.
This file also names the directories on your system
into which a each remote site may read or write files;
and names the protocol or protocols that
.B uucico
uses to exchange files with a given remote system.
.IP \fB/usr/lib/uucp/uucico\fR
The \fBuucico\fR command.
.IP \fB/usr/lib/uucp/uuconv\fR
The
.B uuconv
command.
.IP \fB/usr/lib/uucp/uumkdir\fR
The
.B uumkdir
command.
.IP \fB/usr/lib/uucp/uumvlog\fR
The \fBuumvlog\fR command.
.IP \fB/usr/lib/uusched\fR
The script
.BR uusched .
.IP \fB/usr/lib/uucp/uutouch\fR
The \fBuutouch\fR command.
.IP \fB/usr/lib/uucp/uutry\fR
The \fBuutry\fR command.
.IP \fB/usr/lib/uucp/uuxqt\fR
The \fBuuxqt\fR command.
.IP \fB/usr/spool/logs/uucp\fR
Log of \*(UU activity.
.IP \fB/usr/spool/uucp/.Admin/audit.local\fR
.B uucico
stores logging information in this file
when you invoke it with with logging specified.
.IP \fB/usr/spool/uucp/.Admin/xferstats\fR
This file stores the transfer rates of files received or transmitted.
.IP \fB/usr/spool/uucp/.Log\fR
Directory containing \*(UU logfiles, as follows:
.DS
	\fB/usr/spool/uucp/.Log/uucico/\fIsitename\fR
	\fB/usr/spool/uucp/.Log/uux/\fIsitename\fR
	\fB/usr/spool/uucp/.Log/uucp/\fIsitename\fR
	\fB/usr/spool/uucp/.Log/uuxqt/\fIsitename\fR
.DE
.IP \fB/usr/spool/uucp/\fIsitename\fB/TM*\fR
.IS \fB/usr/spool/uucp/.temp/\fIsitename\fB/*\fR
These are temporary files that \fBuucico\fR generates when receiving files.
.IP \fB/usr/spool/uucp/\fIsitename\fB/C.*\fR
Files that instruct the local system either to send or to receive files.
.IP \fB/usr/spool/uucp/\fIsitename\fB/D.*\fR
Work files for outgoing and incoming files.
.II "UUCP^lock files"
.II "lck files"
.II "lock files"
.II "file locking, UUCP"
.IP \fB/usr/spool/uucp/LCK.*\fR
The ``lock'' files \*(UU uses to coordinate its resources.
When a \*(UU program attempts to access a remote site, it writes a ``lock''
file for that site.
This is to prevent \*(UU from accidentally attempting to access the same
site more than once simultaneously.
When the program that wrote the lock file exits successfully, it erases
its lock files, and so makes that site accessible to other \*(UU programs.
.IP \fB/usr/spool/uucp/.Sequence\fR
This directory contains the sequence number of the last file
handled by \*(UU.
.IP \fB/usr/spool/uucp/\fIsitename\fB/X.*\fR
Executed files.
These files will be executed by the command \fBuuxqt\fR, and are
generated by a remote system.
.IP \fB/usr/spool/uucppublic\fR
Public directory accessible by all remote \*(UU systems.
.SH "Attaching a Modem to Your Computer"
.PP
You can use \*(UU to network computers that are within the same office or
the same building.
It is far more common, though, to use uucp to connect computers that are
far apart via modem.
This tutorial assumes that you will be using uucp to exchange files via modem.
.PP
If you have not yet attached a modem to your computer, this section will give
you some useful hints.
It is straightforward to attach a modem to your computer, but you must be
careful.
.PP
First, read the documentation that comes with your modem, and look
for (1) the baud rate at which the modem operates, and (2) the command
protocol that your modem uses.
.PP
Second, check the plug on the back of your modem.
The modem will connect to your computer via a nine-pin or 25-pin D plug, also
known as an ``RS-232 interface''.
Such a plug can be either \fImale\fR or \fIfemale\fR:
the male plug has nine or 25 small pins projecting from it, whereas the
female does not.
.PP
Due to what can only be termed extreme stupidity, IBM AT and AT-compatible
computers use RS-232D plugs for both serial and parallel ports.
\fIBe sure to plug your modem into a serial port, not the parallel port, or you
can damage your computer and your modem!\fR
.PP
Third, obtain a cable to connect one of the serial ports on your computer
to the modem.
The serial ports on an IBM AT or AT-compatible computer are almost always male.
If your modem has a female plug, you will need a male-to-female cable, whereas
if your modem's plug is male (which is very rare), you will need a
female-to-female plug.
Be sure to purchase a standard modem cable for an IBM AT; practically
every computer dealer carries them.
The cable you purchase should support ``full modem control''; if
it does not say on the package, be sure to ask your dealer before you buy it.
If you are handy with a soldering iron you may be able to solder up such a
cable for yourself, but unless you know precisely
what you are doing it probably is not worth the trouble.
.PP
The Lexicon entry
.B RS-232
contains pinouts for both nine- and 25-pin connectors.
When you plug in your cable,
be sure to note whether you plugged it into port \fBcom1\fR,
\fBcom2\fP, \fBcom3\fP or \fBcom4\fP.
.PP
Fourth, reconfigure the serial port to suit your modem.
This involves the following steps:
.IP \fB1.\fR 0.3i
Log in as the superuser
.BR root .
.IP \fB2.\fR
Edit the file \fB/etc/ttys\fR.
This file normally has several lines in it, one that describes the console
and one for each serial port.
Each line has four fields:
a one-character field that indicates whether a login prompt should be
displayed (used only for devices from which people will be logging into
your system); a one-character field that describes whether the device
is local or remote (a local would be a modem from which you wished to dial
out, a remote device would be a modem from which someone could dial in);
a one-character field that describes the speed (or baud rate) at which
the device operates; and a field of indefinite length that names the
device being described.
.IP
If you have plugged into serial port \fBcom1\fR
a 9600-baud modem that will allow remote logins, edit
the line for \fBcom1\fR to read as follows:
.DM
	1rPcom1r
.DE
If you have plugged into serial port \fBcom2\fR a 2400-baud modem from
which you are only going to dial out, edit the line for \fBcom2\fR
to read as follows:
.DM
	0llcom2l
.DE
Note that the second and last character are a lower-case L, not a one.
For more information, see the Lexicon entries
for
.BR ttys .
.IP \fB3.\fR
Test if you have connected your modem.
Turn on your modem; then
log in as the superuser \fBroot\fR and type the following command:
.DM
	echo "foo" >/dev/com\fI?\fPl
.DE
where
.I ?
is the number of the port.
If you are addressing the correct port,
the lights on your modem should blink briefly.
For a more sophisticated test, try to communicate with your modem by using
the command
.BR ckermit .
If you are not familiar with
.BR ckermit ,
see its entry in the Lexicon for details.
.IP \fB4.\fR
When you have finished editing \fB/etc/ttys\fR, type the following command:
.DM
	kill quit 1
.DE
This forces \*(CO to read \fB/etc/ttys\fR and set up its ports in the
manner that you have configured them.
.PP
If you continue to have problems making connections with your modem,
see the volume \fIRS-232 Made Easy\fR, referenced above.
It describes in lavish detail how to connect all manner of devices via
the RS-232 interface.
also check the Lexicon articles \fBmodem\fR and \fBRS-232\fR for helpful
information.
.Sh "Selecting Site and Domain Names"
.PP
The first step to setting up \*(UU is to select a site name for your system.
You probably did this already when you installed \*(CO on your system,
because the \*(CO mail system does not work unless you have named your
system.
If, however, you have not yet named your system, you can do so by editing
the file
.BR /etc/uucpname .
The name you select must have eight characters or fewer, and must be
unique \(em or unique, at least, to the system into which you will log in.
Avoid names taken from popular culture, such as ``calvin,'' ``hobbes,''
or ``arnold'' \(em these have already been used many times.
See the Lexicon entry
.B uucpname
for more details.
.PP
Next, select a domain name for your system.
Again, you probably did this when you installed \*(CO.
A \fIdomain\fR is a set of \*(UU systems that together form one group with
a common name.
Even if you do not belong to a domain, you must set a domain for your system,
because
.B mail
expects it.
By convention, you can use your site name plus the suffix \fB.UUCP\fR to
create a domain.
The domain name is written into file
.BR /etc/domain .
See the Lexicon entry
.B domain
for details.
.PP
You must edit \fB/etc/uucpname\fR and \fB/etc/domain\fR to
install these names.
.SH "Set Up a UUCP Site by Hand"
.PP
This chapter walks you through the setting up of
a typical remote site, and explains what it's doing (and why) at each step
of the way.
We hope that when you have finished reading it, you will grasp at least
the principles of how \*(UU works.
.PP
Setting up \*(UU to call a remote system can be confusing and difficult,
mainly because there are many points at which this task can fail.
However, with patience and with the cooperation of the administrator of
the system that you will be contacting, you can accomplish this task.
Fortunately, once you have succeeded in exchanging files with another
system, your connection should work indefinitely without needing modification.
Fortunately, too, \*(UU is designed in such a way that you can reuse
system descriptions; so over time this process should become easier.
.PP
To set up \*(UU so it can contact another system, you must enter information
into files
.BR /usr/lib/uucp/sys ,
.BR /usr/lib/uucp/dial ,
and
.BR /usr/lib/uucp/port .
.B uucico
reads these files to learn how to communicate with a given remote system.
.PP
The rest of this section describes how to configure your system so it can
communicate with
.BR mwcbbs ,
Mark Williams Company's bulletin board.
A later section describes how to use \*(UU's commands and utilities
to work with a remote system once you have established communications.
.Sh "port:  Describe a Serial Port"
.PP
To call a given remote system, \*(UU must know about the devices
that it uses to communicate with that site.
File \fB/usr/lib/uucp/port\fR
describes ports that \fBuucico\fR can use.
.PP
Before you proceed any further, answer the following questions:
.IP \fB1\fR 0.3i
What serial device will be used for communications?
For example, the port \fB/dev/com2l\fR.
.IP \fB2\fR
At what speed will communication take place?
.IP \fB3\fR
What name will you give to this port?
.IP \fB4\fR
Does communication via this port involve a modem?
.IP \fB5\fR
If a modem is used, what dialer entry from \fB/usr/lib/uucp/dial\fR should
.B uucico
use?
.PP
With the above questions answered, let's put together our
.B port
entry.
.PP
A \fBport\fR entry begins with the lines that names the port.
Because \fBmwcbbs\fR is the system to be called for the purposes of this
example, let's call this port \fBMWCBBS\fR.
The port entry should look like this:
.DM
	port MWCBBS
	type modem
	device /dev/com2l
	baud 2400
	dialer hayes
.DE
Note that in the \fBdialer\fR line of this entry,
an underscore was used instead of a normal space character.
Do not use spaces in the
fields used to name the port, the actual device nor the dialer.
.PP
Note also the \fBtype\fR line of this port entry.
It says
.BR modem .
The only other valid value for this line is
.BR direct ,
which you would use should you be contacting a system via a wire that
runs directly from your serial port to the other system.
Because the goal here is to call
\fBmwcbbs\fR, you must specify
.BR modem .
.Sh "dial: Describe a Modem"
.PP
Because the port described above has a modem attached to it,
you must tell \fBuucico\fR how to talk to that modem.
The following example assumes that you will call
.B mwcbbs
via a 2400-bps, Hayes-compatible modem.
If you are not familiar with sending instructions to a modem,
stop here and find the manual for your modem,
or find someone familiar with modem communications before continuing.
.PP
To write the
.B dial
entry for your modem, you must answer the following questions.
Some of the questions may be unclear at first, but
don't worry \(em we'll get into the details and get you through this:
.IP \fB1\fR 0.3i
What will you name this dial entry?
For example, you could call your 2400-baud Hayes modem \fBhayes\fR.
.IP \fB2\fR
What command tells the modem to dial out?
Most Hayes-compatible modems use the command \fBATDT\fR (for Touch-Tone
telephone lines) or \fBATDP\fR (for pulse-dial lines).
.IP \fB3\fR
What message does the modem return when it connects to the remote
system's modem?
Most Hayes-compatible modems return the phrase \fBCONNECT 2400\fR).
.IP \fB4\fR
What messages does the modem send when it fails to connect to the remote
system's modem?
Example include \fBBUSY\fR,
\fBNO ANSWER\fR, \fBNO DIALTONE\fR, and \fBNO CARRIER\fR.
.IP \fB5\fR
What command tells the modem to hang up?
Most Hayes-compatible modems use the command \fBATH0\fR.
.IP \fB6\fR
How many seconds do you want to give the modem to make the connection
before \fBuucico\fR times out and gives up try to connect via this modem?
.PP
Some above information is optional,
but you should find all of it to write a thorough description of your modem.
.PP
An entry in
.B dial
begins with the line that names the modem.
In the earlier example for the
.B port
file, we decided to use the modem named \fBhayes\fR.
Therefore, the first line in our example is:
.DM
	dialer hayes
.DE
.II "chat script"
Our next step is to write a chat script for the modem.
At this point, a short discussion of chat scripts is in order.
A
.I "chat script"
gives the dialogue (or ``chat'')
that \*(UU has with a device or a remote system.
It consists of pairs of messages:
an
.I expect
message, which is a prompt that your system expects to receive from the
device or remote system, and a
.I response
message, which is what your system sends in response to it.
Spaces separate messages from each other;
therefore, you cannot use a space character within a message.
Instead, use the escape sequence \fB\es\fR to represent a space character.
If you want
.B uucico
to send a message
immediately, or to send a response message without waiting
for an expect message,
use an empty string \fB""\fR to represent the expect string.
.PP
The next line in our example give the chat script with which
.B uucico
dials your modem.
This is built from your answer to question 2, above.
Because a Hayes modem normally does not send a prompt to request a dial command,
use an empty string for the expect string.
The usual command to dial a Hayes compatible modem is \fBATDT\fR
(as described above), followed by the telephone number to dial.
The chat script for this dialogue is as follows:
.DM
	chat "" atdt\eD
.DE
Let's take a closer look at this chat script.
It begins with \fB""\fR, which tells
.B uucico
to expect nothing from the modem.
.B atdt\eD
is the message that
.B uucico
is to send to the modem.
.B atdt
is the dial command for a Touch-Tone telephone line, as noted above.
The escape sequence
.B \eD
represents the telephone number.
We use an escape sequence instead of the literal telephone number
because you can dial many different remote sites from the same modem.
.B uucico
finds the telephone number to dial from the entry for
.B mwcbbs
in the file
.BR sys ,
as will be described below.
.PP
Although this simple chat script will dial the desired telephone number,
it can be improved.
For example,
.B uucico
may fail to connect to the remote system for any number of reasons.
The more information
we can get back from the modem, the easier it will be to debug any problems
we have in connecting to the remote system.
If the modem is set up to return verbal result codes,
.B uucico
can check for these codes to determine if a connection to a remote modem
succeeded.
(For more information on \fIverbal result codes\fR,
see the manual that came with your modem.)
The following adds these features to our chat script for a Hayes-compatible
modem:
.DM
	chat "" ATQ0E1V1L2M1DT\eD CONNECT\es2400
.DE
Look at this chat script carefully.
Again,
.B uucico
expects nothing and sends a command to make the modem dial out.
The command
.B ATQ0E1V1L2M1DT\eD
has a lot of information packed into it.
It is written mostly in Hayes-ese,
so we'll break it up and show you what each portion means:
.IP \fBAT\fR 0.3i
Attention:
tell the modem that the following is a command.
.IP \fBQ0\fR
Return result codes.
.IP \fBE1\fR
Echo commands sent to it.
(You can tell
.B uucico
to log what the modem returns, so you can see exactly what it is doing.)
.IP \fBV1\fR
Use the long form of result codes.
.IP \fBL2\fR
Set the loudness of the dial tone to medium.
.IP \fBM1\fR
Turn on the speaker on the modem.
Sometimes it is helpful to hear when the modem dials out.
.IP \fBDT\fR
Dial Touch-Tone.
.IP \fB\eD\fR
The \*(UU escape sequence that represents the telephone number, as described
above.
.PP
Now, look at the end of this chat script:
we have added a second expect message.
This tells
.B uucico
that after it has dialed the telephone number for this site,
it should expect the string \fBCONNECT 2400\fR.
When
.B uucico
sees this message, it assumes that a modem connection
was successfully established and that it can continue normally.
If
.B uucico
does not see this message within a given period of time
(as will be described below) or if it receives different message,
it assumes that the connection attempt failed.
When this occurs,
.B uucico
aborts the connection attempt.
.PP
Note that, as mentioned earlier, you must use the escape sequence \fB\es\fR
to represent the space character in the phrase \fBCONNECT 2400\fR.
.PP
To review, the dial entry for dialer
.B hayes
now looks like this:
.DM
	dialer hayes
	chat "" ATQ0E1V1L2M1DT\eD CONNECT\es2400
.DE
The next line, \fBchat-timeout\fR, gives the number of seconds that
.B uucico
should wait before it quits trying to connect to a remote system.
The default is 40 seconds.
You may or may not prefer to change this value;
but for the purpose of this example,
let's assume that you want
.B uucico
to wait 60 seconds before it times out.
The
.B dial
entry for
.B hayes
now looks like this:
.DM
	dialer hayes
	chat "" ATQ0E1V1L2M1DT\eD CONNECT\es2400
	chat-timeout 60
.DE
Recall that our chat script for this modem turned on verbal result codes.
Now, it is time to take advantage of this.
Hayes compatible modems, will, in general,
return any number of messages if a connection fails, depending upon the cause
of the failure.
The entry
.B chat-fail
defines a string that the modem returns when it has failed to connect.
When
.B uucico
receives the string, it realizes that the attempt to connect has failed,
and quits.
The description for a modem can include any number of
.B chat-fail
entries.
Let's add some messages
that a typical Hayes-compatible modem might return
when it fails to connect to a remote modem.
The
.B dial
entry for the modem named
.B hayes
now looks like:
.DM
	dialer hayes
	chat "" ATQ0E1V1L2M1DT\eD CONNECT\es2400
	chat-timeout 60
	chat-fail BUSY
	chat-fail NO\esCARRIER
	chat-fail NO\esANSWER
.DE
Modem-result codes offer many advantages.
By looking for specific messages,
.B uucico
knows immediately when the modem cannot connect to a remote site, and
quits immediately rather than waiting to time out.
More important,
.BR uucico 's
log files will show why the connection failed,
which eases the debugging of connection problems.
.PP
We can add yet another safeguard to this dialer entry.
By naming \fBabort-chat\fR and \fBcomplete-chat\fR scripts,
we can tell
.B uucico
to chat again with the modem after a communication session has ended.
These scripts can, for example, ensure that the modem hangs up the
telephone and turns off error messages and echoing.
If the port for this modem normally is enabled,
then it is vital that you turn off echoing and error messages,
to prevent the modem and your \*(CO getting into an infinite
dialogue when the modem echoes \*(CO's login prompt.
For details on this problem, see the Lexicon entry for \fBmodem\fR.
The following adds
\fBabort-chat\fR and \fBcomplete-chat\fR scripts to our description of
.BR hayes :
.DM
	dialer hayes
	chat "" ATQ0E1V1L2M1DT\eD CONNECT\es2400
	chat-timeout 60
	chat-fail BUSY
	chat-fail NO\esCARRIER
	chat-fail NO\esANSWER
	complete-chat "" +++ OK ATH0E0V0Q1
	abort-chat "" +++ OK ATH0E0V0Q1
.DE
Notice that the two chat scripts are identical.
.B uucico
runs this script whenever a session ends \(em
regardless of whether it failed or completed successfully.
As with the
.B chat
script, the first entry is an empty string, to indicate that
.B uucico
should expect nothing before it sends its first response message \(em
in this case the string \fB+++\fR, which in Hayes-ese is the escape
sequence that puts the modem into command mode.
The modem replies with the message \fIOK\fR.
When
.B uucico
sees \fBOK\fR, it sends the reply
.BR ATH0E0V0Q1 ,
which is the Hayes command sequence to
(1) hang up the telephone,
(2) turn off echoing,
(3) turn off verbose error messages, and
(4) turn off error messages altogether.
.PP
Please note that the last six lines of this dialer entry are optional.
The only required lines are the first two.
.Sh "sys: Individual System Configuration"
.PP
Having completed the
.B port
and
.B dial
entries for calling
.BR mwcbbs ,
the last \(em and most difficult \(em step
is to describe
.B mwcbbs
to
.BR uucico .
This is done by writing an entry in file \fB/usr/lib/uucp/sys\fR.
The entry does the following:
.IP \(bu 0.3i
Names the remote system.
.IS \(bu
Specifies valid times to call the system.
.IS \(bu
Specifies telephone number to use when calling the system.
.IS \(bu
Specifies valid protocols to use to exchange files with the system.
.IS \(bu
Restricts read and write access for the remote system.
.PP
Before we continue,
please note that the following example is typical of an entry in
.BR sys ,
but it is by no means exhaustive.
Please refer to the Lexicon article \fBsys\fR for more information.
.PP
Now, be prepared to answer the following questions:
.IP \fB1\fR 0.3i
What is the name of the remote system?
.IP \fB2\fR
When is it legal for
.B uucico
to telephone the remote system?
.IP \fB3\fR
At what speed will communications take place?
.IP \fB4\fR
On which port will the call to the remote system be made?
.IP \fB5\fR
What telephone number, if any, must be dialed?
.IP \fB6\fR
What is the ``chat sequence'' (chat script) to be used to log into the system?
.IP \fB7\fR
What protocol should be used to transfer files?
.IP \fB8\fR
How should your system identify itself to the remote system?
.PP
An entry in 
.B sys
must begin with the line
\fBsystem \fIsysname\fR, where
.I sysname
is the name of the remote system.
This both names the remote system and tells
.B uucico
that a new entry is beginning.
Make this entry into \fBsys\fR:
.DM
	system mwcbbs
.DE
The next line of the entry, \fBtime\fR, restricts when the
.B uucico
may call the remote system.
You can restrict calls to a given day of the week
by using the following abbreviations:
.DS
	\fBWk\fR	Every weekday (Monday through Friday)
	\fBSu\fR	Sunday
	\fBMo\fR	Monday
	\fBTu\fR	Tuesday
	\fBWe\fR	Wednesday
	\fBTh\fR	Thursday
	\fBFr\fR	Friday
	\fBSa\fR	Saturday
.DE
You can restrict the time of contact;
all time notation must be in military time.
The following give some examples of day/time entries:
.DM
.ta 0.5i 2.0i
	\fB0100-0200\fR	Valid to call every day between 1 and 2 AM
	\fBMo0100-0200\fR	Valid to call every Monday between 1 and 2 AM
	\fBSa\fR	Valid to call at any time on every Saturday
	\fBAny\fR	Valid to call at any time
	\fBNever\fR	Never call the system
.DE
To restrict the valid times for calling
.B mwcbbs
to Saturday and Sunday nights from 10 to 11:15 PM, add the line:
.DM
	time SaSu2200-2315
.DE
Our \fBsys\fR entry now looks like this:
.DM
	system mwcbbs
	time SaSu2200-2315
.DE
The next line of the \fBsys\fR entry, \fBbaud\fR, is self-explanatory and is
answered by the third question above.
For this example, we will assume a speed of 2400 bps.
The
.B sys
entry now look like:
.DM
	system mwcbbs
	time SaSu2200-2315
	baud 2400
.DE
The next line, \fBport\fR, names the port that
.B uucico
is to use to telephone the remote system.
This must name a port that is defined in file
.BR port ;
in this case we'll use port
.BR MWCBBS ,
which we wrote earlier:
.DM
	port MWCBBS
.DE
The next line, \fBphone\fR, is answered by question 5, above; this, too, is
self-explanatory.
Add the following line to our
.B sys
entry:
.DM
	phone 17085590412
.DE
.B uucico
uses this telephone number to expand the escape sequence \fB\eD\fR, used in the
.B dial
entry defined above.
.II "mwcbbs^telephone numbers"
Note, by the way, that you can access
.B mwcbbs
through the following three telephone numbers:
.DS
.ta 0.5i 3.5i
	1200/2400-baud generic modem:	708-559-0412
	9600-baud Trailblazer modem:	708-559-0445
	9600-baud V.32 or HST modem:	708-559-0452
.DE
.PP
This example assumes that you have a generic 2400-baud modem; but you should
select the number that best suits your modem.
.PP
The next line, \fBchat\fR, is the chat script that
.B uucico
uses to log into the remote system.
Please refer to the previous section about the \fBdial\fR file for
a brief description of what a chat script is and how it is laid out;
or see the Lexicon entry for
.BR sys :
.DM
	chat "" \en in:--in: nuucp word: public word: \fIserialnum\fP
.DE
Please note that the string
.I serialnum
represents the serial number of your \*(CO system.
You must use this number to log into system
.BR mwcbbs .
.PP
Let's take a moment to review this line.
By the time
.B uucico
needs the information from this line,
it has already successfully dialed into and connected to one of the modems on
.BR mwcbbs .
Now it must log into the system.
The chat script tells
.B uucico
to expect nothing and to immediately send out a newline,
as represented by the escape sequence \fB\en\fR.
.PP
After
.B uucico
sends the newline character, it waits for
.BR in: ,
i.e., the final characters in the
.B mwcbbs
login prompt
.BR "mwcbbs 386 login:" .
It is not necessary to expect the entire login prompt, just enough of it to let
.B uucico
know that it can send the next message \(em in this case, your system's
login identifier.
If
.B uucico
does not receive
.BR in: ,
it sends another newline character 
and again waits for
.BR in: .
(\fB--in:\fR is equivilant to \fB-\en-in:\fR).
.PP
Once it receives
.BR in: ,
.B uucico
sends
.BR nuucp ,
which is the login name that
.B mwcbbs
expects to receive from your system.
.B uucico
now expects the message
.B word:
which is the is the tail of the prompt
.B mwcbbs
really transmits,
.BR password: .
When
.B uucico
sees
.BR word: ,
it sends 
.BR public ,
which is the password that
.B mwcbbs
expects.
Once
.B mwcbbs
receives the password it expects, it sends out a prompt for a
remote-access password \(em hence, this chat script again expects to see
.B word:
again.
When it receives this second password prompt, this chat script tells
.B uucico
to send your system's serial number.
This completes the chat script.
.PP
The
.B sys
entry for
.B mwcbbs
now looks like this:
.DM
	system mwcbbs
	time SaSu2200-2315
	baud 2400
	port MWCBBS
	phone 17085590412
	chat "" \en in:--in: nuucp word: public word: \fIserialnum\fR
.DE
Now, we will add the
.B protocol
line to this entry.
This tells
.B uucico
which protocol to use when exchanging files with the remote system.
Different implementations of \*(UU use various protocols for exchanging files.
The
.B g
protocol was the first \*(UU protocol to be invented, and is still
the most commonly used.
.II "Chesson, Greg"
(By the way, this protocol is named after its designer, Greg Chesson.)
Since then, other protocols have been invented.
Each protocol has its strengths and weaknesses; you should weigh carefully
how you are communicating with the remote system and what protocols the
remote system, supports before you select a protocol.
Please refer to the Lexicon article
\fBsys\fR for a complete list of the protocols that the Taylor \*(UU
package recognizes, and for information on when each should be used.
.PP
Please note, too, that the
.B g
protocol is not implemented uniformly by all versions of \*(UU.
For instance, some
.B g
protocols can only send data in packets of 64 bytes, using
three \fIsliding windows\fR.
Other implementations can support up to 4,096 bytes per packet,
using seven sliding windows.
The more windows supported,
the greater that transfer rate of data.
The larger the packet size per window, the greater the transfer rate of data.
For a detailed discussion of the internals of \*(UU protocols,
please refer to third-party publications
or to documentation for the Taylor \*(UU system
in the file
.BR /usr/src/alien/uudoc.tar.Z .
Taylor \*(UU can be configured from as
little as three windows and 64 bytes per packet,
to seven windows and 4,096 bytes per packet.
You can set these parameters in the \fBsys\fR file;
for details, see the Lexicon entry for
.BR sys .
In this example, we will use the default values of seven windows and 64-byte
packets.
.PP
After all of this discussion, add the following line to the
.B sys
entry we are building:
.DM
	protocol g
.DE
For the next step,
you must select the name by which your system identifies itself to
.BR mwcbbs ,
asked in question 8, above.
``But wait!'' you say.
``Didn't I answer this in the chat script that we labored over just earlier?
We identified ourselves as `nuucp'.''
Well, not exactly, as we will make clear.
.PP
The login name of
.B nuucp
is not the same as the site name with which we wish to identify ourselves.
Once our system has logged into the remote system (in this case,
.BR mwcbbs ),
the remote system fires up
.B uucico
on its end.
Once again our system must identify itself \(em but this time to
.BR uucico ,
not to the login program.
.PP
Think of the login sequence this way:
as a normal user must log in giving his
name and a password or two just to run a shell, so to must
.B uucico
log into
.BR mwcbbs .
It logs in with the name
.B nuucp
and gives the necessary passwords;
however,
.BR mwcbbs ,
instead of invoking a shell for user
.BR nuucp ,
invokes
.BR uucico
instead.
(If you look in the file
.BR /etc/passwd ,
you'll see that the last entry for each user is the program that the system
runs for that user; usually it is a shell, but sometimes it's another program,
e.g.,
.BR uucico ).
Now,
.B uucico
on
.B mwcbbs
begins to talk with
.B uucico
on your system.
The real work of transfering files can begin, as long as
.B mwcbbs
recognizes the sitename with which your system identifies itself.
.PP
.B mwcbbs
does not recognize many remote systems,
yet it receives calls from more than 100 different systems per day.
How does
.B mwcbbs
handle all of these calls if it really only knows a few dozen systems?
Most systems identify themselves by the same name,
.BR bbsuser .
.B mwcbbs
is set up to always recognize the remote site
.BR bbsuser .
.PP
Where does this name come from?
In normal circumstances, this name is read from the file
.BR /etc/uucpname .
This file holds the name you gave your system when you installed \*(CO;
you were required to select a name because electronic mail will
not work without it.
(For more information on this file, see its entry in the Lexicon.)
The odds are that you did not choose the name
.BR bbsuser .
Let's say that when you installed \*(CO onto your system, you chose the name
.BR foobar .
Now, when you system calls
.BR mwcbbs ,
by default it will identify itself to
.BR mwcbbs 's
edition of
.B uucico 
as
.BR foobar .
However,
.B mwcbbs
doesn't know
.B foobar
from Adam, so it logs your system off and hangs up the telephone.
.PP
To get around this, you must insert the line
.B myname
into the
.B sys
entry for
.BR mwcbbs .
Add this line to the sys entry we are building:
.DM
	myname bbsuser
.DE
Now, whenever your system calls
.BR mwcbbs ,
it will identify itself to
.BR mwcbbs 's
.B uucico
as
.BR bbsuser .
.PP
At this point, we could fill dozens of pages with
discussions of the items you can configure in
.BR sys .
However,  we now have all of the information we need to call
.BR mwcbbs .
For a fuller discussion of
.BR sys ,
look up its entry in the Lexicon.
.PP
For now, the completed
.B sys
entry for
.B mwcbbs
is as follows:
.DM
	system mwcbbs
	time SaSu2200-2315
	baud 2400
	port MWCBBS
	phone 17085590412
	chat "" \en in:--in: nuucp word: public word: \fIserialnum\fP
	protocol g
	myname bbsuser
.DE
There are a few other items that you will find yourself configuring as part
of a typical entry in
.B sys .
.PP
First, you must indicate whether the remote system and request files from
your system, and transfer files into it.
You may wish to deny this capacity to some remote systems, but you do want
to grant it to
.BR mwcbbs ,
as the whole point of access that system is to have it download files to you.
So, add the following two lines to the bottom of the entry for
.BR mwcbbs :
.DM
	request yes
	transfer yes
.DE
Next, you must name the directories on your system
from which the remote system can request files,
and the directories on the remote system onto which
your system has permission to write files.
By default, remote systems are limited to requesting files from directory
.B /usr/spool/uucppublic
and its subdirectories.
To change this default, add the instructions
.B remote-send
and
.B remote-receive
to the entry in
.B sys
for the remote site.
For example, suppose you decided to let
.B mwcbbs
read the directory
.BR /usr/private ,
but not
.BR /usr/private/myfiles .
You also decided to let
.B mwcbbs
write files into
.BR /tmp ,
but not into directory
.BR /tmp/secret .
To do this, add the following instructions to the
.B sys
entry for
.BR mwcbbs :
.DM
	remote-send /usr/private !/usr/private/myfiles
	remote-receive /tmp !/tmp/secret
.DE
Naming a directory in
.B remote-send
or
.B remote-receive
lets the remote system, respectively, read from that directory
and all of its sub-directories.
However, if you prefix a directory name with an exclamation point `!',
that directory and its subdirectories are specifically excluded from being
accessed by the remote system.
The following gives the normal settings for these directories:
.DM
	remote-send /usr/spool/uucppublic /tmp
	remote-receive /usr/spool/uucppublic /tmp
.DE
Next, you must name the directories from which your system can send files
to the remote system, and into which your system can write the files that
you have requested from the remote system.
These are named by, respectively, the instructions
.B local-send
and
.B local-receive .
The following gives the normal settings for these directories:
.DM
	local-send /usr/spool/uucppublic /tmp
	local-receive /usr/spool/uucppublic /tmp
.DE
If you are confused about how these instructions differ from the
instructions
.B remote-send
and
.BR remote-receive ,
just remember that the
.B remote
instructions name directories for send/receive requests initiated by
the remote system; where as the
.B local
instructions name directories for send/receive requests initiated by
your system.
.PP
One last entry needs to go into this file:
you need to name the commands that
.B mwcbbs
can execute on your system.
It needs to execute at least the commands
.B rmail
and
.B uucp
so that it can send you mail and upload files to your system.
So, add the following line to the end of the entry for
.BR mwcbbs :
.DM
	commands rmail uucp
.DE
With these lines, our
.B sys
entry for
.B mwcbbs
is complete.
It looks like this:
.DM
	system mwcbbs
	time SaSu2200-2315
	baud 2400
	port MWCBBS
	phone 17085590412
	chat "" \en in:--in: nuucp word: public word: \fIserialnum\fP
	protocol g
	myname bbsuser
	request yes
	transfer yes
	remote-send /usr/spool/uucppublic /tmp
	remote-receive /usr/spool/uucppublic /tmp
	commands rmail uucp
.DE
This is a typical
.B sys
entry for calling a remote system.
.SH "Simplifying a UUCP Configuration With uuinstall"
.PP
The program
.B /usr/bin/uuinstall
has been included to help build, modify, or delete entries in the files
.BR sys ,
.BR port ,
and
.BR dial ,
making configuring \*(UU easier.
.B uuinstall
uses a system of screens and windows to gather and organize the information
\*(UU needs to work with a remote system.
While
.B uuinstall
does not remove all of the pain from setting up communications with a
remote site, it does make this (admittedly complex) task easier.
.PP
This section shows how
.B uuinstall
can simplify the process of configuring \*(UU to communicate with
.BR mwcbbs .
.PP
If you have not read the previous section of this chapter, regarding
configuring \*(UU to call \fBmwcbbs\fR\fI, please do so now.
That section discusses in detail many topics that will not be repeated here.
In particular, look at the questions asked in the previous section
regarding configuring the files
.BR port,
.BR dial ,
and
.BR sys ,
and be prepared to answer them in this section.
.Sh "Invoking uuinstall"
.PP
To invoke
.BR uuinstall ,
just type
.B uuinstall
at the shell prompt.
.PP
For security reasons, only the superuser
.B root
and user
.B uucp
can invoke this program.
If other users could access this program,
they would have access to the information necessary
to log into the remote systems listed in
.BR sys .
Only priviledged users should have access to this information.
.PP
When you invoke
.BR uuinstall ,
the following screen appears:
.B1
.sp
                     UUCP Configuration: Main menu
.sp 3
	<s>ys file      Configuration information for specific systems
	<p>ort file     Information for individual ports
	<d>ial file     Configuration information for dialing modems
.sp 3
         Press the letter corresponding to the file you wish to examine
                            or <q> to quit
.sp
.B2
.Sh "The Port File"
.PP
The menu says it all:
choose the file to work with.
For the sake of simplicity, we will modify the system files in the same
order that we did in the extended example that appeared in the previous
section; so, press
.B p
to select the file
.BR port .
.PP
When you make your selection,
.B uuinstall
displays its \fBaction menu\fR:
.B1
.sp
	                     Action menu
.sp
	     You have selected to work with the port file.
.sp
	                Do you wish to:
	                        <a>dd an entry
	                        <d>elete an existing entry
	                        <m>odify an existing entry
	                        <v>iew an existing entry
.sp
	Press the letter corresponding to the action you wish to perform
	            or press <RETURN> for the main menu.
.sp 2
.B2
.PP
Press
.B a
to add an entry.
The following screen appears:
.B1
.sp
	         Port File Entry Screen
.sp 2
	        port   [_             ]
	        type   [     ]
	        device [/dev/             ]
	        baud   [     ]
	        dialer [              ]
.sp 2
	Enter the name that you want associated with
	   the device that this entry will define.
	          Enter nothing to cancel.
.sp 2
.B2
.PP
It is time to enter our information.
To do so, type the appropriate information into each field on the screen.
When you have entered all of the information required by that field, press
\*(RT;
the cursor drops to the next field on the screen.
.PP
When the cursor enters selected fields,
.B uuinstall
displays a help message to describe the information that you must enter.
The message will, in many cases, give an example of valid data.
.PP
If you wish not to enter anything into a given field, type \*(RT
when the cursor enters that field.
Some fields are
.IR required :
that is, the system demands that you type something into the field;
in this case, the cursor will not move until you have entered something.
After you have completed the last line,
.B uuinstall
prompts you to ask if it should add the new entry to file
.BR port .
.PP
From here, configuring a
.B port
entry is simple.
Look at the questions that we asked in the previous section
about the information that a
.B port
entry needs.
Earlier, we decided to name this port \fBMWCBBS\fR; therefore, type
.B MWCBBS
into the first field on the screen \(em the one labelled
.BR port .
Press \*(RT;
the cursor jumps to the next field, which is labelled
.BR type .
.PP
The port can be either of two types:
.B direct
or
.BR modem .
Since we're using a modem to telephone
.BR mwcbbs ,
type
.B m
into the field labelled
.BR type .
You would type
.B d
(for \fIdirect\fR) should the remote system be connected to yours via a
line that runs directly from your serial port to the remote system.
The cursor jumps to the next field, which is labelled
.BR device ,
and positions itself just to the right of the string
.BR /dev/ .
.PP
Now, type the name of the device associated with this port.
Earlier, we chose
.BR /dev/com2l .
Because
.B /dev/
is already in place, just type
.BR com2l .
Again, press \*(RT;
the cursor jumps to the field labelled
.BR baud .
.PP
Type the speed that communications will take place at:
in this example,
.BR 2400 ,
then press \*(RT.
The cursor jumps to the field labelled
.BR dialer .
.PP
Finally, type the name of the dialer script that
.B uucico
is to use to talk with the modem plugged into this port.
As you recall, our example uses the script named
.BR hayes ;
so type that into this field and press \*(RT.
.PP
The completed entry, as we typed it here, looks like this:
.B1
.sp
	       Port File Entry Screen
.sp 2
	Do you wish to write this entry? (y/n)_
.sp
	        port   [MWCBBS]
	        type   [modem]
	        device [/dev/com2l]
	        baud   [2400]
	        dialer [hayes     ]
.sp 2
.B2
.PP
.B uuinstall
now asks if you wish to write this entry into file
.BR /usr/lib/uucp/port .
Type
.BR y ,
and press \*(RT.
.B uuinstall
saves the information and returns to its main menu.
.Sh "The Dial File"
.PP
From the main menu, press
.B d
to select the file
.BR dial .
This will take us to the action menu.
Again, select
.B a
to add an entry.
.B uuinstall
then displays the following screen:
.B1
.sp 2
	                 Dial File Entry Screen
.sp
	dialer:        [_             ]
	chat:          [                                             ]
	chat-timeout:  [   ]
	chat-fail:     [                         ]
	chat-fail:     [                         ]
	chat-fail:     [                         ]
	complete-chat: [                                             ]
	abort-chat:    [                                             ]
.sp 2
	Enter the name of the dialer that
	this entry describes.
	Leaving this field blank aborts entry.
.sp 2
.B2
.PP
Begin by typing the name of the dialer script.
In the previous example, we named it
.BR hayes .
Type this in the first line; then press \*(RT.
The cursor jumps to the next field, which is labelled
.BR chat .
.PP
Now, type the chat script that
.B uucico
will use to dial the modem.
Please refer to the previous section for a discussion of what a chat
script is, and of the elements that the dialer chat should contain.
In the previous section, we decided to use the following script:
.DM
	"" ATE0Q1V1L1M0DT\eD CONNECT\es2400
.DE
Type this, then press \*(RT.
The cursor jumps to the next field, which is labelled
.BR chat-timeout .
.PP
Type the number of seconds that 
.B uucico
should wait before it aborts its attempt to dial out on the modem.
Press \*(RT;
the cursor jumps to first field labelled
.BR chat-fail .
.PP
Into the next three fields,
enter messages that the modem might return when an attempt to connect to
a remote system fails.
In the previous section, we selected the messages
.BR NO\esCARRIER ,
.BR BUSY ,
and
.BR NO\esDIALTONE .
Type the first message, then press \*(RT.
The cursor jumps to the second
.B chat-fail
field; type the second message and press \*(RT again.
The cursor jumps to the third
.B chat-fail
field; type the third message and press \*(RT once again.
The cursor jumps to the field labelled
.BR complete-chat .
.PP
Note that
.B uuinstall
gives you space to enter three
.B chat-fail
messages.
You can, however, enter an indefinite number of these messages
into a dialer script.
If you wish to enter more than three
.B chat-fail
messages, you must edit the file
.BR /usr/lib/uucp/dial
by hand.
.PP
The next two fields, respectively labelled
.B complete-chat
and
.BR abort-chat ,
let you enter the chat scripts that
.B uucico
should execute when, respectively,
a call completes successfully or fails for some reason.
As we noted in the previous section, these scripts are optional; however,
you are well advised to enter them, to ensure that the modem is returned to
its correct state after a call is completed.
In the previous section, we devised the following script for our
Hayes-compatible modem:
.DM
	"" +++ OK ATH0E0V0Q1
.DE
Type that script into the field labelled
.BR complete-chat ,
then press \*(RT.
Type again type it into the field labelled
.BR abort-chat ,
and press \*(RT again.
.PP
The completed dial entry screen should now look like this:
.B1
.sp 2
	           Dial File Entry Screen
.sp
	dialer:        [hayes     ]
	chat:          ["" ATE0Q1V1L1M0DT\eD CONNECT\es2400]
	chat-timeout:  [60]
	chat-fail:     [BUSY]
	chat-fail:     [NO\esDIALTONE]
	chat-fail:     [NO\esCARRIER]
	complete-chat: ["" +++ OK ATH0E0V0Q1]
	abort-chat:    ["" +++ OK ATH0E0V0Q1]
.sp 2
	     Do you wish to save this entry? (y/n)
.sp 2
.B2
.PP
If you are comfortable with your entry as it is, then press
.B y
to write it into
.BR /usr/lib/uucp/dial .
If not, press
.BR n .
In either case,
.B uuinstall
returns to its main menu.
.Sh "The sys File"
.PP
It is now time to describe a remote system, in this case
.BR mwcbbs ,
to this system.
From the main menu, press
.B s
to select the
.B sys
file, then select
.B a
from the action menu to add an entry to the sys file.
The following screen will appear:
.B1
.sp 2
	                      Sys File Entry Screen
.sp 2
	system:     [_       ]
	time:       [                                             ]
	speed:      [     ]
	port:       [             ] 
	phone:      [                                             ]
	chat:       [                                                       ]
	myname:     [        ]
	protocol:   [   ]
	commands:   [                                                       ]
	read-path:  [                                                       ]
	write-path: [                                                       ]
.sp 2
	Enter the name of a remote uucp system. You should
	limit the name to 8 characters.
.sp
	Leaving this field blank aborts entry.
.sp 2
.B2
.PP
Because we are using the remote system
.B mwcbbs
as our example, type
.B mwcbbs
into the field labelled
.BR system ,
then press \*(RT;
the cursor jumps to the field labelled
.BR time .
.PP
For this field, let's get a little creative.
Let's limit calling
.B mwcbbs
to after 6 PM and before 6 AM on weekdays, but permit any time on weekends.
Type the following line in the
.B time
field:
.DM
	Sa,Su,Wk1800-0600
.DE
Press \*(RT,
which moves the cursor to the field labelled
.BR speed .
.PP
Now enter the speed or (``baud rate'') at which communications will occur.
Earlier, we selected 2400 bps; so type
.BR 2400
and then \*(RT.
.PP
The cursor is now in the field labelled
.BR port .
Type the name of the port via which we will call
.BR mwcbbs .
As noted above, we will be using the port that we named
.BR MWCBBS ;
so type that into this field and then press \*(RT.
.PP
The cursor is now in the field labelled
.BR phone ,
which holds the telephone number for the remote system.
To find the telephone number for
.BR mwcbbs ,
check the release notes that came with your copy of \*(CO; then
type it into this field and press \*(RT.
The cursor jumps into the field
labelled
.BR chat ,
for the chat script.
.PP
We discussed the chat script in some detail in the previous section.
Enter the following chat script in this field:
.DM
	"" \en in:--in: nuucp word: public word: \fIserialno\fP
.DE
Remember to replace
.I serialno
with the serial number provided with your \*(CO package.
When you have finished typing the chat script, press \*(RT.
this moves the cursor to the field labelled
.BR myname .
.PP
Because
.B mwcbbs
does not grant access to just any system,
your system must identify itself as a system that
.B mwcbbs
already knows about.
.B mwcbbs
grants access to every system that identifies itself as
.BR bbsuser ;
so type
.B bbsuser
into this field, and press \*(RT.
The cursor moves into the field labelled 
.BR protocol .
.PP
This field holds the protocol with which your system will exchange files with
.BR mwcbbs .
The Taylor \*(UU package supports several protocols.
.B mwcbbs
in turn recognizes protocols
.BR a ,
.BR g ,
and
.BR i .
Please refer to the Lexicon article
.B sys
for more information on available protocols, and the strengths and weaknesses
of each.
For the purposes of this example, type
.B g
(for the
.B g
protocol), then press \*(RT.
.PP
The cursor is now in the field labelled
.BR commands ,
which lists the commands that the remote system may execute on your system.
Because
.B mwcbbs
will not be calling you, just press \*(RT.
.B uuinstall
will write a default list of commands into this field, then
move the cursor to the next field.
.PP
The last two fields, which respectively are labelled
.B read-path
and
.BR write-path ,
limit the directories that the remote system can, respectively, write into
or read from.
They point to the
.B remote-send
and
.B remote-receive
instructions within a
.B sys
entry.
Press \*(RT for each field;
.B uuinstall
will write into each field the default directory, which is
.BR /usr/spool/uucppublic .
.PP
With the template completed, your entry should look like this:
.B1
.sp 2
	                      Sys File Entry Screen
.sp 2
	system:     [mwcbbs]
	time:       [Sa,Su,Wk1800-0600]
	speed:      [2400]
	port:       [MWCBBS]
	phone:      [17085590412]
	chat:       ["" \en in:--in: nuucp word: public word: \fIserialno\fP]
	myname:     [bbsuser]
	protocol:   [g]
	commands:   [rmail rnews uucp uux]
	read-path:  [/usr/spool/uucppublic]
	write-path: [/usr/spool/uucppublic]
.sp 2
	           Do you wish to save this entry? (y/n)
.sp 2
.B2
.PP
If you are comfortable with the information you entered, press
.B y
to have it written to the file
.BR /usr/lib/uucp/sys .
.PP
This concludes our examples of configuring \*(UU to call remote systems.
Due to the extreme flexibility of the Taylor \*(UU package, it is not feasible to
review all possible configurations available to you.
Each of the configuration files,
.BR /usr/lib/uucp/sys ,
.BR /usr/lib/uucp/port ,
and
.BR /usr/lib/uucp/dial ,
can include more commands than are reviewed here.
Many of these are reviewed in the Lexicon entries for each of the files.
A complete set of ASCII text Taylor \*(UU documentation,
as provided with the distribution available from
several internet sites, is included in the file
.BR /usr/src/alien/uudoc104.tar.Z .
The complete source code for the Taylor \*(UU package as distributed with
\*(CO is available from
.BR mwcbbs .
.Sh "Modifying an Existing Entry"
.PP
The above examples show how to use
.B uuinstall
to enter a new entry into files
.BR port ,
.BR dial ,
and
.BR sys .
You can also use
.B uuinstall
to delete, view, or modify existing entries in these files.
Of these, the most useful is the feature for modifying an existing entry.
.PP
Let's say that we want to modify our entry for dialer
.B MWCBBS
to include a fourth
.BR chat-fail
instruction, for the modem message
.BR ERROR .
To do so, do the following:
.IP \(bu 0.3i
Invoke
.B uuinstall
from the shell, as described above.
.IP \(bu
When
.B uuinstall
displays its main menu, type
.BR d ,
to edit the file
.BR /usr/lib/uucp/dialer .
.IP \(bu
When
.B uuinstall
displays its action screen, type
.BR m ,
to modify this file.
.IP \(bu
.B uuinstall
then displays the names of all of the entries in this file.
Use the arrow keys on your terminal to move the cursor to the entry
that you wish to enter, in this case
.BR MWCBBS ;
then press \*(RT.
.IP \(bu
.B uuinstall
extracts the entry for
.B MWCBBS
from
.BR /usr/lib/uucp/dial ,
writes it into a temporary file, then invokes the \*(ME editor
for that temporary file.
You can use the usual \*(ME commands to edit this entry.
In this case, add the line
.DM
	chat-fail ERROR
.DE
into the entry.
.IP \(bu
When you have finished editing the entry, type
.BR <ctrl-Z> .
.IP \(bu
.B uuinstall
will prompt you and ask if you wish to save your changes.
Type
.B y
if you do,
.B n
if you do not.
.PP
Note that if you do save your changes, you can always use
.B uuinstall
to remodify the entry.
.SH "Configuring UUCP for Dial-in Access"
.PP
The above examples show how to configure your \*(UU system so that it can
dial out to another remote system.
Configuring \*(UU so that other systems can dial into your system is, for the
most part, like configuring it to dial out; but this task does present
some special problems that you must consider.
The following example shows how to set up \*(UU so that remote system
.B dalek
can dial into your system.
.Sh "Giving a Remote UUCP Site a Login"
.PP
At this point, you are now the systems administrator of your \*(CO
system who must tell someone else how to set up her \*(UU to log into
your system.
We've shown you the flip side of this by showing you how to access
.BR mwcbbs :
now the job is yours.
.PP
When a \*(UU site calls your system, it must log in as would any ordinary
user would.
Once it has logged in, however, it runs the command
.B /usr/lib/uucp/uucico
rather than a shell, which a normal user would run.
This portion of what you must set up is configured in the file
.BR /etc/passwd .
.PP
You can create a \*(UU login by running the command
.BR newusr ;
then edit the last field of the
.B /etc/passwd
entry for the login you just established to run the command 
.B /usr/lib/uucp/uucico
instead of a
.B /bin/sh
or
.BR /usr/bin/ksh .
.PP
You could also create a \*(UU login by manually editing
.B /etc/passwd
and copy the entry for user
.BR uucp ,
but change the user name of 
.B uucp
to something else.
.PP
You must also define the home directory if using
.BR newusr .
Because this is a \*(UU account, the home directory appears
under the directory
.BR /usr/spool/uucp .
For example, if you wanted site
.B dalek
to call you, you might establish an
.B /etc/passwd
entry that looks like:
.DM
	dalek:\fIpassword\fP:6:6:Coherent-Coherent \e
		copy:/usr/spool/uucp:/usr/lib/uucp/uucico
.DE
Please note that
.I password
in the entry for
.B dalek
represents the encrypted password you assigned to site
.BR dalek .
Give the password to the system administrator of site
.B dalek
so that she may properly configure her chat script to log into your system.
.PP
If we were to stop right here,
.B dalek
could call your system, log in, and begin a \*(UU session.
Unfortunately, since we've yet to configure the \*(UU
files themselves to know about
.BR dalek ,
your site would quickly terminate the call when
.B dalek
identified itself to your system after completing its chat script.
.Sh "Configuring a Spooling Directory for Remote UUCP Access"
.PP
Each \*(UU site that calls your system must have a
.I "spooling directory"
in
.BR /usr/spool/uucp .
While logged in as
.BR root ,
go to the directory
.B /usr/spool/uucp
and run the command:
.DM
	/usr/lib/uucp/uumkdir dalek
.DE
.PP
.Sh "Configuring UUCP Files"
.PP
To control what
.B dalek
does on your system,
you should describe it in file
.BR /usr/lib/uucp/sys .
\*(CO includes a dummy entry in this file that you can easily modify
for site
.BR dalek .
You should make an entry that looks like this:
.DM
	system dalek
	time Never
	commands rmail rnews uucp uux
	remote-send /usr/spool/uucppublic !/usr/spool/uucppublic/bobfiles
	remote-receive /usr/spool/uucppublic !/usr/spool/uucppublic/private
.DE
.PP
This entry names
.B dalek
as a system \*(UU recognizes.
Your system will never call
.BR dalek ,
because the \fBtime\fR command (which gives the legal dates and times
during which the remote system can be contacted) states
.BR Never .
.PP
The \fBcommands\fR instruction names the commands that you permit
.B dalek
to execute on your system.
This line overrides any permissions that may be available to
.B dalek
on the system level:
that is, this line can stop
.B dalek
from executing commands that it otherwise would have permission to
execute (e.g., \fBls\fR or \fBcat\fR), but it cannot enable
.B dalek
to execute commands that it normally would not be permitted to run
(e.g., \fBsu\fR, \fBshutdown\fR, or \fBreboot\fR).
.PP
The lines \fBremote-send\fR and \fBremote-receive\fR name, respectively,
the directories whose contents
.B dalek
can read, and the directories into which it can write files.
Once again, these lines can stop
.B dalek
from accessing directories that it otherwise would be allowed to access
(e.g., \fBtmp\fR), but they cannot give
.B dalek
permission to manipulate directories that it normally would not be able
to access (e.g., \fBbin\fR).
.PP
In brief, all you have done is identified
.B dalek
to your system, named the commands it can execute on your system,
and limited the directories it can access.
It's that easy!
.Sh "One Last, Loose Thread"
.PP
With the spooling directory created, we are almost done.
Run this command:
.DM
	/usr/lib/uucp/uutouch dalek
.DE
It will place a dummy command in
.BR dalek 's
spooling directory.
More important, it returns an error if it finds some errors in the \*(UU
configuration for
.BR dalek .
.PP
Unfortunately, we cannot give you a test system that will call your system
to test your \*(UU configuration.
You will have to use this section as a guide
to configure for another \*(UU site to call yours.
.SH "Requesting Files From a Remote UUCP System"
.PP
To request a file from a remote \*(UU system, you must know
where that file is on the remote system.
The file
.B howto.start
can be found in the directory
.B /usr/spool/uucppublic/mwcnews
on
.BR mwcbbs .
This file introduces
.BR mwcbbs ,
its features and intended uses, and how to request files from it.
.PP
With this bit of knowledge, we can now request the file with the command
.BR uucp .
.PP
.B uucp
is very simple.
Invoked it with a specific site to call, and file to upload or download.
For example, the command:
.DM
	uucp mwcbbs!/usr/spool/uucppublic/mwcnews/howto.start /tmp
.DE
tells your machine to call
.BR mwcbbs ,
download the file
.DM
	/usr/spool/uucppublic/mwcnews/howto.start
.DE
and put it into directory
.B /tmp
on your system.
The call will take place seconds after you enter the command,
unless you tell
.B uucp
to spool the request.
For more information on
this and other arguments, see the Lexicon entry for
.BR uucp .
.PP
Please note that the entry for
.B mwcbbs
in
.B /usr/lib/uucp/sys
must specify that
.B mwcbbs
can write to
.B /tmp
as part of the
.B remote-receive
instruction.
.PP
To send a file to
.BR mwcbbs ,
use the command:
.DM
	uucp \fIfilename\fP mwcbbs!/usr/spool/uucppublic/uploads/
.DE
This command uploads a copy of
.I filename
to the directory
.B /usr/spool/uucppublic/uploads
on
.BR mwcbbs .
Again, the call takes place within seconds, unless you tell
.B uucp
to spool the request.
.PP
At this point, we have completed our
.B uucp
configuration to ``talk''
to
.BR mwcbbs ,
and we have requested our first file.
You can tell
.B uucp
to download other files from
.BR mwcbbs ;
only the file names and path names will change.
.SH "Sending Files to a Remote UUCP System"
.PP
Suppose, for example that site \fBsanta\fR has been described to
your \*(UU system, and everyone has permission to read from your
current directory.
Suppose, too, that you have permission to write into directory
\fB/usr/spool/reports/parents\fR.
To send the files \fBgood.kids\fR and \fBbad.kids\fR to \fBsanta\fR,
type the following command:
.DM
	uucp good.kids bad.kids santa!/usr/spool/reports/parents
.DE
The
.B uucp
command compels \*(UU to copy one or more files from your site to a remote site.
\*(UU queues both files automatically and sends them at the next scheduled time.
.PP
Note, too, the use of the \*(Ql!\*(Qr in the above command.
The `!' separates a site name from another site name, from
a directory name, or from a user ID.
In the above example, the `!' indicates that directory
.B /usr/spool/reports/parents
can be found at site
.BR santa .
One feature of a \*(UU network is that any member can send
files to any other member.
That does not mean that every member must have full permissions
with every other member; rather, for the sake of efficiency it
is possible to route files through one or more intermediate
computers, to allow batch transmissions of files.
For example, to send the file \fBvisibility\fR
to user \fBblitzen\fR via machines
\fBsanta\fR and \fBreindeer\fR, use the following command:
.DM
	uucp visibility santa!reindeer!blitzen!/usr/spool/weather/usa
.DE
In this example, the string
.B santa!reindeer!blitzen!/usr/spool/weather
indicates that directory
.B /usr/spool/weather
can be contacted at site
.BR blitzen ,
which in turn can be contacted via site
.BR reindeer ,
which in turn can be contacted via site
.BR santa .
This scenario assumes that site \fBreindeer\fR
has permission to write into directory
.B /usr/spool/weather
on site
.BR blitzen ,
and that site
.B santa
has permission to upload files to site
.BR reindeer .
(And, of course, that you have permission to upload files to site
.BR santa .)
If any of these are not true, the transaction will fail.
.\".PP
.\"With \*(UU networks growing to international dimensions, such path
.\"names can become quite complex. 
.\"The command \fBmail\fR has an \fBalias\fR function that allows
.\"you to define a user's \*(UU path name under a simpler
.\"name that serves as that user's alias.
.\".B mail
.\"reads the file
.\".B /usr/lib/mail/aliases
.\"for every user listed on its command line.
.\"If it finds a match, then it substitutes the description in
.\".B aliases
.\"for the user name.
.\"If the entry in aliases consists of two or more fields separated by
.\"exclamation points,
.\".B mail
.\"then invokes the
.\".B uucp
.\"command to copy the mail message to the named site.
.\"For example, if you regularly send mail to user
.\".B joe
.\"at site
.\".B widget ,
.\"then insert the following entry into
.\".BR /usr/lib/mail/aliases :
.\".DM
.\"	joe:	widget!joe
.\".DE
.\"Make sure, first, that you have described site
.\".B widget
.\"to \*(UU or this will not work.
.\"Second, make sure that your local system does not have a user named
.\".BR joe ;
.\"if it does, his mail thereafter will be shipped to the other
.\".B joe
.\"at the remote site.
.SH "UUCP Administration"
.PP
Once you have written and debugged the descriptions of your ports,
dialers, and systems,
administering \*(UU consists mainly of
reviewing the log files periodically to ensure that all connections
are being made, and all programs executed correctly.
The command
.B uulog
will assist you in this.
When you type the command
.DM
	uulog widget
.DE
.B uulog
will open all of the log files associated with site
.BR widget ,
and display them for you.
Given that the log files for given site are kept in four different
directories, this can be a great convenience.
.PP
Logfiles are organized as follows:
.DM
	/usr/spool/uucp/.Log/uucico/\fIsitename\fP
	/usr/spool/uucp/.Log/uucp/\fIsitename\fP
	/usr/spool/uucp/.Log/uux/\fIsitename\fP
	/usr/spool/uucp/.Log/uuxqt/\fIsitename\fP
.DE
As you can see, one logfile for each site is kept in a directory named
after a given \*(UU command.
\*(UU records every transaction; so by reading these files, you can see
whether your \*(UU commands are succeeding.
.PP
If you are having trouble with your \*(UU connections,
send files through \*(UU and observe how they fail.
You may need to use \fBuuinstall\fR a few times to tweak your description
of the remote site.
If all else fails, contact Mark Williams Company.
.PP
If all is going well, you should run
.B /usr/lib/uucp/uumvlog
every day.
This keeps the log files from getting out of hand.
The previous section on setting the polling time describes how to do this.
.PP
The main task of the \*(UU administrator is
to monitor the \*(UU log files to see that hardware
is functioning correctly, and that files are transferred
correctly.
For example, failure to connect with a remote site after
several attempts may mean that the remote site is having
problems with its modem, or that it is scheduling outgoing
calls for when you were scheduled to call in.
Likewise, failure to receive scheduled calls from several
sites may indicate equipment failure on your end.
.PP
Finally, the \*(UU administrator must monitor the use of
disk space on the system.
Old mail and messages, multiple copies of files, and files
automatically input by various subscription and network services
can eat up disk space rapidly; you must prune extraneous material
ruthlessly.
.SH "Networks"
.PP
\*(UU becomes truly useful when you are hooked into a network of machines
that exchange information.
Through \*(UU, you can gain access to the Internet,
through which you can exchange news and mail with others users
around the world.
.PP
This section briefly describes the services you can obtain from
a network, and the networks now available.
.Sh Services
.PP
Many different services are available from networks:
domain-name service (DNS), mail exchanger service, and connectivity.
.PP
.II "UUCP^domain-name service"
DNS is the registration of an Internet-style domain,
e.g.,
.B mwc.com
or
.BR baqaqi.chi.il.us .
This usually divides into two subservices \(em
registration in an existing domain, and registration of your own domain.
For this service you need two sites on the Internet willing
to either register you in their nameserver or run a nameserver for
your domain.
A
.I nameserver
gives other people's machines information
about your registered machine or registered domain.
In particular, the nameserver publishes an MX record,
which tells machines how to get mail to you.
(There are other kinds of records, but MX
records are the important ones for \*(UU sites.)
.PP
.II "UUCP^mail forwarding"
MX service is mail forwarding.
The MX record published by the
nameserver must point at a machine directly on the Internet.
This machine will the be responsible for figuring out how to actually get
the mail to you.
.PP
.I Connectivity
usually means a \*(UU connection.
Because almost everybody
in a major city is connected to everyone else, a \*(UU connection
to anybody effectively translates to an indirect connection to the Internet.
.Sh "Available Networks"
.PP
.II UUCP^UUNET
.II UUNET
.B UUNET
is a company in Falls Church, Virginia, that provides a large
number of networking services, including all of those mentioned above.
.PP
.II UUCP^UUCP network"
The
.B "UUCP network"
is an extremely
informal group of machines defined only by the fact that they connect
to each other via the \*(UU protocol.
It is one of the largest
networks in the world and has no central control.
.PP
.II UUCP^UseNet
.II UseNet
.B UseNet
is a network of machines defined by the fact that they
exchange UseNet news with each other.
UseNet is also an anarchy \(em
no central organization runs it.
It includes machines in the \*(UU network and the Internet,
as well as hundreds of other networks.
It is the largest network in the world.
.PP
.II Internet
.II UUCP^Internet
The
.B Internet
is a group of high-speed networks which all
communicate with
.IR "Internet Protocol" ,
.II TCP/IP
.II UUCP^TCP/IP
i.e., TCP/IP.
The networks that
comprise the Internet are mostly academic and research networks run by
large central organizations, such as the National Science Foundation or
the Australian Academic Research Project.
The center of the Internet
is the NIC (Network Information Center), whose address is
.BR nic.ddn.mil .
.PP
The
.B internet
(lower-case `i') is defined by connectivity under mail.
It is technically larger than UseNet, though
less is said about it because it is so weakly defined.
.PP
For information on networks, what is available on them, and how to connect
to them, we strongly recommend the book \fIThe Whole Internet:
User's Guide and Catalog\fR, cited above.
For a copy, check your local bookstore, or telephone the publisher,
O'Reilly & Company, at 1-800-998-9938.
.SH "Debugging UUCP Problems"
.PP
When you have a problem with \*(UU \(em and in particular, a problem with
telephoning another \*(UU system \(em you must have
a clear picture of what is occurring, and what is not occurring.
For instance, if you try to call
.B mwcbbs
and \*(UU fails, you must determine what is working properly
before the failure takes place.
.PP
The following subsections describe common problems with \*(UU, and gives
some hints on how to solve them.
Please review them
.I carefully
before you telephone Mark Williams Company to ask for help.
If you do not, we will ask that you do review them and call back
only if you still cannot solve the problem.
.Sh "Define the Problem Exactly"
.PP
\*(UU problems can take many forms.
Define the problem
.IR exactly .
This process may actually help you solve the problem.
Statements like ``I'm having a problem using \*(UU'' or
``\*(UU doesn't work'' do not describe problems relating to \*(UU.
You need to know
.I exactly
what does or does not happen when you try to connect with another site.
Please review it before you call Mark Williams Technical Support.
.\" i.e., RTFM!
.PP
Before you do anything else, trying running
.B uucp
and
.B uucico
with the option
.BR \-x .
This option tells these programs to log what they do; the logs are written
into the subdirectories of directory
.BR /usr/spool/uucp/.Admin .
Often, these log messages will point directly to the problem.
.PP
A subtle error within a \*(UU configuration file can cause no end
of grief when you try to debug a \*(UU problem.
To help spot these potential problems,
run the command \fB/usr/lib/uucp/uuchk\fR.
This command generates a full report on your configuration files.
It will spot and report on syntax errors in your configuration files.
You will, of course, have to fix by hand whatever
.B uuchk
finds to be in error.
.PP
The following sections discuss commonly encountered problems.
.Sh "Enabling and Disabling Ports"
.PP
.II enable
.II disable
On some \*(CO systems, the permissions for the programs
.B /etc/enable
and
.B /etc/disable
are set to:
.DM
	-r-x------   root     root
.DE
That is, these commands can be executed only by user
.BR root .
This is to close a security hole; otherwise, a person who breaks into
your system can disable any port she wants \(em including the console.
.PP
If you have only one modem, and you both initiate and receive \*(UU
sessions on that modem, you may run into problems with enabling and
disabling that port.
.II uucico^permissions
For the communications program
.B uucico
to be able to enable and disable the port on its own,
.B /etc/enable
and
.B /etc/disable
must have the permissions:
.DM
	-r-s--s--x   root     root
.DE
These permissions permit
.B uucico
to dial out on an enabled port on its own; but it reopens the security hole.
.I "Caveat utilitor."
.PP
.II chmod
For information on how to change permissions, see the Lexicon entry for the
command
.BR chmod .
.Sh "Stale Requests and Multiple Requests"
.PP
From time to time, you may accidentally issue the same
.B uucp
request more than once.
.PP
Note that if
.B uucp
fails because you failed to connect with the remote site,
one action you should
.I not
take is to repeat the
.B uucp
command.
If you do this,
.B uucp
will simply queue another request for the same file or files that you
requested with the previous
.B uucp
command.
When connection is finally made, multiple
.B uucp
requests will be executed, thus downloading multiple copies of the same file
or files.
Depending upon the size of the file or files, this could be an expensive
mistake.
.PP
To remove stale requests, log in as user
.B uucp
or user
.BR root ;
then
.B cd
to directory \fB/usr/spool/uucp/\fIsitename\fR and remove the extraneous
requests.
(You can also do this to remove mail files about which you have had second
thoughts.)
.PP
Note that a
.B uucp
generates one file, which has the prefix
.BR C .
A mail message generates two files:
one with the prefix
.BR C ,
which tells the remote system what to do with the mail message; the
other has the prefix
.BR D ,
which holds the text of the message.
Read the existing files to make sure that you are removing the correct
files.
.Sh "Problems With Lock Files"
.PP
If
.B uucico
wishes to dial on a modem but somebody else is already using it,
you will see the message
.DM
	ERROR: All matching ports in use
.DE
in the log file for the site
.B uucico
is attempting to call.
The solution simply is to wait until the port clears.
.PP
If
.B uucico
is already communicating with a given remote system, or if a lock
file exists for a given remote system, you will see the message
.DM
	ERROR: System already locked
.DE
in the log file for the site
.B uucico
is attempting to call.
.PP
Sometimes lock files are not cleared properly, and so tie up the system
long after they should have been removed.
Such files are called
.I stale
lock files.
The solution is to use the command
.B uurmlock
to remove stale files.
.PP
Note that in some instances, permission problems may stop
.B uurmlock
from removing lock files.
In this case, log in as the superuser
.B root
and execute the command:
.DM
	rm /usr/spool/uucp/LCK*
.DE
.Sh "Enabling Ports, /etc/ttys Problems"
.PP
.B uucico
reads a port's status in file
.BR /etc/ttys ,
then restores that status after it finishes its work.
If you had disabled a port by hand, it remains disabled after
.B uucico
has worked with it \(em which means, of course, that no remote system
can dial into your system via that port.
To re-enable a port, use the command
.BR /etc/enable .
.PP
Note, too, that file
.B /etc/ttys
is sensitive to the order in which devices are named within it.
The port into which you have plugged your modem must have an entry for
both the remote device (e.g.,
.BR /dev/com1r )
and the local device (e.g.,
.BR /dev/com1l ).
Note that the entry for the raw device
.I must
precede the entry for the local device.
If it does not,
.B uucico
will not be able to dial out properly.
.PP
Note, too, that device
.B /dev/console
.I must
be the last entry in
.BR /etc/ttys ,
or your system will not be able to dial out via a serial port.
.Sh "Permission Problems"
.PP
Incorrect permissions on
files and directories will harm \*(UU's behavior.
.PP
.B uucico
runs as a user named
.B uucp
and therefore does not have any special permission or privileges
that give it free access to every file or directory on your system.
For example, consider what \*(UU does when it attempts to contact a
remote system:
.IP \fB1.\fR 0.3i
The daemon
.B uucico
checks the directory
.B /usr/spool/uucp
to see if any lock files are present;
these lock files indicate whether someone has already logged into the port
from which
.B uucico
wishes to dial out.
.IP \fB2.\fR
If
.B uucico
finds a lock file for the port it wants to use, it checks the file
.B /usr/lib/uucp/sys
for information on an alternate way to contact the remote system.
If
.B uucico
finds an alternate way to contact the remote system, it tries that way.
If it does not, then it quits.
.IP \fB3.\fR
When
.B uucico
finds a port that is available,
it then check file
.B /etc/ttys
to see if the port is already enabled for remote logins.
.IP \fB4.\fR
If the port is enabled,
.B uucico
disables the port and makes its call.
.PP
So far, so good.
If, however,
.B uucico
does not have read and write permission on the port device from which
it is attempting to make its call, its attempt to make the call will fail.
.PP
For another example of how directory-level permission affect the behavior
of \*(UU, consider how \*(UU transfers files.
When \*(UU transfers files,
it stores those files as temporary files in the directory
\fB/usr/spool/uucp/\fIsitename\fR,
where
.I sitename
is the name of the system with which \*(UU is communicating.
All files and directories under
.B /usr/spool/uucp
must be owned by user
.B uucp
and group
.BR uucp ,
or \*(UU cannot transfer files correctly.
.PP
\*(UU can also write temporary files into directory
\fB/usr/spool/uucp/.Temp/\fIsitename\fR.
This directory must also be owned by user and group
.BR uucp .
.PP
The permissions on the serial port from which you will dial out
can affect the behavior of \*(UU.
\*(UU must have permission to read and write to that port.
The device specified by the
.B line
entry in 
.B /usr/lib/uucp/port
should have permissions of
.B 666
(see the Lexicon entry for the command
.BR chmod ).
.PP
.B uucp
should own all of its spooling directories.
The
.I "spooling directory"
is the directory into which
\*(UU writes stores data and command files for the site being contacted.
The spool directory for a given remote site resides under
.B /usr/spool/uucp
and is named after the remote site.
For example, your system will use directory
.B /usr/spool/uucp/mwcbbs
to store files being exchanged with
.BR mwcbbs .
Likewise,
.B mwcbbs
has the directory \fI/usr/spool/uucp/\fIyoursystem\fR,
where \*(UU stores
files to be exchanged with
.IR yoursystem .
.Sh "UUCP Cannot Find Its Own Files"
.PP
If the command
.B /usr/bin/uucp
says it can not get its own name when you invoke it,
then you give yourself a \*(UU site name of no more than
seven characters in the file
.BR /etc/uucpname .
.PP
The command
.B /bin/mail
command may also return a similar message.
The cure is the same.
.PP
As noted above,
a \*(UU command may also fail to execute because permissions are
set up incorrectly on the \*(UU executables.
\*(UU commands frequently invoke one another.
For instance,
.B uucico
invokes
.B uuxqt
after it has communicated with a remote system.
.B uuxqt
processes all files uploaded from the remote system.
.B uuxqt
may, in turn, invoke other commands \(em for example, it can invoke
.B smail
to deliver mail to a user on your
system or forward mail to a user on another remote system.
If the permissions on a \*(UU executable are incorrect,
it may find that, when it tries to complete a task, it
does not have permission to write to a given directory or a log.
A list of \*(UU permissions appears in the Lexicon entry for
.BR UUCP .
Make sure that the permission on your file conform to what is given there.
.Sh "Modem Configuration"
.PP
The commonest source of error lies in modem configuration.
Each modem has its idiosyncrasies, and command languages differ from
device to device; however, in general your modem should be configured
as follows:
.IP \(bu 0.3i
Echo off.
.IS \(bu
No result codes.
.IS \(bu
Carrier detect (DCD) is true.
.IS \(bu
Terminal ready (DTR) is true.
.IS \(bu
DCD follows DTR.
.PP
If you are working with a high-speed modem (9600 baud or faster),
you should also configure it to do the following:
.IP \(bu 0.3i
Lock modem to computer baud rate at 19,200
if your modem supports it;, if not, lock it to 9600 baud.
.IS \(bu
Set modem for RTS/CTS handshaking.
.IS \(bu
Use a flow-control device for the port's description in file
.BR /usr/lib/uucp/port .
See the Lexicon entry
.B asy
for details on how to do this.
.PP
Telebit modems present special problems for configuration.
They are designed to be used with \*(UU, and in general
they are fast and robust; but
because they do more than ordinary modems, they require more
extensive setup to work correctly.
The Lexicon entry for
.B modem
gives detailed information on how to set up a Trailblazer modem so that
it works properly with the \*(CO implementation of \*(UU.
.PP
If your modem supports data compression, it may not be ideal to use this
feature with every remote site.
For instance, attempting to compress files that already are compressed
(as are the files on the Mark Williams bulletin board),
only adds to the data stream and
.I reduces
overall throughput.
Before you turn on data compression, make sure that the files you are
downloading are
.I not
compressed.
.Sh "The Modem Does Not Respond"
.PP
When you try to call a system via the commands
.B /usr/bin/uucp
or
.B /usr/lib/uucp/uucico
and the modem does not respond (i.e., the lights on the modem do not
flicker), look at file
.BR /usr/lib/uucp/port .
Check the permissions on the
serial port used to dial out on, as specified therein.
.PP
In some cases, you will see the error message
.DM
	Retry time not reached
.DE
Taylor \*(UU puts a horizon on callouts:
if a call to a given site fails, \*(UU will wait a predetermined
amount of time before it tries again.
If you are repeatedly invoking
.B uucp
or
.B uucico
from the command line, you may see this error.
To get around this limitation, use the command-line option
.BR \-f ;
for example:
.DM
	uucico -s systemname -f
.DE
This problem can also arise if a previous connection to a site failed.
In this case, \*(UU writes a bad-connection report into file
.DM
	/usr/spool/uucp/.Status/\fIsystemname\fP
.DE
where
.I systemname
names the system you are trying to contact.
\*(UU does this to keep your system from wasting time contacting a system
whose connection is defective, even if you use the \fB\-S\fR option with
.BR uucico .
Remove this file and \*(UU will resume dialing out.
Note that this will not clear up the problem that triggered the
original bad-connection report, and the connection may fail for other reasons.
.Sh "The Modem Responds But Does Not Dial"
.PP
In some cases, the modem responds (i.e., its lights flicker)
but it does not dial out.
This can have any of several causes.
.PP
First, the modem's register settings may be incorrect.
Review them.
Check the above example for some simple examples of how to set
modem registers, and check the documentation that came with your modem.
.PP
You may be trying to access the modem through the remote COM port,
e.g.,
.BR /dev/com1r .
In this instance, the system awaits a carrier signal, as you cannot
open a remote COM port without; therefore, no communication ever begins.
The solution is to use the local device, e.g.,
.BR /dev/com1l .
This way, the system will not wait for the carrier to come up on the modem,
and dialing will begin.
.Sh "The Modem Dials But No Connection Made"
.PP
Sometimes a modem will dial out but no connection is made.
This is typically caused by plugging the telephone line
into the wrong port on the modem.
.PP
Check the log file for the site you are calling.
It will usually give a message that indicates what the problem really is.
If calling
.BR mwcbbs ,
use the command:
.DM
	uulog mwcbbs
.DE
.Sh "The Modem Dials, Carrier Is Established, Nothing Else Happens"
.PP
The first suspect is the modem's register settings.
The modem register settings that we discussed above generally work well for
.B uucp
to dial out to another system,
.I if
your modem is Hayes-compatible.
If it is not, or if it is an off-brand that only
.I claims
to be Hayes-compatible,
check your modem's documentation and make sure that
the register settings are correct.
.PP
To get a picture of what is happening, run the command
.B /usr/lib/uucp/uucico 
with the option \fB-xchat\fR.
If calling
.BR mwcbbs ,
use the command:
.DM
	/usr/lib/uucp/uucico -Smwcbbs -xchat
.DE
This tells your system to call
.B mwcbbs
and to write debugging information into its log file
.BR /usr/spool/uucp/.Admin/audit.local ,
which you can review later.
This is very useful in determining if there is a problem in a chat script.
.Sh "uulog Shows Lost Packets"
.PP
If your \*(UU communication sessions terminate prematurely,
your system may be losing characters on the serial ports.
An indication of this problem is the appearance of the message
.B "Lost Packets"
in your \*(UU logs.
If your system exhibits these symptoms during transfers on 4800-bps or
higher-speed lines,
.I
we strongly urge you to replace your existing
8250- or 16450-based UARTs with those based upon the 16550A design,
such as the National Semiconductor NS16550AFN.
.R
These newer UARTs are pin-compatible with the older UARTs.
\*(CO automatically senses and enables them when it boots.
.Sh "uulog Shows Incorrect Response"
.PP
This points to one of four problems:
.IP \fB1.\fR 0.3i
Your site is sending an improper site name
to the remote system (in other words, the remote site doesn't
know about your system).
.IP \fB2.\fR
The remote site does not have a spooling directory
for your site.
.IP \fB3.\fR
Your site does not have a spooling directory for the remote
site
.IP \fB4.\fR
.B /usr/lib/uucp/sys
contains an error or incorrect chat script.
.Sh "Files Refuse To Be Sent or Cannot Be Received"
.PP
Check the instructions
.BR local-send ,
.BR local-receive ,
.BR remote-send ,
and
.BR remote-receive
in the remote system's entry in file
.BR /usr/lib/uucp/sys .
This can result in the remote system being denied permission to load files
onto your system, or read files from it.
Make sure these instructions are set correctly, and point to directories
in which user
.B uucp
has read and write permission.
.Sh "File Transfers Fail With imsg Statements"
.PP
One problem is frequently encountered when \*(CO systems attempt to \*(UU
with Sun workstations:
connections are made correctly, but when file transfers fail with numerous
iterations of the debug message ``imsg'' until the script times out.
This is caused by differences in parity:
the \*(CO chat scripts by default use no parity,
while those on the other system do.
The solution is to change the chat script on the Sun system to set
no parity when it talks with \*(CO system.
.Sh "Files are Being Lost"
.PP
If a configuration problem exists on your side of a \*(UU connection,
files could be lost.
This may be true if
.B uucico
or
.B uuxqt
are running with incorrect permissions.
Taylor \*(UU notes problems of this nature in its log files.
If it can preserve a file that would otherwise be lost,
.B uucico
saves the file in directory
.BR /usr/spool/uucp/.Preserve ,
and logs the fact that it has saved it.
.Sh "Non-COHERENT UUCP Site Problems"
.PP
It is important to understand that \*(CO's \*(UU is designed to be
compatible with other implementations of \*(UU, but may not use the same
configuration files.
This can complicate the debugging of problems when you attempt to establish
communication with a system that uses a different implementation of \*(UU.
.PP
We will supply whatever assistance we can, but if it is
determined that the non-\*(CO site is part of the problem,
it is up to you to find how that non-\*(CO site has configured itself.
You may even need to set up a
conference call among yourself, the remote site's administrator,
and MWC Technical Support.
.SH "Where to Go From Here"
.PP
As we mentioned earlier, \*(CO does not now implement the entire Taylor \*(UU
package.
Full sources for Taylor \*(UU are available from
.BR mwcbbs ,
and from various sites on the Internet.
.PP
This tutorial only touches upon the esentials of configuring \*(UU for
communicating with other systems.
Taylor \*(UU is much more conformable than this tutorial may have led you
to believe.
.PP
For a fuller description of the Taylor \*(UU configuration files, see the
Lexicon entries for
.BR dial ,
.BR port ,
and
.BR sys .
For further information, check the Lexicon entry for each \*(UU command,
as well as the overview article
.BR UUCP .
This article will also point you to related articles in the Lexicon,
such as the ones for
.B modem
and
.BR RS-232 .
