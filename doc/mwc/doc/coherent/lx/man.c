.TH man "" "" Command
.PC "Display Lexicon entries"
\fBman [\-dw] [\fIpage ...\fR]
.PP
.HS
.SH Options:
.IC \fB\-d\fR
Dump a listing of all available manual pages
.IC \fB\-w\fR
Print only file name where document resides
.IC \fB\-V\fR
Print the version of
.B man
that you are running.
.HE
.B man
prints each manual
.I page
onto the standard output.
This normally is an entry from the \*(CO Lexicon, although it can
be a manual page from any other source as well.
.PP
When used with the option
.BR \-w ,
it prints the path name of the file instead of printing the document itself.
When used with option
.BR \-d ,
it dumps a list of all available manual pages to the standard output device,
for your perusal.
.PP
By default,
.B man
uses the pager
.B more
to display text.
To use another pager, e.g.,
.BR scat ,
define the environmental variable
.BR PAGER :
.DM
	export PAGER="/bin/scat"
.DE
.PP
.B man
normally searches for manual pages in the directory
.BR /usr/man .
.II MANPATH
.II "environmental variables^MANPATH"
However, if the environmental variable
.B MANPATH
is set,
.B man
searches for manual pages in each directory that it names.
.B MANPATH
must name one or more directories, with directories separated by a colon `:'.
.SH "Index Files"
To locate a manual page,
.B man
reads index files.
It assumes that every file
.B /usr/man/*.index
is an index file;
it then opens these files, and searches them
for the manual entry you have requested.
.PP
Prior to release 4.2, an index file consisted of entries that had the
format:
.DS
	\fIrelative-path-name article-name\fR
.DE
.PP
where
.I relative-path-name
gave the subdirectory and file in
.B /usr/man
that held the manual-page entry,
.I article_name
gave the name of the article as it appears in the Lexicon.
Beginning with release 4.2,
.B man
uses index entries of the form:
.DS
	\fIrelative-path-name article_name description\fR
.DE
.PP
.I description
gives a brief summary of the article.
Fields must be separated by one more white-space characters.
For example, entries
.DM
.ta 0.5i 2.0i 3.0i
	COHERENT1/bc	bc	Interactive calculator with arbitrary precision
	LOCAL/chess	chess	Interactive chess program
.DE
.PP
associate manual-page file
.B /usr/man/COHERENT1/bc
with the Lexicon entry for the command
.BR bc .
Likewise, rules for the user-written chess game
.B chess
are found in file
.BR /usr/man/LOCAL/chess .
.PP
.B man
can read index entries prepared in either the ``old'' or the ``new'' form.
We encourage you to use the new form, because this format also allows
the index entries to be used by the command
.BR apropos .
.SH "Adding Manual-Page Entries"
When writing new manual-page entries for \*(CO, we recommend
that you place them into a subdirectory of
.BR /usr/man .
This subdirectory should be uniquely named to avoid
possible name-space collisions.
A good rule of thumb is to name the subdirectory after the application
with which it is associated.
Also, when all manual-pages associated with a given application
reside in a specific subdirectory,
you can update the manual pages easily.
.PP
You should also add a uniquely named index file to directory
.B /usr/man
that identifies each of the newly added manual pages.
This index file should use the ``new'' format described above;
and its name should end with the suffix
.BR .index .
.SH Files
\fB/usr/man/*\fR \(em Directories that hold manual pages
\fB/usr/man/*.\index\fR \(em Index files
.SH "See Also"
.Xr "apropos," apropos
.Xr "commands," commands
.Xr "help," help
.Xr "install," install
.Xr "PAGER," pager
.Xr "Using COHERENT" using_coh
.SH Notes
.II "manual^discrepancies with on-line documentation"
The manual pages that are included with your release of the \*(CO system
may include entries that have been corrected and updated since
your \*(CO manual was printed.
If there is a discrepancy between an on-line manual page and the printed
\*(CO manual, you should assume that the on-line manual page is correct.
