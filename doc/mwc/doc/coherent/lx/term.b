.TH term "" "" "System Administration"
.PC "Format of compiled terminfo file"
.PP
.II "terminfo^file format"
Before it can be used, a file of
.B terminfo
information must be compiled with the command
.BR tic .
It is read by the command
.BR setupterm .
.PP
Once compiled, the binary
.B terminfo
file is moved into a sub-directory of directory
.BR /usr/lib/terminfo .
To avoid a linear search of a huge \*(CO directory, a two-level
scheme is used to name the subdirectories:
\fB/usr/lib/terminfo/\fIC\^\fB/\fIname\^\fR,
where
.I name
names the terminal and
.I C
is the first character of
.IR name .
For example, the
.B terminfo
entry for the Wyse 150
terminal is kept in the file \fB/usr/lib/terminfo/w/wyse150\fR.
Synonyms for a terminal exist as links to the same compiled file.
.PP
The binary
format of a
.B terminfo
file has been designed to be the same on all hardware.
The file is divided into six parts:
header, terminal names, boolean flags,
numbers, strings, and string table.
.SH Header
The
.I header
section begins the file.
This section contains the following six short integers:
.IP \fB1.\fR 0.3i
The magic number (octal 0432).
.IP \fB2.\fR
The size, in bytes, of the
.I names
section.
.IP \fB3.\fR
The number of bytes in the
.I boolean
section.
.IP \fB4.\fR
The number of short integers in the
.I numbers
section.
.IP \fB5.\fR
The number of offsets (short integers) in the
.I strings
section.
.IP \fB6.\fR
The size, in bytes, of the
.I string
table.
.PP
A \fIshort integer\fR is two bytes long.
Under the
.B term
file format, 0xFFFF represents \-1;
all other negative value are illegal.
Minus 1 generally means that a capability is missing from this terminal.
All short integers are aligned on a short-word boundary.
.SH Names
The
.I names
section
contains the first line of the
.B terminfo
description, which lists the
names for the terminal, each name separated by a vertical bar `|'.
The section is 
terminated with a NUL.
.SH Boolean
The
.I boolean
section contains the boolean flags for terminals.
There is one flag for each boolean capacity recognized by
.BR terminfo .
The flags appear in the order described in the header file
.BR term.h .
Each flag is one byte long, and is set to zero or one, depending upon
whether the capacity is absent or present in this terminal.
If necessary,
this section is ended with a NUL
to ensure that the next section begins on an
even byte.
.SH Numbers
The
.I numbers
section is similar to the
.I flags
section.
There is one entry for each numeric capacity recognized by
.BR terminfo ,
each capacity being represented by a short integer.
A value of \-1 indicates that this terminal lacks this capability.
Entries appear in the order described in the header file
.BR term.h .
.SH Strings
The
.I strings
section also contains one short integer for each string capability
recognized by
.BR terminfo .
A value of \-1 means that this terminal lacks this capability.
Otherwise, the value gives an offset from the beginning of the string table.
Entries appear in the order described in the header file
.BR term.h .
.PP
Special characters in \fB^X\fR or \fB\e\fIc\fR
notation are stored in their interpreted form.
Padding information and parameter information are stored
intact in uninterpreted form.
.SH "String Table"
The final section is the
.IR "string table" .
It contains all the values of string capabilities referenced in the
.I string
section.
Each string is null terminated.
.\".SH Example
.\"The following
.\".B terminfo
.\"file describes the Microterm ACT 4 terminal:
.\".DM
.\"microterm|act4|microterm act iv,
.\"    cr=^M, cud1=^J, ind=^J, bel=^G, am, cub1=^H,
.\"    ed=^_, el=^^, clear=^L, cup=^T%p1%c%p2%c,
.\"    cols#80, lines#24, cuf1=^X, cuu1=^Z, home=^],
.\"0
.\".DE
.\".PP
.\"The following gives an octal dump of the file compiled from this description:
.\".DM
.\"000 032 001      \e0 025  \e0  \eb  \e0 212  \e0   "  \e0   m   i   c   r
.\"020   o   t   e   r   m   |   a   c   t   4   |   m   i   c   r   o
.\"040   t   e   r   m       a   c   t       i   v  \e0  \e0 001  \e0  \e0
.\"060  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0  \e0
.\"100  \e0  \e0   P  \e0 377 377 030  \e0 377 377 377 377 377 377 377 377
.\"120 377 377 377 377  \e0  \e0 002  \e0 377 377 377 377 004  \e0 006  \e0
.\"140  \eb  \e0 377 377 377 377  \en  \e0 026  \e0 030  \e0 377 377 032  \e0
.\"160 377 377 377 377 034  \e0 377 377 036  \e0 377 377 377 377 377 377
.\"200 377 377 377 377 377 377 377 377 377 377 377 377 377 377 377 377
.\"*
.\"520 377 377 377 377      \e0 377 377 377 377 377 377 377 377 377 377
.\"540 377 377 377 377 377 377 007  \e0  \er  \e0  \ef  \e0 036  \e0 037  \e0
.\"560 024   %   p   1   %   c   %   p   2   %   c  \e0  \en  \e0 035  \e0
.\"600  \eb  \e0 030  \e0 032  \e0  \en  \e0
.\".DE
.SH Files
\fB/usr/lib/terminfo/*\fR \(em Default location of object files
.SH "See Also"
.Xr "Administering COHERENT," administe
.Xr "curses," curses
.Xr "infocmp," infocmp
.Xr "tic," tic
.Xr "terminfo" terminfo
.br
Strang, J., Mui, L., O'Reilly, T.:
\fItermcap and terminfo\fR.
Sebastopol, CA:
O'Reilly & Associates, Inc., 1991.
.SH Notes
The total compiled file cannot exceed 4,096 bytes.
The
.I name
field cannot exceed 128 bytes.
