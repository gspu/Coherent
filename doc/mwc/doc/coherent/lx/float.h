.TH "float.h" "" "" "Header File"
.PC "Define constants for floating-point numbers"
.PP
The header file
.B float.h
defines the following manifest constants,
which mark the limits for computation of floating-point numbers.
The prefixes
.BR DBL ,
.BR FLT ,
and
.B LDBL
refer, respective, to
.BR double ,
.BR float ,
and
.BR "long double" :
.IP \fBDBL_DIG\fR
.II DBL_DIG
Number of decimal digits of precision.
.IP \fBDBL_EPSILON\fR
.II DBL_EPSILON
Smallest possible floating-point number \fIx\fR,
such that 1.0 plus \fIx\fR does not test equal to 1.0.
.IP \fBDBL_MANT_DIG\fR
.II DBL_MANT_DIG
Number of digits in the floating-point mantissa for base
.BR FLT_RADIX .
.IP \fBDBL_MAX\fR
.II DBL_MAX
Largest number that can be held by type
.BR double .
.IP \fBDBL_MAX_EXP\fR
.II DBL_MAX_EXP
Largest integer such that the value of
.B FLT_RADIX
raised to its power minus one is less than or equal to
.BR DBL_MAX .
.IP \fBDBL_MAX_10_EXP\fR
.II DBL_MAX_10_EXP
Largest integer such that ten
raised to its power is less than or equal to
.BR DBL_MAX .
.IP \fBDBL_MIN\fR
.II DBL_MIN
Smallest number that can be held by type
.BR double .
.IP \fBDBL_MIN_EXP\fR
.II DBL_MIN_EXP
Smallest integer such that the value of
.B FLT_RADIX
raised to its power minus one is greater than or equal to
.BR DBL_MIN .
.IP \fBDBL_MIN_10_EXP\fR
.II DBL_MIN_10_EXP
Smallest integer such that ten
raised to its power is greater than or equal to
.BR DBL_MAX .
.IP \fBFLT_DIG\fR
.II FLT_DIG
Number of decimal digits of precision.
.IP \fBFLT_EPSILON\fR
.II FLT_EPSILON
Smallest floating-point number \fIx\fR, such that 1.0 plus \fIx\fR does not
test equal to 1.0.
.IP \fBFLT_MANT_DIG\fR
.II FLT_MANT_DIG
Number of digits in the floating-point mantissa for base
.BR FLT_RADIX .
.IP \fBFLT_MAX\fR
.II FLT_MAX
Largest number that can be held by type
.BR float .
.IP \fBFLT_MAX_EXP\fR
.II FLT_MAX_EXP
Largest integer such that the value of
.B FLT_RADIX
raised to its power minus one is less than or equal to
.BR FLT_MAX .
.IP \fBFLT_MAX_10_EXP\fR
.II FLT_MAX_10_EXP
Largest integer such that ten
raised to its power is less than or equal to
.BR FLT_MAX .
.IP \fBFLT_MIN\fR
.II FLT_MIN
Smallest number that can be held by type
.BR float .
.IP \fBFLT_MIN_EXP\fR
.II FLT_MIN_EXP
Smallest integer such that the value of
.B FLT_RADIX
raised to its power minus one is greater than or equal to
.BR FLT_MIN .
.IP \fBFLT_MIN_10_EXP\fR
.II FLT_MIN_10_EXP
Smallest integer such that ten
raised to its power is greater than or equal to
.BR FLT_MIN .
.IP \fBFLT_RADIX\fR
.II FLT_RADIX
Base in which the exponents of all floating-point numbers are represented.
.IP \fBFLT_ROUNDS\fR
.II FLT_ROUNDS
Manner of rounding used by the implementation.
The ANSI Standard defines the rounding codes as follows:
.DM
.ta 0.1iR 0.4i
	\fB\-1\fR	Indeterminable, i.e., no strict rules apply
	\fB0\fR	Toward zero, i.e., truncation
	\fB1\fR	To nearest, i.e., rounds to nearest representable value
	\fB2\fR	Toward positive infinity, i.e., always rounds up
	\fB3\fR	Toward negative infinity, i.e., always rounds down
.DE
.IP
\*(CO uses type-1 rounding.
.IP \fBLDBL_DIG\fR
.II LDBL_DIG
Number of decimal digits of precision.
.IP \fBLDBL_EPSILON\fR
.II LDBL_EPSILON
Smallest floating-point number \fIx\fR, such that 1.0 plus \fIx\fR does not
test equal to 1.0.
.IP \fBLDBL_MANT_DIG\fR
.II LDBL_MANT_DIG
Number of digits in the floating-point mantissa for base
.BR FLT_RADIX .
.IP \fBLDBL_MAX\fR
.II LDBL_MAX
Largest number that can be held by type
.BR "long double" .
.IP \fBLDBL_MAX_EXP\fR
.II LDBL_MAX_EXP
Largest integer such that the value of
.B FLT_RADIX
raised to its power minus one is less than or equal to
.BR LDBL_MAX .
.IP \fBLDBL_MAX_10_EXP\fR
.II LDBL_MAX_10_EXP
Largest integer such that ten
raised to its power is less than or equal to
.BR LDBL_MAX .
.IP \fBLDBL_MIN\fR
.II LDBL_MIN
Smallest number that can be held by type
.BR "long double" .
Must be no greater than 1E-37.
.IP \fBLDBL_MIN_EXP\fR
.II LDBL_MIN_EXP
Smallest integer such that the value of
.B FLT_RADIX
raised to its power minus one is greater than or equal to
.BR LDBL_MIN .
.IP \fBLDBL_MIN_10_EXP\fR
.II LDBL_MIN_10_EXP
Smallest integer such that ten
raised to its power is greater than or equal to
.BR LDBL_MIN .
.SH "See Also"
.Xr "double," double
.Xr "float," float
.Xr "header files" header_fi
.br
\*(AS, \(sc5.2.4.2.2
.SH Notes
\*(CO's C compiler does not yet implement type
.BR "long double" .
