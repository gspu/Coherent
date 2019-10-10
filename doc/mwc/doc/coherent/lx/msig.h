.TH msig.h "" "" "Header File"
.PC "Machine-dependent signals"
.B "#include <signal.h>"
.PP
The header file
.B msig.h
defines the machine-dependent signals that the \*(CO system uses
to communicate with its processes.
The header file
.B signal.h
declares constants for the machine-independent signals, and
includes
.BR msig.h .
.SH "See Also"
.Xr "header files," header_fi
.Xr "signal.h" signal.h
.SH Notes
This header file is obsolete, and will be dropped from a future
release of \*(CO.
Its use is strongly discouraged.
