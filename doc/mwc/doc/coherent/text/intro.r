.ds TL "Introduction"
.NH "Introduction" 1
.PP
.II "COHERENT^description"
\*(CO is a professional operating system designed for use on machines
that can run \*(MD.
It has many of the features and functionality of the \*(UN
operating system, but is the creation of Mark Williams Company.
\*(CO gives your
computer multi-tasking, multi-user capabilities without the tremendous
overhead, both in hardware and money, required by current
editions of UNIX.
\*(CO is what \*(UN used to be:
a well-designed system with selected tools and well-designed
utilities that bring out the best in modest computer systems.
.PP
The \*(CO system consists of the following:
.IP \(bu 0.3i
A fully multi-tasking, multi-user kernel.
.IP \(bu
Choice of Bourne or Korn shells.
.IP \(bu
The Mark Williams C compiler, linker,
assembler, archiver, and other tools.
.IP \(bu
A suite of commands, including editors, languages, tools, and utilities.
.IP \(bu
Drivers for peripheral devices, including terminals;
ASCII, PostScript, and PCL printers; dumb serial cards; and tape backup.
.IP \(bu 
Libraries,
including the standard C library, the mathematics library, and libraries for
.B curses
and socket emulation.
.IP \(bu
Numerous tools, utilities, and games.
.PP
For a list of some third-party programs that you can run under \*(CO,
see the release notes that accompany this manual.
New programs are released regularly, so consult the Mark Williams Bulletin
Board for the latest information.
.SH "What Is COHERENT?"
.PP
\*(CO is a multiuser, multitasking operating system.
.I Multiuser
means that with \*(CO, more than one person can use your computer
at any given time.
.I Multitasking
means that with \*(CO, any user can run more than one program at any
given time.
The design of \*(CO employs a few elegant concepts
to give you a powerful and flexible system that is easy to use.
.Sh "What is an Operating System?"
.PP
An \fIoperating system\fR is the master program that controls the
operation of all other programs.
It loads programs into memory, controls their execution, and controls
a program's access to peripheral devices, such as printers, modems, and
terminals.
.PP
Some operating systems (e.g., \*(MD)
permit only one user to use the computer at a time;
and that user can run only one program at a time.
However, you may well
want your computer to support more than one user at a time, and run more than
one program simultaneously.
Sharing not only yields many economies (such as allowing a group of
users to share one printer), but also allows the users to communicate with each
other and so work together more efficiently.
.PP
Any multitasking operating system must be able to do the following tasks
efficiently:
.IP \(bu 0.3i
Schedule computer time
.IS \(bu
Control mass-storage devices (disks and tape drives)
.IS \(bu
Organize disk-storage space
.IS \(bu
Protect programs from conflict
.IS \(bu
Protect stored information from destruction
.IS \(bu
Ease cooperation among users
.PP
Today's operating systems also provide \fItools\fR.
These are programs that are bundled with the operating system, and that
are designed to help you do your work more efficiently.
For example, you need editors, compilers, debuggers, and assemblers
to develop and test programs.
Text formatters and spelling checkers
help you write memoranda, manuals, or books.
Command processors (also called \fIshells\fR)
help you run the computer easily.
Status checkers tell you what programs are being run, who is using
the system, and how much space is left on your disk.
.PP
The combination of operating system and its
tools transforms a boxful of wires and circuits into a useful machine.
.Sh "Design Philosophy"
.PP
A computer system is not an end in itself; rather,
it is a ``bench'' for constructing tools to solve specific problems.
If the operating system is too specialized or limited, the range of
problems it can help you solve will be narrow.
On the other hand, if the operating system is too detailed,
then it becomes complex, idiosyncratic, and potentially unreliable.
.PP
The following quotation from John Conway summarizes well the
philosophy that underlies the design of the \*(CO system:
.II "Conway, John"
.QS
.I
The engineer who wants a machine for some
specific purpose will normally approve the simplest machine that does the job.
He will not usually prefer a multiplicity of parts with the same effect,
nor will he countenance the insertion of components with no function.
.R
.QE
.II "device-independent I/O"
The \*(CO system follows this approach throughout.
In brief, \*(CO is what \*(UN used to be:
an efficient system of selected tools and well-designed utilities, that
brings out the best in your computer system.
.SH "Installation"
.PP
The release notes that come with \*(CO describe how to install
\*(CO.
The release notes also list hardware that is known to work with \*(CO,
and hardware that is known \fInot\fR to work with \*(CO.
Before you begin to install \*(CO on your system, be sure to check
those lists and make sure that your system is compatible with \*(CO.
.PP
Please note that Mark Williams Company tries to keep these lists up to date,
but it is not possible to keep pace with the continual introduction of new
machines and new models.
If you do not find your machine on either list, the odds are that \*(CO
will work correctly with it.
.SH "User Registration and Reaction Report"
.PP
.II "user reaction report"
.II "manual^user reaction report"
Before you continue, fill out the User Registration Card that came with
your copy of \*(CO.
When you return this card, you become eligible for direct telephone support
from the \*(MW technical staff, and you will
automatically receive information about all new releases and updates.
.PP
If you have comments or reactions to the \*(CO software or documentation,
please fill out and mail the User Reaction Report included at the end of the
manual.
We especially wish to know if you found errors in this manual.
\*(MW needs your comments to continue to improve \*(CO.
.SH "Technical Support"
.PP
\*(MW provides free technical support to all registered users of \*(CO.
If you are experiencing difficulties with \*(CO, outside the area of
programming errors, feel free to contact the Mark Williams
Technical Support Staff.
You can telephone,
send electronic mail, or write.
Please note that this support is available \fIonly\fR if you have returned
your User Registration Card for \*(CO.
.PP
.\" RTFM!
Before you contact Mark Williams Technical Support with your problem,
\fIplease check this manual first.\fR
If you do not find an article in the Lexicon that addresses your problem,
be sure to check the index at the back of the manual.
Often, the information that you want is kept in an article that you didn't
consider, and the index will point you to it.
.PP
Another good way to find a topic in the manual is to use the command
.BR apropos ,
which is part of the \*(CO system.
.B apropos
finds every article in the Lexicon that mentions a given term or phrase.
For details on how to use this command, see its entry in the Lexicon.
.PP
If the manual does not solve your problem \(em or if you find it to be
misleading or difficult to understand \(em then
Mark Williams Technical Support is available to help you.
You can reach Technical Support via any number of routes:
.IP "\fBElectronic Mail\fR"
If you have access to the Internet, send mail to \fBsupport@mwc.com\fR.
This is the preferred means of communication.
Be sure to include your surface address and telephone number
as well as your e-mail address,
so we can contact you even if return electronic mail fails.
.IP "\fBFAX\fR"
Send your technical FAXes to 1-708-291-6750.
.IP "\fBSurface Mail\fR"
Write to Technical Support, Mark Williams Company, 60 Revere Drive,
Northbrook, IL 60062.
.IP "\fBTelephone\fR"
To contact Technical Support via telephone,
call 1-708-291-6700, between 9 AM and 5 PM, Central Time.
Please have at hand your manual for \*(CO,
as well as your serial number and version number.
Please collect as much information as you can concerning your difficulty
before you call.
If possible, call while you are at your machine, so the technical support
person can walk you through your problem.
.Sh "Help Us Help You"
.PP
Mark Williams Technical Support wants to help you fix your problem as
quickly as possible, so you can enjoy your \*(CO system.
.\" and get the hell off our back.
You can help us to help you by doing the following:
.PP
Before you contact Technical Support, 
\fIwrite down\fR
as carefully as possible what you did that triggered the problem.
Copy down exactly any error messages that appeared on the screen.
.PP
If the problem is triggered by a script or program, try to edit
the script or program to the chunk of code that triggers the problem.
The smaller the chunk of code, the better.
.PP
In your message, please include the following information:
.IP \(bu 0.3i
The make of your computer, and the type and clock speed of its
microprocessor.
.IP \(bu
The amount of RAM that you have.
.IP \(bu
The size and make of your hard disk, and the make of its controller.
.IP \(bu
If the problem affects the video display, include the make of your
display (i.e., tube) and controller card.
.PP
If you have found an error in the manual, please mention the page on
which the error occurs.
.PP
This information will help us to to clear up your problem as quickly
as possible.
.SH "How To Use This Manual"
.II "manual^how to use"
.PP
\*(CO encompasses an entire world of computing.
Before you learn the signposts of this world, you may find it difficult
to perform even simple tasks; and you may feel confusion and frustration.
.PP
\*(CO, however, more than anything else, consists of tools; and
this manual is one of the most useful tools that your \*(CO system has.
It is designed to guide you through the \*(CO system, to answer your
questions, and to lead you into areas of the system that may never have
explored on your own.
If you can learn to use it effectively, you will both lower the amount of
frustration you will have to endure,
and increase your productivity.
.PP
Beginners, in particular, should look over this manual carefully.
The following sub-sections describe the steps a beginner should take
when she begins to work with this manual.
.Sh "Elementary Tutorials"
.PP
The following tutorials teach the essentials of \*(CO:
they teach both essential information and essential skills.
Every beginner should work through these tutorials:
.IP \(bu 0.3i
First, read the tutorial \fIUsing the COHERENT System\fR.
This tutorial will introduce the \*(CO system and its tools.
It also teaches you such important tasks as how to shut the system
down properly (hint \(em \fInever\fR just shutting the machine off!)
and how to boot it again.
.IP \(bu
Then look at the subsequent tutorial,
\fIIntroducing sh, the Bourne Shell\fR.
The shell is the program through which you give commands to your \*(CO
system; and it incorporates a powerful programming language of its own.
Some of this information will seem obscure to you; but the as with
any language, the more you use the shell's programming language the more
quickly will you acquire fluency.
.IP \(bu
You should then look into learning how to use a text editor under \*(CO.
You will need to use a text editor in order to write scripts, programs,
and documents, which is the heart of using any information-processing
tool like \*(CO.
.IP
\*(CO offers two screen-oriented text editors:
MicroEMACS, and
.BR vi .
The manual has a tutorial on MicroEMACS.
You will find it to be fairly easy to learn how to perform simple text
editing with MicroEMACS.
.Sh "Advanced Tutorials"
.PP
The above tutorials will teach you the rudiments of how to use \*(CO.
The next set of tutorials introduce some of \*(CO's tools and languages.
These tutorials are rather specialized.
A beginner should look only at the ones that interest her:
.IP "\fIIntroduction to the ed Line Editor\fR"
.B ed
is an editor from the early days of \*(UN.
It is line oriented, which means that you edit text by typing commands
rather than by moving a cursor around the screen.
.B ed
is powerful and useful tool, but is of limited appeal to most beginners.
.IP "\fIIntroduction to the sed Stream Editor\fR"
.B sed
is another line-oriented editor.
However, it differs from
.B ed
in that it works non-interactively:
you write a program in the
.B sed
language, then filter one or more files through the program.
.B sed
is very useful if you wish to process large amounts of text quickly
and in an automated manner, but is of limited appear to most beginners.
.IP "\fIThe C Language\fR"
This is a primer on the C language, for persons who have never programmed
before.
C is the native language of \*(CO and \*(UN.
If you are interested in learning something about C, you should look at
this tutorial.
.IP "\fIIntroduction to the awk Language\fR"
.B awk
is a general-purpose for processing text.
With
.BR awk ,
you can process both ordinary text and tables; thus, you can quickly
implement simple data-base programs and other useful tools.
If you are at all interested in processing text or data under \*(CO,
you should look at this tutorial.
.IP "\fIIntroduction to lex, the Lexicon Analyzer\fR"
.B lex
is a tool with which you can generate programs to analyze text lexically.
You write a set of rules for
.BR lex ,
and it generates a program that you can compile and run.
.B lex
is a very useful tool for programmers who have to perform sophisticated
analysis of text.
For many beginners, however, it is of limited appeal.
.IP "\fIIntroduction to yacc\fR"
.B yacc
is another tool that with which you can generate programs.
It is often used with
.B lex
to build sophisticated tools, such as compilers.
If you are interested in building such tools, or if the problem
of parsing text interests you, you will find this tutorial to be helpful.
.IP "\fIbc Desk Calculator Language\fR"
.B bc
is a language in which you can write programs to perform calculations.
The numbers you calculate can be of infinite size and precision
(or as infinite as your system's memory allows).
You can perform calculations ``on the fly,'' as with a desk calculator;
or you can write programs that you store on disk and run repeatedly.
If number-crunching interests you, you should find this tutorial helpful.
.IP "\fIIntroduction to the m4 Macro Processor\fR"
.B m4
is a macro processor.
A
.I macro
is a sign or symbol whose interpretation can be deferred until needed.
For example, a writer may embed a macro in a letter in place of the
name of the person to whom the letter is addressed, and replace the
macro with the name only when the letter is printed.
This permits her to use the same letter over and over again, with the
computer replacing the macro with a different name.
Most \*(CO tools can process macros to a limited extent.
.BR m4 ,
however, will teach you about macro processing, and let you write
your own macro-process programs.
.IP "\fIThe make Programming Discipline\fR"
.B make
is a utility that builds things out of other things.
This tool is absolutely essential to anyone who writes programs
or generates reports.
.IP "\fInroff, The Text-Formatting Language\fR"
.B nroff
is a text-formatting language.
With it (and its related program
.BR troff ),
you can format letters, documents, or even entire books.
For example, this manual was formatted with \*(CO
.BR troff .
.IP "\fIUUCP, Remote Communications Utility\fR"
One of the most attractive features of \*(CO or \*(UN is its ability
to communicate with other computers without needing the assistance of
a human operator.
\*(UU is a set of programs with which you can exchange electronic mail
and files with other \*(CO and \*(UN systems.
With \*(UU (and some help from other computer systems) you can even tie into
the Internet, and participate in the global computer network that is
being expanded every day.
This tutorial will introduce you to \*(UU, and help you get \*(UU up and
running on your system.
.Sh "The Lexicon"
.PP
The bulk of this manual consists of the Lexicon.
This is a set of more than 1,000 articles, arranged in alphabetical order.
Each article discusses one aspect of the \*(CO system:
a command; a library function; a system file; or a general discussion of
a technical topic, such as how to hook up a terminal to your system.
.PP
At first glance, the Lexicon looks like a rag-bag of material
that is in no particular order.
However, this is not true:
it actually has a carefully designed internal structure.
Once you learn this structure, you can use it both
to help you look up a specific item of information quickly,
or take a guided tour through some aspect of the \*(CO system
that you otherwise might never have explored.
.PP
Internally, the Lexicon has a \fItree structure\fR.
Just as the roots and branches of a tree grow from a central trunk,
dividing and subdividing as they progress, so too the articles of the
Lexicon grow from one central article, and divide into ever more detailed
discussions as they go along.
.PP
The central article is the one called
.BR COHERENT .
It gives an overview of the \*(CO system \(em its design philosophy,
how it relates to other operating systems, and its internal structure.
This article introduces (or ``branches into'') the following
three ``overview'' articles:
.IP "\fBRunning COHERENT\fR"
This article introduces other articles that describe things
you can do with \*(CO:
the commands and tools that an ordinary user would use in the course of
her daily work.
.IP "\fBProgramming COHERENT\fR"
This articles introduces other articles that describe how to write
programs for \*(CO.
These articles describe, among other things, the \*(CO C compiler,
and the the libraries and header files included with \*(CO.
.IP "\fBAdministering COHERENT\fR"
This article introduces articles of interest to perons who administer
a \*(CO system.
These articles cover such topics as how to set up mail and \*(UU
on your system; the ``magic'' files that \*(CO uses to manage itself;
and the \*(CO kernel and its device drivers.
.PP
For example, consider that you are a programmer who wants to learn
if \*(CO has a library function that compares two strings.
You would turn first to the article called
.BR COHERENT ,
which would point you to the overview article called
.BR "Programming COHERENT" .
Looking in there, you see a reference to an article on
.BR libraries .
This, in turn, points to the article on
.BR libc ,
which is the standard C library.
Turning to this article, you find a section on string functions, which
has brief summaries of the functions
.BR memcmp() ,
.BR strcmp() ,
and
.BR strncmp() .
Each of these, in turn, is described in detail in its own Lexicon entry, which
By reading each entry, you can quickly find which function suits your purposes.
.PP
As you can see, the overview article briefly summarized the
articles that are available on a given topic.
If you want details, you can turn to the articles themselves \(em which you
can find easily because all of the articles printed in alphabetical order.
.PP
Another approach is to look directly for an article on the subject that
interests you.
For example, suppose you wanted to learn about \*(CO's mail system.
You could open the Lexicon and look for an article called
.BR mail ;
and just as you supposed, there it is.
.PP
If you're looking for a discussion of a specialized topic that does not
have its own article in the Lexicon, look in the Index, which is at the
back of the manual.
Often, you will find an entry that points to the information you want.
.PP
Finally, many users just like to open the Lexicon and leaf through it at
random.
Often, they discover nooks and crannies within the \*(CO system that they
never would have encountered otherwise.
.SH "Where To Go From Here"
.PP
The next step is to install \*(CO if you have not yet done so.
Fill out the user registration card at the back of this manual
and mail it to Mark Williams Company, so you will become eligible for
technical support.
If you are new to \*(UN or \*(CO, turn to the tutorial \fIUsing the
COHERENT System\fR; otherwise, you may wish to study a specialized tutorial
or begin to explore the Lexicon.
.PP
We hope that you enjoy using your \*(CO system!
