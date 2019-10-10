.NH "Installing BBL Images for UNIX or COHERENT"
.PP
The following document describes how to install the Summa Bartholomei
package of images onto your \*(UN or \*(CO system.
If you have a \*(CO system, it also describes how to use the enclosed
macros and binding files to include the images in the output of
.BR troff .
.SH "Installing Summa Bartholomei"
.PP
The Summa Bartholomei package consists of files compressed by the GNU
utility
.BR gzip ,
and are stored on a
.B tar
archive.
To copy the files from the disk onto your system,
.B cd
to the directory in which you wish to store the images, then insert the
disk into your high-density, 3.5-inch disk drive and type the following
command:
.DM
	tar xvf /dev/\fIdevice\fR
.DE
where
.I device
is the name of the device from which you are reading the archive.
The files are stored in directory
.BR summa_bart ,
which is created in the current directory.
.PP
When the disk is copied onto your system,
.B cd
to the sub-directory
.BR summa_bart ,
then type the following command to un-compress the files:
.DM
	gzip \-d *.gz
.DE
That's all there is to it.
If you are using \*(UN, you should remove the file
.B tmac.eps
and the files
.BR *.bnd ,
as you cannot use them.
If you are using \*(CO, read on; the following will show you how to use
these files with \*(CO's implementation of
.BR troff .
.SH "Using BBL Images with COHERENT troff"
.PP
As noted above, in addition to the encapsulated PostScript images, this
package contains binding files and a set of macros that can be used with
the \*(CO operating system's implementation of
.BR troff .
They
.I cannot
be used with standard \*(UN
.B troff
as they employ some extensions to the
.B troff
language that are unique to the \*(CO implementation of
.BR troff .
.PP
To use the macros, \*(CO users log in as
.B root
or
.BR bin ,
then move the file
.B tmac.eps
into directory
.BR /usr/lib .
.PP
The macro package
.B tmac.eps
reads a binding file that gives the bounding-box for the image, and names
the file that holds the image itself.
(For details on what the bounding box is and how these macros use it,
see the comments at the head of
.BR tmac.eps .)
A binding file, by convention, has the suffix
.BR .bnd ;
for example, the image
.B cmz.eps
has the binding file
.BR cmz.bnd .
.PP
To patch an image into a
.B troff
document, you must do the following:
.IP \fB1.\fR 0.3i
Define the string
.BR XA ,
which names the base directory of the images.
.IP \fB2.\fR
Call the appropriate binding file, using one of the macros in
.BR tmac.eps .
.IP \fB3.\fR
Include
.B tmac.eps
on your
.B troff
command line.
.PP
For example, assume that you have installed the Summa Bartholomei
into directory
.BR /usr/bbl .
The following example, called
.BR test.eps ,
gives a brief program that prints the capital `Z' from this package:
.sp \n(pDu
.nf
.ft L
.vs 9p
	.ds XA /usr/bbl
	.SH
	An Example Image
	.PP
	This mass of text precedes the image.
	This mass of text precedes the image.
	This mass of text precedes the image.
	This mass of text precedes the image.
	This mass of text precedes the image.
	This mass of text precedes the image.
	This mass of text precedes the image
	This mass of text precedes the image.
	This mass of text precedes the image:
	.PH 1 1 /usr/bbl/summa_bart/sbz.bnd
	.PP
	This mass of text follows the image.
	This mass of text follows the image.
	This mass of text follows the image.
	This mass of text follows the image.
	This mass of text follows the image.
	This mass of text follows the image.
	This mass of text follows the image.
	This mass of text follows the image.
	This mass of text follows the image.
.vs 10p
.PP
As you can see, this example includes the binding file
.BR sbz.bnd ,
not the image file
.BR sbz.eps .
To compile and print this example, use the following command:
.DM
	troff -p -ms -meps test.eps | lp
.DE
Note that the \fB.eps\fR macros do not set up page; they only position
images.
You must use a standard
.B troff
macro package, such as \fB\-ms\fR or \fB-man\fR, to set up the page.
.PP
That all there is to it.
.SH "The tmac.eps Macros"
.PP
.B tmac.eps
includes the following macros:
.IP "\fB.PH\fR"
Picture Here \(em center a picture on the page at the point where this
macro is called.
It takes three arguments:
the image's X scaling factor;
the image's Y scaling factor;
and full path name of the image's binding file.
The macro ensures that enough space is on the page to print the
and repositions the text and the image appropriately so there is
no collision or overwriting.
If there's not enough space, the image
is bumped to the next page.
Example call:
.DM
	.PH 1 1 /usr/bbl/sb12.bnd
.DE
.IP \fB.PF\fR
Picture Fill \(em fill up the rest of the white space on the
current page with a picture.
It takes three arguments:
the image's X scaling factor;
the image's Y scaling factor;
and full path name of the image's binding file.
If the remaining white space on the page cannot hold the image, it
prints nothing.
Example call:
.DM
	.PF 1 1 /usr/bbl/sb12.bnd
.DE
.IP \fB.PL\fR
Picture Left \(em set an image flush left on the page at the point where this
macro is called.
It takes three arguments:
the image's X scaling factor;
the image's Y scaling factor;
and the full path name of the image's binding file.
Unlike the macros
.B .PF
and
.BR .PH ,
which clear clear out a horizontal band on the page,
.B .PL
sets a trap and indents properly, so text can flow around the image.
If the image will not fit on this page, do nothing.
Example call:
.DM
	.PL 1 1 /usr/bbl/sb12.bnd
.DE
.IP \fB.PB\fR
Picture Beginning \(em set an image flush left on the page at the point
where this macro is called.
Unlike
.BR .PL ,
the image does
.I not
have to have text appear before it on the page.
Use this macro for initial capitals and things like that.
It takes three arguments:
the image's X scaling factor;
the image's Y scaling factor;
and full path name of the image's binding file.
Example call:
.DM
	.PB 1 1 /usr/PostScript/bbl/sb12.bnd
.DE
.IP \fB.PR\fR
Picture Right \(em set an image flush right on the page at the point where this
macro is called.
It takes three arguments:
the image's X scaling factor;
the image's Y scaling factor;
and full path name of the image's binding file.
Example call:
.DM
	.PR 1 1 /usr/PostScript/bbl/sb12.bnd
.DE
.PP
We hope you enjoy using these macros.
