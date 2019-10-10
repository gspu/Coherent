.II "sh^tutorial"
.ds TL "The Bourne Shell"
.NH "Introducing sh, the Bourne Shell" 1
.PP
.II script
.II shell
.II commands^COHERENT
.B sh
is the command that invokes the Bourne shell, which is the \*(CO
system's default command interpreter.
The Bourne shell interpets commands, and much more.
It is, in effect, both a programming language and an interpreter.
.PP
At least one writer has noted that the shell is
the original ``fourth-generation language'' \(em that is, a powerful
programming language that is straightforward enough to be programmed
by non-programmers.
You will find that taking a little time to master the rudiments of
the shell programming language will pay enormous benefits in making
best use of your \*(CO system.
.SH "Simple Commands"
.PP
.II "shell^simple commands"
The shell command language is built around simple commands.
For example, the following command lists all files in the current directory:
.DM
	lc
.DE
.II semicolons
.II ;
You can combine
several simple commands on one line by separating them with semicolons:
.DM
	who;du;mail
.DE
.II "shell^sequential execution of commands"
The shell executes
the commands in sequence as if they had been typed:
.DM
	who
	du
	mail
.DE
In both of these examples, \fBdu\fR does not begin execution until \fBwho\fR
is finished, and \fBmail\fR does not begin until \fBdu\fR is done.
.SH "Special Characters"
.PP
The shell treats the following characters
specially;
if you want to use them without their special
meaning, you must precede them with the backslash character \fB\e\fR, or
enclose them within quotation marks:
.DM
	*  ?  [  ]  |  ||  ;  {  }  (  )  &  &&
	$  =  :  \(ga  '  "  <  >  <<  >>
.DE
The function of these characters will be explained later in this
section.
To use one of these characters in a command, for example `?', type:
.DM
	echo \e?
.DE
In addition, the shell treats the following words in a special way when they
appear as the first word of a command:
.DM
.ta 0.5i 1.25i 2.0i 2.75i 3.5i 4.25i
	if	then	elif	else	fi
.sp \n(pDu
	case	esac
.sp \n(pDu
	do	done
.sp \n(pDu
	for	in
.sp \n(pDu
	until	while	break
.sp \n(pDu
	test
.DE
.SH "Running Commands in the Background"
.PP
.II "commands^concurrent execution"
.II &
The shell can execute
commands simultaneously as well as sequentially.
This means that while the shell is executing one command,
it lets you type and execute another command.
Under the shell, the number of commands you can execute at the same
time is limited mainly by the amount of memory and disk space on your
system.
.PP
If a command is followed by the
special character `&', the shell begins to execute it
immediately, and prompts you to enter another command.
For example, if you need to \fBsort\fR a large file
but want to continue with other commands while the
sort is executing, you can type:
.DM
	sort >bigfile.sorted bigfile.unsorted &
	ed prog
.DE
This allows you to
edit file \fBprog\fR while your computer quietly executes the sort
in the background.
.PP
.II commands^background
.II process^id
When you run a command with \fB&\fR,
the shell types the \fIprocess\fR \fIid\fR of the command started in
background.
When the \*(CO system runs a command, it assigns that command
a \fIprocess id\fR, which is a number that uniquely identifies that
command to \*(CO.
Normally, there is no need to be concerned about these numbers.
However, when you run commands in the background,
the shell tells you the id of the background process so you
can keep track of its execution.
.PP
.II ps
The command
.DM
	ps
.DE
lists the processes you are currently running.
If you have no background jobs, the response is:
.DM
	TTY PID
	30: 362 -sh
	30: 399 ps
.DE
.II process
.II ps
The first column shows the number that \*(CO has assigned
to your terminal.
This is the same terminal number printed out by \fBwho\fR.
The second column shows the process id;
the third column shows the program or command executing.
The characters \fB-sh\fR in the third column means the login shell.
There are two processes because the shell is running the \fBps\fR
command as a separate process.
.PP
Once you have started a background command, \fBps\fR
shows you the process entry, which has the process id that the
shell typed out for you.
.PP
.II "background process"
.II process^background
.II wait
If you need the results from a background job,
you can wait for it to finish by issuing the command:
.DM
	wait
.DE
The shell will then accept no further commands until all your background jobs
are finished.
If there are no background jobs, there will be no delay.
.SH Scripts
.PP
.II "file^of commands"
.II "commands^in files"
Many of the commands that you use in \*(CO are \fIprograms\fR,
such as \fBed\fR.
Others, like the \fBman\fR command, are \fIscripts\fR, or files that
merely contain calls to other commands.
You can write scripts on your own, simply by using a text editor
to type into a file the commands you wish to execute.
If you frequently use a set of commands,
you can save yourself from having to type them over and over by
simply typing them once into a script.
.PP
.II sh
.II script
.II shell^script
For example, suppose that you wish to check periodically the amount of
disk space that you have used, the amount of disk space still available, and
see who is using the system.
You can write a script to do all of this automatically.
Create the script \fBgood.am\fR by
typing the following commands:
.DM
	ed
	a
	du
	df
	who | sort
	mail
	.
	w good.am
	q
.DE
From now on, to execute the above-listed commands, you need only type:
.DM
	sh good.am
.DE
where \fBsh\fR is a command that means: read commands from a file, in this
case \fBgood.am\fR.
If you can issue a command from your terminal, you can also execute it
from within a script.
.PP
.II chmod
.II sh
You can make a command file directly executable by using the command
.BR chmod .
For example, the command
.DM
	chmod +x good.am
.DE
lets you execute the script \fBgood.am\fR by typing
.DM
	good.am
.DE
and leaving off the \fBsh\fR.
Once you have done the \fBchmod\fR
command, you can still issue the commands by typing:
.DM
	sh good.am
.DE
as well as use \fBed\fR or \*(ME to change the contents of the script.
.PP
.II script
Notice that the commands called by a script may themselves be scripts.
This is illustrated by the following script, \fBsecond.sh\fR:
.DM
	ed
	a
	good.am
	lc
	.
	w second.sh
	q
.DE
Thus, typing:
.DM
	sh second.sh
.DE
calls the script \fBgood.am\fR, and also calls the command \fBlc\fR.
.SH "\&.profile: Login Shell Script"
.PP
.II prompt
.II directory^home
.II "home directory"
When you log into the system and before you are issued your first prompt,
\*(CO checks your home directory for a file named \fB.profile\fR;
if it is present, the shell executes the commands it contains.
.PP
This enables you to have \*(CO execute commands as soon as you log in.
Check if your installation provides one for you by doing an \fBlc\fR
(be sure that your current directory is the home directory).
If the file is there, print it by saying:
.DM
	cat .profile
.DE
Some of the commands may be of the form:
.DM
	PATH=':/bin:/usr/bin'
.DE
This sort of command will be discussed below.
.SH "Substitutions"
.PP
Scripts of the form shown above are processed by the \*(CO shell
without change.
.II "substitution^in commands"
However,
the \*(CO shell increases the power of commands by performing three kinds
of substitutions within commands before it executes them.
.PP
First, it replaces
special characters in commands with file names from
the current or other directories.
This allows you to issue a single command that processes several files.
.PP
Second, you can give a script \fIarguments\fR, much like arguments that are
passed to a Pascal, Algol, or C procedure.
This lets you target the
action of the script to a specific file name specified when you call it.
.PP
Third, the output of one command can be ``piped'' into another command
to serve as its input.
.PP
.II echo
We will use the command \fBecho\fR to illustrate these
kinds of substitution.
Remember that substitutions take place for all
commands in the same way that they do for \fBecho\fR.
.SH "File Name Substitution"
.PP
File names are often used as command parameters.
That is, you will often tell a command to do something to one
or more files.
By using special shell characters, you can substitute file names in commands.
These special characters describe file name \fIpatterns\fR for the shell to
look for in the directory.
When the shell finds the file names, it replaces the pattern with them.
.PP
.II *
.II patterns
The asterisk
\fB*\fR
matches any number of any characters in file names.
Thus,
.DM
	echo *
.DE
echoes all the file names in the current directory, whereas
.DM
	echo f*
.DE
gives all file names that begin with the letter \fBf\fR, and
.DM
	echo a*z
.DE
lists all names that begin with \fBa\fR and end with \fBz\fR.
.PP
.II *
.II echo
To illustrate more clearly, create two files by typing
.DM
	cat >zz1
	<ctrl-D>
	cat >zz2
	<ctrl-D>
.DE
Then the \fBecho\fR command
.DM
	echo zz*
.DE
produces the output:
.DM
	zz1 zz2
.DE
Thus, by using a single \fB*\fR, you can substitute several file names into
a command.
In other words, the command
.DM
	echo zz*
.DE
is equivalent to
.DM
	echo zz1 zz2
.DE
If no file names fit the pattern, the special characters
are not changed, but left in the command exactly as you typed them.
To illustrate, type the command
.DM
	rm zz*
	echo zz*
.DE
The first command will remove all files whose names begin with \fBzz\fR, and is
therefore equivalent to:
.DM
	rm zz1 zz2
.DE
The \fBecho\fR command that follows, however, echoes
.DM
	zz*
.DE
because no files begin with \fBzz\fR; they were just removed.
.PP
.II *
.II apostrophe
Enclosing command words within apostrophes
prevents the shell from matching file names with the
enclosed characters.
In the unlikely event that you have a file whose name is
.DM
	zz*
.DE
that you want to remove, use the command
.DM
	rm 'zz*'
.DE
The \fB*\fR
is enclosed within apostrophes, and therefore is not changed by the shell.
.PP
.II ?
.II "question mark"
.II "file^creating empty"
Another special character \fB?\fR match any one letter.
To see how this works,
create empty files \fBfile1\fR, \fBfile2\fR, and \fBfile33\fR by typing:
.DM
	>file1
	>file2
	>file33
.DE
The command
.DM
	echo file?
.DE
replies
.DM
	file1 file2
.DE
because \fB?\fR does not match \fB33\fR.
.PP
.II ]
.II [
You can use
brackets \fB[\fR and \fB]\fR
to indicate a choice of single characters in a pattern:
.DM
	echo file[12]
.DE
This command replies:
.DM
	file1 file2
.DE
To match a range of characters, separate the beginning and end of the range
with a hyphen.
The command
.DM
	echo [a-m]*
.DE
prints any file name beginning with a lower-case letter from the first
half of the alphabet, and is exactly equivalent to:
.DM
	echo [abcdefghijklm]*
.DE
.II "order^of matched file names"
When such patterns find several file names,
they are inserted in alphabetical order.
.PP
.II patterns
.II /
Because the character \fB/\fR
is important in path names, the shell does not match it
with \fB*\fR or \fB?\fR in
patterns.
The slash must be matched explicitly; that is, it is matched only by a
\fB/\fR itself.
Therefore, to find all the files in the
\fB/usr\fR directories with the name \fBnotes\fR, type:
.DM
	echo /usr/*/notes
.DE
The asterisk matches all the subdirectories of \fB/usr\fR that contain a
file named \fBnotes\fR.
.PP
In addition, a leading period in a file name must be
matched explicitly.
If you have a file in your current directory with the
name \fB.profile\fR, the command
.DM
	echo *file
.DE
does not match it.
.PP
These patterns can appear anywhere within a command or a command file.
.SH "Parameter Substitution"
.PP
.II parameter^positional
Each shell script can have up to nine \fIpositional parameters\fR.
This lets you write scripts that can be used for many circumstances.
Recall that command parameters follow the command
itself and are separated by tabs or spaces.
An example of a command reference with two parameters is:
.DM
	show first second
.DE
where \fBfirst\fR and \fBsecond\fR are the parameters.
.PP
.II script
.II $
To substitute the positional parameters in the script, use the character
\fB$\fR followed by the decimal number of the parameter.
Consider the following example.
First, create two sample files:
.DM
	cat > first
	line 1
	line two
	line 3
	<ctrl-D>
	cat > second
	line 1
	line 2
	line 3
	<ctrl-D>
.DE
.II parameter^positional
Then, issue the commands
.DM
	cat first
	cat second
	diff first second
.DE
.II parameter^fewer
.II parameter^null
Inspect the output carefully.
The command
.B diff
compares two files and prints all lines that differ.
In this case, it prints
.B "line two"
and
.BR "line 2" .
.PP
Now, build the script
.BR show ,
which uses parameter substitution:
.DM
	ed show
	a
	cat $1
	cat $2
	diff $1 $2
	.
	wq
	chmod +x show
.DE
To demonstrate the effet of
.BR show ,
type:
.DM
	show first second
.DE
Inspect the output and compare it with the output you received earlier.
.PP
If you issue the
.B show
command with fewer than the required number of parameters, the shell
substitutes an empty string in its place.
For example, using the command
.DM
	show first
.DE
is equivalent to
.DM
	cat first
	cat
	diff first
.DE
where the null string has been substituted for
.BR $2 .
.PP
The example above shows the parameter references separated from each
other by a space.
In some uses, you may wish to prefix a substituted parameter to a name
or a number.
When more than one digit follows a
.BR $ ,
The shell picks up the first digit as the number of the parameter.
To illustrate, build the shell file \fBpos\fR:
.DM
	ed
	a
	echo $167
	.
	w pos
	q
	chmod +x pos
.DE
Then call the script with
.DM
	pos five
.DE
and the result will be:
.DM
	five67
.DE
.SH "Shell Variable Substitution"
.PP
.II shell^variable
In addition to positional parameters, the shell provides \fIvariables\fR.
You can assign values to variables, test them, and substitute them in
commands.
.PP
.II "case sensitivity^in shell variable"
The variable name can be built from letters, numbers, and the
underscore character; for example:
.DM
	high_tension
	a
	directory
	167
.DE
Note that keywords must not be single digits, because the shell then treats
them as positional parameters.
Be aware that the shell treats upper-case and lower-case letters
differently in variable names.
.PP
.II shell^variable
.II variable^shell
An assignment statement gives a value to a shell variable:
.DM
	a=welcome
.DE
You can inspect their value with the \fBecho\fR command:
.DM
	echo $a
.DE
The shell substitutes
the value of the variable \fBa\fR in the \fBecho\fR command,
which then appears as
.DM
	echo welcome
.DE
\*(CO responds to this command by printing:
.DM
	welcome
.DE
Don't forget the \fB$\fR when referring to the value.
.PP
.II space
Notice that the shell looks for special characters in any command
that it sees \(em this includes the \fIspace\fR character.
To avoid problems, enclose the value to be assigned in apostrophes:
.DM
	phrase='several words long'
.DE
There are several uses for variables.
One is to hold a long string that you
expect to type repeatedly as part of a command.
If you are editing files in a subdirectory like
.DM
	/usr/wisdom/source/widget
.DE
you can abbreviate if you set a variable \fBpw\fR to:
.DM
	pw='/usr/wisdom/source/widget'
.DE
Then simply using \fB$pw\fR in a command
.DM
	echo $pw
.DE
substitutes the long path name.
.PP
.II keyword^parameters
.II parameter^keyword
Another use of shell variables is as keyword parameters to commands.
These then can be used the same way as positional parameters.
To see how this works, create another script resembling \fBshow\fR:
.DM
	ed
	a
	cat $one
	cat $two
	diff $one $two
	.
	w show2
	q
	chmod +x show2
.DE
To use \fBshow2\fR, issue:
.DM
	one=first two=second show2
.DE
This is equivalent in effect to:
.DM
	cat first
	cat second
	diff first second
.DE
.II brace
Unlike positional parameters, keyword parameters may be several
characters in length.
If you want some text to follow immediately a keyword
parameter, enclose the keyword parameter in braces.
To illustrate this,
build a command file called \fBbrace\fR, as follows:
.DM
	ed
	a
	echo 'with brace:' ${a}bc
	echo 'without brace:' $abc
	.
	w brace
	q
	chmod +x brace
.DE
Call the command file with \fBa\fR set:
.DM
	a=567 brace
.DE
The result is:
.DM
	with brace: 567bc
	without brace:
.DE
.II "parameter^assigning keyword"
When used in this way, the keyword
parameters must be assigned before
the command and on the same line as the command.
In this case, the assignment of keyword parameters does not affect
the variable after the command is executed.
For example, if you type:
.DM
	one=ordinal
	one=first two=second show2
	echo 'value of one is ' $one
.DE
\fBecho\fR produces:
.DM
	value of one is ordinal
.DE
.II parameter^positional
Variables set other than on the line of a command are not normally
accessible to a script.
To illustrate, build a parameter display script:
.DM
	ed
	a
	echo 1 $1 2 $2 p1 $p1 p2 $p2
	.
	w pars
	q
	chmod +x pars
.DE
This will be used to show the behavior of parameters.
The parameters to \fBecho\fR without a \fB$\fR help to read the output.
To pass positional parameters, type:
.DM
	pars ay bee
.DE
The output is:
.DM
	1 ay 2 bee p1 p2
.DE
To pass keyword parameters, type:
.DM
	p1=start p2=begin pars
.DE
The result is:
.DM
	1 2 p1 start p2 begin
.DE
To illustrate that the setting of \fBp1\fR and \fBp2\fR did not ``stick'', type:
.DM
	echo $p1 $p2 'to show'
.DE
\fBecho\fR replies:
.DM
	to show
.DE
This indicates that \fBp1\fR and \fBp2\fR are not set.
.PP
Illustrating that variables set separately from a command are not seen by
the command, type:
.DM
	p1=outside1 p2=outside2
	pars
.DE
This replies:
.DM
	1 2 p1 p2
.DE
.II export
.II shell^variable
.II variable^shell
By using the \fBexport\fR command, however, such variables can be made
available to commands.
The commands
.DM
	export p1 p2
	p1='see me' p2=hello
	pars
.DE
produce:
.DM
	1 2 p1 see me p2 hello
.DE
This indicates that after the \fBexport\fR
of \fBp1\fR and \fBp2\fR, they are available to other commands.
Once a variable has appeared in an \fBexport\fR command,
its value can be changed without a need to \fBexport\fR it again.
.SH "Command Substitution"
.PP
.II "grave accent"
By enclosing a command between \fB\(ga\fR characters, you can
feed its output onto the command line of another command.
For example
.DM
	echo \(gals\(ga
.DE
echoes the output of the \fBls\fR command.
.SH "Special Shell Variables"
.PP
.II cd
.II HOME
.II directory^home
.II default^directory
When you log into the \*(CO system, it sets
the shell variable \fBHOME\fR to your \fIhome\fR or default directory path.
If your user name is \fBhenry\fR, then the command
.DM
	echo $HOME
.DE
on most systems prints:
.DM
	/usr/henry
.DE
The change directory command \fBcd\fR sets the working directory
to the path found in \fBHOME\fR if no argument is given.
.PP
.II PS1
.II PS2
.II >
.II $
.II default^prompt
.II prompt
The shell normally prompts you with \fB$\fR for commands, and with
\fB>\fR if more information is needed.
These two prompts are taken by the shell from the variables
\fBPS1\fR and \fBPS2\fR.
You can change these if you want different prompts, for example
.DM
	PS1="Fred's Software Palace: "
	PS2='!'
.DE
To have these take effect each time you log in, put the assignment statements
in your \fB.profile\fR file.
.PP
.II /bin
.II PATH
The shell variable \fBPATH\fR lists the path names of
directories that contain commands.
To show the contents of
.BR PATH ,
type:
.DM
	echo $PATH
.DE
It typically will show:
.DM
	:/bin:/usr/bin
.DE
.II ": (colon)"
.II "parent directory"
.II directory^parent
This means that the shell looks for a command first in the current
directory, then in \fB/bin\fR, and, if not found there, then in \fB/usr/bin\fR.
The path names are separated by `:'.
This means that an empty string precedes the first `:', the current directory.
Another common setting for \fBPATH\fR is:
.DM
	:..:/bin:/usr/bin
.DE
This means that the shell seeks commands
first in the current directory, then in  `..'
(the parent directory of the current directory),
then in \fB/bin\fR, and finally in \fB/usr/bin\fR.
.SH "dot . : Read Commands"
.PP
.II "dot command"
Similar to the command \fBsh\fR is the \fB.\fR command.
The command
.DM
	. cfil
.DE
causes the shell to read and execute commands from \fBcfil\fR.
.PP
This differs from the \fBsh\fR command in several respects.
First, there's no way to pass parameters to \fBcfil\fR with the `.' command.
Second, the \fBsh\fR command executes another shell to read the commands,
whereas `.' simply reads the commands directly.
Finally, all the string
variables and parameters are accessible by \fBcfil\fR.
.PP
The command file \fBgood.am\fR created earlier can be executed with:
.DM
	. good.am
.DE
This has the same effect.
Similarly, the `.' can itself be used within a command file:
.DM
	ed
	a
	. good.am
	lc
	.
	w third.sh
	q
.DE
Then, the command
.DM
	. third.sh
.DE
has the same result as the command:
.DM
	sh third.sh
.DE
.SH "Values Returned by Commands"
.PP
.II failure
.II success
.II commands^value
.II "value from command"
Most \*(CO commands return a value that indicates success or failure.
For example, if
.B grep
cannot find your file, it issues a diagnostic message and
returns a value that tells the shell that something went wrong.
You can examine this value by typing the command:
.DM
	echo $?
.DE
This tells you the value returned by the last command executed.
Zero indicates success (true),
whereas a non-zero value indicates failure (false).
Note that this convention is the opposite of that in the C language
(a fact that has led to confusion on occasion).
.PP
.II cmp
You can use the value returned by a command to affect decisions about
executing other commands.
.SH "test: Condition Testing"
.PP
.II test
For most commands, the return value is a side-effect of their operation.
However, the \fBtest\fR command's only task is to return a value.
This command can test many conditions, and return a value to indicate
whether the requested condition is true or false.
.PP
The command
.DM
	test -f file01
.DE
returns true (zero) if \fBfile01\fR exists and is not a directory.
To check if a file is a directory, use:
.DM
	test -d file01
.DE
.II parameter^substitution
.II testing^strings
.II "substitution^of parameters"
.B test
can also test strings.
This is useful when you are using parameter substitution.
To illustrate, build the following command:
.DM
	ed
	a
	test $1 = $2
	echo 'test 1 & 2 for equal:' $?
	test $1 != $2
	echo 'test 1 & 2 for not equal:' $?
	.
	w test.ed
	q
	chmod +x test.ed
.DE
Because the `=' is a parameter, be sure to surround it with space characters.
.PP
This command file tests its two parameters for equality.
Try the commands:
.DM
	test.ed one two
	test.ed one one
.DE
The
.B test
command has many other options;
see the Lexicon entry for
.B test
for details.
.SH "Executing Commands Conditionally"
.PP
.II ||
.II cmp
Type the following commands to create two files:
.DM
	cat >file1
	line one
	line two
	line three
	<ctrl-D>
	cat >file2
	line one
	two is different
	line three
	<ctrl-D>
.DE
Now, compare the files and print the return value:
.DM
	cmp -s file1 file2
	echo $?
.DE
The command
.B cmp
compares two files byte-by-byte; the
.B \-s
option tells
.B cmp
merely to indicate whether the files were the same.
The command
.DM
	echo $?
.DE
prints
.B 1
(false) because the files are not the same.
.PP
To process a second command based on the result returned by the first, type:
.DM
	cmp -s file1 file2 || cat file2
.DE
The characters \fB||\fR signify that the following command \fBcat\fR should be
executed if the \fBcmp\fR
command returns a non-zero value, which it will for this example.
.PP
.II &&
The two characters \fB&&\fR execute the command that follows them
only if the preceding command returns true (zero).
.PP
To see how this works, create a third file with the command:
.DM
	cp file1 file3
.DE
Type the command:
.DM
	cmp -s file1 file3 && rm file3
.DE
This command removes \fBfile3\fR if \fBcmp\fR
indicates that \fBfile1\fR and \fBfile3\fR are identical.
Because \fBcmp\fR is preceded by the copy command
\fBcp\fR, the files \fBfile1\fR and \fBfile3\fR are identical,
and so \fBfile3\fR is removed.
.SH "Control Flow"
.PP
Because the shell is a programming language as well as a program, it
provides constructs for conditional execution and loops.
These are \fBfor\fR, \fBif\fR, \fBwhile\fR, \fBuntil\fR, and \fBcase\fR.
Also, a subshell can be executed within `(' and `)'.
.Sh "for: Execute a Loop"
.PP
The \fBfor\fR construct processes a set of commands once for
each element in a list of items.
.PP
.II do
.II done
To illustrate \fBfor\fR,
type the following commands to \*(CO:
.DM
	for i in a b c
	do echo $i
	done
.DE
The items
.BR a ,
.BR b ,
and
.B c
form the list of value that the variable
.B i
assumes.
The shell executes \fBecho\fR with \fBi\fR assuming each
value in turn.
The result of these commands is:
.DM
	a
	b
	c
.DE
.II done
.II >
.II for
Notice that after you type the line containing \fBfor\fR,
\*(CO prompts with a different character \fB>\fR
(on most \*(CO systems).
The shell does this to remind you that you must type more information.
After you type the line containing \fBdone\fR, the prompt again becomes
\fB$\fR.
.PP
The \fBfor\fR command is usually used within a script.
Also, you can leave off the list of value to the index variable;
when you do this, the shell by default uses the arguments typed on
the script's command line as the values for the index variable.
To illustrate, type:
.DM
	ed
	a
	for i
	do echo $i
	echo '---'
	done
	.
	w script.for
	q
	chmod +x script.for
.DE
The statement 
.B "for i"
is equivalent to:
.DM
	for i in $*
.DE
where \fB$*\fR means ``all positional parameters''.
Notice that two commands are repeated for each value of \fBi\fR.
Now, call
.B script.for
with the following command line:
.DM
	script.for 1 2 3 4 test
.DE
The result is:
.DM
	1
	---
	2
	---
	3
	---
	4
	---
	test
	---
.DE
.Sh "if: Execute Conditionally"
.PP
\fBif\fR tests the result of a command and
conditionally executes other commands based upon that result.
It can be used instead of \fB&&\fR and \fB||\fR, as shown above.
To demonstrate this, first type the command:
.DM
	cp file1 file3
.DE
This creates
.B file3
(because we deleted it on the previous page).
Then type:
.DM
	if cmp -s file1 file2 
	then cat file3
	fi
.DE
This means that the shell executes
.DM
	cat file3
.DE
if \fBcmp\fR returns zero (true).
.PP
.II fi
.II else
.II if
.II false
To get the same result as given by the previously illustrated command:
.DM
	cmp -s file1 file3 && rm file3
.DE
with the \fBif\fR statement, also use \fBelse\fR:
.DM
	if cmp -s file1 file3
	then
	else rm file3
	fi
.DE
The commands between \fBelse\fR and \fBfi\fR are executed if the result
of the command following the \fBif\fR is false or non-zero.
Note that there is no command following \fIthen\fR.
.PP
.II if
.II elif
The \fBelif\fR statement lets you test several
conditions with one \fBif\fR statement and act on the one that is true.
In general terms,
.DM
	if command1
	then action1
	elif command2
	then action2
	elif command3
	then action3
	else action4
	fi
.DE
The items labeled \fIcommand\fR and \fIaction\fR are both commands or lists of
commands.
.PP
First, the shell executes \fBcommand1\fR.
If the result is true, it performs \fBaction1\fR.
If the result from \fBcommand1\fR is not true, the shell then
executes \fBcommand2\fR.
If its result is true, then it performs \fBaction2\fR.
This process continues so long as none of the commands return a true result.
If none of the command results are true, the action following the
\fBelse\fR is executed.
.PP
To illustrate
.BR elif ,
create a shell script that list on your terminal
only one of the three file-name arguments.
Use the command
.DM
	test -f name
.DE
which returns true if \fIname\fR is an existing non-directory
file.
.DM
	ed
	a
	if test -f $1
	then cat $1
	elif test -f $2
	then cat $2
	elif test -f $3
	then cat $3
	else echo 'None are files'
	fi
	.
	w cat.1
	q
	chmod +x cat.1
.DE
Now, let's exercise
.BR cat.1 .
Type:
.DM
	cat.1 file1 file2 file3
	cat.1 file3 file2 file1
	cat.1 foo bar baz
.DE
Examine the results.
.Sh "while: Execute a Loop"
.PP
.II while
Another looping or repetitive shell statement is the \fBwhile\fR statement.
The commands
.DM
	while command1
	do command2
	done
.DE
first performs \fIcommand1\fR.
If its result is true, \fIcommand2\fR is
executed, and \fIcommand1\fR is again executed.
This process continues until \fIcommand1\fR returns false (non-zero).
.Sh "until: Another Looping Construct"
.PP
.II until
The construct
.B until
resembles
.BR while .
For example, the commands:
.DM
	until command1
	do command2
	done
.DE
execute \fIcommand2\fR until \fIcommand1\fR returns true (zero).
.Sh "case: Serial Conditional Execution"
.PP
.II case
The \fBcase\fR statement resembles the \fBif\fR
statement in that it offers a multiple choice.
To illustrate, type the following script, which lets you choose
one of several ways to list the contents of a directory:
.DM
	ed
	a
	case $1 in
	     1) ls -l;;
	     2) ls;;
	     3) lc;;
	     *) echo unknown parameter $1;;
	esac
	.
	w dir
	q
	chmod +x dir
.DE
.II )
.II esac
.II case
The words \fBcase\fR and \fBesac\fR bracket the entire \fBcase\fR statement.
The effect of the command
.DM
	dir 2
.DE
is equivalent to:
.DM
	ls
.DE
Each choice within the \fBcase\fR statement is indicated by a string followed
by \fB)\fR:
.DM
	2)
.DE
indicates what is to be executed if argument \fB$1\fR has the value \fB2\fR.
.PP
.II case
.II *
.II "choices^in case statements"
.II *)
.II "vertical bar"
.II |
The strings that select the choices may be patterns.
The choice `*)' signifies that a match can be made on any string.
Notice that this resembles the use of \fB*\fR to substitute any file name.
An expression of the form
.DM
	[1-9])
.DE
in a \fBcase\fR statement matches any digit from 1 through 9.
A list of alternatives can be presented by separating
the choices with a vertical bar:
.DM
	a|b|c) command
.DE
.II ";; (double semicolon)"
Each command or command list in the case choice must be
terminated by a double semicolon \fB;;\fR.
.SH Summary
.PP
The shell is a command programming language that handles simple commands
as well as complex commands that can iterate as well as make decisions.
Three kinds of substitution are provided to increase the power of your
commands.
.PP
For more information about the shell, see the tutorial for the shell
that follows in this manual.
For more information about a given command, see its entry in the Lexicon.
.PP
Note, too, that the \*(CO system also includes the Korn shell \fBksh\fR.
This is a superset of the Bourne shell described here, and has many features
that you may find useful.
For information about this shell, see the Lexicon entry for \fBksh\fR.
