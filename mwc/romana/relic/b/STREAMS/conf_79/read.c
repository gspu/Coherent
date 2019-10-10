/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		USE_PROTO
 *		LOCAL
 *		ARGS ()
 *	<limits.h>
 *		LONG_MAX
 *		LONG_MIN
 *		ULONG_MAX
 *	<string.h>
 *		memchr ()
 *	"buildobj.h"
 *		BUILD_OK
 *		build_t
 *		builder_alloc ()
 *		build_addchar ()
 *		build_begin ()
 *		build_end ()
 *		build_error ()
 *		build_release ()
 *	"ehand.h"
 *		ehand_t
 *		CHAIN_ERROR ()
 *		POP_HANDLER ()
 *		PUSH_HANDLER ()
 *		throw_error ()
 *	"input.h"
 *		IN_EOF
 *		input_t
 *	"lex.h"
 *		CLASS_FLUSH
 *		CLASS_SEP
 *		lex_t
 *		classify ()
 */

#include <sys/compat.h>
#include <limits.h>
#include <string.h>

#include "buildobj.h"
#include "ehand.h"
#include "input.h"
#include "lex.h"

#include "read.h"


/*
 * Simple glue functions to encapsulate the input system.
 */

#ifdef	USE_PROTO
int (read_char) (input_t * input)
#else
int
read_char ARGS ((input))
input_t	      *	input;
#endif
{
	if (input == NULL)
		throw_error ("NULL parameter passed to read_char ()");

	return (* input->in_read) (input);
}


#ifdef	USE_PROTO
void (unread_char) (input_t * input)
#else
void
unread_char ARGS ((input))
input_t	      *	input;
#endif
{
	if (input == NULL)
		throw_error ("NULL parameter passed to unread_char ()");

	(* input->in_unread) (input);
}


#ifdef	USE_PROTO
void (read_error) (input_t * input)
#else
void
read_error ARGS ((input))
input_t	      *	input;
#endif
{
	if (input == NULL)
		throw_error ("NULL parameter passed to read_error ()");

	(* input->in_error) (input);
}


#ifdef	USE_PROTO
void (read_close) (input_t * input)
#else
void
read_close ARGS ((input))
input_t	      *	input;
#endif
{
	if (input == NULL)
		throw_error ("NULL parameter passed to read_close ()");

	(* input->in_close) (input);
}


/*
 * Simple helper to ensure that we don't bump into EOF or EOL too early.
 */

#ifdef	USE_PROTO
void (check_not_eol) (int ch)
#else
void
check_not_eol ARGS ((ch))
int		ch;
#endif
{
	if (ch == '\n')
		throw_error ("premature end of line");

	if (ch == READ_EOF)
		throw_error ("premature end of file");
}


/*
 * We use this function to eat anything that the lexical specification
 * considers flushable until we see an end-of-line or end-of-file.
 */

#ifdef	USE_PROTO
int (expect_eol) (input_t * input, lex_t * lexp, int ch)
#else
int
expect_eol ARGS ((input, lexp, ch))
input_t	      *	input;
lex_t	      *	lexp;
int		ch;
#endif
{
	if (ch != '\n' && ch != READ_EOF)
		while ((ch = (* input->in_read) (input)) != '\n') {

			if (ch == IN_EOF)
				return READ_EOF;

			if (classify (lexp, ch, 1) != CLASS_FLUSH)
				break;
		}

	return ch;
}


/*
 * Read a token from a file; this function may use the subclassed version to
 * support more efficient tokenization if possible.
 *
 * Note that we don't null-terminate the data or do any other funky stuff. If
 * our caller wants to to that, well that's fine, and we don't finish the
 * object so that the caller can extend it. Note that by returning the token
 * length that we built, this function can be used to incrementally extend
 * variable-length data and allow the positions of the subparts to be properly
 * recovered.
 */

#ifdef	USE_PROTO
int (read_token) (input_t * input, lex_t * lexp, build_t * heap,
		  token_t * tokenp)
#else
int
read_token ARGS ((input, lexp, heap, tokenp))
input_t	      *	input;
lex_t	      *	lexp;
build_t	      *	heap;
token_t	      *	tokenp;
#endif
{
	int		ch;
	int		err;

	if (input == NULL || lexp == NULL || heap == NULL || tokenp == NULL)
		throw_error ("invalid parameters in read_token ()");

	if (input->in_readtok != NULL) {
		/*
		 * Use the subclassed version.
		 */

		tokenp->tok_heap = NULL;

		tokenp->tok_data = (* input->in_readtok) (input, lexp,
							  & tokenp->tok_len);

		return (* input->in_read) (input);
	}


	tokenp->tok_heap = heap;
	tokenp->tok_data = NULL;
	tokenp->tok_len = 0;

	for (;;) {
		if ((ch = (* input->in_read) (input)) == IN_EOF)
			return ch;

		switch (classify (lexp, ch, tokenp->tok_len == 0)) {

		case CLASS_FLUSH:
			continue;

		case CLASS_SEP:
			return ch;

		default:
			break;
		}


		/*
		 * We have read a valid non-separator character, add it to the
		 * current input symbol.
		 */

		tokenp->tok_len ++;

		if ((err = build_addchar (heap, ch)) != BUILD_OK)
			throw_error ("build_addchar () reported %d (%s)", err,
				     build_error (err));
	}
}


/*
 * Handy function for clients of read_token () to finish up any build-heap
 * allocation in the usual case where read_token () is simply expected to copy
 * data to the heap.
 *
 * If it was adding data to the heap, add a NULL terminator for the usual case
 * where we would also like the object to be useable as a string.
 */

#ifdef	USE_PROTO
void (token_end) (token_t * tok)
#else
void
token_end ARGS ((tok))
token_t	      *	tok;
#endif
{
	char		null;

	if (tok->tok_heap == NULL)
		return;

	null = 0;

	if (tok->tok_len != 0 ?
			(build_add (tok->tok_heap, 1, & null) != 0) ||
			 (tok->tok_data =
				build_end (tok->tok_heap, NULL)) == NULL :
			build_end (tok->tok_heap, NULL) != NULL)
		throw_error ("Error ending token construction");
}


/*
 * In the case where a token was able to be scanned in-place, it is often
 * necessary to copy it to a heap, even if only temporarily. As with the above
 * token_end, we terminate the token as if it was a string.
 *
 * If the token is actually in a different heap from the one given, we move it
 * to the new heap. This works in with some special behaviour in the build
 * system where object building can be temporarily suspended, allowing some
 * kinds of recursive operations to work on borrowed heap space.
 */

#ifdef	USE_PROTO
void (token_copy) (token_t * tok, build_t * heap)
#else
void
token_copy ARGS ((tok, heap))
token_t	      *	tok;
build_t	      *	heap;
#endif
{
	char		null;
	unsigned char *	data;
	int		err;

	if (tok->tok_heap == heap)
		return;

	null = 0;

	if ((err = build_begin (heap, tok->tok_len, tok->tok_data)) != 0 ||
	    (err = build_add (heap, 1, & null)) != 0 ||
	    (err = BUILD_NO_OBJECT,
	     (data = build_end (heap, NULL)) == NULL))
		throw_error ("Cannot copy token data to heap, error %s",
			     build_error (err));

	if (tok->tok_heap != NULL &&
	    (err = build_release (heap, tok->tok_data)) != 0)
		throw_error ("Cannot release data from old heap, error %s",
			     build_error (err));

	tok->tok_data = data;
	tok->tok_heap = heap;
}


/*
 * If a read token has been found to be not needed, it may be discarded with
 * this function. If it was copied to a heap, then the heap memory is
 * released.
 */

#ifdef	USE_PROTO
void (token_discard) (token_t * tok)
#else
void
token_discard ARGS ((tok))
token_t	      *	tok;
#endif
{
	int		err;

	if (tok->tok_heap != NULL &&
	    (err = build_release (tok->tok_heap, tok->tok_data)) != 0)
		throw_error ("Cannot release token data, error %s",
			     build_error (err));
}


/*
 * Simply discard flushable input until the next non-flushable input
 * character.
 */

#ifdef	USE_PROTO
void (read_flush) (input_t * input, lex_t * lexp)
#else
void
read_flush ARGS ((input, lexp))
input_t	      *	input;
lex_t	      *	lexp;
#endif
{
	int		ch;

	while ((ch = (* input->in_read) (input)) != IN_EOF) {

		if (classify (lexp, ch, 1) != CLASS_FLUSH) {

			(* input->in_unread) (input);
			break;
		}
	}
}


/*
 * To help with the numeric conversions, here we define a simple conversion
 * utility that converts a character to a digit independent of character set
 * and digit case.
 *
 * ... a truly general way of doing this would be nice ... maybe some kind of
 * virtual-machine interpreter would be up to it ... hmmm.
 */

enum {
	NOT_DIGIT = -1
};

#ifdef	USE_PROTO
LOCAL int (char_to_digit) (int ch, int radix)
#else
LOCAL int
char_to_digit ARGS ((ch, radix))
int		ch;
int		radix;
#endif
{
	static CONST char digits [] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F',
		'a', 'b', 'c', 'd', 'e', 'f'
	};
	static CONST char values [sizeof (digits)] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15,
		10, 11, 12, 13, 14, 15
	};
	CONST char    *	temp;


	if ((temp = (CONST char *) memchr (digits, ch,
					   sizeof (digits))) == NULL ||
	    (ch = values [temp - digits]) >= radix)
		return -1;

	return ch;
}


/*
 * Both read_ulong () and read_long () need to be able to select a radix for
 * the number in question based on an explicit radix prefix. This code does
 * that for both functions; under certain circumstances, the numeric input
 * might be completed by this code.
 */

#ifdef	USE_PROTO
LOCAL int (choose_radix) (input_t * input, unsigned long * ulongp,
			  int * radixp)
#else
LOCAL int
choose_radix ARGS ((input, ulongp, radixp))
input_t	      *	input;
unsigned long *	ulongp;
int	      *	radixp;
#endif
{
	int		ch;
	int		errflag;

	* ulongp = 0;

	/*
	 * Perform a radix-selection step, looking for 0, 1-9, 0X, or 0x as
	 * indications of what radix to read the rest of the number in.
	 */

	switch (ch = (* input->in_read) (input)) {

	case IN_EOF:
		return 0;

	case '0':	/* octal or hexadecimal */
		switch (ch = (* input->in_read) (input)) {

		case IN_EOF:
			return 1;

		case 'x':
		case 'X':
			/*
			 * For the case of radix-16 numbers with an explicit
			 * radix in the text, we have a special error case,
			 * consisting of an 0x or 0X followed by something
			 * that is not a valid digit.
			 *
			 * The easiest way to test for this is to try
			 * converting the first digit right here.
			 */

			* radixp = 16;
			errflag = -1;

			if ((ch = (* input->in_read) (input))
						    == IN_EOF)
				return -1;
			break;

		default:
			* radixp = 8;
			errflag = 1;
			break;
		}
		break;

	default:
		* radixp = 10;
		errflag = 0;
		break;
	}


	/*
	 * The need for the extra error check required by hexadecimal numbers
	 * could have made life difficult for the caller in terms of working
	 * out whether this function actually began reading a number or not.
	 *
	 * To simplify this, we ensure that this function always consumes at
	 * least the first digit. Of course, the actual response to the next
	 * character not being a valid digit is different in each case, so we
	 * also deal with that.
	 */

	if ((ch = char_to_digit (ch, * radixp)) == -1) {

		(* input->in_unread) (input);
		return errflag;
	}

	* ulongp = ch;
	return 0;
}


/*
 * Read an unsigned long number from the input. No initial whitespace is
 * skipped, no sign character is permitted, and the first value that is not
 * valid for a number of the given radix ends conversion.
 *
 * If "radix" is 0, the usual C radix specifiers are recognized. This version
 * of the code has a maximum "radix" value of 16.
 *
 * A return value of 0 indicates no number was seen, a return value of -1
 * indicates an invalid number was seen (such as 0xZ, or a number that is too
 * large to be represented accurately with an unsigned long), and a return
 * value of 1 indicates a number was successfully read.
 */

#ifdef	USE_PROTO
int (read_ulong) (input_t * input, unsigned long * ulongp, int radix)
#else
int
read_ulong ARGS ((input, ulongp, radix))
input_t	      *	input;
unsigned long *	ulongp;
int		radix;
#endif
{
	char		ch;
	unsigned long	temp;
	unsigned long	radix_max;
	int		read_something;

	if (input == NULL || ulongp == NULL || radix < 0 || radix > 16)
		throw_error ("Invalid parameter passed to read_ulong ()");

	* ulongp = 0;

	if (radix == 0) {

		switch (ch = choose_radix (input, ulongp, & radix)) {

		case 0:
			break;

		default:
			return ch;
		}

		read_something = 1;
	} else
		read_something = 0;

	/*
	 * In order to detect overflow portably, we figure out the smallest
	 * value that will cause overflow when multiplied by the radix, and
	 * test against that before the multiplication. If the addition of the
	 * value of "ch" causes overflow, that can be detected by a value of
	 * "temp" that is smaller after the addition, according to the rules
	 * of ANSI/ISO unsigned arithmetic. Note that unsigned overflow is
	 * required to be non-signalling in an ANSI/ISO environment.
	 */

	temp = * ulongp;
	radix_max = ULONG_MAX / radix;

	for (;;) {

		if ((ch = (* input->in_read) (input)) == IN_EOF)
			break;

		if ((ch = char_to_digit (ch, radix)) == -1) {

			(* input->in_unread) (input);
			break;
		}

		if (temp > radix_max) {
			/*
			 * Will overflow during the multiplication.
			 */

			read_something = -1;
			temp = ULONG_MAX;

			continue;
		}

		read_something = 1;
		temp = (temp * radix) + ch;

		if (temp < ch) {
			/*
			 * Overflowed during the addition.
			 */

			read_something = -1;
			temp = ULONG_MAX;

			continue;
		}
	}

	* ulongp = temp;

	return read_something;
}


/*
 * Read a signed long number from the input. No initial whitespace is skipped,
 * and the sign character must immediately precede the digits of the number
 * (or the redix specifier), and the first value that is not valid for a
 * number of the given radix ends conversion.
 *
 * If "radix" is 0, the usual C radix specifiers are recognized. This version
 * of the code has a maximum "radix" value of 16.
 *
 * A return value of 0 indicates no number was seen, a return value of -1
 * indicates an invalid number was seen (such as 0xZ, or a number that is too
 * large to be represented accurately with a signed long), and a return value
 * of 1 indicates a number was successfully read.
 */

#ifdef	USE_PROTO
int (read_long) (input_t * input, long * longp, int radix)
#else
int
read_long ARGS ((input, longp, radix))
input_t	      *	input;
long	      *	longp;
int		radix;
#endif
{
	int		ch;
	unsigned long	temp;
	int		sign;

	/*
	 * To save time and effort, we simply test for an initial sign flag,
	 * use read_ulong () to convert a number, and then range check the
	 * result before converting it to signed form.
	 */

	switch (ch = (* input->in_read) (input)) {

	case IN_EOF:
		return 1;

	case '-':
		sign = -1;
		break;

	case '+':
		sign = 1;
		break;

	default:
		/*
		 * There is no sign character that we can see, return the
		 * lookahead character to the input source so that it will be
		 * checked by read_ulong ().
		 */

		sign = 0;
		(* input->in_unread) (input);
	}

	ch = read_ulong (input, & temp, radix);


	/*
	 * Before we range-check the result that we are going to return, it
	 * pays to note that the range of signed numbers may well not be
	 * symmetric. Typically, there are more negative numbers than non-zero
	 * positive numbers, so that "- LONG_MIN" is not a legal long integer.
	 *
	 * Producing a value of LONG_MIN without getting into implementation-
	 * defined (or undefined, in K&R) territory is tricky because of the
	 * integral promotions. We'll work around it by subtracting from -1
	 * rather than zero.
	 *
	 * We'd better test that the range of negative integers is at most one
	 * greater than the range of non-zero positive integers. We can't do
	 * the test if the preprocessor does arithmetic wrong, though, and
	 * many do.
	 */

#if	-23UL > 0
# if	(- (LONG_MIN + 0UL)) - 1 > LONG_MAX
#  error	There are too many negative integers!
# endif
#else
	/* Your preprocessor does arithmetic wrong */
#endif


	if (sign < 0 && temp != 0) {

		temp -= 1;

		if (temp > - (unsigned long) LONG_MIN - 1) {

			* longp = LONG_MIN;
			return -1;
		} else
			* longp = -1 - (long) temp;

	} else if (temp > LONG_MAX) {

		* longp = LONG_MAX;
		return -1;
	} else
		* longp = temp;


	/*
	 * If we saw a sign (of either kind) and nothing else, that's an
	 * error.
	 */

	return (sign != 0 && ch == 0) ? -1 : ch;
}


/*
 * Read a single unsigned long or a numeric range (indicated by a pair of
 * unsigned longs separated by a hyphen without any intervening whitespace).
 */

#ifdef	USE_PROTO
int (read_ulongs) (input_t * input, lex_t * lexp, unsigned long * number,
		   int rangeflag)
#else
int
read_ulongs ARGS ((input, lexp, number, rangeflag))
input_t	      *	input;
lex_t	      *	lexp;
unsigned long *	number;
int		rangeflag;
#endif
{
	if ((rangeflag != RANGE && rangeflag != NO_RANGE) ||
	    input == NULL || lexp == NULL || number == NULL)
		throw_error ("Invalid parameter to read_ulongs ()");

	/*
	 * We permit initial whitespace according to the current lexical
	 * idea of what whitespace is.
	 */

	read_flush (input, lexp);

	if (read_ulong (input, number, 0) != 1)
		throw_error ("Illegal unsigned long number");

	if (rangeflag == RANGE) {
		int		ch;

		if ((ch = (* input->in_read) (input)) != IN_EOF &&
		    ((* input->in_unread) (input),	/* for effect */
		     ch == '-')) {
			/*
			 * Read the second part of the range.
			 */

			if (read_ulong (input, number + 1, 0) != 1)
				throw_error ("Illegal second half of unsigned long range");
		} else
			number [1] = number [0];
	}

	return (* input->in_read) (input);
}


/*
 * Read a single integer or a numeric range (indicated by a pair of integers
 * separated by a hyphen without any intervening whitespace).
 */

#ifdef	USE_PROTO
int (read_ints) (input_t * input, lex_t * lexp, int * number, int rangeflag)
#else
int
read_ints ARGS ((input, lexp, number, rangeflag))
input_t	      *	input;
lex_t	      *	lexp;
int	      *	number;
int		rangeflag;
#endif
{
	long		value;

	if ((rangeflag != RANGE && rangeflag != NO_RANGE) ||
	    input == NULL || lexp == NULL || number == NULL)
		throw_error ("Invalid parameter to read_ints ()");

	/*
	 * We permit initial whitespace according to the current lexical
	 * idea of what whitespace is.
	 */

	read_flush (input, lexp);

	if (read_long (input, & value, 0) != 1 ||
#ifdef	__COHERENT__
		0)	/* Coherent compiles the test below to bad code */
#else
	    value > INT_MAX || value < INT_MIN)
#endif
		throw_error ("Illegal integer number");

	number [0] = (int) value;

	if (rangeflag == RANGE) {
		int		ch;

		if ((ch = (* input->in_read) (input)) != IN_EOF &&
		    ((* input->in_unread) (input),	/* for effect */
		     ch == '-')) {
			/*
			 * Read the second part of the range.
			 */

			if (read_long (input, & value, 0) != 1 ||
#ifdef	__COHERENT__
				0)	/* Coherent compiles to bad code */
#else
			    value > INT_MAX || value < INT_MIN)
#endif
				throw_error ("Illegal second half of integer range");
		}

		number [1] = (int) value;
	}

	return (* input->in_read) (input);
}


/*
 * Read a single integer or a numeric range (indicated by a pair of integers
 * separated by a hyphen without any intervening whitespace).
 */

#ifdef	USE_PROTO
int (read_longs) (input_t * input, lex_t * lexp, long * number, int rangeflag)
#else
int
read_longs ARGS ((input, lexp, number, rangeflag))
input_t	      *	input;
lex_t	      *	lexp;
long	      *	number;
int		rangeflag;
#endif
{
	if ((rangeflag != RANGE && rangeflag != NO_RANGE) ||
	    input == NULL || lexp == NULL || number == NULL)
		throw_error ("Invalid parameter to read_longs ()");

	/*
	 * We permit initial whitespace according to the current lexical
	 * idea of what whitespace is.
	 */

	read_flush (input, lexp);

	if (read_long (input, number, 0) != 1)
		throw_error ("Illegal long-integer number");

	if (rangeflag == RANGE) {
		int		ch;

		if ((ch = (* input->in_read) (input)) != IN_EOF &&
		    ((* input->in_unread) (input),	/* for effect */
		     ch == '-')) {
			/*
			 * Read the second part of the range.
			 */

			if (read_long (input, number + 1, 0) != 1)
				throw_error ("Illegal second half of long-integer range");
		}
	}

	return (* input->in_read) (input);
}


/*
 * Read a single unsigned integer or a numeric range (indicated by a pair of
 * unsigned integers separated by a hyphen without any intervening
 * whitespace).
 */

#ifdef	USE_PROTO
int (read_uints) (input_t * input, lex_t * lexp, unsigned int * number,
		  int rangeflag)
#else
int
read_uints ARGS ((input, lexp, number, rangeflag))
input_t	      *	input;
lex_t	      *	lexp;
unsigned int  *	number;
int		rangeflag;
#endif
{
	unsigned long	value;

	if ((rangeflag != RANGE && rangeflag != NO_RANGE) ||
	    input == NULL || lexp == NULL || number == NULL)
		throw_error ("Invalid parameter to read_ints ()");

	/*
	 * We permit initial whitespace according to the current lexical
	 * idea of what whitespace is.
	 */

	read_flush (input, lexp);

	if (read_ulong (input, & value, 0) != 1 || value > UINT_MAX)
		throw_error ("Illegal unsigned integer number");

	number [0] = (unsigned int) value;

	if (rangeflag == RANGE) {
		int		ch;

		if ((ch = (* input->in_read) (input)) != IN_EOF &&
		    ((* input->in_unread) (input),	/* for effect */
		     ch == '-')) {
			/*
			 * Read the second part of the range.
			 */

			if (read_ulong (input, & value, 0) != 1 ||
			    value > UINT_MAX)
				throw_error ("Illegal second half of unsigned integer range");
		}

		number [1] = (unsigned int) value;
	}

	return (* input->in_read) (input);
}
