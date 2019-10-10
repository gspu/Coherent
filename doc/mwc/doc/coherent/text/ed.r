.II "ed^tutorial"
.ds TL "ed Interactive Line Editor"
.NH "Introduction to the ed Line Editor" 1
.PP
This tutorial introduces the interactive editor \fBed\fR.
It is intended both for readers who want a
tutorial introduction to \fBed\fR, and those who want to use specific
sections as a reference.
.PP
Related tutorials include those for \fBsed\fR,
the stream editor, and for \fBme\fR, the \*(ME screen editor.
This tutorial assumes that you already understand the basics of
using the \*(CO system, such as what a file is, what it means
to edit text, and how to issue commands to the operating system.
If you not yet know your way around the \*(CO system, we
suggest that you first study the
\fIUsing the COHERENT System\fR, which
appears in the front of this manual.
It covers the basics of using \*(CO and introduces many useful programs.
.Sh "Why You Need an Editor"
.PP
A significant feature of computers is the capacity to store, retrieve,
and operate upon information.
A computer can store many different kinds of information:
programs, computer commands and instructions, data for programs, financial
information, electronic mail, or natural-language text (e.g., French,
English) destined for a manuscript or book.
.PP
\fBed\fR is a
program with which you can enter and edit text on your computer.
You can use \fBed\fR to create or change computer programs,
natural-language manuscripts, files of commands, or any other file
that consists of text that you can read.
.PP
\fBed\fR is designed to be easy to use, and requires little
training to get started.
The fundamental commands are simple, but have
enough flexibility to perform complex tasks.
.Sh "Learning To Use the Editor"
.PP
Practice on your part will help you learn quickly.
The following sections contain examples that illustrate each topic
discussed.
We strongly recommend that you type each example presented
as you encounter it in the text.
Even if you understand the concept presented, performing the example
reinforces the lesson, and you will learn more quickly how to use \fBed\fR.
.PP
In addition to reading the text and doing the examples as you encounter
them in the text, try your own variations on the commands,
and branch out on your own.
Try things that you suspect might work, but are not shown as examples.
.SH "General Topics"
.PP
This section presents the background information you will need
to understand how \fBed\fR works.
.PP
To help illustrate the discussion to follow, log into your \*(CO
system and type the following commands:
.DM
	ed
	a
	this is a sample
	ed session
	.
	w test
	q
.DE
.PP
This example calls \fBed\fR, then uses the \fBa\fR command to add lines to
the text kept in memory.
The period signals the end of the additions.
The \fBw\fR command writes the lines of text to file \fBtest\fR, and
the command \fBq\fR tells \fBed\fR to return to \*(CO.
You will notice that after you type the \fBw\fR command,
\fBed\fR will respond with
.DM
	28
.DE
which is the number of characters in the file.
.PP
Thus, to enter \fBed\fR, simply type
.DM
	ed
.DE
and to exit, type
.DM
	q
.DE
You can also exit by typing
.II "ed^<ctrl-D>"
.BR <ctrl-D> :
that is, hold down the \fBcontrol\fR key on your keyboard,
and at the same time strike the \fBD\fR key.
.PP
Notice that you are issuing two different kinds of commands in the above
example:
the command
.B ed
is given to the \*(CO shell, to invoke the editor;
the rest of the commands are given to the editor.
After \fBed\fR is given the \fBq\fR command, it exits, and
following commands are processed by \*(CO.
.Sh "ed, Files, and Text"
.PP
\fBed\fR works with one file at a time.
With \fBed\fR, you can create a file, add to a file, or
change a file previously created.
.PP
As you use \fBed\fR to create or change files, you will type both
\fItext\fR and controlling \fIcommands\fR into the editor.
Text is, of course, the matter that you are creating or changing.
Commands, on the other hand, tell \fBed\fR what you want it to do.
As you will see shortly, there is a simple way to tell \fBed\fR
whether what you are typing is text or commands.
.PP
\fBed\fR has about two dozen commands.
Almost every one is only one letter long.
Although they may seem terse, they are easy to learn.
You will appreciate the brevity of the commands
once you begin to use \fBed\fR regularly.
.PP
.II "ed^carriage return"
You must end
each command to \fBed\fR by striking the
.B <return>
key.
.if \nX=0 \{\
This key is present on all terminals.
However, the labeling of the key may vary.
It may be called
\fBnewline\fR, \fBlinefeed\fR, \fBenter\fR, or \fBeol\fR, and is larger
than any key on the keyboard except for the space bar. \}
.if \nX=1 \{\
On the IBM PC, it is labeled with an arrow pointing to the left,
and is positioned where the carriage return on an electric typewriter
is usually found. \}
This key will be called
the \fB<return>\fR key in the remainder of this document.
.Sh "Creating a File"
.PP
The example shown above created a file.
Here is another example of file creation \(em here, creating
a file called \fBtwoline\fR:
.DM
	ed
	a
	Two line Example,
	thank you.
	.
	w twoline
	q
.DE
The letter \fBa\fR tells \fBed\fR to add lines to the file.
You are creating a new file with this example; and when \fBed\fR
creates a new file, it is initially empty.
The \fBw\fR command writes the lines you have added to file \fBtwoline\fR.
The command \fBq\fR tells the editor that you
are finished, whereupon it returns to \*(CO.
You can use the \*(CO command
.if \nX=0 \fBcat\fR
.if \nX=1 \fBtype\fR
to list the contents of the new file:
.DM
	cat twoline
.DE
the reply will be:
.DM
	Two line Example,
	thank you.
.DE
Each command used here will be described in detail in later
sections.
.Sh "Changing an Existing File"
.PP
Suppose that a manuscript file of yours needs a few spelling corrections.
\fBed\fR will help you make them.
To begin, simply name the file to correct when you issue the \*(CO command:
.DM
	ed \fIfilename\fR
.DE
where \fIfilename\fR stands for the name of the file that you wish to edit.
For example, the following adds a line to the file \fItwoline\fR, which
we just created:
.DM
	ed twoline
	$a
	This is the third line of the file.
	.
	w
	q
.DE
Listing the file with
.if \nX=0 \fBcat\fR
.if \nX=1 \fBtype\fR
gives:
.DM
	Two line Example,
	thank you.
	This is the third line of the file.
.DE
The command \fB$a\fR tells \fBed\fR to add one or more
lines at the end of the file.
.PP
Correcting the spelling of a misspelled word is easy with \fBed\fR.
You can rearrange groups of words in a manuscript, and you can
move or copy larger portions of text,
such as a paragraph, from one spot to another.
.Sh "Working on Lines"
.PP
\fBed\fR uses the \fIline\fR as the basic unit of
information; for this reason, it is called a \fIline-oriented\fR editor.
A line is defined as a
group of characters followed by an end-of-line character, which is invisible.
When you type out a file on your terminal, each line in the file will be
shown on your terminal as one line.
The commands for \fBed\fR are based upon lines.
When you add material to a file, you will be adding lines.
If you remove or change items, you will do so to groups of lines.
.PP
.II "ed^line, number"
\fBed\fR knows each line by its number.
A line's number, in turn, indicates its position within the file:
the first line is number 1, the second line is number 2, and so on.
.PP
\fBed\fR remembers the line you worked on most recently.
This can help shorten the commands you type,
as well as reduce the need for you to remember line numbers.
The line most recently worked on is called the \fIcurrent\fR line.
\fBed\fR commands use
a shorthand symbol for the current line:
the period \*(Ql\fB.\fR\*(Qr.
.PP
Another shorthand symbol used in \fBed\fR commands is \fB$\fR,
which represents the number of the last line in the file.
.PP
Many of the \fBed\fR commands operate on more than one line at a time.
Groups of lines are denoted by a range of line numbers, which
appears as a prefix to the command.
.Sh "Error Messages"
.PP
If you type a command to \fBed\fR incorrectly, \fBed\fR respond with:
.DM
	?
.DE
This indicates that it has detected an error.
Many times, this error will
be evident to you when you review the command that you just typed.
.PP
If you do not see what the error is, you can get a more lengthy
description by typing to \fBed\fR:
.DM
	?
.DE
It will reply with an error message.
.SH "Basic Editing Techniques"
.PP
This section discusses in more detail
the elementary techniques and commands that you
need to use \fBed\fR.
With the material presented in this
section, you will be able to do most basic editing tasks.
.PP
Again, it is recommended that you type each example.
This will help you understand each example, as well as remember the technique
it demonstrates.
.Sh "Creating a New File"
.PP
To begin, let us presume that you need to create an entirely new file
named \fBfirst\fR.
Perhaps you only want one line in the file, and it is to read
.DM
	This is my first example
.DE
.II "ed^prompt character"
These are the steps that you will need to go through to create this file.
.PP
.PP
The first step is to invoke the \fBed\fR program.
To do this, simply type
.DM
	ed
.DE
Remember that you must end each line of commands or text line by
pressing the \fB<return>\fR key,
because \fBed\fR will not act upon it until you do.
Thus, you invoke the editor by typing \fBed\fR
and a \fB<return>\fR.
Notice that these two characters must be lower case.
.PP
.II "ed^adding lines"
\fBed\fR is now ready for commands.
The first command that you will use is the append command
.BR a .
This tells \fBed\fR to add lines to the text in memory,
which will later be written to the file.
The number of lines that \fBed\fR can hold in memory depends upon
the amount of memory in your computer.
.II "ed^sed"
For editing very large files, you should use
.BR sed ,
the \*(CO stream editor, which is described in its own tutorial.
.PP
\fBed\fR will continue to add lines until you type a line
that contains \fIonly\fR a period.
While it is adding lines, \fBed\fR does not recognize commands.
.PP
.II "ed^. (dot)"
After you issue the \fBa\fR command, you can type the lines to be
included, concluding with a line that consists only of a period.
This special line signals \fBed\fR that you want to stop appending lines.
The information that you have typed so far is:
.DM
	ed
	a
	This is my first example
	.
.DE
Next, you must tell \fBed\fR to write the edited text into a file.
Do so by issuing the write command
.BR w ,
plus the name of the file that is to hold the edited text.
For example,
if you wish to store this example in a file named
\fBfirst\fR, issue the command:
.DM
	w first
.DE
\fBed\fR will write the file and tell you how many characters were
written, in this case 25.
.PP
Finally, to quit the editor issue the quit command:
.DM
	q
.DE
The commands you type after this will be interpreted
and acted upon by \*(CO.
.PP
Now, review the example in its entirety.
First you invoked \fBed\fR by typing
.BR ed
at the \*(CO prompt.
Then you issued the add command \fBa\fR
to add lines to the file.
added lines with the \fBa\fR command,
and finished the adding by typing a line that consists only of a period.
You then wrote the editing text into a file by issuing the write command
.BR w ,
and finally you exited from \fBed\fR by issuing the quit command \fBq\fR.
The complete example is:
.DM
	ed
	a
	This is my first example
	.
	w first
	q
.DE
\fBed\fR replied to the
.B w
command by printing the number of characters it wrote into the file.
After you typed
.B q ,
\*(CO prompted you for a command again.
.Sh "Changing a File"
.PP
Suppose that you wish to change the file that you have just created:
you want to add two more lines to the file so that the original line
will be sandwiched between the new lines.
You want the file to contain:
.DM
	Example two, added last
	This is my first example
	Example two, added first
.DE
You will do this with \fBed\fR using two new commands.
.PP
.II "ed^file, name, in ed command"
Again, you start by telling \*(CO to run \fBed\fR.
This time, however, you must type the name of the
file that you are changing after the characters \fBed\fR:
.DM
	ed first
.DE
\fBed\fR will remember this file name for later use with the \fBw\fR command.
.PP
\fBed\fR reads the file in preparation for editing, and tells you the number
of characters that it read in, again 25.
.PP
.II "ed^current, line"
After reading the file, \fBed\fR automatically sets the
current line to the last line read in.
.PP
Now, add the third line shown in the second example by entering:
.DM
	a
	Example two, added first
	.
.DE
This resembles the first example.
In that case, however, the file had no information, whereas now it does.
How did \fBed\fR know where to add the lines?
.PP
.II "ed^current, line"
The \fBa\fR command adds lines after the \fIcurrent line\^\fR.
When \fBed\fR reads a file, it initially sets the current line to the
last line read in;
therefore, the \fBa\fR command added the new line after the last line.
.PP
The current line is used implicitly or explicitly by most
commands, so it is helpful to know where it is.
In general, the current line is left at the last line \fBed\fR has processed.
If you lose track of the current line,
you can ask \fBed\fR to tell you where it is, as you will see shortly.
.PP
To add the very first line to the second example, you will use yet
another command, the insert command \fBi\fR.
This command is identical to
the \fBa\fR command, except that it inserts lines \fIbefore\fR the
current line rather than after it.
.PP
Another word about the current line.
After an \fBa\fR
command finishes, the current line is the last line added.
Thus, after the
addition of \*(QLExample two, added first\*(QR above, the current line is
now the last line in the file.
So, if you were to do the \fBi\fR command immediately, you would
be adding lines just before the last line, which is not what you want to do.
.PP
Nearly every \fBed\fR command is flexible enough to allow you to specify the
line upon which the command is to operate.
Now you can complete the second example:
.DM
	1i
	Example two, added last
	.
.DE
.II "ed^inserting lines"
The numeral \fB1\fR before the \fBi\fR tells \fBed\fR to
insert lines before the first line in the file.
The line-number prefix is used frequently, and applies to nearly every command.
.PP
Now, to finish the second example and save it into the same file, type:
.DM
	w
	q
.DE
Note that the file name was left off the \fBw\fR command.
\fBed\fR remembers the name of the file that you began with,
and uses that name if none is used with the \fBw\fR command.
Therefore, the edited text is written back into file \fBfirst\fR.
Note, too, that the previous contents of the
file \fBfirst\fR are lost when you write the new file \fBfirst\fR.
Alternatively, you can type:
.DM
	w second
.DE
This leaves the contents of \fBfirst\fR unchanged and creates a new file called
\fBsecond\fR.
.PP
In case you forget, \fBed\fR can tell you the name of the file
with which you began.
Simply type the command:
.DM
	f
.DE
If you had used \fBf\fR any time while working on this second example,
\fBed\fR would have replied:
.DM
	first
.DE
Remember to use the
.B q
command to leave \fBed\fR and return to \*(CO.
.Sh "Printing Lines"
.PP
As you use \fBed\fR to edit a file, you will find it most useful to print
sections of the file on your terminal.
This helps you see what you
have done (and sometimes what you have not done), and helps you pinpoint where
you wish to make changes.
.PP
.II "ed^print command"
The print command \fBp\fR
prints the current line unless you specify a line number.
.PP
Continuing with the example begun above, when you type the commands
.DM
	ed first
	p
.DE
\fBed\fR replies by printing
.DM
	Example two, added first
.DE
which is the last line in the file named \fBfirst\fR from the previous
example.
.PP
Again, like the commands
.B i
and
.BR a ,
if you want \fBed\fR to print a line
other than the current one, just prefix the \fBp\fR
command with a line number.
Thus, if you want to print the second line in the file, type:
.DM
	2p
.DE
\fBed\fR will reply with:
.DM
	This is my first example
.DE
.II "ed^line, number ranges"
If you wish to print more than one line of a file,
you can tell \fBed\fR to print a \fIrange\fR of line numbers:
type the numbers of the first and last lines you wish to see,
separated by a comma.
For example, to print all three lines in the second example, type:
.DM
	1,3p
.DE
\fBed\fR responds by printing all lines.
This same principle applies to other commands.
The print command can also appear after other commands such as \fBs\fR or
.BR d ,
which are discussed later in this section.
.Sh "Abbreviating Line Numbers"
.PP
.II "ed^$"
.II "ed^*"
\fBed\fR recognizes some shorthand descriptions for certain line numbers.
The number of the last line can be represented by the dollar sign
.BR $ .
Thus, the command
.DM
	1,$p
.DE
prints every line in the file.
The advantage of this shorthand is that
the command as typed works for any file, regardless of its size.
This construct of \fB1,$p\fR is used often enough that it has an
abbreviation of its own:
.DM
	*p
.DE
The number of the current line can also be abbreviated by using the
period or dot in the place of a line number.
To print all lines from the beginning of the file through the current line,
type:
.DM
	1,.p
.DE
To print all lines from the current line through the end of the file, type:
.DM
	.,$p
.DE
The special symbol
.B &
prints one screenful of text.
Simply type:
.DM
	&
.DE
This is equivalent to:
.DM
	.,.+22p
.DE
If there are fewer than 23 lines between the current line and the end
of the file,
it is equivalent to
.DM
	.,$p
.DE
.II "ed^$"
All forms of the \fBp\fR command change the current
line to the last line printed.
The command
.DM
	.,$p
.DE
after printing changes the current line to the last line of the file.
.Sh "How Many Lines?"
.PP
You can easily see the current line with \fBp\fR.
Type:
.DM
	p
.DE
This tells \fBed\fR to print the current line.
On your terminal, try the command:
.DM
	.p
.DE
You will see that it does the same thing as \fBp\fR.
.PP
.II "ed^="
To discover how large your file is,
just type:
.DM
	=
.DE
\fBed\fR will reply by typing the number of lines in the file.
.PP
.II "ed^.="
To find the number of the current line, use the \fBdot equals\fR command:
.DM
	.=
.DE
\fBed\fR responds with the number of the current line.
.Sh "Removing Lines"
.PP
.II "ed^removing lines"
Editing means removing lines of text, as well as adding them.
To illustrate how \fBed\fR lets you remove lines of text,
create another example file with \fBed\fR:
.DM
	ed
	a
	This is the first line.
	The second line is good.
	However, line three is bad.
	line four wishes to go away.
	line 5 similarly wants to be forgotten,
	as does line 6.
	the next to last line stays.
	as does the last line in the file.
	.
	w example3
	q
.DE
This creates a file named \fBexample3\fR.
.PP
Now, you can practice removing lines that you no longer want.
Begin editing the file by typing:
.DM
	ed example3
.DE
Now, print the contents of the file by typing:
.DM
	1,$p
.DE
Our first task is to delete lines 3 through 6.
First, delete line 3, then print the entire file again.
.DM
	3d
	1,$p
.DE
and \fBed\fR will respond with
.DM
	This is the first line.
	The second line is good.
	line four wishes to go away.
	line 5 similarly wants to be forgotten,
	as does line 6.
	the next to last line stays.
	as does the last line in the file.
.DE
.II "ed^deleting lines"
Notice that the original file's third line is no longer there.
Line 3 is now what used to be line 4.
Remember that the line
numbers \fIalways\fR begin with 1 for the first line of the file and
progress consecutively even after the file has been changed.
Thus, deleting a line will change the line number of each line from the deleted
line to the the last line in the file.
.PP
You still need to remove three more lines.
You can do this with one command:
.DM
	3,5d
.DE
Again, type
.B *p
to print the contents of the file:
.DM
	This is the first line.
	The second line is good.
	the next to last line stays.
	as does the last line in the file.
.DE
Finally, write the updated file and quit:
.DM
	w
	q
.DE
This illustrates how to delete lines, both singly and in a group.
.Sh "Abandoning Changes"
.PP
Sometimes, you may make a mistake;
rather than damage your file with badly edited text,
you may wish to abandon what you have done and begin all over again.
You can do so by using the \fBq\fR
command in a different fashion than is shown above.
.PP
If you tell \fBed\fR to
.B q
before you tell it to write the file with \fBw\fR,
you abandon any changes made since beginning editing.
However, to prevent you from accidentally selecting this option, \fBed\fR
checks to see if you have made any changes to the file; and if you have,
it responds with a question mark \*(Ql\fB?\fR\*(Qr.
To tell \fBed\fR that you know what you are doing and really do wish
to abandon the edited file,
reply with a second \fBq\fR.
\fBed\fR will then quit and return you to \*(CO.
.PP
You can avoid the question mark prompt by typing the upper-case
.B Q
rather than lower-case \fBq\fR:
\fBed\fR will exit without regard to unsaved changes.
You can also exit from \fBed\fR by typing the end-of-file key
\fB<ctrl-D>\fR.
.Sh "Substituting Text Within a Line"
.PP
.II "ed^changing lines"
If you type a line incorrectly, or later wish to
rearrange some words or symbols within it, you know enough about \fBed\fR
now to do so.
You only need to delete the line with the delete command \fBd\fR
and re-type the line with the insert command \fBi\fR.
To see how this is done, prepare the file
.BR example4 ,
as follows:
.DM
	ed
	a
	Software technology today has
	adbanced to the point that large
	software projects unherd of in
	earlier times are undertaken and
	.
	w example4
	q
.DE
This example has two misspelled words.
We will correct each of them using different \fBed\fR features.
.PP
The first method will be the direct way that you probably can anticipate.
Give the following commands to the editor exactly as shown:
.DM
	ed example4
	2d
	i
	advanced to the point that large
	.
.DE
These commands use the delete command \fBd\fR to delete the second line,
and then uses the insert command \fBi\fR to insert the correct new line
in its place.
.PP
Use the command
.DM
	*p
.DE
to verify that the file now contains:
.DM
	Software technology today has
	advanced to the point that large
	software projects unherd of in
	earlier times are undertaken and
.DE
.II "ed^substitute command"
You can also use
a second method to change the spelling of a word.
This is the substitute command
.BR s .
This command is very powerful, and probably is used more frequently
than any other \fBed\fR command.
.PP
.II "ed^pattern"
The substitute command
.B s
is more complex than commands we have discussed so far, in
that it has more elements, as follows:
First is a line number or optional range of line numbers.
Then comes the letter \fBs\fR, to invoke the substitute command itself.
Third comes two \fIpatterns\fR or \fIstrings\fR, which
are set off from the rest of the command and
from each other with the \fBslash\fR character.
For example:
.DM
	1,$s/pattern1/pattern2/
.DE
Here, \fIpattern1\fR represents the string that you want \fBed\fR to
replace, and \fIpattern2\fR is the string that \fBed\fR is to substitute
in place of \fIpattern1\fR.
Note that three slashes separate the two patterns from
the \fBs\fR, from each other, and from the end of the line.
These slashes must always be present.
.PP
With this command, you can correct the second spelling error in the
example4:
.DM
	3s/herd/heard/
	p
.DE
\fBed\fR replies:
.DM
	software projects unheard of in
.DE
Note that these two command lines can be condensed to one:
.DM
	3s/herd/heard/p
.DE
The meaning of these commands is:
on the third line of the file,
change \fBherd\fR to \fBheard\fR and, when finished, print the entire line.
Without the \fBp\fR command, \fBed\fR will change the line as you
direct, but will not show you the new line.
It is a good idea to
print lines that you substitute in this manner until you gain in
confidence with \fBed\fR.
Some \fBed\fR experts always print the lines after substitution.
.PP
Type
.DM
	.
	w sample.text
	q
.DE
to stop entering text, then save the newly typed text into file
.B sample.text
and exit (``quit'') from
.BR ed .
.PP
After these two changes, the file looks like this:
.DM
	Software technology today has
	advanced to the point that large
	software projects unheard of in
	earlier times are undertaken and
.DE
Although the above example substitutes one word for another,
note that the \fBs\fR command can replace any consecutive group
of characters with any other:
it may be one word, several words (including the space characters
that separate them), or a fragment of a word.
.PP
Because \fBed\fR looks for patterns rather words, you should keep in mind
that it may find the wrong pattern.
For example, assume that the current line in a file is
.DM
	let not rain fall on a parade
.DE
and instead you want to say:
.DM
	let not rain fall on the parade
.DE
You command \fBed\fR to:
.DM
	s/a/the/p
.DE
and are shocked to discover that the result is:
.DM
	let not rthein fall on a parade
.DE
A better command to
give \fBed\fR would have been a substitute command that substituted the
letter \fBa\fR preceded and followed by a space:
.DM
	s/ a / the /p
.DE
Another correct way to do this task is to indicate within the
substitution command which of several possible matches within the line is
to be substituted.
In our example, it is actually the third \fBa\fR that we
are trying to match, so we could have used the special form of the command
.DM
	s3/a/the/p
.DE
to get \fBed\fR to select the one we wanted.
.Sh "Undoing Substitutions"
.PP
If you did change \fBa\fR to \fBthe\fR inappropriately, you can retract the
substitution by issuing the undo command
.DM
	u
.DE
before you move on to another current line.
.PP
To illustrate this, enter this example:
.DM
	ed
	a
	let not rain fall on a parade
	.
	w undo
	q
.DE
Now, perform the substitution with
.DM
	ed undo
	s/a/the/p
.DE
which will result in:
.DM
	let not rthein fall on a parade
.DE
To retract the substitution, simply type:
.DM
	u
	p
.DE
This undoes the substitution and prints the result.
.PP
Note that the undo command
undoes the substitution only on the current line.
Remember that if your substitution command operated over a range of lines,
when it finishes the current line is the last one upon which the substitution
was made.
Thus,
if you made an inappropriate substitution over a range of lines,
the undo command will fix only the last line.
.Sh "Global Substitutions"
.PP
As you saw with the above examples, the
.B s
command substitutes only the \fIfirst\fR occurrence of the requested pattern
on a given line.
.PP
.II "ed^global substitute"
A different form of the substitute command finds
every occurrence of the indicated string on a line.
Simply add the letter
.B g
for \fIglobal\fR after the third slash in the substitute command,
and \fBed\fR finds and changes every one:
.DM
	s/pattern1/pattern2/g
.DE
So, if the current line contains a phrase:
.DM
	a rose is a rose is a rose
.DE
and we tell \fBed\fR to substitute
.DM
	s/a/the/g
.DE
the line is changed to:
.DM
	the rose is the rose is the rose
.DE
Again, be careful that your command does not inadvertently match
all or part of a word that you wish to keep untouched.
.Sh "Special Characters"
.PP
.II "ed^characters, special"
In its first two parts, the substitute command uses
some special punctuation characters.
They will be discussed below in detail.
However, you should be aware of these
characters and avoid them until you progress to the advanced section, for
unless used properly, they will give you undesired results.
The characters are:
.DM
	[ ^ $ * . \e &
.DE
They are used
.if \nX=0 in \fBed\fR and other \*(CO programs
to form complex patterns.
.Sh "Ranges of Substitution"
.PP
Perhaps you need to change several lines that have the same misspelling
or need the same editorial change.
\fBs\fR can do that for you also.
Simply prefix the command \fBs\fR with the line-number range as
you would do with \fBp\fR.
Borrowing the \*(QLrose\*(QR example again, if the saying were typed:
.DM
	a rose is
	a rose is
	a rose
.DE
then you could do the same change as before, but across the entire file by
typing
.DM
	1,$s/a/the/
.DE
Note that the
.B g
after the
.B s
command has been omitted here, because you know that the string that
you want to change appears only once on each line.
.PP
If some of the lines do not have the string you want to change,
\fBed\fR will not complain that the string is missing.
However, if none of the lines in the
range has the requested string, \fBed\fR will print a \fB?\fR.
.SH "Intermediate Editing"
.PP
.II "ed^commands, advanced"
This section introduces the more advanced command features of \fBed\fR.
Although you have already learned enough about \fBed\fR
to become productive, this section covers additional
features that will increase your editing power considerably.
.PP
This section discusses the following topics:
relative line numbering, moving blocks of text,
finding strings, using special characters in substitution
and search commands, processing global commands, and marking lines.
.Sh "Relative Line Numbering"
.PP
.II "ed^line, numbers, relative"
As discussed in the previous section, most
commands allow you to use
line numbers to control their range of operation.
Before the command you can enter a single line number; for example:
.DM
	1p
.DE
This, of course, prints the first line of the file.
You may also specify a \fIrange\fR of line numbers,
by entering two numbers separated by a comma.
For example,
if the file contains at least ten lines, the command
.DM
	1,10p
.DE
prints the first ten lines of the file.
.PP
.II "ed^. (dot)"
The period (dot) always represents the number of the current line.
For example, to print the file from the first line through the
current line, just type:
.DM
	1,.p
.DE
A command used without a line number always acts on the current line only.
For example, typing
.DM
	p
.DE
is equivalent to typing:
.DM
	.p
.DE
.PP
.II "ed^+"
.II "ed^-"
There is yet another level of shorthand to line numbering \(em
the plus and minus characters.
These characters indicate \fIoffsets\fR from the current line.
For example, the command
.DM
	.+3p
.DE
prints the third line after the current line.
Likewise, the command
.DM
	.-1p
.DE
prints the line that precedes the current line.
Note that using a line offset changes the current line to the one
addressed.
Thus, after the above command is executed, the current line will
be the one that preceded the original current line.
.PP
You can abbreviate this notation still further by leaving out the dot.
The commands
.DM
	+p
	-p
.DE
do the following:
First, 
.B ed
advances to the next line and prints it;
then it backs up to the previous line (which was the
original current line) and printing it.
.PP
You can place several of these commands on one line to move the
current line multiple lines.
To back up three lines and then print, type:
.DM
	---p
.DE
Note that
in the absence of any other command,
\fBed\fR defaults to the \fBp\fR command.
Thus
.DM
	---
.DE
is equivalent to
.DM
	---p
.DE
and
.DM
	5
.DE
is identical to:
.DM
	5p
.DE
The print command has one more abbreviation.
If \fBed\fR is expecting a command from you
and you type nothing except \fB<return>\fR, \fBed\fR interprets this as a
command to advance the current line to the next line and print it.
This action is equivalent to
.DM
	+
.DE
or
.DM
	.+1
.DE
\fB<return>\fR is the shortest command in \fBed\fR.
.PP
All of the abbreviations for line numbers can be used by other commands that
expect a range of line numbers.
For example, if you want to delete five lines
centered about the current line, you could type:
.DM
	.-2,.+2d
.DE
and you would get your wish.
.PP
Note that
.B ed
does not allow you to specify a line number that is beyond the range of the
file; this is regardless of whether you are typing a line number or any
form of abbreviated line numbering.
For example,
suppose the current line is the last line in the file
and you type:
.DM
	+
.DE
This tells \fBed\fR to \*(QLadvance one line then print\*(QR;
however, this is impossible
because you are at the last line of the file, so there is no next line
to print.
When you request an impossible line number,
\fBed\fR replies by printing a question mark.
Note, however, that the current line is always be valid so
long as the file has at least one line in it.
Thus, unless the file is empty, the command
.DM
	.
.DE
will never give an error message.
.Sh "Changing Lines"
.PP
Earlier, an example of spelling correction was solved two ways.
The first way was the clumsy way of deleting a line and
retyping the entire line.
This strategy means much work to change a single letter,
so the substitute command was introduced instead.
.PP
On occasion, however, it is handy to be able to
change lines en masse \(em as was done by deleting then inserting.
\fBed\fR provides this power with the change command
.BR c .
In general terms,
.DM
	\fIm\^\fP,\fIn\fPc
	new lines
	to be inserted
	.
.DE
removes lines \fIm\fR through \fIn\^\fR, and insert new lines up to
the period in place of them.
.Sh "Moving Blocks of Text"
.PP
.II "ed^move, blocks of text"
When handling text, you will often need to shift a block of text from
one position to another.
In a manuscript, for example, you may need to rearrange the order of
paragraphs to increase clarity.
In a program, you may need to rearrange the order in which
procedures appear.
.PP
To allow you to do this easily, \fBed\fR provides a move command
.B m
that moves a block of text from one point in the file to another.
.PP
.B m
is different from the other commands that we have discussed
so far, in that line numbers follow as well as precede the \fBm\fR
command itself.
The line number that follows the command
gives the line \fIafter\fR which the text is to be moved.
So, the general form of the move command is
.DM
	\fIb\^\fP,\fIe\^\fPm\fId\fP
.DE
which means \*(QLmove lines \fIb\fR through
\fIe\fR to after line \fId\^\fR\*(QR.
.PP
To see how this works, first build the following file:
.DM
	ed
	a
	    This is a paragraph of natural language
	text. Due to stylistic considerations, it
	really should be the second paragraph.
	    If you can read this paragraph first,
	the text has been properly arranged, and
	our move example has been successfully done.
	.
	w example5
	q
.DE
The file \fBexample5\fR contains two paragraphs, each three lines long.
We will now move the first paragraph to after the second paragraph.
.PP
You can do this in either of two ways:
you can move the first paragraph to after the second paragraph, or you
can move the second paragraph to before the first paragraph.
Either gives the same result, but the commands are somewhat different.
To shift the first paragraph to after the second paragraph, type:
.DM
	ed example5
	1,3m$
	*p
	Q
.DE
Remember that \fB$\fR always represents the last line in the file.
The result is:
.DM
	    If you can read this paragraph first,
	the text has been properly arranged, and
	our move example has been successfully done.
	    This is a paragraph of natural language
	text. Due to stylistic considerations, it
	really should be the second paragraph.
.DE
To move the second paragraph to before the first, type:
.DM
	4,6m0
.DE
.II "ed^line, number zero"
Note that the destination is 0, which means that the text is
to be moved to immediately after line 0.
Because there is no line number 0,
the move command interprets this to mean the beginning of the file.
.PP
Of course, in our small example, line
number abbreviations and knowledge of the current line
may be used in a number of different ways to perform exactly the same action.
For example,
.DM
	1,3m.
.DE
says to move lines 1 through 3 of the file to the line after the current
line.
When you invoke \fBed\fR, it always sets the line number to the last
line in the file.
Thus, this form of the command has the same effect as the previous forms.
.PP
If the destination of a move command is not specified, \fBed\fR assumes the
current line.
Therefore, the command
.DM
	1,3m
.DE
also repositions the first paragraph correctly.
.PP
The move command changes the line numbers in the file,
although the number of lines in the file remains the same.
The different forms of the move command will, however, yield different
settings for the current line.
.PP
After a move command, the current line becomes the number of the last line
moved.
Thus, if you moved the first paragraph to after the second paragraph,
the current line will be reset to the last line in the file \(em
the original line 3.
However, if you moved the second paragraph to before the first paragraph,
the current line would be reset to line 3 \(em which was originally
the last line in the file.
.Sh "Copying Blocks of Text"
.PP
.II "ed^copying blocks of texts"
The transfer command \fBt\fR resembles
the move command, except that it copies text rather than moving it.
When you move text, it is erased from its original position.
When you copy text, however, the text then appears both in its
original position and in the position to which you copied it.
\fBed\fR uses the term \fItransfer\fR rather than \fIcopy\fR
because the command \fBc\fR is already used as the change command.
.PP
The form of the transfer command is as follows:
.DM
	\fIb\^\fP,\fIe\^\fPt\fId\fP
.DE
This means to transfer (copy) the group of lines that begins
with \fIb\fR and that ends with \fIe\fR (inclusive) to after line \fId\fR.
.PP
After copying the text, \fBed\fR sets the current line to the last line copied.
.Sh "String Searches"
.PP
.II "ed^line, locators"
The methods of line location that have been discussed
to this point all involve line numbers.
They specified an absolute line number, a relative line number,
or a shorthand symbol such as \fB.\fR or \fB$\fR.
.PP
Often, however, line numbers are not useful, because there is
no easy way to tell what number a line has,
how many lines ago a block of text began, and so on.
.PP
\fBed\fR's solution to this problem is to
locate a line by asking \fBed\fR to search for a pattern of text.
\fBed\fR begins searching on the line that follows the current line, and
looks for a line that matches the specified pattern.
If it finds a line that contains the requested pattern, \fBed\fR
resets the current line to that line.
.PP
If \fBed\fR encounters
the end of the file before it finds a match, \fBed\fR jumps to the
first line in the file, and continues its search from there.
If it finds no match by
the time it returns to the line where the search began,
\fBed\fR gives up and issues an error message \(em
the question mark \fB?\fR.
.II "ed^?"
Remember, if you type a question mark in response to an error message,
\fBed\fR will tell you in more detail what the error is.
.PP
What does it mean to \*(QLmatch\*(QR a pattern?
The simplest meaning is that two patterns are the
same \(em the strings have exactly the same characters in
exactly the same order.
To see how this works, type the following to create file
.BR example6 :
.DM
	ed
	a
	    This is an example that we will
	use for string searching.  There
	is much natural language here as well
	as some genuine arbitrary strings.
	890,;+	 foxtrot
	qwertyuiop ##
	.
	w example6
	q
.DE
Now, to locate and print
any line contains the pattern \fBfox\fR, type:
.DM
	ed example6
	/fox/p
.DE
In response, \fBed\fR prints the line:
.DM
	890,;+	 foxtrot
.DE
Also, you can use string expressions to print a range of lines.
For example:
.DM
	/This/,/much/p
.DE
This prints:
.DM
	  This is an example that we will
	use for string searching. There
	is much natural language here as well
.DE
That is, it printed all lines from the first line that contains the
pattern \fBThis\fR through the first line that contains the pattern
\fBmuch\fR.
.PP
Pattern searches can also be combined with relative line numbers.
If you have a Pascal program file with several
procedures in it, but you find that you need to rearrange the procedures,
you can combine the power of the move command with the string searches.
.DM
	PROCEDURE A;
	...
	...
	PROCEDURE B;
	...
	...
	PROCEDURE C;
.DE
Assume that the section of text that begins with \fBPROCEDURE A\fR should
follow the line that contains \fBPROCEDURE B\fR.
The following command moves the text properly:
.DM
	/PROCEDURE A/,/PROCEDURE B/-1m/PROCEDURE C/-1
.DE
This commands \fBed\fR (1) to locate the chunk of text that begins
with a line containing the pattern \fBPROCEDURE A\fR and ends with the
line just before the first line that contains the pattern \fBPROCEDURE B\fR,
and then (2) move that text to just before the first line that contains
the pattern \fBPROCEDURE C\fR.
As you can see, you can pack a lot of information into one \fBed\fR command.
.PP
Let's look at this command in more detail, to see exactly how it works.
First, remember that the move command \fBm\fR is defined as
.DM
	\fIb\^\fP,\fIe\^\fPm\fId\fP
.DE
where \fIb\fR indicates the first line of the text to be moved, \fIe\fR
indicates the last line of the text to be moved, and \fId\fR indicates the
line that the moved text is to follow.
Thus, \fIb\fR corresponds to the number
of the line that contains
\fBPROCEDURE A\fR and is the first line of the procedure
in question.
\fIe\fR, however, corresponds to the line before
the \fBPROCEDURE B\fR begins, by virtue of the \-1.
Here is an example of mixing pattern searches with relative line
numbers, as mentioned above.
Thus, you have found the beginning and ending lines of procedure A.
.PP
The final string search locates the first line of subroutine C.
The move command normally moves text to \fBafter\fR the given line;
and because we wish to move the text to \fIbefore\fR the line
that contains \fBPROCEDURE C\fR, we must include the \-1 to move the
text up one line.
.Sh "Remembered Search Arguments"
.PP
As discussed earlier, line numbers may be abbreviated in many ways.
They may be entered as \fB.\fR, or \fB+\fR, or \fB-\fR, and certain
combinations of these.
With some commands, pressing \fB<return>\fR tells
.B ed
to use the current line number.
.PP
\fBed\fR encourages you to abbreviate the search string.
If you enter no string between the slashes
in a search or substitution, then \fBed\fR
uses the last-used search string.
A common use is
in the global substitution command (which will be discussed
in detail later in this section):
.DM
	g/please remove this string/s// /p
.DE
This does not quite remove it, but replaces it with a blank.
The last-used
string can be specified by a string search, a substitute command, or a reverse
string search (also discussed later in this section).
Also, the remembered search argument may also be used in any one of these.
You can use the
remembered search feature to \*(QLwalk\*(QR through the file, finding the next
occurrence of a remembered search pattern.
.Sh "Uses of Special Characters"
.PP
As powerful as the line locator seems, some features are even
even more powerful.
These will be discussed in the Expert Editing section, below.
However, these more
powerful capabilities depend upon certain punctuation marks used in a
special way.
As you use the line locator (as well as the substitute
command), be aware of these following characters:
.DM
	[ ^ $ * . \e &
.DE
They have special significance to \fBed\fR when they appear in a string
search or a substitution pattern.
.PP
If you need to use one of these characters without invoking its special
meaning, precede it with a backslash \*(Ql\e\*(Qr.
This tells \fBed\fR not to interpret the character in a special way.
.PP
For example, to find a backslash character, type the search
command:
.DM
	/\e\e/
.DE
.II "ed^backslash"
If any of these characters is to be used in another context,
for example,
within lines that you are adding with the \fBa\fR command, it should
.I not
be preceded with the backslash.
Only use the backslash to hide the meaning when
it appears within the string search command, or within the first part
of the substitution command.
.Sh "Global Commands"
.PP
.II "ed^global, command"
The global commands
.B g
and
.B v
let you repeat commands on all lines within a specified range.
For example, to print all lines that contain the
word \fBexample\fR, type:
.DM
	g/example/p
.DE
The global command can prefix almost any command.
For example, the following
command deletes all lines that contain three consecutive plus signs:
.DM
	g/+++/d
.DE
Likewise, the command
.DM
	g/foxtrot/.-2,.+2p
.DE
prints the five lines that surrounds any line that contains the word
\fBfoxtrot\fR.
.PP
A common use of the global command is to perform global substitution.
The command
.DM
	g/PROCEDURE/s/PROCEDURE/PROC/gp
.DE
performs the substitution on each line that contains the string
\fBPROCEDURE\fR and prints the resulting line.
.PP
This may appear similar to the command
.DM
	1,$s/PROCEDURE/PROC/gp
.DE
but is different in that the global command prints each of the changed
lines, whereas the substitute command prints only the last line changed.
Also, the method of operation of these two commands is different.
.PP
A related command
.B v
performs much the same task, but
executes the commands only for lines that do \fInot\fR contain
the specified string.
Thus, to print all the lines that do not have the letter \fBw\fR, use:
.DM
	v/w/p
.DE
For more sophisticated uses of the \fBg\fR and \fBv\fR commands and how
they work, see the section on Expert Editing.
.Sh "Joining Lines"
.PP
.II "ed^joining lines"
What do you do if you inadvertently hit \fB<return>\fR as you are adding
lines and need to combine the two lines?
.DM
	ed
	a
	Look out, I seem to have hit ret
	urn in the
	middle of a word and don't know
	what to do!
	.
	w rid
	q
.DE
Rather that retyping the entire line, you can use the join command \fBj\fR:
.DM
	ed rid
	1,2j
	1,$p
.DE
This gives:
.DM
	Look out, I seem to have hit return in the
	middle of a word and don't know
	what to do!
.DE
If no line number is specified, \fBj\fR joins the current line and the
following line.
If a single line number is specified, join operates on
that and the following line.
.PP
Several lines can be joined by using the form of the command:
.DM
	\fIa\^\fP,\fIb\fPj
.DE
This joins lines \fIa\fR through \fIb\fR into one line.
Likewise, the command
.DM
	1,$j
.DE
joins all the lines in the file into one line.
Then, the command \fB.p\fR or
.B p
prints the entire file.
.PP
Note that the command
.DM
	3j
.DE
does the same job as the command
.DM
	3,4j
.DE
The join command generates its own second line number if none is
specified, so that the command
.DM
	\fIn\^\fPj
.DE
is equivalent to
.DM
	\fIn\^\fP,\fIn\^\fP+1j
.DE
where \fBn\fR is a line number.
This command is the only one that
interprets a missing line number this way.
.Sh "Splitting Lines"
.PP
.II "ed^spliting lines"
You can split one line into two with the substitute command
.BR s .
To illustrate, suppose you typed in the following commands:
.DM
	ed
	a
	This line wants to be two, with this second.
	.
	w split
	q
.DE
To perform the split, type:
.DM
	ed split
	s/two, /two,\e
	/p
	*p
	w
	q
.DE
The line split is caused by the backslash that precedes the \fB<return>\fR.
This tells \fBed\fR that the \fB<return>\fR does not terminate the
command, but that it is part of the substitution.
The contents of file \fBsplit\fR are now:
.DM
	This line wants to be two,
	with this second.
.DE
.Sh "Marking Lines"
.PP
As you edit a manuscript or program,
it is sometimes handy to be able to leave a
``bookmark'' in the text for later reference.
\fBed\fR provides this feature with the mark command
.BR k .
To mark the next line that has the word \fBfind\fR, use
.DM
	/find/ka
.DE
where the letter \fBa\fR is the mark.
To print the line that has been so marked, use:
.DM
	'ap
.DE
You can place
these references anywhere that a line number is expected.
.PP
The mark must be one lower-case letter.
Also, each mark is associated with one line.
Marking a line with the \fBk\fR command does not
change the current line.
.PP
Marks can be especially handy when you move paragraphs with the
\fBm\fR command.
They give you a chance to review the sections
that you will be moving before you do the move.
.PP
For example,
suppose that you have a manuscript with a paragraph that must be
moved to a different part of the document.
Create the following example:
.DM
	ed
	a
	    This is a paragraph, first line, that
	needs to be moved.
	text
	text
	And this is the last sentence of the paragraph.
	    Next paragraph begins here.
	text
	text
	text
	    This is the spot that we want the paragraph
	to precede.
	.
	w example7
	q
.DE
Now, place three marks to help with the move:
.DM
	ed example7
	/first line,/ka
	/Next paragraph/kb
	/is the spot/kc
.DE
This marks the first line to be moved with \fBa\fR, the line after the last
to be moved with \fBb\fR, and the paragraph's destination with \fBc\fR.
But
you can see that the move command moves lines to the line \fIafter\fR the third
number specified, so let's change the third mark:
.DM
	'c-1kc
.DE
Now we can use \fBc\fR in the move command without arithmetic.
Now, print
the paragraph to be moved to be sure that the marks are correct.
.DM
	'a,'bp
.DE
\fBed\fR replies with
.DM
	    This is a paragraph, first line, that
	needs to be moved.
	text
	text
	And this is the last sentence of the paragraph.
	    Next paragraph begins here.
.DE
You can see that we would move one line too many if we used the marks as they
are.
So, change \fBb\fR also.
.DM
	'b-1kb
.DE
Now, do the move:
.DM
	'a,'bm'c
	1,$p
.DE
The file now contains:
.DM
	    Next paragraph begins here.
	text
	text
	text
	    This is a paragraph, first line, that
	needs to be moved.
	text
	text
	And this is the last sentence of the paragraph.
	    This is the spot that we want the paragraph
	to precede.
.DE
Marking sections of text can increase the ease with which you solve your
complex \fBed\fR problems.
.Sh "Searching in Reverse Direction"
.PP
.II "ed^reverse searching"
All scanning, processing, and searching has been
shown going from the beginning of the file toward the end.
Sometimes it
is useful to find some word that occurs \fBbefore\fR the current line.
.PP
You can get \fBed\fR to do string searching in the reverse direction by
specifying the search with question marks
.B ?
rather than slashes
.BR / .
To find the previous occurrence of the word \fBlast\fR, use:
.DM
	?last?
.DE
This form of searching can be useful in finding the beginning and end of
a \fBrepeat/until\fR statement.
For example,
if the current line is in the
middle of a Pascal \fBrepeat/until\fR group, you can print the group with
the command:
.DM
	?repeat?,/until/p
.DE
The reverse search is like the forward search in every way except the
direction of search.
The search begins one line before the current or
specified line, and proceeds toward the beginning of the file.
If the
string is not found by the time that the search reaches the beginning of
the file, the search resumes at the end of the file, and progresses
towards the starting point of the search.
If the string is not found
when the search reaches the original starting point, the question-mark
error message is issued signifying no match.
.PP
Also, the command
.DM
	??
.DE
uses the remembered search argument.
.SH "Expert Editing"
.PP
.II "ed^advanced commands"
.II "ed^commands, advanced"
This section describes the most
advanced \fBed\fR commands.
.Sh "File Processing Commands"
.PP
.II "ed^file, editing commands"
Earlier, we discussed the commands
.DM
	ed
.DE
and:
.DM
	ed filename
.DE
.B ed
also has file-handling commands that go beyond those already discussed.
.PP
Suppose that you entered the command
.DM
	ed file1
.DE
only to discover when you examined the contents of
.B file1
that you really wish to edit file
.BR file2 .
You could correct this error by exiting from
.B ed
and then re-invoking it for
.BR file2 .
However, \fBed\fR command
.B e
lets you close out the current file and begin to edit a new file
without exiting from the editor.
For example, to stop editing
.B file1
and begin to edit
.BR file2 ,
simply issue the command:
.DM
	e file2
.DE
If you had made any changes to
.BR file1 ,
.B ed
will prompt you with a
.BR ? ,
which is its way of asking if you wish to throw away the changes you have
made to this file.
If you immediately repeat the command, \fBed\fR proceeds even if
there are unsaved changes.
The command
.DM
	E new
.DE
commands
\fBed\fR to edit the new file, whether or not there are unsaved changes.
.PP
.BR ed 's
``read'' command
.B r
also reads a new file, but adds it to the file being edited
instead of replacing the current file with it.
This can be handy for copying one file into another one.
For example,
if you have a manuscript prefix stored in the file \fBprefix\fR
to include the prefix at the beginning of the file you are editing,
type:
.DM
	0r prefix
.DE
\fBr\fR inserts the file being read
after the line number specified;
in this case, line 0 means at the beginning of the file.
If used
without a line number, \fBr\fR appends the newly read lines
to the end of the file.
.PP
.BR ed 's
command
.B w
writes the entire file if no line number is
specified; however, you can specify line numbers.
For example
.DM
	1,3w new
.DE
writes the first three lines to file \fBnew\fR.
If the file name is
omitted, the lines are written to the remembered file name.
.PP
The \fBw\fR command is unique in that it never changes the current
line.
This is true regardless of what line numbers are specified in the
range for the command, or how those line numbers were developed.
.PP
The
.B W
command resembles the \fBw\fR command, except that it
appends lines to the end of the file, whereas \fBw\fR creates a new file
and erases any previous contents.
.PP
The
.B f
command prints the remembered file name that was set in
.DM
	ed filename
.DE
or
.DM
	e filename
.DE
or
.DM
	w filename
.DE
commands.
You can also use \fBf\fR to reset the remembered name, by typing:
.DM
	f newname
.DE
This form of the command tells you what the new remembered file name is,
even though you just typed it in.
.PP
Note that the command
.DM
	w filename
.DE
changes the remembered name only if there is currently no remembered name,
as does the \fBr\fR command.
.Sh "Patterns"
.PP
.II "ed^special characters"
.II "ed^characters, special"
Earlier, you were cautioned that certain punctuation characters have
special effect in search and substitute commands.
These characters are:
.DM
	[ ^ $ * . \e &
.DE
They are used to form powerful substitute and locator commands.
An orderly combination of these special characters is called a
.IR pattern ,
sometimes called a \fIregular expression\^\fR.
You can use a pattern
to find or \fImatch\fR a variety of strings with one search argument.
.PP
The simplest patterns use alphabetic characters and numeric digits, which
match themselves.
For example,
.DM
	/ab/
.DE
finds and prints the next line containing the string \fBab\fR.
.PP
The next simplest character to use in a pattern is the period or dot.
It matches any character except the \fBnewline\fR
character that separates lines.
Two periods in succession match any
two consecutive characters, and so on.
For example, if you have a file that contains algebraic
statements of the form
.DM
	a+b
	c+e
	a-b
	a/b
	d*e
.DE
and wanted to find and print any line involving \fBa\fR and \fBb\fR
(in that order), then use the search statement:
.DM
	/a.b/
.DE
The \fB.\fR in this example matches \fB+\fR, \fB\-\fR, and \fB/\fR.
.PP
Then, you ask, how do I find a string that contains a period?
For example, if you want to find all the sentences that ended with ``lost.''
(that is, the word \fBlost\fR followed by a period), you might first try:
.DM
	/lost./p
.DE
This, however, also matches the string ``lost\ '' (the word \fBlost\fR
followed by a space), which is not what you want.
.PP
This is where the special character backslash comes in
handy.
A backslash tells \fBed\fR to treat the next
character as a regular character, even if it usually is a special character.
Thus, to find ``lost.'', you need only type:
.DM
	/lost\e./p
.DE
This will not incorrectly find ``lost\ ''.
If you want to find backslashes in your file, simply say:
.DM
	/\e\e/p
.DE
.Sh "Matching Many With One Character"
.PP
The asterisk
.B *
matches an indefinite number of characters.
For example, to remove extra spaces between words in a document, type
.DM
	g/##*/s//#/p
.DE
(The character \fB#\fR has been substituted here for the space character to
make the example more readable.)
This replaces each series of spaces
by one space.
.PP
Note that there are two spaces before the \fB*\fR in the search string.
This is necessary because the \fB*\fR matches any length of string,
including zero.
Therefore, searching for a space followed by any number
of spaces finds strings that are at least one space long.
.PP
The \fB*\fR matches the longest possible string of the previous character.
This requires careful attention on your part, because the string matched by
\fB*\fR might be longer than your required string, or even zero in length.
Either way could give you unexpected results.
.PP
If you have a line
.DM
	a+b-c
.DE
in your file and want to change it to
.DM
	a+c
.DE
type the command:
.DM
	s/a.*c/a+c/p
.DE
However, if the line read instead
.DM
	a+b-c*d+c
.DE
and you applied the command, the result would be
.DM
	a+c
.DE
since the \fB.*\fR matches the longest string between any
\fBa\fR and any \fBc\fR.
.Sh "Beginning and Ending of Lines"
.PP
.II "ed^caret"
The characters 
.B ^
and
.B $
match, respectively, the beginning and ending of a line.
Thus, you can find and print all lines that end with a bang:
.DM
	g/bang$/p
.DE
or those that begin with a whimper:
.DM
	g/^whimper/p
.DE
These two characters can also help you find lines of specific length.
If you need to see all lines exactly five characters long, the command
.DM
	g/^.....$/p
.DE
does the trick.
To find and delete all blank lines, type:
.DM
	g/^ *$/d
.DE
Note that this time the
.B *
matches a string of zero spaces.
However, this is correct, because
a blank line includes lines that have nothing in
them, as well as lines that contain only spaces.
.Sh "Replacing Matched Part"
.PP
.II "ed^&"
In many cases of substituting, you find yourself extending a word, or
adding information to the end of a phrase.
This can lead to extensive retyping of characters.
The special \fB&\fR character can help out.
.PP
This character is special only when used in the right part, or \fIpattern2\fR
of the substitute command.
It means ``the string that matched the left part''.
For example, to add \fBing\fR to the word \fBhelp\fR in the
current line, use:
.DM
	s/help/&ing/
.DE
The ampersand may appear more than once in the right side.
.PP
This can be more interesting if the left part has a non-trivial
.IR pattern .
For every word in a line that is preceded by two or more spaces,
double the number of spaces before it:
.DM
	s/###*/&&/gp
.DE
(Again, spaces have been replaced with \fB#\fR for clarity.)
.Sh "Replacing Parts of Matched String"
.PP
A more sophisticated feature, which is similar to the ampersand,
helps you to rearrange parts of a line.
To see how this works, create a file by typing:
.DM
	ed
	a
	first part=second part
	.
	w eql
	q
.DE
Two special bracket symbols, \fB\e(\fR and \fB\e)\fR can be used
to delineate patterns in the left part of a substitution expression.
Then, you can use the special symbols
\fB\e1\fR, \fB\e2\fR, etc., to insert the delimited parts.
The symbol
\fB\e(\fR marks the beginnning of the pattern, and \fB\e)\fR marks the end.
For example,
to delete everything in the line except the characters to the left of the
\fB=\fR, type
.DM
	ed eql
	s/^\e(.*\e)=.*/\e1/p
	Q
.DE
In the substitute command, the \^^\^ matches the beginning of the line,
\fB.*\fR matches ``first part'', and \fB=.*\fR matches the rest of the line.
The symbol \fB\e1\fR signifies the matched characters between the
first \fB\e(\fR (the only one in this example) and \fB\e)\fR.
The
.B p
then prints the result, which will be:
.DM
	first part
.DE
To interchange the two parts, type
.DM
	ed eql
	s/\e(.*\e)=\e(.*\e)/\e2=\e1/
	p
	wq
.DE
The result is
.DM
	second part=first part
.DE
The first portion of the substitution expression,
.DM
	\e(.*\e)=\e(.*\e)
.DE
can be thought of as being in three parts.
The first part
.DM
	\e(.*\e)
.DE
matches all characters up to but not including the \fB=\fR, which are
.DM
	first part
.DE
The second part
.DM
	=
.DE
matches the \fB=\fR in the line, and finally the third part
.DM
	\e(.*\e)
.DE
matches all characters following the ``='', or
.DM
	second part
.DE
The remainder of the substitution expression
.DM
	\e2=\e1
.DE
which is the replacement part, rebuilds the line in interchanged order.
The symbol \fB\e2\fR replaces the matched string enclosed in the second pair
of \fB\e(\fR \fB\e)\fR delimiters, and the symbol \fB\e1\fR inserts the
matched string enclosed in the first pair of \fB\e(\fR \fB\e)\fR.
.PP
The right side of the substitution inserts the second matched expression
(from \fB\e2\^\fR), then inserts the \fB=\fR sign again, followed finally with
the first part of the line from \fB\e1\fR.
.PP
This may appear involved, but can be immensely valuable in situations
that require rearrangement of a large number of lines.
.PP
The next special characters for patterns that we will consider are the
bracket characters
.B [
and
.BR ] .
These are used to define the character class.
Inside the brackets, put a group of characters;
\fBed\fR will match any of them if it appears.
For example, to print a line that contains any odd digit, say:
.DM
	g/[13579]/p
.DE
For even more power and flexibility, you can combine character classes
with the asterisk.
For example, the following command
finds and prints all lines that contain a negative number
followed by a period:
.DM
	g/-[0123456789]*\e./p
.DE
This matches lines containing the following example strings:
.DM
	-1.
	-666.
	-3.7.77
.DE
You can also match all lower-case letters by listing them in brackets,
but the following abbreviation simplifies this:
.DM
	g/[a-z]/p
.DE
This can also be used for the negative number example above:
.DM
	g/-[0-9]*\e./p
.DE
Most special characters lose their original meaning within the
brackets, but one of the special characters,
.B caret
\|^\|, gets a new meaning.
In this context, it matches all characters \fIexcept\fR
those listed in the brackets.
For example, the following pattern
matches a string that begins with \fBK\fR and continues
with any character except a number:
.DM
	/K[^0-9]/
.DE
This matches:
.DM
	KQ
	KK
	KK9
.DE
but not:
.DM
	K7
	kK0
.DE
.PP
Other special characters may be part of a character class, but lose
their special meaning when used in that context.
Remember,
however, that if you want to match the right bracket, it
must appear first in the list.
So, to find all occurrences of special characters in the file, type:
.DM
	g/[]^\e.*[&]/p
.DE
.Sh "Listing Funny Lines"
.PP
The \fBp\fR command prints lines with graphic characters in them.
It also prints lines with non-graphic (or \fIcontrol\fR\^) characters,
but these do not appear on the screen.
For example, printing a line that contains
the BEL character \fB<ctrl-G>\fR will
ring your terminal's bell, but you will not see where
the BEL character occurs within the line.
.PP
The
.B l
command behaves like the \fBp\fR command, except that it also
decodes and prints control characters.
For example, if you use the \fBl\fR command to print
a line that containing the word \fBbell\fR followed by a
BEL character, you would see:
.DM
	bell\e007\en
.DE
Note that ``007'' is the ASCII value for \fB<ctrl-G>\fR.
(ASCII is the system of encoding characters within your computer;
see \fBASCII\fR in the Lexicon for the full ASCII table.)
The \fBl\fR command displays the
backspace character \fB<ctrl-H>\fR as a hyphen
first overstruck with a \fB<\fR and then a newline, which appears as
\fB\en\fR on your screen.
It displays a tab character as a \fB-\fR, first overstruck with a \fB>\fR
and followed by a newline character, which appears as \fB>\en\fR.
If the line being listed with \fBl\fR is too long
to be displayed on one line on your screen,
\fBl\fR separates it into two lines, with the backslash character
placed at the end of the first line to indicate the split.
.PP
All other features of the
.B p
command apply to the
.B l
command.
.Sh "Keeping Track of Current Line"
.PP
.II "ed^current, line"
The most commonly used abbreviation in \fBed\fR is the dot,
or period, which stands for the current line.
Many commands can
change the value of the dot, and it is useful to you to be able
to anticipate this change when using the abbreviation.
.PP
Different classes of commands affect the value of the dot in different
ways; in general, however, the simple explanation is usually correct:
the current line is the last line processed by the last command to be
executed.
.PP
Consider, for example, how the substitution command \fBs\fR
changes the current line:
.DM
	1,$s/flow/change/
	p
.DE
In this example,
the current line will be the last line modified by the substitutions;
and that will be the line that the \fBp\fR command prints.
.PP
The
.B w
command is an exception to this rule.
It does not change the current line,
regardless of any line range selection or how these ranges are developed.
.PP
The
.B r
command changes the current line to the last of the
lines read.
.PP
The
.B d
command sets the current line to the line after the last
line deleted unless the last line in the file was deleted, in which case the
new last line becomes the current line.
.PP
The line insertion commands
.BR i ,
.BR c ,
and
.B a
all leave the current line as the last line added.
If no lines are added, however, their behaviors differ:
\fBi\fR and \fBc\fR effectively back up the last line by one,
whereas \fBa\fR leaves it the same.
.Sh "When Current Line Is Changed"
.PP
When the current line changes is also important.
Normally, the current line does not change until the command is completed.
.PP
To illustrate, create a file \fBsemi\fR by typing:
.DM
	ed
	a
	begin
	second
	first
	in between
	second
	last
	.
	w semi
	q
.DE
Now, edit the file and type all lines from \fBfirst\fR to \fBsecond\fR:
.DM
	ed semi
	/first/,/second/p
	Q
.DE
This will cause an error!
The reason is that the search command begins
with current line set to \fB$\fR, so ``first'' is found on line 3.
But the
search for ``second'' also begins with the current line set at \fB$\fR, and
finds ``second'' on line 2.
Thus, the command translates to
.DM
	3,2p
.DE
which is clearly invalid.
.PP
.II "ed^;"
To do what was intended, use the
.B semicolon
\fB;\fR instead of the comma to separate the two searches.
This forces \fBed\fR to change
the current line to be changed after the search for \fBfirst\fR
rather than after the entire command.
Thus, the commands
.DM
	ed semi
	/first/;/second/p
	Q
.DE
are correct and will do what is intended.
The result will be:
.DM
	first
	in between
	second
.DE
The search for \fBfirst\fR still begins with the current line set at
.BR $ .
However, after it finds \fBfirst\fR,
.B ed
resets the current line to 3, and
begins the search for \fBsecond\fR there, and succeeds on line 5.
.PP
Finally, to be sure of where the current line is, you can use the
.B p
command to show you the line;
or you can have \fBed\fR tell you the number of the current line by typing:
.DM
	.=
.DE
To give you a perspective on where you are with respect to the end of the
file, type
.DM
	&=
.DE
and \fBed\fR will tell you the number of the last line in the file.
.PP
You can put any line number expression before \fB=\fR and it will type the
result.
For example
.DM
	/next/=
.DE
types the number of the next line to contain ``next'' (if there is one).
The command
.B =
never changes the line number.
.Sh "More About Global Commands"
.PP
.II "ed^global, command"
.II "ed^commands, global"
All the global commands discussed thus far have been
followed by only one command \(em substitute, print, and delete.
You can, however, put several commands after a global command,
and execute each of those commands for each line that matches.
.PP
To change all occurrences of the word \fBcacophonous\fR to the word
\fBnoisy\fR and print the three lines that follow, issue the command:
.DM
	g/cacophonous/s//noisy/\e
	.+1,.+3p
.DE
Here, the additional commands are separated by the backslash before the
\fB<return>\fR.
Several commands can be added, and all but the last need
the backslash at the end.
.PP
This will work for the line-adding commands, as well.
To insert a
spelling warning before each line that contains the word \fBoccurrance\fR,
issue the command:
.DM
	g/occurrance/i\e
	((the following line needs spelling check))\e
	.
.DE
Note that the last line of the \fBi\fR group can be entered without a
backslash, in which case the line containing only the period must be
omitted.
This has the same effect as:
.DM
	g/occurrance/i\e
	((the following line needs spelling check))
.DE
You should not depend upon the setting of the current line in any
multiline global command.
There are two reasons for this.
First, if one of the commands is a substitute
and the string is not found in the
matched line, the current line will not be changed.
.PP
Second, the global command operates in two phases.
The first part scans the file for lines that match the string argument.
.B ed
marks these lines
internally in a manner similar to the \fBk\fR command.
The second phase then executes the commands on each of the marked lines.
Therefore, you cannot count on a string search following the \fBg\fR to set
the current line number.
.PP
Again, the
.B v
command behaves in the same way, except that it selects lines that do
\fInot\fR match the pattern.
.PP
Caution is advised when using remembered search arguments, for a
similar reason.
A search argument is remembered only if the search has been executed.
Thus, in a command of the form
.DM
	g/backup/s//reverse/\e
	s/backin /backing/
.DE
the first remembered search may use \fBbackup\fR on some occasion, and
``\fBbackin\fR\'' on others.
The reason for this is that the second phase of the \fBg\fR
command begins with a remembered search argument of \fBbackup.\fR
After the
second line of the multiline command executes, the remembered search
argument is ``\fBbackin\fR\ ''.
This remains throughout the remainder of the second
.B g
phase.
.PP
Thus, it is recommended that you avoid remembered search arguments when
using multiline global commands.
.Sh "Issuing COHERENT Commands Within ed"
.PP
While you are using \fBed\fR, you can issue \*(CO
commands by prefixing them with the
.B !
command.
.PP
This can be useful if, for example,
you need to discover a file name while in
the middle of an edit, and you want
to find it without leaving \fBed\fR.
Thus, to list your directory while in \fBed\fR, type:
.DM
	!lc
.DE
\fBed\fR sends the command to \*(CO and echoes a \fB!\fR character
when the command is finished.
.PP
There is no limitation on the type of command that you may issue with
this feature.
It is even plausible that you want to start another \fBed\fR.
.SH "For More Information"
.PP
The Lexicon article on \fBed\fR summarizes its commands and options.
The \*(CO system also includes three other useful editors:
\fBsed\fR, the stream editor; \*(ME, the screen editor; and
\fBvi\fR, a clone of the standard UNIX screen editor.
\*(ME and \fBsed\fR are introduced with their own tutorials, and
each is summarized in the
Lexicon.
