.ds TL "TCP/IP Commands"
.NH "Introduction to TCP/IP Commands"
.PP
This chapter introduces the commands with which
you can interact with other hosts on your network.
You can use these commands to communicate
with any other host that supports TCP/IP.
.PP
For information on terminology and the concepts that underlie TCP/IP, see the
previous chapter,
.IR "Administering TCP/IP" .
.SH "ftp:  Transfer Files"
.PP
The command
.B ftp
lets you to transfer files between your local COHERENT host
and any remote host that runs an
.B ftp
server.
.PP
To use
.BR ftp ,
invoke the
.B ftp
client on your local system.
It will establish connection with the
.B ftp
server on the remote host.
After you log into the remote host, you can issue
.B ftp
subcommands to exchange files with that host.
.PP
The remote server can be the one supplied by NCM.
In this section, it is assumed that all
.B ftp
server messages come from a Racal InterLan server running on a remote UNIX host.
If the remote host is running another type of server,
the messages you see may be different.
If the remote host runs a different operating system,
.B ftp
uses the format for that system
(for example, directory listings appear in the style of the remote host).
.Sh "How to Invoke ftp"
.PP
To invoke this command, type
.B ftp
followed by the name of the remote host.
You can name the host by either its host name or its Internet address.
For example, to connect to host
.B whouse
whose Internet address is 89.0.0.1, type either
.DM
	ftp whouse
.DE
or
.DM
	ftp 89.0.0.1
.DE
If
.B ftp
can make the connection, you see messages similar to the following:
.DM
	Connected to whouse ftp server
	(Version 1.00 Mon Sep 26 00:36:28 EDT 1986) ready
.DE
The first message comes from the
.B ftp
client on your system; it verifies that the
server has succeeded in connecting to the remote host.
The message echoes the host's name or its Internet address, whichever you used.
The second message comes from the
.B ftp
server on the remote host;
it uses a numbered response to identify itself and indicate its status.
(The server also displays its version number and date of creation.)
The significance of the server's numbered responses is described below.
.PP
If you invoke
.B ftp
without an argument,
you enter
.BR ftp 's
command interpreter;
this is signified by the prompt
.BR ftp>> .
You can then connect to a remote host by issuing the
.B ftp
command
.BR open .
.BR ftp 's
suite of commands is introduced below.
.PP
You can include various options on the
.B ftp
command line.
For a complete list of
.BR ftp 's
command-line options, see its entry in the Lexicon at the back of this manual.
.Sh "Connection Error Messages"
.PP
Sometimes
.B ftp
cannot contact a server on the remote host:
the remote host may not be up
or not running an
.B ftp
server.
.PP
Your
.B ftp
client tries for a reasonable time
to connect to the remote host, then quits
and sends you the error messages:
.DM
	Connection timed out.
.DE
After you get this error message, the
.B ftp
prompt
.B ftp>
appears.
You can then try to connect again by using the
.B ftp
command
.BR open .
.PP
You will also fail to connect if you type an invalid Internet address
or an invalid host name (for example, if you type
.B badrock
instead of
.BR whouse ).
In this case, you get the following error message:
.DM
	badrock: Unknown host.
.DE
When you get this error message, issue the
.B open
command with the correct host name at the
.B ftp
prompt.
.Sh "Logging in to the Remote Host"
.PP
After the
.B ftp
client establishes a connection with a remote host,
you must log into the remote host.
.B ftp
then prompts you for your user name and password.
.PP
For example, assume that you have connected to a remote host called
.BR whouse .
By default,
.B ftp
also uses the name you gave on the local host (for example,
.BR hillary )
as the user name on the remote host.
You therefore see
.DM
	Name (whouse:hillary):
	331 Password required for hillary
	Password (whouse:hillary):
.DE
To use the default login name that
.B ftp
displays, simply press the
.B <Return>
key; if you wish to log under a different user name, type that instead and
then press
.BR <Return> .
.B ftp
then prompts you for your password; enter that and press
.BR <Return> .
.PP
After
.B ftp
accepts your password, it displays a message that verifies that
you have logged into the remote system, then the
.B ftp
prompt.
Once you see the prompt, you can issue any of the
.B ftp
commands to exchange files with the remote host,
obtain information,
or alter the behavior of
.B ftp
itself.
.PP
If you do not give the correct password, the login
process fails and you receive the following messages:
.DM
	530 Login incorrect.
	Login failed.
.DE
These messages display the two types of error messages you can get while using
.BR ftp .
The first message, preceded by the three-digit number,
comes from the server;
the second comes from the client.
To try to log in again, issue the
.B user
command described in the next section.
.Sh "Issuing ftp Commands"
.PP
Under
.BR ftp ,
you must connect to a remote host and log in before you can issue
the full suite of
.B ftp
commands.
For a complete list of its commands, see the entry for
.B ftp
in the Lexicon at the end of this manual.
.PP
If, for example, you try to give a command like
.B ascii
before you have established a connection, you see the following error message:
.DM
	Not connected.
.DE
To connect to a remote host, you must
first issue the command
.BR open ,
as described below.
.PP
Type all
.B ftp
commands in lower-case letters.
You can abbreviate the subcommands, as long as the
abbreviation is unambiguous; for example, the subcommand
.B binary
can be abbreviated to
.BR bi ,
to distinguish it from
.BR bell ,
which can be abbreviated to
.BR be .
.PP
If you make a mistake when you issue an
.B ftp
subcommand, you receive the message:
.DM
	Invalid command.
.DE
The following subsections introduce some of the most important
.B ftp
commands.
.Sh "open:  Connect to a Remote Host"
.PP
If you have not already connected to a remote host,
or wish to connect to another remote host after you have
closed a connection, use the command
.B open
followed by a host name or Internet address.
If you issue the command without specifying a host name or Internet address,
.B ftp
prompts you for an entry.
The following commands opens a connection to remote host
.BR whouse :
.DM
	open whouse
.DE
If you type a valid host name or Internet address and
.B ftp
can make the connection, you receive the connection
messages shown above, and are prompted to log into the remote host.
.Sh "user:  Log into a Remote Host"
.PP
The command
.B user
lets you log into a remote host.
Just type
.B user
at the
.B ftp
prompt, followed by your login identifier.
.B ftp
will prompt you for your password.
.B ftp
will then send you a message to indicate if you have succeeded in
logging into the remote host.
.Sh "ascii, binary, and type:  Choose a File-Transfer Type"
.PP
.B ftp
can transfer both
ASCII and binary (or ``image'') files.
.BR ftp ,
however, uses different methods to transfer each type of file;
you must tell it what type of file you wish to transfer at any given time.
To select a file type, use the commands
.BR ascii ,
.BR binary ,
and
.BR type .
.PP
The command
.B ascii
tells
.B ftp
that you wish to transfer text files.
This is the default.
.PP
The command
.B binary
tells
.B ftp
that you wish to transfer binary files.
You then receive the following message:
.DM
	200 Type set to I.
.DE
The letter `I' in the message stands ``image''.
.PP
The command
.B type
also changes the file-transfer type.
You can specify
.BR ASCII ,
.BR binary ,
or
.BR image .
For example, the command
.DM
	type image
.DE
changes the file type to image (i.e., binary).
.PP
If you do not specify a type, the
.B type
command displays the current file-transfer type.
.Sh "dir and ls:  Read Directory Contents"
.PP
.B ftp
provides commands with which you can read and manipulate directories.
The commands
.B dir
and
.B ls
print the contents of a directory on the remote host \(em
by default, the directory that you are in.
.PP
Both commands can be invoked with an argument that names a directory
or files on the remote host.
For example, the command
.DM
	dir /bin
.DE
prints the contents of directory
.B /bin
on the remote host.
You can use wildcards in an argument; for example, the command
.DM
	ls /bin/uu*
.DE
prints the name of every file in
.BR /bin
that begins with the string
.BR uu .
For details on how to use wildcards, see the article
.B wildcards
in the COHERENT manual's Lexicon.
.PP
These commands differ in that
.B dir
prints the name of each file, its size, and the date it was last modified,
whereas
.B ls
just prints its name.
.PP
When you read the contents of a directory on the remote system,
the remote system's server sends you numbered
responses to show how it has handled your request.
For example, the message
.DM
	Opening data connection for /bin/ls (89.0.0.1, 1166).
.DE
names the directory that you are reading, and
(in parentheses) the Internet address and the port number,
respectively, separated by a comma.
.Sh "cd and pwd:  Change Directory, Print Working Directory"
.PP
.B ftp
also lets you change your directory.
To change to another directory on the remote host, type
.B cd
followed by the name of the new directory.
As with the COHERENT shell, the abbreviation `..' stands for the
parent directory of the current directory.
To change directories on your local host, type
.B lcd
(for ``change local directory'')
followed by the name of the directory to which you wish to switch.
.PP
For example, to change from directory
.B /usr1/tony
on the remote host to
.BR /usr1/tony/junk ,
for example, type:
.DM
	cd junk
.DE
When
.B ftp
changes the directory to the one you specify, it prints the message:
.DM
	200 CWD okay.
.DE
The command
.B pwd
tells you what directory you are in on the remote host.
.Sh "get and put: Transfer Files"
.PP
The commands
.BR get ,
.BR mget ,
.BR put ,
and
.B mput
copy files between your local host and the remote host.
The
.B get
commands copy files from the remote host to your local host; whereas the
.B put
commands copy them from your local host to the remote host.
The syntax of these commands closely resembles that of the COHERENT command
.BR cp .
.PP
To transfer one file from the remote host to your local host,
use the command
.BR get ,
followed by the name of the file on the remote host that you wish to copy
and the name of the file on your local host into which you wish to copy it.
If you do not name a file on your local system,
.B ftp
gives the copied file the same name it had on the remote host.
If your system already has a file by that name in your current directory,
.B ftp
overwrites it.
If you type
.B get
without an argument,
.B ftp
prompts you for a remote file, and then for the local file.
.PP
For example, to copy file
.B myfile
from the remote host to your local host, type:
.DM
	get myfile
.DE
To copy the file
.B work1
into file
.B morework
on your local system, issue the following:
.DM
	get work1 morework
.DE
If
.B ftp
does not encounter any errors, you will see the following messages:
.DM
	150 Opening data connection for work1 (89.0.0.2, 1124).
	226 Transfer complete.
.DE
To copy one file from your system to the remote system, use the command
.BR put ,
followed by the name of the file you are transferring and the name of
the file into which it will be written.
If you do not name a file on the remote host,
.B ftp
gives it the same name it has on the local host.
If the current directory on the remote host already has a file
by that name, the
.B ftp
server on the remote system will change its version number,
notify you that a file already exists under that name,
or overwrite the existing file,
depending upon the implementation of the server.
If you simply type the command
.BR put ,
.B ftp
first prompts you for a local file, and then for the name of the remote file.
.PP
For example, to transfer file
.B lanplan.p1
to the remote host under the name
.BR lanplan.p2 ,
type:
.DM
	put lanplan.p1 lanplan.p2
.DE
Assuming there are no errors,
.B ftp
prints the message:
.DM
	Opening data connection
.DE
The commands
.B mget
and
.B mput
copies multiple files.
.B mget
copies the files you name on its command line from the remote host to
your local host; whereas
.B mput
copies them from your local host to the remote host.
These commands also let you use wildcards to name multiple files, assuming
that you have turned on wildcards for
.B ftp
(as described below).
.PP
For example, to transfer the files
.BR profits ,
.BR taxes ,
and
.B lawyer.fee
to the remote host's current directory, type:
.DM
	mput profits taxes lawyer.fee
.DE
To copy every file with the suffix \fB.c\fR
from the current directory on the remote host into your
current directory on the local host, type:
.DM
	mget *.c
.DE
When
.B ftp
copies files (whether ASCII or binary)
from your local COHERENT host to a remote host,
the files retain their contents.
They may not, however, retain all of their original attributes.
This problem arises because COHERENT has file attributes
that do not correspond to the attributes assigned on other
operating systems.
For example, if you
copy your COHERENT files to
a remote VMS host system and then back to your local COHERENT host,
.B ftp
cannot guarantee that the returned files will
have all of their original attributes.
.PP
Even between two COHERENT systems, there is the possibility that file
attributes will be lost during file transfers.
For example, an executable file that is
.BR ftp 'd
from one COHERENT system
to another will not have execute status on the remote system.
.PP
.B ftp
does not have any way to restoring file attributes lost during transfer.
However, you can use the COHERENT commands
.BR chmod ,
.BR chown ,
and
.B chgrp
to restore the attributes of all files copied onto your local host.
See these commands' entries
in the COHERENT manual's Lexicon for details on how to use them.
.Sh "!:  Give a Command to the Local Shell"
.PP
To issue a command to your local COHERENT system while
connected to a remote host, use the command
.B !
followed by the command you want to execute.
For example, to delete the local file
.BR pcnotes ,
type:
.DM
	ftp>> !rm pcnotes
.DE
When the command has finished executing, you automatically return to
.BR ftp .
.PP
If you type
.B !
without a command,
.B ftp
spawns a new shell on your local host.
You can issue multiple commands to your local COHERENT system, while
.B ftp
maintains your connection to the remote host.
When you exit from the shell,
you return to the
.B ftp
session.
.PP
Note that when you use
.B !
to create a new shell,
any directory operations you perform do not affect your current directory in
.BR ftp .
When you return to
.BR ftp ,
your current directory returns to being what it was before you issued the
.B !
command.
.Sh "help and ?:  Get Help"
.PP
The command
.B help
displays information about an
.B ftp
command.
For example, if you type
.DM
	help mput
.DE
.B ftp
prints a message on how to use
.BR mput .
.PP
If you do not name a command,
.B ftp
simply lists its commands.
.PP
The command
.B ?
is a synonym for
.BR help .
.Sh "status:  Check the Status of ftp"
.PP
The command
.B status
displays the current settings of the
.B ftp
client.
For example,
if you have not changed any of the default settings and you are
connected to remote host
.BR whouse ,
.B status
would display the following:
.DM
	Connected to whouse.
	Mode:stream; Type:ascii; Form:non-print; Structure:file;
	Verbose:on; Bell:off; Prompting:on; Globbing:on;
	Hash-mark printing:off; Use of PORT cmds:on
.DE
As shown,
.B ftp
has various settings that you can toggle on or off.
To toggle a setting, simply issue the appropriate command.
For example, to turn verbose mode off (when you do not
want to see any server messages), issue the verbose command.
.PP
Please note that
.B ftp
allows you to change the file-transfer type
(by using the commands \fBascii\fR and \fBbinary\fR, as described above);
.B ftp
does
.I not
let you change the settings
.BR mode ,
.BR form ,
and
.B structure
because they define how
.B ftp
transmits data.
The mode
.B stream
transmits all data as a stream of bytes,
instead of as a block or as compressed data.
The format
.B non-print
specifies that
.B ftp
should not interpret the vertical-format information
that normally are used when a file is printed.
This format is designed for files that are processed or just stored using
.BR ftp .
The file-structure setting determines that a file in
.B ftp
will have no internal structure (instead of a record or page structure)
and will be considered a continuous stream of bytes.
.Sh "close, bye, and quit:  Close a Connection and Exit from ftp"
.PP
The command
.B close
closes a connection to a particular host but does not end your
.B ftp
session.
When you have closed a connection, you are free either to exit from
.BR ftp ,
or to
.B open
a connection with another remote host.
Note that you cannot
.B open
a connection to a new remote host until you have
.BR close d
the connection to the host to which you are currently connected.
.PP
To exit from
.BR ftp ,
type the command
.BR bye .
This command closes
the connection with the remote host, and return you to the COHERENT shell.
The command
.B quit
is a synonym for
.BR bye .
.Sh "Interpreting ftp Server Messages"
.PP
During the course of an
.B ftp
session, you will receive messages from both the
.B ftp
client (which is running on your local host), and the
.B ftp
server (which is running on the remote host).
Messages that consist of a three-digit number followed by text
come from the server.
Although most messages are self-explanatory, you should find it helpful to
know what the three-digit numbers mean.
.PP
Although each digit has its own meaning, the first digit is the most
important.
The following explains what each first digit signifies:
.IP \fB1\fR 0.3i
A positive preliminary reply.
This means that probably you will receive another message before you can
issue another command.
An example is the message
.DM
	150 Opening data connection
.DE
that appears after you issue the commands
.BR dir ,
.BR get ,
or
.BR put .
.IP \fB2\fR
The
.B ftp
server succeeded in carrying out your command
and you are free to issue a new command.
An example is the message:
.DM
	200 PORT command okay
.DE
.IP \fB3\fR
Your command is fine, but the
.B ftp
server needs more information before it can execute the command.
An example is the message:
.DM
	331 Password required for hillary
.DE
.IP \fB4\fR
The
.B ftp
server did not accept your command,
but that this error condition is not permanent:
you can try again later.
An example is the message:
.DM
	425 Can't build data connection
.DE
.IP \fB5\fR
A permanent error condition:
you probably made a mistake when specifying the command or answering a prompt.
An example is the message:
.DM
	530 Login incorrect
.DE
.Sh "Sample Session"
.PP
In the following session, user
.B beowulf
wants to connect to a remote host called
.BR denmark .
User input appears in
.IR italic ;
system output appears in Roman.
.DS
	\fIftp denmark\fR
	Connected to denmark.
	220 denmark FTP server (Version 1.00 Tues Oct 1600:32:21 EDT 1986) ready
	Name (denmark:beowulf):
	331 Password required for beowulf.
	Password (denmark:beowulf): \fIpassword\fR
	230 User beowulf logged in.
.DE
.DS
	ftp>> \fIpwd\fR
	250 /usr/beowulf is current directory.
.DE
.DS
	ftp>> \fIcd heorot\fR
	200 CWD okay.
.DE
.DS
	ftp>> \fIpwd\fR
	250 /usr/beowulf/heorot is current directory.
.DE
.DS
	ftp>> \fIls\fR
	200 PORT command okay.
	150 Opening data connection for /bin/ls	(89.0.0.3,1216)
	grendel
	grendelsmom
	hrothgar
	hrunting
	hunferth
	226 Transfer complete.
.DE
.DS
	ftp>> \fIget grendel\fR
	150 Opening data connection for grendel (129.0.0.34,1124)
	226 Transfer complete.
.DE
.DS
	ftp>> \fIclose\fR
	221 Goodbye.
	ftp>>
.DE
.SH "rcpy:  Copy Files Among Remote Hosts"
.PP
The command
.B rcpy
lets you copy one file, multiple files, or entire directories
among hosts on which you have login accounts.
.PP
For you to use this command, the
remote host
must be running the
.B rshl
server.
You must have an account on the remote host,
and the remote host must name your local host in its copy of the file
.BR /etc/hosts.equiv .
If your login on the remote host does not have the same name as your
login on your local host, then the file
.B \&.rhosts
in your home directory on the remote host must name your local host and
your login identifier on the local host.
This is described more fully below.
.PP
.B rcpy
has a number of advantages over
.BR ftp .
For example, it can handle third-party transfers:
that is, it can transfer a file from one remote host to another, without
having to use your local host as a go-between.
It does not require that you log into the system to which you are
copying files, nor does it have
.BR ftp 's
long list of commands.
However,
.B rcpy
does not confirm a successful transfer \(em it assumes you will do so
on your own.
.Sh "How to Invoke rcpy"
.PP
To invoke
.BR rcpy ,
type the command followed by the names of the source file and the
destination file.
Each argument specifies a file's name and host, in the form:
.DM
	host:filename
.DE
If you omit
.IR host: ,
.B rcpy
assumes that you mean the local host.
Remote file names are interpreted relative to your home directory on
the remote host; to override this default, use full path names.
You should not use a literal colon in a file name,
because
.B rcpy
will think that the colon is the separator between the host and file names.
.PP
For example, to copy file
.B hrunting
from your home directory on host
.B denmark
to file
.B sword
in your home directory on host
.BR sweden ,
use the command:
.DM
	rcpy denmark:hrunting sweden:sword
.DE
If
.B sword
already exists in your home directory on host
.BR sweden ,
.BR rcpy
overwrites it.
.Sh "Copying Directories and Multiple Files"
.PP
To copy an entire directory, issue
.B rcpy
with the option
.B \-r
(recursion), followed by arguments that name the source and
target directories.
When you specify
.BR \-r ,
.B rcpy
traverses the source directory and transfers all files and subdirectories.
.PP
For example, to copy the directory
.B /usr/siegfried/dragon
on system
.B nibelung
to your home directory on system
.BR sweden ,
type the command:
.DM
	rcpy -r nibelung:/usr/siegfried/dragon sweden:
.DE
.PP
You can also use the option
.B \-r
to copy multiple individual files.
For example, to copy files
.B sword
and
.B shield
into your home directory on host
.BR sweden ,
type:
.DM
	rcpy -r sword shield sweden:
.DE
As with all COHERENT commands, you can use wildcards on the
.B rcpy
command line to name multiple files.
If a file name contains a literal wildcard character, be sure to
quote it before you name it on the
.B rcpy
command line.
For information on wildcards, see the entry
.B wildcards
in the COHERENT manual's Lexicon; and for information quoting wildcards, see
the entries for
.B sh
and
.B ksh
again in the COHERENT manual's Lexicon
.Sh "Specifying a Different Remote User name"
.PP
By default,
.B rcpy
assumes that you have the same login identifier on all of the
hosts among which you are copying files.
However,
.B rcpy
permits you to copy files even if your login identifiers differ.
For this mechanism to work, your local host and local login identifier must
appear in the file
.B \&.rhosts
in your home directory on the remote host.
.PP
For example, assume that on the local host
.B lepanto
your login identifier is
.BR fred ,
whereas on the remote host
.B mwc
your login identifier is
.BR fwb .
In your home directory on
.BR mwc ,
you should have a file \fB.rhosts\fR, which should contain the entry:
.DM
	lepanto	fred
.DE
You can use this mechanism to allow other users to masquerade as yourself
when they communicate with a host;
however, such a person assumes all of your privileges on that host, so you
would be wise not to do that unless you absolutely had to.
.PP
To transfer to a host on which you have a different login identifier,
use the form
.DM
	host.ruser:filename
.DE
for file arguments, where
.I ruser
is your name on the remote host.
.PP
To extend the above example, to transfer file
.B docfiles
from your local system to your home directory on system
.BR mwc ,
use the command:
.DM
	rcpy docfiles mwc.fwb:docfiles
.DE
.Sh "Error Messages"
.PP
Under certain conditions,
.B rcpy
may not be able to connect with a remote host.
An attempt to connect will fail if, for example,
the remote host is not operating
or if remote host is not running the server
.BR rshl .
.B rcpy
tries to make the connection for a reasonable period of time,
then sends you the error message
.DM
	Connection timed out.
.DE
and quits.
.PP
You will also get an error message if you specify an
invalid host name or make a typing error in the host name.
.SH "rlogin:  Log into a Remote Host"
.PP
The command
.B rlogin
lets you log into a remote host as if you were directly connected to it.
When you invoke
.BR rlogin ,
the client connects with the
.B rlogind
server that is running on the remote host.
.PP
.B rlogin
makes certain assumptions about the remote host, and observes
various login conventions.
For example, if the remote host identifies your local host in its file
.BR /etc/hosts.equiv ,
.B rlogin
automatically uses your local user name to identify you to the remote host.
.PP
When you use
.B rlogin
to log into a remote host, that remote host executes your
.B \&.profile
automatically, just as if you had logged directly into that host.
Before you use
.B rlogin
to log into a remote host, make sure that your account on the remote system
will support your connection into your local host \(em for example, that the
terminal type is set correctly.
.Sh "How to Invoke rlogin"
.PP
If your local system shares user names with the
remote host to which you want to connect, invoke
.B rlogin
from the COHERENT shell:
type
.B rlogin
followed by the host name of the remote system.
.PP
Assume, for example, that a user called
.B beowulf
has accounts under that name on two hosts,
.B denmark
and
.BR geatland .
He is logged in locally to
.BR geatland ,
and wants to log in to the remote host
.BR denmark .
If
.B denmark
names
.B geatland
in its file
.BR /etc/hosts.equiv ,
.B beowulf
can connect to
.B denmark
by issuing the following
.B rlogin
command:
.DM
	rlogin denmark
.DE
If
.B rlogin
can make the connection,
.B beowulf
is logged into
.BR denmark ;
depending on the configuration of the remote host, he may be prompted for
the password to his account on the remote host.
When the login process completes, he gets the shell prompt from
.B denmark
and can proceed as if he were directly connected.
.Sh "Specifying a Different Remote User Name"
.PP
If you want to log into the remote host under a different user name,
use the option
.B \-l
on the
.B rlogin
command line, followed by the name by which you wish to log into the remote
host.
For example, if user
.B beowulf
wants to log into
the remote host
.B denmark
under the account
.BR hygelac ,
he gives the
.B rlogin
command as follows:
.DM
	rlogin denmark -l hygelac
.DE
If
.B rlogin
connects with host
.BR denmark ,
it prompts
.B beowulf
for the password to account
.BR hygelac .
.PP
The process can be even simpler.
The remote user can maintain in his login directory a file called
.B \&.rhosts
that names the users on other hosts who can log into his account.
For information on how to use this file, see its entry in the Lexicon
at the back of this manual.
.Sh "Error Messages"
.PP
Under certain conditions,
.B rlogin
may not be able to make the connection.
An attempt to connect fails, for example,
if the remote host is down, or is not running the
.B rlogin
server.
.B rlogin
waits a reasonable period of time before
it quits and sends you the following error message:
.DM
	Connection timed out.
.DE
You also receive an error message if you specify an
invalid host name \(em either by naming a host that is not on the
network, or by misspelling a host's name.
.Sh "Spawning a Local Shell"
.PP
Once you are logged into a remote host, you may
wish to suspend the session to perform local operations.
To do so, type:
.DM
	~!
.DE
This spawns a shell on your local host.
You can issue one or more commands to your local host.
However, if you invoke
.B rlogin
under this new shell, you must break that connection
before you can return to your original
.B rlogin
session.
When you exit from the shell, you return to your
.B rlogin
session.
.Sh "Ending an rlogin Session"
.PP
To close your connection to the remote host, simply log out from that host.
.B rlogin
closes the connection automatically, and displays the message
.DM
	Connection closed.
.DE
on your screen.
.PP
To abort a
.B rlogin
session before you have logged into a remote host,
type the escape sequence:
.DM
	~.
.DE
For a summary of
.BR rlogin 's
capabilities and syntax, see its entry in the Lexicon at the back of this
manual.
.SH "rshl:  Execute a Command on a Remote Host"
.PP
The command
.B rshl
executes a command (or command file) on a remote host that is running the
.B rshl
server.
Its name is a contraction of the phrase ``remote shell'';
as the name implies, it executes a command or script under a shell
on a remote system.
Functionally,
.B rshl
resembles the 4.3BSD utility
.BR rsh ;
it has been renamed to avoid colliding with the UNIX System-V utility
.BR rsh ,
which implements a restricted shell.
.PP
To execute a command on a remote host, invoke the
.B rshl
client on your local host and establish a connection with the
.B rshl
server on the remote host.
Because
.B rshl
performs the function
.B rlogin
when you do not specify a command to execute, any
host to which you want to connect should be running the
.B rlogin
server as well as the
.B rshl
server.
.PP
Like
.BR rlogin ,
.B rshl
makes certain assumptions about the remote system and has
various login requirements.
For example, the remote host must identify your local host in the file
.BR /etc/hosts.equiv .
With your local host listed in the remote host's file,
your user identifier on your local host is used
as the remote user name and you do not need a password.
Unlike
.BR rlogin ,
.B rshl
does not prompt you for a password if your local user name does not exist
on the remote host:
it just denies you permission to execute the command.
.Sh "How to Invoke rshl"
.PP
In most circumstances, you will use
.B rshl
to execute a command on a remote host on which you have an account
under the same user name as on your local host.
For example, if your login identifier is
.B fred
on your local host, it should be
.B fred
on the remote host as well.
.PP
In this case, type
.B rshl
followed by the name of the remote system
and the command to be executed on that system.
For example, assume that your login identifier is
.BR fred ,
your local host is named
.BR lepanto ,
and that you want execute script
.B jobs
that resides in your home directory on the remote host
.BR mwc .
Give the
.B rshl
command as follows:
.DM
	rshl mwc cat jobs
.DE
If
.B rshl
can make the connection,
it
.BR cat s
file
.B jobs
and displays the contents onto your screen.
However, if the remote host did not recognize
.B lepanto
as a host with which it shares account names,
it would have denied you permission to execute the command.
.PP
If you invoke
.B rshl
without telling it what command to execute on the remote host,
then it will attempt to log you into that remote host, just as if
you had invoked the command
.BR rlogin .
.PP
.B rshl
sets some restrictions on the commands you can give.
You cannot, for example, specify an interactive command like
.BR vi ;
nor can you run commands for which you have no permission, e.g.,
.BR /etc/shutdown .
.PP
The arguments to the command that you wish to execute on the remote host
may include wildcards.
You must be careful, however, to ensure that the wildcards are quoted so
that they are not expanded by the shell on your local host before they
are passed to the remote host.
To extend the above example,
if you wished to concatenate every file with the suffix
.BR \&.c ,
you would type the command:
.DM
	rshl mwc cat '*.c'
.DE
Enclosing the string
.B *.c
within apostrophes protects it from being expanded by your local shell.
For more information on wildcards, see the entry for
.B wildcards
in the COHERENT Lexicon; for information on quoting, see the COHERENT Lexicon's
entries for
.B ksh
and
.BR sh .
.PP
Finally,
.B rshl
sets file names relative to your home directory on the remote system \(em
just as if you were logged into that system.
To bypass this default, use absolute path names.
.Sh "Specifying a Different Remote User Name"
.PP
If you wish to execute a command on a remote host under a user identifier
that differs from the one you use on the local host,
use the \fB\-l\fR option to
.BR rshl .
Every user controls who can log into his
account from other hosts by means of a file named
.B \&.rhosts
that resides in his home directory.
For information on this file, see the entry for
.B rhosts
in the Lexicon at the back of this manual.
.Sh "Error Messages"
.PP
.B rshl
may not be able to connect with a remote host;
for example,
the remote host may be down, or may not be running the
.B rshl
server.
.B rshl
tries for a reasonable period of time to make the connection,
then quits and sends you the message:
.DM
	Connection timed out.
.DE
You will also get an error message if you name a host that is not on your
network, or if you misspell the name of a host.
.SH "ruptime:  Get Information About Remote Hosts"
.PP
To obtain information about other hosts on the network,
use the command
.BR ruptime .
This command
lists every host on the network that is running the
.B rwho
server, and gives information about its status.
.PP
This example demonstrates the output of
.B ruptime
in its simplest form:
.DM
.ta 0.5i 1.5i 2.5i
	kalamazoo	up	3+02:09, 6 users, load 0.00, 0.00, 0.00
	paris	down	2+23:11
	wobegon	up	4+01:42, 7 users, load 0.00, 0.00, 0.00
	planetx	down	4+05:57
	metropolis	up	4+03:52, 9 users, load 1.10, 0.42, 0.22
.DE
The first column gives the host name.
The next columns indicate whether the host is up or down;
the time that the host has been up or down in \fIdays+hours:minutes\fR;
the number of users logged in;
and load averages.
.PP
Through command options, you can tell
.B ruptime
to display the list of hosts in order of uptime, number of
users, or load averages.
For a fuller description of
.BR ruptime ,
see its entry in the Lexicon at the back of this manual.
.SH "rwho:  Information About Users on Remote Hosts"
.PP
The command
.B rwho
prints the name and other information about
users who are logged into the hosts on your network.
Note that it can return information only about the hosts that
are running an
.B rwho
server.
.PP
By default,
.B rwho
lists users alphabetically by user name, as in this example:
.DM
.ta 0.5i 1.5i 3.0i
	auntie	paris:ttyn0	May 29 10:22
	charlie	paris:ttyn1	May 29 10:58 :01
	donna	gotham:ttyp0	May 29 06:54 :05
	elmer	wobegon:VTA142	May 29 10:36 
	fawn	nome:tty03	May 29 10:24 :02
	gary	wobegn:VTA148:	May 29 12:13
	mack	gotham:ttyp1	May 29 12:21 :09
.DE
The first four columns show the user's name,
the name of the host into which the user is logged,
the name of the port into which the user is logged,
and the date and time that the user logged in.
If a user has not typed at his terminal for one minute or more,
.B rwho
displays this idle time in the last column.
.PP
Normally, the list does not include users who have
been idle for one hour or more.
To tell
.B rwho
to show all users,
regardless of idle time, use the option
.BR \-a .
For a summary of this command's options, see its entry in the Lexicon at the
back of this manual.
.SH "telnet:  Network Terminal Emulator"
.PP
The final section in this chapter introduces the command
.BR telnet .
This command lets you log into a remote host via the network.
Unlike the command
.BR rlogin ,
.B telnet
gives you a virtual terminal through which you can connect
to the remote host as if you were directly connected to that host.
.Sh "How to Invoke telnet"
.PP
To invoke
.B telnet
from within the COHERENT shell,
type the command followed by the name of the host to which you wish to connect.
For example, to connect to a remote host whose name is
.BR scotty ,
type the following command at your shell prompt:
.DM
	telnet scotty
.DE
You can identify the remote host either by its name (as shown above) or by
its Internet address, in standard dot notation.
For example:
.DM
	telnet 89.0.0.2
.DE
When you connect to the remote host,
.B telnet
automatically enters input mode;
this means that it passes all text you type to the remote host.
However, if you issue the
.B telnet
command without an argument, you enter
.BR telnet 's
command mode.
In this mode, you can give commands to
.B telnet
to modify its behavior.
This is described below.
.PP
When
.B telnet
succeeds in connecting with the remote host,
you see the following messages:
.DM
	Trying...
	Connected to scotty.
	Keepalive is disabled
	Escape character is '^]'.
.DE
These messages verify that the connection has been made and identify
the escape character that enables you to move from
.B telnet
input mode to telnet command mode.
(The escape character is described later in this section.)
Note that when you specify an Internet
address, the connected message echoes the address instead of the host name.
.Sh "Error Messages"
.PP
.B telnet
may not be able to make a connection
if the remote host is not up or is not running the
.B telnet
server.
It also fails if you enter an
invalid Internet address, or misspell the name of the remote host.
.B telnet
waits for reasonable period of time,
then sends you the error message:
.DM
	Trying...
	telnet: connection: Connection timed out.
.DE
.B telnet
then automatically enters command mode, as shown by the fact that it
displays the prompt
.BR telnet>> .
To try again to connect to a remote host,
use the
.B telnet
command
.BR open ,
as described below.
.Sh "Input Mode"
.PP
When
.B telnet
connects to the remote host, it automatically enters input mode.
In this mode,
.B telnet
sends all text that you type to the remote host.
In this mode, you must follow the conventions of the remote host's operating
system, just as if you were logged directly into that system.
.Sh "Command Mode"
.PP
.B telnet
also has a command mode.
In this mode, you can issue commands directly to
.BR telnet .
These commands can change its default settings, or tell it to perform
some task or other.
.PP
.B telnet
enters command mode automatically if you invoke the
.B telnet
server without naming a remote host or Internet address.
You can also enter command mode while logged into a remote host,
by typing the
.B telnet
escape character.
(\fBtelnet\fR displays its escape character when you connect to a
remote host; the default is \fB<ctrl-]>\fR.)
When you enter command mode,
.B telnet
displays the prompt
.BR telnet>> ,
which indicates that it is ready to receive commands from you.
.PP
You must type all of
.BR telnet 's
commands in lower-case letters.
You can abbreviate a command as long as the abbreviation is unambiguous;
for example, you can abbreviate the command
.B close
as
.BR cl .
.PP
If you make a mistake when you issue a
.B telnet
command, it displays the message:
.DM
	Invalid command
.DE
If entered command mode by pressing the
.B telnet
escape character, you can return to input mode by pressing
the \fB<Return>\fR key a few times.
If you entered command mode by invoking
.B telnet
without an argument, then you will enter input mode automatically as soon
as you connect to a remote host.
.PP
The following introduces a few of the more important
.B telnet
commands:
.IP \fBopen\fR
Open a connection with a remote system.
If you issue
.B open
without specifying a host name or Internet address,
.B telnet
prompts you for that information.
.IP
For example,
to connect to a remote host called
.BR sesame ,
type the following at the
.B telnet
prompt:
.DM
	open sesame
.DE
.IP
After you log in, enter input mode, as described earlier.
.IP "\fBescape\fR"
Change the escape character.
To use it, type
.BR escape ,
then enter the new character at the prompt.
The character does not appear on the screen when you type it.
After you enter the character and press \fB<Return>\fR,
.B telnet
displays the new escape character.
.IP
The following example changes the escape character to \fB<ctrl-E>\fR:
.DM
	telnet>> escape
	new escape character:	\fB<ctrl-E>\fP
	Escape character is '^E'.
.DE
.IP
Note that \fB<ctrl-E>\fR in the above example is not typed literally.
.IP
Once you change the escape character, you must use
the new character to re-enter command mode.
.IP "\fB!\fR"
Spawn a shell on your local system.
Your
.B telnet
session goes into suspended animation,
and you can give commands directly
to your system via the newly spawned shell.
If you wish, you can even invoke another
.B telnet
session.
Once you have finished working with your shell, type \fBexit\fR to return
the suspended
.B telnet
session.
.IP \fB?\fR
Ask
.B telnet
for help.
When you type a question mark without an argument,
.B telnet
prints a list of its commands.
To obtain information about a given command, type a question mark followed
by that command.
For example, to get information about the command
.BR open ,
type:
.DM
	? open
.DE
.IP
.B telnet
replies:
.DM
	connect to a site
.DE
.Sh "Closing a Connection and Quitting"
.PP
When you are done with your work on the remote host,
log out in the manner acceptable to that host (for example, by typing
\fB<ctrl-D>\fR or
.BR exit ).
The host then breaks the connection and you get the following message:
.DM
	Connection closed by foreign host.
.DE
You then return to COHERENT command level.
.PP
To close the connection and remain in
.BR telnet ,
return to command mode and issue the
.B close
command.
You then get the following message and the
.B telnet
prompt:
.DM
	Connection closed.
	telnet>>
.DE
To connect to a different host, you can then issue another
.B open
command.
Note that you must close your current
connection before you try to establish a new connection.
.PP
If you are in command mode and wish to leave
.B telnet
(and thereby break your connection with the remote host), use the command
.BR quit .
This command closes the
.B telnet
session, and returns you to the COHERENT shell.
Note that you should explicitly log off the remote host before you break
the
.B telnet
session.
A rational operating system will automatically log you off when it
detects that the connection is dropped, but you should not count on this
happening.
.Sh "A Sample telnet Session"
.PP
The following gives a sample
.B telnet
session.
In this session, user
.B hillary
invokes the
.B telnet
client program to log into a remote host called
.BR whouse .
The text in Roman shows what the user typed, the text in
.I italic
shows what
.B telnet
says to the user, and the text in
.B bold
shows what the remote system says to the user:
.DS
	telnet
	\fItelnet>>\fR open whouse
	\fITrying...\fR
	\fIConnected to whouse.\fR
	\fIEscape character is '^]'\fR
.DE
.DS
	\fB4.3BSD (whouse)\fR
	\fBlogin:\fR hillary
	\fBPassword:\fR [password]
	\fBLast login: Mon Aug 22 20:28:15 on ttyh2\fR
	\fB4.3BSD #86: Wed Sept 10 15:39:08 EST 1986\fR
.DE
.DS
.ta 0.5i 2.0i 3.5i
	\fB%\fRls
	\fBcabinet	congress	defense\fR
	\fBeconomy	foreign.policy	health\fR
.DE
.DS
	\fB%\fRmkdir socks
.DE
.DS
.ta 0.5i 1.5i
	\fB% \fR[escape character]
	\fItelnet>>\fR ?
	\fICommands may be abbreviated. Commands are:\fR
	\fIopen 	connect to a site\fR
	\fIclose	close current connection\fR
	\fIquit	exit telnet\fR
	\fI!	spawn local shell\fR
	\fIescape	set escape character\fR
	\fIstatus	print status information\fR
	\fIoptions	toggle viewing of options processing\fR
	\fIcrmod	toggle mapping of received carriage returns\fR
	\fI?	print help information\fR
	\fItelnet>>\fR escape
	\fInew escape character:\fR	<ctrl-E>
	\fIEscape character is '^E'.\fR
.DE
.DS
	\fB%\fRexit
	\fIConnection closed by foreign host.\fR
.DE
.SH "Where to Go From Here"
.PP
As has been noted several times in this chapter,
the Lexicon at the back of this manual summarizes the commands
supported by the NCM implementation of TCP/IP.
