/**
 *
 * long
 * myatol( s )		-- Ascii to Long integer conversion.
 * char * s;
 *
 *	Input:	s = pointer to string containing a numeric prefix.
 *
 *	Action:	Parse input string.
 *		Parse optional leading sign character '-'.
 *		Parse optional numeric base specification '0', '0o', and '0x'.
 *		Parse following numeric digits.
 *
 *	Return:	Long integer value.
 *
 *	Notes:	Numeric parsing terminates on first non-digit.
 */
long
myatol( s )
	register char * s;
{
	register int base;
	register int sign;
	auto 	long valu;

	/*
	 * Check for leading negative sign.
	 */
	sign = 1;
	if ( *s == '-' ) {
		sign = -1;
		s++;
	}

	/*
	 * Check for base specification.
	 */
	base = 10;
	if ( *s == '0' ) {
		switch ( *++s ) {
		case 'x':	base = 16;	++s;	break;
		case 'o':	base =  8;	++s;	break;
		default:	base =  8;
		}
	}

	for ( valu = 0L; *s != '\0'; s++ ) {

		/*
		 * Decimal digit.
		 */
		if ( ('0' <= *s) && (*s <= '9') ) {
			valu *= base;
			valu += *s - '0';
		}

		/*
		 * Upper case hex digit.
		 */
		else if	( (base == 16) && ('A' <= *s) && (*s <= 'F') ) {
			valu *= base;
			valu += *s - ('A' - 10);
		}

		/*
		 * Lower case Hex digit.
		 */
		else if	( (base == 16) && ('a' <= *s) && (*s <= 'f') ) {
			valu *= base;
			valu += *s - ('a' - 10);
		}

		/*
		 * Not a digit.
		 */
		else
			break;
	}

	if ( sign < 0 )
		valu = -valu;

	return valu;
}
