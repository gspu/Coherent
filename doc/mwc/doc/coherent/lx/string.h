.TH string.h "" "" "Header File"
.PC "Declarations for string library"
.B "#include <string.h>"
.PP
.B string.h
is the header that holds the prototypes of all ANSI
routines that handle strings and buffers.
It declares the following routines:
.LB
\fBfnmatch()\fR	Match a string with a normal expression
\fBindex()\fR	Search string for a character; use \fBstrchr()\fR instead
\fBmemccpy()\fR	Copy a region of memory up to a set character
\fBmemchr()\fR	Search a region of memory for a character
\fBmemcmp()\fR	Compare two regions of memory
\fBmemcpy()\fR	Copy one region of memory into another
\fBmemmove()\fR	Copy one region of memory into another with which it overlaps
\fBmemset()\fR	Fill a region of memory with a character
\fBpnmatch()\fR	Match string pattern
\fBstrcat()\fR	Concatenate two strings
\fBstrcmp()\fR	Compare two strings
\fBstrncat()\fR	Append one string onto another
\fBstrncmp()\fR	Compare two lengths for a set number of bytes
\fBstrcpy()\fR	Copy a string
\fBstrncpy()\fR	Copy a portion of a string
\fBstrcoll()\fR	Compare two strings, using locale information
\fBstrcspn()\fR	Return length one string excludes characters in another
\fBstrdup()\fR	Duplicate a string
\fBstrerror()\fR	Translate an error number into a string
\fBstrlen()\fR	Measure a string
\fBstrpbrk()\fR	Find first occurrence in string of character from another string
\fBstrchr()\fR	Find leftmost occurrence of character in a string
\fBstrrchr()\fR	Find rightmost occurrence of character in a string
\fBstrspn()\fR	Return length one string includes character in another
\fBstrstr()\fR	Find one string within another string
\fBstrtok()\fR	Break a string into tokens
\fBstrxfrm()\fR	Transform a string, using locale information
.SH Example
This example reads from
.B stdin
up to
.B NNAMES
names, each of which is no more than
.B MAXLEN
characters long.
It then removes duplicate names, sorts the names,
and writes the sorted list to the standard output.
It demonstrates the functions
.BR shellsort() ,
.BR strcat() ,
.BR strcmp() ,
.BR strcpy() ,
and
.BR strlen() .
.DM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
.DE
.DM
#define NNAMES 512
#define MAXLEN 60
.DE
.DM
char *array[NNAMES];
char first[MAXLEN], mid[MAXLEN], last[MAXLEN];
char *space = " ";
.DE
.DM
int compare();
.DE
.DM
main()
{
	register int index, count, inflag;
	register char *name;
.DE
.DM
	count = 0;
	while (scanf("%s %s %s\en", first, mid, last) == 3) {
		strcat(first, space);
		strcat(mid, space);
		name = strcat(first, (strcat(mid, last)));
		inflag = 0;
.DE
.DM
		for (index=0; index < count; index++) 
			if (strcmp(array[index], name) == 0) 
				inflag = 1;
.DE
.DM
		if (inflag == 0) {
			if ((array[count] =
				malloc(strlen(name) + 1)) == NULL) {
				fprintf(stderr, "Insufficient memory\en");
				exit(EXIT_FAILURE);
			}
			strcpy(array[count], name);
			count++;
		}
	}
.DE
.DM
	shellsort(array, count, sizeof(char *), compare);
	for (index=0; index < count; index++)
		printf("%s\en", array[index]);
	exit(EXIT_SUCCESS);
}
.DE
.DM
compare(s1, s2)
register char **s1, **s2;
{
	return(strcmp(*s1, *s2));
}
.DE
.SH "See Also"
.Xr "header files," header_fi
.Xr "libc," libc
.Xr "strcasecmp()," strcasecm
.Xr "strcasencmp()" strcasenc
.br
\*(AS, \(sc7.1.1
.SH Notes
.II strings.h
Some implementations of \*(UN call this header file
.BR strings.h .
If you are porting code to \*(CO, you may have to modify the
.B #include
directives that invoke this header file.
.PP
The ANSI standard allows adjacent string literals, e.g.:
.DM
	"hello" "world"
.DE
.PP
Adjacent string literals are automatically concatenated.
Thus, the compiler will automatically concatenate the above example into:
.DM
	"helloworld"
.DE
.PP
Because this departs from the Kernighan and Ritchie description
of C, it will generate a warning message if you use the compiler's
\fB\-VSBOOK\fR option.
