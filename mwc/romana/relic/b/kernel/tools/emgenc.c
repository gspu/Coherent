/*
 * /conf/emgenc.c - C support for tool to generate emulator kernel.
 *
 * Usage:  /conf/emgenc <kernel-object-directory>
 *
 * Argument is "kCohxx", with "xx" >= 72.
 */

#define ANSLEN	10

#include <stdio.h>
#include <string.h>
#include <sys/sysi86.h>

/*
 * Get kernel revision level with uname().  Must be r72 or greater.
 *
 * Get ndp type with sysi86(SI86FPHW,).
 * If ndp present, ask if they want to continue.
 * If emulator present, exit with error status.
 */
main(argc, argv)
int argc;
char * argv[];
{
	int result;
	int rev = 0;
	int data;
	char ans[ANSLEN];

	if (argc < 2 || strncmp(argv[1], "kCoh", 4)) {
		fprintf(stderr,
"Missing kernel object directory /conf/kCoh72 or later.\n");
		exit(1);
	}
	fprintf(stderr, "Using kernel object directory %s.\n", argv[1]);
	rev = atoi(argv[1] + 4);
	if (rev < 72) {
		fprintf(stderr, "Can only attach emulator to r72 or later.\n");
		exit(1);
	}

	result = sysi86(SI86FPHW, &data);
	if (result >= 0) {
		switch (data) {
			case FP_NO:
				break;
			case FP_SW:
				break;
			case FP_287:
			case FP_387:
				fprintf(stderr,
"Coprocessor detected.  You do not need FP emulation.\n\n"
"You may link the emulator module to the COHERENT kernel, but\n"
"the FP coprocessor, rather than the emulator, will be used.\n"
"To force use of the emulator on systems with a coprocessor, do\n"
"    /conf/patch -v <kernel-name> \"ndpType=1:s\"\n\n");
				fprintf(stderr,
"Continue [y]? ");
				result = fgets(ans, ANSLEN, stdin);
				if (result
				  && tolower(ans[0]) != 'y'
				  && tolower(ans[0]) != '\n')
					exit(1);
				break;
			default:
				fprintf(stderr, "unexpected result from SI86FPHW\n");
				exit(1);
		}
	} else {
		perror(argv[0]);
		exit(1);
	}
	exit(0);
}
