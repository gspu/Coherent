/*
 * Turn the warning and error comments into nroff
 * in the form suggested by Fred Buntzen.
 *
 * Input lines are formatted as
 *	sort field
 *	message
 *	message type
 *	zero or more comment lines
 *
 * Output lines are formatted as
 * .IP "\fB message \fR" (message type)
 * comment
 * comment ...
 */
#include <stdio.h>

enum state { start, startMsg, inMsg, pct, mType, At, bsl, inComm };

main(argc, argv)
char *argv[];
{
	register enum state state;
	register c;
	char *p, delim;

	delim = (NULL == argv[1]) ? '@' : argv[1][0];

	for (state = start; EOF != (c = getchar()); ) {
		switch (state) {
		case start:	/* discard sort field */
			if (c != delim)
				continue;
			printf(".IP \"\\fR");
			state = startMsg;
			continue;

		case startMsg:	/* first char of message */
			if ('.' == c) {
				putchar('\\');
				putchar('&');
			}
			state = inMsg;

		case inMsg:
			if (delim == c) {
				state = mType;
				printf("\\fR (");
				continue;
			}
			switch (c) {
			case '\\':
				state = bsl;
				continue;
			case '%':
				state = pct;
				continue;

			case '\n':
				state = start;
				printf("\\fR\"\n");
				continue;

			case '\'':
				putchar('\\');
			}
			break;

		case mType:
			if (delim == c) {
				state = At;
				printf(")\"\n");
				continue;
			}
			else if ('\n' == c) {
				putchar('"');
				state = start;
			}
			break;

		case bsl:
			state = inMsg;
			switch (c) {
			case '\\':
				printf("\\e");
				continue;
			default:	/* to be expanded */
				putchar('\\');
			}
			break;

		case pct:
			switch (c) {
			case 'c':
				p = "c";
				break;
			case 'd':
				p = "n";
				break;
			case 'x':
				p = "0xn";
				break;
			case 's':
				p = "string";
				break;
			default:
				continue;
			}
			printf("\\fI%s\\fR", p);
			state = inMsg;
			continue;

		case At:
			switch (c) {
			case ' ':	/* discard first space */
				state = inComm;
				continue;
			case '\n':	/* back to start */
				state = start;
				continue;
			default:	/* normal output */
				if (delim == c)
					continue;
				state = inComm;
			}
			break;

		case inComm:			
			if (delim == c) {
				c = '\n';
				state = At;
			}
			else if ('\n' == c)
				state = start;
		}
		putchar (c);
	}
	exit(0);
}
