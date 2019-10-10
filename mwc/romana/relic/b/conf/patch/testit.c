/*
 * Test program for "patch" to work on.
 */
#include <stdio.h>
#define MY_DEFAULT 17

char my_char = MY_DEFAULT;
short my_short = MY_DEFAULT;
int my_int = MY_DEFAULT;
long my_long = MY_DEFAULT;

main()
{
	printf("&my_char:  %x\n", &my_char);
	printf("&my_short: %x\n", &my_short);
	printf("&my_int:   %x\n", &my_int);
	printf("&my_long:  %x\n", &my_long);

	if (MY_DEFAULT != my_char) {
		printf("my_char patched to:  %x\n", (int) my_char);
	} else {
		printf("my_char is defaulted.\n");
	}
	if (MY_DEFAULT != my_short) {
		printf("my_short patched to:  %x\n", (int) my_short);
	} else {
		printf("my_short is defaulted.\n");
	}
	if (MY_DEFAULT != my_int) {
		printf("my_int patched to:  %x\n", my_int);
	} else {
		printf("my_int is defaulted.\n");
	}
	if (MY_DEFAULT != my_long) {
		printf("my_long patched to:  %lx\n", my_long);
	} else {
		printf("my_long is defaulted.\n");
	}

	printf("Unpatched nix() will never return.\n");
	printf("Calling nix():\n");
	nix();
	printf("Returning from nix().\n");

} /* main() */

