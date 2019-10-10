Read_me.fp 1/15/92
steve's notes on cc386 software floating point runtime support

Functions return floating point values in the 64-bit register pair %edx:%eax.
Like other C functions, floating point support routines
should preserve registers %esi, %edi, and %ebx (and all segment registers).
The routines which are generated directly as calls by the code tables must
also preserve all unused registers, including %ecx (and %edx if not used
for the return value), because the register may be used in indexing.
For efficiency of the generated code, many floating point support routines
take register arguments; they do not observe standard C calling conventions.

Arithmetic operations:
	double		_dadd(double d)
			return d + %edx:eax in %edx:%eax
	double		_ddiv(double d)
			return d / %edx:eax in %edx:%eax
	double		_dmul(double d)
			return d * %edx:eax in %edx:%eax
	double		_drdiv(double d)
			return %edx:eax / d in %edx:%eax
	double		_drsub(double d)
			return %edx:eax - d in %edx:%eax
	double		_dsub(double d)
			return d - %edx:eax in %edx:%eax
	double		_fadd(float f)
			return f + %edx:eax in %edx:%eax
	double		_fdiv(float f)
			return f / %edx:eax in %edx:%eax
	double		_fmul(float f)
			return f * %edx:eax in %edx:%eax
	double		_fsub(float f)
			return f - %edx:eax in %edx:%eax

Conversions:
	double		_dfcvt()
			convert float in %eax to double, return in %edx:%eax
	double		_dicvt()
			convert int in %eax to double, return in %edx:%eax
	double		_ducvt()
			convert unsigned int in %eax to double, return in %edx:%eax
	"float"		_fdcvt()
			convert double in %edx:%eax to float, return in %eax
	int		_idcvt()
			convert double in %edx:%eax to int, return in %eax
	int		_ifcvt()
			convert float in %eax to int, return in %eax
	unsigned int	_udcvt()
			convert double in %edx:%eax to unsigned int, return in %eax
	unsigned int	_ufcvt()
			convert float in %eax to unsigned int, return in %eax

Comparisons:
	void		_dcmp(double d)
			compare double in %edx:%eax to d, set flags accordingly;
			pops its argument before returning

Library routines requiring assembler support:
	frexp()
	ldexp()
	modf()
	ieee_d(double *idp, double *ddp)
	ieee_f(float *ifp, float *dfp)
		Convert from DECVAX format to IEEE format.
		ddp/dfp points to a DECVAX format double/float to convert.
		idp/ifp points to a destination for the converted IEEE value;
		idp/ifp may be identical to ddp/dfp for in-place conversion.
	decvax_d(double *ddp, double *idp)
	decvax_f(float *dfp, double *ifp)
		Convert from IEEE format to DECVAX format.
		idp/ifp points to an IEEE format double/float to convert.
		ddp/dfp points to a destination for the converted DECVAX value;
		ddp/dfp may be identical to idp/ifp for in-place conversion.
		
Unresolved issues:
	negation: currently tables flip sign bit, so 0.0 -> -0.0;
		call runtime support routine instead to avoid -0.0?
	Denormals -- support or ignore?
	leaves.t: ZTEST calls, probably wrong for denormal IEEE

Floating point formats:
	DECVAX double:	1 sign +  8 exponent + 56 mantissa, bias  129
	DECVAX float:	1 sign +  8 exponent + 24 mantissa, bias  129
	IEEE double:	1 sign + 11 exponent + 53 mantissa, bias 1023
	IEEE float:	1 sign +  8 exponent + 24 mantissa, bias  127
In memory, compiler stores d[0] = lo mantissa, ..., d[7] = sign/hi exp.;
	LO [addr] = d[0]...d[3], HI [addr] = d[4]...d[7].
In register EDXEAX, EDX contains sign/exp/hi mantissa, EAX lo mantissa;
	LO [EDXEAX] = EAX, HI [EDXEAX] = EDX.

