.SH "C for Beginners"
.II "introduction to C programming"
.II "C programming^introduction"
.PP
This section briefly introduces the C programming language.
It is in two parts.
The first part describes what a programming language is, and
gives the history of the C programming language.
It also introduces some concepts basic to C, such as
.IR "structured programming" ,
.IR pointer ,
and
.IR operator .
The second part walks through a C programming session.
It emphasizes how a C programmer deals with a real problem, and
demonstrates some aspects of the language.
.PP
This chapter is not designed to teach you the entire C language.
It introduces you to C, so you can read
the rest of this manual with some understanding.
We urge you to look up individual topics of C programming in
the Lexicon, and especially to study the example programs given there.
.Sh "Programming Languages and C"
.PP
Before beginning with C, it is worthwhile to review how a
microprocessor and a computer language work.
.PP
.II microprocessor
A \fImicroprocessor\fR is the part of your computer that actually computes.
.II instructions
Built into it is a group of \fIinstructions\fR.
Each instruction tells the microprocessor to perform a task;
for example, one instruction adds two numbers together,
another stores the result of an arithmetic operation in memory,
and a third copies data from one point in memory to another.
.PP
.II "instruction set"
Together, a microprocessor's instructions form its \fIinstruction set\fR.
The instruction set is, in effect, the microprocessor's
\*(QLnative language\*(QR.
.PP
A microprocessor also contains areas of very
fast storage, called \fIregisters\fR.
.II register
The registers are essential to
arithmetic and data handling within the microprocessor.
How many registers a microprocessor has, and how they are designed,
help to determine how much memory the microprocessor can read and write, or
\fIaddress\fR, and how the microprocessor handles data.
.II address
.PP
.II "computer language"
A \fIcomputer language\fR, as the name implies, lets a human being
use the microprocessor's instruction set.
.II "assembly language"
The lowest level language is called \*(QLassembly language\*(QR.
In assembly language, the programmer calls instructions
directly from the microcomputer's instruction set, and
manipulates the registers within the microprocessor.
To write programs in assembly language, a programmer must
know both the microprocessor's instruction set and the
configuration of its registers.
.Sh "Assembly and High-Level Languages"
.PP
With assembly language,
the programmer can tailor the program specifically to the microprocessor.
However, because each microprocessor has
a unique instruction set and configuration of registers, a
program written in one microprocessor's assembly language cannot
be run on another microprocessor.
For example, no program
written in the assembly language for the Motorola 68000 microprocessor
can be run on the \*(BM or any \f(SCPC\fP-compatible computer.
The program must be entirely rewritten in the assembly language for
the Intel microprocessor, which is difficult and time consuming.
.PP
.II "high-level language"
A \fIhigh-level language\fR helps programmers to avoid these problems.
The programmer does not need to know the microprocessor in detail;
instead of specific microprocessor instructions, he writes a set of
logical constructions.
These constructions are then handed to another program,
which translates them into the
instructions and register calls used by a specific microprocessor.
In theory, a program written in a high-level language can be run on
any microprocessor for which someone has written a translation program.
.PP
A high-level language
allows the programmer to concentrate on the task being executed,
rather than on the details of registers and instructions.
This means that programs can be written more quickly than in assembly
language, and can be maintained more easily.
.Sh "So, What Is C?"
.PP
.II "Ritchie, Dennis"
As noted earlier, C was invented at AT&T Bell Laboratories
by Dennis Ritchie and Ken Thompson.
They created C specifically to re-write the
\*(UN operating system from PDP-11 assembly language.
Ritchie designed C to have the power, speed,
and flexibility of assembly language,
but the portability of high-level languages.
.PP
.II "Kernighan, Brian W."
.II "The C Programming Language"
In 1978, Ritchie and Brian W. Kernighan published
\fIThe C Programming Language\fR,
which described and defined the C language.
.II ANSI
In 1988, the American National Standards Institute (ANSI) published its
standard for the C language.
This standard has, on the whole, become the basis for current implementations
of C.
.PP
Because C is modeled after assembly language,
it has been called a \*(QLmedium-level\*(QR language.
The programmer doesn't have to worry about specific registers or specific
instructions, but he can use all of the
power of the computer almost as directly as he can with assembly language.
.PP
Because C was written by experienced programmers for experienced programmers,
it makes little effort to protect a programmer from himself.
A programmer can easily write a C program that is
legal and compiles correctly but crashes when run.
.II operators
Also, C's punctuation marks,
or \*(QLoperators\*(QR, closely resemble each other.
Thus, a mistake in typing can create a legal
program that compiles correctly but behaves very differently from what
you expect.
.Sh "Structured Programming"
.II "structured programming"
.PP
C is a \fIstructured language\fR.
This means that a C program is assembled from a number of sub-programs,
or \fIfunctions\fR, each of which performs a discrete task.
If this concept is difficult to grasp, consider the following example.
.PP
Suppose you want to turn a file of text into upper-case letters
and print it on the screen.
This job seems simple, but a program to do it must perform five tasks:
.IP \fB1.\fR 0.3i
Read the name of the file to open.
.IP \fB2.\fR
Open the file so it can be read, in much the same way that you
must open a book before you can read it.
.IP \fB3.\fR
Read the text from the file.
.IP \fB4.\fR
Turn what is read into upper-case letters.
.IP \fB5.\fR
Print the transformed text onto the screen.
.PP
A good program will also perform the following tasks:
.IP \fB1.\fR 0.3i
Check that the file requested actually exists.
.IP \fB2.\fR
Check that the file requested is actually a
text file rather than a file of binary information;
the latter makes very little sense when printed on the screen.
.IP \fB3.\fR
Close the program neatly when the work is finished.
.IP \fB4.\fR
Stop processing and print an error message if a problem occurs.
.PP
A structured language like C allows you to write a separate function
for each of these tasks.
.PP
.II function
A structured programming language offers two major advantages
over a non-structured language.
First, it is easier to debug a function than an
entire program because the function can be unplugged from the
program as a whole, made to work correctly, and then plugged back in again.
Second, once a function works, it can be used again and again in
different programs.
.II library
This allows you to create a \fIlibrary\fR of
reliable functions that you can pull off the shelf whenever you need them.
.PP
The functions within a program communicate by passing values to each other.
The value being passed can be an integer, a character, or \(em most
commonly \(em an address within memory where a function can
find data to manipulate.
.II pointer
This passing of addresses, or \fIpointers\fR, is the most efficient way to
manipulate data because by receiving one number, a function
can find its way to a large amount of data.
This speeds up a program's execution.
.PP
C adds some extra tools to help you construct programs.
To begin, C allows you to store functions in compiled form.
These precompiled functions are added only when the program is finally
loaded into memory; this spares you the trouble of having to recompile
the same code again and again.
.II macro
.II "header file"
Second, C adds a preprocessor that expands definitions, or \fImacros\fR,
and pulls in special material stored in \fIheader files\fR.
This allows you to store often-used
definitions in one file and use them just by adding one line to your program.
.Sh "Writing a C Program"
.PP
As noted above, a C program consists of a bundle of sub-programs,
or \fIfunctions\fR, which link together to perform the task you want done.
.II main()
Every C program must have one function that is called \fBmain\fR.
This is the main function; when the computer reads this,
it knows that it must begin to execute the program.
All other functions are subordinate to \fBmain\fR.
When the \fBmain\fR function is finished, the program is over.
.PP
To see how these elements work, review the program \fBhello.c\fR,
which you worked with earlier in this tutorial:
.DM
	main()
	{
		printf("Hello, world\en");
	}
.DE
As you can see, this program begins with the word \fBmain\fR.
The program begins to work at this point.
.II argument
The parentheses after \fBmain\fR
enclose all of the \fIarguments\fR to \fBmain\fR \(em or would, if
this program's \fBmain\fR took any.
An argument is an item of information that a function uses in its work.
.PP
.II braces
The braces \*(Ql{\*(Qr and \*(Ql}\*(Qr
enclose all the material that is subsidiary to \fBmain\fR.
.PP
.II printf()
The word
\*(QLprintf\*(QR \fIcalls\fR a function called \fBprintf()\fR.
This function performs formatted printing.
The line of characters (or \*(QLstring\*(QR) \fIHello, world\fR
is the argument to \fBprintf()\fR:
this argument is what \fBprintf()\fR is to print.
.PP
.II "carriage return"
The characters \*(Ql\en\*(Qr stand for a newline character.
This character ``tosses the carriage'', or moves the cursor to a new
line and returns it to the leftmost column on your screen.
Using this character ensures that when printing is finished,
the cursor is not left fixed in the middle of the screen.
.II semicolon
Finally, the semicolon \*(Ql;\*(Qr at the end of the command
indicates that the function call is finished.
.PP
One point to remember is that \fBprintf()\fR is \fInot\fR part of
the C language.
Rather, it is a \fIfunction\fR that was written by Mark Williams Company,
then compiled and stored in a library for your use.
This means that you do not have to re-invent a formatted printing
function to perform this simple task:
all you have to do is \fIcall\fR the one that Mark Williams has written
for you.
.PP
Although most C programs are more complicated than this example,
every C program has the same elements:
a function called \fBmain()\fR, which marks where execution begins and ends;
braces that fence off blocks of code;
functions that are called from libraries;
and data passed to functions in the form of arguments.
.SH "A Sample C Programming Session"
.PP
This section walks you through a C programming session.
It shows how you can go about planning and writing a program in C.
.PP
C allows you to be precise in your programming,
which should make you a stronger programmer.
Be careful, however, because C does exactly what you tell it to do,
nothing more and nothing less.
If you make a mistake, you can produce
a legal C program that does very unexpected things.
.Sh "Designing a Program"
.PP
Most programmers prefer to work on a program that does something fun or useful.
Therefore, we will write something useful:
a version of the \*(CO utility
.BR scat ,
that we'll call
.BR display .
It will do the following:
.IP 1. 0.3i
Open a text file on disk.
.IP "2."
Display its contents in 23-line chunks (one full screen).
.IP "3."
After displaying a chunk,
wait to see if the user wants to see another chunk.
If the user presses the \fB<return>\fR key alone, display another chunk;
if the user types any other key before pressing the \fB<return>\fR key, exit.
.IP "4."
Exit automatically when the end of file is reached.
.PP
As you can see, the first step in writing a program is to
write down what the program is to do, in as much detail as you
can manage, and preferably in complete sentences.
.PP
Now, invoke \fBed\fR or \*(ME and get ready to type in the program:
.DM
	ed display.c
.DE
or:
.DM
	me display.c
.DE
We suggest that you use the \*(ME editor, because this tutorial will
make numerous changes to the program as it progresses and it will be easier
to see these changes in context if you use a screen editor rather than a
line editor.
The rest of this tutorial assumes that you are using \*(ME.
If you are not familiar with \*(ME, it is briefly described in
\fIUsing the COHERENT System\fR.
A tutorial for \*(ME also appears in this manual,
or you may wish to see the entry for \fBme\fR in the Lexicon.
.PP
In the above commands, the suffix
.B .c
on the file name indicates that this is a file of C code.
If you do not use this suffix, the \fBcc\fR command will not recognize
that this is a file of C code and will refuse to compile it.
.PP
.II comment
Begin by inserting a description of the program into the top of the file in
the form of a
.IR comment .
When a C compiler sees the symbol
\*(Ql/*\*(Qr, it throws away everything it reads until it sees the symbol
\*(Ql*/\*(Qr.
This lets you insert text into your program to explain
what the program does.
.PP
Type the following:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
Save what you have typed by pressing \fB<ctrl-X>\fR and then \fB<ctrl-S>\fR.
Now, anyone, including you, who looks at this program will know exactly
what it is meant to do.
.Sh "The main() Function"
.PP
As described earlier, the C language permits
.IR "structured programming" .
This means that you can break your program into a group of discrete
functions, each of which performs one task.
Each function can be perfected by itself, and then used again and again
when you need to execute its task.
C requires, however, that you signal which function is the \fImain()\fR
function, the one that controls the operation of the other functions.
Thus, each C program must have a function called
.BR main() .
.PP
.II main()
Now, add
.B main()
to your program.
Type the code that is shaded, below:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
.ID
.SL
main()
.SL
{
.SL
}
.IE
The parentheses \*(QL()\*(QR show that
.B main()
is a function.
If \fBmain()\fR were to take any arguments,
they would be named between the parentheses.
The braces \*(QL{}\*(QR delimit all code that is subordinate to
.BR main() ;
this will be explained in more detail below.
.PP
Note that the shortest legal C program is
.BR main(){} .
This program doesn't do anything when you run it,
but it will compile correctly and generate an executable file.
.PP
Now, try compiling the program.
Save your text by typing
.BR <ctrl-X><ctrl-S> ,
and then exit from the editor by typing
.BR <ctrl-X><ctrl-C> .
Compile the program by typing:
.ID
	cc display.c
.IE
When compilation is finished, type
.BR display .
The shell will pause briefly, then return the prompt to your screen.
As you can see, you now have a legal, compilable C program, but one
that does nothing.
.Sh "Open a File and Show Text"
.PP
The next step is to install routines that open a file and print
its contents.
For the moment, the program will read only a file called \fBdisplay.c\fR,
and not break it into 23-line portions.
.PP
Type the shaded lines into your program, as follows:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
.ID
.SL
#include <stdio.h>
.sp \n(PDu
main()
{
.SL
	char string[128];
.SL
	FILE *fileptr;
.IE
.ID
.SL
/* Open file */
.SL
	fileptr = fopen("display.c", "r");
.IE
.ID
.SL
/* Read material and display it */
.SL
	for (;;) {
.SL
		fgets(string, 128, fileptr);
.SL
		printf("%s", string);
.SL
	}
}
.IE
Note first how comments are inserted into the text, to guide the reader.
.PP
Now, note the lines
.ID
	char string[128];
	FILE *fileptr;
.IE
.II char
.II FILE
These
.I declare
two data structures.
.II "data structure"
That is, they tell \*(PN to set aside a specific amount of memory for them.
.PP
.II array
The first declaration, \fBchar string[128];\fR,
declares an array of 128 \fBchar\fRs.
A \fBchar\fR is a data entity that is exactly one byte long;
this is enough space to store exactly one alphanumeric character in memory,
hence its name.
An \fIarray\fR is a set of data elements that are recorded together in memory.
In this instance, the declaration sets aside 128 \fBchar\fRs-worth
of memory.
This declaration reserves space in memory
to hold the data that your program reads.
.PP
.II pointer
The second declaration, \fBFILE *fileptr\fR, declares a
.I pointer
to a \fBFILE\fR structure.
The asterisk shows that the data element points to something,
rather than being the thing itself.
When a variable is declared to be a pointer, the C compiler
sets aside enough space in memory to hold an \fIaddress\fR.
.II address
When your program reads that address, it then knows where the
actual data are residing, and looks for them there.
C uses pointers extensively, because it is much more efficient to
pass the address of data than to pass the data themselves.
You may find the concept of pointers to be a little difficult to grasp;
however, as you gain experience with C, you will find that
they become easy to use.
.PP
.II structure
The \fBFILE\fR structure is the data entity that holds all the
information your program needs to read information from or write
information to a file on the disk.
For a detailed discussion of the \fBFILE\fR structure, see its entry
in the Lexicon.
For now, all you need to remember is that this declaration
sets aside a place to hold a pointer to such a structure, and the
structure itself holds all of the information your program needs
to manipulate a file on disk.
In effect, the variable
.B fileptr
is used within your program as a synonym for the file itself.
.PP
Now, the line
.ID
	fileptr = fopen("display.c", "r");
.IE
.II fopen()
opens the file to be read.
The function
.B fopen()
\fIopens\fR the file, fills the
.B FILE
structure,
and fills the variable
.B fileptr
with the address of where that structure resides in memory.
.PP
.B fopen()
takes two arguments.
The first is the name of the file to be opened, within quotation marks.
The second argument indicates the
.I mode
in which to open the file;
.B r
indicates that the file will be read rather than written into.
.PP
The lines
.ID
	for(;;)
	{
.IE
.II for
begin a
.IR loop .
.II loop
A loop is a section of code that is executed repeatedly until a
condition that you set is fulfilled.
For example, you may define a loop that executes until the
value of a particular variable becomes greater than zero.
.PP
.B for
is built into the C language.
Note that it has braces, just like
.B main()
does; these braces
mean that the following lines, up to the next right brace (\fB}\fR)
are part of this loop.
You can set conditions that control how a
.B for
loop operates; in its present form, it will loop forever.
This will be explained in more detail shortly.
.PP
Two library functions are executed within the loop.
The first,
.ID
	fgets(string, 128, fileptr);
.IE
.II fgets()
reads a line from the file named in the
.B fileptr
variable, and writes it into the character array called
.BR string .
The middle argument ensures that no more than 128 characters will be
read at a time.
The second line within this loop,
.ID
	printf("%s", string);
.IE
.II printf()
prints the line.
.B printf()
is a powerful and subtle function; in its present form,
it prints on the screen the string contained in the variable
.IR string .
.PP
Finally, the line at the top of the program:
.ID
	#include <stdio.h>
.IE
.II #include
tells the C preprocessor \fBcpp\fR to read the
.I "header file"
.II "header file"
called
.BR stdio.h .
.II stdio.h
The term \*(QLSTDIO\*(QR stands for \*(QLstandard input and output\*(QR;
.B stdio.h
declares and defines a number of routines that will
be used to read data from a file and write them onto the screen.
.PP
When you have finished typing in this code, again compile the program
as you did earlier.
If an error occurs, check what you have typed and make sure that it
\fIexactly\fR matches the code shown on the previous page.
If you find any errors, fix them and then recompile.
If errors persist, check it in the table of error messages that appear
at the end of this tutorial.
.PP
When compilation is finished, execute \fBdisplay\fR as you did earlier.
You will see the text from
.B display.c
scroll across the screen.
When the text is finished, however, the \*(CO prompt does not return;
you have not yet inserted code that tells the program to recognize that
the file is finished.
Type
.B <ctrl-C>
to break the program and return to \*(CO.
.Sh "Accepting File Names"
.PP
Of course, you will want \fBdisplay\fR to be able to display the
contents of any file, not just files named
.BR display.c .
The next step is to add code that lets you pass arguments to the
program through its command line.
This task requires that you give the
.B main()
function two arguments.
By tradition, these are always called
.B argc
and
.BR argv .
How they work will be described in a moment.
.II argc
.II argv
.PP
The enhanced program appears as follows.
You should change or insert the lines that are shaded:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
.ID
#include <stdio.h>
.SL
#define MAXCHAR 128
.IE
.ID
.SL
main(argc, argv)
.SL
/* Declare arguments to main() */
.SL
int argc;
.SL
char *argv[];
{
.SL
	char string[MAXCHAR];
	FILE *fileptr;
.IE
.ID
/* Open file */
.SL
	fileptr = fopen(argv[1], "r");
.IE
.ID
/* Read material and display it */
	for (;;) {
.SL
		fgets(string, MAXCHAR, fileptr);
		printf("%s", string);
	}
}
.IE
First, a small change has been added:
the line
.ID
	#define MAXCHAR 128
.IE
.II #define
defines the
.I "manifest constant"
.B MAXCHAR
to be equivalent to 128.
This is done because the \*(QLmagic number\*(QR 128 is used throughout
the program.
If you decide to change the number of characters
that this program can handle at once, all you would have to do is
to change this one line to alter the entire program.
This cuts down on mistakes in altering and updating the program.
If you look lower in the program, you will see that the declaration
.ID
	char string[128]
.IE
has been changed to read
.ID
	char string[MAXCHAR]
.IE
The two forms are equivalent;
the only difference is that the latter is easier to use.
It is a good idea to use manifest constants wherever possible,
to streamline changes to your program.
.PP
Now, look at the line that declares
.BR main() .
You will see that
.B main()
now has two arguments:
.B argc
and
.BR argv .
.PP
The first is an
.BR int ,
.II int
or integer, as shown by its declaration \(em \fBint argc;\fR.
.B argc
gives the
.I number
of entries typed on a command line.
For example, when you typed
.ID
	display filename
.IE
the value of \fBargc\fR was set to two:
one for the command name itself, and one for the file-name argument.
.B argc
and its value are set by the compiler.
You do not have to do anything to ensure that this value is set correctly.
.PP
.BR argv ,
on the other hand,
is an array of pointers to the command line's arguments.
In this instance, \fBargv[1]\fR points to
name of the file that you want \fBdisplay\fR to read.
This, too, is set by \*(PN, and works automatically.
.PP
.II fopen()
If you look below at the line that declares
.BR fopen() ,
you will see that
.B display.c
has been replaced with
.BR argv[1] ;
this means that you want
.B fopen()
to open the file named in the first argument to the \fBdisplay\fR command.
.PP
Now, try running the program by typing
.ID
	display display.c
.IE
Be sure that you give the command only one file name as an argument,
no more and no less.
Code that checks against errors has not yet been inserted,
and handing it the wrong number of arguments could cause problems for you.
.PP
.B display
will open
.B display.c
and print its contents on the screen.
You still need to type \fB<ctrl-C>\fR when printing is finished;
the code to recognize the end of the file will be inserted later.
.Sh "Error Checking"
.PP
Obviously, the program runs at this stage, but is still fragile,
and could cause problems.
The next step is to stabilize the program by writing code to check for errors.
To do so, a programmer must first write code to capture error conditions,
and then write a routine to react appropriately to an error.
.PP
Our edited program now appears as follows:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
.ID
#include <stdio.h>
#define MAXCHAR 128
.IE
.ID
main(argc, argv)
/* define arguments to main() */
int argc;
char *argv[];
{
	char string[MAXCHAR];
	FILE *fileptr;
.IE
.ID
.SL
/* Check if right number of arguments was passed */
.SL
	if (argc != 2)
.SL
		error("Usage: display filename");
.IE
.ID
/* Open file */
.SL
	if ((fileptr = fopen(argv[1], "r")) == NULL)
.SL
		error("Cannot open file");
.IE
.ID
/* Read material and display it */
	for (;;) {
		fgets(string, MAXCHAR, fileptr);
		printf("%s", string);
	}
}
.IE
.ID
.SL
/* Process error messages */
.SL
error(message)
.SL
char *message;
.SL
{
.SL
	printf("%s", message);
.SL
	exit(1);
.SL
}
.IE
The additions to the program are introduced by comments.
.PP
The first addition
.ID
	if (argc != 2)
		error("Usage:  display filename");
.IE
checks to see if the correct number of arguments was passed on the command line;
that is to say, it checks to make sure that you named a file
when you typed the \fBdisplay\fR command.
.PP
As noted above,
.B argc
is the number of arguments on the command line, or rather, the
number of arguments plus one, because the command name itself is
always considered to be an argument.
The statement \fBif (argc != 2)\fR checks this.
The
.B if
statement is built into C.
If the condition defined between its parentheses is true, then do
something, but if it is not true, do nothing at all.
The operator
.B !=
means \*(QLdoes not equal\*(QR.
Therefore, our statement means that if \fBargc\fR is not
equal to two (in other words, if there are not two and only two
arguments to the \fBdisplay\fR command \(em the command name itself plus
a file name), execute the function
.BR error .
.B error
is defined below.
.PP
Our
.B fopen()
function also has some error checking added (which will be
described in a moment):
.ID
	if ((fileptr = fopen(argv[1], "r")) == NULL)
		error("Cannot open file");
.IE
.II fopen()
.B fopen()
returns a value called \*(QLNULL\*(QR
.II NULL
if, for any reason, it cannot open the file you requested.
Thus, our new
.B if
statement says that if
.B fopen()
cannot open the file named on the command line
(that is,
.BR argv[1] ),
it should invoke the
.B error()
function.
.PP
C always executes nested functions from the \*(QLinside out\*(QR.
That means that the innermost function (that is, the function that
is enclosed most deeply within the pairs of parentheses) is executed first.
Its result, or what it \fIreturns\fR, is then passed to next
outermost function as an argument; that function is then executed
and what it returns is, in turn, passed to the function that
encloses it, and so on.
In this instance, the innermost function is
.ID
	fileptr = fopen(argv[1], "r")
.IE
.B fopen()
is executed and what it returns is written into
.BR fileptr .
What \fBfopen\fR returned is then passed to the next outer
operation; in this case, it is compared with NULL, as follows:
.ID
	(fileptr = fopen(argv[1], "r")) == NULL)
.IE
What that operation returns is then passed to the outermost function,
in this case the \fBif\fR statement, which evaluates what it is
passed, and acts accordingly.
If
.B fileptr
is NULL (that is, if \fBfopen\fR couldn't open the file),
the \fBif\fR statement will be true and the \fBerror\fR function called.
If, however, the file was opened, \fBfileptr\fR
will not equal NULL and the program will proceed.
.PP
As this example shows, C allows a programmer to nest
functions quite deeply.
Although nested functions are sometimes difficult to untangle when
you read them, they make programming much more convenient.
.PP
Finally, at the bottom of the file is a new function, called
.BR error() :
.ID
	error(message)
	char *message;
	{
		printf("%s", message);
		exit(1);
	}
.IE
This function stands outside of
.BR main() ,
as you can tell because it appears outside of
.BR main() 's
closing brace.
This function is called only when your program needs it.
If there are no errors, the program progresses only
until the closing brace in \fBmain\fR
and the \fBerror\fR function is never called.
.PP
.B error()
takes one argument, the message that is to be printed on the screen.
This message is defined by the routine that calls
.BR error() .
.B error()
uses the function
.B printf()
to print the message, then calls the
.B exit()
.II exit()
function; this, as its name implies, causes the program to stop.
The argument
.B 1
is a special signal that tells \*(CO that something went wrong with
your program.
.PP
When the error checking code is inserted, recompile the program
and execute it without an argument.
Previously, this would cause the program to crash;
now, all it does is print the message
.ID
	Usage: display filename
.IE
and terminate the program.
.Sh "Print a Portion of a File"
.PP
So far, our utility just opens a file and streams its contents over
the screen.
Now, you must insert code to print a 23-line portion of the file.
At present, it will only print the first 23 lines, and then exit.
.PP
To do so, you must insert another
.B for
loop.
.II for
Unlike our first loop, which ran forever, this one will cycle only
23 times, and then stop.
Our updated program appears as follows:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
.ID
#include <stdio.h>
#define MAXCHAR 128
.IE
.ID
main(argc, argv)
int argc;
char *argv[];
{
	char string[MAXCHAR];
	FILE *fileptr;
.SL
	int ctr;
.IE
.ID
/* Check if right number of arguments was passed */
	if (argc != 2)
		error("Usage: display filename");
.IE
.ID
/* Open file */
	if ((fileptr = fopen(argv[1], "r")) == NULL)
		error("Cannot open file");
.IE
.ID
.SL
/* Output 23 lines */
	for (;;) {
.SL
		for (ctr = 0; ctr < 23; ctr++) {
			fgets(string, MAXCHAR, fileptr);
			printf("%s", string);
.SL
		}
.SL
		exit(0);
	}
}
.IE
.ID
/* Process error messages */
error(message)
char *message;
{
	printf("%s", message);
	exit(1);
}
.IE
The new
.B for
loop is nested inside the loop governed by \fBfor(;;)\fR.
The program also declares a new variable,
.BR ctr ,
at the beginning of the program.
\fBctr\fR keeps track of how many times the loop has executed.
Now, look at the line:
.ID
	for (ctr = 0; ctr < 23; ctr++)
.IE
.II for
It has three sub-statements, which are separated by semicolons.
The first sub-statement sets
.B ctr
to zero;
the second says that execution is to continue as long as
.B ctr
is less than 23; and the third says that
.B ctr
is to be increased by one every time the loop executes
(this is indicated by the
.B ++
appended to
.BR ctr ).
With each iteration of this loop, \fBfgets()\fR reads
a line from the file named on the \fBdisplay\fR command line,
and \fBprintf()\fR prints it on the screen.
.PP
Also, an
.B exit()
call has been set after this new loop.
This ensures that the program
will exit automatically after the loop has finished executing.
This is a temporary measure, to make sure that you no longer have
to type \fB<ctrl-C>\fR to return to the shell.
.PP
When you have updated the program, recompile it in the usual way.
When you run it with an appropriate file of an appropriate length, e.g.,
.B display.c
itself,
.B display
will show the first 23 lines of the file, and then
the shell's prompt will return.
.PP
The program is now approaching its final form.
.Sh "Checking for the End of File"
.PP
The next-to-last step in preparing the program is teaching it to
recognize the end of a file when it sees it.
This does not appear to be needed now because the program exits
automatically after 23 lines or fewer, but it will be quite necessary
when the program begins to display more than one 23-line portion of text.
.PP
The function
.B fgets()
checks to see if it has arrived at the end of a file, and returns a
special value if it has.
.B fgets()
normally returns the address of the string into which it writes its output;
however, if it runs into the end of a file (or if any other error occurs),
it returns the special value NULL.
By reading the value of what \fBfgets()\fR returns,
.B display
can detect if the end of the file has been encountered, and stop reading.
To do so, the \fBfgets()\fR statement must be set within an \fBif\fR
statement.
The
.B if
statement will capture what
.B fgets()
returns, and continue execution as long as the value of the number
returned is not NULL.
.PP
The updated program now appears as follows:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
.ID
#include <stdio.h>
#define MAXCHAR 128
.IE
.ID
main(argc, argv)
int argc;
char *argv[];
{
	char string[MAXCHAR];
	FILE *fileptr;
	int ctr;
.IE
.ID
/* Check if right number of arguments was passed */
	if (argc != 2)
		error("Usage: display filename");
.IE
.ID
/* Open file */
	if ((fileptr = fopen(argv[1], "r")) == NULL)
		error("Cannot open file");
.IE
.ID
.SL
/* Output 23 lines, while checking for EOF */
	for (;;) {
		for (ctr = 0; ctr < 23; ctr++) {
.SL
			if (fgets(string, MAXCHAR, fileptr) != NULL)
				printf("%s", string);
.SL
			else
.SL
				exit(0);
		}
		exit(0);
	}
}
.IE
.ID
/* Process error messages */
error(message)
char *message;
{
	printf("%s", message);
	exit(1);
}
.IE
First, note that the comment that describes the program's output
has been changed to reflect our changes to the program.
It is important for a programmer
to ensure that the comments and the code are in step with each other.
.PP
Our new
.B if
statement
.ID
	if (fgets(string, MAXCHAR, fileptr) != NULL)
.IE
checks what
.B fgets()
returns:
if it does not return NULL, the end of the file has not been reached,
the \fBif\fR statement is true and the program prints out the next line.
(The operator \fB!=\fR indicates \*(QLnot equal\*(QR.)
If it returns NULL, however, the end of file has been
reached, the \fBif\fR statement is false so the \fBelse\fR statement
is executed, which causes \fBdisplay\fR to exit.
.PP
Note, too, that a new control statement is introduced:
.BR else .
.II else
This, like
.BR if ,
is built into the C language.
An
.B else
statement is always paired with an
.B if
statement; together, they mean that if the condition for which
.B if
is testing is true, the program should do one thing; otherwise, it
should do something else.
In this case, the program says that if the end of file has not been
reached, another line has been read from the file and should be printed on
the screen; however, if it has been reached, then the program should exit.
As you can imagine,
.B if/else
pairs are common in C programming; they are logical and useful.
.PP
One more task must be done on our program; then it is finished.
.Sh "Polling the Keyboard"
.PP
For the program to be complete, it has to ask you if you want to
see another 23-line portion of text whenever the argument contains
more than 23 lines.
The program should write another portion if you press the \fB<return>\fR key
alone; if you type any other key before you press \fB<return>\fR, then it
should exit.
.PP
To do so, we will print a query on the screen, then read what the
user has typed and interpret it.
When these changes are inserted, the program is complete:
.ID
/*
 * Truncated version of the 'scat' utility.
 * Open a file, print out 23 lines, wait.
 * If user types <return>, print another 23 lines.
 * If user types any other key, exit.
 * Exit when EOF is read.
 */
.IE
.ID
#include <stdio.h>
#define MAXCHAR 128
.IE
.ID
main(argc, argv)
int argc;
char *argv[];
{
	char string[MAXCHAR];
	FILE *fileptr;
	int ctr;
.IE
.ID
/* Check if right number of arguments was passed */
	if (argc != 2)
		error("Usage: display filename");
.IE
.ID
/* Open file */
	if ((fileptr = fopen(argv[1], "r")) == NULL)
		error("Cannot open file");
.IE
.ID
/* Output 23 lines, while checking for EOF */
	for (;;) {
		for (ctr = 0; ctr < 23; ctr++) {
			if (fgets(string, MAXCHAR, fileptr) != NULL)
				printf("%s", string);
			else
				exit(0);
		}
.SL
/* Query if user wishes to continue */
.SL
		printf("Continue? ");
.SL
		fflush(stdout);
.SL
		fgets(string, MAXCHAR, stdin);
.IE
.ID
.SL
		if (string[0] != '\en')
			exit(0);
	}
}
.IE
.ID
/* Process error messages */
error(message)
char *message;
{
	printf("%s", message);
	exit(1);
}
.IE
These new lines introduce a few new twists.
The lines
.ID
	printf("Continue? ");
	fflush(stdout);
.IE
print the prompt \fBContinue?\fR on the screen.
Note that no \*(Ql\en\*(Qr appears after the prompt; this ensures that
the cursor does \fInot\fR jump to the next line, but stays next to the prompt.
Because no \*(Ql\en\*(Qr appears after the line, however, you have to
force it to appear on the screen; this is accomplished with the statement:
.ID
	fflush(stdout);
.IE
.B fflush()
flushes matter to an output device.
.B stdout
points to a file stream, just like the stream that you opened with the call to
.BR fopen() ,
earlier in the program.
.B stdout
is opened in the header file \fBstdio.h\fR, which was read at the
beginning of the program; it always points to the user's screen.
.PP
The next line reads the user's keyboard:
.ID
	fgets(string, MAXCHAR, stdin);
.IE
This version of \fBfgets\fR reads matter into our array \fBstring\fR;
however, instead of reading the file pointed to by \fBfileptr\fR, it reads
what is pointed to by \fBstdin\fR.
.B stdin
is a stream that is also defined in \fBstdio.h\fR; it always points to the
user's keyboard.
.PP
Finally, the statement
.ID
	if (string[0] != '\en')
.IE
checks what the user typed by reading the first (that is, the zero-th)
character written in the array \fBstring\fR by the preceding call to
\fBfgets()\fR.
(Note that with C, counting always begins with zero rather than one.)
If the user just types \fB<return>\fR, then \fBstring[0]\fR will hold
\*(Ql\en\*(Qr; and the \fBif\fR statement will \fInot\fR be true,
the program jumps to the preceding \fBfor\fR statement, and more text
is written to the screen.
However, if the user types anything before typing \fB<return>\fR, the
\fBif\fR statement will succeed and the program will exit.
This may seem a little convoluted, but it actually is a straightforward
and efficient way to receive information from the user.
.PP
After you have inserted these changes, again compile the program.
.PP
When compilation is finished, try typing
.ID
	display display.c
.IE
The first 23 lines of the source code to the program
now appear on your screen.
Hit \fB<return>\fR; the next 23 lines appear.
Now, type any other key, and then press \fB<return>\fR:
the program exits.
.PP
You now have a simple but helpful
.B display
utility.
.Sh "For More Information"
.PP
This section has given you a brief, concentrated introduction
to writing a C program.
If you are new to programming, much of what happened must seem strange,
but we hope it helped you to appreciate
the logic of how C works.
.PP
Numerous books are on the market to teach beginners how to program in C;
the following section gives a small bibliography of books on C.
Also, look at the sample C programs in the Lexicon.
These demonstrate how to use many of the functions available
to you with \*(CO.
