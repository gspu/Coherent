.ds TL Errata
.ds TI "RELEASE NOTES"
.NH "Notes and Errata"
.PP
The following notes limitations that have been discovered with \*(CO.
It also notes errors that have been found in the \*(CO manual, and
describes extensions to the \*(CO Lexicon that have been since the manual
was printed (but are available in the on-line Lexicon).
Try as we might, no system the size and complexity of \*(CO will ever
be entirely free of errors.
We hope that you will note these problems in your manual, and that you will
accept our apologies for any inconvenience they may have caused you.
.SH "Known Limitations"
.PP
\*(CO 4.2 contains the following limitations:
.IP \(bu 0.3i
Program
.B uuconv
converts \*(UU files from the format used by \*(CO prior to release 4.0
into that used by Taylor \*(UU.
If you are installing \*(CO, or updating to release 4.2.14 from releases
4.0 or 4.2, you will not use this program.
However, if you are updating from a release of \*(CO prior to 4.0,
you will use
.BR uuconv ,
and you may run into some problems.
.B uuconv
makes a number of assumptions about the form and content of your files.
If those assumptions are not met, the program may write something
non-functional into one of the new \*(UU files.
Please note the following in particular:
.RS
.IP \(bu 0.3i
On occasion, times are converted without a day of week into the
.B sys
file.
.IP \(bu
Some commands that the old
.B Permissions
file allowed do not always make it into
.BR sys .
.RE
.IP
Be sure to read the generated files
.BR sys ,
.BR dial ,
and
.B port
in directory
.B /usr/lib/uucp
to make sure that all is correct.
For details, check the Lexicon entries for these files.
.RE
.IP \(bu
The command
.B uumvlog
manages the log files that are kept in directory /usr/spool/uucp/.Log.
However, it does not touch the files kept in directory
.BR /usr/spool/uucp/.Admin .
These files log all \*(UU connections to your system, and can grow quite large.
You must manage the files by hand.
