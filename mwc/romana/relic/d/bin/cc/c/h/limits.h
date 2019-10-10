/*
 * ANSI C Numerical limits
 *	8 bit byte
 *	char is signed
 *	16 bit short
 *	16 bit int
 *	32 bit long
 *	2's complement arithmetic
 */
#define CHAR_BIT	8
#define SCHAR_MIN	0x00000080
#define SCHAR_MAX	0x0000007F
#define UCHAR_MAX	0x000000FF
#define CHAR_MIN	SCHAR_MIN
#define CHAR_MAX	SCHAR_MAX
#define SHRT_MIN	0x00008000
#define SHRT_MAX	0x00007FFF
#define USHRT_MAX	0x0000FFFF
#define INT_MIN		SHRT_MIN
#define INT_MAX		SHRT_MAX
#define UINT_MIN	USHRT_MIN
#define LONG_MIN	0x80000000L
#define LONG_MAX	0x7FFFFFFFL
#define ULONG_MAX	0xFFFFFFFFL
