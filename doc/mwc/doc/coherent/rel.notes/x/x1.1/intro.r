.ds TI "RELEASE NOTES"
.ds TL "Introduction"
.NH "Introduction"
.PP
Thank you for selecting X Windows for \*(CO.
.PP
These notes describe the changes to this product from release 1.0.
They also describe how to install X Windows for \*(CO onto your
\*(CO system, and the additional packages of X software that are
available from Mark Williams and other vendors.
.PP
We suggest you read these notes carefully before you begin to install
X Windows for \*(CO:
otherwise, you may stumble over a problem for which a fix has already
been described.
.SH "Changes from Release 1.0"
.PP
This release corrects a number of minor problems with the
original release of X Windows for \*(CO, and includes some extra features.
We hope that these changes will enhance your enjoyment of this product.
.PP
The following describes changes to X Windows for \*(CO since release 1.0:
.IP \(bu 0.3i
The manual has been revised.
Typographical errors have been corrected, and technical descriptions
have been improved and made more thorough.
.IP \(bu
The installation script has been thoroughly rewritten.
The old script would, under certain conditions, write a configuration file
that did not work correctly; nor did it always react correctly to error
conditions.
The new script is more accurate and more robust.
.IP \(bu
The installation program now uses the new command
.BR findmouse .
This command, as its name implies, helps you to find the port into which
your mouse is plugged.
.IP \(bu
Command
.B bdftopcf
is now included.
The command generates a PCF bitmapped font from a BDF font-description file.
You can use this to help import fonts from third-party suppliers.
.IP \(bu
Script
.B mkdirhier
has been rewritten to work correctly.
.IP \(bu
Script
.B startx
has been rewritten.
It now refuses to start X from within X.
.IP \(bu
Header files
.BR Xos.h ,
.BR Xfuncs.h ,
and
.BR Xlib.h
have been corrected.
Previous versions contained errors that stopped the compilation of
some third-party X programs.
.IP \(bu
Configuration file
.B x386.cf
has been corrected.
The previous version contained an error that prevented a
.B Makefile
generated from an
.B Imakefile
from being compiled by the GNU C compiler.
.IP \(bu
The release now includes the source archive
.BR xwave.gtz ,
which contains source code for the X application
.BR xwave .
This is used as an example of how to recompile an X application
under X Windows for \*(CO.
.SH "Known Bugs"
.PP
X Windows for \*(CO contains the following bugs.
We are working to correct them, and corrected versions of the applications
in question will be posted as soon as they are available.
In the meantime, however, you should be warned of them.
.IP \(bu 0.3i
The X bit-map editor
.B bitmap
does not always work correctly.
In particular, it does not properly load a bit-mapped image.
Attempting to do so from within the application may lock up X Windows.
.IP \(bu
Resizing the X calculator
.B xcalc
may cause X to crash.
.B xcalc
does work correctly, however, if you use it as it first appears on
the screen.
.IP \(bu
The X server may leave garbage characters in the terminal's input queue when
it exits.
Type
.B <ctrl-U>
to erase them before you type the first command after you exit X.
.SH "Errors in the Manual"
.PP
As of this writing, no errors have been discovered in the manual.
.PP
If you discover an error, please send e-mail to manual@mwc.com.
Describe the error and the page on which it occurs.
Please be specific.
This will ensure that the error is corrected in the next printing of
the manual.
