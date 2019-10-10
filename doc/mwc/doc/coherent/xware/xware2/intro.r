.NH "Xware 2: Games"
.PP
Congratulations on purchasing Xware 2, which is a package of X Games for \*(CO.
.PP
This package also holds the library
.BR libXpm.a ,
whose functions manipulate pix-mapped images;
a revised version of the \*(CO command
.BR make ;
some revised X header files;
and a manual page for every game.
.SH "What Is Xware?"
Xware is a series of packages from Mark Williams Company
that bundle some of the most popular and useful software
for the X Window System.
Each program in a Xware package comes with an executable binary;
source code;
instructions and scripts to help you recompile and install the program;
and a manual page for the program edited and formatted in the \*(CO
Lexicon format, which you can view with the \*(CO command
.BR man .
.PP
Other Xware packages are available or are being prepared.
These include packages of X window managers, graphics programs,
tools and utilities, and development tools.
Xware makes it easy and convenient for you
to enlarge your supply of X software.
.SH "System Resources"
.PP
The games in this package require eight megabytes of RAM.
.PP
When you install this package, it takes up about three megabytes.
You should assume that de-archiving and installing a game will
consume one-half to three-quarters of a megabyte.
The total amount of disk space consumed depends upon how many games you
install, and whether you leave the source archives on your system or
delete them.
Directions on how to install a game are given below.
.PP
The size of the screen required varies from game to game.
A few run on a 640\(mu480 desktop; most use 800\(mu600;
a few use 1024\(mu768.
Those that use the 1024\(mu768 desktop can, in most instances,
be run under 800\(mu600; however, you will have to shift its
window around to see all of the game's actions.
.PP
Most games recompile correctly with the \*(CO C compiler.
A few must be recompiled with the GCC compiler.
Each archive contains a file named
.BR README.Coh ,
which describes how to install the game and how to recompile it.
.SH "Contents of This Package"
.PP
The individual games are kept in the form of compressed archives;
when you install this package onto your system, all of the archives are
copied into directory
.BR /usr/X11/games .
The following describes each game included in this package:
.IP "\fBcbzone\fR"
Drive a tank around a desert battlefield.
Battle enemy tanks, supertanks, helicopters, cruise missiles, and
landers.
This game features striking animation.
It uses a 1024\(mu768 window, but you can run it on an 800\(mu600 screen.
.IP "\fBroids\fR"
This is an implementation of the arcade game ``Asteroids'':
You guide a spaceship through an asteroid field and blow up rocks.
.IP "\fBspider\fR"
This implements a sophisticated game of two-deck solitaire.
It uses a 1024\(mu768 window.
You can play it on an 800\(mu600 screen, but you will have to shift the
window from time to time to see where all the cards lie.
.IP "\fBsvb\fR"
Spy vs. Bob:
Guide your spy through a skyscraper, as he is pursued by the
evil, pipe-smoking Bobs.
.IP "\fBtetris\fR"
Yet another implementation of ``Tetris''.
This game features attractive graphics.
.IP "\fBxchomp\fR"
This implements a version of the arcade game ``PacMan'', but with
some twists \(em including a number of different mazes.
.IP "\fBxconq\fR"
Fight to dominate the world!
At the beginning of the game, you control a nation; as you guide your
nation's development, it encounters one or more other nations that are
controlled by the computer.
Each nation tries to dominate all of the others, through trade, alliances,
and warfare.
The game continues until one nation rules all.
The topography and technology can be imaginary, or modelled after a
historical setting.
.IP "\fBxhextris\fR"
Yet another implementation of ``Tetris,'' except that the shapes are
built out of hexagons instead of squares.
As a result, there are more shapes, and they are more difficult to fit
together.
Not for the timid!
.IP "\fBxlander\fR"
Your space ship is in orbit around the Moon.
You must guide it to a safe landing before it crashes or runs out of fuel.
.IP "\fBxlife\fR"
This implements Conway's game of Life under X.
Also included is a library of shapes and screens that you can patch together,
just to make Life interesting!
.IP "\fBxmahjongg\fR"
This game implements the solitaire version of the Chinese game Mah-Jongg.
It uses a 1024\(mu768 window; but
you can play it on an 800\(mu600 screen if you shift the window from time
to time to see where all the tiles lie.
.IP "\fBxminesweep\fR"
This is an X implementation of the board game ``minesweeper''.
You must deduce where the mines are hidden on an otherwise blank game board.
Guess wrong, and the mine goes off!
.IP "\fBxpipeman\fR"
The water levels are rising; and you must fit together randomly
selected sections of pipe to keep it from hitting the floor.
.IP "\fBlibXpm.a.z\fR"
This archive holds the sources for the library
.BR Xpm ,
which holds X pixmap (XPM) functions.
This archive
also contains the command
.BR sxpm ,
which demonstrates how to use and display an XPM.
.IP
Note that when you install this package onto your system,
the compiled archive
.B libXpm.a
is automatically copied into directory
.BR /usr/X11/lib .
.IP "\fBxrobots\fR"
This is an X implementation of the game ``Daleks,'' seen on the Atari ST
and other machines.
You are pursued by evil robots; and your only chance is to trick them
into crashing into each other.
.SH "Installing This Package"
.PP
To install this package onto your system, log in as the superuser
.B root
and type the installation command:
either
.DM
	/etc/install Xware2 /dev/fva0 3
.DE
if you have a 3.5-inch floppy-disk drive, or
.DM
	/etc/install Xware2 /dev/fha0 3
.DE
if your floppy-disk drive is 5.25 inches.
.B install
copies the programs into directory
.BR /usr/X11/games ,
installs the few extra tools you need to recompile these programs,
and automatically installs the manual pages for the programs.
.SH "Installing an Individual Program"
.PP
As described above, each program is kept in a compressed archive in directory
.BR /usr/X11/games .
Each archive contains a compiled version of the program, ready for running;
the sources for the program;
a file called
.BR README.Coh ,
which describes how to recompile the program, should you wish;
and a script named
.BR Install.Coh ,
which installs the program for you.
To install a program so you can play it, do the following:
.IP \(bu 0.3i
.B cd
to directory
.BR /usr/X11/games .
.IP \(bu
Use the command
.B gtar
to de-archive and uncompress the archive that holds the program you want.
For example, if you want to install program
.BR cbzone ,
type the command:
.DM
	gtar -xvzf cbzone.gtz
.DE
.IP \(bu
Change to the directory that holds the newly de-archived game.
For example, when you un-tar archive
.BR cbzone.gtz ,
the game and its source files are copied into directory
.BR cbzone .
.IP \(bu
Use the command
.B su
to become the superuser
.BR root ,
and execute script
.BR Install.Coh .
Each game has such a script, and it handles all the details of
installation for you.
.IP \(bu
After you have installed the game, you may wish to remove the source
code and other files that you do not need.
To do so,
.B cd
back to the game's parent directory, and use the command
.B "rm \-rf"
to remove the directory and its contents.
For example, after you have installed
.BR cbzone ,
you can remove this directory by typing the following commands:
.DM
	cd ..
	rm -rf cbzone
.DE
Note that the archive from which you copied the game,
.BR cbzone.gtz ,
is untouched by removing the directory
.BR cbzone .
You can always de-archive the game again should you wish to work with
its game files or re-install it.
.PP
If you wish to recompile a game, follow the directions given in file
.B README.Coh
in that game's source directory.
.SH "Copyrights"
.PP
Please note that the software in this package comes from third-party sources.
It is offered ``as is'', with no guarantee or warranty stated or implied,
whatsoever.
The copyrights on the source code and any executables built from it
belong to the persons who wrote the original code; full copyright
statements are included with the source code to each game.
Please read and respect these statements.
.SH "Technical Support"
.PP
The sources in this package have been compiled and run successfully under
\*(CO.
If you have a problem bringing up one of the games or with
compiling the source code as it is configured in this package, our
technical support team will attempt to help you.
However, because of the highly technical nature of the source
code in these packages, Mark Williams Company
.I cannot
give you technical assistance should you modify any of the source code
included in this package in any manner whatsoever.
.SH "A Final Word"
.PP
All of the source code for these games, and for all programs shipped in our
Xware packages, is available for free on the Mark Williams bulletin board
system, and on other publicly accessible archives.
The tutorial for
.B UUCP
in the \*(CO manual describes how to contact the BBS and make its services
available to you.
