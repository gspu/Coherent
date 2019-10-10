.TH coff.h "" "" "Header File"
.PC "Format for COFF objects"
.B "#include <coff.h>"
.PP
.II "Common Object File Format"
.B coff.h
describes the Common Object File Format (COFF), which is the
object format used by \*(CO 386.
.SH "What Is COFF?"
.II "COFF^definition"
In brief, COFF is the \*(UN System V standard for file formats.
It defines the formats for relocatable object modules, for
executable files, and for archives.
.PP
A COFF file is built around three sections, or \fIsegments\fR:
.IP \fBtext\fR
This holds executable machine code.
It is write protected \(em the operating system is forbidden to overwrite it.
.II "protected mode^definition"
(This is why operating systems that use COFF or similar formats are said
to run in ``protected mode.'')
.IP \fBdata\fR
This holds initialized data, that is, the data that the program finds
when it begins execution.
The program can read and write into this segment.
.IP \fBbss\fR
This segment holds unitialized data.
It is simply a mass of space that is initialized to zeroes.
It is contiguous with the \fBdata\fR segment.
The term \fBbss\fR from the old IBM mainframe days, and stands for ``block
started by symbol''.
.PP
Not all segments have to be included in every COFF file.
Further, some implementations of COFF define their own segments that
manipulate special features of the operating system or hardware.
.PP
The following describes the structure of a COFF file.
The areas within the file are described in the order in which they appear.
.IP "\fB1.\fR \fIfile header\fR"
This holds information set when the file was created, such as the date and
time it was created, the number of segments in the file, a pointer
to the symbol table, and status flags.
.IP "\fB2. \fIoptional header\fR"
This gives information set at run-time, such as the address of the program
entry point, and the size of the code and data segments.
.IP "\fB3. \fIsegment headers\fR"
The next area holds a header for each segment in the file.
Each header describes its segment's characteristics and contains pointers
to the segment's contents, relocation information, line-number information,
and other useful addresses.
.IP "\fB4. \fIsegment contents\fR"
The next area holds the contents of the segments used in this file.
.IP "\fB5. \fIrelocation information\fR"
The fifth area gives relocation information, one set of information for
each segment in the file.
The linker
.B ld
uses this information to generate the executable file at link time.
.IP "\fB6. \fIline-number information\fR"
This area holds debug information, one set of information for each segment.
This area is optional.
.IP "\fB7. \fIsymbol table\fR"
This area holds information used by both the linker and the debugger.
.IP "\fB8. \fIstring table\fR"
This table holds very long names of variables.
.PP
Most of this information is irrelevant to the average user, or even the
average developer of software.
To the average user, COFF is ``a machine that would go of itself'';
you can run or compile programs without worrying what the linker puts
where, or why.
These details, however, can be very important if you are writing tools that
manipulate the internals of files, such as archivers or debuggers.
If you need detailed information on COFF and how to manipulate it, see
\fIUnderstanding and Using COFF\fR (citation appears below).
.PP
For more information on how the COFF format affects \*(CO's language
tools, see the Lexicon articles for
.BR ar ,
.BR "as" ,
.BR cc ,
.BR db ,
and
.BR ld .
.SH "See Also"
.Xr "ar," ar
.Xr "as," as
.Xr "cc," cc
.Xr "cdmp," cdmp
.Xr "coffnlist()," coffnlist
.Xr "file formats," file_form
.Xr "header files," header_fi
.Xr "ld" ld
.br
.II "Gircys, Gintaras R."
Gircys, G.R.: \fIUnderstanding and Using COFF\fR.
Sebastopol, Calif., O'Reilly & Associates, Inc., 1988.
