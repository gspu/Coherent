#include "asm.h"

getnb()
{
	register c;

	do {
		if ((c = *ip) != 0)
			++ip;
	} while (c==' ' || c=='\t');
	return (c);
}

get()
{
	register c;

	if ((c = *ip) != 0)
		++ip;
	return (c);
}

nxtc(c)
{
	if (c == *ip) {
		++ip;
		return (1);
	}
	return (0);
}

unget(c)
{
	if (c != 0)
		--ip;
}

getmap(d)
{
	register c, v, n;

	if ((c = get()) == '\0')
		qerr("unexpected end of line");
	if (c == d)
		return (-1);
	if (c == '\\') {
		c = get();
		switch (c) {

		case 'b':
			c = '\b';
			break;

		case 'f':
			c = '\f';
			break;

		case 'n':
			c = '\n';
			break;

		case 'r':
			c = '\r';
			break;

		case 't':
			c = '\t';
			break;

		case 'v':					/* 150 */
			c = 013;	/* vertical tab */	/* 150 */
			break;					/* 150 */
		case 'x':
			for(n = v = 0; n < 2; n++) {	/* hex stuff */
				v = (v << 4) + (c = get());
				if(c >= '0' && c <= '9')
					v -= '0';
				else if(c >= 'a' && c <= 'f')
					v -= ('a' - 10);
				else if(c >= 'A' && c <= 'F')
					v -= ('A' - 10);
				else
					qerr("Invalid hex expression");
			}
			c = v;
			break;		
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			n = 0;
			v = 0;
			while (++n<=3 && c>='0' && c<='7') {
				v = (v<<3) + c - '0';
				c = get();
			}
			unget(c);
			c = v;
			break;
		}
	}
	return (c);
}

more()
{
	register c;

	c = getnb();
	unget(c);
	if (c == '\0' || c == '/' || c == ';')
		return (0);
	return (1);
}
