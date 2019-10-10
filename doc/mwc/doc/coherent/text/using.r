.ds TL "Using COHERENT"
.NH "Using the COHERENT System"
.PP
This tutorial introduces the \*(CO system.
It introduces such basic concepts as \fIcommand\fR and \fIfile system,\fR
and walks you through simple exercises to help you gain some familiarity
with the dimensions of \*(CO.
If you are new to \*(CO, you should read through this tutorial first.
Not every section in here will be immediately useful to every user; for
example, a beginner will probably not need to study the section on
system administration, at least at first.
But sooner or later, you will need to work with all of the material in
this tutorial.
.PP
If you are unfamiliar with what an \fIoperating system\fR is, or if
you are unsure how \*(CO differs from other operating systems
(such as \*(MD), turn to the Lexicon article for \*(CO.
There, you will find a brief description of what an operating system is
and what makes \*(CO special.
.PP
Before you can begin to use this tutorial, you must install \*(CO
on your computer.
If you have not yet done so, turn to the Release Notes that came with
this manual and follow the directions in them.
.SH "How Do I Begin?"
.PP
For everyone, there's that first time.
You have installed \*(CO on your computer, you've checked the file system,
mounted all of your file systems, and have gone into multi-user mode.
Now you are sitting in front of your computer and all you see on your
screen is the enigmatic phrase:
.DM
	Coherent 386 login:
.DE
``What,'' you ask yourself, ``do I do now?''
Well, the rest of this section will tell you how to get started with \*(CO.
.Sh "Logging in"
.PP
To begin, you must \fIlog in\fR.
Unlike \*(MD, \*(CO is a multi-user system:
many people can use the same computer.
They can access it either via terminals that you plug into your
computer's serial ports, or via modem.
Each user owns his personal set of files,
his special way of setting up his environment,
his own mailbox, and other things which are special to him alone.
Because many people can use \*(CO, before you begin to work with
\*(CO you must tell it who you are.
This process of identifying yourself to \*(CO is called \fIlogging in\fR.
.II "logging in^definition"
That mysterious prompt
.DM
	Coherent 386 login:
.DE
is \*(CO's way of asking you who you are.
.PP
To log in, type your personal login identifier.
You set this identifier when you installed \*(CO onto your computer.
Most people set their login identifier to their initials or their
first names, usually all in lower-case letters.
Once you type your login identifier,
press the \*(RT key (sometimes labelled \fB<Enter>\fR or \fB<Return>\fR).
If you did not set up a login for yourself during
installation, log in as the superuser \fBroot\fR and add one for yourself.
For information on how to log in as the superuser, see below.
For information on how to add a new user, see the section on \fBAdding
a New User\fR, below, or see the Lexicon article for the command \fBnewusr\fR.
.PP
While you were installing \*(CO on your system,
you were given the option of setting a password for your login identifier.
.II crackers
This is done to stop other users from logging in as you \(em and
to keep ``crackers'' from dialing into your system and vandalizing it.
If you did set a password, after you enter your login identifier \*(CO
prompts you for it with the following prompt:
.DM
	Password:
.DE
Type your password.
Note that \*(CO does \fInot\fR display the password on the screen as you
type it;
this is to prevent bystanders from seeing your password over your
shoulder as you type it.
After you type your password, again type \*(RT.
.PP
If you entered your login identifier and passwords correctly, \*(CO
will display the command prompt:
.DM
	$
.DE
This is \*(CO's way of saying, ``Give me a command, I'm ready to go!''
If you made a mistake while logging in, either with your login
identifier or your password, \*(CO will reply,
.DM
	Login incorrect:  try again.
.DE
and again display its login prompt:
.DM
	Coherent 386 login:
.DE
Try again, until you do manage to log in.
If you have received the `$' prompt, congratulations!
\*(CO is now ready to work with you.
.Sh "Special Terminal Keys"
.PP
.II eol
.II enter
.II linefeed
.II return
The next sections will introduce you to some elementary \*(CO commands.
Before we continue, however, you must first become familiar with a few
special keys on your computer's keyboard, and with the special meanings
they have to the \*(CO system.
.PP
One special key on the keyboard will be used frequently in your work:
the \*(RT key.
As noted above, this key is sometimes labelled \fB<Enter>\fR.
.PP
You must conclude every command you type into \*(CO by pressing \*(RT.
This tells \*(CO that you have finished typing, and that you now want it
to execute your command.
\*(CO will not execute your command until you press this key.
.PP
.II cont
.II "control key"
.II "ctrl key"
Another special key is the \fBcontrol\fR key.
This key is usually labelled \fBCtrl\fR or \fBcntl\fR or \fBcont\fR.
Most terminals place it to the left of the keyboard.
This key is used to send certain special characters.
.PP
The \fBctrl\fR key is like another kind of shift key:
to use it, hold it down while you press another key.
For example,
to send the computer a \fB<ctrl-D>\fR character, hold down the \fBctrl\fR
key, strike the \fBD\fR key, then release both keys.
.PP
Because control characters have no corresponding printable characters,
in this tutorial they will be represented in the form:
.DM
	<ctrl-D>
.DE
for the character \fBctrl-D\fR.
.PP
.II stty
.II <kill>
While you are typing information into the \*(CO system, you can correct what
you type before \*(CO processes it.
Two keys will help you do this.
The first is the \fB<kill>\fR
character, which erases the line entirely and allows you to begin again.
This is usually \fB<ctrl-U>\fR.
.PP
.II <ctrl-H>
.II <erase>
The other key is the \fB<erase>\fR character, normally
.B <ctrl-H>
or the
.B <backspace>
key.
This moves the cursor one character to the left, to erase the most recently
typed character.
.PP
.II <interrupt>
.II "rub out key"
.II "del key"
One more special key is the \fB<interrupt>\fR key.
This key aborts a command before it normally finishes.
By default, \fB<ctrl-C>\fR is the abort key on your keyboard.
.Sh "Try Some COHERENT Commands"
.PP
.II cat
.II <Return>
Now that you've logged in to your \*(CO system, try a few simple
\*(CO commands to get a feel for \*(CO.
Type the following examples just as they are shown, and observe
what \*(CO does in response to each.
Be sure to press \*(RT to end each line.
.PP
The first example uses the command
.BR cat ,
to let you type a small chunk of text and save it in a file.
.DM
	cat >file01
	This is a sample COHERENT file.
	<ctrl-D>
.DE
Remember, don't type \fB<ctrl-D>\fR literally \(em rather, hold down the
\fBctrl\fR key and press `D' at the same time.
.PP
In the above script, the characters
.B cat
tell \*(CO to invoke its concatenation program.
The characters \fB>file01\fR tells \*(CO to write what you type into a file
that you name \fBfile01\fR.
The line
.DM
	This is a sample COHERENT file.
.DE
is the text that \*(CO writes into \fBfile01\fR.
Finally, \fB<ctrl-D>\fR signals \*(CO that you have finished typing.
.PP
Now type:
.DM
	cat file01
.DE
This command again invokes the concatenation program \fBcat\fR, but this
time tell it to print on your screen the contents of \fBfile01\fR, which you
just created.
In reply to your command, \*(CO should print on your screen:
.DM
	This is a sample COHERENT file.
.DE
which is the text you typed in the previous exercise.
.PP
Finally, type the command:
.DM
	lc
.DE
This command lists all of the files that you have in the current directory.
In reply to your command, \*(CO should print on your screen:
.DM
	Files:
	    file01
.DE
which is the file you just created.
(You may see other files as well.)
.PP
Congratulations!
You have just made \*(CO work for you.
.PP
.II lc
To review:
The first command, \fBcat\fR, created a file and filled it with some text.
The second \fBcat\fR command copied the file onto your terminal's screen.
Finally, the command \fBlc\fR printed the name of each of your files.
The following sections of this tutorial describe each of these commands
in more depth.
Each command also has its own entry in the Lexicon, which appears in the
second half of this manual; look there for a full description of each
command, what it does, and how you can use it.
.Sh "Giving Commands to COHERENT"
.PP
Once you have logged into \*(CO, all of its resources are yours to command.
\*(CO's \fIcommands\fR give you control over these resources.
.II command^definition
.PP
Every \*(CO command has the same structure:
the \fIcommand name\fR, which tells \*(CO the command you want it
to execute; and the \fIarguments\fR, which detail what you want the
command to do, how you want it to do it, and to what you want it done.
.PP
.II "commands^first part"
Some commands consist only of the command name, and do not take arguments.
For example, the command
.DM
	lc
.DE
which was introduced in the previous section, has
.B lc
as the first part and prints the names of
all files in the current directory, in columns.
If you have no files, \fBlc\fR prints nothing.
.PP
.II argument
.II parameter
The second part of the command consists of the \fIarguments\fR
given to the command.
(These are also known by the term \fIparameters\fR.)
Arguments are separated from each other by spaces or tab characters.
.PP
.II option
The arguments of the command are further divided into
\fIoptions\fR and \fInames\fR.
\fINames\fR usually name files;
\fIoptions\fR modify the action of the command.
An option is usually prefixed by a hyphen `-'.
.PP
.II cat
.II parameter^name
An example of a \fIname\fR argument is shown in this example of a
\fBcat\fR command:
.DM
	cat file01
.DE
This command types the contents of \fBfile01\fR on your terminal.
The name argument is \fBfile01\fR.
.PP
.II option
.II ls
.II parameter^option
For an example of options, consider the command
.BR ls .
\fBls\fR lists your file names one name per line.
Thus, typing
.DM
	ls
.DE
produces a list of the form:
.DM
	file01
.DE
However, \fBls\fR can tell you more about a file than just its name.
To see additional information about each file, type:
.DM
	ls -l
.DE
The `\-l' option to \fBls\fR prints a ``long'' output, of the following form:
.DM
	-rw-r--r-- 1 \fIyou\fP    17 Sat Aug 15 17:20 file01
.DE
This listing shows the size of the file, the date it was created or last
modified, and its degree of protection.
The letters to the left of the listing give the permissions for the
file; these describe who is allowed to do what to the file.
These are described in detail in the Lexicon articles
for the commands \fBls\fR and \fBchmod\fR.
The other entries on that line respectively name the owner of the file
(in this case, \fIyou\fR); the size of the file, in bytes;
the date and time the file was last modified; and, finally, the file's name.
.PP
As an example of combining an option parameter with a name parameter,
consider the command:
.DM
	ls -l file01
.DE
This invokes the command \fBls\fR, tells it to print a long listing,
and tells it to list only the file \fBfile01\fR.
.PP
As you will see in the following sections, almost all \*(CO commands
have this syntax.
.Sh "help, man, apropos: Help with Commands"
.PP
.II help
The \*(CO system has three commands that give information about
other commands:
\fBhelp\fR, which prints a brief summary of
how to use a command;
\fBman\fR, which prints the full Lexicon entry
for that command on your screen; and
.BR apropos ,
which shows all commands (all Lexicon entries, really) which relate to
a given subject.
.PP
To find out about the \fBhelp\fR command, type
.DM
	help
.DE
by itself, or type:
.DM
	help help
.DE
The latter command tells \fBhelp\fR to print the help entry for the
\fBhelp\fR command itself.
.PP
To get information on the \fBlc\fR command, type:
.DM
	help lc
.DE
You will see something very like the following:
.DM
	lc -- List directory's contents in columnar format
.sp \n(pDu
	lc [ -1abcdfp ] [ directory ...]
.sp \n(pDu
	Options:
	     -1        List files one per line instead of in columns
	     -a        List all files in directory (including `.' and `..')
	     -b        List block-special files only
	     -c        List character-special files only
	     -d        List directories only
	     -f        List regular files only
	     -p        List pipe files only
.sp \n(pDu
	Options can be combined.  If no directory is specified, the current
	directory is used.
.DE
.II man
To obtain detailed information on a command, use the \fBman\fR command.
(\fBman\fR is short for ``manual''.)
As noted above, the \fBman\fR prints on your screen a duplicate of that
command's entry in the Lexicon.
To learn more about the \fBhelp\fR command, type:
.DM
	man help
.DE
If your screen fills with information, \fBman\fR
will wait for you to press the spacebar to continue.
This is to prevent you from missing information should it scroll too fast.
.PP
Finally, the command
.B apropos
print information about all Lexicon articles that are
.I "a propos"
a given topic.
For example, if you want to know what Lexicon articles are
.I "a propos"
the subject of printers, type the command:
.DM
	apropos printer
.DE
\*(CO replies by printing something like the following:
.DM
	chreq	Change priority, lifetime, or printer for a job
	epson	Prepare files for Epson printer
	hp	Prepare files for Hewlett-Packard LaserJet printer
	hpd	Spooler daemon for laser printer
	lp	Spool a job for printing
	lpd	Spooler daemon for line printer
	lpioctl.h	Definitions for line-printer I/O control
	lpr	Spool a job for printing on a dot-matrix printer
	lpshut	Turn off the printer daemon despooler
	lpskip	Abort/restart current job on line printer
	lpstat	Give status of printer or job
	printer	How to attach and run a printer
	prps	Prepare files for PostScript-compatible printer
	route	Show or reset a user's default printer
.DE
Read the summary descriptions of each Lexicon article to see which ones
look promising; then either look them up in this manual, or use the
.B man
command to display them on your screen.
.PP
Our survey of elementary commands will conclude by describing two
important tasks:
how to reboot the computer, and how to log out.
.Sh "Shutting Down COHERENT and Rebooting"
.II "COHERENT^shutting down"
.II "shutting down COHERENT"
.II "COHERENT^rebooting"
.II "rebooting COHERENT"
.PP
Under many operating systems, such as \*(MD, rebooting is as simple
as pressing a couple of keys or cycling power on the computer.
The \*(CO system, however, is a multi-user, multi-tasking operating
system that is more sophisticated than \*(MD or similar operating systems.
\*(CO maintains an elaborate system of internal buffers that are
designed to reduce the frequency with which a program has to read data
from, or write data to, the hard disk.
If you were just to turn the computer off and turn it on again, all of
the data in those buffers would be lost.
At the very least, each user would lose whatever data he was working
with at the time; at worst, the \*(CO file system could be damaged
and files lost.
.PP
For this reason, it is extremely important that you
shut down \*(CO properly.
You \fImust\fR follow these procedures if you want to shut off the
computer, or if you wish to reboot \*(MD.
.PP
To shut down \*(CO, do the following:
.IP \(bu 0.3i
When you see the \*(CO command prompt, type either
.B <ctrl-D>
or the command
.BR exit .
This will log you out of your system.
(Logging out is described in more detail in the following section.)
.IP \(bu 0.3i
When you see the prompt
.DM
	Coherent 386 login:
.DE
type
.BR root ,
to log in as the superuser
.BR root .
\*(CO will ask you for the superuser's password; type the password
that you assigned to the superuser when you installed \*(CO onto your computer.
The Lexicon article on \fBsuperuser\fR describes what the superuser is;
as will later sections of this tutorial.
.IP \(bu
Once you have logged in as the superuser, type the following command:
.DM
	/etc/shutdown halt 0
.DE
As its name implies, this command shuts down the \*(CO system.
The command will ask you if you really, truly wish to shut down \*(CO;
reply `y', for ``yes''.
.IP \(bu
Now, you can turn the computer off.
Or, you can type \fB<ctrl><alt><del>\fR, or press the reset button on
your computer (should it have one).
.IP
After you have rebooted your computer,
just sit back and wait until you receive the \fBCoherent 386 login:\fR prompt
on your screen.
.IP
If you wish to reboot \*(MD, watch the computer:
wait until you see the computer attempting to read from the floppy-disk drive.
At that moment, press the number key that corresponds to the hard-disk
sector on which you stored \*(MD, from 0 to 7.
For example, if \*(MD is kept on partition 2, then press \fB2\fR when
the computer is attempting to read the floppy-disk drive.
Be sure to press the number key that is on the main bank of keys,
\(em \fInot\fR the key on the numeric keypad.
.PP
That's all there is to it.
Shutting down is relatively simple and straightforward;
but if you do not take the time to shut \*(CO down properly,
you will find that you have destroyed some or all of your data.
.PP
By the way, the Lexicon articles on
.B booting
and
.B login
describe in detail the processes of booting and logging into your \*(CO system.
.Sh "Logging Out"
.PP
.II "logging out"
As noted above, \fIlogging in\fR tells \*(CO who you are and that
you wish to work with \*(CO for a while.
When you have finished working with \*(CO, you must tell \*(CO
that you are done for now.
This process is called \fIlogging out\fR.
.PP
.II <ctrl-D>
.II exit
There are two ways to log out.
Each involves typing a special command to the \*(CO prompt.
The first way is to type \fB<ctrl-D>\fR at the \*(CO prompt.
The second is to type the command:
.DM
	exit
.DE
Each of these commands has the same effect:
the \*(CO system flushes all buffers that you ``own'' and prints the prompt
.DM
	Coherent 386 login:
.DE
on your screen.
At this point, you cannot issue any commands to \*(CO; but you
(or someone else) can log into \*(CO from your terminal.
.PP
Please note that logging out is \fInot\fR the same as shutting down \*(CO.
When you shut down \*(CO, you are shutting down the entire system.
When you log out, however, you are simply ceasing to work with \*(CO.
After you log out, however, \*(CO continues to work on its own:
organizing files, exchanging information with other computers via modem,
executing programs for users who have logged in via modem or other terminals,
and in general making itself useful.
If you shut off the computer after you log out, you will damage the
file system, just the same as if you shut it off while you were logged in.
.PP
The following sections in this tutorial will go into \*(CO's commands
in more detail.
All, however, build on the elementary actions presented here:
logging into \*(CO;
issuing commands;
receiving responses from \*(CO;
and logging out.
.SH "Working With Files and Directories"
.PP
.II file
.II directory
The \fIfile\fR and the \fIdirectory\fR are the cornerstones of
the \*(CO system.
Practically everything you do on the system will involve files:
changing files, invoking files, transmitting or receiving files,
filling files up or emptying files out.
And, directories let you organize masses of files into a rational hierarchy.
.PP
This section discusses manipulating files and directories under the
\*(CO system.
It covers the following:
.IP \(bu 0.3i
What \fIfile\fR and \fIdirectory\fR mean to \*(CO
.IP \(bu
Introduces the commands for manipulating files, directories and their contents
.IP \(bu
Discusses more advanced topics,
such as creating and mounting new file systems
.IP \(bu
Tours the \*(CO file system
.PP
This section of the tutorial covers much ground in a relatively brief space.
Readers who are new to personal computers should concentrate on the
earlier sub-sections, which cover elementary topics;
whereas more experienced readers may wish to concentrate on the later
sub-sections, which cover the more technical material.
.Sh "File Names"
.PP
.II file^name
A \fIfile\fR is a mass of electronic impulses that is given a name
and stored on a disk.
Files are given names to make them easy for you to retrieve.
\*(CO has rules about how files can be named, to ensure that each
file's name is unique.
.PP
The following are examples of legal file names:
.DM
	.profile
	File01
	cmd.sh
	file01
	test.c
.DE
.II "case sensitivity^in file names"
.II "lower case^in file names"
File names are generally made up of upper-case and lower-case letters and
numbers.
\*(CO, unlike \*(MD, distinguishes capital letters from lower-case letters;
therefore, to \*(CO the file names \fBFile01\fR and \fBfile01\fR are different.
.PP
Any character can be used to name a file, including a control character.
We recommend, however, that you name files using only
upper- or lower-case alphabetic characters, numerals, and the
punctuation marks `.' or `_'.
.PP
The file name must not be more than 14 characters long.
If you specify a longer name, characters beyond the 14th will be
lopped off and thrown away.
For example, \*(CO regards the file names
.DM
	this_is_very_long_file_name_1
.DE
and
.DM
	this_is_very_long_file_name_2
.DE
as being identical.
.Sh "Introduction to Directories"
.PP
.II directory
A \fIdirectory\fR is a group of files that have been given a name.
Directories let you organize files systematically.
This may not seem important now, but as you work with \*(CO you will
find that you accumulate hundreds, or even thousands, of files; without
system of directories to organize files, you would quickly lose track of
what each file held, and find it nearly impossible to find any given file
within your system.
.PP
Because files are stored within directories, the complete name of a file
actually consists of its name plus the name of the directory in which it
is stored.
This lets \*(CO distinguish files that have the same name but are stored
in different directories.
\*(CO uses the slash character `/' to distinguish a directory name
from a file name; for example, to view the contents of file \fBjunk\fR
in directory \fBtext_files\fR, you would use the command:
.DM
	cat text_files/junk
.DE
This system of naming will be described in full in the next sub-section;
for the moment, just bear in mind that for \*(CO to find a file, you
must tell \*(CO not only the name of the file, but the name of the
directory in which it is kept.
.PP
.II "home directory"
.II directory^home
When you work with \*(CO, you are always ``in'' a directory.
The directory you happen to be ``in'' at any given moment is called the
\fIcurrent directory\fR.
The current directory is the one whose files you are working with
at this moment.
When you type the name of a file and do not mention what directory it is
stored in, \*(CO assumes that the file is kept in the current directory.
\*(CO includes commands that let you shift from one directory to another.
.PP
When you log into \*(CO, \*(CO places you ``in''
a directory that you ``own''.
This directory is called your \fIhome directory\fR.
You control all of the files in your home directory; it is your
``base of operations'' for working within \*(CO.
.Sh "Path Names"
.PP
.II root
.II "path name"
As you may have deduced by now, a directory can contain both
files and other directories.
The directories within a directory may themselves contain
both files and directories; which then may contain other files
and directories; and so on.
.PP
This design of directories branching into other directories,
which in turn branch into still other directories, is called
\fItree structured\fR.
As the tree-metaphor implies, the \*(CO system of directories
has a \fIroot directory,\fR that is, a directory that is not
contained in any other directory but from which all other directories
descend, directly or indirectly.
The name of the root directory is simply:
.DM
	/
.DE
.II usr
One subdirectory of the root directory is called \fBusr\fR.
This subdirectory contains the home directories of all users.
Other common paths for home directories are
.B /u
and
.BR /usr/acct .
To list the names of all user directories, type the command:
.DM
	lc /usr
.DE
If your login name is \fBhenry\fR, then the command
.DM
	lc /usr/henry
.DE
lists the names of the files in your home directory.
Please note that in the argument \fB/usr/henry\fR, the first slash
names the root directory; all subsequent slashes serve simply
to separate one directory name from the next.
.PP
.II "path name"
.II "path name^fully specified"
The name \fB/usr/henry\fR is called a \fIpath name\fR.
The term ``path name'' means the full name of a given file or
directory \(em including all the directories that lead from the
root directory to it.
.PP
Path names may be full or partial.
All full path names begin with \fB/\fR
for root, and continue with further subdirectory names.
Path names that do not begin with a slash are partial;
\*(CO automatically
prefixes them with the path name of the current
directory to make them complete before it uses them.
.PP
.II "slash^in path name"
The elements of path names are separated by slashes, so if there were a file in
\fBnewdirectory\fR named \fBnewfile\fR, you would refer to it as
.DM
	newdirectory/newfile
.DE
.II "home directory"
.II directory^home
The absence of a beginning slash indicates that the path name begins in the
current directory.
Thus, if your home directory name is \fBhenry\fR, then another
way to name the path to \fBnewfile\fR is to type:
.DM
	/usr/henry/newdirectory/newfile
.DE
The following diagram gives a rough description of the structure of the
\*(CO file system:
.ie p .PH 1 1 \\*(XD/using.eps
.el \{
.DM
                   /
          ==================
         bin              usr
                  ==================
                henry            other
.DE
\}
Please note that unlike a real tree, the root of a tree structure
has its root at the top rather than at the bottom.
Here, the root directory `/' is at the top of the structure.
It contains the directories \fBbin\fR and \fBusr\fR (among many others).
Directory \fBusr\fR contains directories \fBhenry\fR and \fBother\fR
(again, among many others.
These directories can contain many other directories and subdirectories.
.PP
.II root
.II "path name"
.II directory^root
In summary, a path name lists all the subdirectories leading from the
root directory to the file in question.
In the above example, \fBnewfile\fR is a file in subdirectory
\fBnewdirectory\fR, which in turn is a file in the home directory \fBhenry\fR,
which is further a file in the directory \fBusr\fR.
The directory \fBusr\fR
is a file in the master or \fBroot\fR directory for the system.
.PP
You don't need to specify all of this, fortunately, whenever you want to
specify a file in a subdirectory.
\*(CO assumes that
partially specified path names are within the current directory.
Therefore, you can specify a subdirectory by specifying the name of the
directory first, followed by the rest of the path name.
.PP
\*(CO also allows two special abbreviations for directories.
The abbreviation  `..' always represents the current directory's \fIparent\fR
directory.
In the case of the directory \fB/usr/henry\fR, directory \fBusr\fR
is the parent of directory \fBhenry\fR.
In other words, `..' stands for the directory in which the current directory
resides.
Every directory in the system except the root directory has a parent.
For the root directory, `..' refers to itself.
.PP
.II "current directory"
.II directory^current
Another
directory abbreviation is \*(Ql.\*(Qr, which means the \fIcurrent\fR directory.
.PP
The following sub-sections describe the commands that \*(CO includes
for manipulating files and directories.
As you work with \*(CO, you will use these commands continually, so it
would be worth your while to spend a little time learning them.
.Sh "ls, lc: Listing Your Directory"
.PP
.II lc
.II ls
This sub-section introduces two of the more commonly used commands:
\fBls\fR and \fBlc\fR.
Both
.B ls
and
.B lc
list the files in a directory.
.PP
To see how these commands work,
presume that your directory has the files created in previous sections and
that you did not remove directory \fBnewdirectory\fR.
.II /bin
To list the files in your directory, simply use the command with no parameters:
.DM
	 ls
.DE
This produces a list of files, such as:
.DM
	another
	backup
	doc1
	doc2
	file01
	file02
	newdirectory
	stuff
.DE
The command
.B lc
also lists file names, but it prints the files and directories separately,
in columns across the screen.
For example, typing
.DM
	lc
.DE
gives something of the form:
.DM
	Directories:
	   backup newdirectory
	Files:
	   another doc1 doc2 file01 file02
	   stuff
.DE
If you want to list files in a directory other than your own,
name that directory as an argument to the command.
For example, \fB/bin\fR
is a directory in the \*(CO system that contains commands.
Type
.DM
	lc /bin
.DE
and \fBlc\fR will print the contents of \fB/bin\fR.
.PP
.II options
Both
.B ls
and
.B lc
can take options.
An option is indicated by a hyphen `-'.
The option must appear before any other argument.
For example, to list only the files in the directory for user \fBcarol\fR,
leaving out any directories, use the \fBf\fR option with \fBlc\fR:
.DM
	lc -f /usr/carol
.DE
Or, if you type the command
.DM
	lc -f
.DE
the \*(CO system prints all of the files in the current directory.
The following gives the commonly used options to the command \fBlc\fR:
.DS
	\fB-d\fR	List directories only, omitting files
	\fB-f\fR	List files only, omitting directories
	\fB-1\fR	List files in single-column format
.DE
\fBls\fR produces a list of file names, one per line,
and optionally much more information.
To produce all the information, use the \fB-l\fR option
(note that this is an \*(QLel\*(QR, not a numeral 1):
.DM
	ls -l
.DE
The following gives a sample of the long list that this option produces.
Headings have been added to show the meaning of each column:
.II mode
.DM
.ta 0.9i 1.1i 2.2i 2.75i 3.7i 4.3i
\fI			Size,	      Modification
   Mode	#	Owner	Bytes	Date	Time	Name\fP
-rw-r--r--	1	you	17	Wed Aug 19	17:51	file01
drwxrwxrwx	2	you	32	Wed Aug 19	17:53	backup
-rw-r--r--	1	you	17	Wed Aug 19	17:53	doc1
.DE
The meaning of each column will be explained later.
For now, note that the last column gives the name of each file,
and the fourth column from the left gives the size of each file, in bytes.
.Sh "cat: Print Contents of a File"
.PP
.II cat
The command
.B cat
opens and prints the contents of a text file \(em that is, a
file of source code, a document, or a message file.
For example, to list the contents of file \fBfile01\fR, type:
.DM
	cat file01
.DE
.II "standard output"
This command types the file's contents on the terminal
(sometimes also called the \fIstandard output\fR\^).
.PP
.II redirection
.II file^concatenation
Another use for \fBcat\fR \(em the use from which it gets its
name \(em is to \fIconcatenate\fR several files on the standard output.
For example, the command
.DM
	cat one two three
.DE
prints the files \fBone\fR, \fBtwo\fR, and \fBthree\fR,
one after the other, on your screen.
.PP
.II "I/O redirection"
.II >
You can use
.B cat
to concatenate several files into one file by
\fIredirecting\fR the standard output into a file.
The special character `>' tells \*(CO
to redirect the standard output into a file.
For example, the command
.DM
	cat one two three >four
.DE
concatenates files \fBone two three\fR into file \fBfour\fR.
\fBfour\fR need not exist prior to this command; if it does,
its contents are replaced by the data redirected into it.
.PP
Redirection is a very useful feature of \*(CO that will be used
through the rest of this tutorial.
The `>' operator also gives an example of the set of operators that
can be used with \*(CO commands.
These operators, which increase the power of each \*(CO command,
will be described in detail later in this tutorial.
.Sh "more: List Files on the Screen"
.PP
.II more
If the file you list with \fBcat\fR
is more than 24 lines long,
the beginning lines of the file scroll off the
screen too quickly for you to read them.
To ensure that you see all of the lines in the file, use the
command \fBmore\fR.
.PP
.B more
prints a file in 24-line chunks.
After it has listed a chunk of text, it pauses
and waits for you to press \fB<space>\fR.
If you call \fBmore\fR with an option of \fB-s\fR,
.DM
	more -s file
.DE
it will skip all blank lines that are in the text file.
.Sh "mkdir: Create a Directory"
.PP
.II mkdir
The command
.B mkdir
creates a new directory.
For example,
to create a new directory named \fBnewdirectory\fR, type the following command:
.DM
	mkdir newdirectory
.DE
If you follow this command with \fBlc\fR, it lists your regular files,
but it also lists \fBnewdirectory\fR separately as a directory:
.DM
	Directories:
	    newdirectory
	Files:
	    file01	 file02
.DE
To refer to any files in
.BR newdirectory ,
use its name in specifying the path name.
.PP
.II file^creation
Now, create a file in the new directory:
.DM
	cat >newdirectory/newfile
	lines to be
	contained in newfile
	<ctrl-D>
.DE
This command copies lines to the file described by the partial path name
\fBnewdirectory/newfile\fR.
.Sh "cd: Change Directory"
.PP
.II cd
The command
.B cd
changes the current working directory.
For example, the command
.DM
	cd newdirectory
.DE
moves you into directory
.B newdirectory
that you created in the previous sub-section.
Now, if you type the command \fBlc\fR, to show the contents of the current
directory, it will show the following:
.DM
	Files:
	    newfile
.DE
To return to the previous directory, use the command:
.DM
	cd ..
.DE
As noted earlier, the abbreviation `..' always indicates the current
directory's parent directory.
.Sh "pwd: Print Working Directory"
.PP
.II pwd
The command
.B pwd
prints the name of the current, or \fIworking,\fR directory.
For example, if your login name is \fBhenry\fR, then if you type
.DM
	pwd
.DE
you will see:
.DM
	/usr/henry
.DE
Now, use the \fBcd\fR command to switch to directory \fBnewdirectory\fR,
as follows:
.DM
	cd newdirectory
.DE
When you type
.DM
	pwd
.DE
you will see:
.DM
	/usr/henry/newdirectory
.DE
Finally, use the \fBcd\fR command to return to the previous directory,
as follows:
.DM
	cd ..
.DE
When you type
.DM
	pwd
.DE
you now see:
.DM
	/usr/henry
.DE
If you are ever unsure what directory you are in, use the
.B pwd
command.
.Sh "mv, cp: Move and Copy Files"
.PP
.II mv
.II "move files"
.II "file^move"
.II "rename files"
.II "file^rename"
The command
.B mv
moves files.
You can move a file from one name to another within the current directory
(in effect rename the file), or you can move a file from one directory
to another.
.B mv
takes two parameters:
the first names the file to be moved;
the second names either the new name that you are giving to the file,
or the directory into which you are moving the file.
.PP
For example, to move file \fBfile01\fR into directory \fBnewdirectory\fR, type:
.DM
	mv file01 newdirectory
.DE
To see where \fBfile01\fR is now, type the following command:
.DM
	lc newdirectory
.DE
The result is:
.DM
	Files:
	    newfile
.DE
To move \fBnewfile\fR back into the current directory, use the command:
.DM
	mv newdirectory/newfile .
.DE
Remember, the abbreviation `.' always stands for the current directory.
.PP
As noted above, the \fBmv\fR command can also be used to rename files
within the current directory.
For example, to change the name of \fBnewfile\fR to \fBoldfile\fR, use
the following command:
.DM
	mv newfile oldfile
.DE
If the current directory already has a file named \fBoldfile\fR, it will
be thrown away and replaced with the file that used to be named \fBnewfile\fR.
.PP
.II cp
.II file^copying
The command
.B cp
copies a file.
This command has two parameters:
the first names the file to be copied, and
the second names the file or directory into which it is to be copied.
For example, to copy \fBoldfile\fR in the current directory back into
\fBnewfile\fR, use the following command:
.DM
	cp oldfile newfile
.DE
If \fBnewfile\fR already exists, it will be replaced by a copy of
.BR oldfile .
.PP
If you wished to copy \fBnewfile\fR into directory \fBnewdirectory\fR,
use the command:
.DM
	cp newfile newdirectory
.DE
Now, when you type the command
.DM
	lc newdirectory
.DE
you will see:
.DM
	Files:
	    newfile
.DE
As you can see, \fBnewfile\fR has been copied into \fBnewdirectory\fR.
If \fBnewdirectory\fR had already contained a file called \fBnewfile\fR,
that file would have been replaced with the newer \fBnewfile\fR being
copied into \fBnewdirectory\fR.
.PP
The following example summarizes what's been presented so far about
files and directories.
For purposes of the example, assume that your login name is \fBhenry\fR,
and that you have in your home directory files \fBdoc1\fR and \fBdoc2\fR
that you wish to back up for safekeeping.
.PP
Before you can back up these files, you must first create them.
First, use the command \fBcat\fR to create file \fBfile01\fR, as follows:
.DM
	cat >doc1
	a few
	lines of
	text
	<ctrl-D>
.DE
Likewise, create file \fBdoc2\fR:
.DM
	cat >doc2
	second file
	with some text
	<ctrl-D>
.DE
(Don't forget that \fB<ctrl-D>\fR
means to hold the control key down and simultaneously type \fBD\fR.)
.PP
The command \fBlc\fR will now show you the files and directories in your
current directory:
.DM
	Directories:
	    newdirectory
	Files:
	    doc1    doc2    newfile    oldfile
.DE
The next step is to create the directory to hold the back-up copies.
To help remind yourself what the directory is for, name it \fBbackup\fR.
.DM
	mkdir backup
.DE
Now, \fBlc\fR shows you:
.DM
	Directories:
	    backup	 newdirectory
	Files:
	    doc1    doc2    newfile    oldfile
.DE
The next step is to use \fBcp\fR to copy your files into \fBbackup\fR:
.DM
	cp doc1 backup
	cp doc2 backup
.DE
After you issue these commands, \fBlc\fR still says:
.DM
	Directories:
	    backup	 newdirectory
	Files:
	    doc1    doc2    newfile    oldfile
.DE
However, if you list the contents of subdirectory \fBbackup\fR
.DM
	lc backup
.DE
you will see:
.DM
	Files:
	    doc1 doc2
.DE
The files have been successfully copied into the back-up directory.
.PP
For a full description of these commands and the options available with each,
see their respective entries in the Lexicon.
.Sh "rm, rmdir: Remove Files and Directories"
.PP
.II rm
.II "file^removal of"
The command
.B rm
removes a file.
For example, if you wish to remove file
.B doc2
in directory
.BR backup ,
type the following command:
.DM
	rm backup/doc2
.DE
After typing this command, use the command \fBlc\fR to show the contents
of directory \fBbackup\fR, as follows:
.DM
	lc backup
.DE
You should see:
.DM
	Files:
	    doc1
.DE
As you can see, file \fBdoc2\fR has been removed.
.PP
You can remove several files at once, simply by listing them on the
.B rm
command's command line.
For example:
.DM
	rm file01 file02
.DE
removes files
.B file01
and
.BR file02 .
.PP
Note that once you remove a file with \fBrm\fR, it is gone forever.
The \*(CO system does not warn you if you \fBrm\fR several files
at once; it will assume that you know what you're doing and carry out
your command silently.
For this reason, be careful when you use the \fBrm\fR command, or you
may receive a rude surprise.
.PP
.II rmdir
.II "directory^removing"
.II "remove a directory"
You cannot use the command
.B rm
to remove a directory.
\*(CO does this to help prevent you from wiping out an entire
file system with one simple \fBrm\fR command.
To remove a directory, use the command
.BR rmdir .
For example, to remove the directory
.BR newdirectory ,
type:
.DM
	rmdir newdirectory
.DE
Note that before you can delete a directory, that directory must not
have any files or directories in it.
If you try to remove a directory that has files or directories in it,
\*(CO will print an error message on your screen and refuse to
remove the directory.
.PP
For a full description of these commands and the options available with each,
see their respective entries in the Lexicon.
.Sh "du, df: How Much Space?"
.PP
Files occupy space on your hard disk.
.II "Parkinson's law"
(A corollary to Parkinson's law states that files expand
to fill the disk allotted to them.)
It is somewhat disconcerting to
attempt to save a large file, only to find that you have run out of disk space.
To help you manage your hard disk, \*(CO includes the commands
.B du
and
.BR df .
.PP
.II du
.II "disk usage"
The disk-usage command
.B du
tells you how much disk space the files in the current directory
occupy.
If the directory has sub-directories, these are listed separately.
.II disk^block
.II "block, disk"
.B du
prints disk usage in blocks; each block is 512 bytes (half a kilobyte).
.PP
.II df
The disk-free command
.B df
tells you how many blocks are left free on your disk.
By default it prints information only about the file system you are now in.
.PP
If you find that you are running low on disk space, you must free up some
space.
You can do that by removing files you no longer need; by \fIcompressing\fR
files that you do not use often; or by backing files up to floppy disk and
then removing them.
We have already described how to remove files.
Look in the Lexicon entry for the command \fBcompress\fR for information on
how to compress and uncompress files.
Following sections in this tutorial will describe how to copy files to floppy
disk.
.PP
For more information on these commands, see their respective entries in the
Lexicon.
.Sh "ln: Link Files"
.PP
.II links
\*(CO allows a file to have more than one name.
When you create a file, you give it a name; \*(CO \fIlinks\fR the
name you give the file with its internal system of managing files.
(For more information on how \*(CO identifies files, see the Lexicon
entry for \fBi-node\fR.)
\*(CO allows you to give a file more than one name; another way of
expressing this is to say that
you can give a file \fImultiple links\fR.
.PP
.II ln
.II file^links
To create a new link to an existing file, use the command \fBln\fR.
This command takes two arguments:
the first names the file to which you wish to give a new link, and the
second gives the name that you wish to link to that file.
If the name you are linking to a file is already being used by a
file, \*(CO will not let you link the file to that name.
.PP
For example to link the file \fBdoc1\fR to the name \fBanother\fR,
use the following command:
.DM
	ln doc1 another
.DE
The ``new'' file has the same data in it as the ``old'' file;
in fact, the names \fBdoc1\fR and \fBanother\fR are synonyms for the same file.
.PP
.II rm
.II file^links
The next point is somewhat subtle.
When you use the command \fBrm\fR to remove a file, what you are
actually doing is breaking the link between that file and its name.
The file is not actually removed from disk until all links are broken
between it and all of its names.
In the above example, if you use the command
.DM
	rm another
.DE
to remove the file \fBanother\fR, the file
.B doc1
remains in existence, and the data to which the names \fBanother\fR
and \fBdoc1\fR pointed remains on the disk.
If you then use the command
.DM
	rm doc1
.DE
to remove \fBdoc1\fR, then you will have broken all links between that
file and the \*(CO system, and \*(CO removes it from the disk.
.PP
Links are useful if you wish a file to be used in two different contexts
but have the same data.
For example, if you use file \fBdoc1\fR in two different manuscripts, you can
create links to the file in two different directories, one for each
manuscript.
Thus, any changes you make to the file under either its names appear
automatically in both manuscripts.
.PP
As always, see the Lexicon for a full description of the
.B ln
command.
.Sh "File Permissions"
.PP
As you recall, the command
.B "ls -l"
prints a mass of information about each file.
The following repeats the information that appeared when you
typed \fBls -l\fR:
.DM
.ta 0.9i 1.1i 2.2i 2.75i 3.7i 4.3i
\fI			Size,	      Modification
   Mode	#	Owner	Bytes	Date	Time	Name\fP
-rw-r--r--	1	you	17	Wed Aug 19	17:51	file01
drwxrwxrwx	2	you	32	Wed Aug 19	17:53	backup
-rw-r--r--	1	you	17	Wed Aug 19	17:53	doc1
.DE
Column 3 names the owner; in this example, \fByou\fR represents
your login name, whatever you have set it to.
Column 4 gives the size of the file, in bytes.
Columns 5 through 7 give the day of the week and the date on which
the file was last modified.
Column 8 gives the time the file was last modified or, if the file
was last modified more than a year ago, the year it was last modified.
Column 9 gives the name of the file.
.PP
.II permission^access
.II "access permission"
Column 1 gives the \fImode\fR of the file.
The mode summarizes the \fIpermissions\fR attached to this file.
.PP
Before going further, the concept of file permissions should be reviewed.
\*(CO is a multi-user operating system, which means that more than one
person can log into the system, walk through its file system, execute
commands, and manipulate files.
Every user has files that she ``owns'' \(em that is, that she has created
and that she wishes to protect against being altered or removed by others.
After all, it would be disconcerting if you were to log into your system,
only to find that some of your key files had been trashed by another user,
without your knowledge or permission.
.PP
The \*(CO system protects files by its system of file permissions.
Permissions have two aspects:
the \fItype\fR of permission, and the \fIscope\fR of permission.
There are three \fItypes\fR of permission:
.IP "\fBread permission\fR"
Permission to read a file.
.IP "\fBwrite permission\fR"
Permission to write into a file.
.IP "\fBexecute permission\fR"
Permission to execute a file, assuming that file contains executable
code instead of text.
.PP
Likewise, there are also three types of \fIscope:\fR
.IP "\fBuser\fR"
The permissions extended to the owner of the file.
.IP \fBgroup\fR
The permissions extended to the group of users to which the owner belongs.
For more information on what \fIgroup\fR is, see the Lexicon entry for
.BR group .
.IP \fBother\fR
The permissions extended to all other users.
.PP
The \fImode\fR column describes all permissions attached to a file.
It also gives other information about a file, such as whether the
file is a directory.
Taking the entry for file \fBfile01\fR as an example, we see:
.DM
1 2  3  4   # Owner            Size      Date         Time    File name
-rw-r--r--  1 you         17 Sat Aug 15 17:20  file01
.DE
As you can see, the mode field is divided into four subfields, in this
example labelled `1' through `4'.
.PP
Subfield 1 indicates whether this file is a directory.
If the file were a directory, this
would contain a \fBd\fR; otherwise, it contains a hyphen.
.PP
.II "mode field"
Subfields 2 through 4 describe the type of permission extended to,
respectively, the owner, the owner's group, and other users.
Each subfield consists of three characters.
The first character indicates whether the file is readable; if it is,
then the character is an `r'; otherwise, it's a hyphen.
The second character indicates whether the file is writable; if it is,
then the character is a `w'; otherwise, it's a hyphen.
The third character indicates whether the file is executable; if it is,
then the character is an `x'; otherwise, it's a hyphen.
.PP
In the above example, file \fBfile01\fR has permissions:
.DM
	-rw-r--r--
.DE
These grant read and write permission to its owner,
read permission to the other members of the owner's group,
and read permission to all other users.
.PP
The \*(CO system has a set of default permissions that it applies to
every file when it's created.
To change this default set of permissions, use the command \fBumask\fR.
For information about this command, see its entry in the Lexicon.
To change the permissions of an existing file, use the command \fBchmod\fR,
as described in the following sub-section.
.Sh "chmod: Change File Permissions"
.PP
.II chmod
.II permission^write
.II "write permission"
To change the mode of a file, use the change-mode command \fBchmod\fR.
For example, to protect file \fBdoc1\fR in directory \fBbackup\fR
from being overwritten, use the command:
.DM
	chmod -w backup/doc1
.DE
where the \fB-w\fR means ``remove write permission''
and is followed by the file name.
Henceforth, if you try to write into this file, the \*(CO system
will refuse to do so and will print an error message on your screen.
.PP
.II permission^read
.II "read permission"
To allow other users to read the backup file \fBdoc2\fR, type:
.DM
	chmod o+r backup/doc2
.DE
where the letter \fBo\fR signifies ``other users'', and the \fB+r\fR tells
\fBchmod\fR to grant read permission.
.PP
To see the new set of permissions, type the command:
.DM
	ls -l backup
.DE
As you can see, the mode string has changed from what it was above.
.PP
Directory access permissions are similar to file access permissions
in that they can easily be changed via command
.BR chmod .
However, the permission bits have different meanings for directories.
Permitting reads on a directory allows the user to see
the contents of the directory via commands such as
.B lc
or
.BR ls ;
permitting execution on a directory allows access to the files
in the directory;
and permitting writes on a directory allows the user to create
or delete files in the directory, regardless of the permissions
on the actual file.
The latter causes the most difficulty for new users since
they mistakenly associate file deletion permissions with the
actual file rather than with the directory that contains the file.
.Sh "Creating and Mounting a File System"
.PP
Earlier, we described how the \*(CO system consists of a tree of
directories; and how that tree branches from the root directory `/'.
This is a useful description, and true as far as it goes; but the full
situation is a little more complex.
.PP
The tree of \*(CO directories in fact consists of any number of
\fIfile systems,\fR each of which exists on its own physical device.
A \fIphysical device\fR may be a partition on your hard disk, a floppy
disk, or even a chunk of RAM.
.PP
The \*(CO system contains a suite of commands that let you create
a new file system on a physical device, and graft (or \fImount\fR) that
new file system onto the \*(CO directory tree.
The following few sub-sections will walk you through the steps of creating
a new file system on a floppy disk and mounting it onto your existing
\*(CO directory tree.
These descriptions may be a bit too advanced for beginners; but most users
will find them to be interesting and helpful.
.Sh "fdformat: Format a Floppy Disk"
.PP
The first step in creating our new file system is to format a floppy disk.
The command
.B fdformat
formats a diskette.
When a diskette is formatted, \*(CO writes information on each
track that makes it possible for the floppy disk to hold a file system.
.PP
.B fdformat
uses the following syntax:
.DM
	/etc/fdformat \fIdevice\fR
.DE
where
.I device
is the name of the device to be formatted.
To format a high-density, 5.25-inch diskette, use the command:
.DM
	/etc/fdformat /dev/rfha0
.DE
To format a high-density, 3.5-inch diskette, type:
.DM
	/etc/fdformat /dev/rfva0
.DE
To format a low-density, 5.25-inch diskette, type:
.DM
	/etc/fdformat /dev/rf9a0
.DE
For this example, we'll assume that you have a high-density, 5.25-inch
floppy disk.
Insert into drive 0 (that is, drive A) of your computer, and type the
command:
.DM
	/etc/fdformat -v /dev/rfha0
.DE
The \fB\-v\fR option to \fBfdformat\fR tells it to verify that the disk
is sound.
This option means that the command will take longer to execute, but
in the long run it's worth it as it will ensure that you do not waste
time to trying to copy data onto a flawed disk.
For details on the command
.BR fdformat ,
see its entry in the Lexicon.
.PP
When this command has finished executing, leave the floppy disk in drive 0.
.Sh "mkfs: Create a File System"
.PP
.II mkfs
.II file^prototype
The command
.B mkfs
creates a file system on a physical device.
This command has the following syntax:
.DM
	/etc/mkfs \fIspecial proto\fR
.DE
\fIspecial\fR names the physical device on which the file system is
to be built.
\fIproto\fR is either a number or a file name.
If it is a number, \fBmkfs\fR builds a file system of that size in blocks.
.PP
For our example, type the command:
.DM
	/etc/mkfs /dev/fha0 2400
.DE
This command writes a file system onto device \fB/dev/fha0\fR, which
in this case represents the floppy disk in drive 0 that we just formatted.
The number 2400 represents the number of blocks that fits onto such a disk.
Please note that the above example is for a 5.25-inch, high-density floppy
disk.
For directions on how to create a file system on a floppy disk of different
size or density, see the Lexicon articles for
.B "floppy disks"
or
.BR mkfs .
.PP
.II prototype
If \fIproto\fR is not a number,
.B mkfs
assumes that it is a prototype file.
The command
.B badscan
scans a physical device for bad blocks and writes such a prototype file for
you.
Prototype files are beyond the scope of this example; but for information
on them see the Lexicon entry for \fBbadscan\fR or the Lexicon entry for
\fBfloppy disks\fR.
The latter article summarizes all the ways in which floppy disks are used
by the \*(CO system.
.Sh "mount: Mount a File System"
.PP
.II mount
Now that you have formatted your floppy disk and built a file system
on it, you can \fImount\fR the newly created file system.
\fIMounting\fR grafts this device's file system onto the \*(CO system's
directory tree.
Thereafter, you can write files onto that device, read them, remove them,
or do anything else that you wish with that device and its contents.
.PP
.B mount
has the following syntax:
.DM
	/etc/mount \fIdevice directory\fR
.DE
.I device
names the physical device whose file system is to be mounted.
.I directory
names the \fIbase directory\fR for that file system.
The base directory is the directory by which the file system is accessed.
For example, directory \fB/usr\fR is the base directory for the file system
that holds all users' home directories.
We'll describe base directories a little further in a few paragraphs.
.PP
For purposes of our example, type the following command:
.DM
	/etc/mount /dev/fha0 /f0
.DE
This mounts the file system on the disk in drive 0 onto base directory
\fB/f0\fR.
.PP
The base directory by convention is a directory in the root directory `/'.
You do not have to do this, however.
For example, if your user name was \fBhenry\fR and you wished to mount
the file system on the floppy disk in your home directory, you could type:
.DM
	/etc/mount /dev/fha0 /usr/henry/backup
.DE
This will mount the file system on the floppy disk onto directory
.B /etc/henry
and name its base directory as
.BR backup .
Note that if directory
.B backup
already existed in directory
.BR /usr/henry ,
its contents will be inaccessible until you unmount the file system
on the floppy disk.
Unmounting is discussed in the following sub-section.
.PP
For more information on mounting a file system, see the Lexicon
article \fBmount\fR.
.Sh "Using a Newly Mounted File System"
.PP
Now that you have created and mounted a file system, you can use it like
any other directory.
To see how this works, type the following command:
.DM
	cat >/f0/testfile
	Here's some text we're writing onto the
	newly mounted file system on a floppy disk.
	<ctrl-D>
.DE
Here you can use the \fBcat\fR command to write some text into file
.BR testfile ,
which lives on the floppy disk you just mounted.
To see that this text has been written there, type:
.DM
	cat /f0/textfile
.DE
You should see the floppy-disk drive whirl briefly, and the following
appear on your screen:
.DM
	Here's some text we're writing onto the
	newly mounted file system on a floppy disk.
.DE
You can now use this file system like any other, even though it lives on
a floppy disk rather than your hard disk.
As you can see, this is an easy way to extend the size of your \*(CO
system's file system.
.Sh "umount: Unmount a File System"
.PP
.II umount
Finally, when you have finished working with a file system, you must use
the command
.B umount
to un-mount it.
This command prunes the file system on a given physical
device from the \*(CO system's directory tree.
You will use this command frequently as you use floppy disks.
.PP
.B umount
takes one argument:
the name of the physical device being unmounted.
In our example, the command
.DM
	/etc/umount /dev/fha0
.DE
unmounts the file system on the high-density, 5.25-inch floppy disk
insert into drive 0 (that is, drive A) on your computer.
.PP
Under unsophisticated operating systems like \*(MD, you can insert or
remove floppy disks without giving the matter a second though.
The \*(CO system, however, uses a complex set of buffers to speed
the reading and writing of information to the floppy disk; for this
reason, if you simply yank a floppy disk out of its drive, all of the
information in the \*(CO system's buffers will be lost.
Worse, if you yank out a floppy disk and insert a \*(CO-formatted
floppy disk, the \*(CO system will write the data in its buffers onto
that new floppy disk \(em and probably destroy its file system in the process.
Unmounting a file system tells the \*(CO system to flush all information
in its buffers and write it onto the disk.
.PP
To emphasize this point, please read the following carefully:
.PP
.I
If you mount a floppy disk, you must use the \fBumount\fP command
to unmount it before you remove the disk from its drive.
If you do not, data will be destroyed.
.R
.PP
This concludes the discussion of how to mount create a file system,
mount it, and use it.
See the Lexicon article \fBfloppy disks\fR for further information on
how to do this task.
.PP
The following two sub-sections discuss how to check a file system,
to ensure its integrity.
.Sh "fsck: Check a File System"
.PP
The command
.B fsck
checks a file system, to ensure its integrity.
For example:
.DM
	fsck /dev/root
.DE
where \fB/dev/root\fR is a disk device, checks the file system
located on device \fB/dev/root\fR.
.PP
.II "root^file system"
.II "file system^root"
.II umount
.II fsck
If possible, you should
.B umount
the file system before you check it.
You cannot \fBumount\fR the root file system.
If you can't unmount it, be sure that no other users are on the system
(i.e., that you are in single-user mode),
then reboot the system immediately \fIwithout\fR performing a
.BR sync .
.II sync
If other users are creating or expanding files
while the file systems are being checked,
.B fsck
will report false errors.
.PP
.II i-node^list
If \fBfsck\fR finds any discrepancies,
it writes appropriate messages onto the console (that is, the screen
directly plugged into your computer).
An absence of messages indicates that there are no
problems with the file system.
The appendix to this manual gives all of \fBfsck\fR's error messages,
and suggests how you should respond to each.
.PP
\*(CO's boot routines run \fBfsck\fR automatically, and will rerun it
if necessary to fix problems with the file system.
For more information on \fBfsck\fR, see its entry in the Lexicon.
.Sh "Devices, Files, and Drivers"
.PP
The next few sub-sections introduce the topic of special files and devices.
You brushed this topic in the earlier section that described how to format
and mount a file system on a floppy disk; the following few sections go into
it more systematically.
Beginners will probably find that much of this sub-section is
mystifying, but experienced users and ambitious beginners probably
will find much of value here.
.PP
.II "device-independent I/O"
To begin, the \*(CO system is designed to provide device-independent I/O.
Devices and files are handled in a consistent way.
Each I/O device is represented as a \fIspecial file\fR in directory \fB/dev\fR.
For example, if your system has a line printer device named \fBlp\fR,
you can list a file, named \fBprog\fR for example, on the printer by saying:
.DM
	cat prog >/dev/lp
.DE
Another example is to copy the file \fBprog\fR with the \fBcp\fR command to
your terminal:
.DM
	cp prog /dev/tty
.DE
There are two types of special files represented in
.B /dev ,
and when you list
.B /dev
with \fBlc\fR it will separate them.
.PP
.II "special file^block"
.II "file^block special"
The first type is a \fIblock special\fR file.
This type includes disks and magnetic tape.
These devices are read and written in blocks of 512 bytes,
and can be randomly accessed.
(As a practical note, note that magnetic tape can be
read in a random fashion only by positioning backwards and forwards one
record at a time; disks can be read or written in a totally random fashion.)
.PP
The I/O to and from block devices is buffered to improve overall system
performance.
When a program writes a block of data, the data are held
in a buffer to be written at a later time.
If the same block is read twice in a row,
the data for it is still available in memory and do not
have to be fetched from the physical device.
.PP
.II /etc/update
.II sync
A special program named
.B /etc/update
forces all buffered data to the physical device periodically by calling the
command
.B sync ,
to protect against losing data in the case of an accident,
such as a power failure.
If you must bring the system down, you must force the latest data
to be written by typing the command \fBsync\fR.
.Sh "Character-Special Files"
.PP
.II "special file^block"
.II "file^block special"
.II "file^special character"
The second kind of special file is called a \fIcharacter-special\fR file.
Included in this class are devices that are not block special:
terminals, printers, and so on.
Disks and tapes can also be treated as character special files.
For every block special file for a disk, such as
.DM
	/dev/at0c
.DE
there is usually a character-special file:
.DM
	/dev/rat0c
.DE
.II "raw files"
.II file^raw
.II fsck
.II dump
Character-special files are sometimes called \fIraw\fR files, hence the prefix
\fBr\fR in \fBrat0c\fR.
A raw file has no buffering
or other intermediate processing performed on its information.
This difference is an efficient benefit to commands such as
.B dump
and
.BR fsck ,
which do their own buffering.
.Sh "tty Processing"
.PP
.II terminal^mode
.II "cooked files"
.II files^cooked
One special set of devices has other processing \(em the \fBtty\fR or
terminal files.
A terminal-special file with this special processing is
called a \fIcooked\fR device.
The processing includes handling the \fBkill\fR,
\fBerase\fR, \fBinterrupt\fR, \fBquit\fR, \fBstop\fR, \fBstart\fR, and
\fBend-of-file\fR characters.
Processing can be disabled with the command
.B stty
so the program deals with the raw device.
However, using a raw \fBtty\fR device generally has negative effects
on performance of the \*(CO system.
.Sh "A Tour Through the File System"
.PP
.II "file system^layout"
Our introduction to \*(CO's system of files and directories
concludes with a tour of the \*(CO file system.
Much of this material has been described earlier.
.Sh "General File System Layout"
.PP
.II directory^root
.II root^directory
.II /
The base of the file system is the \fBroot\fR directory, whose name is simply:
.DM
	 /
.DE
Most of the files in the root are directories.
To list the files in the \fBroot\fR directory, type:
.DM
	lc /
.DE
.Sh "/bin"
.PP
.II /bin
Most of the commonly used commands are programs contained in
.BR /bin ,
such as the command \fBlc\fR used in the above example.
Foreign commands, such as \*(ME and
.BR kermit ,
are placed in directory
\fB/usr/bin\fR.
.PP
.II directory^current
.II "current directory"
.II /
.II shell
.II sh
.II PATH
The shell
does not automatically look in \fB/bin\fR for commands, but consults
the variable
.B PATH
to determine where commands are to be found.
A typical value for \fBPATH\fR is:
.DM
	/bin:/usr/bin:.
.DE
This tells the shell to look for commands in three places (in this order):
.BR /bin ,
.BR /usr/bin ,
and finally \fB.\fR, the current directory.
The shell does not consult
.B PATH
if the command contains one or more \fB/\fR characters, indicating a complete
or partial path specification.
.Sh "/dev"
.PP
.II /dev
Devices in the \*(CO system are accessed through files in the directory
.BR /dev .
If there is a line printer available on the system named \fBlp\fR,
you can print characters from a file named \fBtestdata\fR by typing
the command:
.DM
	cat testdata >/dev/lp
.DE
All devices on the system are represented in the \fB/dev\fR directory.
Note that it is not recommended you access devices directly, but use the
\*(CO system's utilities that \fIspool\fR files to them.
This will prevent two users attempting to write material to a device
simultaneously, and so garbling the output.
For example, to access the line-printer device, use the spooler \fBlp\fR.
See the Lexicon's entries on \fBprinter\fR and \fBdevice drivers\fR.
.Sh "/drv"
.PP
.II /drv
A unique feature of the \*(CO system is the concept of loadable
device drivers.
This feature lets \*(CO system programmers write
their own device drivers without modifying the rest of the system.
Drivers can be unloaded, modified,
and reloaded without halting and rebooting the system.
Loadable drivers are kept in directory
.BR /drv .
To load a driver, type:
.II /etc/drvld
.II drvld
.DM
	/etc/drvld /drv/\fIdriver\fR
.DE
where
.I driver
is the driver to load.
See the Lexicon's entry on
.B drvld
for more information.
.Sh "/etc"
.PP
.II /etc
Several commands that you will use in your role as system administrator
are kept in directory
.BR /etc .
These are described in detail elsewhere in this guide.
They include commands for system accounting, booting the system,
mounting the system, create file systems, and control system time.
.PP
.II passwd
.II /etc/passwd
.II /etc/ttys
Also in
.B /etc
are several data files used in system administration.
These include \fB/etc/passwd\fR, the file containing user
names, ids, and passwords; news files; and file
.BR /etc/ttys ,
which describes the properties of each user terminal attached to the system.
.Sh "/lib"
.PP
.II "standard I/O"
.II /lib
The \*(CO system provides
many useful functions for performing input and output (I/O) and
mathematics, for use in your C programs.
These and other libraries, along with the
phases of the C compiler itself, are kept in directory
.BR /lib .
This directory includes files containing standard system calls,
standard I/O, and mathematical routines such as \fBsin\fR, \fBcos\fR,
and \fBlog\fR.
.Sh "/usr"
.PP
.II /usr
The directory
.B /usr
contains user directories, along with a few system directories.
.PP
.II /usr/adm
.B /usr/adm
contains additional information of interest to the system administrator.
.PP
.II /usr/bin
.B /usr/bin
contains commands that were not entirely created by Mark Williams Company.
.PP
.II /usr/games
.B /usr/games
contains computer games.
.II /usr/games/lib/fortunes
.B /usr/games/lib/fortunes
holds a set of \fIbon mots\fR; the game \fBfortune\fR
selects one at random and prints it on your screen.
A call to this game can be placed in a user's
.BR .profile ,
so he will see a new fortune each time that he logs on.
To add fortunes of your own, just edit the file
.BR /usr/games/lib/fortunes .
.PP
.II /usr/include
The directory
.B /usr/include
contains header files for C programs, such as
.BR stdio.h .
Other header files define formats of files and
other important data structures in the system.
.PP
.II "nroff macros"
.II ms
.II man
.II nroff
.II units
.II /usr/lib/crontab
.II cron
.B /usr/lib
contains the macro files
.B ms
and
.B man
used the \fBnroff\fR text processor; the unit conversion tables for the
command
.BR units ;
and the file
.B /usr/lib/crontab
used to hold commands for
.BR cron .
This directory also holds the C libraries.
.PP
.II /usr/man
.B /usr/man
contains manual sections referenced by the commands \fBman\fR
and \fBhelp\fR commands.
.PP
.II /usr/msgs
.B /usr/msgs
stores messages displayed by the command
.BR msgs .
.PP
.II /usr/pub
.B /usr/pub
contains public files, such as telephone numbers and a copy of the ASCII table.
.PP
.II /usr/spool
.B /usr/spool
contains information for line-printer spooling, and
mail that has not yet been delivered.
.Sh "/u"
.PP
.II /u
In some systems, users' directories are placed on a separate device to
save space.
Because a separate device has a separate file system, the
directory on that device is called
.BR /u .
.Sh "Files: Conclusion"
.PP
This concludes this tutorial's discussion of files and directories.
The rest of this tutorial introduces \*(CO's suite of commands,
and discusses topics of special interest to persons who are administering
\*(CO systems.
.SH "Introduction to COHERENT Commands"
.PP
This section introduces \*(CO's commands.
The \*(CO system comes with more than 200 commands, which perform a
variety of work, from formatting text, to editing files, to performing
low-level administration of the system.
The commands that manipulate files and directories were introduced in
the previous section;
there are, however, many other varieties of commands, many of which
will be introduced here.
To begin, we'll introduce the \*(CO system's master command, the
.IR shell .
.Sh "The Shell"
.PP
.II sh
.II shell
When you type commands into the \*(CO system, it appears that you
are communicating directly with the computer.
This is not exactly true, however.
When you type into the \*(CO system, you are actually working with
a special \*(CO program, the shell.
This program reads, interprets, and executes every
command that you type into the system.
The shell can also interpret, expand, and otherwise flesh out what you type;
this is done to help spare you unnecessary typing, and to permit you to
assemble powerful commands with only a few keystrokes.
.PP
Please note, in passing, that the \*(CO system comes with two shells:
the Korn shell \fBksh\fR and the Bourne shell \fBsh\fR.
These shells have somewhat different features.
The descriptions in this section assume that you are using \fBsh\fR,
which is \*(CO's default shell.
.PP
The shell is so powerful that mastering it is a major accomplishment;
however, you can take advantage of much of what the shell offers by
learning a few simple commands and procedures.
.PP
This section introduces some commands commonly used by \*(CO users.
For more information on these or other commands see \fBhelp\fR and \fBman\fR.
Also, consult the Lexicon.
.PP
Please note the following special punctuation characters:
.DM
	* ? [ ] | ; { }
	( ) $ = : \(ga ' " < > << >>
.DE
These characters have special meaning to the shell, and typing them can
cause the shell to behave quite differently from what you may expect.
Do not use these characters until you have read the following section,
which discusses their use, or until they are presented in examples.
.Sh "Redirecting Input and Output"
.PP
.II cat
.II >
.II who
.II "I/O redirection"
.II standard^output
.II "user name"
Most \*(CO commands write their output to the
\fIstandard output\fR device, which is normally your terminal's screen.
For example, \fBwho\fR prints on your terminal the name of each user
currently logged into your \*(CO system:
.DM
	who
.DE
By using the special character \fB>\fR,
you can redirect the output of \fBwho\fR into a file.
The command
.DM
	who >whofile
.DE
writes this information into \fBwhofile\fR.
The operator \fB>\fR tells \*(CO to \fIredirect\fR the standard output.
Later, you can list the information on your terminal using \fBcat\fR:
.DM
	cat whofile
.DE
Once the information is in a file, you can process it in other ways.
For example
.DM
	sort whofile
.DE
sorts the contents of
.B whofile
and prints the results on your screen.
In this way, you can display the users' names on your terminal
in alphabetical order.
.PP
.II mail
.II <
.II standard^input
You can also redirect the \fIstandard input\fR
to accept input from a file rather than from your terminal.
To redirect the standard input, use the special character \fB<\fR
before the name of the file that you want read as the standard input.
For example, the command \fBmail\fR
sends electronic mail to another user; normally, it \*(QLmails\*(QR
what you type on the standard input, but you can use \*(Ql<\*(Qr to
tell it to mail the contents of a file instead.
.DM
	mail fred < whofile
.DE
mails the contents of \fBwhofile\fR to user \fBfred\fR.
.Sh "Pipes"
.PP
.II pipe
.II |
The \fIpipe\fR is an important feature of the \*(CO system.
Pipes allow you to hook several programs together by redirecting
the output of one into the input of the next.
A pipe is represented by the character \*(Ql|\*(Qr in the command line.
.PP
.II filter
Most \*(CO programs are written to act as \fIfilters\fR.
A filter is a program that reads its input one line at a time or one
character at a time, performs some transformation upon what it has read,
and then writes the transformed data to the standard output device.
You can easily perform complex transformations on data by hooking a number
of simple filters together with pipes.
Consider, for example, the command:
.DM
	who | sort
.DE
Here, the command \fBwho\fR generates a list of persons who are
logged into the system.
The output of \fBwho\fR is then piped to the program \fBsort\fR, which
sorts the list of users into alphabetical order and prints them on the
standard error device.
.PP
The power and flexibility of the \*(CO operating system owes much to
the pipe.
.Sh "Superuser"
.PP
.II superuser
A special user in the \*(CO system, called the \fIsuperuser\fP,
has privileges greater than those of other users.
The superuser can read all files (except encrypted files) and execute
all programs.
You must be logged in as the superuser during certain
phases of your work as system administrator.
.PP
.II root
There are two ways to access the \*(CO system as the superuser.
The first is to login under the user name
.BR root .
When the system prompts
.DM
	Coherent 386 login:
.DE
reply:
.DM
	root
.DE
This automatically makes you
.BR superuser .
To remind you that you are superuser, the
\*(CO system prompts you with \fB#\fR instead of the usual \fB$\fR.
.PP
The second way to acquire the privileges of superuser is to issue the command
.DM
	su
.DE
when you are logged in as a user other than \fBroot\fR.
You must have privileges to access \fBroot\fR to do this,
and you must know the password for \fBroot\fR.
When you type
.DM
	<ctrl-D>
.DE
in this mode, \*(CO returns you to your previous identity.
.PP
To be the superuser for only one command, use the form of the command
.DM
	su root \fIcommand\fR
.DE
\fIcommand\fR is the command to be executed as superuser.
For example, to edit
the message of the day file \fB/etc/motd\fR if you are not the superuser, type
.DM
	su root me /etc/motd
.DE
When you finish using \*(ME, your original user id will be unchanged.
.PP
To limit access to privileged resources, the \*(CO system requires
users to enter \fIpasswords\fR before being granted that privilege.
Users may be required to enter passwords before logging in.
.PP
.II prompt
.II password
If the
.B root
user has a password, you will be prompted for it.
If you do not enter it correctly, the system will tell you
.DM
	Sorry
.DE
and not allow you to become the
.BR superuser .
.PP
It is normal practice to protect access to superuser status by setting the
password.
If you are the only user of your \*(CO system, or if you deeply trust
all other users, you do not have to do so.
However, because the superuser can perform any sort of mayhem on your system,
it is advisable to set the password, especially if outsiders can dial into
your system via modem.
.Sh "vsh:  The Visual Shell"
.PP
.II vsh
Some users prefer to work visually rather than type explicit commands on a
command line.
For these users, \*(CO offers the command
.BR vsh ,
its
.IR "visual shell" .
.PP
.B vsh
does
.I not
give you a true windowing system, like X or Microsoft Windows; nor does
it use a mouse.
However,
.B vsh
does give you a visual desktop for your files and commands.
You can use the arrow keys on your terminal or console to select a command
or a file; execute commands with one keystroke; program your function keys
to execute commands automatically; and in general make \*(CO easier to use.
.PP
.B vsh
is described in full in its Lexicon entry.
However, to get the flavor of
.BR vsh ,
try the following exercises:
.IP \(bu 0.3i
Before you begin, make sure that \*(CO knows what kind of terminal you
are working at.
To check that type the command:
.DM
	echo $TERM
.DE
If you are working at your computer's console, you should
.B ansipc
on your screen; whereas if you have logged in from an \*(BM plugged into
your computer's serial port, you should see the response
.BR vt100 .
If you do not see the correct response, do
.I not
do the following exercises, because all you will on your screen is a jumble.
.IP \(bu
If your terminal's type is set correctly, invoke the visual shell
by typing
.B vsh
on your command line.
.IP \(bu
In a moment,
.B vsh
draws its desktop on your screen.
You will see five windows:
Two long, narrow windows across the top of your screen;
a big window on the left;
and two small windows stacked on top of each other on the right.
The top narrow window has a number of commands in it; the large
window on the left displays all of the files in the current directory.
The top file displayed in the large, left window is highlighted.
.IP \(bu
The two stacked windows on the right of the screen give information
about your system.
They give, for example, the name of your system, the number of bytes
in the current directory, your login identifier, whether you have
mail waiting for you, and other information.
.IP \(bu
Press the arrow key \(da.
The bar of highlighting moves to the next name displayed in the
large, left window.
The scroll bar in the large, left window also creeps down a little.
Now, press the arrow key \(ua.
The highlighting bar moves up again.
.IP \(bu
Press
.B E
(for Exit).
.B vsh
opens a pop-up window and asks if you want to really exit.
Press \*(RT to accept the underlined option, `y'.
When you do so,
.B vsh
then exits and returns you to the \*(CO command line.
.PP
.B vsh
is a powerful command that makes \*(CO much easier for the average user.
See its Lexicon entry for details on its features and how to use it.
.Sh "Manipulating Text Under COHERENT"
.PP
The \*(CO system includes a number of commands and utilities with
which you can process text.
The phrase
.I "process text"
means to edit it and prepare it for printing.
.Sh "MicroEMACS: Text Screen Editor"
.PP
.II "MicroEMACS"
.II "me"
.II "screen editor"
\*(PN includes a full-featured screen editor, called \*(ME.
\*(ME allows you to divide the screen into sections, called
.IR windows ,
and display and edit a different file in each one.
It has a full search-and-replace function,
allows you to define keyboard macros, and has a large set of commands
for killing and moving text.
.PP
Also, \*(ME has a full help function for C programming.
Should you need information about any macro or library function that
is included with \*(PN, all you need to do is move the text cursor over
that word and press a special combination of keys; \*(ME will then
open a window and display information about that macro or function.
.PP
For a list of the \*(ME commands, see the Lexicon entry for
.BR me ,
the \*(ME command.
A following section of this manual gives a full tutorial on \*(ME.
In the meantime, however, you can begin to use \*(ME by learning a
half-dozen or so commands.
.PP
To invoke \*(ME, type the command
.DM
	me hello.c
.DE
at the \*(PN prompt.
This invokes \*(ME to edit a file called \fBhello.c\fR.
Now, type the following text, as it is shown here.
If you make a mistake, simply backspace over it and type it correctly;
the backspace key will wrap around lines:
.DM
	main()
	{
		printf("hello, world\en");
	}
.DE
When you have finished, \fIsave\fR the file by typing
.B "<ctrl-X><ctrl-S>"
(that is, hold down the control key and type `X', then hold down
the control key and type `S').
\*(ME will tell you how many lines of text it just saved.
Exit from the editor by typing
.BR "<ctrl-X><ctrl-C>" .
.PP
Now, re-invoke \*(ME by typing
.DM
	me hello.c
.DE
The text of the file you just typed is now displayed on the screen.
Try changing the word
.B hello
to
.BR Hello ,
as follows:
First, type
.B <ctrl\-N> .
That moves you to the
.I next
line.
(The command
.B <ctrl-P>
would move you to the \fIprevious\fR line, if there were one.)
Now, type the command
.BR <ctrl-F> .
As you can see, the cursor moved \fIforward\fR one space.
Continue to type
.B <ctrl-F>
until the cursor is located over the letter `h' in
.BR hello .
If you overshoot the character, move the cursor \fIbackwards\fR by typing
.BR <ctrl-B> .
.PP
When the cursor is correctly positioned, delete the `h' by typing the
.I delete
command
.BR <ctrl-D> ;
then type a capital \*(QlH\*(Qr to take its place.
.PP
With these few commands, you can load files into memory, edit them, create
new files, save them to disk, and exit.
This just gives you a sample of what \*(ME can do, but it is enough
so that you can begin to do real work.
.PP
Now, again \fIsave\fR the file by typing
.BR <ctrl-X><ctrl-S> ,
and exit from \*(ME by typing
.BR <ctrl-X><ctrl-C> .
.PP
Just as a reminder, the following table gives the \*(ME commands
presented above:
.DS
.ta 0.4i 1.75i
	\fB<ctrl-N>\fR	Move cursor to the \fInext\fP line
	\fB<ctrl-P>\fR	Move cursor to the \fIprevious\fP line
.sp \n(PDu
	\fB<ctrl-F>\fR	Move cursor \fIforward\fR one character
	\fB<ctrl-B>\fR	Move cursor \fIbackward\fR one character
.sp \n(PDu
	\fB<ctrl-D>\fR	\fIDelete\fR a character
.sp \n(PDu
	\fB<ctrl-X><ctrl-S>\fR	\fISave\fR the edited file
	\fB<ctrl-X><ctrl-C>\fR	Exit from \*(ME
	\fB<ctrl-Z>\fR	Save a file and exit
.DE
Note that on some terminals, the arrow keys will not work.
Note, too, that some remote terminals may have trouble using
.BR <ctrl-S> ,
if they use XON/XOFF to control flow.
In this case, use \fB<ctrl-Z>\fR instead.
.PP
For more information, see the tutorial for \*(ME included with
in this manual.
.Sh "pr, prps, lp: Print Files"
.PP
The command \fBlp\fR prints files for you, making sure that your request
does not conflict with other uses of the printer.
To print a file, type the command
.DM
	lp \fIfile\fR
.DE
substituting the name of the file to be printed for ``file''.
If you don't name a file on the command line,
.B lp
prints what it receives from the standard input.
Thus, you can use \fBlp\fR in pipes; this allows you to print immediately
matter that you type on your keyboard.
.PP
.B lp
will take your file and try to print it on any printer you have plugged into
your computer's parallel port.
If you do not have a printer plugged in, or if it is not turned on,
.B lp
will hold onto your files until the printer becomes ready;
it will wait days, if necessary, until the printer becomes available.
.PP
.B lp
is also intelligent enough to handle requests from several different users:
if more than one user wants to print a file,
.B lp
will print them one at a time.
In this way, the \*(CO system lets several users share one printer.
.PP
.II pr
.B lp
does nothing to the file other than print it.
This means that it does not attach page heading to a file,
nor does it break up the file into page-sized chunks.
Another command, \fBpr\fR, does this for you.
It paginates the standard input, giving a header with date, file name,
page number, and line numbers.
The paginated output appears on the standard output.
.PP
To print a paginated file on the line printer, type:
.DM
	pr \fIfile\fR | lp
.DE
Note the use of the pipe `|', which passes the output of
.B pr
as input to
.BR lp .
.PP
The command
.B prps
is like
.BR pr ;
however, it writes a file in the PostScript page-description language,
suitable for printing on a PostScript printer.
If you have PostScript on your system, you should use
.B prps
instead of
.B pr
to paginate text for printing.
.PP
\*(CO has many more features for printing files than can be covered here.
For more details on
.B lp
and on how to print text, see the Lexicon entry for
.BR printer .
.Sh "nroff, troff: Text Formatters"
.PP
.II nroff
.II troff
The commands
.B nroff
and
.B troff
format text for display or printing.
They are, in fact, text-formatting languages:
you type commands into your text file, and
.B nroff
or
.B troff
interprets the commands to format the text in the manner that you want.
.PP
.B nroff
and
.B troff
differ in the style of formatting that they perform.
.B nroff
formats text into monospaced font, like that on an ordinary typewriter,
Its output is suitable for display on the screen.
.B troff
formats text into proportionally spaced fonts, like those seen on this
page.
Its output is suitable for printing on a laser printer or other
sophisticated typesetting device.
The commands for
.B nroff
and
.B troff
closely resemble each other.
The following descriptions will assume that you are using
.BR nroff ,
but they apply to
.B troff
as well.
.PP
.BR nroff 's
programming language is quite complex and sophisticated.
This manual includes a tutorial that introduces \fBnroff\fR's
language.
You can, however, use \fBnroff\fR to perform simple formatting
tasks by using the \fBms\fR macro package.
The following describes some of the more commonly used \fBnroff\fR
commands.
.PP
To see how \fBnroff\fR works, type the following script:
.DM
	cat >script.r
	.ds CF "Print on Bottom of Each Page"
	Here is some text.
	Here is some more text.
	.PP
	The above command set a new paragraph.
	Yet more text.
	.SH
	Here is a Section Heading
	.PP
	More text.
	\efBThis is printed in bold face.\efR
	This printed in Roman.
	\efIThis is printed in italics or underlined.\efR
	.PP
	Here's some more text.
	Here's yet more text.
	And more text yet.
	<ctrl-D>
.DE
Now, format and display the text with the following command:
.DM
	nroff -ms script.r | more
.DE
You will see the text formatted for your screen.
The string \fB Print on Bottom of Each Page\fR appears at the bottom
of the display.
The following describes the \fBnroff\fR commands with which this formatting
was performed.
.PP
.BR nroff 's
commands are introduced in either of two ways:
by a period `.' in the \fIfirst\fR column of a line; or by a backslash `\e'
occurring anywhere in a line.
The following reviews this script in detail.
.IP "\fB.ds CF\fR"
This defines the text to appear on the bottom of each page.
If the text is more than one word long, it must be enclosed within quotation
marks.
.IP "\fB.PP\fR"
Begin a new paragraph.
.B nroff
skips one line and indents the following line by five spaces (one-half inch).
.IP "\fB.SH\fR"
Print a section heading.
.B nroff
skips one line and prints in boldface
the line of text that follows this command.
.IP \fB\efB\fR
Print the following text in \fBboldface\fR.
.IP \fB\efR\fR
Print the following text in Roman.
.IP \fB\efI\fR
Print the following text in \fIitalics\fR.
.PP
With these few commands, you can perform simple formatting of your text.
.PP
To print the formatted text, use the command
.BR lp .
For example, to print \fBscript.r\fR on a line printer, use the command:
.DM
	nroff -ms script.r | lp
.DE
This discussion is sufficient to get you started, but it just scratches the
surface of what you can do with \fBnroff\fR and \fBtroff\fR.
See their respective entries in the Lexicon for details of what these commands
can do.
See the tutorial for \fBnroff\fR that appears later in this manual for a
thorough introduction to the formatting language used by these commands.
.Sh "Miscellaneous Commands"
.PP
\*(CO includes numerous commands that perform miscellaneous tasks.
These include some of the most useful, and entertaining,
commands in the \*(CO system.
.Sh "who: Who Is on the System"
.PP
.II who
To find who is logged into the system, use the \*(CO command \fBwho\fR.
This command lists who is logged into the \*(CO system, one name per line.
You will see your own user name there as well.
.PP
If you sit down at a terminal that is not in use, but at which
someone has already logged in, the following
command tells you who is logged in:
.DM
	who am i
.DE
\*(CO replies with the name of the user logged in at that terminal.
.Sh "write: Electronic Dialogue"
.PP
.II msg
.II write
.II <ctrl-D>
The command \fBwrite\fR lets you carry on a ``conversation'' with another user.
The conversation continues until you or the other user type
\fB<ctrl-D>\fR on his terminal.
.PP
.II write
For example, user \fBfred\fR
can begin a conversation with user \fBanne\fR by typing:
.DM
	write anne
.DE
On \fBanne\fR's terminal, the message
.DM
	Message from fred...
.DE
will appear.
To establish the other half of the communication, \fBanne\fR should then say
.DM
	write fred
.DE
and a similar notification appears on \fBfred\fR's terminal.
.PP
.II write
At this point, both users simply type lines on their terminal and \fBwrite\fR
sends the message to the other user.
To avoid typing at the same time, each user should end
a ``speech'' by typing a line that has the single letter
.DM
	o
.DE
to signify ``over'', or ``go ahead''.
When the other user sends you this, you know it is your turn to ``talk'',
and vice versa.
.PP
.II write
When your communication is finished, you should type
.DM
	oo
	<ctrl-D>
.DE
Here, \fBoo\fR means ``over and out'',
and the \fB<ctrl-D>\fR terminates the \fBwrite\fR command.
Note that \fBo\fR and \fBoo\fR are polite conventions,
and are not necessary to using \fBwrite\fR.
.Sh "mail: Send an Electronic Letter"
.PP
.II mail
You can send electronic mail to another user on your \*(CO system by using
the command
.BR mail .
This command works whether or not that person is logged into the system at
the time you type your message.
The message is stored in an electronic \*(QLmailbox\*(QR, and the user will be
notified that a message is waiting for him the next time he logs into your
system.
.PP
Before you can use \fBmail\fR on your system, you must run the program
\fBuuinstall\fR.
This program will ask you some questions about how you have configured
your \*(CO system, and will write files of information that \fBmail\fR
and the communications protocol \*(UU need to deliver your mail.
For detailed directions on how to run \fBuuinstall\fR, see the section
\fIInstalling UUCP\fR in the \*(UU tutorial that appears later in this manual.
.PP
Among other things, this program will ask you to name your ``site'' and
your ``domain''.
Without going into too much detail at this point, the site is
\fInom de plume\fR by which your machine is known to other \*(CO or
\*(UN systems.
Site names generally are not computer-ese; \fBconan\fR, \fBterminator\fR,
\fBlepanto\fR, \fBchelm\fR, and \fBsmiles\fR are all examples of site names.
If you don't intend to communicate with other systems, use your first name
as the site name.
The domain is the name by which a group of related machines are together known.
If you and a number of other local \*(CO systems wish to be known together,
you can establish a domain and register it with the network.
Domain names, too, should be descriptive.
If you don't intend to use a domain, set the domain name to \fBUUCP\fR.
.PP
To mail a message to user \fBanne\fR, just type:
.DM
	mail anne
.DE
.B mail
immediately prompts you for a title for your message:
.DM
	Subject:
.DE
You can type the message's subject, which will be used to title the message,
or you can just press \*(RT.
.PP
Once you have titled your message, type the body of the message.
You can conclude your message in any of three ways:
you can type \fB<ctrl-D>\fR, type a period `.' at the beginning of a line,
or a question mark `?' at the beginning of a line.
The first two methods end the message immediately; the last method, however,
invokes an editor, and lets you edit the message further before sending
it on to the intended recipient.
Environmental variable
.BR EDITOR ,
if defined, selects the editor to be used.
.PP
For example, to send your message to user \fBanne\fR, you might do the
following.
First, invoke \fBmail\fR:
.DM
	mail anne
.DE
Next, give your message a title:
.DM
	Subject:  I'll be working late
.DE
Finally, type the body of the message:
.DM
	I'll be working late.  I hope to get home before Catherine
	and George go to bed.  Please remind Ivan and Marian to do
	their homework.  Marian should remember to practice her
	violin.
	\fB<ctrl-D>\fR
.DE
If you wish, you can first type your message into a file and then mail it.
For example:
.DM
	cat >hb.msg
	All come to the birthday party at four
	next to the pump room.
	<ctrl-D>
.DE
To mail the message to user \fBjill\fR, type:
.DM
	mail jill <hb.msg
.DE
You can send a mail message to several users at one time by listing each
user's name on the command line.
For example, the command
.DM
	mail jill jack ted barb < hb.party
.DE
mails the contents of file
\fBhb.party\fR to \fBjill\fR, \fBjack\fR, \fBted\fR, and \fBbarb\fR.
To illustrate the use of the mail command, send yourself a mail message.
Type the following;
substitute your user name for ``you'' in the mail command:
.DM
	mail you
	Subject:  test the COHERENT mail system
	This is a note to
	myself to test
	mail.
	.
.DE
.II mail^receiving
.II "receiving mail"
If someone has sent you mail, the \*(CO system will tell you:
.DM
	You have mail.
.DE
when you log in.
.PP
To receive mail, type the \fBmail\fR command with no parameters:
.DM
	mail
.DE
If you have no mail, \*(CO will tell you:
.DM
	No mail.
.DE
If you do have mail, the system will print
each message on your terminal, along
with the user name of the sender, and the date and time that the message
was mailed.
.PP
.II mail
.II <Return>
After each message, the \fBmail\fR program types a question
mark \fB?\fR and waits for your reply.
You can type any of the following commands in reply to the prompt:
.IP \fBd\fR
Delete the message.
.IP \fB<Return>\fR
Proceed to the next message.
.IP "\fBs \fIfile\fR"
Save, or copy, the message into
.IR file .
.IP \fBq\fR
Quit \(em exit from \fBmail\fR and return to the shell.
.PP
You will know that you are finished with all of your messages when
\fBmail\fR sends you a \fB?\fR without typing anything before it.
.PP
.B mail
can also send messages to other \*(CO or \*(UN systems via the \*(UU utility.
See the accompanying tutorial on \*(UU to see how you can set up
\*(CO to do this.
.Sh "msgs: Cumulative Message Board"
.PP
.II msgs
The message of the day disappears when a new message is inserted.
If a user does not log in for several days,
the message of the day may no longer be there.
For items that you want everyone to see,
such as hours of operation or new operating procedures, you should use
.B msgs
instead of
.BR motd .
.PP
.B msgs
helps users get all important messages, even if they don't log in every day.
The system remembers which users have seen each message.
After a user logs in, invoking
.B msgs
will show the number, date, and author of each message
written since the user last logged in.
Therefore it is easy for the
user to stay up to date with the system-wide messages.
.PP
To add a message to the file, simply mail the message to
.BR msgs .
To title the message, write it as the first line in the message,
after the ``Subject:'' prompt from
.BR mail .
.PP
The home directory for \fBmsgs\fR will grow over time, as more and more
messages accumulate.
Also, if a new user is enrolled on your \*(CO system, he may have to
wade through several hundred messages when he first logs in.
Therefore, you should purge the home directory for \fBmsgs\fR every now
and again; you may wish to throw away the announcements of office parties
three Christmases ago, and save important information on diskette.
.PP
.B msgs
keeps track of what messages each user has read by recording the number
of the last message read in the file
.BR $HOME/.msgsrc .
When each user logs on, his version of
.B \&.msgsrc
is inspected to determine the last message seen.
If messages were added after that,
.B msgs
prints the ones the user wants to see, and then updates
.BR \&.msgsrc .
.Sh "grep: Find Patterns in Text Files"
.PP
.II grep
.II patterns
.II "regular expressions"
The command
\fBgrep\fR lets you find lines that contain a \fIpattern\fR within one
or more files.
Patterns are sometimes called \fIregular expressions\fR.
.PP
.II grep
To illustrate \fBgrep\fR, create file \fBdoc1\fR by typing:
.DM
	cat >doc1
	a few lines
	of text.
	<ctrl-D>
.DE
Then the command
.DM
	grep text doc1
.DE
prints the second line of file \fBdoc1\fR:
.DM
	of text.
.DE
.II patterns
The first parameter to \fBgrep\fR is the \fIpattern\fR
for which you are looking;
the rest of the arguments are the names of files to be examined.
\fBtext\fR is the pattern and \fBdoc1\fR is the file.
.PP
To find if a particular user is on the system, pipe \fBwho\fR into \fBgrep\fR:
.DM
	who | grep you
.DE
(Substitute the user name in question for \fByou\fR.)
Try it with your user name.
The pattern is \fByou\fR, but no file name is specified.
\fBgrep\fR reads input from the standard input, which in this example
is connected to the output of the \fBwho\fR command.
.PP
You can specify several files to be searched;
simply put the additional file names after the first:
.DM
	grep pattern doc1 doc2
.DE
Or, you can search all files in the current directory for the pattern with
.DM
	grep pattern *
.DE
The asterisk will be interpreted to mean all files,
and grep will look for \fIpattern\fR in each.
.PP
.II grep
.II patterns
The search pattern can be a \fIpattern\fR.
Patterns are fully discussed in the tutorial for
.BR ed .
.PP
You can also locate lines that do \fInot\fR contain given patterns by using
the \fBgrep\fR option \fB-v\fR.
.DM
	grep -v bugs prog1 prog2
.DE
This command finds and prints all lines in files \fBprog1\fR and
\fBprog2\fR that do not contain the pattern \fBbugs\fR.
.Sh "date: Print the Date"
.PP
.II date
.II time
The \*(CO system keeps track of the time and date.
To find the date and time, use the command:
.DM
	date
.DE
\*(CO responds with the day of the week, the month day and year,
and the time of day.
.PP
.II GMT
.II timezone
Internally, the \*(CO system records
the date and time as the number of seconds since January 1, 1970,
00:00:00 Greenwich Mean Time (GMT).
This means that files created in one time zone and
referenced in another time zone will bear the correct time.
The time and date printed out is converted from the internal form to the
local time.
.Sh "passwd: Change Your Password"
.PP
.II password
You should change your password from time to time, to ensure that no
unauthorized person can gain access to your files (or to the system as a whole).
.PP
It is easy to change passwords on the \*(CO system:
.II passwd
just type the command \fBpasswd\fR.
.B passwd
first asks you for your current password (if you have one),
and then asks you to enter your new password twice.
Entering the new password twice helps ensure that the system gets the
password as you want it.
If you do not type it the same way both times, \*(CO will say:
.DM
	Password not changed.
.DE
You must then begin again with the command \fBpasswd\fR.
.PP
Be sure the password is something that you can remember.
It is recommended that the password be at least six characters long.
Do not write it down, but memorize it.
You can use a four-letter password, but if you do, you
should mix upper-case and lower-case letters to make it more difficult
for outsiders to guess.
.Sh "stty: Change Terminal Behavior"
.PP
.II terminal
Because a wide variety of terminals can be used with the \*(CO
system, you must pass some information to
the \*(CO system so it can handle your terminal correctly.
.PP
.II stty
The command
.B stty
describes the information \*(CO currently has for you;
you can then use
.B stty
with arguments to change how \*(CO handles your terminal.
.PP
For example,
\*(CO normally echoes each character you type, as you type it.
However, if your terminal also echoes what you type,
you will see double characters.
To prevent this, issue the command:
.DM
	stty -echo
.DE
The program
.B login
uses this feature when you type your password, to help keep it secret
from anyone who is kibbitzing at your desk.
.PP
To set the echo feature again, type:
.DM
	stty echo
.DE
.II tab
When you first log in, the system presumes that your terminal does not
directly handle the \fBtab\fR character, so when \*(CO sends a \fBtab\fR to
your terminal it simulates it with spaces.
If your terminal does handle tabs, issue the command:
.DM
	stty tabs
.DE
The \*(CO system will no longer substitute spaces for tabs.
To go back to substitution,
.DM
	stty -tabs
.DE
.II kill
.II erase
.II <kill>
.II <erase>
The
.B <erase>
character lets you delete the previously typed character.
The \fB<kill>\fR character lets you delete the line that you have been
typing but have not yet finished.
By default, \*(CO sets these to, respectively, \fB<ctrl-H>\fR and
\fB<ctrl-U>\fR.
To change them to, respectively, \fB<ctrl-E>\fR and \fB<ctrl-K>\fR,
use the \fBstty\fR command as follows:
.DM
	stty erase ^E kill ^K
.DE
The carat `^' tells \fBstty\fR that you want to specify a control character.
.PP
.II kill
.II erase
To reset erase and kill to the default values at login, the command
.DM
	stty ek
.DE
suffices.
This is equivalent to:
.DM
	stty erase ^H kill ^U
.DE
To see what your current terminal parameter settings are, type
.DM
	stty
.DE
with no arguments.
.Sh "Scheduling Commands For Regular Execution"
.PP
.II "event scheduling"
The command \fBcron\fR is a valuable tool for using your \*(CO system.
With it, you can instruct \*(CO to execute commands at various times of
the day or night, even if nobody is logged into the system.
.PP
To specify a command to be executed at some later time, simply enter
one line of information in a ``cron'' file.
(Where the cron file lives will described below.)
For example, assume that you want to greet user
.BR norm ,
if he is logged into the system on Monday morning.
You can do this by sending him a message at 8:13 on Monday.
Use \*(ME to add the following line to the cron file:
.DM
	13 8 * * 1	msg norm%You are sure in early!
.DE
The numbers and \fB*\fR at the beginning specify the time:
.DM
	13 8 * * 1
.DE
The \fB13\fR means ``13 minutes past the hour''.
(\fBcron\fR numbers the minutes zero through 59.)
The \fB8\fR means ``8 AM''.
(\fBcron\fR numbers the hours of the day zero through 23, with zero
indicating 12 AM.)
The positions containing
.B *
normally specify the day and month.
The two \fB*\fR characters mean ``any day'' and ``any month''.
Finally, the \fB1\fR means ``day 1 of the week,'' which is Monday.
(\fBcron\fR numbers the days of the week zero
through six, with zero indicating Sunday.)
The breakdown of this command is shown as follows:
.DS
.ta 0.5i 1.5i
	minute	13
	hour	8
	day of month	* \(em all days
	month	* \(em all months
	day of week	1 \(em Monday
.DE
Because each entry in the cron file must be on one line, the symbol
.B %
represents the beginning of the input string.
If the information is too long for one line, type a backslash character
before you press \*(RT to end the line.
The backslash tells
.B cron
to ignore the \fB<Return>\fR character.
.PP
With this information in the file, \fBcron\fR executes the command
.DM
	msg norm
	Am Monday!
.DE
at 8:13 every Monday morning.
.PP
.II "24-hour time"
.II cron
.B cron
expects time to be in the 24-hour clock,
so 1 PM is represented as \fB13\fR hours.
If you need to print a literal percent sign `%', precede it with a backslash:
.DM
	\e%
.DE
The times for
.B cron
commands can be even more complex than the numbers and \fB*\fR shown above.
.PP
.II "- (hyphen)"
You can express a range for any of the five parts
of a time by separating two numbers with a hyphen.
For example, to send user
.B marianne
a humorous message on week days, use the command:
.DM
	59 11 * * 1-5	 /usr/games/fortune | msg marianne
.DE
.II ", (comma)"
To list a choice of times, separate single numbers
or ranges with commas but no spaces.
To send notification about a meeting on Monday, Wednesday,
and Friday at 3 PM, use:
.DM
	0 15 * * 1,3,5	 echo Meeting at 3:30 ... | mail fred anne joe
.DE
The time specification
.DM
	0 15 * * 1,3,5
.DE
represents the time 1500 (3 PM) on every Monday, Wednesday, and Friday.
.PP
.B mail
and
.B msg
are just some examples of commands that can be used with
.BR cron ;
many others can be used.
For example, \fBcron\fR is commonly used to execute \*(UU commands late at
night, when telephone rates are low.
See the Lexicon article on \fBcron\fR for more information about this command.
If you wish to schedule commands to be run but not on a regular basis,
use command
.BR at .
See its Lexicon article for further details.
.PP
As was mentioned above, you must edit a cron file for the \fBcron\fR
daemon to execute a command automatically.
\*(CO uses a complex scheme of cron files.
If the file \fB/usr/lib/crontab\fR exists, the \fBcron\fR daemon will read
it \(em and only it.
However, if \fB/usr/lib/crontab\fR does not exist,
the \fBcron\fR daemon will look in directory \fB/usr/spool/cron/crontabs\fR
for the set of cron files located there.
Each user can have his own cron file.
All commands in a user's cron file are executed with the suite of permissions
granted to that user; thus, a user cannot use \fBcron\fR to delete files that
do not belong to him.
If a user wishes to create or update his own cron file, use the command
\fBcrontab\fR; see its entry in the Lexicon for details.
.PP
Please note that each flavor of cron file uses the syntax described above.
.Sh "Managing Processes"
.PP
A
.I process
is a command that is undergoing execution.
Because \*(CO is a multi-tasking operating system, numerous processes
can be undergoing execution at the same time.
The following commands let you monitor and, within limits,
affect the operation of the processes your \*(CO system is executing.
.Sh "ps: List Active Processes"
.PP
.II process
.II process^id
.II PID
Each process in the system is assigned a number called the \fIprocess id\fR,
or \fIPID\fP.
Each user logged into the system has one or more processes.
Except in special circumstances,
the first process that he has is the shell, or command-line interpreter.
The commands he types are run by the shell.
.PP
The shell normally waits for a command to terminate
before it begins to process the next command.
.II &
However, if you use the `&' operator,
the shell creates simultaneous processes:
that is, while it executes one command it will let you type another.
Thus, you can execute two or more commands simultaneously.
.PP
.II ps
.II process^id
You can examine the processes associated with your login, or all
processes in the system, with the command
.BR ps .
Type:
.DM
	ps
.DE
The result will resemble:
.DM
	TTY       PID  COMMAND
	color0     36  ksh
	color0   4004  mail
	color0   4005  me
	color0   4009  sh
	color0   4010  ps
.DE
The first column
names the terminal you are running on, in this case the virtual console
.BR color0 .
.II /etc/ttys
This identifier is taken from the file
.BR /etc/ttys ,
with the prefix \fBtty\fR removed from name.
The \fBtty\fR identifier is also printed by the command \fBwho\fR.
The second column
lists the corresponding process identifier (PID).
The third column names each command and gives its parameters, if any.
.B ksh
represents the shell process,
and \fBps\fR represents the \fBps\fR command itself.
.PP
To see all the processes, type:
.DM
	ps -a
.DE
The result will resemble:
.DM
	TTY       PID  COMMAND
	null        1  init
	color3     33  ksh
	color2     34  ksh
	color1     35  ksh
	color0     36  ksh
	com3l      37  login
	color3   3629  sh
	color3   3630  kermit
	color3   3631  kermit
	color0   4004  mail
	color0   4005  me
	color0   4011  sh
	color0   4012  ps
.DE
This display will, of course, differ quite a bit from system to system
and from minute to minute.
.PP
For a full description of all options to
.BR ps ,
see its entry in the Lexicon.
.Sh "kill: Signal Processes"
.PP
.II kill
Occasions will arise when the system administrator
must log other users out of the system.
For example, you may need to bring the system down quickly; or
perhaps a user forgot to log out before leaving the terminal and
did not see your broadcast message requesting that all users log out.
.PP
.II superuser
.II kill
.II root
.II su
The command
.BR kill ,
when used by the superuser, terminates processes.
To log out a user whose shell has process number 300, use the command:
.DM
	kill -9 300
.DE
You must be logged in as
.B root
or use the command
.B su
to
.B kill
a process that belongs to another user.
Each user can kill all processes that he owns, including his own shell
process (which automatically logs him out).
.PP
.B kill
has other uses as well \(em see the Lexicon's entry for \fBkill\fR
for more information.
.SH "Programming Under COHERENT"
.PP
.II C
.II "assembly language"
The \*(CO system provides a number of languages in which you can
write programs.
.PP
The shells included with \*(CO \(em \fBsh\fR, the Bourne shell, and
\fBksh\fR, the Korn shell \(em not only process commands, but are powerful
programming languages in their own right.
For details on how to program in these languages, see their respective
entries in the Lexicon; and see the tutorial \fIIntroducing sh, the Bourne
Shell\fR, which follows in this manual.
.PP
\*(CO includes a full-featured assembler, with which you can
assemble your assembly-language programs.
Assembly language is sometimes required for operations that require
you to work very closely with the operating system or hardware.
For more information on the \*(CO assembler, see the Lexicon entry
for \fBas\fR.
.PP
Most programming that cannot be executed efficiently by a shell language
is done in C, the language in which the \*(CO
system was written.
The \*(CO system comes with a full-featured C compiler, with which
you can compile the program you write in that language.
If you are new to C, the tutorial \fIThe C Language\fR,
which follows in this manual, will introduce you to it.
The following sub-sections briefly describe the tools available
under \*(CO with which you can write, compile, and debug your
C programs.
.Sh "Basic Steps in COHERENT Programming"
.PP
The steps that are necessary to generate a program are:
.DS
	1. Create the program source file
	2. Compile the source program, correcting any errors
	3. Test and debug the program
	4. Run the program
.DE
If you have compilation errors in step 2, or program errors in step 3 or
4, return to step 1.
.PP
.II macro
.II C
.II m4
.II db
.II cc
.II ed
Use your favorite editor to build and change the source program, the \fBcc\fR
command to compile the source program and produce an object program, and
\fBdb\fR to help debug the program.
Although the C compiler provides a macro facility, other languages do not.
Therefore, if the source program uses macros, you can use \fBm4\fR
to expand the macros.
.PP
This section covers each of these steps and provides some example programs.
.Sh "Create the Program Source"
.PP
The first step is to use \*(ME, \fBvi\fR, \fBed\fR, or some other
editor to create the program's source file.
Details on the use of \fBed\fR and \*(ME are covered in their respective
tutorials, which follow in this manual.
Each editor's commands are summarized in its Lexicon article.
.PP
For the first program, try a simple program that prints a short message on
your terminal.
For the sake of simplicity, we'll enter text using \fBcat\fR instead of
invoking an editor.
To build the program, type the following:
.DM
	cat > small.c
	main ()
	{
		printf ("The COHERENT operating system\en");
	}
	<ctrl-D>
.DE
The first line invokes the concatenation program \fBcat\fR to enter the
program's source code.
The \fB<ctrl-D>\fR signals that you have finished entering text.
.PP
.II parentheses
.II main
.II brace
.II {}
The program itself begins with the special word \fBmain\fR
which defines a function and must appear in every C program.
The parentheses, here with
nothing between them, enclose any arguments that are passed to the function.
They are required even if there are no arguments.
The body of the program appears between the braces \fB{\fR and \fB}\fR.
.PP
.II printf
.II library^C
.II "newline^in C strings"
The function \fBprintf\fR is part of the standard library of C programs.
It prints formatted information on the terminal.
In this case it will produce the string enclosed between quotation marks.
The special character string
.DM
	\en
.DE
means ``newline''.
Two lines of output to the terminal can be produced by
.DM
	"line 1\enline 2\en"
.DE
as an argument to \fBprintf\fR.
This appears in the output as:
.DM
	line 1
	line 2
.DE
.II "output formatting"
For a fuller introduction to the C language, see the tutorial on the
\fIThe C Language\fR, which follows in this manual.
.Sh "cc: Compile the Program"
.PP
.II C
.II cc
.II ld
.II compiler^C
.II "C^program linker"
The command \fBcc\fR compiles C programs.
It executes all the parts of the C compiler and the associated linker \fBld\fR.
The linker combines pieces of programs
and includes necessary elements from the library, such as \fBprintf()\fR.
The linker is occasionally called from the command line, but only for more
complex problems than you are trying here.
To compile our test program, type the command
.DM
	cc small.c
.DE
If the compiler detects any errors, it prints a message on the
terminal, along with the line number that contains the error.
You can use this line number to find the error with your editor and fix it.
You can now use the program by simply typing:
.DM
	small
.DE
The tutorial on \fIThe C Language\fR describes \fBcc\fR in greater detail;
also see its entry in the Lexicon for a full summary of its many capabilities.
.Sh "m4: Macro Processing"
.PP
.II m4
To extend the capabilities of all languages, the \*(CO system provides
a macro processor, called \fBm4\fR.
.PP
.II m4
.II parameter^substitution
.II "substitution^of parameters"
Program source for all languages consists of character strings.
Macro processors perform string replacement, whereby a string in the input file
may be replaced by another string.
\fBm4\fR provides parameter substitution, as well as testing values of
currently available strings and conditional processing.
\fBm4\fR is unique in that you can rearrange large
sections of the input text by using the macros.
For more information on \fBm4\fR, see the tutorial
\fIIntroduction to the m4 Macro Processor\fR, which follows in this manual.
.Sh "make: Build Larger Programs"
.PP
.II make
.II program^modularity
All the examples of programs thus far have been self-contained.
As programs grow larger, it is usual to divide the source
program into smaller files.
This simplifies editing, speeds compilation,
increases modularity, and lets several different programs
share common functions.
.PP
.II file^include
Thus, in developing the larger program, you may have several source files
in your directory, possibly a header file or two, and the object files that
result from compilation.
From these are built the executable file that runs when you type its name.
.PP
To change or fix the program, you must edit the source programs or
header files in question with \fBed\fR, recompile the required source, and
relink all the modules.
But, with a change that affects several modules, it can be tricky to
remember exactly which modules need recompilation, and it can be
time-consuming to recompile all modules.
.PP
.II make
.II "file^modification time"
\*(CO provides a command called \fBmake\fR, which solves this problem.
\fBmake\fR examines the time a file was last modified, and the time of
modification of files that it depends upon, and performs the necessary
compilation or other processing.
(\*(CO file system directories
contain the time that each file was created or modified.)
.PP
The tutorial \fIThe make Programming Discipline\fR, which follows in
this manual, fully introduces this powerful and useful program.
.Sh "db: Debug the Program"
.PP
.II debugging
.II program^debugging
.II structured^programming
.II programming^structured
The first and most critical step to debugging
programs is to not put bugs in them!
The methods of structured analysis, design, and programming, or the method of
stepwise refinement can substantially reduce the number of errors in a program.
.PP
One can also place \fBprintf\fR statements at strategic points throughout
the program to display logic flow and key data values.
These display statements should be designed
so that they can be turned off for normal
operation without removing them from the program.
.PP
.II db
On occasion, however, you may find that it is necessary to debug at
the machine level.
If you must, \*(CO's \fBdb\fR will make it possible to do so.
.PP
.II db
.II "machine instructions"
\fBdb\fR provides tools that make the machine
program instructions visible in the most natural notation.
That is, instructions are displayed in a fashion that resembles
assembly language, numbers can be displayed in hexadecimal,
octal, or decimal as needed, and strings of characters
displayed in familiar graphic form.
\fBdb\fR can also patch a program to be run again, as well as to control the
execution of a program with breakpoints and one step at a time.
.PP
.II db
Briefly, to use \fBdb\fR
on a program like our sample \fBsmall\fR above, use the command:
.DM
	db small
.DE
Now you can inspect and display instructions and data in the system,
control execution, and even change the instructions in the program if you
are bold enough.
.PP
To examine a data segment location in the program, simply type the
address of the location.
\fBdb\fR knows about symbols in the program, so if
you want to examine the location corresponding to \fBmain\fR, type:
.DM
	main
.DE
\fBdb\fR types out the value in hexadecimal or octal (depending upon which
is appropriate for your machine).
.PP
You can expand the display command to print many locations at one time,
and choose the format of printout.
To print five locations interpreted as instructions, type
.DM
	main,5?i
.DE
where the format character \fBi\fR follows the question mark indicating format,
and 5 is the count of locations to be printed.
To exit
.BR db ,
type
.DM
	:q
.DE
For a complete list of the format that \fBdb\fR recognizes,
and other details about \fBdb\fR, see its entry in the Lexicon.
.SH "Administering the COHERENT System"
.PP
The \*(CO system can be used by many people at the same time.
One person must coordinate its use, like a key operator
does for an office copier.
This person is called the \fIsystem administrator\fR,
and he sees to it that the \*(CO system runs smoothly every day.
The administrator can also customize the \*(CO
system to the needs of an individual installation.
.PP
Although you may be the only person to use your \*(CO system,
many of the ideas
discussed here are important for making your system work at its best.
Please spend a few minutes reading this manual to familiarize yourself
with the elementary concepts of \*(CO system maintenance.
.Sh "Adding a New User"
.PP
.II user^name
.II user^id
.II password
Each user allowed to use your \*(CO system must have a \fIuser name\fR
and a \fIuser id\fR; the user may also have a
.IR password .
The user name is usually the user's initials or a nickname.
The \fIuser id\fR is an integer number used to identify the
user internally to the system.
As system administrator, you will assign both of these for each user.
This section tells you how.
.PP
.II /etc/passwd
To log in to the system, a user must have an entry in the \fIpassword\fR file
.BR /etc/passwd .
The password file contains each user's name, id, and password if any.
As system administrator, you will maintain this file.
.PP
.II group^name
.II group^id
Likewise, each group of users is assigned a \fIgroup name\fR,
as well as a \fIgroup id\fR.
Groups are not necessary to use the \*(CO
system, but some installations prefer to set up
groups by project or department.
.PP
.II newusr
It is simple to add a new user to the system.
The command
.B newusr
takes care of all the details, and makes an entry in the password file.
You must be logged in as
.BR root .
For example, to create an entry for a user named Henry, log in as
.BR root ,
and then issue the command:
.DM
	/etc/newusr henry "Henry Smith" /usr
.DE
This creates an entry in \fB/etc/passwd\fR for \fBhenry\fR,
creates his home directory in the \fB/usr\fR file system,
creates all appropriate files for him (such as his \fB.profile\fR
and his mailbox), and sets all permissions correctly.
.Sh "System Security"
.PP
One of the most important tasks in running your \*(CO system is
maintaining its security.
Basically, security means two things:
keeping outsiders from logging into your system, and keeping your
system's users from doing untoward things.
This section describes some steps you can take to ensure that your system
is secure.
.Sh "Passwords"
.PP
.II protection
.II password
Passwords provide the first level of \*(CO system security.
.PP
.II login
For systems with passwords,
each user with a password must type his password as part of the login process.
If he enters the password incorrectly, he cannot log in.
.PP
.II password
Your system's administrator can assign a password when she creates a
user's log-in account, as described above.
If you do not assign a password, anyone will be able to log in as that user.
.PP
In any system with passwords, it is especially important to assign a
password to the \fBroot\fR, or \fIsuperuser\fR.
If the superuser does not require a password,
any user can log in as \fBroot\fR and automatically have access to
the powerful tools that control the operation of the system.
.PP
.II passwd
Any user with a password can restrict access to his files.
Once you assign him his password, he can change it with the command
.BR passwd .
However, because of higher privileges,
.B root
can always access everyone's files.
.PP
.II /etc/passwd
The passwords are kept in file
.BR /etc/passwd ,
with the rest of the user login information.
Passwords are encrypted, so reading \fB/etc/passwd\fR will not reveal passwords.
.Sh "File Protection"
.PP
.II file^protection
.II protection
The second level of \*(CO system security is \fIfile protection\fR.
A user can set each of three categories of protection for each of his files.
A standard protection, or \fIaccess permission\fR,
is given to each file when it is created.
.PP
The three categories of permissions are for the user himself,
for other users in his group, and for all other users.
To see the levels of protection of your files, type the command
.DM
	ls -l
.DE
For more details on the meaning of each column in this printout,
see the Lexicon entry for the change-mode command
.BR chmod .
.Sh "Encryption"
.PP
.II crypt
The command
.B crypt
provides a third level of system security.
It lets a user encode and decode information in a file.
The superuser has access to every file in the system; so to protect
sensitive information even from his prying eyes, a user can disguise
it with encryption.
Sensitive system information, such as passwords, are also encrypted
for security purposes; and the
.B mail
command lets users send encrypted mail to each other.
For details about encryption, see the entry on \fBcrypt\fR in the Lexicon.
.Sh "Dumping and Saving Files"
.PP
You should regularly copy your files on floppy disk, to protect your
valuable files against a catastrophic system failure.
The Lexicon article
.B backups
describes in detail how to do this.
.\".II backups^strategies
.\"This section discusses how you can copy files to floppy disk.
.\"You should do this regularly, both to
.\"free up disk space and to back up valued files
.\"to protect them against catastrophe.
.\".PP
.\"There are two general strategies for dumping files.
.\".PP
.\"One strategy uses the commands \fBustar\fR or \fBtar\fR
.\"to create archives of files on a floppy disk.
.\"This strategy is fine for systems that are used by a handful of users,
.\"and that are not used for ``real-world'' jobs, such as running a business.
.\".PP
.\"The other strategy uses the command \fBcpio\fR to implement a system of
.\"regular dumps.
.\"This strategy is preferred for systems that daily amass data of importance
.\"for a real-world job, such as running a business or managing a research
.\"project.
.\".PP
.\"You should always have a system of back-ups for your system.
.\"Which strategy you use depends on how you are using your system.
.\"The following sub-sections describe how to implement each strategy of
.\"back-ups.
.\".PP
.\"Please note that the following descriptions assume that you are using a
.\"5.25-inch, high-density floppy disks set in drive 0 (drive A).
.\".Sh "Back-ups Using ustar"
.\".PP
.\".II ustar
.\".II fdformat
.\"This sub-section describes how to back-up files using the \*(CO command
.\".BR ustar .
.\".PP
.\"The first step is to prepare floppy disks to receive files.
.\"Insert a 5.25-inch floppy disk into drive 0, and then type the following
.\"command:
.\".DM
.\"	/etc/fdformat -v /dev/fha0
.\".DE
.\"The command
.\".B fdformat
.\"formats the diskette, verifying that no media defects exist.
.\"You must perform this task of formatting a floppy disk
.\"only before you use it the first time.
.\".PP
.\"The next step is to create an archive of the files you wish to back up.
.\"Use the portable archive command
.\".B ustar
.\"to collect a mass of files into an archive on the floppy disks.
.\"For example, to archive all files in directory \fBsource\fR, use the
.\"following command:
.\".DM
.\"	ustar cvf /dev/rfha0 source
.\".DE
.\"The options \fBcvf\fR tell \fBustar\fR to create an archive, run in verbose
.\"mode, and write the archive onto the device or into the file
.\"named in the next argument.
.\"\fB/dev/rfha0\fR names the floppy device onto which
.\"you wish to write the archive.
.\"Finally, \fBsource\fR is the directory whose files you wish to back up.
.\".PP
.\"To perform a listing of the contents of the newly created archive, enter
.\".DM
.\"	ustar tvf /dev/rfha0
.\".DE
.\"The options \fBtvf\fR tell \fBustar\fR to list the contents
.\"of the archive, run in verbose mode,
.\"and read the archive from the device or file named in the next argument.
.\".PP
.\"To extract several files from the archive, enter a command of the form
.\".DM
.\"	ustar xvf /dev/rfha0 source/myfile 'source/*.c'
.\".DE
.\"The options \fBxvf\fR tell \fBustar\fR to extract or unarchive
.\"the specified files, run in verbose mode,
.\"and read the archive from the device or file named in the next argument.
.\"Note that the second file argument contains a ``wildcard'' character
.\"and thus must be quoted to prevent expansion by the shell.
.\".PP
.\"For more information on how to use
.\".BR ustar ,
.\"see its entry in the Lexicon.
.\".Sh "Back-ups Using cpio"
.\".PP
.\"The following sub-sections describe how to perform back-ups using the
.\"\*(CO command
.\".BR cpio .
.\".II USENIX
.\".II "Colburn, Mark H."
.\".II cpio
.\"\fBcpio\fR is a public domain program written by Mark H. Colburn for
.\"the USENIX association, which is included with the \*(CO system.
.\"This program performs mass dumps and restores of files using a
.\"universally recognized file format.
.\"In general, \fBcpio\fR is easier to use than \fBdump\fR
.\"and \fBrestor\fR, and its output can be portable among other \*(CO and
.\"\*(UN systems.
.\".PP
.\"In this example, dumps are performed monthly, weekly, and daily.
.\"You should prepare at least three sets of floppy disks for the monthly saves,
.\"giving you three months of full backup.
.\"You will use the diskettes in rotation, with the oldest always used next.
.\".PP
.\"Once a week, you should dump
.\"information in the system that is new or has been changed since the
.\"end of the previous week.
.\"You will need five sets of diskettes,
.\"since some months have five weekends in them.
.\".PP
.\"Finally, every day you should save information that has changed that day.
.\"For these dumps, you will need five sets of diskettes:
.\"one for each working day.
.\"You may need extras in case of weekend work.
.\".PP
.\"Label each set of disks carefully as
.\"\fImonthly\fR, \fIweekly\fR, or \fIdaily\fR.
.\"Label the daily diskettes Monday through Friday,
.\"the weekly diskettes Week 1 through
.\"Week 5, and the monthly diskettes Month 1 through Month 3.
.\"When you do the dump, write the date on the label.
.\".PP
.\"The following gives a step-by-step description of how to use
.\".B cpio
.\"to back up files.
.\".IP 1. 0.3i
.\"Log into the system as
.\".B root .
.\"You must have superuser privileges to perform a dump.
.\".IP 2.
.\"If you have not yet done so, use the command
.\".B fdformat
.\"to format a set of diskettes, as shown above.
.\"With high-density, 5.25-inch diskettes, a rule of thumb is to prepare
.\"one diskette for each megabyte of data to be dumped.
.\".IP 3.
.\"Tell other users to log off the system by typing:
.\".DM
.\".II wall
.\"	/etc/wall
.\"	Please log off.
.\"	Time for file dump.
.\"	<ctrl-D>
.\".DE
.\".sp \n(pDu
.\"If you are the only user on your system, skip this and the following
.\"step.
.\".IP 4.
.\"Be sure that all users are logged off the system by typing the command:
.\".DM
.\".II who
.\"	who
.\".DE
.\"This command names all users who are still on the system.
.\".sp \n(pDu
.\"If they have not logged off in a few minutes, send another message.
.\"Repeat the process until
.\".B who
.\"shows no users except yourself.
.\".IP 5.
.\"When all other users have logged off, execute
.\".B /etc/shutdown
.\"as described near the beginning of this tutorial.
.\".IP 6.
.\"If this is the last workday of the month, perform a \fImonthly\fR dump,
.\"to back up the entire system.
.\"Insert the first volume of the correct monthly dump
.\"floppy disk into the floppy drive, after adding today's
.\"date to the label, and type the commands:
.\".DM
.\"	cd /
.\"	find . -type f -print | cpio -oc >/dev/rfha0
.\".DE
.\"This will dump all files
.\"to the raw, 2400-block, floppy-disk device \fB/dev/rfha0\fR.
.\".B cpio
.\".sp \n(pDu
.\"As more floppies are needed,
.\".B cpio
.\"will ask you to insert them.
.\"After you insert the floppy disk, you will have to type the
.\"device name, e.g.,
.\".BR /dev/rfha0 ,
.\"at \fBcpio\fR's prompt.
.\"Be sure to label each floppy disk with its volume number.
.\".IP 7.
.\"If this is the last work day of the week, but not the last
.\"workday of the month, perform a \fIweekly\fR dump.
.\"Prepare the correct weekly dump diskettes, add today's date to the label,
.\"insert the first diskette, and type the command:
.\".DM
.\"	cd /
.\"	find . -type f -newer cpio.weekly -print | cpio -oc >/dev/rfha0
.\"	touch cpio.weekly
.\".DE
.\"This will dump all files that are younger than file
.\".BR cpio.weekly .
.\".IP 8.
.\"If this is neither the last workday of the month nor the last
.\"workday of the week, you will perform a \fIdaily\fR dump.
.\"Prepare the daily dump diskette
.\"with today's day of the week, add today's date to the label,
.\"insert the first diskette into the drive, and type the command:
.\".DM
.\"	cd /
.\"	find . -type f -newer cpio.daily -print | cpio -oc >/dev/rfha0
.\"	touch cpio.daily
.\".DE
.\"This will dump files that are younger than file
.\".BR cpio.daily .
.\".IP 9.
.\".II sync
.\"Type
.\".B sync
.\"to ensure that all buffers are flushed.
.\".IP 10.
.\".II reboot
.\"When you are finished dumping data, reboot the system by typing the
.\"command:
.\".DM
.\"	/etc/reboot
.\".DE
.\".PP
.\"For more information on how to use \fBcpio\fR and
.\".BR find ,
.\"see their respective entries in the Lexicon.
.\".Sh "Restoring Information"
.\".PP
.\".II restore^files
.\".II file^restoring
.\"If you find that a file has been inadvertently destroyed,
.\"you can restore the information to disk from backup
.\"floppy disk.
.\".PP
.\"To restore a file from a compressed \fBtar\fR archive, use the following
.\"commands.
.\"First, select the appropriate back-up disk, insert it into its drive, and
.\"mount it with the following command:
.\".DM
.\"	/etc/mount /dev/fha0 /f0
.\".DE
.\"Next, use the commands \fBzcat\fR and \fBtar\fR to extract the file you want.
.\"For example, if your archive is called \fBbackup.tar.Z\fR
.\"and the file wish to restore is called \fBmyfile\fR, use the following
.\"command to extract it from its archive:
.\".DM
.\"	zcat /f0/backup.tar.Z | tar xf - myfile
.\".DE
.\"The \fBzcat\fR command reads the compressed archive without requiring
.\"that you uncompress it.
.\"The \fBtar\fR command reads the standard input (as indicated by the hyphen `-')
.\"and extracts \fBmyfile\fR from what it reads.
.\".PP
.\"Once you have extracted your file or files, you can unmount the floppy disk
.\"in the usual way and put it away.
.\".PP
.\"To restore information from back-ups created with \fBcpio\fR, the process is
.\"a little more complicated.
.\"To begin, you must first determine the date and time
.\"that the file was last known to have been modified.
.\"From this date, determine on which set of disks
.\"the file was last correctly dumped.
.\"Find the set of floppy disks labeled with that date,
.\"and mount the first one in the set.
.\"For example, if you wish to restore the file
.\".BR myfile ,
.\"use the command:
.\".DM
.\"	cpio -icdv myfile < /dev/rfha0
.\".DE
.\"This assumes that the disks high-density, 5.25-inch floppies that are
.\"in drive 0 (drive A).
.\"See the Lexicon article
.\".B "floppy disk"
.\"for a table that shows which \*(CO device is associated with which
.\"size and density of disk, and which disk drive.
.\"You may have to insert more than one disk from the set of backups
.\"until you find the one that holds the file you want.
.Sh "System Accounting"
.PP
.II "computer time accounting"
.II accounting
.II accounting^reports
The \*(CO system provides two types of computer time
.I accounting
to help you track the use of the system.
Three commands control the accounting and provide reports at various levels of
detail.
.PP
Note that system accounting adds overhead to your system, because your
system has to do more work to record everything it does, and because
the accounting files can quickly grow to unmanageable sizes.
System accounting is useful for \*(CO systems that are being used
by multiple users who must account for (i.e., pay for) their use of the
system, or in other circumstances where it is important to note each user's
activity.
For most systems that support a handful of users, system accounting simply
isn't worth the bother.
.PP
If, however, you decide that you need system accounting, read on.
.Sh "ac: Login Accounting"
.PP
.II accounting^login
Whenever a user logs into the \*(CO system, it records the user's name,
the terminal number, and the date and time of the login.
It also records when he logs out.
.PP
.II login^time
You can use this information to compute the time
each user, or all users, were logged into the system.
The command
.B ac
prints the total of all login times recorded in the accounting file.
An example of the result is
.DM
	Total:	 8357:00
.DE
You can ask for a summary of total login times for each day by typing:
.DM
	ac -d
.DE
An example result would be:
.DM
	Friday November 13:
		Total:	 53:08
	Saturday November 14:
		Total:	 75:36
	Sunday November 15:
		Total:	 73:15
.DE
Finally, you can summarize the times for individual users with the command:
.DM
	ac -p jack ted fred
.DE
This will show the total login times for these users:
.DM
.ta 0.5i 1.5i
	fred	1100:42
	jack	910:41
	ted	641:58
	Total:	2653:21
.DE
Also,
.DM
	ac -pd
.DE
gives the time for each user, for each day that he logged in.
.PP
.II /usr/adm/wtmp
.II "accounting^starting login"
Login accounting is not automatically operational.
The login information is collected only if the file
.B /usr/adm/wtmp
exists.
.PP
To start login accounting if it is not working, type the command
.DM
	>/usr/adm/wtmp
.DE
while logged in as \fBroot\fR.
This creates the file
.B /usr/adm/wtmp
if it does not exist (and destroys existing information if it does) and
thereby enables login accounting.
.PP
To turn off login accounting while it is running, you can type:
.DM
	rm /usr/adm/wtmp
.DE
After you activate login accounting, you should purge
.B /usr/adm/wtmp
periodically as it grows continuously, and on an
active system will eventually consume much disk space.
To purge the current information but leave accounting turned on, type:
.DM
	>/usr/adm/wtmp
.DE
.Sh "sa: Processing Accounting"
.PP
.II accounting^process
.II system^time
.II user^time
.II "real time"
While login accounting tells you how much time a user spends logged into the
system, it does not tell you the individual commands used.
\fIProcess accounting\fR does so.
Under \*(CO, each execution of each command constitutes a separate process.
(\*(CO's ability to maintain a list of processes and swap each
in and out of memory until all are executed, is what gives \*(CO
its multi-tasking capability.)
Process accounting records system time, user time, and real time for each
command executed by each user on the system.
The command \fBsa\fR reports this information for you,
using a format that you set.
.PP
.B sa
has several options, to generate different reports.
When used with no options,
.B sa
lists the number of times each call is made,
the total CPU time, and the total
real time used by the command, ordered by decreasing CPU time.
This is a summary by command; the following gives an example:
.DM
.ta 0.5i 1.25i 1.75i 2.25i
		#CALL	CPU	REAL
	sh	61	1	832
	ld	5	1	7
	ar	5	0	1
	ranlib	3	0	1
	p	16	0	11
	dld	2	0	1
	lc	19	0	1
	cc	4	0	8
	atrun	43	0	1
	find	1	0	0
	ed	1	0	2
		...
.DE
This report has been truncated to save space.
The listing will depend on what commands are used in your system, and the
characteristics of your hardware.
To summarize by user, use the \fB\-m\fR option:
.DM
	sa -m
.DE
The option \fB\-l\fR separates CPU time expended by users
from that expended by the system.
This command
.DM
	sa -l
.DE
produces:
.DM
.ta 0.5i 1.25i 1.75i 2.25i
		#CALL	USER	SYS	REAL
	sh	61	0	1	832
	ld	5	0	0	7
	ar	5	0	0	1
	ranlib	3	0	0	1
	p	16	0	0	11
	dld	2	0	0	1
	lc	19	0	0	1
	cc	4	0	0	8
	atrun	43	0	0	1
	find	1	0	0	0
	ed	1	0	0	2
	cat	4	0	0	1
	rm	3	0	0	0
		...
.DE
This report has been truncated to save space.
To list the user name and the command name, use \fBsa\fR with the option
\fB\-u\fR.
No times or counts are given.
The command:
.DM
	sa -u
.DE
produces output of the form:
.DM
	tj		p
	tj		lc
	tj		find
	tj		pr
	bin		lc
	tj		spin
	tj		sh
	bin		cc
	...
.DE
This report has been truncated and edited to save space.
In practice, it is longer.
The \fB\-u\fR option overrides other options.
.PP
.II "accounting^starting process"
Process accounting is on only if you turn it on.
To turn on process accounting, type the command:
.DM
	/etc/accton /usr/adm/acct
.DE
while logged in as \fBroot\fR.
.II /usr/adm/acct
The file \fB/usr/adm/acct\fR holds the raw accounting information.
.PP
To turn off process accounting, use the same command with no file name:
.DM
	/etc/accton
.DE
If accounting is not on when you type this command, you will get an error
message.
No information is gathered when accounting is turned off.
.PP
When process accounting is in use, the file
.B /usr/adm/usracct
grows with each user command issued.
You should regularly condense or remove the information, to keep the
file from devouring all free space on your disk.
To condense the information, invoke \fBsa\fR with the \fB\-s\fR option.
You must turn off accounting while condensing information.
.PP
.II /usr/adm/savacct
The information summarized by user will appear in
.BR /usr/adm/usracct ,
and information saved by command is placed in
.BR /usr/adm/savacct .
These summarized files are used in future requests to
.BR sa .
After condensing, you can turn accounting back on.
.PP
Additional options give flexibility to the report.
See the entry for \fBsa\fR in the Lexicon for
additional details on these options.
.SH Conclusion
.PP
The following sections of this manual give tutorials to teach you
how to use many of \*(CO's tools and commands.
The Lexicon contains brief synopses of all commands, library routines,
system calls, and macros available under the \*(CO system.
It also includes many technical references and definitions, to help you
with terminology throughout this manual.
