.TH kb.h "" "" "Header File"
.PC "Define keys for loadable keyboard driver"
.B "#include <sys/kb.h>"
.PP
The header file
.B kb.h
defines macros and manifest constants that are used with
.BR nkb ,
the user-configurable keyboard driver.
It is
.BR include d
with the C programs that the user can modify and compile to remap her
keyboard.
See the Lexicon entries
.B nkb
and
.B "keyboard tables"
for more information.
.PP
.B nkb
is also used with \*(CO system of virtual consoles.
.B kb.h
sets default definitions for function keys, as follows:
.IP "\fBvt0\fR \(em \fBvt15\fR"
Switch to logical session 0 through 15, respectively.
.IP "\fBcolor0\fR \(em \fBcolor15\fR"
Switch to color session 0 through 15, respectively.
.IP "\fBmono0\fR \(em \fBmono15\fR"
Switch to monochrome session 0 through 15, respectively.
.IP \fBvtn\fR
Switch to next higher-numbered open session.
.IP \fBvtp\fR
Switch to next lower-numbered open session.
.IP \fBvtt\fR
Toggle to most recently used open session
.SH "See Also"
.Xr "header files," header_fi
.Xr "virtual console," virtual_c
.Xr "vtkb," vtkb
.Xr "vtnkb" vtnkb
