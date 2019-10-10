.NH "COHERENT X Games Disk"
.PP
The following describes the disk of X games that comes with your copy of
the \*(CO operating system and the X Window System.
It describes how to install the disk, and introduces each game.
.SH "System Requirements"
.PP
Before you install your X Games, please note the following:
.IP \(bu 0.3i
You must first have installed X Windows for \*(CO on your system.
X Windows is required for these games to link and run.
.IP \(bu
You must have at least ten megabytes of disk space free on the file
system that holds directory
.BR /usr/X11 .
If you are not sure whether you have enough disk space,
.B cd
to directory
.B /usr/X11
and type the command:
.DM
	df .
.DE
\*(CO will display the amount of free space, in blocks:
one block equals 512 bytes, or 0.5 kilobyte.
(For details, see the Lexicon entry for
.BR df .)
You must have at least 20,000 blocks free before you can install your
games disk.
If you do not, you must free up space before you can install.
.IP \(bu
These games require that you have eight megabytes of RAM on your system.
If you wish,
you can install the games disk even if you do not have eight megabytes of RAM,
but you will not be able to play any of the games.
.IP \(bu
Most of the games require that your screen have at least 800\(mu600 resolution.
Four (\fBxconq\fR,
.BR spider ,
.BR xmahjongg ,
and
.BR cbzone )
use a 1024\(mu768 screen by default, but are playable in 800\(mu600 if you
are patient.
For details on how to set and change resolution on your screen, see the
manual for X Windows for \*(CO.
.SH "Installing the Games"
.PP
To install the games, place the disk into your A floppy-disk drive
(either 3.5 inch or 5.25 inch).
Then log in as the superuser
.BR root .
If you have a 3.5-inch disk, type the command:
.DM
	/etc/install CohXGames /dev/fva0 1
.DE
If, on the other hand, you have a 5.25-inch disk, type the command:
.DM
	/etc/install CohXGames /dev/fha0 1
.DE
The
.B install
program will copy all of the files onto your hard disk, then ask you
whether you have installed X Windows, whether you have ten megabytes
of disk space free, and whether you have at least eight megabytes of RAM.
If you answer ``No'' to any of these questions, it aborts the installation.
You can install again later when the problem has been cleared up.
.PP
After
.B install
asks you its three questions, it proceeds to de-archive and link the games.
This will take approximately 15 minutes; of course, the actual time taken
depends upon how fast your system is and how much other activity is on it.
Should a problem occur,
.B install
aborts with an error message that describes the problem, so you can fix
it and try again.
.SH "The Games"
.PP
This games disk includes the following 12 games:
.IP \fBcbzone\fR
You guide a tank around a desert battle zone, and fight alien invaders
who have tanks, helicopters, landers, and cruise missiles.
The game features excellent animation.
It uses a 1024\(mu768 screen, but is usable on a 800\(mu600 screen.
.IP \fBspider\fR
Play two-deck solitaire.
This is a game of thought and strategy, rather than luck.
This game uses a 1024\(mu768 screen, although you can play it in 800\(mu600
mode by shifting the window back and forth.
.IP \fBsvb\fR
Spy vs. Bob:
Try to maneuver your spy to the top of a tall building, while dodging the
evil, pipe-smoking Bobs.
.IP \fBxchomp\fR
Race your little man through a maze while he is chased by ghosts.
It resembles the popular arcade game PacMan, but uses more than one maze.
.IP \fBxconq\fR
This is a game of long-term strategy.
You struggle against your computer to control the world,
by exploring, developing your economy, and waging war.
.IP \fBxhextris\fR
Like Tetris, except that the blocks are six-sided and the shapes more
numerous.
A game for those who like a challenge.
.IP \fBxlander\fR
Land your spacecraft on the Moon before you run out of fuel or crash.
.IP \fBxlife\fR
Play the Game of Life.
You can draw a pattern on the screen, then animate it:
as the game progresses, you can watch your pattern ``evolve'',
grow, develop, and die.
The game has numerous features, and comes with a library of shapes
that you can patch into your patterns to make them more interesting.
.IP \fBxmahjongg\fR
An implementation of the Chinese tile game, in solitaire mode.
The design of the tiles is quite attractive, and the game also lets you
devise your own formations.
This uses a 1024\(mu768 window, although you can play in 800\(mu600 by
moving the window around the screen.
.IP \fBxmandel\fR
Explore the Mandelbrot set and Julia sets.
.IP \fBxminesweep\fR
An X implementation of the game ``minesweeper'':
Try to deduce where the mines are hidden on a blank game board.
.IP \fBxpipeman\fR
Connect randomly selected sections of pipe, while staying ahead of
the flow of liquid.
.PP
Each game is described in its own manual page, which was installed
automatically from the games disk.
To read these, use the command
.BR man ;
for example, to read the manual page for the game
.BR xpipeman ,
type the command:
.DM
	man xpipeman
.DE
To print this manual page on a PostScript printer, type the command:
.DM
	man xpipeman | prps | lp
.DE
.SH "Playing the Games"
.PP
All of the games are installed into directory
.BR /usr/X11/bin .
If this directory is in your
.BR PATH ,
then all you have to do to play a game
start up X, as described in the \*(CO X Windows
manual; then type the name of the game into your
.B xvt
or
.B xterm
window.
It's that simple.
If you do not have these directories in your
.BR PATH ,
then prefix the name of the game with the path name of the directory
in which the game resides.
For details on
.BR PATH ,
see its entry in the \*(CO Lexicon.
.PP
When you become more experienced with X Windows, you probably will
want to modify the menus for your window manager, so you can invoke a game
without having to use a terminal window.
For details on how to do this, see the chapter on customizing X Windows in
your X Windows manual.
.SH "More X Packages"
.PP
Other X Windows packages are available from Mark Williams Company, or are
in preparation; we call these
.BR XWare .
These include a package of games, a package of graphics programs, a
package of window managers and fonts, and a package of X utilities and tools.
Every
.B XWare
package includes executables and full source code for every program it
includes.
.PP
Also under preparation is an X Developers' Package.
This will include tools to help you build an X application, and documentation
for the Xlib and Xt libraries.
.PP
For details on these and other Mark Williams products, telephone Mark
Williams Company, or send e-mail to sales@mwc.com.
.PP
We hope you enjoy your X games \(em and that you enjoy using the \*(CO system!
