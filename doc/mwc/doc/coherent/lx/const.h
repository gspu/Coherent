.TH const.h "" "" "Header File"
.PC "Declare machine-dependent constants"
.B "#include <sys/const.h>"
.PP
The header file
.B const.h
declares most machine-dependent constants.
These are constants that change among the various machines for
which the \*(CO system is available; an example is the
clock speed of the processor.
.SH "See Also"
.Xr "header files," header_fi
.Xr "times()" times
.SH Notes
This header file is obsolete and will be dropped from a future
release of \*(CO.
Its use is strongly discouraged.
