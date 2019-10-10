.TH dumptape.h "" "" "Header File"
.PC "Define data structures used on dump tapes"
.B "#include <dumptape.h>"
.PP
.B dumptape.h
defines the data structures used on archives dumped with the command
.BR dump .
Note that the command
.B dump
is regarded as obsolete.
In its place, you should use
.BR pax ,
.BR tar ,
or
.BR cpio .
.SH "See Also"
.Xr "header files" header_fi
.SH Notes
This header file is obsolete, and will be dropped from a future
release of \*(CO.
Its use is strongly discouraged.
