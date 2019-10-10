.TL "Introduction"
.NH "Introduction to COHERENT X"
.PP
Congratulations on your purchase of X Windows for \*(CO!
.II MIT
This product is a port of XFree386 release 1.2,
which implements release 11, revision 5,
of the Massachusetts Institute of Technology's X Window System.
The X Window System (or ``X Windows'' for short)
lets you use your keyboard and mouse to open multiple windows and
run multiple graphics programs simultaneously on your screen.
With X Windows, you can use on your \*(CO system
the wealth of graphics-oriented programs \(em games, graphics-manipulation
programs, images, tools, and utilities \(em available at minimal cost from
many public sources.
.Sh "How to Use This Manual"
.PP
This manual consists of the following:
.IP \(bu 0.3i
An introduction \(em what you are reading right now.
.IP \(bu
An introduction to the X Window System and the window manager
.BR twm ,
including directions on how to customize X Windows and
.B twm
to suit your tastes.
.IP \(bu
An introduction to the X utilities and clients, and directions on how to
use resources to modify clients.
.IP \(bu
A discussion of how to recompile X programs.
This covers such topics as how to construct a
.B Makefile
from an
.BR Imakefile ,
and commonly encountered problems.
.IP \(bu
A Lexicon of manual pages for every
X utility and client included with X Windows for \*(CO.
Note that every Lexicon entry is also available on-line, and can be
read by using the \*(CO command
.BR man .
.PP
If you are experienced in using X,
you can jump to the Lexicon for a summary of the utility or client that
interests you.
The Lexicon entries
.B "X clients"
and
.B "X utilities"
list the clients and utilities included with X Windows for \*(CO.
If you are a beginner, however, we suggest that you read through
the tutorials at the beginning of this manual.
They will help you understand how X works, and so
help you to become productive more quickly.
.SH "System Requirements"
.PP
To run this X Windows for \*(CO, you need the following:
.IP \(bu 0.3i
A computer that is running \*(CO release 4.2 or later.
.IP \(bu
At least four megabytes of RAM.
At least eight megabytes is needed to run the color X server;
16 megabytes is preferred.
.IP \(bu
Twenty-five megabytes of hard-disk space on the file system that holds
directory
.BR /usr/X11 .
More is preferred.
.IP \(bu
A serial mouse \(em that is, a mouse that is plugged into a serial port
on your computer.
Note that X Windows does
.I not
work with a bus mouse \(em that is, a mouse that is plugged directly
into your system's mother board.
We recommend a three-button mouse, although two-button mice can also be used.
This implementation of X works with mice from Microsoft,
Mouse Systems, and Logitech (Mouseman), and with devices that mimic one of
the above, such as those manufactured by Honeywell.
.IP \(bu
A VGA or SVGA video board and monitor.
.SH "Contents of This Package"
.PP
X Windows for \*(CO includes all of the tools you need to
run X on your system.
It also includes tools with which you can import and write X applications.
The package includes the following:
.IP \(bu 0.3i
A large selection of X clients and utilities.
.IP \(bu
Object modules from which the included X programs are linked.
.IP \(bu
Bit-mapped images that can be displayed by an X client.
.IP \(bu
Sample and default configuration files for X programs.
.IP \(bu
A selection of fonts.
.IP \(bu
Header files used by X clients and utilities.
.IP \(bu
Libraries of X functions.
.PP
Please note that the included libraries do
not support a mathematics co-processor.
.SH "Bibliography"
.PP
The X Window System is a huge and elaborate system, for which a huge
literature exists.
This manual can only scratch the surface of X.
It walks you through installing, invoking, and configuring X Windows,
and introduces the tools and clients.
However, much of X lies beyond the scope of this manual, especially if you
wish to import or write X applications.
We recommend the following books for further reading.
.PP
.II "O\'Reilly & Associates"
If you are an experienced programmer and you wish to import or write your
own X Windows applications, there is no substitution for the first five
volumes of \fIThe X Window System,\fR published by O'Reilly & Associates, Inc.
Contact your local bookseller, or write to O'Reilly & Associates, Inc.,
632 Petaluma Avenue, Sebastopol, CA 95472 (1-800-338-6887).
Another useful volume is
\fIThe X Window System in a Nutshell,\fR second edition, edited by
Ellie Cutler, Daniel Gill, and Tim O'Reilly.
(Sebastopol, Calif., O'Reilly & Associates, Inc., 1992), which is
a handy pocket reference for people who already know X internals.
.PP
If you are new to the X Window System, we recommend the following two books:
.IP \(bu 0.3i
Mansfield, Niall:
\fIThe Joy of X:
An Overview of the X Window System.\fR
New York, Addison-Wesley Publishing Co., 1993.
.IP \(bu
Quercia, Valerie, O'Reilly, Tim:
\fIThe X Window System.\fR
Vol. 3,
\fIX Window System User's Guide,\fR ed 4.
Sebastopol, Calif., O'Reilly & Associates, Inc., 1992.
.SH "Technical Support"
.PP
\*(MW provides free technical support to all registered users of X Windows
for \*(CO.
If you are experiencing difficulties with this product,
feel free to contact the Mark Williams Technical Support Staff.
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
finds every article in the Lexicon that mentions a given word or phrase.
For details on how to use this command, see its entry in the \*(CO Lexicon.
.PP
If the manual does not solve your problem \(em or if you find it to be
misleading or difficult to understand \(em then
Mark Williams Technical Support is available to help you.
You can reach Technical Support via the following routes:
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
Please have your manual at hand.
Please collect as much information as you can concerning your difficulty
before you call.
If possible, call while you are at your machine, so the technical-support
person can walk you through your problem.
.Sh "Help Us Help You"
.PP
Mark Williams Technical Support wants to help you fix your problem as
quickly as possible.
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
The make of your computer, and the type and clock speed of its microprocessor.
.IP \(bu
The amount of RAM that you have.
.IP \(bu
The size and make of your hard disk, and the make of its controller.
.IP \(bu
The make of your display (i.e., tube) and controller card, and the amount
of video RAM it has, if known.
.PP
If you have found an error in the manual, please mention the page on
which the error occurs.
.PP
This information will help us to to clear up your problem as quickly
as possible.
.SH "Where To Go From Here"
.PP
If you have not yet done so, you should now install X Windows for
\*(CO onto your system.
For directions on how to do so, turn to the release notes that came with
this manual and follows the directions given there.
.PP
The release notes also give errata \(em that is, mistakes in this manual
that were discovered since its publication, and problems with
X Windows for \*(CO that remain unresolved.
We suggest that you note these in the appropriate places in this manual,
and we hope you will accept our apologies for any inconvenience they may have
have caused you.
.PP
After you have install X Windows for \*(CO, you should turn to the part
of the manual that is most appropriate for your level of experience:
.IP \(bu 0.3i
If you are a beginner, turn to the following tutorial, entitled
.IR "Using the X Window System" .
This walks you through X Windows, and teaches you how to use the window
manager
.BR twm .
.IP \(bu
If you are experienced with the X Window System, turn to
the Lexicon articles entitled
.B "X utilities"
and
.BR "X applications" .
These summarize the utilities and applications that are included with X.
.IP \(bu
If you are interested in porting code to X, turn to the
chapter entitled
.IR "Porting to X" .
This gives practical advice on how to import X code, and walks you through
the modification and compilation of a sample X application.
