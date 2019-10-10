.II sed^tutorial
.ds TL "sed Stream Editor"
.NH "Introduction to the sed Stream Editor" 1
.PP
This is a tutorial for the \*(CO editor \fBsed\fR.
It describes in elementary terms
what \fBsed\fR does.
.PP
This guide is meant for two types of reader: the one who wants a tutorial
introduction to \fBsed\fR, and the one
who wants to use specific sections as
references.
.PP
Related tutorials include \fIUsing the COHERENT System\fR,
which presents the basics of using \*(CO and introduces many useful
programs, and the tutorials for the interactive line editor \fBed\fR
and for the screen editor \*(ME.
.PP
In a nutshell, \fBsed\fR edits
files non-interactively; that is, \fBsed\fR applies your set of commands
to every line of the file being edited.
It is not meant to create a text, as you can do with \fBed\fR,
\fBme\fR, or \fBvi\fR.
Rather, it lets you perform large, intricate
transformations on a file of text, using commands that resemble those
used by \fBed\fR or \fBvi\fR's colon-command mode.
.PP
Although \fBsed\fR is not as easy to control as \fBed\fR or
\*(ME, both of which are interactive,
it can edit a large file very quickly.  
You can use \fBsed\fR
to change computer programs,
natural language manuscripts, command files, electronic mail messages,
or any other type of text file.  
.PP
One last point:
.B sed
normally writes its output to the standard output, which by default is
your screen.
To save its output into a file,
use the shell's `>' operator to redirect the standard output into a file.
.SH "Getting to Know sed"
.PP
\fBsed\fR is a text editor.
It reads a text file 
one line at a time, and applies your set of editing commands
to each line as it is read.
Because it does not
ask you for instructions after it executes each command, \fBsed\fR is
a \fInon-interactive\fR text editor.
.PP
.II "sed^pipes"
The advantages of \fBsed\fR are that it can readily apply the same
editing commands to many files; it can edit a large file
quickly; and it can readily be used with \fIpipes\fR.
A pipe takes the product of
one program and feeds it into another program for further processing.  
If you are unsure of how a pipe works, refer to
\fIsh Shell Command Language Tutorial\fR.
.PP
.II "sed^ed"
\fBsed\fR resembles closely \fBed\fR.
\fBsed\fR and \fBed\fR use almost all of the same
commands, and locate lines in much the same way.
However, there are important differences between \fBed\fR and \fBsed\fR.
\fBed\fR is
interactive:
when you give \fBed\fR a command from the keyboard, it
executes that command immediately and then waits for you to enter
the next command.
\fBsed\fR, on the other hand, accepts your editing commands all at once,
either from the keyboard or, more often, from a file you prepare; 
then, as it reads your text file one line at a time, it applies every
command to every line of text.
Therefore, \fIaddressing\fR (that is,
telling the program what commands should be applied to which lines) is
much more important with \fBsed\fR than with \fBed\fR.
.PP
Keep in mind, too, that \fBsed\fR does not
change your original text file; rather, \fBsed\fR copies it, changes it,
and sends the edited file either to the standard output
or to another file that you name in the command line.
.Sh "Getting Started"
.PP
Here are a few exercises to introduce you to \fBsed\fR.
Type them into your \*(CO system to get a feel for how
\fBsed\fR works.
.PP
As explained above, \fBsed\fR applies a set of editing
commands to your text file.
To edit a file with \fBsed\fR, you must prepare
three elements:
(1) the text file that you wish to edit;
(2) a command file (or \fIscript\fR) that
contains the \fBsed\fR commands you want to apply to the text file;
and
(3) a command line that tells the \*(CO system what you want done and with
which files.
.PP
To begin, then, type the following text into your computer using
the \fBcat\fR command.
(Remember that \fB<ctrl-D>\fR is
typed by holding down the \fIctrl\fR key and simultaneously typing
\fID\fR.)
.DM
	cat >exercise1
	No man will be a sailor who has contrivance enough
	to get himself into a gaol; for being in a ship is
	being in a gaol, with the chance of being drowned.
	<ctrl-D>
.DE
Now, type in the following \fBsed\fR script.  This script will
substitute \fIjail\fR for \fIgaol\fR:
.DM
	cat >script1
	s/gaol/jail/g
	<ctrl-D>
.DE
The last step is to prepare the command line.
The command
line consists of the \fBsed\fR command, the options that tell \fBsed\fR
where its instructions will be coming from (either from a file or directly
from the command line), the name of the file to be edited, and where
the edited file should be send.
The general form of the command line is as follows:  
.DM
	sed [-n] [-e \fIcommands\fP] [-f \fIscriptname\fP] \fItextfile \fP[>\fIfile\fP]\fR
.DE
The \fB\-n\fR option will be explained below, in the section on \fIOutput\fR.
The \fB\-e\fR option tells \fBsed\fR that
\fIcommands\fR follow immediately.
The \fB\-f\fR option tells \fBsed\fR that the
commands are contained in the file \fIscriptname\fR.
\fItextfile\fR is the name of the text file to be edited.
The greater-than symbol \*(Ql>\*(Qr followed
by a file name redirects the edited version of the text file
into \fIfile\fR;
if this option is not used, the edited copy
of the text file will be sent to the standard output.
.PP
In this example, a command script has been prepared, so the \fB\-f\fR
option will be used.
Also, the edited text should appear on the terminal
screen, so the \*(Ql>\*(Qr will not be used.
Type the command line
as follows:
.DM
	sed -f script1 exercise1
.DE
The following text will appear on your screen:
.DM
	No man will be a sailor who has contrivance enough
	to get himself into a jail; for being in a ship is
	being in a jail, with the chance of being drowned.
.DE
You can use
\fBsed\fR not only to substitute one word for another,
but to add lines to files, delete lines, and perform more involved
editing.
No matter how complex your \fBsed\fR editing becomes,
though, \fBsed\fR will always use the basic format just described.  
.PP
The next few sections describe \fBsed\fR's basic commands.  
.Sh "Simple Commands"
.PP
.II "sed^>"
Type in the exercises exactly as shown and examine the results.
Use the \fBcat\fR command to enter the command file
as well as the input file.
The edited text  will appear on your terminal.
Usually when you edit, you will want to redirect the edited text to a new
file; however, for the exercises presented here,
let the edited text appear on your terminal so you
can examine the results immediately.
.Sh "Substituting"
.PP
.II "sed^substitution"
The substitution command is used very often when editing.
\fBsed\fR's substitution command \fBs\fR resembles the same command
in \fBed\fR.
Its form is as follows:
.DM
	s/\fIterm1\fP/\fIterm2\fP/
.DE
This tells \fBsed\fR to substitute \fIterm2\fR for \fIterm1\fR.  
To correct a misspelled word, for example, use this command form:
.DM
	s/mispel/misspell/
.DE
As written, this command changes only the first occurrence of \fBmispel\fR in
each line of your text file.
To change \fIevery\fR occurrence of \fImispel\fR in each line,
add \fBg\fR (the \fBg\fRlobal option) at the end of the command:
.DM
	s/mispel/misspell/g
.DE
If you want to change only the \fIthird\fR
occurrence of \fBmispel\fR on every line,
put a \fB3\fR after the \fBs\fR:
.DM
	s3/mispel/misspell/
.DE
When no digit follows the \fBs\fR and no \fBg\fR follows the
command, only the first occurrence of the term in each line (should
there be one) will be changed.
.PP
To practice the substitution, type the following file into
your system (please include the misspellings):
.DM
	cat >exercise2
	From the Devils Dictionary:
	Hemp, n. A plant from whose fiberous bark is made
	an article of neckware which is frequently put on
	after public speaking in the open air and prevents
	the wearer from tking cold.
	<ctrl-D>
.DE
Now, prepare the following \fBsed\fR script to correct the misspellings:
.DM
	cat >script2
	s/Devils/Devil's/
	s/fiberous/fibrous/
	s/tking/taking/
	<ctrl-D>
.DE
Invoke \fBsed\fR with the following command:
.DM
	sed -f script2 exercise2
.DE
The following will appear on your screen:
.DM
	From the Devil's Dictionary:
	Hemp, n. A plant from whose fiberous bark is made
	an article of neckwear which is frequently put on
	after public speaking in the open air and prevents
	the wearer from taking cold.
.DE
To see how the \fBg\fR command and the number option work,
prepare the following text file:
.DM
	cat >exercise3
	sd	sd	sd	sd
	sd	sd	sd	sd
	sd	sd	sd	sd
	<ctrl-D>
.DE
The following \fBsed\fR script changes the \fIthird\fR \fBsd\fR in each
line to \fBsed\fR:
.DM
	cat >script3
	s3/sd/sed/
	<ctrl-D>
.DE
Invoke \fBsed\fR with the following command line:
.DM
	sed -f script3 exercise3
.DE
The following will appear on your screen:
.DM
	sd	sd	sed	sd
	sd	sd	sed	sd
	sd	sd	sed	sd
.DE
To change \fIevery\fR \fBsd\fR to \fBsed\fR, use the \fBg\fR option.  
Prepare the following \fBsed\fR script:
.DM
	cat >script3a
	s/sd/sed/g
	<ctrl-D>
.DE
The following will appear on your screen:
.DM
	sed	sed	sed	sed
	sed	sed	sed	sed
	sed	sed	sed	sed
.DE
The \fBg\fR command will be most useful for editing prose, when
you have no way to tell how many times a given error will appear on a line.
The numeric option will be most useful for
editing tables and lists.
.Sh "Selecting Lines"
.PP
.II "sed^line^selection"
Each of the substitution commands given
above will be applied to every input line.
Unlike \fBed\fR, there is no error message if no line of text
contains \fIterm1\fR.
.PP
In certain instances, however, you may wish to apply a
particular command only to specific lines.
Lines can be specified (or \fIaddressed\fR)
by \fIpreceding\fR the command with the
identifying line number.
The following exercise demonstrates line selection.
First, prepare the following text file:
.DM
	cat >exercise4
	When a man is tired of London,
	he is tired of life; for there
	is in London all that life can afford.
	<ctrl-D>
.DE
To change the word \fBtired\fR to \fBfatigued\fR on line 2 only,
prepare the following
\fBsed\fR script:
.DM
	cat >script4
	2s/tired/fatigued/
	<ctrl-D>
.DE
Begin the editing of your text file by typing the following
command line:
.DM
	sed -f script4 exercise4
.DE
The following will appear on your screen:
.DM
	When a man is tired of London,
	he is fatigued of life; for there
	is in London all that life can afford.
.DE
Remember that to specify a line number, you must place the number \fIbefore\fR
the command;
but to specify the numeric option (that is, position 
within the line), you must place the number \fIafter\fR the command.
.PP
.II "sed^line^range"
You can define a \fIrange\fR of lines to be edited.
One way to do this is to list the first and last line numbers,
separated by commas, of the block of text in question.
For example, the
following script will change \fBis\fR to \fBwas\fR only in the first
two lines of the text file you just prepared:
.DM
	cat >script4a
	1,2s/is/was/
	<ctrl-D>
.DE
Entering the command line
.DM
	sed -f script4a exercise4
.DE
will bring the following text to your screen:
.DM
	When a man was tired of London,
	he was tired of life, for there
	is in London all that life can afford.
.DE
Note that the word \fBis\fR in line 3 was unaffected by the
substitution command, because it lay outside the range
of lines specified by the command.
.PP
.II "sed^pattern"
You can also select lines by \fIpatterns\fR.
Patterns are \fIstrings\fR (any collection of letters and
numbers, such as a word) that can be combined with commands.
A fuller description of \fIpatterns\fR can be found in the tutorial
for \fBed\fR.
Later on, when we show you other commands, you will
see that line selection by pattern
rather than by line number is quite useful.
.PP
.II "sed^$"
You can use
the end-of-file symbol \*(Ql$\*(Qr for line selection.
When you use this symbol, you do not have to know the exact
number of lines in your text file.
For example, if you want to apply a substitution command from
line 10 through the end of your text file,
the command form is:
.DM
	10,$s/\fIterm1\fR/\fIterm2\fR/
.DE
.Sh "p: Print Lines"
.PP
When \fBsed\fR edits a text file, the edited text is by default
sent to the \fIstandard output\fR, which usually is your terminal's screen.
(As noted above, the edited text can be optionally redirected to
another file by using the shell's \*(Ql>\*(Qr operator.)
Normally, \fBsed\fR prints
every line in the text file, whether the line is changed or not.
.PP
The next exercise will demonstrate these defaults.
First,
type in the following text file:
.DM
.ta 0.5i 1.5i 2.5i
	cat >exercise5
	Bill	g7	r115
	Nora	g8	r115
	Steve	g7	r120
	Ella	g8	r120
	Dave	g7	r115
	Robert	g8	r120
	<ctrl-D>
.DE
Next, create a script that contains no commands, by typing:
.DM
	cat >script5
	<ctrl-D>
.DE
Now, execute this empty script:
.DM
	sed -f script5 exercise5
.DE
Note that \fBsed\fR simply copied your text file to the screen,
without changing it in any way.
.PP
This default, however,
can be inconvenient if you want to print only a selected portion of a file.
Fortunately, with a couple of commands you can control \fBsed\fR's printing.
.PP
The command line option \fB\-n\fR changes \fBsed\fR's printing behavior.
When you invoke \fB\-n\fR, the text file
no longer is printed automatically.
.B sed
prints
only the lines specified by the \fBp\fR command.
The \fBp\fR command makes \fBsed\fR print whatever line (or lines)
to which it is applied.
Use \fB\-n\fR on the command line to stop \fBsed\fR from printing every line
automatically;
then use the \fBp\fR command in the script
to target the lines you want to print.
The following exercise will help you grasp this point.
First, type in the following \fBsed\fR script:
.DM
	cat >script5a
	/g7/p
	<ctrl-D>
.DE
Enter the command line:
.DM
	sed -n -f script5a exercise5
.DE
and the following text will appear on your terminal:
.DM
.ta 0.5i 1.5i 2.5i
	Bill	g7	r115
	Steve	g7	r120
	Dave	g7	r115
.DE
.B sed
prints
only the records of the students in grade 7 (\fBg7\fR).
.PP
It is important to note the order, or \fIsyntax\fR, of the \fB\-n\fR
and \fB\-f\fR command line options.
The correct order is to enter \fB\-n\fR,
then \fB\-f\fR.
(\fB\-nf\fR or \fB\-fn\fR are also acceptable.)
If you type \fB\-f\fR and then \fB\-n\fR, however, all you will get is 
an error message.
.PP
.II "sed^p command^with s"
When you use the \fBp\fR option with a
.B sed
command, \fBsed\fR will print every line of text in which that command
makes a substitution.
This can be useful, but if you are not careful it can also
create some problems.
\fBsed\fR normally prints
every line in your text file, whether or not it is changed
by your script, unless you specify the \fB\-n\fR option in your
command line.
Therefore, if you \fIdo not\fR use the \fB\-n\fR
option in your command line and you \fBdo\fR use the \fBp\fR
option with your \fBs\fR commands, every line that \fBsed\fR
edits will be printed more than once.
.PP
The following script illustrates this point:
.DM
	cat >script5b
	s/g7/g8/gp
	s/r115/r120/gp
	<ctrl-D>
.DE
Now, execute it with the following command:
.DM
	sed -f script5b exercise5
.DE
The result will look like this:
.DM
.ta 0.5i 1.5i 2.5i
	Bill	g8	r115
	Bill	g8	r120
	Bill	g8	r120
	Nora	g8	r120
	Nora	g8	r120
	Steve	g8	r120
	Steve	g8	r120
	Ella	g8	r120
	Dave	g8	r115
	Dave	g8	r120
	Dave	g8	r120
	Robert	g8	r120
.DE
.PP
\fBBill\fR and \fBDave\fR were printed three times:
the first time because
the \fBp\fR option was specified after editing the grade number,
the second time because the \fBp\fR option was specified after
editing the room number, and the third time because the \fB\-n\fR
option was \fInot\fR used on the command line.
\fBSteve\fR and \fBNora\fR were printed twice:
the first time because their lines were edited once
each, and the second time because the \fB\-n\fR option was not used on
the command line.
\fBElla\fR and \fBRobert\fR appeared once because their lines
were not edited at all and the \fB\-n\fR option was not specified in
the command line.
.PP 
To get around this problem, use the \fB\-n\fR option and use \fBp\fR only
once, on the last substitution:
.DM
	cat >script5c
	s/g7/g8/g
	s/r115/r120/gp
	<ctrl-D>
.DE
When you enter the following command line
.DM
	sed -n -f script5c exercise5
.DE
the new result will be:
.DM
.ta 0.5i 1.5i 2.5i
	Bill	g8	r120
	Nora	g8	r120
	Dave	g8	r120
.DE
The \fBw\fR command acts like the \fBp\fR command, except that matched lines
are written to the file whose name follows the \fBw\fR.
The following script shows the correct form:
.DM
	cat >script5d
	s/g8/g9/w grade.9
	s/g7/g8/w grade.8
	<ctrl-D>
.DE
When you execute script5d with this command:
.DM
	sed -f script5d exercise5
.DE
the normal product will be seen produced at your terminal, 
and the edited 
lines will be written to files \fBgrade.8\fR and \fBgrade.9\fR.
File \fBgrade.8\fR will contain:
.DM
.ta 0.5i 1.5i 2.5i
	Bill	g8	r115
	Steve	g8	r120
	Dave	g8	r115
.DE
Note the order in which the two \fBs\fR commands were given.
If their
order were reversed, every text line with \fBg7\fR in it would have \fBg7\fR
changed to \fBg8\fR by the first \fBs\fR command, then have this newly
created \fBg8\fR changed to \fBg9\fR by the second \fBs\fR command.
Thus, \fIall\fR the students would be shown to be in \fBg9\fR,
and every text line would be printed into the file \fBgrade.9\fR.
.Sh "Line Location"
.PP
When you edit a file with \fBsed\fR, it is hard to keep track of line numbers.
As noted earlier, you can locate specific lines with \fBsed\fR by
using patterns as \fIline\fR \fIlocators\fR.
To see how this works, type the following text file into your system:
.DM
	cat >exercise6
	From the Book of Proverbs:
	As a door turneth upon his hinges, so the
	slothful man turneth upon his bed.
	A soft answer turneth away wrath:  but grievous
	words stir up anger.
	<ctrl-D>
.DE
Now, prepare the following \fBsed\fR script:
.DM
	cat >script6
	/door/,/bed/s/turneth/turns/
	<ctrl-D>
.DE
Execute it by entering the following command line:
.DM
	sed -f script6 exercise6
.DE
The text will appear on your terminal this way:
.DM
	From the Book of Proverbs:
	As a door turns upon his hinges, so the
	slothful man turns upon his bed.
	A soft answer turneth away wrath:  but grievous
	words stir up anger.
.DE
Note that the word \fIturns\fR was substituted for the word \fIturneth\fR
only in the first proverb, not the second.
The reason is that the
\fBs\fR command in this instance was preceded by the \fIpatterns\fR
\fBdoor\fR and \fBbed\fR.
These told \fBsed\fR to begin making the substitution
on the first line in which the word \fBdoor\fR appears, and to stop
making the substitution with the first line in which the word \fBbed\fR
appears.
In the text file, the fourth line also contained the word
\fBturneth\fR, but because it lay outside the range of line specified by
the line locators, no substitution was made.
.PP
When \fBsed\fR locates
the last line of a block of text that you have defined, it will immediately 
look for the next occurrence of the first line locator.
If it finds that first line locator, it will then resume making the
substitution to your file until it again finds the second line
locator or comes to the end of the file, whichever occurs first.
In this example, when \fBsed\fR found the word \fBbed\fR, it began
to look again for the word \fBdoor\fR; and if it had found the word \fBdoor\fR,
it would have resumed substituting \fBturns\fR for \fBturneth\fR.
.PP
Remember that, as explained earlier, line numbers can also be used
as line locators.
For example, the \fBsed\fR script
.DM
	2,3s/turneth/turns/
.DE
would have produced the same changes as did the script with the pattern line
locators prepared earlier.
.Sh "Add Lines of Text"
.PP
\fBsed\fR can add lines to your text file.
To see how \fBsed\fR
does this, first prepare the following text file:
.DM
	cat >exercise7
	From the Devil's Dictionary:
	Syllogism, n. A logical formula consisting of a major
	and a minor assumption and an inconsequent.
	<ctrl-D>
.DE
Now, type in the following script:
.DM
	cat >script7
	3a\e
	Economy, n. Purchasing the barrel of whiskey you do not \e
	need for the price of the cow you cannot afford.
	<ctrl-D>
.DE
When you implement the script:
.DM
	sed -f script7 exercise7
.DE
you will see this result:
.DM
	From the Devil's Dictionary:
	Syllogism, n. A logical formula consisting of a major
	and a minor assumption and an inconsequent.
	Economy, n. Purchasing the barrel of whiskey you do not 
	need for the price of the cow you cannot afford.
.DE
The append command \fBa\fR added text \fIafter\fR the third line of the file.
You defined where the text went.
Notice the backslash \*(Ql\e\*(Qr
at the end of the line with the \fBa\fR command.
This indicates that the next line is part of the command.
When you append several lines of text, each line but
the last one to be added must end with a \*(Ql\e\*(Qr as in our example.
.PP
Note that no other editing command, such as \fBs\fR, can affect any line
added with \fBa\fR.
These lines go directly to your screen, or to
a file, should you be sending the edited text there, and are invisible to
all other \fBsed\fR commands.
.PP
The insert command \fBi\fR works like the \fBa\fR command,
except that it adds its lines \fIbefore\fR the
addressed line, rather than after.
The following script shows how the
\fBi\fR command works:
.DM
	cat >script7a
	2i\e
	Peace, n. In international affairs, a period of cheating\e
	between two periods of fighting.
	<ctrl-D>
.DE
Invoking it with this command:
.DM
	sed -f script7a exercise7
.DE
produces this:
.DM
	From the Devil's Dictionary:
	Peace, n. In international affairs, a period of cheating
	between two periods of fighting.
	Syllogism, n. A logical formula consisting of a major
	and a minor assumption and an inconsequent.
.DE
As with the \fBa\fR command, no substitutions or other changes
are performed on lines added with \fBi\fR.
.PP
Note, too, that you can \fIbracket\fR a text line by using the \fBa\fR and
\fBi\fR commands at the same time.
Adding a line with either \fBa\fR
or \fBi\fR does not change line numbers of the text file you are
editing (although it does, of course, change the line numbers of
the file \fBsed\fR writes).
.Sh "Delete Lines"
.PP
The \fBd\fR command deletes lines that you do not want in the edited text.
The original file stays unchanged, of course.
.PP
Lines that match the address (be it a line number, range, or pattern)
of a \fBd\fR command do not appear in the output.
Exercise 8 illustrates the \fBd\fR command:
.DM
	cat >exercise8
	The sun was shining on the sea,
	Shining with all his might.
	He did his very best to make
	The billows smooth and bright --
	And this was odd, because it was
	The middle of the night.
	<ctrl-D>
.DE
Now, you have to define the lines to be deleted
by matching them with a unique pattern or a line number.
To delete lines 3 through 6, prepare this script:
.DM
	cat >script8
	/best/,/night/d
	<ctrl-D>
.DE
The command:
.DM
	sed -f script8 exercise8
.DE
generates this result:  
.DM
	The sun was shining on the sea,
	Shining with all his might.
.DE
Note that when
a line is deleted, no other commands are applied to it.
Usually, if a \fBsed\fR script holds a number of commands,
every one of those commands is applied to every line read from your
text file; however,
\fBsed\fR is logical enough to read the next text line immediately,
should a \fBd\fR command delete the current line
before the series of commands has finished.
.Sh "Change Lines"
.PP
.II "sed^change lines"
The \fBc\fR command combines the \fBi\fR and \fBd\fR options.
Text is inserted before the addressed lines, which are then deleted.
To see how this command works, prepare the following text file:
.DM
	cat >exercise9
	Twas brillig, and the slithy toves
	Did gyre and gimble in the wabe;
	All mimsy were the borogoves,
	And the mome raths outgrabe.
	<ctrl-D>
.DE
Now, type in the following script:
.DM
	cat >script9
	1,2c\e
	Twas brilliant, and the shining cove\e
	Did glare and glimmer in the wave;
	<ctrl-D>
.DE
When you execute your script with the following command line:
.DM
	sed -f script9 exercise9
.DE
the result is:
.DM
	Twas brilliant, and the shining cove
	Did glare and glimmer in the wave;
	All mimsy were the borogoves,
	And the mome raths outgrabe.
.DE
Like the \fBi\fR and \fBa\fR commands,
the \fBc\fR command requires all added lines but the last to end
with \*(Ql\e\*(Qr.
.Sh "Include Lines From a File"
.PP
.II "sed^including a file"
When you edit a file, you may wish to include, or \fIread in\fR,
a second file as part of it.
.II "sed^reading in"
This is done with \fBr\fR command.
To see how this works, type the following file into
your computer, and call it \fBinclude\fR:
.DM
	cat >include
		Then there comes the often-used refrain
		Whose repetitious writing dulls the brain.
	<ctrl-D>
.DE
Now, prepare the file to be edited:
.DM
	cat >exercise10
	To write a poem doesn't take much time;
	Just string some words to rhythm and a rhyme.
	What poets do to language is a crime,
	Words and syntax twisted for a rhyme.
	<ctrl-D>
.DE
When you write your script, you must tell \fBsed\fR where to read in
\fBinclude\fR.
The form of the command should be familiar by now:
.DM
	cat >script10
	/rhyme/r include
	<ctrl-D>
.DE
The result is of
.DM
	sed -f script10 exercise10
.DE
is:
.DM
	To write a poem doesn't take much time;
	Just string some words to rhythm and a rhyme.
		Then there comes the often-used refrain
		Whose repetitious writing dulls the brain.
	What poets do to language is a crime,
	Words and syntax twisting for a rhyme.
		Then there comes the often-used refrain
		Whose repetitious writing dulls the brain.
.DE
Note that the \fBr\fR command
inserted \fBinclude\fR \fIafter\fR the addressed line.
You can address lines by number, of course, as well as by pattern.
.Sh "Quit Processing"
.PP
The \fBq\fR command makes \fBsed\fR stop processing the text file.
You will use this command most often to limit the application your \fBsed\fR
script to a portion of your text file.
For example, if you were editing a large file and
you knew that your commands would be irrelevant to the last half of the
file, you could insert an appropriately addressed \fBq\fR
and save some computer time.
You can also use  this command to print portions of a file.
.PP
To see how this is done, prepare the following text file:
.DM
	cat >exercise11
	An hourglass has a very wide top,
	       a very narrow
	           middle
	        and a bottom
	that is also extremely wide.
	<ctrl-D>
.DE
The following script will print the top of the text file.
Note how the script uses \fBmiddle\fP to address the
line where the file is to be split.
.DM
	cat >script11
	/middle/q
	<ctrl-D>
.DE
The command:
.DM
	sed -f script11 exercise11
.DE
produces:
.DM
	An hourglass has a very wide top,
	       a very narrow
	           middle
.DE
To print out only the lines \fIafter\fR the pattern \fBmiddle\fR, simply delete
the first half of the file with the \fBd\fR command, as follows:
.DM
	cat >script11a
	1,/middle/d
	<ctrl-D>
.DE
The result is the output:
.DM
	        and a bottom
	that is also extremely wide.
.DE
.Sh "Next Line"
.PP
.II "sed^next line"
The \fBn\fR command advances to the next line of the text file.
The \fBn\fR command is useful for instances when you have two or more
interrelated lines, and you want to ensure th
a particular set of patterns is matched over the entire
set of lines.
To see how \fBn\fR works, prepare the following text file:
.DM
	cat >exercise12
	Alpha
	One
	Beta
	Two
	Gamma
	Three
	Delta
	Four
	Epsilon
	Five
	<ctrl-D>
.DE
To print a list of letters alone, type the following script:
.DM
	cat >script12
	n
	d
	<ctrl-D>
.DE
and execute it with the following command line:
.DM
	sed -f script12 exercise12
.DE
The result will be the following:
.DM
	Alpha
	Beta
	Gamma
	Delta
	Epsilon
.DE
Remember that \fBn\fR does \fInot\fR stop processing,
go to the next text line, and begin processing all over again.
Rather, it simply reads the next input line and continues
processing from where it left off.
For example, if your
\fBsed\fR file consisted of three commands, the second of which
was the \fBn\fR command, \fBsed\fR would apply the first
command to the first line it read, then jump to the second line
and apply the last commands.
Then, it would read the third line and begin the pattern over again.
To see how this works, prepare the following text file:
.DM
	cat >exercise13
	Alpha
	Alpha
	Alpha
	Alpha
	<ctrl-D>
.DE
Now type in this script:
.DM
	cat >script13
	s/Alpha/Apple/
	/Apple/n
	s/Alpha/Banana/
	<ctrl-D>
.DE
When you execute the script with this command line:
.DM
	sed -f script13 script13
.DE
the following will appear on your terminal:
.DM
	Apple
	Banana
	Apple
	Banana
.DE
Note that the first substitution command changed the first \fBAlpha\fR
to \fBApple\fR; the \fBn\fR command moved \fBsed\fR to the next
line; and the second \fBs\fR command changed that \fBAlpha\fR
to \fBBanana\fR.
.SH "Advanced sed Commands"
.PP
The following sections discuss
.BR sed 's
advanced features.
They also discuss the method of operation.
.Sh "Work Area"
.PP
As described earlier, \fBsed\fR reads your text file one line at
a time, and applies all of your editing commands to that line.
After
the editing commands have been applied, the edited line is either sent to
the \fIstandard output\fR, written to a file you have named, or thrown
away, depending on what you have told \fBsed\fR to do.
.PP
When \fBsed\fR reads a line from your text file, it copies that line
into a \fIwork area\fR, where it actually executes your editing commands.
\fBsed\fR notes the number of the line in the work area,
then executes each editing command in turn, first
checking to see if the patterns or line numbers specified
in each command actually apply to that line.
After each command is checked in turn and performed if
indicated, \fBsed\fR prints the edited line (if it is supposed to be), and
reads the next text line.
.Sh "Add to Work Area"
.PP
The exercises so far have used only one line in the work area.
The \fBN\fR command, however, tells \fBsed\fR to read a second line into
the work area.
The following exercise illustrates the use of the work area and the \fBN\fR
command.
.DM
	cat >exercise14
	This exercise has a brok
	en word.
	<ctrl-D>
.DE
Now, prepare the following \fBsed\fR script:
.DM
	cat >script14
	/brok$/N
	s/brok\enen/broken/
	s/has/had/
	<ctrl-D>
.DE
and execute it with the following command line:
.DM
	sed -f script14 exercise14
.DE
which produces the following text:
.DM
	This exercise had a broken word.
.DE
You will find it helpful to review this exercise in some detail.
The first command in the script
.DM
	/brok$/N
.DE
tells \fBsed\fR to search for the pattern \fBbrok\fR at the \fIend\fR
of the line of text.
(The dollar sign \*(Ql$\*(Qr in this instance
indicates the end of the line; remember that when the \*(Ql$\*(Qr
is used with a line number, it indicates the end of the \fIfile\fR.)
The \fBN\fR command tells \fBsed\fR to keep this line in the
working space, and copy the \fInext\fR line into the working space as well.
.PP
When \fBsed\fR executes this command on the present text file,
the work area will look like this:
.DM
	This example has a brok<newline>en word.
.DE
Note that the two lines now appear to \fBsed\fR as though they formed
one long line.
The word \fB<newline>\fR represents the end of line character
that tells your terminal or printer to jump to a new line when the text
file is printed out.
This character is invisible, but it is there, and it can be changed or
deleted.
You can describe this character to \fBsed\fR by using
the characters \fB\en\fR.
The first substitution in this script
.DM
	s/brok\enen/broken/
.DE
replaces \fBbrok<newline>en\fR with \fBbroken\fR.
Because the newline character is deleted from the text, what used to be
printed out as two lines on your screen will now be printed out as one.
.PP
Note the difference, too, between the \fBn\fR and \fBN\fR commands.  
The \fBn\fR command will \fIreplace\fR the text line in the work area with the
next line from your text file.
The \fBN\fR command, however,
\fIappends\fR the next line from your text file to the end of the text
already in the working area.
The next exercise demonstrates this difference.
First, create the following text file:
.DM
	cat >exercise15
	Apple
	Apple
	Apple
	Apple
	<ctrl-D>
.DE
Now, prepare the following two scripts:
.DM
	cat >script15
	/Apple/n
	s/Apple/Banana/g
	<ctrl-D>
.sp \n(pDu
	cat >script15a
	/Apple/N
	s/Apple/Banana/g
	<ctrl-D>
.DE
When script15 is executed with the following command line:
.DM
	sed -f script15 exercise15
.DE
this will appear on your screen:
.DM
	Apple
	Banana
	Apple
	Banana
.DE
The \fBn\fR command told \fBsed\fR to print out the line
already in the work area before reading in the next line from the text file.
This meant that \fBsed\fR substituted \fBBanana\fR for \fBApple\fR only on the
\fIsecond\fR line of each pair.
.PP
Note what happens, however, when you run script15a, using this command line:
.DM
	sed -f script15a exercise15
.DE
This text appears:
.DM
	Banana
	Banana
	Banana
	Banana
.DE
Because \fIboth\fR lines of each pair were kept in the
work area, the substitution command changed both of them.
.Sh "Print First Line"
.PP
The \fBP\fR command prints material from the work area.
Unlike the \fBp\fR command,
which prints \fIeverything\fR in the work area, \fBP\fR prints only
the \fIfirst\fR line in the work area.
To see how this works, prepare the following text file:
.DM
	cat >exercise16
	Student:  George
	Teacher:  Mr. Starzynski
	Student:  Marian
	Teacher:  Miss Peterson
	Student:  Ivan
	Teacher:  Mr. Starzynski
	<ctrl-D>
.DE
Now, prepare the following scripts:
.DM
	cat >script16
	/Student/N
	/Mr. Starzynski/p
	<ctrl-D>
.sp \n(pDu
	cat >script16a
	/Student/N
	/Mr. Starzynski/P
	<ctrl-D>
.DE
When the first of these scripts is executed with the following
command line (note the use of the \fB-n\fR option):
.DM
	sed -n -f script16 exercise16
.DE
the result is
.DM
	Student:  George
	Teacher:  Mr. Starzynski
	Student:  Ivan
	Teacher:  Mr. Starzynski
.DE
whereas script16a, when executed as follows:
.DM
	sed -n -f script16a exercise16
.DE
produces
.DM
	Student:  George
	Student:  Ivan
.DE
In
.BR script16 , 
the \fBN\fR command lines pull both the name of the student
and the name of the teacher into \fBsed\fR's work area; the
\fBp\fR command prints the student and teacher in each case where
the teacher is Mr. Starzynski.
In
.BR script16a ,
however, the \fBN\fR pulled both student and teacher into the work area,
the
.B P
command printed only the \fIfirst\fR line of the work area \(em that is,
the name of the student.
.PP
As you can see, \fBP\fR is a powerful tool
that will allow you to select material from
tables, lists, and other repetitive files.
.Sh "Save Work Area"
.PP
\fBsed\fR can create a second work area in addition to the primary
work area in which \fBsed\fR performs its editing.
\fBsed\fR does
not execute any editing commands on the material stored in this
secondary work area; rather, this work area can be used to store
material that you want to edit or insert later.
.PP
The commands \fBh\fR and \fBH\fR copy material from the primary
work area into the secondary work area.
\fBh\fR and \fBH\fR differ
in that \fBh\fR \fIdisplaces\fR any material in the secondary work area
with the line being copied there, whereas \fBH\fR \fIappends\fR the line
being copied onto the material already in the secondary work area.
Note, too,
that both \fBh\fR and \fBH\fR merely \fIcopy\fR the primary work area
into the secondary work area \(em after these commands have been executed,
the material in the primary work area remains intact, and can be
edited further, printed out, or deleted, whichever you prefer.
.PP
The commands \fBg\fR and \fBG\fR copy material back from the secondary
work area into the primary work area.
Again, these commands differ
in that \fBg\fR \fIdisplaces\fR whatever is in the primary work
area with the material from the secondary work area, whereas \fBG\fR
\fIappends\fR the material from the secondary work area onto the
material already in the primary work area.
.PP
The following exercises will demonstrate how these commands are used.
First, create the following text file:
.DM
	cat >exercise17
	fruit:  apple
	berry:  gooseberry
	fruit:  orange
	berry:  raspberry
	fruit:  pear
	berry:  blueberry
	<ctrl-D>
.DE
The first script uses the \fBh\fR and \fBg\fR commands:
.DM
	cat >script17
	/fruit/h
	/fruit/d
	/berry/g
	<ctrl-D>
.DE
When you execute this script with the following command line:
.DM
	sed -f script17 exercise17
.DE
you receive the following text on your screen:
.DM
	fruit:  apple
	fruit:  orange
	fruit:  pear
.DE
Review the last script in detail.
The first command, \fB/fruit/h\fR,
copied the line beginning with \*(QLfruit\*(QR into the secondary work
area, displacing whatever happened to be there.
The command \fB/fruit/d\fR then deleted the line from the
primary work area; if this were not done, it would then have been printed out.
The third command,
\fB/berry/g\fR then recopied the material from the secondary work area into
the primary work area, displacing all lines in the primary work area
that begin with ``berry''.
The result of all this shuffling and displacing was that
the three lines that begin with \fBfruit\fR were printed out.
.PP
The next script demonstrates the \fBH\fR command:
.DM
	cat >script17a
	/fruit/H
	/fruit/d
	/berry/g
	<ctrl-D>
.DE
When you execute this script with the following command line:
.DM
	sed -f script17a exercise 17
.DE
you see:
.DM
	fruit:  apple
	fruit:  apple
	fruit:  orange
	fruit:  apple
	fruit:  orange
	fruit:  pear
.DE
Because the \fBH\fR command \fIappends\fR material into the secondary
work area, rather than replacing it as \fBh\fR does, all three lines
that began with \fBfruit\fR were cumulatively
stored in the secondary work area.
Because the \fBg\fR command was used for every line that began with
\fBberry\fR, the contents of the secondary
work area (that is, the \fBfruit\fR lines)
were written over each of the three lines that began with \fBberry\fR.
.PP
The next script demonstrates the use of the \fBG\fR command:
.DM
	cat >script17b
	/fruit/H
	/fruit/d
	/berry/G
	s/berry://g
	s/fruit://g
	<ctrl-D>
.DE
When you execute this script with the following command line:
.DM
	sed -f script17b exercise17
.DE
you will see:
.DM
	gooseberry
	apple
	raspberry
	apple
	orange
	blueberry
	apple
	orange
	pear
.DE
The \fBH\fR command copies the lines that begin with
\fBfruit\fR into the secondary work area.
The \fBG\fR command then re-copies them from the secondary
work area into the primary work area, and appends them
to the material already in the primary work area \(em that is, to a
line that begins with \fBberry\fR.
.PP
The two substitution commands
then strip off the \fBfruit\fR and \fBberry\fR prefixes; obviously,
these substitutions do not affect the operation of the \fBH\fR and \fBG\fR
commands, but they do create a tidier result.
.PP
By the way, be sure you
distinguish the \fBg\fR command from the \fBg\fR option used with the \fBs\fR
command.
If you do not, what \fBsed\fR finally prints out for
you may appear very strange.
.PP
The final command that uses the secondary work area is \fBx\fR,
which exchanges the two work areas.
The following script shows how this is used:
.DM
	cat >script17c
	/fruit/H
	/fruit/d
	/blueberry/x
	s/berry://g
	s/fruit://g
	<ctrl-D>
.DE
When you execute this script with the following command line:
.DM
	sed -f script17c exercise17
.DE
you see:
.DM
	gooseberry
	raspberry
	apple
	orange
	pear
.DE
The text lines that began with \fBfruit\fR were moved into the
secondary working area.
The \fBx\fR command was executed
when the line that contained the word \fBblueberry\fR was reached, and
the two working areas exchanged their contents.
The \fBfruit\fR lines
were then printed out, while the \fBblueberry\fR line was simply left
in the secondary working at the end of the program, and disappeared
when the program concluded.
.PP
Note that \fBx\fR simply swaps the two working areas \(em there
is no \*(QL\fBX\fR\*(QR command that appends the work areas onto each
other.
.Sh "Transform Characters"
.PP
The \fBy\fR command is a special form of the \fBs\fR command.
With the \fBy\fR command, you can replace a number of
characters easily, without having to write a series of \fBs\fR
commands.
.PP
The form of the command is:
.DM
	y/123/abc/
.DE
In the above example, \fB1\fR will be replaced with \fBa\fR, \fB2\fR
with \fBb\fR,
and \fB3\fR with \fBc\fR throughout the document (no \fBg\fR option is
needed).
For \fBy\fR to work properly there must be a one-to-one
relationship between the characters being replaced and the characters
replacing them.
Also, \fBy\fR cannot make exchanges that involve
more than one character \(em it cannot, for example, replace \fBapple\fR with
\fBbanana\fR.
.PP
One useful task for the \fBy\fR command is to change all upper-case
letters in a file to lower case.
Prepare the following text file to see how this is done:
.DM
	cat >exercise18
	NOW IS THE TIME FOR ALL GOOD MEN TO COME
	TO THE AID OF THE PARTY.  
	<ctrl-D>
.DE
And prepare the following script, which will change
these capitals:
.DM
	cat >script18
	y/ABCDEFGHI/abcdefghi/
	y/JKLMNOPQR/jlkmnopqr/
	y/STUVWXYZ/stuvwxyz/
	<ctrl-D>
.DE
The alphabet is entered here in three chunks, to prevent the
command from being too long to type easily.
Execute this script with the following command line:
.DM
	sed -f script18 exercise18
.DE
The result is:
.DM
	now is the time for all good men to come
	to the aid of the party.
.DE
.Sh "Command Control"
.PP
\fBsed\fR gives you advanced control over the execution of commands.
The next subsections describe how these command controls help you write compact, powerful
scripts.
.Sh "{ }: Command Grouping"
.PP
In several of the exercises presented so far, more than one command
specified the same line locator.
By using braces \*(Ql{\*(Qr and \*(Ql}\*(Qr, you can
bundle commands, which makes writing your scripts easier and
lessens the chance of making a typographical error.
.PP
To see how this is done, type the following exercise:
.DM
	cat >exercise19
	When my love swears that she is made of truth,
	I do believe her, though I know she lies,
	That she might think me some untutored youth,
	Unlearned in the world's false subtleties.
	<ctrl-D>
.DE
Now, prepare the following script:
.DM
	cat >script19
	/truth/{N
	P
	}
	/lies/d
	<ctrl-D>
.DE
When you execute this script with the following command line:
.DM
	sed -f script19 exercise19
.DE
the result on your terminal is:
.DM
	When my love swears that she is made of truth,
	That she might think me some untutored youth,
	Unlearned in the world's false subtleties.
.DE
Note the syntax of this command.
Each subsequent command must go on a line of its own,
as must the right brace \*(Ql}\*(Qr.
If this syntax is not observed, you will receive an error message.
.Sh "!: All But"
.PP
The \fB!\fR flag inverts a line selector; that is to say, the
command will be performed on every line \fIbut\fR the one named
in the line selector.
The following script will show how this works:
.DM
	cat >script19a
	2!d
	<ctrl-D>
.DE
which, when run with the following command line:
.DM
	sed -f script19a exercise19
.DE
produces
.DM
	I do believe her, though I know she lies,
.DE
This script deleted every line \fIexcept\fR line 2.
The \fB!\fR flag may also be used with a range of lines, as indicated by
line numbers or line patterns; in either case, you must place the \fB!\fR flag
\fIafter\fR the line selectors and immediately \fIbefore\fR
the command.
Obviously, the \fB!\fR flag is very powerful,
and can be used to sift out a few desired lines from
a large file.
.Sh "= : Print Line Number"
.PP
You may wish to print only the \fIline number\fR of lines that
contain a selected pattern.
This is done with the \fB=\fR command.
For example, you may wish to know the number of each line in the
exercise that contains the word \fBshe\fR.
The following script:
.DM
	cat >script19b
	/she/=
	<ctrl-D>
.DE
when executed with the following command line (note the \fB-n\fR option):
.DM
	sed -n -f script19b exercise19
.DE
produces this result:
.DM
	1
	2
	3
.DE
These numbers can be stored in a file and used in further editing, or
included with the text of the fully edited file to provide a series
of line markers.
.Sh "Skipping Commands"
.PP
\fBsed\fR normally processes editing commands in order, beginning with the 
first command and proceeding sequentially to the last.
This behavior can be
modified by the branching commands: \fBb\fR, \fBt\fR, and \fB:\fR.
.PP
These commands must be used with the colon (\fB:\fR) command, which
defines a \fIlabel\fR point in the list of commands.
.PP
The \fBbranch\fR command \fBb\fR allows you to skip unconditionally
some editing commands in your script.
The following exercise demonstrates how this can be used:
.DM
	cat >exercise20
	They went to sea in a sieve, they did;
	In a sieve they went to sea;
	In spite of all their friends could say,
	On a winter's morn, on a stormy day,
	In a sieve they went to sea.
	<ctrl-D>
.DE
The following script uses the \fBb\fR command to avoid making certain
changes to the first line of the poem:
.DM
	cat >script20
	s/sea/drink/g
	/They/bend
	s/sieve/ship/g
	:end
.DE
When you execute this script with the following command line:
.DM
	sed -f script20 exercise20
.DE
you will see:
.DM
	They went to drink in a sieve, they did;
	In a ship they went to drink;
	In spite of all their friends could say,
	On a winter's morn, on a stormy day,
	In a ship they went to drink.
.DE
Note that the word \fBsea\fR is changed to \fBdrink\fR throughout the file;
however, when \fBsed\fR noted that the word \fBThey\fR appeared in line 1,
the \fBb\fR command forced it to seek the \fB:\fR command that was
labeled with the word \fBend\fR, and to continue editing only \fIafter\fR
it found the labelled \fB:\fR command.
In so doing, \fBsed\fR skipped
the command to substitute \fBship\fR for \fBsieve\fR,
which is why that substitution was not made in line 1.
.PP
Note the syntax of the \fBb\fR command:
the label follows it without a break.
The text of the label is unimportant,
just so long as it matches that used in the \fBb\fR command; however,
the use of a label allows you to place several \fBb\fR or (as will be
seen) \fBt\fR commands in the same script without mixing them up.
.Sh "t: Test Command"
.PP
The \fBtest\fR command, \fBt\fR, also allows you to change the order in
which editing commands are executed.
Unlike the \fBb\fR command,
which simply examines a line for a given pattern,
the \fBt\fR command \fItests\fR to see if a particular
substitution has been performed.
.PP
The following script demonstrates the use of the \fBt\fR command:
.DM
	cat >script20a
	s/They/they/g
	tend
	s/sieve/ship/
	:end
	s/sea/drink/g
	<ctrl-D>
.DE
which, when executed with the following command line:
.DM
	sed -f script20a exercise20
.DE
produces:
.DM
	they went to drink in a sieve, they did;
	In a ship they went to drink;
	In spite of all their friends could say,
	On a winter's morn, on a stormy day,
	In a ship they went to drink.
.DE
Note that the \fBt\fR command checked to see that \fBthey\fR was
substituted for \fBThey\fR before branching to the \*(Ql:\*(Qr command
labeled with the word \fBend\fR.
.PP
Also note the syntax of the \fBt\fR command:
Like the \fBb\fR command,
the label immediately follows the command and is not separated
by a space; unlike the \fBb\fR command, however, the \fBt\fR command
appears on the line \fIbelow\fR the substitution command for which
it is testing.
.SH "For More Information"
.PP
The Lexicon entry for
.B sed
summarizes its command-line options and commands.
The \*(CO line editor \fBed\fR resembles \fBsed\fR,
except that it works interactively instead of in a stream.
For information on \fBed\fR, see its tutorial or its entry
in the Lexicon.
