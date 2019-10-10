.II "make^tutorial"
.ds TL "make Programming Discipline"
.NH "The make Programming Discipline"
.PP
\fBmake\fR is a utility that guides the building of complex things out of
one or more simpler things.
The ``complex thing'' can be practically
any sort of file that you create regularly, such as a report or a program.
.PP
Under \*(CO,
.B make
is most commonly used to control the building of complex C programs;
and it is in this context that
.B make
shows its power most easily.
This tutorial introduces the features of
.BR make ,
and discusses how to use it to help you build complex C programs easily
and efficiently.
.Sh "How Does make Work?"
.PP
To understand how
.B make
works, it is first necessary to understand how a C program
is built:
how \*(CO takes you from the C source code that you write
to the executable program that you can run on your computer.
.PP
The file of C source code that you write is called a
.IR "source module" .
When \*(CO compiles a source module, it uses the C code in
the source module, plus the code in the header files that the
code calls to produce an
.IR "object module" .
This object module is \fInot\fR executable by itself.
To create an \fIexecutable file\fR, the object module
generated from your source module must be handed to a linker,
which links the code in the object module with the appropriate
library routines that the object module calls, and adds the
appropriate C runtime startup routine.
.PP
For example, consider the following C program, called \fBhello.c\fR:
.DM
	main()
	{
		printf("Hello, world\en");
	}
.DE
When the C compiler compiles the file that contains C code shown above,
it generates an object module called \fBhello.o\fR.
This object module is not executable because it does not contain
the code to execute the function \fBprintf()\fR; that code is
contained in a library.
To create an executable program, you must hand \fBhello.o\fR
to the linker \fBld\fR,
which copies the code for \fBprintf\fR from a library and into
your program, adds the appropriate C runtime startup routine,
and writes the executable file called \fBhello\fR.
This third file, \fBhello\fR, is what you can execute on your computer.
.PP
The term
.I dependency
describes the relationship of executable file to object module
to source module.
The executable program
.I depends
on the object module and one or more libraries.
The object module, in turn, depends on the source module and
its header files (if any).
.PP
A program like \fBhello\fR has a simple set of dependencies:
the executable file is built from one object module, which in turn is
compiled from one source module.
If you changed the source module \fBhello.c\fR, creating an
updated version of \fBhello\fR would be easy:
you would simply compile \fBhello.c\fR to create \fBhello.o\fR,
which you would link with the library and the runtime startup
to create \fBhello\fR.
\*(CO, in fact, does this for you automatically:
all you need to do is type
.DM
	cc hello.c
.DE
and the C compiler takes care of everything.
.PP
On the other hand, the dependencies of a large program can be very complex.
For example, the executable file for the \*(ME screen
editor is built from several dozen object modules,
each of which is compiled from a source module plus one or more header files.
Updating a program as large as \*(ME, even when you
change only one source module, can be quite difficult.
To rebuild its executable file by hand, you must remember
the names of all of the source modules used, compile them,
and link them into the executable file.
Needless to say, it is very inefficient to recompile
several dozen object modules to create an executable when you
have changed only one of them.
.PP
.B make
automatically rebuilds large programs for you.
You prepare a file, called a \fBmakefile\fR, that describes your
program's chain of dependencies.
.B make
then reads your \fBmakefile\fR, checks to see which source
modules have been updated, recompiles only the ones that have
been changed, and then relinks all of the object modules to create
a new executable file.
.B make
both saves you time, because it recompiles only the source
modules that have changed, and spares you the drudgery of
rebuilding your large program by hand.
.Sh "Try make"
.PP
To see how \fBmake\fR works, try compiling a program called
.BR factor .
It is built from the following files:
.DM
	atod.c
	factor.c
	makefile
.DE
All three are kept in directory
.BR /usr/src/sample .
To use them, copy the following files into your current directory.
(By the way, first make sure that you do not already have a file named
.B makefile
in your current directory, or the following commands will overwrite it.)
.DM
	cp /usr/src/sample/atod.c .
	cp /usr/src/sample/factor.c .
	cp /usr/src/sample/makefile .
.DE
Now, type \fBmake\fR.
\fBmake\fR begins by reading \fBmakefile\fR, which describes
all of \fBfactor\fR's dependencies.
It then uses the \fBmakefile\fR description to create
.BR factor .
The following appear on your screen:
.DM
	cc -c factor.c
	cc -c atod.c
	cc -f -o factor factor.o atod.o -lm
.DE
Each of these messages describes an action that \fBmake\fR has
performed.
The first shows that \fBmake\fR is compiling \fBfactor.c\fR, the
second shows that it is compiling \fBatod.c\fR, and the third
shows that it is linking the compiled object modules \fBatod.o\fR
and \fBfactor.o\fR to create the executable file \fBfactor\fR.
.PP
When \fBmake\fR has finished, the shell prompt returns.
To see how your newly compiled program works, type
.DM
	factor 100
.DE
.B factor
calculates the prime factors of its argument \fB100\fR, and
print them on the screen:
.DM
	2 2 5 5
.DE
To see what happens if you try to re-make your file, type
.B make
again.
.B make
will run quietly for a moment, and then exit.
\fBmake\fR checked the dates and times of the
object modules and their corresponding source modules and saw that
the object modules had a time later than that of the source modules.
Because no source module changed, there was no need to recompile
an object module or relink the executable file, so \fBmake\fR
quietly exited.
.PP
To see what happens when one of the source modules changes, try
the following.
Use the \*(ME screen editor to open the file
.B factor.c
for editing.
Insert the following line into the comments at the top, immediately
following the \fB/*\fR:
.DM
	* This comment is for test purposes only.
.DE
Now type
.B <ctrl-Z>
to save the file and exit.
Type \fBmake\fR once again.
This time, you will see the following on your screen:
.DM
	cc -O -c factor.c
	cc -o factor factor.o atod.o -f -lm
.DE
Because you altered the source module \fBfactor.c\fR, its time
was later than that of its corresponding object module, \fBfactor.o\fR.
When
.B make
compared the times of \fBfactor.c\fR and \fBfactor.o\fR, it noted
that \fBfactor.c\fR had been altered.
It then recompiled \fBfactor.c\fR and relinked \fBfactor.o\fR and
\fBatod.o\fR to re-create the executable file \fBfactor\fR.
.B make
did not touch
the source module
.B atod.c
because \fBatod.c\fR had not been changed since the last time it was compiled.
.PP
As you can see, \fBmake\fR simplifies the construction of a
C program that uses more than one source module.
.SH "Essential make"
.PP
Although \fBmake\fR is a powerful program,
its basic features are easy to master.
This section will show you how to construct elementary \fBmakefile\fRs.
.Sh "The makefile"
.II "make^program, specification"
.II "make^specification"
.II "make^file"
.PP
When you invoke \fBmake\fR, it searches the directories named
in the environmental variable
.B PATH
for a file called \fBmakefile\fR or
.BR Makefile .
(You can tell
.B make
to read a file other than
.B makefile
or
.BR Makefile ;
see the description of \fBmake\fR's \fB\-f\fR option, below.)
As noted earlier, the \fBmakefile\fR is a text file that
describes a program's dependencies.
It also describes the type of
program you wish to build, and the commands for building it.
.PP
A \fBmakefile\fR has three basic parts.
.PP
First, the \fBmakefile\fR
describes the executable file's dependencies.
That is, it lists the object modules needed to create the executable file.
The name of the executable file is always followed
by a colon \*(Ql:\*(Qr
.II "make^colon"
and then by the names of files from which the target file is generated.
.PP
For example, if the program \fBfeud\fR
is built from the object modules \fBhatfield.o\fR and \fBmccoy.o\fR,
you would type:
.DM
	feud:    hatfield.o mccoy.o
.DE
If the files
\fBhatfield.o\fR and \fBmccoy.o\fR
do not exist, \fBmake\fR
knows to create them from the source modules \fBhatfield.c\fR and \fBmccoy.c\fR.
.PP
Second, the \fBmakefile\fR holds one or more
.I command
lines.
.II "make^command line"
The command line gives the command to
compile the program in question.
The only difference between a \fBmakefile\fR command
line and an ordinary \fBcc\fR command is
that a \fBmakefile\fR command line \fImust\fR begin with a
space or a tab character.
.PP
For example, the \fBmakefile\fR to generate the program
\fBfeud\fR must contain the following command line:
.DM
		cc -o feud hatfield.o mccoy.o
.DE
For a detailed description of the \fBcc\fR command and
its options, refer to the entry for
.B cc
in the Lexicon.
.PP
Third,
the \fBmakefile\fR lists all of the header files that your program uses.
(If you don't know what a header file is, see the entry for
.B #include
in the Lexicon.)
These files are given so that \fBmake\fR can check if
any had been modified since your program was last compiled.
For example, if the program \fBhatfield.c\fR used the header
file \fBshotgun.h\fR and \fBmccoy.c\fR used the header files
\fBrifle.h\fR and \fBpistol.h\fR, the \fBmakefile\fR to generate
\fBfeud\fR would include the following lines:
.DM
	hatfield.o: shotgun.h
	mccoy.o: rifle.h pistol.h
.DE
Thus, the entire \fBmakefile\fR to generate the program
\fBfeud\fR is as follows:
.DM
	feud: hatfield.o mccoy.o
		cc -o feud hatfield.o mccoy.o
.sp \n(PDu
	hatfield.o: shotgun.h
	mccoy.o: rifle.h pistol.h
.DE
A
.B makefile
can also contain
.I "macro definitions"
.II "make^macro, definition"
and
.IR comments .
.II "make^comment"
These are described below.
.Sh "Building a Simple makefile"
.PP
The program
.B factor
is built from two source modules,
.B factor.c
and
.BR atod.c .
No header files are used.
The \fBmakefile\fR contains the following two lines:
.DM
	factor: factor.o atod.o
		cc -o factor factor.o atod.o -f -lm
.DE
The first line describes the dependency for the executable file
\fBfactor\fR by naming the two object modules needed to build it.
The second line gives the command needed to build \fBfactor\fR.
The option \fB\-lm\fR at the end of the command line
tells \fBcc\fR that this program needs the mathematics library
.B libm
when the program is linked.
No header file dependencies are described
because these programs use no special header files.
.Sh "Comments and Macros"
.PP
.B make
ignores all lines that begin with a pound sign \*(Ql#\*(Qr.
This lets you embed comments within a \fBmakefile\fR,
to \*(QLdocument\*(QR the file so that whoever reads it will
know what it is for.
For example, you may wish to include the following comments
in your \fBmakefile\fR for \fBfactor\fR:
.DM
# This makefile generates the program "factor".
# "factor" consists of the source modules "factor.c" and
# "atod.c".  It uses the standard mathematics library
# "libm", but it requires no special header files.
# "-f" lets you use printf for floating-point numbers.
.sp \n(PDu
	factor: factor.o atod.o
		cc -f -o factor factor.o atod.o -lm
.DE
Anyone who reads this file will know immediately what it is for
by looking at the comments.
.PP
.B make
also lets you define macros within your \fBmakefile\fR.
A \fImacro\fR is a symbol that represents a string of text.
Usually, a macro is defined at the beginning of the \fBmakefile\fR
using a \fImacro definition statement\fR.
This statement uses the following syntax:
.DM
	SYMBOL = string of text
.DE
Thereafter, when you use the symbol in your \fBmakefile\fR, it
must begin with a dollar sign \*(Ql$\*(Qr and be enclosed within
parentheses.
(If the macro name is only one character long, the parentheses are not
required.)
A macro name can use both upper-case and lower-case characters.
.PP
Macros eliminate the chore of retyping long strings of file names.
For example, with the \fBmakefile\fR for the program \fBfactor\fR,
you may wish to use a macro
to substitute for the names of the object modules out of which it is
built.
This is done as follows:
.DM
# This makefile generates the program "factor".
# "factor" consists of the source modules "factor.c" and
# "atod.c".  It uses the standard mathematics library
# "libm", but it requires no special header files.
# "-f" lets you use printf for floating-point numbers.
.DE
.DM
.ta 0.5i 1.0i
OBJ = factor.o atod.o
factor: $(OBJ)
	cc -o factor $(OBJ) -f -lm
.DE
The macro \fBOBJ\fR is used in this \fBmakefile\fR.
If you use a macro that has not been defined,
\fBmake\fR substitutes an empty string for it.
The use of a macro makes sense when generating
large files out of a dozen or more source modules.
You avoid retyping the source module names, and
potential errors are avoided.
.PP
Note that you can define macros in the
.BR makefile ,
in the environment, or as a command-line argument.
A macro defined as a command-line argument always overrides a definition
of the same macro in the environment or in the
.BR makefile .
Normally, a definition in a
.B makefile
overrides a definition of the same macro name in the environment;
however, the
.B \-e
option to
.B make
forces definitions in the environment to override those in the
.BR makefile .
.Sh "Setting the Time"
.PP
As noted above, \fBmake\fR checks to see which source modules
have been modified before it regenerates your C program.
This is done to avoid wasteful
recompiling of source modules that have not been updated.
.PP
\fBmake\fR determines that a source module has been altered
by comparing its date against that of the target program.
For example, if the object module
\fBfactor.o\fR
was generated on March 16, 1992,
10:52:47 A.M., and the source module \fBfactor.c\fR was modified on
March 20, 1992, at 11:19:06 A.M., \fBmake\fR will know that \fBfactor.c\fR
needs to be recompiled because it is \fIyounger\fR than \fBfactor.o\fR.
.Sh "Building a Large Program"
.PP
As shown earlier, \fBmake\fR can ease the task of
generating a large program.
The following gives a \fBmakefile\fR that can be used to generate the
screen editor \*(ME:
.DM
# makefile for "MicroEMACS"
.sp \n(PDu
CFLAGS = -O
LFLAGS = /usr/lib/libterm.a
OBJ=ansi.o basic.o buffer.o display.o file.o \e
	fileio.o line.o main.o random.o region.o \e
	search.o spawn.o termio.o vt52.o window.o \e
	word.o tcap.o
.sp \n(PDu
me: $(OBJ)
	cc -o me $(OBJ) $(LFLAGS)
.sp \n(PDu
$(OBJ):	ed.h
.DE
Note that this
.B makefile
has been simplified for the purposes of this tutorial; the actual
.B makefile
that builds the \*(CO edition of \*(ME is considerably more complex.
.PP
The first line in the above
.B makefile
gives commentary that describes the file does.
The next five lines define macros that are used on the target and command lines.
The first macro will be discussed in the following section.
The second macro substitutes for the name of a special library that is needed
to create this program.
The third macro, which is three lines long, stands
for the names of the source modules that produce \*(ME.
A backslash \*(Ql\e\*(Qr must be used to tell \fBmake\fR
that the macro's definition extends onto the next line.
.PP
The next line names the target file (\fBme\fR) and the files
used to construct it, here represented by the macro \fBOBJ\fR.
.PP
Next comes the command line, which gives the compilation
to be performed.
This line \fImust\fR begin with a space or a tab.
.PP
The last line lists the header file \fBed.h\fR, which is
required by all of the files used to generate \*(ME.
.SH "Command-Line Options"
.II "make^command line, options"
.II "make^command line"
.II "make^-"
.II "make^hyphen"
.II "make^options"
.PP
Although \fBmake\fR is controlled by your \fBmakefile\fR,
you can also control \fBmake\fR by using command-line options.
These allow you to alter \fBmake\fR's activity without
editing your \fBmakefile\fR.
.PP
Options must follow the command name on the command line
and begin with a hyphen, \*(Ql\-\*(Qr, using the following format.
The square brackets merely indicate that you can select
any of these options;
do \fInot\fR type the brackets when you use the \fBmake\fR command:
.DM
	make [ \-deinpqrst ] [ \-f \fIfilename\fP ]
.DE
Each option is described below.
.IP \fB\-d\fR 0.4i
.II "make^debug option"
Debug option:
.B make
describes all of its decisions.
You can use this to debug your \fBmakefile\fR.
.IP \fB\-e\fR
Environment option:
force definitions in the environment to override those in the
.BR makefile .
For example, if the
.B makefile
defines
.DM
	foo=makefoo
.DE
and the environment defines
.DM
	foo=envfoo
.DE
then
.B $(foo)
expands to
.B makefoo
if you use the command
.B make
but expands to
.B envfoo
if you use the command
.BR "make \-e" .
.IP "\fB\-f \fIfilename\fR"
.II "make^file option"
File option:
Tell \fBmake\fR that its commands
are in a file other than \fBmakefile\fR.
For example, the command
.DM
	make -f smith
.DE
tells \fBmake\fR to use the file
\fBsmith\fR rather than \fBmakefile\fR.
.II "make^program, specification"
.II "make^specification"
If you do not use this option, \fBmake\fR
searches the directories named in the environmental variable \fBPATH\fR,
and then the current directory for a file entitled
.B makefile
or
.B Makefile
.II "make^Makefile"
to execute.
.II "make^file"
.IP \fB\-i\fR
Ignore errors:
.II "make^ignore errors option"
.B make
ignores error returns from commands and continues processing.
.II "make^error status"
.II "make^command, error"
Normally,
.B make
exits if a command returns an error status.
.IP \fB\-n\fR
.II "make^no execution option"
No execution:
.B make
tests dependencies and modification times but
does not execute commands.
This option is especially helpful when constructing or debugging a
.BR makefile .
.IP \fB\-p\fR
Print:
.II "make^print option"
.II "make^macro, printing"
.II "make^target, printing"
.II "make^printing"
.B make
prints all macro definitions and target descriptions.
.IP \fB\-q\fR
Quit option:
Return a zero exit status if the targets are up to date.
Do not execute any commands.
.II "make^macro, definition"
.IP \fB\-r\fR
Rules option:
.II "make^rules option"
.II "make^no rules option"
.B make
does not use the default macros and commands from
.B /usr/lib/makemacros
.II "make^/usr/lib/makemacros"
.II "make^macros"
and
.BR /usr/lib/makeactions .
.II "make^/usr/lib/makeactions"
.II "make^actions"
These files will be described below.
.IP \fB\-s\fR
Silence:
.II "make^silent option"
.II "make^command, printing"
.B make
does not print each command line as it is executed.
.IP \fB\-t\fR
Touch:
.II "make^touch option"
.B make
changes the
.II "make^modification time"
.II "make^file modification time"
modification time of each executable file and object module
to the current time.
This suppresses recreation of the executable file,
and recompilation of the object modules.
Although this option is used typically after a purely cosmetic change
to a source module or after adding a definition to a header file,
it must be used with great caution.
.Sh "Other Command Line Features"
.II "make^command line, macro definition"
.II "make^command line"
.II "make^macro, definition"
.II "make^macro"
.PP
In addition to the options listed above, you
may include other information on your command line.
.PP
First, you can define macros on the command line.
A macro definition must \fIfollow\fR all command-line options.
Arguments, including spaces, must be surrounded by quotation marks,
as spaces are significant to the shell.
For example, the command line
.DM
	make -n -f smith "OPT=-DTEST"
.DE
tells \fBmake\fR to run in the \fIno execution\fR mode, read
the file \fBsmith\fR instead of \fBmakefile\fR, and define the macro
\fBOPT\fR to mean \fB\-DTEST\fR.
.PP
The ability to define macros on the command line means that you
can create a \fBmakefile\fR using macros that are not yet defined;
this greatly increases \fBmake\fR's flexibility and makes it even
more helpful in creating and debugging large programs.
In the above example, you can define a command line as follows:
.DM
		cc $(OPT) example.c
.DE
When you define the macro \fBOPT\fR on the command line, then the
program is compiled using the \fB\-DTEST\fR option, which defines
the preprocessor variable
.BR TEST .
.PP
As noted above, a macro defined on the command line always overrides
an identically named macro defined either in the environment or in the
.BR makefile .
.PP
.II "make^command line, target specification"
.II "make^command line"
.II "make^target"
.II "make^target, specification"
.II "make^target, program"
Another command-line feature lets you change the
name of the \fItarget file\fR on the command line.
Normally, the target file is the executable file that you
wish to create, although, as will be seen, it does not have to be.
As will be discussed below,
a \fBmakefile\fR can name more than one target file.
.B make
normally assumes that the target is the first target file named in
.BR makefile .
However, the command line may name one or more
target files at the end of the line,
after any options and any macro definitions.
.PP
To see how this works, recall the program \fBfactor\fR described above.
\fBfactor\fR is generated out of the source modules \fBfactor.c\fR
and \fBatod.c\fR.
The command
.DM
	make atod.o
.DE
with the \fBmakefile\fR outlined above would produce the
following \fBcc\fR command line:
.DM
	cc -c atod.c
.DE
if the object module
\fBatod.o\fR
did not exist or were outdated.
Here,
.B make
compiles
.B atod.c
to create the target specified in the \fBmake\fR command line,
that is, \fBatod.o\fR, but it does not create
.BR factor .
This feature allows you to apply your \fBmakefile\fR to only a
portion of your program.
.PP
The use of special, or \fIalternative\fR, target files is discussed below.
.SH "Advanced make"
.PP
This section describes some of \fBmake\fR's advanced features.
For most of your work, you will not need these features; however,
if you create an extremely complex program, you will find them most helpful.
.Sh "Default Rules"
.II "make^default rules"
.PP
The operation of \fBmake\fR is governed by a set of \fIdefault rules\fR.
These rules were designed to simplify the compilation of a typical program;
however, unusual tasks may require that you bypass or alter the default rules.
.PP
To begin,
.B make
uses information from the files
.B /usr/lib/makemacros
.II "make^/usr/lib/makemacros"
.II "make^macros"
and
.B /usr/lib/makeactions
.II "make^/usr/lib/makeactions"
.II "make^usr/lib/makeactions"
.II "make^actions"
to define default macros and compilation commands.
.B make
uses the commands in \fBmakemacros\fR and \fBmakeactions\fR whenever the
.B makefile
specifies no explicit regeneration commands.
The command line option
.B \-r
tells
.B make
not to use the macros and actions defined in \fBmakemacros\fR
and \fBmakeactions\fR.
.PP
As shown in earlier examples,
.B make
knows by default to generate the object module
.B atod.o
from the source module
.B atod.c
with the command
.DM
	cc -c atod.c
.DE
The macro
.B .SUFFIXES
.II "make^.SUFFIXES"
defines the suffixes
.B make
knows about by default.
Its definition in \fBmakemacros\fR
includes both the
.B .o
and
.B .c
suffixes.
.PP
\fBmake\fR's files \fBmakemacros\fR and \fBmakeactions\fR
use pre-defined macros to increase their scope and flexibility.
These are as follows:
.IP \fB$<\fR 0.4i
.II "make^$<"
This stands for the name of the file or files that cause the action
of a default rule.
For example, if you altered the file \fBatod.c\fR and then invoked
\fBmake\fR to rebuild the executable file \fBfactor\fR,
\fB$<\fR would then stand for \fBatod.c\fR.
.IP \fB$*\fR
.II "make^$*"
This stands for the name of the target of a default rule
with its suffix removed.
If it had been used in the above example, \fB$*\fR
would have stood for \fBatod\fR.
.sp \n(PDu
\fB$<\fR and \fB$*\fR work
.I only
with default rules;
these macros will not work in a
.BR makefile .
.IP \fB$?\fR
.II "make^$?"
This stands for the names of the files that cause the action and that are
younger than the target file.
.IP \fB$@\fR
This
.II "make^$@"
stands for the target name.
.PP
You can use the macros \fB$?\fR and \fB$@\fR in a
.BR makefile .
For example, the following rule updates the archive
.B libx.a
with the objects defined by macro
.B $(OBJ)
that are out of date:
.DM
	libx.a:	$(OBJ)
		ar rv libx.a $?
.DE
.PP
For more information on archives, see the Lexicon entry for the command
.BR ar .
.PP
\fBmakemacros\fR also contains default commands that describe
how to build additional kinds of files:
.IP \(bu 0.3i
\fBAS\fR and \fBASFLAGS\fR call the
.II "make^assembler"
assembler
.B as
to assemble
.B .o
files out of files with the suffix
.BR .s ,
which
.B make
assumes hold assembly language.
.IP \(bu
.B YACC
and
.B YFLAGS
.II "make^yacc"
call \fByacc\fR to build
.B .o
or
.B .c
files from files with the suffix
.BR .y ,
which
.B make
assumes hold
.B yacc
source code.
.IP \(bu
.B LEX
and
.B LFLAGS
call
.B lex
.II "make^lex"
to build
.B .o
or
.B .c
files from files with the suffix
.BR .l ,
which
.B make
assume hold
.B lex
source code.
.PP
You can change the default rules of \fBmake\fR by changing
them in \fBmakeactions\fR
and changing the definition of any of the macros as given in
\fBmakemacros\fR.
.Sh "Source File Path"
.PP
If a file is not specified with an absolute path name beginning with `/',
.B make
first looks for the file in the current directory.
If the file is not found in the current directory,
.B make
searches for it in the list of directories specified by the macro
.BR $(SRCPATH) .
This allows you to compile a program in an object directory separate from the
source path.
.PP
For example
.DM
	export SRCPATH=/usr/src/local/me
	make
.DE
or alternatively
.DM
	make SRCPATH=/usr/src/local/me
.DE
builds objects in the current directory as specified by the
.B makefile
from sources kept in directory
.BR /usr/src/local/me .
To test changes to a program built from several source files, copy only
the files you wish to change to the current directory;
.B make
will use the local sources and find the other sources on the
.BR $(SRCPATH) .
.PP
Note that
.B $(SRCPATH)
can be a single directory, as in the above example, or a list of
directories.
In the latter case, each entry in the list must be separated by a colon `:',
as described in the Lexicon entry for the function
.BR path() .
.Sh "Double-Colon Target Lines"
.II "make^double colon"
.II "make^target, line"
.PP
.II "make^archive"
.II "make^colon"
An alternative form of target line simplifies the task of maintaining archives.
This form uses the double colon \*(QL::\*(QR
instead of a single colon \*(Ql:\*(Qr
to separate the name of the target from those of the files on which it depends.
.PP
A target name can appear on only one single-colon target line,
whereas it can appear on several double-colon target lines.
The advantage of using the double-colon target lines is that
.B make
remakes the target by executing the commands (or its default commands)
for the \fIfirst\fR such target line for which
the target is older than a file on which it depends.
.PP
For example, for the program
\fBfactor\fR described earlier,
assume that two versions of the source modules \fBfactor.c\fR
and \fBatod.c\fR exist:
fBfactora.c\fR plus \fBatoda.c\fR,
and \fBfactorb.c\fR plus \fBatodb.c\fR
The \fBmakefile\fR would appear as follows:
.DM
	OBJ1 = factora.o atoda.o
	OBJ2 = factorb.o atodb.o
.DE
.DM
	factor:: $(OBJ1)
		cc -c $(OBJ1) -lm
.sp \n(PDu
	factor:: $(OBJ2)
		cc -c $(OBJ2) -lm
.DE
This \fBmakefile\fR tells \fBmake\fR to do the following:
(1) Check if either
\fBfactora.o\fR or \fBatoda.o\fR is younger than \fBfactor\fR.
(2) If either one is, regenerate
\fBfactor\fR using this version of these files.
(3) If neither
\fBfactora.o\fR nor \fBatoda.o\fR is younger than \fBfactor\fR,
then check to see if either
\fBfactorb.o\fR or \fBatodb.o\fR is
younger than \fBfactor\fR.
(4) If either of them is, then
regenerate \fBfactor\fR using the youngest version of these files.
.PP
This technique allows you to maintain multiple versions of source
files in the same directory and selectively recompile the most
recently updated version without having to edit your \fBmakefile\fR
or otherwise trick the system.
.PP
You cannot target a file in both a single-colon and a double-colon target line.
.Sh "Special Targets"
.II "make^special targets"
.II "make^target"
.PP
A few target names have special meanings to
.BR make .
The name of each special target begins with \*(Ql.\*(Qr and
contains upper-case letters.
.PP
The target name
.B .DEFAULT
.II "make^.DEFAULT"
defines the default commands
.B make
uses if it cannot find any other way to build a target.
The special target
.B .IGNORE
.II "make^.IGNORE"
in a
.B makefile
has the same effect as the
.B \-i
command line option.
Similarly,
.B .SILENT
.II "make^.SILENT"
.II "make^silent option"
has the same effect as the
.B \-s
command line option.
.Sh Errors
.II "make^errors"
.PP
.B make
prints \*(QL\fIcommand\fR exited with status \fIn\fR\*(QR
and exits if an executed \fIcommand\fR returns an error status.
.II "make^command, error"
.II "make^error status"
.II "make^ignore errors option"
.II "make^-"
However, it ignores the error status and continues processing if the
.B makefile
command line begins with a hyphen \*(Ql\fB-\fR\*(Qr or if the
.B make
command line specifies the
.B \-i
option.
.PP
.B make
reports an error status and exits if the user interrupts it.
.II "make^interrupt"
It prints \*(QL\fBcan't open \fIfile\fR\*(QR
if it cannot find the specification
.IR file .
It prints \*(QL\fBTarget \fIfile\fB is not defined\fR\*(QR
or \*(QL\fBDon't know how to make \fItarget\fR\*(QR
if it cannot find an appropriate
.I file
or commands to generate
.IR target .
Other possible errors include syntax errors in the specification file,
macro definition errors, and running out of space.
The error messages
.B make
prints are generally self-explanatory.
The section
.BR "Error Messages" ,
at the end of this manual,
lists
.BR make's
error messages and describes them briefly.
.Sh "Exit Status"
.II "make^exit status"
.II "make^return value"
.PP
.B make
normally returns a status of zero if it succeeds, and of one if an error occurs.
With the
.B \-q
option (described above),
.B make
returns zero if all files are up to date and two if they are not up to date.
.SH "Alternative Uses"
.PP
.B make
is a program that helps you construct complex things
from a number of simpler things.
.PP
.B make
usually is used to build complex C programs:
the executable file is made from object modules,
which are made from source modules and header files.
However, you can also use \fBmake\fR to build any file that is
constructed from one or more source modules.
For example, an accountant can use \fBmake\fR
to generate monthly reports from daily inventories:
all the accountant has to do is prepare a \fBmakefile\fR that describes
the dependencies (that is, the name of the monthly report he
wishes to create and the names of the daily inventories from which it
is created), and the command required to generate the monthly report.
Thereafter, to recreate the report, all the accountant has to do to
generate a monthly report is type \fBmake\fR.
.PP
In another example, the
.B makefile
can trigger program-maintenance commands.
For example, the target name
.B backup
might define commands to copy source modules to another directory; typing
.B "make backup"
saves a copy of the source modules.
Similar uses include removing temporary files, building archives,
executing test suites, and printing listings.
A
.B makefile
is a convenient place to keep all the commands used to maintain a program.
.PP
The following example shows a
.B makefile
that defines two special target files,
.B printall
and
.BR printnew ,
to be used with the source files for the program \fBfactor\fR.
.DM
# This makefile generates the program "factor".
# "factor" consists of the source modules "factor.c" and
# "atod.c".  It uses the standard mathematics library
# libm, but it requires no special header files.
.sp \n(PDu
OBJ = factor.o atod.o
SRC = factor.c atod.c
.sp \n(PDu
factor: $(OBJ)
	cc -o factor $(OBJ) -lm
.sp \n(PDu
# program to print all the updated source modules
# used to generate the program "factor"
.DE
.DM
printall:
	pr $(SRC) | lpr
	>printnew
.sp \n(PDu
printnew: $(SRC)
	pr $? | lpr
	>printnew
.DE
In this instance, typing the command
.DM
	make printall
.DE
forces \fBmake\fR to generate the target \fBprintall\fR
rather than the target \fBfactor\fR, which is the
default as it appears first in the \fBmakefile\fR.
The
.B pr
and \fBlpr\fR commands are then used to print a listing of all files defined by
.BR SRC .
The macro \fBOBJ\fR cannot be used with these commands because it
would trigger the printing of the object files, which would not be of much use.
It also creates an empty file
.BR printnew .
This new file serves only to record the time the listing is printed.
This tactic is performed in order to record the time that the listing was
last generated so that \fBmake\fR will know what files have
been updated when you next use \fBprintnew\fR.
.PP
Typing the command
.DM
	make printnew
.DE
forces \fBmake\fR to generate the target \fBprintnew\fR
rather than the default target \fBfactor\fR.
\fBprintnew\fR prints only the files named in the macro
.B SRC
that have changed since any files were last printed.
.SH "Where To Go From Here"
.PP
The Lexicon article on \fBmake\fR summarizes \fBmake\fR's options and
features.
The source code included with the \*(CO system and with the COHware
packages include \fBmakefile\fRs.
Studying them will show you how \fBmake\fR has been used to control
the building of large, real-world applications.
