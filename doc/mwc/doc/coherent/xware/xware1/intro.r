.NH "Xware 1:  Window Managers"
.PP
Congratulations on purchasing Xware 1 Window managers package for \*(CO.
This package contains the X window managers
.BR olwm ,
which uses the \*(OL interface to X;
.BR olvwm ,
which is \*(OL plus a virtual desktop;
.BR fwm ,
a window manager whose appearance resembles that of Motif,
and that is derived from
.BR twm ;
and
.BR fvwm ,
which is
.B fwm
plus a virtual desktop.
.PP
Also included in this package are more than 150 X fonts that can be used
by these window managers, as well as by
.B twm
and other X applications; tools for rebuilding these programs; resource
files; and documentation.
.SH "What Is Xware?"
The Xware is a series of packages from Mark Williams Company
that bundle some of the most popular and useful software
for the X Window System.
Each program in an Xware package comes with executable binary;
source code;
instructions and scirpt to help you recompile and install the program;
and a manual page for the program edited and formatted in the \*(CO
Lexicon format, which you can view with the \*(CO command
.BR man .
.PP
Other Xware packages are available or are being prepared.
These include packages of X games, graphics programs, tools and utilities,
and a kit for developing X applications of your own.
Xware makes it easy and convenient to enlarge your supply of X software.
.SH "Contents of This Package"
.PP
This package contains source code for the window managers, executable binary
files, ancillary and resource files, fonts, and the tools you need to recompile
them.
(The only exception is that to recompile the window managers
.B fwm
and
.BR fvwm ,
you must have the GCC C compiler installed on your system.
You can purchase
a copy of this C compiler from Mark Williams Company,
or from any of a number of public-domain sources.
It is not included with this package.)
The following describes the contents in detail:
.PP
Executable binaries:
.DS
.B
	/usr/X11/bin/fvwm
	/usr/X11/bin/fwm
	/usr/X11/bin/olvwm
	/usr/X11/bin/olwm
.DE
Archives of source code:
.DS
.B
	/usr/X11/window_manage/fvwm.gtz
	/usr/X11/window_manage/xpm.gtz
	/usr/X11/window_manage/olwm.gtz
	/usr/X11/window_manage/olvwm.gtz
.DE
Archives of fonts:
.DS
.B
	/usr/X11/lib/fonts/font.char.tar
	/usr/X11/lib/fonts/font.cour.tar
	/usr/X11/lib/fonts/font.helv.tar
	/usr/X11/lib/fonts/font.lu.tar
	/usr/X11/lib/fonts/font.lub.tar
	/usr/X11/lib/fonts/font.lut.tar
	/usr/X11/lib/fonts/font.misc.tar
	/usr/X11/lib/fonts/font.ncen.tar
	/usr/X11/lib/fonts/font.symb.tar
	/usr/X11/lib/fonts/font.tim.tar
.DE
Bit-mapped images used by
.B fwm
and
.BR fvwm :
.DS
.B
	/usr/X11/include/X11/bitmaps/fvwm.bitmap
	/usr/X11/include/X11/bitmaps/fvwm.xpm
	/usr/X11/include/X11/bitmaps/default.xbm
.DE
Manual pages:
.DS
.B
	/usr/man/Win.managers/fvwm
	/usr/man/Win.managers/fwm
	/usr/man/Win.managers/olvwm
	/usr/man/Win.managers/olvwmrc
	/usr/man/Win.managers/olwm
	/usr/man/win.man.index
.DE
Libraries, defaults files, and resource files:
.DS
.B
	/usr/X11/lib/fvwm/system.fvwmrc
	/usr/X11/lib/fvwm/mwmlike.fvwmrc
	/usr/X11/lib/openwin-menu
	/usr/X11/lib/app-defaults
	/usr/X11/lib/libXpm.a
.DE
Finally, a revised version of
.B make
to help you recompile your programs.
.SH "System Resources"
.PP
To run this package, you need the following:
.IP \fB1.\fR 0.3i
A computer system capable of running X Windows for \*(CO, and that has
X Windows for \*(CO installed on it.
.IP \fB2.\fR
Approximately five megabytes of free disk space on the device that holds
directory
.BR /usr/X11 .
To see how much space is available, type the following commands:
.DM
	cd /usr/X11
	df .
.DE
\*(CO will display the disk space available, in blocks.
Because one block is 512 bytes of storage (one-half kilobyte), you must
have at least 10,000 blocks available before you can install this package.
.PP
If you do not yet have X Windows for \*(CO installed on your system, you
must install it first before you install this package.
Likewise, if the device that holds directory
.B /usr/X11
does not have at least five megabytes of space on it, you must remove or
compress files until sufficient space is available.
.PP
With regard to the amount of RAM needed,
the window managers vary greatly.
The following table compares sizes of these window managers with that of
.BR twm ,
the window managers included with X Windows for \*(CO:
.DS
.ta 0.5i 1.5i
	\fBtwm\fR	300 kilobytes
	\fBfwm\fR	220 kilobytes
	\fBfvwm\fR	289 kilobytes
	\fBolwm\fR	297 kilobytes
	\fBolvwm\fR	401 kilobytes
.DE
.B fvwm
and
.B olvwm
also use more memory for their virtual desktops.
Note, too, that
some applications may use more memory than before,
because more bit-mapped fonts will be available for them to pull into memory.
You should take the size of each window manager into account when you select
the one to use by default.
.SH "How to Install This Package"
.PP
Installation is very simple.
Log in as the superuser
.BR root .
If your drive A is 5.25 inches, then insert the first disk
of the package into drive A and type the command:
.DM
	/etc/install Xware1 /dev/fha0 3
.DE
Note the capital letter in ``Xware''.
.PP
If, however, your drive A is 3.5 inches, then insert the first disk
of the package into drive A and type the command:
.DM
	/etc/install Xware1 /dev/fva0 3
.DE
Again, note the capital letters in ``Xware''.
.PP
That's all there is to it.
.B install
will ask you a few questions, then place all of the appropriate files
automatically.
.SH "Invoking a Window Manager"
.PP
To invoke one of your new window managers, you must edit your
.B xinitrc
file \(em either
.B $HOME/.xinitrc
or
.BR /usr/X11/lib/xinit/xinitrc ,
depending upon how you have configured your system.
When you edit the file, you must comment out
.BR twm ,
and insert the name of the new window manager.
.PP
For example, assume that your
.B xinitrc
file reads as follows:
.DM
	sunclock -iconic &
	xterm  -geometry 80x25+26+23 -e hmon &
	xterm -ls -geometry 80x25+3+133 &
	twm
.DE
To use the window manager
.B fwm
instead of
.BR twm ,
edit this file to read as follows:
.DM
	sunclock -iconic &
	xterm  -geometry 80x25+26+23 -e hmon &
	xterm -ls -geometry 80x25+3+133 &
	#twm
	fwm
.DE
Then type
.B startx
to invoke X, as you usually do.
That's all there is to it.
.SH "Recompiling a Window Manager"
.PP
Source files for all four window managers are kept in directory
.BR /usr/X11/window_manage .
.Sh "Recompiling fvwm and fwm"
.PP
The window managers
.B fvwm
and
.B fwm
are both built from the same set of sources.
The following gives directions on how to build either.
.PP
Note that
.B fvwm
and
.B fwm
are written in ANSI C; therefore, you must have the GCC C compiler installed
on your system before you can recompile either.
.PP
Before you can use GCC to recompile
.B fvwm
or
.BR fwm ,
you must edit its
.B specs
file, and replace line
.DM
	%scrt1.o %scrtbegin.o -u _dtefg -L/lib/ndp -L/usr/lib/ndp
.DE
with:
.DM
	%scrt1.o %scrtbegin.o -u _dtefg -L/lib -L/usr/lib
.DE
This instructs GCC to link the program with the \*(CO libraries that do
.I not
use hardware floating-point processing.
After you have finished compiling the window manager, you should re-edit
.B specs
to return it to its original configuration.
.PP
Before you build either window manager, you must de-archive the sources.
To do so,
.B cd
to directory
.BR /usr/X11/window_manage ,
and then type the command:
.DM
	gtar -xvzf fvwm.gtz
.DE
When this is done,
.B cd
to directory
.BR fvwm .
.PP
To rebuild
.BR fwm ,
type:
.DM
	make -f Makefile.fwm
.DE
.PP
To build
.BR fvwm ,
type the command:
.DM
	make -f Makefile.fvwm
.DE
When your window manager is re-compiled, move it into directory
.BR /usr/X11/bin .
Then, edit your
.B xinitrc
as described above.
.Sh "Recompiling olwm"
.PP
.B olwm
compiles correctly with the default \*(CO compiler
.BR cc .
You do
.I not
need to use GCC, although you can use it if you wish.
(Note that if you do use GCC, you must modify its
.B specs
file as described above.)
.PP
To recompile
.BR olwm ,
do the following:
.IP \fB1.\fR 0.3i
.B cd
to directory
.BR /usr/X11/window_manage .
.IP \fB2.\fR
Type the following command to de-archive the sources:
.DM
	gtar -xvzf olwm.gtz
.DE
.IP \fB3.\fR
.B cd
to directory
.BR olwm/olgx .
.IP \fB4.\fR
Type
.BR make .
This rebuilds the \*(OL graphics library.
.IP \fB5.\fR
.B cd
to directory `..'.
.IP \fB6.\fR
Type
.BR make .
This rebuilds the application.
.PP
To install the package, copy the newly compiled file
.B olwm
into directory
.BR /usr/X11/bin .
Then, edit your
.B xinitrc
as described above, so that it invokes
.B olwm
instead of
.BR fwm .
.Sh "Recompiling olvwm"
.PP
.B olvwm
compiles correctly with the default \*(CO compiler
.BR cc .
You do
.I not
need to use GCC, although you can use it if you wish.
(Note that if you do use GCC, you must modify its
.B specs
file as described above.)
.B olvwm
also uses the tools
.B lex
and
.BR yacc ,
which are included as part of \*(CO.
.PP
To recompile
.BR olvwm ,
do the following:
.IP \fB1.\fR 0.3i
.B cd
to directory
.BR /usr/X11/window_manage .
.IP \fB2.\fR
Type the following command to de-archive the sources:
.DM
	tar xvzf olvwm.gtz
.DE
.IP \fB3.\fR
.B cd
to directory
.BR olvwm/olgx .
.IP \fB4.\fR
Type
.BR make .
This rebuilds the \*(OL graphics library.
.IP \fB5.\fR
.B cd
to directory `..'.
.IP \fB6.\fR
Type
.BR "make -f Makefile.Coh" .
This rebuilds the application.
.PP
To install the package, copy the newly compiled file
.B olvwm
into directory
.BR /usr/X11/bin .
Then, edit your
.B xinitrc
as described above, except that it should invoke
.B olvwm
instead of
.BR fwm .
.SH Fonts
This package installs a number of fonts that are required by the
window managers.
You will find that
.B twm
also looks more attractive with them.
.PP
The following describes the entire suite of fonts.
Note that all use the ISO Latin 1 character set (U.S. English) unless
otherwise marked:
.sp \n(pDu
.nf
.B /usr/X11/lib/fonts/75dpi:
.sp \n(pDu
.ta 1.5i
.tc . 0.1i
\fBcharB08.pcf.Z\fR	Bitstream Charter, bold Roman normal, 8 point
\fBcharB10.pcf.Z\fR	Bitstream Charter, bold Roman normal, 10 point
\fBcharB12.pcf.Z\fR	Bitstream Charter, bold Roman normal, 12 point
\fBcharB14.pcf.Z\fR	Bitstream Charter, bold Roman normal, 14 point
\fBcharB18.pcf.Z\fR	Bitstream Charter, bold Roman normal, 18 point
\fBcharB24.pcf.Z\fR	Bitstream Charter, bold Roman normal, 24 point
\fBcharBI08.pcf.Z\fR	Bitstream Charter, Bold Italic normal, 8 point
\fBcharBI10.pcf.Z\fR	Bitstream Charter, Bold Italic normal, 10 point
\fBcharBI12.pcf.Z\fR	Bitstream Charter, Bold Italic normal, 12 point
\fBcharBI14.pcf.Z\fR	Bitstream Charter, Bold Italic normal, 14 point
\fBcharBI18.pcf.Z\fR	Bitstream Charter, Bold Italic normal, 18 point
\fBcharBI24.pcf.Z\fR	Bitstream Charter, Bold Italic normal, 24 point
\fBcharI08.pcf.Z\fR	Bitstream Charter, Medium Italic normal, 8 point
\fBcharI10.pcf.Z\fR	Bitstream Charter, Medium Italic normal, 10 point
\fBcharI12.pcf.Z\fR	Bitstream Charter, Medium Italic normal, 12 point
\fBcharI14.pcf.Z\fR	Bitstream Charter, Medium Italic normal, 14 point
\fBcharI18.pcf.Z\fR	Bitstream Charter, Medium Italic normal, 18 point
\fBcharI24.pcf.Z\fR	Bitstream Charter, Medium Italic normal, 24 point
\fBcharR08.pcf.Z\fR	Bitstream Charter, Medium Roman normal, 8 point
\fBcharR10.pcf.Z\fR	Bitstream Charter, Medium Roman normal, 10 point
\fBcharR12.pcf.Z\fR	Bitstream Charter, Medium Roman normal, 12 point
\fBcharR14.pcf.Z\fR	Bitstream Charter, Medium Roman normal, 14 point
\fBcharR18.pcf.Z\fR	Bitstream Charter, Medium Roman normal, 18 point
\fBcharR24.pcf.Z\fR	Bitstream Charter, Medium Roman normal, 24 point
\fBcourB08.pcf.Z\fR	Adobe Courier, Bold Roman normal, 8 point
\fBcourB10.pcf.Z\fR	Adobe Courier, Bold Roman normal, 10 point
\fBcourB12.pcf.Z\fR	Adobe Courier, Bold Roman normal, 12 point
\fBcourB14.pcf.Z\fR	Adobe Courier, Bold Roman normal, 14 point
\fBcourB18.pcf.Z\fR	Adobe Courier, Bold Roman normal, 18 point
\fBcourB24.pcf.Z\fR	Adobe Courier, Bold Roman normal, 24 point
\fBcourBO08.pcf.Z\fR	Adobe Courier, Bold slated Roman normal, 8 point
\fBcourBO10.pcf.Z\fR	Adobe Courier, Bold slated Roman normal, 10 point
\fBcourBO12.pcf.Z\fR	Adobe Courier, Bold slated Roman normal, 12 point
\fBcourBO14.pcf.Z\fR	Adobe Courier, Bold slated Roman normal, 14 point
\fBcourBO18.pcf.Z\fR	Adobe Courier, Bold slated Roman normal, 18 point
\fBcourBO24.pcf.Z\fR	Adobe Courier, Bold slated Roman normal, 24 point
\fBcourO08.pcf.Z\fR	Adobe Courier, Medium slated Roman normal, 8 point
\fBcourO10.pcf.Z\fR	Adobe Courier, Medium slated Roman normal, 10 point
\fBcourO12.pcf.Z\fR	Adobe Courier, Medium slated Roman normal, 12 point
\fBcourO14.pcf.Z\fR	Adobe Courier, Medium slated Roman normal, 14 point
\fBcourO18.pcf.Z\fR	Adobe Courier, Medium slated Roman normal, 18 point
\fBcourO24.pcf.Z\fR	Adobe Courier, Medium slated Roman normal, 24 point
\fBcourR08.pcf.Z\fR	Adobe Courier, Medium Roman normal, 8 point
\fBcourR10.pcf.Z\fR	Adobe Courier, Medium Roman normal, 10 point
\fBcourR12.pcf.Z\fR	Adobe Courier, Medium Roman normal, 12 point
\fBcourR14.pcf.Z\fR	Adobe Courier, Medium Roman normal, 14 point
\fBcourR18.pcf.Z\fR	Adobe Courier, Medium Roman normal, 18 point
\fBcourR24.pcf.Z\fR	Adobe Courier, Medium Roman normal, 24 point
\fBhelvB08.pcf.Z\fR	Adobe Helvetica, Bold Roman normal, 8 point
\fBhelvB10.pcf.Z\fR	Adobe Helvetica, Bold Roman normal, 10 point
\fBhelvB12.pcf.Z\fR	Adobe Helvetica, Bold Roman normal, 12 point
\fBhelvB14.pcf.Z\fR	Adobe Helvetica, Bold Roman normal, 14 point
\fBhelvB18.pcf.Z\fR	Adobe Helvetica, Bold Roman normal, 18 point
\fBhelvB24.pcf.Z\fR	Adobe Helvetica, Bold Roman normal, 24 point
\fBhelvBO08.pcf.Z\fR	Adobe Helvetica, Bold slanted Roman normal, 8 point
\fBhelvBO10.pcf.Z\fR	Adobe Helvetica, Bold slanted Roman normal, 10 point
\fBhelvBO12.pcf.Z\fR	Adobe Helvetica, Bold slanted Roman normal, 12 point
\fBhelvBO14.pcf.Z\fR	Adobe Helvetica, Bold slanted Roman normal, 14 point
\fBhelvBO18.pcf.Z\fR	Adobe Helvetica, Bold slanted Roman normal, 18 point
\fBhelvBO24.pcf.Z\fR	Adobe Helvetica, Bold slanted Roman normal, 24 point
\fBhelvO08.pcf.Z\fR	Adobe Helvetica, Medium slanted Roman normal, 8 point
\fBhelvO10.pcf.Z\fR	Adobe Helvetica, Medium slanted Roman normal, 10 point
\fBhelvO12.pcf.Z\fR	Adobe Helvetica, Medium slanted Roman normal, 12 point
\fBhelvO14.pcf.Z\fR	Adobe Helvetica, Medium slanted Roman normal, 14 point
\fBhelvO18.pcf.Z\fR	Adobe Helvetica, Medium slanted Roman normal, 18 point
\fBhelvO24.pcf.Z\fR	Adobe Helvetica, Medium slanted Roman normal, 24 point
\fBhelvR08.pcf.Z\fR	Adobe Helvetica, Medium Roman normal, 8 point
\fBhelvR10.pcf.Z\fR	Adobe Helvetica, Medium Roman normal, 10 point
\fBhelvR12.pcf.Z\fR	Adobe Helvetica, Medium Roman normal, 12 point
\fBhelvR14.pcf.Z\fR	Adobe Helvetica, Medium Roman normal, 14 point
\fBhelvR18.pcf.Z\fR	Adobe Helvetica, Medium Roman normal, 18 point
\fBhelvR24.pcf.Z\fR	Adobe Helvetica, Medium Roman normal, 24 point
\fBluBIS08.pcf.Z\fR	B&H Lucida, Bold Italic normal sans serif, 8 point
\fBluBIS10.pcf.Z\fR	B&H Lucida, Bold Italic normal sans serif, 10 point
\fBluBIS12.pcf.Z\fR	B&H Lucida, Bold Italic normal sans serif, 12 point
\fBluBIS14.pcf.Z\fR	B&H Lucida, Bold Italic normal sans serif, 14 point
\fBluBIS18.pcf.Z\fR	B&H Lucida, Bold Italic normal sans serif, 18 point
\fBluBIS19.pcf.Z\fR	B&H Lucida, Bold Italic normal sans serif, 19 point
\fBluBIS24.pcf.Z\fR	B&H Lucida, Bold Italic normal sans serif, 24 point
\fBluBS08.pcf.Z\fR	B&H Lucida, Bold Roman normal sans serif, 8 point
\fBluBS10.pcf.Z\fR	B&H Lucida, Bold Roman normal sans serif, 10 point
\fBluBS12.pcf.Z\fR	B&H Lucida, Bold Roman normal sans serif, 12 point
\fBluBS14.pcf.Z\fR	B&H Lucida, Bold Roman normal sans serif, 14 point
\fBluBS18.pcf.Z\fR	B&H Lucida, Bold Roman normal sans serif, 18 point
\fBluBS19.pcf.Z\fR	B&H Lucida, Bold Roman normal sans serif, 19 point
\fBluBS24.pcf.Z\fR	B&H Lucida, Bold Roman normal sans serif, 24 point
\fBluIS08.pcf.Z\fR	B&H Lucida, Medium Italic normal sans serif, 8 point
\fBluIS10.pcf.Z\fR	B&H Lucida, Medium Italic normal sans serif, 10 point
\fBluIS12.pcf.Z\fR	B&H Lucida, Medium Italic normal sans serif, 12 point
\fBluIS14.pcf.Z\fR	B&H Lucida, Medium Italic normal sans serif, 14 point
\fBluIS18.pcf.Z\fR	B&H Lucida, Medium Italic normal sans serif, 18 point
\fBluIS19.pcf.Z\fR	B&H Lucida, Medium Italic normal sans serif, 19 point
\fBluIS24.pcf.Z\fR	B&H Lucida, Medium Italic normal sans serif, 24 point
\fBluRS08.pcf.Z\fR	B&H Lucida, Medium Roman normal sans serif, 8 point
\fBluRS10.pcf.Z\fR	B&H Lucida, Medium Roman normal sans serif, 10 point
\fBluRS12.pcf.Z\fR	B&H Lucida, Medium Roman normal sans serif, 12 point
\fBluRS14.pcf.Z\fR	B&H Lucida, Medium Roman normal sans serif, 14 point
\fBluRS18.pcf.Z\fR	B&H Lucida, Medium Roman normal sans serif, 18 point
\fBluRS19.pcf.Z\fR	B&H Lucida, Medium Roman normal sans serif, 19 point
\fBluRS24.pcf.Z\fR	B&H Lucida, Medium Roman normal sans serif, 24 point
\fBlubB08.pcf.Z\fR	B&H Lucida Bright, Demibold Roman normal, 8 point
\fBlubB10.pcf.Z\fR	B&H Lucida Bright, Demibold Roman normal, 10 point
\fBlubB12.pcf.Z\fR	B&H Lucida Bright, Demibold Roman normal, 12 point
\fBlubB14.pcf.Z\fR	B&H Lucida Bright, Demibold Roman normal, 14 point
\fBlubB18.pcf.Z\fR	B&H Lucida Bright, Demibold Roman normal, 18 point
\fBlubB19.pcf.Z\fR	B&H Lucida Bright, Demibold Roman normal, 19 point
\fBlubB24.pcf.Z\fR	B&H Lucida Bright, Demibold Roman normal, 24 point
\fBlubBI08.pcf.Z\fR	B&H Lucida Bright, Demibold Italic normal, 8 point
\fBlubBI10.pcf.Z\fR	B&H Lucida Bright, Demibold Italic normal, 10 point
\fBlubBI12.pcf.Z\fR	B&H Lucida Bright, Demibold Italic normal, 12 point
\fBlubBI14.pcf.Z\fR	B&H Lucida Bright, Demibold Italic normal, 14 point
\fBlubBI18.pcf.Z\fR	B&H Lucida Bright, Demibold Italic normal, 18 point
\fBlubBI19.pcf.Z\fR	B&H Lucida Bright, Demibold Italic normal, 19 point
\fBlubBI24.pcf.Z\fR	B&H Lucida Bright, Demibold Italic normal, 24 point
\fBlubI08.pcf.Z\fR	B&H Lucida Bright, Medium Italic normal, 8 point
\fBlubI10.pcf.Z\fR	B&H Lucida Bright, Medium Italic normal, 10 point
\fBlubI12.pcf.Z\fR	B&H Lucida Bright, Medium Italic normal, 12 point
\fBlubI14.pcf.Z\fR	B&H Lucida Bright, Medium Italic normal, 14 point
\fBlubI18.pcf.Z\fR	B&H Lucida Bright, Medium Italic normal, 18 point
\fBlubI19.pcf.Z\fR	B&H Lucida Bright, Medium Italic normal, 19 point
\fBlubI24.pcf.Z\fR	B&H Lucida Bright, Medium Italic normal, 24 point
\fBlubR08.pcf.Z\fR	B&H Lucida Bright, Medium Roman normal, 8 point
\fBlubR10.pcf.Z\fR	B&H Lucida Bright, Medium Roman normal, 10 point
\fBlubR12.pcf.Z\fR	B&H Lucida Bright, Medium Roman normal, 12 point
\fBlubR14.pcf.Z\fR	B&H Lucida Bright, Medium Roman normal, 14 point
\fBlubR18.pcf.Z\fR	B&H Lucida Bright, Medium Roman normal, 18 point
\fBlubR19.pcf.Z\fR	B&H Lucida Bright, Medium Roman normal, 19 point
\fBlubR24.pcf.Z\fR	B&H Lucida Bright, Medium Roman normal, 24 point
\fBlutBS08.pcf.Z\fR	B&H Lucida Typewriter, Bold Roman normal sans serif, 8 point
\fBlutBS10.pcf.Z\fR	B&H Lucida Typewriter, Bold Roman normal sans serif, 10 point
\fBlutBS12.pcf.Z\fR	B&H Lucida Typewriter, Bold Roman normal sans serif, 12 point
\fBlutBS14.pcf.Z\fR	B&H Lucida Typewriter, Bold Roman normal sans serif, 14 point
\fBlutBS18.pcf.Z\fR	B&H Lucida Typewriter, Bold Roman normal sans serif, 18 point
\fBlutBS19.pcf.Z\fR	B&H Lucida Typewriter, Bold Roman normal sans serif, 19 point
\fBlutBS24.pcf.Z\fR	B&H Lucida Typewriter, Bold Roman normal sans serif, 24 point
\fBlutRS08.pcf.Z\fR	B&H Lucida Typewriter, Medium Roman normal sans serif, 8 point
\fBlutRS10.pcf.Z\fR	B&H Lucida Typewriter, Medium Roman normal sans serif, 10 point
\fBlutRS12.pcf.Z\fR	B&H Lucida Typewriter, Medium Roman normal sans serif, 12 point
\fBlutRS14.pcf.Z\fR	B&H Lucida Typewriter, Medium Roman normal sans serif, 14 point
\fBlutRS18.pcf.Z\fR	B&H Lucida Typewriter, Medium Roman normal sans serif, 18 point
\fBlutRS19.pcf.Z\fR	B&H Lucida Typewriter, Medium Roman normal sans serif, 19 point
\fBlutRS24.pcf.Z\fR	B&H Lucida Typewriter, Medium Roman normal sans serif, 24 point
\fBncenB08.pcf.Z\fR	Adobe New Century Schoolbook, Bold Roman normal, 8 point
\fBncenB10.pcf.Z\fR	Adobe New Century Schoolbook, Bold Roman normal, 10 point
\fBncenB12.pcf.Z\fR	Adobe New Century Schoolbook, Bold Roman normal, 12 point
\fBncenB14.pcf.Z\fR	Adobe New Century Schoolbook, Bold Roman normal, 14 point
\fBncenB18.pcf.Z\fR	Adobe New Century Schoolbook, Bold Roman normal, 18 point
\fBncenB24.pcf.Z\fR	Adobe New Century Schoolbook, Bold Roman normal, 24 point
\fBncenBI08.pcf.Z\fR	Adobe New Century Schoolbook, Bold Italic normal, 8 point
\fBncenBI10.pcf.Z\fR	Adobe New Century Schoolbook, Bold Italic normal, 10 point
\fBncenBI12.pcf.Z\fR	Adobe New Century Schoolbook, Bold Italic normal, 12 point
\fBncenBI14.pcf.Z\fR	Adobe New Century Schoolbook, Bold Italic normal, 14 point
\fBncenBI18.pcf.Z\fR	Adobe New Century Schoolbook, Bold Italic normal, 18 point
\fBncenBI24.pcf.Z\fR	Adobe New Century Schoolbook, Bold Italic normal, 24 point
\fBncenI08.pcf.Z\fR	Adobe New Century Schoolbook, Medium Italic normal, 8 point
\fBncenI10.pcf.Z\fR	Adobe New Century Schoolbook, Medium Italic normal, 10 point
\fBncenI12.pcf.Z\fR	Adobe New Century Schoolbook, Medium Italic normal, 12 point
\fBncenI14.pcf.Z\fR	Adobe New Century Schoolbook, Medium Italic normal, 14 point
\fBncenI18.pcf.Z\fR	Adobe New Century Schoolbook, Medium Italic normal, 18 point
\fBncenI24.pcf.Z\fR	Adobe New Century Schoolbook, Medium Italic normal, 24 point
\fBncenR08.pcf.Z\fR	Adobe New Century Schoolbook, Medium Roman normal, 8 point
\fBncenR10.pcf.Z\fR	Adobe New Century Schoolbook, Medium Roman normal, 10 point
\fBncenR12.pcf.Z\fR	Adobe New Century Schoolbook, Medium Roman normal, 12 point
\fBncenR14.pcf.Z\fR	Adobe New Century Schoolbook, Medium Roman normal, 14 point
\fBncenR18.pcf.Z\fR	Adobe New Century Schoolbook, Medium Roman normal, 18 point
\fBncenR24.pcf.Z\fR	Adobe New Century Schoolbook, Medium Roman normal, 24 point
\fBsymb08.pcf.Z\fR	Adobe Symbol, Medium Roman normal, 8 point
\fBsymb10.pcf.Z\fR	Adobe Symbol, Medium Roman normal, 10 point
\fBsymb12.pcf.Z\fR	Adobe Symbol, Medium Roman normal, 12 point
\fBsymb14.pcf.Z\fR	Adobe Symbol, Medium Roman normal, 14 point
\fBsymb18.pcf.Z\fR	Adobe Symbol, Medium Roman normal, 18 point
\fBsymb24.pcf.Z\fR	Adobe Symbol, Medium Roman normal, 24 point
\fBtech14.pcf.Z\fR	DEC Terminal, Medium Roman normal, 14 point
\fBtechB14.pcf.Z\fR	DEC Terminal, Bold Roman normal, 14 point
\fBterm14.pcf.Z\fR	DEC Terminal, Medium Roman normal, 14 point
\fBtermB14.pcf.Z\fR	DEC Terminal, Bold Roman normal, 14 point
\fBtimB08.pcf.Z\fR	Adobe Times, Bold Roman normal, 8 point
\fBtimB10.pcf.Z\fR	Adobe Times, Bold Roman normal, 10 point
\fBtimB12.pcf.Z\fR	Adobe Times, Bold Roman normal, 12 point
\fBtimB14.pcf.Z\fR	Adobe Times, Bold Roman normal, 14 point
\fBtimB18.pcf.Z\fR	Adobe Times, Bold Roman normal, 18 point
\fBtimB24.pcf.Z\fR	Adobe Times, Bold Roman normal, 24 point
\fBtimBI08.pcf.Z\fR	Adobe Times, Bold Italic normal, 8 point
\fBtimBI10.pcf.Z\fR	Adobe Times, Bold Italic normal, 10 point
\fBtimBI12.pcf.Z\fR	Adobe Times, Bold Italic normal, 12 point
\fBtimBI14.pcf.Z\fR	Adobe Times, Bold Italic normal, 14 point
\fBtimBI18.pcf.Z\fR	Adobe Times, Bold Italic normal, 18 point
\fBtimBI24.pcf.Z\fR	Adobe Times, Bold Italic normal, 24 point
\fBtimI08.pcf.Z\fR	Adobe Times, Medium Italic normal, 8 point
\fBtimI10.pcf.Z\fR	Adobe Times, Medium Italic normal, 10 point
\fBtimI12.pcf.Z\fR	Adobe Times, Medium Italic normal, 12 point
\fBtimI14.pcf.Z\fR	Adobe Times, Medium Italic normal, 14 point
\fBtimI18.pcf.Z\fR	Adobe Times, Medium Italic normal, 18 point
\fBtimI24.pcf.Z\fR	Adobe Times, Medium Italic normal, 24 point
\fBtimR08.pcf.Z\fR	Adobe Times, Medium Roman normal, 8 point
\fBtimR10.pcf.Z\fR	Adobe Times, Medium Roman normal, 10 point
\fBtimR12.pcf.Z\fR	Adobe Times, Medium Roman normal, 12 point
\fBtimR14.pcf.Z\fR	Adobe Times, Medium Roman normal, 14 point
\fBtimR18.pcf.Z\fR	Adobe Times, Medium Roman normal, 18 point
\fBtimR24.pcf.Z\fR	Adobe Times, Medium Roman normal, 24 point
.sp \n(pDu
\fB/usr/X11/lib/fonts/misc:\fR
.sp \n(pDu
\fB10x20.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB12x24.pcf.Z\fR	Sony Fixed, Medium Roman normal
\fB5x7.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB5x8.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB6x10.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB6x12.pcf.Z\fR	Misc Fixed, Medium Roman semicondensed
\fB6x13.pcf.Z\fR	Misc Fixed, Medium Roman semicondensed
\fB6x13B.pcf.Z\fR	Misc Fixed, Bold Roman semicondensed
\fB6x9.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB7x13.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB7x13B.pcf.Z\fR	Misc Fixed, Bold Roman normal
\fB7x14.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB7x14B.pcf.Z\fR	Misc Fixed, Bold Roman normal
\fB7x14rk.pcf.Z\fR	Misc Fixed, Medium Roman normal (Japanese)
\fB8x13.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB8x13B.pcf.Z\fR	Misc Fixed, Bold Roman normal
\fB8x16.pcf.Z\fR	Sony Fixed, Medium Roman normal
\fB8x16rk.pcf.Z\fR	Sony Fixed, Medium Roman normal (Japanese)
\fB9x15.pcf.Z\fR	Misc Fixed, Medium Roman normal
\fB9x15B.pcf.Z\fR	Misc Fixed, Bold Roman normal
\fBclB6x10.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB6x12.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB8x10.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB8x12.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB8x13.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB8x14.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB8x16.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB8x8.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclB9x15.pcf.Z\fR	Schumacher Clean, Bold Roman normal
\fBclI6x12.pcf.Z\fR	Schumacher Clean, Medium Italic normal
\fBclI8x8.pcf.Z\fR	Schumacher Clean, Medium Italic normal
\fBclR4x6.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR5x10.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR5x6.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR5x8.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR6x10.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR6x12.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR6x13.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR6x6.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR6x8.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR7x10.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR7x12.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR7x14.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR7x8.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR8x10.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR8x12.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR8x13.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR8x14.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR8x16.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR8x8.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBclR9x15.pcf.Z\fR	Schumacher Clean, Medium Roman normal
\fBcursor.pcf.Z\fR	Cursor (cursors)
\fBdeccurs.pcf.Z\fR	DEC cursor (cursors)
\fBdecsess.pcf.Z\fR	DEC session
\fBheb6x13.pcf.Z\fR	Misc Fixed, Medium Roman semicondensed (Hebrew)
\fBheb8x13.pcf.Z\fR	Misc Fixed, Medium Roman normal (Hebrew)
\fBnil2.pcf.Z\fR	Misc nil, Medium Roman normal
\fBolcursor.pcf.Z\fR	Sun Open Look cursor (cursors)
\fBolgl10.pcf.Z\fR	Sun Open Look glyph, 10 point (glyphs)
\fBolgl12.pcf.Z\fR	Sun Open Look glyph, 12 point (glyphs)
\fBolgl14.pcf.Z\fR	Sun Open Look glyph, 14 point (glyphs)
\fBolgl19.pcf.Z\fR	Sun Open Look glyph, 19 point (glyphs)
.fi
.SH "Copyrights"
.PP
Please note that the software in this package comes from third-party
sources.
It is offered ``as is'', with no guarantee or warranty stated or implied,
whatsoever.
The copyrights on the source code and any executables built from it
belong to the persons who wrote the original code; full copyright
statements are included with the source code to each window manager.
We ask that you read and respect these statements before you modify or
redistribute any of the sources or binaries in these packages.
.SH "Technical Support"
.PP
The sources in this package have been compiled and run successfully under
\*(CO.
If you have a problem bringing up one of the window managers or with
compiling the source code as it is configured in this package, our
technical support team will attempt to help you.
However, because of the highly technical nature of the source
code in these packages, Mark Williams Company
.I cannot
give you technical assistance should you modify any of the source code
included in this package in any manner whatsoever.
.SH "A Final Word"
.PP
All of the source code for these games, and for most programs shipped in our
Xware packages, is available for free on the Mark Williams bulletin board
system, and on other publicly accessible archives.
The tutorial for
.B UUCP
in the \*(CO manual describes how to contact the BBS and make its services
available to you.
