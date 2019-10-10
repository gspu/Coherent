.TH "limits.h" "" "" "Header File"
.PC "Define numerical limits"
.B "#include <limits.h>"
.PP
The header file
.B <limits.h>
defines macros that set the numerical limits for the translation environment.
.PP
The following table gives the macros defined in
.BR limits.h .
Each value given is the macro's minimum maximum:
a conforming implementation of C must  meet these limits, and
may exceed them.
.IP \fBCHAR_BIT\fR
.II CHAR_BIT
Number of bits in a
.BR char .
This must be at least eight.
.IP \fBCHAR_MAX\fR
.II CHAR_MAX
Largest value representable in an object of type
.BR char .
If the implementation defines a
.B char
to be signed, then it is equal to the value of the macro
.BR SCHAR_MAX ;
otherwise, it is equal to the value of the macro
.BR UCHAR_MAX .
.IP \fBCHAR_MIN\fR
.II CHAR_MIN
Smallest value representable in an object of type
.BR char .
If the implementation defines a
.B char
to be signed, then it is equal to the value of the macro
.BR SCHAR_MIN ;
otherwise, it is zero.
.IP \fBINT_MAX\fR
.II INT_MAX
Largest value representable in an object of type
.BR int ;
it must be at least 32,767 (0x7FFF).
.IP \fBINT_MIN\fR
.II INT_MIN
Smallest value representable in an object of type
.BR int ;
no less more \-32,767.
.IP \fBLONG_MAX\fR
.II LONG_MAX
Largest value representable in an object of type
.BR "long int" ;
it must be at least 2,147,483,647 (0x7FFFFFFFL).
.IP \fBLONG_MIN\fR
.II LONG_MIN
Smallest value representable in an object of type
.BR "long int" ;
it must be at most \-2,147,483,647.
.IP \fBMB_LEN_MAX\fR
.II MB_LEN_MAX
Largest number of bytes in any multibyte character, for any locale;
it must be at least one.
.IP \fBOPEN_MAX\fR
.II OPEN_MAX
The maximum number of file descriptors that a process can hold
at any given time.
.IP
Please note that this constant gives a ``snapshot'' of the state of \*(CO
at this time.
Using this constant in a program, in particular to size an array, greatly
decreases the portability of a program, and may cause it to behave incorrectly.
To determine the number of file descriptors that the operating system permits
right now, use the system call
.BR sysconf() .
.IR "Caveat utilitor!"
.IP \fBSCHAR_MAX\fR
.II SCHAR_MAX
Largest value representable in an object of type
.BR "signed char" ;
it must be at least 127.
.IP \fBSCHAR_MIN\fR
.II SCHAR_MIND
Smallest value representable in an object of type
.BR "signed char" ;
it must be at most \-127.
.IP \fBSHRT_MAX\fR
.II SHRT_MAX
Largest value representable in an object of type
.BR "short int" ;
it must be at least 32,767 ((\fBshort\fR)0x7FFF).
.IP \fBSHRT_MIN\fR
.II SHRT_MIN
Smallest value representable in an object of type
.BR "short int" ;
it must be at most \-32,767.
.IP \fBUCHAR_MAX\fR
.II UCHAR_MAX
Largest value representable in an object of type
.BR "unsigned char" ;
it must be at least 255.
.IP \fBUINT_MAX\fR
.II UINT_MAX
Largest value representable in an object of type
.BR "unsigned int" ;
it must be at least 65,535 ((\fBunsigned int\fR)0xFFFF).
.IP \fBULONG_MAX\fR
.II ULONG_MAX
Largest value representable in an object of type
.BR "unsigned long int" ;
it must be at least 4,294,967,295 ((\fBunsigned long\fR)0xFFFFFFFFL).
.IP \fBUSHRT_MAX\fR
.II USHRT_MAX
Largest value representable in an object of type
.BR "unsigned short int" ;
it must be at least 65,535 ((\fBunsigned short\fR)0xFFFF).
.SH "See Also"
.Xr "header files" header_fi
.br
\*(AS, \(sc5.2.4.2.1
.br
\*(PX Standard, \(sc2.8
.SH Notes
.B limits.h
sets fixed limits.
If a limit is not completely fixed, then the symbol is not defined,
and a process must use
.B sysconf()
or
.BR pathconf() ,
as appropriate, to find the limit's value for the current run of the process.
