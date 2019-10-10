.TH ctype.h "" "" "Header File"
.PC "Header file for data tests"
.B "#include <ctype.h>"
.PP
.B ctype.h
declares and defines the following routines, which can check and
transform character types:
.sp \n(pDu
.nf
.ta 1.0i
\fB_tolower()\fR	Convert an upper-case character to lower case
\fB_toupper()\fR	Convert a lower-case character to upper case
\fBisalnum()\fR	Test if alphanumeric character
\fBisalpha()\fR	Test if alphabetic character
\fBisascii()\fR	Test if ASCII character
\fBiscntrl()\fR	Test if a control character
\fBisdigit()\fR	Test if a numeric digit
\fBisgraph()\fR	Test if a graphics character
\fBislower()\fR	Test if lower-case character
\fBisprint()\fR	Test if printable character
\fBispunct()\fR	Test if punctuation mark
\fBisspace()\fR	Test if a tab, space, or return
\fBisupper()\fR	Test if upper-case character
\fBisxdigit()\fR	Test if hexadecimal numeral
\fBtoascii()\fR	Convert a character to ASCII
\fBtolower()\fR	Convert an upper-case character to lower case
\fBtoupper()\fR	Convert a lower-case character to upper case
.fi
.SH Example
The following example demonstrates
.BR isalnum() ,
.BR isalpha() ,
.BR isascii() ,
.BR iscntrl() ,
.BR isdigit() ,
.BR islower() ,
.BR isprint() ,
.BR ispunct() ,
and
.BR isspace() .
It prints information about the type of characters it contains.
.DM
#include <ctype.h>
#include <stdio.h>
.DE
.DM
main()
{
	FILE *fp;
	char fname[20];
	int ch;
	int alnum = 0;
	int alpha = 0;
	int allow = 0;
	int control = 0;
	int printable = 0;
	int punctuation = 0;
	int space = 0;
.DE
.DM
	printf("Enter name of text file to examine: ");
	fflush(stdout);
	gets(fname);
.DE
.DM
	if ((fp = fopen(fname, "r")) != NULL) {
		while ((ch = fgetc(fp)) != EOF) {
.DE
.DM
			if (isascii(ch)) {
				if (isalnum(ch)) 
					alnum++;
				if (isalpha(ch)) 
					alpha++;
				if (islower(ch)) 	
					allow++;
				if (iscntrl(ch)) 	
					control++;
				if (isprint(ch)) 
					printable++;
				if (ispunct(ch)) 
					punctuation++;
				if (isspace(ch)) 
						space++;
.DE
.DM
			} else {
				printf("%s is not ASCII.\en", 
						fname);
				exit(1);
			}
		}
.DE
.DM
		printf("%s has the following:\en", fname);
		printf("%d alphanumeric characters\en", alnum);
		printf("%d alphabetic characters\en", alpha);
		printf("%d alphabetic lower-case characters\en", 
			allow);
		printf("%d control characters\en", control);
		printf("%d printable characters\en", printable);
		printf("%d punctuation marks\en", punctuation);
		printf("%d white space characters\en", space);
		exit(0);
.DE
.DM
	} else  {
		printf("Cannot open \e"%s\e".\en", fname);
		exit(2);
	}
}
.DE
.SH "See Also"
.Xr "header files," header_fi
.Xr "libc" libc
.br
\*(AS, \(sc7.3
.SH Notes
The argument for a
.B ctype
function or macro should be an
.B int
that is representable as an
.B "unsigned char"
or EOF \(em i.e., [-1, 0, ..., 255],
as described in the ANSI standard \(sc4.3.
.PP
The functions
.BR _tolower() ,
.BR _toupper() ,
.BR isascii() ,
and
.B toascii()
are not part of the ANSI standard.
Programs that use them may not be portable to all implementations of C.
