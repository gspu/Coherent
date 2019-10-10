.NH "Recompiling X Applications"
.PP
.II "X^recompile"
.II "compiling X applications"
.II "importing X applications"
A wealth of X source code is available to the public.
In all probability, somebody has already written the X program that you want.
By checking publically accessible archives, you will find many interesting,
useful, and amusing programs.
The following gives hints on how to recompile X source code under \*(CO.
.PP
In many cases, importing an X application to \*(CO is simply matter of
recompiling its source code.
This section describes how to do so.
It also discusses some problems that are commonly encountered
during recompilation.
.SH "Imakefiles and Makefiles"
.PP
.II make
.II Makefile
The \*(UN/\*(CO application
.B make
manages the building of programs.
It reads the contents of a
.BR Makefile ,
which describes how to build the program, and names the source modules
from which the program is created.
If you are not familiar with
.BR make ,
see tutorial for it that appears in your \*(CO manual.
.PP
Although
.B make
is the programmer's best friend, adapting a
.B Makefile
from one operating system to another can be difficult.
This is because different operating systems \(em even different
flavors of \*(UN or \*(UN-like systems like \*(CO \(em can vary
quite a bit in their capacities, in the functions available in their
libraries, and in how they have structured their header files and libraries.
.PP
.II Imakefile
.II xmkmf
.II imake
To help you avoid this problem, most X applications use the utility
.B imake
to build a
.BR Makefile .
.B imake
reads the contents of an
.B Imakefile
that is written by the programmer, invokes the C preprocessor
.B cpp
to combine the
.B Imakefile
with a set of configuration files designed for your operating system,
and writes a
.B Makefile
from which you can build the application.
In many instances, the
.B Imakefile
simply names the application you wish to build and the source files
from which it is built.
An example
.B Imakefile
is shown below.
.PP
The script
.B xmkmf
invokes
.B imake
with the arguments and configuration files that are appropriate for \*(CO.
In most instances, all you have to do to build a
.B Makefile
is type
.BR xmkmf ;
and once the
.B Makefile
is built, all you have to do to build the application is type
.BR make .
.PP
Naturally, some problems may arise during this process.
The commonest ones are discussed below.
.SH "Modifications to Makefiles"
.PP
Some applications come with a
.B Makefile
instead of an
.BR Imakefile .
If this is the case, you probably will need to modify this
.B Makefile
so that the program will compile correctly under \*(CO.
.PP
The following changes must be made to most
.BR Makefile s:
.IP \(bu 0.3i
X Windows for \*(CO keeps X header files in directory
.BR /usr/X11/include/X11 .
Most other releases of X keep their header files in directory
.BR /usr/include/X11 ;
therefore, you must add an instruction to the
.B Makefile
to tell it to look in the correct directory.
.IP
A
.B Makefile
must use the option -I to name this directory explicitly:
.DM
	-I/usr/X11/include
.DE
.IP
This tells the C compiler to look in
.B /usr/X11/include
for any header file whose name is prefixed with X11, such as:
.DM
	#include <X11/Xos.h>
.DE
Often, this is done as part of the macro
.BR CCFLAGS .
Therefore, if you see an instruction of the form:
.DM
	-I/usr/include/X11
.DE
.IP
change it to read:
.DM
	-I/usr/X11/include
.DE
.IP \(bu
X Windows for \*(CO keeps its X libraries in directory
.BR /usr/X11/lib .
A
.B Makefile
must name that directory explicitly.
Many
.BR Makefile s
use a macro named something like
.B LIBFLAGS
to set linking options and name libraries.
Before the list of libraries, you must add the option:
.DM
	-L/usr/X11/lib
.DE
.IP \(bu
Add
.B \-lXbsd
to the end of the list of libraries to be linked into the application.
This tells the linker to link in library
.BR libXbsd.a ,
which holds the socket-emulation routines.
.IP \(bu
If the option
.B \-lsocket
appears on the list of libraries, remove it.
This library holds the Berkeley socket functions; these are emulated in
.B libXbsd.a
and so are not needed.
.IP \(bu
Add the option
.B \-DCOHERENT
to the macro
.BR CCFLAGS .
If you make any \*(CO-specific changes to the source code, you can bracket
them with the preprocessor directives
.DM
	#ifdef COHERENT
	    ...
	#endif
.DE
.IP
and the instruction
.B \-DCOHERENT
will ensure that they appear in the compiled program.
.PP
Once you have made these changes, you can begin compiling.
.SH "Problems Seen During Compilation"
.PP
The following discusses problems that can come up during compilation.
.IP \(bu 0.3i
The \*(CO compiler is not ANSI compliant, although it recognizes some ANSI
extensions to the C language.
If the X application is written using ANSI-specific grammar (in particular,
function prototypes), you must compile it with an ANSI-compliant compiler,
e.g., GNU C.
When doing so, modify the
.B Makefile
or
.B Imakefile
to define the macro
.B CC
to
.BR gcc .
.IP \(bu
During the link phase, you may see the linker complain about the
undefined symbol
.BR select .
This indicates that one of the source modules has called the socket function
.BR select() .
Under X Windows for \*(CO, this function is named
.BR soselect() .
.IP
To fix this problem, go through the sources and conditionally replace every
instance of
.DM
	select( [arguments] );
.DE
.IP
with:
.DM
	#if defined(COHERENT)
		soselect ( [arguments] );
	#else
		select ( [arguments] );
	#endif
.DE
.IP
Note that this is the proper way to add new \*(CO-specific code to X sources.
If the application comes only with a
.BR Makefile ,
you must make sure the instruction
.B \-DCOHERENT
appears as part of the macro
.BR CFLAGS ;
however, this is not necessary if you have built a
.B Makefile
from an
.BR Imakefile ,
as
.B xmkmf
ensures that this instruction is included automatically.
.IP \(bu
The \*(CO C compiler does
.I not
include by default the code to print floating-point numbers,
as this code increases the size of the linked executable noticeably.
If your X application uses any of the options
.BR %e ,
.BR %f ,
or
.B %g
with the function
.BR printf() ,
and if you are compiling with \*(CO's
.B cc
command, you
.I must
add the option
.B \-f
to the macro
.BR CFLAGS .
Note that you must do this in either a
.B Makefile
or an
.BR Imakefile ,
as
.B xmkmf
does not add this option by default.
.\" got that?
.IP
You do not need to do this if you are compiling with GNU C.
.IP \(bu
Some applications redeclare the manifest constant
.BR PI ;
\*(CO declares this constant in header file
.BR <math.h> .
The \*(CO C compiler will abort if manifest constants are redeclared
(GNU C does not);
so if this problem arises,
conditionalize out the declaration of
.B PI
within the application, as shown above.
.IP \(bu
You may find problems with the following routines:
.BR bcopy() ,
.BR bcmp() ,
.BR bzero() ,
.BR index() ,
and
.BR rindex() .
The problems usually consist of a clash
between the manner in which these routines are declared or defined within
the application, and they way they are declared or defined in the \*(CO
header files.
In most instances, you should conditionalize out the
declaration within the application, and ensure that the proper \*(CO
header file is included.
For details, use the command
.B man
command to view the manual entries for the routine in question.
.IP \(bu
Some versions of \*(UN declare string-handling functions in the header file
.BR <strings.h> .
\*(CO, however, keeps them in
.BR <string.h> .
If the compiler complains that it cannot open
.BR strings.h ,
conditionally replace it in the sources with
.BR <string.h> ,
as described above.
.IP \(bu
If you are compiling with GNU C, the linker may complain of a number of
undefined symbols of the form
.BR _dmul .
This is due to the fact that GNU C always creates code that performs hardware
floating-point arithmetic and by default attempts to link in the \*(CO
libraries that contain hardware floating-point routines,
but the \*(CO X libraries were compiled
to use software floating-point arithmetic.
To get around this problem, you must modify the file
.B specs
for GNU C:
change the line that reads
.DM
	%scrt1.o %scrtbegin.o -u _dtefg -L/lib/ndp -L/usr/lib
.DE
to:
.DM
	%scrt1.o %scrtbegin.o -u _dtefg -L/lib -L/usr/lib
.DE
.IP
Note that this is a work-around for GNU C, and may create problems of its own.
For example, when you link software floating-point code with modules
compiled to use hardware floating-point arithmetic, the function
.B atof()
will always return
.BR NAN .
.IP
For a fuller description of how \*(CO manages floating-point arithmetic,
see the Lexicon entry for
.BR cc .
.PP
If other problems arise that are not described here, please send a
detailed description to MWC Technical Support, as described at the beginning
of this manual.
.SH "Recompiling an Example Application"
.PP
.II "Friedman, Mike"
.II "Riddle, Paul"
.II "McBeath, Jim"
The following walks you through the recompilation of the X application
.BR xwave .
This application, whose source code is included with X Windows for \*(CO,
was written by
Mike Friedman, Paul Riddle, and Jim McBeath.
It draws a three-dimensional animation of a wave.
By setting command-line parameters, you can dictate the size of the
grid used and the type of wave to be plotted.
The following steps describe how to recompile this program:
.IP \(bu 0.3i
.B cd
to directory
.BR /usr/X11/src .
.IP \(bu
De-archive the sources by typing the command:
.DM
	gtar -xvzf xwave.gtz
.DE
.IP \(bu
.B cd
to directory
.BR xwave .
.IP \(bu
Type the command
.BR xmkmf .
This builds a
.B Makefile
from the
.B Imakefile
included with this package.
.IP \(bu
To recomile, type
.BR make .
.IP \(bu
When compilation has finished, install the program as follows:
.DM
	mv xwave /usr/X11/bin
.DE
.IP \(bu
To test the program, invoke X as described in an earlier section;
then type
.BR xwave .
The application opens a window and animates a sample wave.
.PP
That's all there is to it.
A formatted manual page for
.B xwave
is included in directory
.BR /usr/X11/src/xwave/man .
.PP
The following gives the contents of
.BR xwave 's
.BR Imakefile :
.DM
	LOCAL_LIBRARIES = $(XLIB)
	OBJS = xwave.o force.o plot.o prop.o
	SRCS = xwave.c force.c plot.c prop.c
	SYS_LIBRARIES = -lm
	ComplexProgramTarget(xwave)
.DE
When
.B xmkmf
and
.B imake
process this five-line
.BR Imakefile ,
it expands in a 370-line
.BR Makefile .
The following describes each line in the
.BR Imakefile .
.PP
.B LOCAL_LIBRARIES
names the libraries to be linked into the program.
The macro
.B XLIB
means that the programmer wants to include the standard suite of
X libraries for your system.
The configuration files included with X Windows for \*(CO give the information
that
.B imake
needs to expand
.B XLIB
into the correct set of libraries.
.PP
.B OBJS
and
.B SRCS
name, respectively, the object modules from which the application is built,
and the source files from which those objects are compiled.
.PP
.B SYS_LIBRARIES
names the system libraries to be included, apart from the X libraries.
The programmer has included the argument
.BR \-lm ,
which indicates that the program needs routines within the mathematics library
.BR libm.a .
For details on the contents of this library, see its entry in the \*(CO Lexicon.
.PP
Finally, the line
.DM
	ComplexProgramTarget(xwave)
.DE
is a macro whose body is kept in file
.BR /usr/X11/lib/config/Imake.rules .
This macro contains all of the instructions and commands that
.B make
needs to build a complex X program.
The argument
.B xwave
names the application to be built.
.PP
For a discussion of the macros that can be included in an
.BR Imakefile ,
see the Lexicon entry for
.B imake
in this manual.
.SH "Building Your Own Makefile"
.PP
If you do not want to use
.B xmkmf
and
.BR imake ,
you can build a
.B Makefile
by hand.
You probably will never need to do this, but knowing how to do so
may come in handy some time:
.IP \fB1.\fR 0.3i
Build a skeletal
.BR Makefile .
Place the following declarations at its beginning:
.DM
	CFLAGS  = -I/usr/X11/include
	LIBS    = -L/usr/X11/lib -lX11 -lXbsd
.DE
.IP
Follow this with a declaration of the object modules from which this
application is built.
As a rule of thumb, there is one object for each source module.
For example, if the application consists of source modules
.BR foo.c ,
.BR bar.c ,
and
.BR baz.c ,
write the following into your
.BR Makefile :
.DM
	OBJS    = foo.o bar.o baz.o
.DE
.IP 
Finally, add the target line for the executable you wish to build.
If the application is to be called
.BR xapp ,
add the following to your skeletal
.BR Makefile :
.DM
	xapp: $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) $(LIBS)
.DE
.IP \fB2.\fR
Use the command
.DM
	cc -c -I/usr/X11/include/X11 \fIsource\fP.c
.DE
.IP
for each
.I source
module.
Some modules will compile correctly on the first try; others will
require several attempts.
Those that require several attempts may require that you use the
.B cc
option
.B \-D
to set one or more switches within the source module.
Use the command
.B grep
to find every instance with the source module of the C preprocessor directive
.BR #if .
These will indicate the options that are available within the source code,
and suggest which switches you should set.
.IP \fB3.\fR
.II nm
Once you have succeeded in compiling all of the source files to objects, type
.BR make .
Because all of the objects already exist,
.B make
will attempt to link an executable.
.IP
At this stage, you probably will see errors about undefined symbols.
Note which symbols are undefined; then use the
\*(CO utility
.B nm
to list the symbols in all of the X libraries and find which libraries
contains the symbols you need.
Include them in your
.BR Makefile .
.IP
.II libX11.a
.II libXbsd.a
Note that an X application always need the libraries
.B libX11.a
and
.BR libXbsd.a ;
the declarations given above invoke those libraries automatically.
The difficulty is in arranging the other libraries in the proper order
in the
.BR Makefile .
This can be done only by trial and error:
sometimes many trials and many errors.
When you have the correct order, copy the information into your
.BR Makefile .
.IP
Unresolved identifiers within the program may also be manifest constants
that the programs expects to be set with the
.B \-D
option to the
.B cc
command.
.PP
When you have finished linking the program, you should have a working
executable.
Thereafter, you can use the
.B Makefile
you just created to
rebuild the application, should you decide to modify the source code.
.PP
Most applications come with a manual page that describe the program and
how to run it.
You may, for example, need to install a font or a resource file in the
appropriate directory.
You will, of course, need to test the program to make sure that it runs
correctly.
We have found that, in most instances, if a program can be compiled and
linked under \*(CO, it will also run correctly.
.SH "Where To Get X Sources"
.PP
Numerous X programs have already been ported to \*(CO.
Archives of sources are available for free on the Mark Williams BBS.
For directions on how to contact the MWC BBS, see the directions in the
tutorial for UUCP that appears in the \*(CO manual.
.PP
If you have access to the Internet, you can retrieve source files from
the site
.BR raven.alaska.edu .
Use
.B ftp
to access that site; then log in as ``anonymous''.
New sources are added continually.
.PP
The master site for X software is
.BR ftp.x.org .
If you have access to the Internet, you can log into that system via
anonymous
.BR ftp .
This site contains many megabytes of sources, so you would be well
advised to think about what you would like to retrieve before you
enter this site.
.PP
Finally, Mark Williams Company sells packages of X applications
that have been ported to \*(CO.
Called
.BR Xware ,
these packages bring together interesting, useful, and amusing programs
for your \*(CO X system.
Each has been ported to \*(CO; most include full source code and a formatted
manual page that can be viewed with the \*(CO
.B man
command.
For details on
.BR Xware ,
see the release notes that come with this manual.
